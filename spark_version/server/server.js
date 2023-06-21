// server.js

const express = require('express');
const { get } = require('http');
const app = express();
const port = 3000; // porta do servidor
app.use(express.static('public'));

const mariadb = require('mariadb');

const pool = mariadb.createPool({
    host: "host.docker.internal",
    //host: "localhost",
    port: 3306,
    user: 'root',
    password: 'secret',
    database: "dashboard",
    acquireTimeout: 20000 // 20 seconds
});

// let conn = await pool.getConnection();

async function getData() {
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


// async function getData() {
//   let conn;
//   try {
//     conn = await pool.getConnection();
//     const use = await conn.query("USE dashboard");
//     console.log(use);
//     const rows = await conn.query("SELECT * from dashboard.carros limit 1");
//     console.log(rows); // [ {val: 1}, meta: ... ]
//   } catch (err) {
//     throw err;
//   } finally {
//     if (conn) return conn.end();
//   }
//  }

getData().then((data) => {
  console.log(data)
});

app.get('/data', (req, res) => {
  getData().then((data) => {
    console.log(data);
    res.send(data);
  });
  // getData().then((data) => {
  //   res.send(data);
  // })
});

app.get('/test', (req, res) => {
  res.send('Hello World!');
});

app.listen(port, () => {
  console.log(`Servidor rodando em http://localhost:${port}`);
});


