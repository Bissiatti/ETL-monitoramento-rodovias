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

function updateData2() {
  fetch('/rodovia')
    .then(response => response.json())
    .then(data => {
      // Insert data into HTML
      const element2 = document.getElementById('data-container2-1');
      console.log(data);
      const table = document.createElement('table');

      // Transpose data
      const transposedData = Object.keys(data[0]).map(key => [key, ...data.map(row => row[key])]);

      // Create table rows
      transposedData.forEach(rowData => {
        const tr = document.createElement('tr');
        rowData.forEach((value, index) => {
          const cell = index === 0 ? document.createElement('th') : document.createElement('td');
          cell.textContent = value;
          tr.appendChild(cell);
        });
        table.appendChild(tr);
      });

      // Insert table into HTML
      element2.innerHTML = ''; // Clear previous data
      element2.appendChild(table);
    });
    fetch('/top')
    .then(response => response.json())
    .then(data => {
      // Insert data into HTML
      const element = document.getElementById('data-container2-2');
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
      intervalId = setInterval(updateData2, 1000); // Call updateData every 1000ms (1 second)
    } else {
      console.log('not checked')
      // disable real time
      clearInterval(intervalId);
    }
  });
  



if (realTime.checked) {
  console.log('checked')
  setInterval(updateData, 2000); // Call updateData every 1000ms (1 second)
  setInterval(updateData2, 2000); // Call updateData every 1000ms (1 second)
}else{
  console.log('not checked')
}

  