from celery import Celery
from cassandra.cluster import Cluster

app = Celery('tasks', broker='pyamqp://guest@host.docker.internal:5672//')

@app.task
def adiciona_carro(placa, rodovia, pos_x, pos_y, tempo_da_simulacao):
    cluster = Cluster(['cassandra'])
    session = cluster.connect()
    session.execute("USE simulacao;")
    query = f"""
    INSERT INTO positions (placa, rodovia, tempo_da_simulacao, pos_x, pos_y)
    
    VALUES ('{placa}', '{rodovia}', '{tempo_da_simulacao}', {pos_x}, {pos_y});    
    """
    session.execute(query)

    session.shutdown()
    cluster.shutdown()

