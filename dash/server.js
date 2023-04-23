// server.js

const express = require('express');
const app = express();
const port = 3000; // porta do servidor

const fs = require('fs').promises;

app.use(express.static('public'));

app.use(express.static('public')); // define a pasta 'public' como estática

// cria um Object vazio

var data = {};

// une os dados do aquivo JSON ao Object

var filesNames = [];

path = './output'

async function readJsonFiles(files) {
  console.log(path + "/" + files[0]);
  const promises = files.map(file => fs.readFile(path + "/" + file, 'utf8'));
  const data = await Promise.all(promises);
  const jsons = data.map(JSON.parse);
  return Object.assign({}, ...jsons);
}


async function readFolder(folder) {
  const files = await fs.readdir(folder);
  // faça algo com o array de nomes de arquivos
  
  // Como obter os files que não estão contidos no array filesNames
  return files;
}




// Exemplo de uso
readJsonFiles(filesNames)
  .then(json => {
    // faça algo com o objeto json resultante
    console.log(json);
  });

app.get('/data', (req, res) => {
  // rota para retornar os dados do arquivo JSON
  readFolder(path)
  .then((files) => {
    files = files.filter(file => !filesNames.includes(file));
    filesNames = filesNames.concat(files);
    console.log(files);
    readJsonFiles(files)
  .then((json) => {
    // concatenar o json com a variavel data
    data = Object.assign({}, data, json);
    console.table(data);
    console.group(data);
    }).then(res.json(data));
  });
});

app.listen(port, () => {
  console.log(`Servidor rodando em http://localhost:${port}`);
});