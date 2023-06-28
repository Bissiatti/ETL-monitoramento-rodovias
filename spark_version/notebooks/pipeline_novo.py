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

def atualiza_media(media_atual, tamanho_atual, media_add, tamanho_add):
    if media_add == None:
        return media_atual
    if tamanho_atual == 0:
        return media_add
    tamanho_total = tamanho_atual + tamanho_add
    return (media_atual/tamanho_total)*tamanho_atual + (media_add/tamanho_total)*tamanho_add

def processa_velocidade_media(batch):
    global vel_media, n_vel_media
    batch = batch.filter(F.col("vel_y").isNotNull())
    
    # group by "rodovia" and aggregate the mean of "velocidade"
    mean_df = batch.groupBy("rodovia").agg(F.mean(F.abs("vel_y").alias('vel_y')).alias('vel_y'))
    # collect the rows as a list
    mean_rows = mean_df.collect()
    # create a dictionary with "rodovia" as key and mean as value
    mean_dict = {row.asDict()["rodovia"]: row.asDict()["vel_y"] for row in mean_rows}
    # group by "rodovia" and aggregate the mean of "velocidade"
    length_df = batch.groupBy("rodovia").agg(F.count("vel_y"))
    length_df = length_df.withColumnRenamed("count(vel_y)", "vel_y")
    # collect the rows as a list
    length_rows = length_df.collect()
    # create a dictionary with "rodovia" as key and mean as value
    length_dict = {row.asDict()["rodovia"]: row.asDict()["vel_y"] for row in length_rows}
    for key in length_dict.keys():
        n_vel_media[key]+=length_dict[key]
        vel_media[key] = atualiza_media(vel_media[key], n_vel_media[key], mean_dict[key], length_dict[key])

def processa_tempo_cruzamento(batch):
    global n_tempo_medio, tempo_medio
    batch = batch.filter(F.col("tempo_cruzamento").isNotNull())
    # group by "rodovia" and aggregate the mean of "velocidade"
    batch.show()
    mean_df = batch.groupBy("rodovia").agg(F.mean("tempo_cruzamento"))
    mean_df = mean_df.withColumnRenamed("avg(tempo_cruzamento)", "tempo_cruzamento")
    # collect the rows as a list
    mean_rows = mean_df.collect()
    # create a dictionary with "rodovia" as key and mean as value
    mean_dict = {row.asDict()["rodovia"]: row.asDict()["tempo_cruzamento"] for row in mean_rows}

    # group by "rodovia" and aggregate the mean of "velocidade"
    length_df = batch.groupBy("rodovia").agg(F.count("tempo_cruzamento"))
    length_df = length_df.withColumnRenamed("count(tempo_cruzamento)", "tempo_cruzamento")
    # collect the rows as a list
    length_rows = length_df.collect()
    # create a dictionary with "rodovia" as key and mean as value
    length_dict = {row.asDict()["rodovia"]: row.asDict()["tempo_cruzamento"] for row in length_rows}
    for key in length_dict.keys():
        n_tempo_medio[key] += length_dict[key]
        tempo_medio[key] = atualiza_media(tempo_medio[key], n_tempo_medio[key], mean_dict[key], length_dict[key])

total = 0
last = 0
i = 0
p = [[key]+list(params[key].values()) for key in params.keys()]
p = ss.createDataFrame(p, ["Rodovia"]+list(params[list(params.keys())[0]].keys()))

T = 10000

schema = StructType([
  StructField('placa', StringType(), True),
  StructField('total_multas', IntegerType(), True),
  StructField('tempo_da_simulacao', IntegerType(), True),
  StructField('proibidoCircular', IntegerType(), True)
])

df_multas = ss.createDataFrame([], schema)

schema2 = StructType([
  StructField('placa', StringType(), True),
  StructField('total_perigosa', IntegerType(), True),
  StructField('tempo_da_simulacao', IntegerType(), True),
  StructField('perigosa?', IntegerType(), True),
  StructField('tempo_da_simulacao_intervalo',IntegerType(),True)
])

df_perigosa = ss.createDataFrame([], schema2)

Velocidades_Maximas = p.select(F.col('rodovia'), F.col("VelocidadeMaxima"))
Aceleracoes_Maximas = p.select(F.col('rodovia'), 0.8*F.col("AceleracaoMaxima"))
Aceleracoes_Maximas = Aceleracoes_Maximas.withColumnRenamed("(AceleracaoMaxima * 0.8)", "AceleracaoMaxima")

rodovias = p.select(F.collect_list('rodovia')).collect()[0][0]
vel_media = {rodovia:0 for rodovia in rodovias} #"BR-116": 0, "BR-040": 0, "BR-135": 0, "BR-393": 0}
n_vel_media = {rodovia:0 for rodovia in rodovias} # {"BR-116": 0, "BR-040": 0, "BR-135": 0, "BR-393": 0}

tempo_medio = {rodovia:0 for rodovia in rodovias}
n_tempo_medio = {rodovia:0 for rodovia in rodovias}

def atualiza_media(media_atual, tamanho_atual, media_add, tamanho_add):
    if media_add == None:
        return media_atual
    if tamanho_atual == 0:
        return media_add
    tamanho_total = tamanho_atual + tamanho_add
    return (media_atual/tamanho_total)*tamanho_atual + (media_add/tamanho_total)*tamanho_add

def multas(batch):
    global df_multas, T
    # Criar uma janela que particiona por placa e ordene por tempo_da_simulacao
    window = Window.partitionBy("placa").orderBy("tempo_da_simulacao")
    
    # Calcular a soma cumulativa de multas por carro na janela
    batch = batch.withColumn('multado',F.col('multado').cast('int'))
    batch = batch.withColumn("total_multas", F.sum("multado").over(window))
    batch = batch.withColumn('proibidoCircular', F.lit(0))

    window2 = Window.partitionBy("placa").orderBy("tempo_da_simulacao")

    df = batch.filter(F.col('multado')==1).select('placa','total_multas','tempo_da_simulacao','proibidoCircular')

    df = df_multas.union(df)
    # Calcular o número de linha por carro na janela
    df = df.withColumn("num_linha", F.row_number().over(window2))

    df = df.withColumn("primeiro_tempo", F.first("tempo_da_simulacao").over(window))
    df = df.withColumn("ultimo_tempo", F.last("tempo_da_simulacao").over(window))
    df = df.withColumn('proibidoCircular',F.when(((F.col('ultimo_tempo')-F.col('primeiro_tempo')) < T) & (F.col('total_multas')>10),1).otherwise(0))
    
    # Filtrar as linhas que tenham o número de linha menor ou igual a 10
    df_multas = df.filter(df.num_linha <= 10).select('placa','total_multas','tempo_da_simulacao','proibidoCircular')

T_perigosa = 200
N_eventos = 3
I_perigosa = 1000

def perigosas(batch):
    global df_perigosa, T_perigosa, N_eventos,I_perigosa
    batch = batch.withColumn('perigosa_quant',F.col('acima_vel').cast('int') + F.col('acima_acel').cast('int') + F.col('troca_faixa').cast('int'))
    # Criar uma janela que particiona por placa e ordene por tempo_da_simulacao
    window = Window.partitionBy("placa").orderBy("tempo_da_simulacao")
    
    # Calcular a soma cumulativa de multas por carro na janela
    batch = batch.withColumn("total_perigosa", F.sum("perigosa_quant").over(window))
    batch = batch.withColumn('perigosa?', F.lit(0))
    batch = batch.withColumn('tempo_da_simulacao_intervalo',F.lit(I_perigosa))

    window2 = Window.partitionBy("placa").orderBy("tempo_da_simulacao")

    df = batch.filter(F.col('perigosa_quant')>=1).select('placa','total_perigosa','tempo_da_simulacao','perigosa?','tempo_da_simulacao_intervalo')

    df = df_perigosa.union(df)
    # Calcular o número de linha por carro na janela
    df = df.withColumn("num_linha", F.row_number().over(window2))

    df = df.withColumn("primeiro_tempo", F.first("tempo_da_simulacao").over(window))
    df = df.withColumn("ultimo_tempo", F.last("tempo_da_simulacao").over(window))
    df = df.withColumn('perigosa?',F.when(((F.col('ultimo_tempo')-F.col('primeiro_tempo')) < I_perigosa) & (F.col('total_perigosa')>N_eventos),1).otherwise(0))
    df = df.withColumn('tempo_da_simulacao_intervalo',F.col('tempo_da_simulacao_intervalo')+F.col('tempo_da_simulacao'))
    
    # Filtrar as linhas que tenham o número de linha menor ou igual a N
    df_perigosa = df.filter(df.num_linha <= N_eventos).select('placa','total_perigosa','tempo_da_simulacao','perigosa?','tempo_da_simulacao_intervalo')
    
collision_tolerance = 1
collision_tolerance_quad = 0.5

def getVelMedia(df2):
    df2['vel_media'] = df2['rodovia'].map(vel_media)
    return df2

def getTempoMedia(df2):
    df2['tempo_medio'] = df2['rodovia'].map(tempo_medio)
    return df2

a = 1
b = 100
c = 100

start_time = time.time()
df_cruzamento = ss.createDataFrame([], "rodovia: string, placa: string, tempo_inicio: bigint, tempo_final: bigint,tempo_cruzamento: bigint")
while i < 1000:
    start_time2 = time.time()
    st=time.time()
    query = f"SELECT MAX(tempo_da_simulacao) FROM simulacao;"
    max = list(session.execute(query))[0][0]

    b = max
    try:
        a = max - 100
    except:
        a = 0
    
    query = f"SELECT * FROM simulacao WHERE tempo_da_simulacao >= {a} AND tempo_da_simulacao <= {b} ALLOW FILTERING;"
    r = list(session.execute(query))
    
    et=time.time()
    
    if r != []:
        print("GO!")
        i+=1
        
        st=time.time()
        
        df = ss.createDataFrame(r)
        
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

        processa_velocidade_media(df)
        
        df = df.join(Velocidades_Maximas,on='rodovia',how='left')
        df = df.join(Aceleracoes_Maximas,on='rodovia',how='left')
        
        df = df.withColumn('acima_vel',F.abs(col('vel_y'))>F.abs(col('VelocidadeMaxima')))
        df = df.withColumn('acima_acel',F.abs(col('acel_y'))>F.abs(col('AceleracaoMaxima')))
        
        df = df.withColumn("troca_faixa", col("pos_x") != lag("pos_x", 1).over(windowSpec))
        
        # contador de trocas
        
        df = df.withColumn('multado',((F.col('acima_vel') == 1) & (lag('acima_vel').over(windowSpec) == 0)))
        df = df.withColumn("on_road", (((col("pos_y") > 0) & (col('pos_y') < 800))))
        
        df = df.withColumn('tempo_inicio',when(((F.col('on_road') == True) & (lag('on_road').over(windowSpec) == False)), F.col("tempo_da_simulacao")).otherwise(None))
        df = df.withColumn('tempo_final',when(((F.col('on_road') == True) & (lead('on_road').over(windowSpec) == False)), F.col("tempo_da_simulacao")).otherwise(None))
        df = df.withColumn('tempo_cruzamento',F.lit(None))
        df_cruzamento = df_cruzamento.union(df.select('rodovia', 'placa', 'tempo_inicio','tempo_final','tempo_cruzamento').filter((F.col('vel_y') != 0) & (F.col('tempo_inicio').isNotNull() | F.col('tempo_final').isNotNull())))
        windowSpec2 = Window.partitionBy('placa','rodovia').orderBy('tempo_final')
        
        df_cruzamento = df_cruzamento.withColumn('tempo_cruzamento', col('tempo_final') - lag('tempo_inicio').over(windowSpec2))
        
        processa_tempo_cruzamento(df_cruzamento)
        df_cruzamento = df_cruzamento.filter(df_cruzamento.tempo_cruzamento.isNull())
        df = df.withColumn('time_on_road',F.lit(0))
        
        windowSpec = Window.partitionBy("placa",'rodovia').orderBy('tempo_da_simulacao')
        
        df = df.withColumn("prev_pos_y", lag("pos_y", 1).over(windowSpec))
        windowSpec = Window.partitionBy('rodovia',"placa").orderBy('tempo_da_simulacao')
        
        df = df.withColumn("prev_pos_y", lag("pos_y", 1).over(windowSpec))

        
        multas(df)

        perigosas(df)

        window3 = Window.partitionBy("placa").orderBy("tempo_da_simulacao")

        df = df.join(df_multas,on=['placa','tempo_da_simulacao'],how='left')


        df = df.join(df_perigosa,on=['placa','tempo_da_simulacao'])

        # Calcular o máximo da coluna proibidoCircular na janela
        df = df.withColumn("proibidoCircular", F.max("proibidoCircular").over(window3))

        df = df.withColumn("perigosa_I", when(((lag("perigosa?", 1).over(window3) == 1) & (F.col('tempo_da_simulacao') < F.col('tempo_da_simulacao_intervalo'))), 1).otherwise(0))
        
        # df = df.join(df_multas,on=['placa','tempo_da_simulacao'],how='left')
        
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

        
        df2 = df2.join(unique_placas,on='rodovia',how='left')
        df2 = df2.join(unique_colisao,on='rodovia',how='left')

        df2 = df2.fillna(0)
        
        
        end_time = time.time()
        elapsed_time = end_time - start_time2
        elapsed_time
        
        df2 = df2.withColumn('tempo_processamento', F.lit(elapsed_time))
        df2 = df2.withColumn('tempo_processamento', F.lit(elapsed_time))
        df2 = df2.withColumn('tempo_processamento', F.lit(elapsed_time))

        
        datap2 = df2.toPandas()

        datap2 = getVelMedia(datap2)

        datap2 = getTempoMedia(datap2)
        
        query2 = f""" REPLACE INTO rodovias (nome_rodovia,horario_registro,total_veiculos,veiculos_colisao,tempo_processamento,velocidade_media,tempo_medio_cruzamento)
        
                VALUES {','.join([str(i) for i in list(datap2.to_records(index=False))])};
                        
                """.replace("None", "NULL").replace("\n", "").replace("nan", "NULL")        
            
        data = df.select('placa','pos_x','pos_y','acel_y','vel_y', 'rodovia', 'tempo_da_simulacao',F.col('proibidoCircular'),'Risco_Colisão',F.col('troca_faixa'))#,'acima_vel')
        datap = data.toPandas()


        query = f""" REPLACE INTO carros (placa, pos_x, pos_y, aceleracao, velocidade, rodovia, horario_registro, multas, risco_colisao, direcao_perigosa)
    
                    VALUES {','.join([str(i) for i in list(datap.to_records(index=False))])};
                    
                    """.replace("None", "NULL").replace("\n", "").replace("nan", "NULL")
    
        cursor.execute(query)
        connection.commit()
        cursor.execute(query2)
        connection.commit()

    
        end_time = time.time()
        elapsed_time = end_time - start_time2
        # total = total + elapsed_time
    
        print('Done!')

    b+=c
    a+=c
