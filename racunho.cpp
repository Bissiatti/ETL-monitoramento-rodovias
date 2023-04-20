#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <unistd.h>
#include <nlohmann/json.hpp>
#include <dirent.h>

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

void read_aggregate(std::vector<json> * hash_agg, std::vector<std::string> rodovias, int index){
        // Start by gathering the data from .txt file
    int file_number = 1;
    std::string rodovia = rodovias[index];

//    while(){ // Commented for checks and debugs
    while(file_number < 2){
    json json_file = lerArquivoSimula(rodovia, file_number);

    (*hash_agg)[index] = json_file;

    file_number++;
    }

}

double calc_speed(std::vector<json> *hash_agg,
                  int index, std::string placa)
{
//TODO: colocar os last_frames reais para teste
std::vector<std::string> last_frames = {"29991", "30008"};
json dados_rodovia = (*hash_agg)[index];
json dados_frame_1 = dados_rodovia[last_frames[0]];
json dados_frame_2 = dados_rodovia[last_frames[1]];
double pos1[2];
pos1[2] = dados_frame_1[placa];
double pos2[2];
pos2[2] = dados_frame_2[placa];

//return std::sqrt(std::pow(pos2[0]-pos1[0], 2) +
//std::pow(pos2[1]-pos[1], 2))/
//(int(last_frames[1])-int(last_frames[0]))
}

int main()
{

    int NUM_THREADS = 1;

    // Lista dos nomes das rodovias
    std::vector<std::string> rodovias;
    rodovias.push_back("BR-040");

    // Criado o que contará com os dados agregados
    std::vector<json> hash_agg(NUM_THREADS);
    std::vector<std::thread> threads(NUM_THREADS);

    for (int l = 0; l < NUM_THREADS; l++)
    {
        threads[l] = std::thread(read_aggregate, &hash_agg, rodovias, l);
    }

    threads[0].join();
    std::cout << hash_agg << "\n";

    return 0;
}
