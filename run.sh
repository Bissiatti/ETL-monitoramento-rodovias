#!/bin/bash

# Define a variável MY_INSTALL_DIR com o diretório desejado
MY_INSTALL_DIR=$HOME/.local

# Cria o diretório para a construção
mkdir -p cmake/build

# Entra no diretório
pushd cmake/build

# Gera os arquivos de geração usando o cmake
cmake -DCMAKE_PREFIX_PATH=$MY_INSTALL_DIR ../..

# Compila o projeto usando o make
make -j 4

# Sai do diretório
popd
