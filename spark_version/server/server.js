// server.js

const express = require('express');
const { get } = require('http');
const app = express();
const port = 3000; // porta do servidor
app.use(express.static('public'));

const mariadb = require('mariadb');
const pool = mariadb.createPool({
  host: 'localhost',
  user: 'root',
  password: '8pJcpe/h0h_%',
  port: 3306
});

async function getData() {
  let conn;
  try {
    conn = await pool.getConnection();
    const rows = await conn.query("SELECT * ");
    console.log(rows); // [ {val: 1}, meta: ... ]
  } catch (err) {
    throw err;
  } finally {
    if (conn) return conn.end();
  }
}


app.get('/data', (req, res) => {
  getData().then((data) => {
    res.send(data);
  })
});

app.listen(port, () => {
  console.log(`Servidor rodando em http://localhost:${port}`);
});