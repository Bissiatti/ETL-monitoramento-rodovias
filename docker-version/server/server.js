// server.js

const express = require('express');
const { get } = require('http');
const app = express();
const port = "0.0.0.0:8000"; // porta do servidor
//const port = 3000; // porta do servidor

app.use(express.static('public'));

const mysql = require('mysql');

const pool = mysql.createPool({
    host: "host.docker.internal",
    //host: "localhost",
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

// getlen();

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

function getTop() {
  return new Promise((resolve, reject) => {
    pool.getConnection()
      .then(conn => {
        const str_query = "SELECT placa, COUNT(DISTINCT rodovia) AS num_rodovias FROM carros GROUP BY placa ORDER BY num_rodovias DESC LIMIT 100;"
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


function getRodovia() {
  return new Promise((resolve, reject) => {
    pool.getConnection()
      .then(conn => {
        const str_query = "SELECT * FROM rodovias WHERE horario_registro = (SELECT MAX(horario_registro) FROM carros);"
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

app.get('/rodovia', (req, res) => {
  getRodovia().then((data) => {
    res.send(JSON.stringify(data, (key, value) =>
      typeof value === 'bigint' ? value.toString() : value
    ));
  });
});

app.get('/top', (req, res) => {
  getTop().then((data) => {
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
