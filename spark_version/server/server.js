// server.js

const express = require('express');
const { get } = require('http');
const app = express();
const port = 3000; // porta do servidor
app.use(express.static('public'));

const mariadb = require('mariadb');

const pool = mariadb.createPool({
    // host: "host.docker.internal",
    host: "localhost",
    port: 3306,
    user: 'root',
    password: 'secret',
    database: "dashboard",
    acquireTimeout: 20000 // 20 seconds
});

// let conn = await pool.getConnection();
var i = 1

async function getlen() {
    let conn;
    try {
        conn = await pool.getConnection();
        const rows = await conn.query("SELECT COUNT(*) FROM carros;");
        console.log(rows);
    } catch (err) {
        throw err;
    } finally {
        if (conn) return conn.end();
    }
}

getlen();

function getData() {
  return new Promise((resolve, reject) => {
    pool.getConnection()
      .then(conn => {
        const str_query = "SELECT * FROM carros WHERE horario_registro = (SELECT MAX(horario_registro) FROM carros);"
        //const str_query = "SELECT * FROM carros WHERE horario_registro = "+i+");"
        //i = i + 1
        conn.query(str_query)
          .then(rows => {
            resolve(rows);
            conn.end();
          })
          .catch(err => {
            reject(err);
            conn.end();
          });
      })
      .catch(err => {
        reject(err);
      });
  });
}

app.get('/data', (req, res) => {
  getData().then((data) => {
    res.send(JSON.stringify(data, (key, value) =>
      typeof value === 'bigint' ? value.toString() : value
    ));
  });
});


app.get('/test', (req, res) => {
  res.send('Hello World!');
});

app.listen(port, () => {
  console.log(`Servidor rodando em http://localhost:${port}`);
});