#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <unistd.h>
#include <chrono>
#include <filesystem>
#include <mutex>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

json lerArquivoSimula(std::string rodovia, int i){

    std::string file_number = "_" + std::to_string(i) + std::string(".txt");
    std::string file_name = rodovia + file_number;

    while (true) {
        std::ifstream file(file_name);

        if (file.is_open()) {
            json j;
            file >> j;
            return j;
        }

        std::cout << "Não encontrado o arquivo " << file_name << std::endl;
        sleep(2);
    }

}

void read_aggregate(std::vector<json> * hash_agg, std::vector<std::string> rodovias,
                
                int index,   std::vector<std::mutex> * mutexes){

        // Start by gathering the data from .txt file
    int file_number = 1;
    std::string rodovia = rodovias[index];

//    while(){ // Commented for checks and debugs
    while(file_number < 3){
    json json_file = lerArquivoSimula(rodovia, file_number);

        if (file_number == 1)
                    {
                        (*hash_agg)[index] = json_file;
                    }
                    else
                    {
                        (*mutexes)[index].lock();
                        (*hash_agg)[index].update(json_file); // Impede a leitura do arquivo enquanto este for atualizado
                        (*mutexes)[index].unlock();
                    }

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
    std::vector<std::mutex> mutexes(NUM_THREADS); 
    
    // Um mutex diferente para cada rodovia -> Controlar acesso na escrita e nos cálculos

    for (int l = 0; l < NUM_THREADS; l++) {
              threads[l] = std::thread(read_aggregate, &hash_agg, rodovias, l, &mutexes);
    }

    threads[0].join();
    std::cout << hash_agg << "\n";

    return 0;
}
