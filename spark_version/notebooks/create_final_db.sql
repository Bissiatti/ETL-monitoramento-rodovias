-- Created by Vertabelo (http://vertabelo.com)
-- Last modification date: 2023-06-19 23:39:13.997

-- tables
-- Table: carros
CREATE SCHEMA dashboard;

USE dashboard;

CREATE TABLE carros (
    placa varchar(10)  NOT NULL,
    pos_x float(4,3)  NOT NULL,
    pos_y float(4,3)  NOT NULL,
    aceleracao float(3,3)  NULL,
    velocidade float(3,3)  NULL,
    multas int  NULL,
    risco_colisao bool  NULL,
    direcao_perigosa bool  NULL,
    velocidade_acima bool  NULL,
    CONSTRAINT carros_pk PRIMARY KEY (placa)
);

-- Table: contem
CREATE TABLE contem (
    id int  NOT NULL AUTO_INCREMENT,
    carros_placa varchar(10)  NOT NULL,
    rodovias_horario_registro bigint  NOT NULL,
    rodovias_nome_rodovia varchar(20)  NOT NULL,
    CONSTRAINT contem_pk PRIMARY KEY (id)
);

-- Table: rodovias
CREATE TABLE rodovias (
    nome_rodovia varchar(20)  NOT NULL,
    horario_registro bigint  NOT NULL,
    velocidade_media float(4,2)  NOT NULL,
    tempo_medio_cruzamento float(4,2)  NOT NULL,
    tempo_processamento float(6,4)  NOT NULL,
    total_veiculos int  NULL,
    veiculos_acima_vel int  NOT NULL,
    veiculos_colisao int  NOT NULL,
    CONSTRAINT rodovias_pk PRIMARY KEY (horario_registro,nome_rodovia)
);

-- foreign keys
-- Reference: contem_lista_carros (table: contem)
ALTER TABLE contem ADD CONSTRAINT contem_lista_carros FOREIGN KEY contem_lista_carros (carros_placa)
    REFERENCES carros (placa);

-- Reference: contem_rodovias (table: contem)
ALTER TABLE contem ADD CONSTRAINT contem_rodovias FOREIGN KEY contem_rodovias (rodovias_horario_registro,rodovias_nome_rodovia)
    REFERENCES rodovias (horario_registro,nome_rodovia);

-- End of file.

