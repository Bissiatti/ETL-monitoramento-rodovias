import mysql.connector as database
import os

connection = database.connect(
    host="host.docker.internal",
    port=3306,
    user="root",
    password="secret"
)


cursor = connection.cursor()



cursor.execute(query)