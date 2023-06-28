python3 create_mariadb.py
cqlsh cassandra 9042 -f "create_db.cql"
cd mock
celery -A tasks worker --loglevel=info -c 12