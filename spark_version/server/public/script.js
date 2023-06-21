// dataFrames = {};

// var keysBrs = {};

// var brs = [];

// let tempoExecucao = {};

// var lastFrame;

// var api;

// let realTime = document.querySelector('#realTime')

// function realTimeUpdate(){
//   if(realTime.checked) {
//       // search on json
//     let value = lastFrame;
//     console.log(value)
//     if (value == '') {
//       return;
//     }
//     while (table.rows.length > 1) {
//       table.deleteRow(table.rows.length - 1);
//     }
//     table.parentElement.setAttribute('style', 'display: flex;')

//     var frame;
//     console.log(dataFrames)
//     for(let br in dataFrames) {
//       console.log(keysBrs[br][value])
//       if (dataFrames[br][keysBrs[br][value]] == undefined) {
//         continue;
//       }
//       const valueFrame = document.getElementById('value-box')
//       valueFrame.innerHTML = keysBrs[br][value];
//       addRow(dataFrames[br][keysBrs[br][value]],br);
//       updateNumberRoad();
//       frame = keysBrs[br][value]
//   }
//     // value = Object.keys(keysBrs['BR-039']).length - 1;
//     // if (value == '') {
//     //   return;
//     // }
//     // value = parseInt(keysBrs['BR-039'][value]);
//     // console.log(value)
//     // console.log(keysBrs)
//     // console.log(keysBrs['BR-039'])
//     // console.log(keysBrs['BR-039'][value])
//     // var frame;
//     // console.log(dataFrames)
//     // for(let br in dataFrames) {
//     //   if (dataFrames[br][keysBrs[br][value]] == undefined) {
//     //     continue;
//     //   }
//     //   const valueFrame = document.getElementById('value-box')
//     //   valueFrame.innerHTML = "<p>"+value+"<p/>";
//     //   addRow(dataFrames[br][keysBrs[br][value]],br);
//     //   updateNumberRoad();
//     //   frame = keysBrs[br][value]
//     // }
//     // // get time of execution when frame between to frames of timeExecucao
//     // const timeFrame = document.getElementById('TempoAnalise')
//     // console.log(tempoExecucao)
//     // timeFrame.lastElementChild.innerHTML = tempoExecucao[Object.keys(tempoExecucao)[0]];
//     // for(let f in Object.keys(tempoExecucao)){
//     //   if (f > 0 && Object.keys(tempoExecucao)[f] > frame) {
//     //     timeFrame.lastElementChild.innerHTML = tempoExecucao[Object.keys(tempoExecucao)[f-1]];
//     //     break;
//     //   } 

//     // }
//   }
// }

// realTime.addEventListener('click', (e) => {
//   if (e.target.checked) {
//     fetch('/realTime:timestart').then((response) => {
//       console.log(response)
//     }
//     )}
//   })

// realTime.addEventListener('click', (e) => {
//   realTimeUpdate();
// })
// var table = document.getElementById("tableData");
// function addRow(frameObj,road) {
//   console.log(road)
//    // Obtém a tabela pelo id
//   table.parentElement.setAttribute('style', 'display: flex;')
//   let keys = Object.keys(frameObj);
//   let numberCarsUpLimit = 0;
//   for(let i = 0; i < keys.length; i++) {
//     var newRow = table.insertRow(); // Insere uma nova linha no final da tabela

//     // Adiciona células à nova linha
//     var cell1 = newRow.insertCell(0);
//     var cell2 = newRow.insertCell(1);
//     var cell3 = newRow.insertCell(2);
//     var cell4 = newRow.insertCell(3);
//     var cell5 = newRow.insertCell(4);
//     var cell6 = newRow.insertCell(5);
//     var cell7 = newRow.insertCell(6);
//     var cell8 = newRow.insertCell(7);
//     var cell9 = newRow.insertCell(8);
    
//     // Define o conteúdo das células
//     cell1.innerHTML = road;
//     cell2.innerHTML = keys[i];
//     cell3.innerHTML = frameObj[keys[i]]['Velocidade'];
//     cell4.innerHTML = frameObj[keys[i]]['Posicao'];
//     cell5.innerHTML = frameObj[keys[i]]['Batido'];
//     if (api[keys[i]] != undefined) {
//       cell7.innerHTML = api[keys[i]]['nome'];
//       cell8.innerHTML = api[keys[i]]['modelo'];
//       cell9.innerHTML = api[keys[i]]['ano'];
//     }

//     if(frameObj[keys[i]]['Acima da Velocidade']){
//       numberCarsUpLimit += 1;
//     }
//     if(frameObj[keys[i]]['Risco Colisão'].length != 0){
//       cell6.style.backgroundColor = 'red';  
//       // convert array to string
//       let risco = frameObj[keys[i]]['Risco Colisão'].join(', ');
//       cell6.innerHTML = risco;

//     }else{
//       cell6.innerHTML = 'false';
//     }
//     const timeFrame = document.getElementById('TempoAnalise')
//     let timeProcess = frameObj[keys[i]]['Tempo de Execução'];
//     timeFrame.lastElementChild.innerHTML = timeProcess;
//   }
//   numberCars = document.getElementById('TotalCarros');
//   numberCars.lastElementChild.innerHTML = keys.length;

//   numberCarsUp = document.getElementById('CarrosVelocidade');
//   numberCarsUp.lastElementChild.innerHTML = numberCarsUpLimit;

// }

// function updateNumberRoad() {
//   const numberRoad = document.getElementById('TotalRodovias');
//   numberRoad.lastElementChild.innerHTML = Object.keys(dataFrames).length;
// }

// search = document.getElementById('searchInput');

// search.addEventListener('keyup', (e) => {
//   // search on json
//   if( e.key == 'Enter') {
//     value = search.value;
//     console.log(value)
//     if (value == '') {
//       return;
//     }
//     while (table.rows.length > 1) {
//       table.deleteRow(table.rows.length - 1);
//     }
//     table.parentElement.setAttribute('style', 'display: flex;')

//     var frame;
//     console.log(dataFrames)
//     for(let br in dataFrames) {
//       console.log(keysBrs[br][value])
//       if (dataFrames[br][keysBrs[br][value]] == undefined) {
//         continue;
//       }
//       const valueFrame = document.getElementById('value-box')
//       valueFrame.innerHTML = keysBrs[br][value];
//       addRow(dataFrames[br][keysBrs[br][value]],br);
//       updateNumberRoad();
//       frame = keysBrs[br][value]
//     }
    
//   }
// })


// function atualizarDadosCovid() {
//   // Fazer requisição AJAX para obter o arquivo JSON local
//   const xhr = new XMLHttpRequest();
//   xhr.open('GET', '/data', true); // URL do arquivo JSON localizado no servidor
//   xhr.onreadystatechange = function() {
//     dataFrames = {};
//     keysBrs = {};
//     if (xhr.readyState === XMLHttpRequest.DONE) {
//       if (xhr.status === 200) {
//         const dados = JSON.parse(xhr.responseText);
//         console.log(dados)
//         if (dados == null) {
//           return;
//         }
//         let keysNames = Object.keys(dados);
//         console.log(keysNames)
//         api = dados['api']
//         // remove api from keys
//         keysNames.splice(keysNames.indexOf('api'),1);
//         console.log(keysNames)
//         for (let i = 0; i < keysNames.length; i++) {
//           if (keysBrs[keysNames[i]] == undefined) {
//             let keysFrames = Object.keys(dados[keysNames[i]]);
//             keysFrames = keysFrames.sort((a, b) => {
//               return parseInt(a.match(/\d+/)[0]) - parseInt(b.match(/\d+/)[0]);
//             })
//             lastFrame = keysFrames[keysFrames.length - 1];
//             console.log(lastFrame)
//             dataFrames[keysNames[i]] = dados[keysNames[i]];
//             keysBrs[keysNames[i]] = keysFrames;
//           }else{
//             let keysFrames = Object.keys(dados[keysNames[i]]);
//             keysFrames = keysFrames.sort((a, b) => {
//               return parseInt(a.match(/\d+/)[0]) - parseInt(b.match(/\d+/)[0]);
//             })
//             keysBrs[keysNames[i]] = [...keysBrs[keysNames[i]], ...keysFrames];
//             dataFrames[keysNames[i]] = Object.assign({}, dataFrames[keysNames[i]], dados[keysNames[i]]);
//             lastFrame = keysFrames[keysFrames.length - 1];
//           }
//           console.log(dataFrames)
//           let plate = Object.keys(dataFrames[keysNames[i]][lastFrame])[0]
//           tempoExecucao[lastFrame] = dataFrames[keysNames[i]][lastFrame][plate]['Tempo de Execução'];
//           console.log(keysBrs[keysNames[i]].length)
//           lastFrame = keysBrs[keysNames[i]].length - 1;
//         }
//         realTimeUpdate();
//       } else {
//         console.error('Erro ao obter dados de casos de COVID-19:', xhr.statusText);
//       }
//     }
//   };
//   xhr.send();
// }

// // Chamar a função de atualização de dados a cada segundo
// setInterval(atualizarDadosCovid, 5000);

fetch('/data')
  .then(response => response.json())
  .then(data => {
    // Insert data into HTML
    const element = document.getElementById('data-container');
    console.log(data);
    const table = document.createElement('table');
    const headerRow = document.createElement('tr');

    // Create table headers
    Object.keys(data[0]).forEach(key => {
      const th = document.createElement('th');
      th.textContent = key;
      headerRow.appendChild(th);
    });
    table.appendChild(headerRow);

    // Create table rows
    data.forEach(rowData => {
      const tr = document.createElement('tr');
      Object.values(rowData).forEach(value => {
        const td = document.createElement('td');
        td.textContent = value;
        tr.appendChild(td);
      });
      table.appendChild(tr);
    });

    // Insert table into HTML
    element.appendChild(table);

  });

  function updateData() {
    fetch('/data')
      .then(response => response.json())
      .then(data => {
        // Insert data into HTML
        const element = document.getElementById('data-container');
        console.log(data);
        const table = document.createElement('table');
        const headerRow = document.createElement('tr');
  
        // Create table headers
        Object.keys(data[0]).forEach(key => {
          const th = document.createElement('th');
          th.textContent = key;
          headerRow.appendChild(th);
        });
        table.appendChild(headerRow);
  
        // Create table rows
        data.forEach(rowData => {
          const tr = document.createElement('tr');
          Object.values(rowData).forEach(value => {
            const td = document.createElement('td');
            td.textContent = value;
            tr.appendChild(td);
          });
          table.appendChild(tr);
        });
  
        // Insert table into HTML
        element.innerHTML = ''; // Clear previous data
        element.appendChild(table);
      });
  }
  
  let intervalId;

  realTime.addEventListener('change', function() {
    if (realTime.checked) {
      console.log('checked')
      intervalId = setInterval(updateData, 1000); // Call updateData every 1000ms (1 second)
    } else {
      console.log('not checked')
      // disable real time
      clearInterval(intervalId);
    }
  });
  



if (realTime.checked) {
  console.log('checked')
  setInterval(updateData, 1000); // Call updateData every 1000ms (1 second)
}else{
  console.log('not checked')
}

  