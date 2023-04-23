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
        sleep(1);
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
        sleep(1);
    }

}

void read_aggregate(json * hash_agg, std::vector<std::vector<int>> * frames_indexes,
                    std::vector<std::string> rodovias, int index,   std::vector<std::mutex> * mutexes){

        // Start by gathering the data from .txt file
    int file_number = 0;
    std::string rodovia = rodovias[index];

//    while( file_number < 3){ // Commented for checks and debugs
    while( true ){
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

    //sleep(1); 

    file_number++;
    }

}


void calcula_carros(json * hash_agg, std::vector<std::vector<int>> * frames_indexes, json * rodovia_frame_agg,
                    std::vector<std::string> rodovias, int index,
                    std::vector<std::mutex> * mutexes){

    std::string rodovia = rodovias[index];

    (*rodovia_frame_agg)[rodovia] = json {};

    int ultimo_index = 0; // inicializamos esta variável que ajudará a iterar

    int ultimo_frame = 0;

    int contador = 0;

    double frame_num = 0;

    while (true){

        int waiting = 1; // contador que fará com que não seja salvo arquivos do output das iterações

        (*mutexes)[index].lock(); 

            std::vector<int> frames_inds = (*frames_indexes)[index];
            json inner_hash = (*hash_agg)[rodovia];                 //Como controlar o tamanho do que é pego para ser calculado?

        (*mutexes)[index].unlock();

/*         for (auto j = 0; j < frames_inds.size(); j++){
        std::cout << "frames_indexes de dentro do calculas_carros " << j << " " << frames_inds[j] << std::endl; 
        } */

// ---------- Iterações pelos frames ----------------
        for (auto frame_index = ultimo_index+1; frame_index < frames_inds.size() ; frame_index++ ){
                        // Começamos pulando o primeiro que não terá dados interessantes

            waiting = 0; // Se entrou no for, é por que não esta mais esperando e há mais coisas para iterar

            std::string frame = std::to_string( frames_inds[frame_index] );
            std::string ultimo_frame_str = std::to_string( ultimo_frame );

            frame_num = frames_inds[frame_index];

            (*rodovia_frame_agg)[rodovia][frame] = json {};

// ---------- Iterações pelos carros ----------------
            for (auto carro = inner_hash[  frame ].begin(); carro != inner_hash[  frame ].end(); ++carro){
                
                std::string placa = carro.key();

                (*rodovia_frame_agg)[rodovia][frame][placa] = json {};

                //// --------------- Cálculos de cada carro 

                // Posição ---------------
                (*rodovia_frame_agg)[rodovia][frame][placa]["Posicao"] = carro.value();

                // Velocidade ---------------
                if ( (*rodovia_frame_agg)[rodovia][ultimo_frame_str].contains(placa) ){
                double t_1 = inner_hash[frame][ placa ][ 1 ]; // Estaremos olhando apenas para o valor Y do eixo

                double t_0 = inner_hash[ultimo_frame_str][ placa ][ 1 ]; 

                double diff_frame = frame_num - ultimo_frame;

                (*rodovia_frame_agg)[rodovia][frame][placa]["Velocidade"] = std::abs(t_1 - t_0) / diff_frame ;
                }

                // Aceleração ---------------
                if ( (*rodovia_frame_agg)[rodovia][ultimo_frame_str][placa].contains("Velocidade") ){
                double vel_1 = (*rodovia_frame_agg)[rodovia][frame][placa]["Velocidade"];; // Estaremos olhando apenas para o valor Y do eixo

                double vel_0 = (*rodovia_frame_agg)[rodovia][ultimo_frame_str][placa]["Velocidade"]; 

                double diff_frame = frame_num - ultimo_frame;

                (*rodovia_frame_agg)[rodovia][frame][placa]["Aceleração"] = (vel_1 - vel_0) / diff_frame ;
                } 



            }

            // FAZER AQUI A QUESTÃO DO CÁLCULO + ACIDENTE AQUI

            ultimo_frame = frame_num;

            ultimo_index = frame_index;

        }


        if( !waiting){
        
        std::string filename_output = rodovia + std::string("_output_") + std::to_string(contador) + std::string(".txt");

        std::ofstream file(filename_output);
        file << (*rodovia_frame_agg)[rodovia];
        file.close();

        std::cout << "arquivo " << filename_output <<  " salvo" << std::endl;

        contador++;
        }

        sleep(1.5);

    }


}

int main() {

    int NUM_THREADS = 1;

    //Lista dos nomes das rodovias
    std::vector<std::string> rodovias;

    rodovias.push_back("BR-101");

    //Criado o que contará com os dados agregados
    json hash_agg;
    std::vector<std::thread> threads_files(NUM_THREADS);
    std::vector<std::thread> threads_calculations(NUM_THREADS);
    std::vector< std::vector<int> > frames_indexes(NUM_THREADS);
    std::vector<std::mutex> mutexes(NUM_THREADS); 

    //Criar o JSON para as velocidades, acidentes etc -> resultado dos cálculos
    json rodovia_frame_agg;
    
    // Um mutex diferente para cada rodovia -> Controlar acesso na escrita e nos cálculos

   // inicializa o JSON de cálculos
    for (int l = 0; l < NUM_THREADS; l++) {
              rodovia_frame_agg[ rodovias[l] ] = json {}; // rodovias_dict

    }


    for (int l = 0; l < NUM_THREADS; l++) {
              threads_files[l] = std::thread(read_aggregate, &hash_agg, &frames_indexes, rodovias, l, &mutexes);
    }


    for (int l = 0; l < NUM_THREADS; l++) {
              threads_calculations[l] = std::thread(calcula_carros, &hash_agg, &frames_indexes, &rodovia_frame_agg, rodovias, l, &mutexes);
    }
    

    sleep(15); // sleep simples para testes


    return 0;
}
