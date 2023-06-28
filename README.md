# ETL-monitoramento-rodovias

Repositório do trabalho final da disciplina de Computação Escalável de 2023 do curso de Ciência de Dados da FGV - EMAp

O repositório é divido em 2 implementações principais, sendo uma em C++ e a outra utilizando PySpark. Dentro das pastas das respectivas implementações há instruções de como executa-las.

## Como executar a versão do PySpark

Para executar a versão spark é necessário ter o Docker instalado e executar o seguinte comando na pasta spark-version:

```bash
docker-compose up
```

Em seguida, abra o notebook no endereço http://localhost:8888/

Abra o terminal do notebook e execute o seguinte comando para iniciar os bancos de dados:

```bash
bash init_dbs.sh

```

- Obs: caso ocorra algum erro no comando acima, aguarde alguns segundos e tente novamente. Os bancos de dados podem demorar alguns segundos para iniciar.

Para iniciar o simulador, no terminal do notebook, execute o seguinte comando:

```bash
python3 mock/simulator.py BR-116
```

Para iniciar o ETL pyspark, no terminal do notebook, execute o seguinte comando:

```bash
python3 pipeline_novo.py
```

Por fim, abra uma nova janela do seu navegador e acesse o endereço http://localhost:3000/ para visualizar o dashboard.
