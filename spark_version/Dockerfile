# use the jupyter/pyspark-notebook image as the base
FROM jupyter/pyspark-notebook

# install the required packages using pip
RUN pip install celery redis cassandra-driver mysql-connector-python cqlsh pyamqp pygame

# expose the port 8888 for jupyter lab
EXPOSE 8888

# set the environment variable for jupyter lab
ENV JUPYTER_ENABLE_LAB=yes