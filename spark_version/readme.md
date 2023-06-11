## Para fazer banco funcionar:
Abrir terminal novo e digitar os seguintes comandos em ordem, separadamente:
```
cqlsh cassandra 9042
SOURCE 'work/create_db.cql'
```

Banco Cassandra estará funcionando.

## Fazer Celery funcionar:
Em outro terminal do Jupyter Lab, navegamos até a pasta */work/mock*, e então rodamos
```
celery -A tasks worker --loglevel=info -c N
```
Sendo o N o número de consumidores que queremos ter trabalhando paralelamente.

## Rodar a simulação
Abrindo um novo terminal no Jupyter Lab, digitamos o seguinte:
```
python3 mock/simulador.py nome_da_rodovia
```
Devemos substituir *nome_da_rodovia* por um nome de rodovia previsto no arquivo `parametros.json`: BR-040, BR-116...

## Acessar o MariaDB
Entrando em um Prompt de Comando fora do Jupyter Lab, rodamos os seguinte código:
```
docker exec -it spark_version-mariadb-1 mariadb --user root -p
```
Com este comando, nos conectamos com o container do MariaDB. A senha que deve ser fornecida é *secret*.

