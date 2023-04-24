# Sistema de monitoramento de rodovias, etl, simulação e dashboard

## Simulador de tráfego

1. python 3
2. pip
3. pygame (para visualiar a simulação, opcional)
    pip install pygame
4. numpy
    pip install numpy

Para executar o simulador, basta executar o arquivo `simulador.py` a partir da raiz do projeto com o comando:

```bash
python mock/simulador.py nome-da-rodovia
```
Observação: a rodovia precisa existir nos arquivos de configuração do simulador, que estão na pasta `mock/parametros.json`.

## ETL

1. Intalar o gcc e o g++ (para compilar o código C++ do simulador)
2. Instalar o git
3. Instalar a bibloteca de json para C++:

```bash
git clone https://github.com/nlohmann/json.git
```

4. Instalar o cmake e compilar a biblioteca:

```bash
cd json
cmake .
make
sudo make install
```

Em seguida compile o código C++ do simulador e execute o arquivo `main`:

```bash
cd ..
g++ main.cpp -o main
./main
```
 
## Dashboard

1. Instalar o nodejs
2. Instalar o npm
3. Instalar as dependências do projeto:

```bash
cd dash
npm install
```

4. Iniciar o servidor:

```bash
npm start
```

5. Acessar o dashboard em `http://localhost:3000`

## Resumo da execução

1. Execute o simulador, em seguida execute o ETL e por fim o dashboard com o comando:

```bash
python mock/simulador.py BR-040 && g++ main.cpp -o main && ./main && cd dash && npm start
```
E abra o dashboard em `http://localhost:3000`, a cada 5 segundos o dashboard será atualizado com os dados do ETL. 