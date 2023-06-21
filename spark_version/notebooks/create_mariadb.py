import mysql.connector as database
import os

print("Establishing connection to the Database")

connection = database.connect(
    host="host.docker.internal",
    port=3306,
    user="root",
    password="secret"
)


cursor = connection.cursor()

file = open('create_final_db.sql', 'r')
query = file.read()

cursor.execute(query)
print("Schema and Tables succesfully created")

cursor.close()
connection.close()
