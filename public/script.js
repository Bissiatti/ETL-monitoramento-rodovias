// // Dados de exemplo para as três séries de dados
// const data = {
//   labels: ['Janeiro', 'Fevereiro', 'Março', 'Abril', 'Maio'],
//   datasets: [{
//     label: 'Série 1',
//     data: [10, 20, 30, 40, 50], // Dados da primeira série
//     borderColor: 'red', // Cor da linha da primeira série
//     fill: false // Não preencher a área abaixo da linha da primeira série
//   }, {
//     label: 'Série 2',
//     data: [20, 30, 40, 50, 60], // Dados da segunda série
//     borderColor: 'blue', // Cor da linha da segunda série
//     fill: false // Não preencher a área abaixo da linha da segunda série
//   }, {
//     label: 'Série 3',
//     data: [30, 40, 50, 60, 70], // Dados da terceira série
//     borderColor: 'green', // Cor da linha da terceira série
//     fill: false // Não preencher a área abaixo da linha da terceira série
//   }]
// };

// // Opções de configuração do gráfico
// const options = {
//   responsive: true, // Tornar o gráfico responsivo
//   maintainAspectRatio: false, // Não manter a proporção de aspecto
//   scales: {
//     x: {
//       display: true,
//       title: {
//         display: true,
//         text: 'Mês' // Rótulo do eixo X
//       }
//     },
//     y: {
//       display: true,
//       title: {
//         display: true,
//         text: 'Valor' // Rótulo do eixo Y
//       }
//     }
//   }
// };

// // Criar o gráfico de linha com os dados e opções fornecidas
// const ctx = document.getElementById('myChart').getContext('2d');
// new Chart(ctx, {
//   type: 'line',
//   data: data,
//   options: options
// });


function atualizarDadosCovid() {
  // Fazer requisição AJAX para obter o arquivo JSON local
  const xhr = new XMLHttpRequest();
  xhr.open('GET', '/data', true); // URL do arquivo JSON localizado no servidor
  xhr.onreadystatechange = function() {
    if (xhr.readyState === XMLHttpRequest.DONE) {
      if (xhr.status === 200) {
        const dados = JSON.parse(xhr.responseText);
        console.log(dados);
        const numeroCasos = JSON.stringify(dados);        // Extrair o número de casos dos dados recebidos
        const casosCovidElement = document.getElementById('casos-covid');
        casosCovidElement.textContent = 'Número de Casos de COVID-19: ' + numeroCasos; // Atualizar o elemento na página com os dados recebidos
      } else {
        console.error('Erro ao obter dados de casos de COVID-19:', xhr.statusText);
      }
    }
  };
  xhr.send();
}

// Chamar a função de atualização de dados a cada segundo
setInterval(atualizarDadosCovid, 10000);