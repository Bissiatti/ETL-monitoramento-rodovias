CREATE KEYSPACE simulacao WITH replication = {'class': 'SimpleStrategy', 'replication_factor': 1};
USE simulacao;
CREATE TABLE simulacao (
  placa text,
  rodovia text,
  tempo_da_simulacao int,
  pos_x double,
  pos_y double,
  PRIMARY KEY ((rodovia,tempo_da_simulacao),placa)
);