#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <unistd.h>
#include <chrono>
#include <filesystem>
#include <mutex>
#include "functions.hpp"
#include "API/API.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main() {

     int NUM_THREADS = 0;

     //Lista dos nomes das rodovias
    std::vector<std::string> rodovias;

    APIQueue apiQueue(10);

    //Carregando dados de parametros
	json parametros;
    json batches_adm;
    std::string parameters_json = std::string("./mock/parametros.json");
	std::ifstream file_parameters_json(parameters_json, std::ios::in);
    if (file_parameters_json.is_open()) {
            file_parameters_json >> parametros;
        }else{
            std::cout << "Não encontrado o arquivo " << parameters_json << std::endl;
        }
	
	  for (auto& parametros_rodovia : parametros.items()) {
        rodovias.push_back(parametros_rodovia.key());
		batches_adm[parametros_rodovia.key()] = {};
		batches_adm[parametros_rodovia.key()]["Ultimo"] = 0;
        NUM_THREADS++;
  }   

    //Criado o que contará com os dados agregados
    json hash_agg;
    std::vector<std::thread> threads_files(NUM_THREADS);
    std::vector<std::thread> threads_calculations(NUM_THREADS);
    std::vector< std::vector<int> > frames_indexes(NUM_THREADS);
    std::vector<std::mutex> mutexes(NUM_THREADS);

    for (int l = 0; l < NUM_THREADS; l++) {
              threads_files[l] = std::thread(read_aggregate, &hash_agg, &frames_indexes, rodovias, l, &mutexes, &batches_adm);
    }


    for (int l = 0; l < NUM_THREADS; l++) {
              threads_calculations[l] = std::thread(calcula_carros, &hash_agg, &frames_indexes,
                                                            rodovias, l, &mutexes, &parametros, &batches_adm, &apiQueue);
    }
    

    while (true){
        continue;
    }

    return 0;

}
