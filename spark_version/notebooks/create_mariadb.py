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
<<<<<<< HEAD

connection.commit()
print("Schema and Tables succesfully created")

=======
print("Schema and Tables succesfully created")

>>>>>>> 38e256f6bfaa36537a93dab2cd079b63be138acb
cursor.close()
connection.close()
