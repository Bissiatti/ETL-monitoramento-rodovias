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

std::vector<int> lerArquivoIndices(std::string rodovia, int i){

    std::string file_number = "_" + std::to_string(i) + std::string(".txt");
    std::string file_name = rodovia + "_ind" + file_number;
    
    while (true) {
        std::ifstream file(file_name);

        if (file.is_open()) {
            std::vector<int> numbers{ std::istream_iterator<int>(file), std::istream_iterator<int>() };
            return numbers;
        }

        std::cout << "Não encontrado o arquivo " << file_name << std::endl;
        sleep(2);
    }

}

void read_aggregate(json * hash_agg, std::vector<std::vector<int>> * frames_indexes,
                    std::vector<std::string> rodovias, int index,   std::vector<std::mutex> * mutexes){

        // Start by gathering the data from .txt file
    int file_number = 0;
    std::string rodovia = rodovias[index];

//    while(){ // Commented for checks and debugs
    while(file_number < 2){
    json json_file = lerArquivoSimula(rodovia, file_number);
    std::vector<int> new_indexes = lerArquivoIndices(rodovia, file_number);

        if (file_number == 0)
                    {
                        (*mutexes)[index].lock(); 
                        (*hash_agg)[rodovia] = json_file;

                        (*frames_indexes)[index] = new_indexes;
                         // Deve ser adicionado aqui
                        //ou deve ter um mutex próprio?

                        (*mutexes)[index].unlock();

                    }
                    else
                    {
                        (*mutexes)[index].lock();

                        (*hash_agg)[rodovia].update(json_file); // Impede a leitura do arquivo enquanto este for atualizado
                        
                        (*frames_indexes)[index].insert((*frames_indexes)[index].end(), new_indexes.begin(), new_indexes.end());
                         // Deve ser adicionado aqui
                        //ou deve ter um mutex próprio?
                        
                        (*mutexes)[index].unlock();
                    }

    file_number++;
    }

}


void calcula_carros(json * hash_agg, std::vector<std::vector<int>> * frames_indexes,
                    std::vector<std::string> rodovias, int index,
                    std::vector<std::mutex> * mutexes){

    std::string rodovia = rodovias[index];
    
    (*mutexes)[index].lock(); 

    std::vector<int> frames_inds = (*frames_indexes)[index];
    json inner_hash = (*hash_agg)[rodovia];                 //Como controlar o tamanho do que é pego para ser calculado?

    (*mutexes)[index].unlock();

     for (auto j = 0; j != frames_inds.size() ; j++ ){                   //itera pelos frames

        std::string frame = std::to_string( frames_inds[j] );
        
        for(auto k = 0; k != inner_hash[  frame ].size() ; k++ ){       //itera pela lista dicionário de carros

            for(auto carro = inner_hash[  frame ][k].begin() ; carro != inner_hash[  frame ][k].end() ; ++carro ){   // itera pelos carros

            std::cout << carro.key() << std::endl;

            }

        }

     }

}

int main() {

    int NUM_THREADS = 1;

    //Lista dos nomes das rodovias
    std::vector<std::string> rodovias;
    rodovias.push_back("BR-040");

    //Criado o que contará com os dados agregados
    json hash_agg;
    std::vector<std::thread> threads_files(NUM_THREADS);
    std::vector<std::thread> threads_calculations(NUM_THREADS);
    std::vector< std::vector<int> > frames_indexes(NUM_THREADS);
    std::vector<std::mutex> mutexes(NUM_THREADS); 

    //Criar o JSON para as velocidades, acidentes etc -> resultado dos cálculos
    json rodovia_frame_agg;
    
    // Um mutex diferente para cada rodovia -> Controlar acesso na escrita e nos cálculos

    for (int l = 0; l < NUM_THREADS; l++) {
              threads_files[l] = std::thread(read_aggregate, &hash_agg, &frames_indexes, rodovias, l, &mutexes);
    }

    threads_files[0].join();

    for (int l = 0; l < NUM_THREADS; l++) {
              threads_calculations[l] = std::thread(calcula_carros, &hash_agg, &frames_indexes, rodovias, l, &mutexes);
    }
    
    threads_calculations[0].join();

    return 0;
}
