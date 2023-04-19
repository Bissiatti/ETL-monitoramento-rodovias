#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <unistd.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

json lerArquivoSimula(std::string rodovia, int i){

    std::string file_number = "_" + std::to_string(i) + std::string(".txt");
    std::string file_name = rodovia + file_number;

    std::ifstream file(file_name);

    if (!file.is_open()) {
    std::cout << "Não foi possível abrir o arquivo." << std::endl;
    }

    json j;
    file >> j;

    return j;
}

void read_aggregate(std::vector<json> * hash_agg, std::vector<std::string> rodovias, int index){
        // Start by gathering the data from .txt file
    int file_number = 1;
    std::string rodovia = rodovias[index];

//    while( true ){ // Commented for checks and debugs
    while(file_number < 2){
    json json_file = lerArquivoSimula(rodovia, file_number);

    (*hash_agg)[index] = json_file;

    file_number++;
    }

}


int main() {

    int NUM_THREADS = 1;

    //Lista dos nomes das rodovias
    std::vector<std::string> rodovias;
    rodovias.push_back("BR-040");

    //Criado o que contará com os dados agregados
    std::vector<json> hash_agg(NUM_THREADS);
    std::vector<std::thread> threads(NUM_THREADS);

    for (int l = 0; l < NUM_THREADS; l++) {
              threads[l] = std::thread(read_aggregate, &hash_agg, rodovias, l);
    }

    threads[0].join();
    std::cout << hash_agg << "\n";

    return 0;
}
