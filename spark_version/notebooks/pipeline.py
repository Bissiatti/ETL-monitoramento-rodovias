import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from pyspark.sql.types import * 
from pyspark.sql import SparkSession, DataFrame as SparkDataFrame
import pyspark.sql.functions as F
from pyspark.sql.functions import col,isnan, when, count, coalesce
from pyspark import SparkContext
from pyspark.sql import SQLContext
from pyspark.sql.window import Window
from pyspark.sql.functions import col, lag, lead
import json
from functools import reduce
import sys
from cassandra.cluster import Cluster
import os
import time
import warnings
import json
from pyspark.sql import Window
from pyspark.sql.functions import lag, col
import mysql.connector as database
import time
import sqlalchemy

# from mock.tasks import adiciona_carro}
cluster = Cluster(['cassandra'])
session = cluster.connect()

ss = SparkSession.builder \
           .appName('SparkByExamples') \
           .config("spark.jars", "/usr/share/java/mariadb-java-client.jar") \
           .getOrCreate()
sql = SQLContext(ss)

warnings.simplefilter(action='ignore', category=FutureWarning)

session.execute("USE simulacao")

params = json.load(open('./mock/parametros.json'))

connection = database.connect(
    host="host.docker.internal",
    port=3306,
    user="root",
    password="secret"
)

cursor = connection.cursor(buffered=True)
cursor.execute("USE dashboard;")

start_time = time.time()
total = 0
last = 0
i = 0
while i < 100:
    start_time2 = time.time()
    st=time.time()
    query = f"SELECT MAX(tempo_da_simulacao) FROM simulacao;"
    max = list(session.execute(query))[0][0]

    b = max
    try:
        a = max - 20
    except:
        a = 0
    
    query = f"SELECT * FROM simulacao WHERE tempo_da_simulacao >= {a} AND tempo_da_simulacao <= {b} ALLOW FILTERING;"
    r = list(session.execute(query))
    
    et=time.time()
    
    if r != []:
        print(f"Tempo de extração do banco: {et-st}")
        i+=1
        
        st=time.time()
        
        df = ss.createDataFrame(r)
        
        p = [[key]+list(params[key].values()) for key in params.keys()]
        p = ss.createDataFrame(p, ["Rodovia"]+list(params[list(params.keys())[0]].keys()))
        
        Velocidades_Maximas = p.select(F.col('rodovia'), F.col("VelocidadeMaxima"))
        Aceleracoes_Maximas = p.select(F.col('rodovia'), 0.8*F.col("AceleracaoMaxima"))
        Aceleracoes_Maximas = Aceleracoes_Maximas.withColumnRenamed("(AceleracaoMaxima * 0.8)", "AceleracaoMaxima")
        
        collision_tolerance = 1
        collision_tolerance_quad = 0.5
        windowSpec = Window.partitionBy("placa").orderBy("tempo_da_simulacao")
        
        df = df.withColumn("prev_pos_y", lag("pos_y", 1).over(windowSpec))
        df = df.withColumn("prev_tempo_da_simulacao", lag("tempo_da_simulacao", 1).over(windowSpec))
        df = df.withColumn("vel_y", (col("pos_y") - col("prev_pos_y")) / (col("tempo_da_simulacao") - col("prev_tempo_da_simulacao")))
        df = df.withColumn("prev_vel_y", lag("vel_y", 1).over(windowSpec))
        df = df.withColumn("acel_y", (col("vel_y") - col("prev_vel_y")) / (col("tempo_da_simulacao") - col("prev_tempo_da_simulacao")))
        
        df = df.withColumn("posicao_prevista", col("pos_y") + col("vel_y") * (collision_tolerance) + col("acel_y") * collision_tolerance_quad)
        
        window_spec_rf = Window.partitionBy("rodovia", "pos_x").orderBy('pos_y')
        lag_column = col("posicao_prevista") - lag(col("posicao_prevista")).over(window_spec_rf)
        lead_column = lead(col("posicao_prevista")).over(window_spec_rf) - col("posicao_prevista")
        
        # Add the lag column to the DataFrame
        df = df.withColumn("Risco_Colisão", when(((lag_column < 0) & (col("rodovia") == lag(col("rodovia")).over(window_spec_rf)) & (col("pos_x") == lag(col("pos_x")).over(window_spec_rf)))| ((lead_column < 0) & (col("rodovia") == lead(col("rodovia")).over(window_spec_rf)) & (col("pos_x") == lead(col("pos_x")).over(window_spec_rf))), 1).otherwise(0))
        
        df = df.join(Velocidades_Maximas,on='rodovia',how='left')
        df = df.join(Aceleracoes_Maximas,on='rodovia',how='left')
        
        df = df.withColumn('acima_vel',F.abs(col('vel_y'))>F.abs(col('VelocidadeMaxima')))
        df = df.withColumn('acima_acel',F.abs(col('acel_y'))>F.abs(col('AceleracaoMaxima')))
        
        df = df.withColumn("troca_faixa", col("pos_x") != lag("pos_x", 1).over(windowSpec))
        
        # contador de trocas
        
        df = df.withColumn('multado',((F.col('acima_vel') == 1) & (lag('acima_vel').over(windowSpec) == 0)))
        
        windowSpec = Window.partitionBy("placa",'rodovia').orderBy('tempo_da_simulacao')
        
        df = df.withColumn("prev_pos_y", lag("pos_y", 1).over(windowSpec))
        df = df.withColumn("on_road", (((col("pos_y") > 0) & (col('pos_y') < 800)) & (col("prev_pos_y") > 0)).cast("int"))
        df = df.withColumn('time_on_road',F.lit(0))

        ## CALCULO DA RODOVIA ##

        # Obter o valor máximo da coluna tempo_da_simulacao
        df2 = df.select('rodovia', 'tempo_da_simulacao')
        max_tempo = df2.select(F.max("tempo_da_simulacao")).collect()[0][0]
        
        # Filtrar o dataframe pelo valor máximo
        df2 = df2.filter(df2.tempo_da_simulacao == max_tempo)
        
        # Aplicar um unique no dataframe df2
        df2 = df2.distinct()
        
        max_tempo = df2.select(F.max("tempo_da_simulacao")).collect()[0][0]
        df2 = df2.filter(df2.tempo_da_simulacao == max_tempo)
        
        # Criar uma janela de tamanho 1 sobre a coluna rodovia
        w = Window.partitionBy(F.col("rodovia")).orderBy(F.col("rodovia")).rangeBetween(0, 0)
        
        # Contar as placas distintas por rodovia
        unique_placas = df.filter(df.tempo_da_simulacao == max_tempo)\
                          .withColumn("placa_distinct", F.size(F.collect_set("placa").over(w)))\
                          .select("rodovia", "placa_distinct")\
                          .distinct()
        
        # Contar as placas distintas por rodovia que tiveram colisão
        unique_colisao = df.filter((df.tempo_da_simulacao == max_tempo) & (df.vel_y == 0))\
                          .withColumn("placa_distinct_colissao", F.size(F.collect_set("placa").over(w)))\
                          .select("rodovia", "placa_distinct_colissao")\
                          .distinct()
        
        df2 = df2.join(unique_placas,on='rodovia',how='inner')
        df2 = df2.join(unique_colisao,on='rodovia',how='inner')
        
        
        end_time = time.time()
        elapsed_time = end_time - start_time2
        elapsed_time
        
        df2 = df2.withColumn('tempo_processamento', F.lit(elapsed_time))
        df2 = df2.withColumn('tempo_processamento', F.lit(elapsed_time))
        df2 = df2.withColumn('tempo_processamento', F.lit(elapsed_time))
        
        
        datap2 = df2.toPandas()
        
        query2 = f""" INSERT IGNORE INTO carros (nome_rodovia,horario_registro,total_veiculos,veiculos_colisao,tempo_processamento)
        
                VALUES {','.join([str(i) for i in list(datap2.to_records(index=False))])};
                        
                """.replace("None", "NULL").replace("\n", "").replace("nan", "NULL")
        
            
        data = df.select('placa','pos_x','pos_y','acel_y','vel_y', 'rodovia', 'tempo_da_simulacao',F.col('multado').cast('int'),'Risco_Colisão',F.col('troca_faixa'))#,'acima_vel')
        datap = data.toPandas()


        query = f""" INSERT IGNORE INTO carros (placa, pos_x, pos_y, aceleracao, velocidade, rodovia, horario_registro, multas, risco_colisao, direcao_perigosa)
    
                    VALUES {','.join([str(i) for i in list(datap.to_records(index=False))])};
                    
                    """.replace("None", "NULL").replace("\n", "").replace("nan", "NULL")
    
        cursor.execute(query)
        connection.commit()

    
        end_time = time.time()
        elapsed_time = end_time - start_time2
        # total = total + elapsed_time
    
        cursor.execute("SELECT COUNT(*) FROM carros;")

