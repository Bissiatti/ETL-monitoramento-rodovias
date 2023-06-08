## Para fazer banco funcionar:
Abrir terminal novo e digitar os seguintes comandos em ordem, separadamente:
```
cqlsh cassandra 9042
SOURCE '../create_db.cql'
```

Banco Cassandra estará funcionando.

## Fazer Celery funcionar:
Em outro terminal do Jupyter Lab, navegamos até a pasta */work/mock*, e então rodamos
```
celery -A tasks worker --loglevel=info -c N
```
Sendo o N o número de consumidores que queremos ter trabalhando paralelamente.