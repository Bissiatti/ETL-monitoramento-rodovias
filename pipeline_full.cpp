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

    std::string file_name = rodovia + "_" + std::to_string(i);
    std::string file_name_json = std::string("./data/") + file_name + std::string(".json");

    while (true) {
        std::ifstream file(file_name_json, std::ios::in);

        if (file.is_open()) {
            json j;
            file >> j;
            return j;
        }

        std::cout << "Não encontrado o arquivo " << file_name_json << std::endl;
        sleep(1);
    }

}

std::vector<int> lerArquivoIndices(std::string rodovia, int i){

    std::string file_name = rodovia + "_" + std::to_string(i);
    std::string file_name_txt = std::string("./delta-time/") + file_name + std::string(".txt");
    
    while (true) {
        std::ifstream file(file_name_txt, std::ios::in);

        if (file.is_open()) {
            std::vector<int> numbers{ std::istream_iterator<int>(file), std::istream_iterator<int>() };
            return numbers;
        }

        std::cout << "Não encontrado o arquivo " << file_name_txt << std::endl;
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

    sleep(3); // sleep simples para testes -----------------------------------------------------------------

    file_number++;
    }

}


void calcula_carros(json * hash_agg, std::vector<std::vector<int>> * frames_indexes,
                    std::vector<std::string> rodovias, int index,
                    std::vector<std::mutex> * mutexes, json * parametros){

    std::string rodovia = rodovias[index];

    json rodovia_frame_agg = json {};
    
    rodovia_frame_agg[rodovia] = json {};

    int ultimo_index = 0; // inicializamos esta variável que ajudará a iterar

    int ultimo_frame = 0;

    int contador = 0;

    double frame_num = 0;
	
	// número de frames no qual consideraremos como risco de colisão
	// caso seja previsto que os carros batam após esse número de frames
	int frame_tolerancia_colisao = 1;

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
        for (auto frame_index = ultimo_index+1; frame_index < frames_inds.size() ; ++frame_index ){
                        // Começamos pulando o primeiro que não terá dados interessantes

            waiting = 0; // Se entrou no for, é por que não esta mais esperando e há mais coisas para iterar

            std::string frame = std::to_string( frames_inds[frame_index] );
            std::string ultimo_frame_str = std::to_string( ultimo_frame );

            frame_num = frames_inds[frame_index];

            rodovia_frame_agg[rodovia][frame] = json {};

// ---------- Iterações pelos carros ----------------
            for (auto carro = inner_hash[  frame ].begin(); carro != inner_hash[  frame ].end(); ++carro){
                
                std::string placa = carro.key();

                rodovia_frame_agg[rodovia][frame][placa] = json {};

                //// --------------- Cálculos de cada carro 

                // Posição ---------------
                rodovia_frame_agg[rodovia][frame][placa]["Posicao"] = carro.value();
				double pos_prevista;
				//s = s_0
				pos_prevista = inner_hash[frame][ placa ][ 1 ];
				
                // Velocidade ---------------
                if ( rodovia_frame_agg[rodovia][ultimo_frame_str].contains(placa) ){
				
                double t_1 = inner_hash[frame][ placa ][ 1 ]; // Estaremos olhando apenas para o valor Y do eixo

                double t_0 = inner_hash[ultimo_frame_str][ placa ][ 1 ]; 

                double diff_frame = frame_num - ultimo_frame;

                rodovia_frame_agg[rodovia][frame][placa]["Velocidade"] = std::abs(t_1 - t_0) / diff_frame ;
				
				double vel_1 = rodovia_frame_agg[rodovia][frame][placa]["Velocidade"];; // Estaremos olhando apenas para o valor Y do eixo
				
                bool acima_da_vel = (vel_1 > (*parametros)[rodovia]["velocidadeMaxima"]);
				
                rodovia_frame_agg[rodovia][frame][placa]["Acima da Velocidade"] = acima_da_vel; 
				//s = s_0 +v*t
				pos_prevista = t_1 + vel_1*diff_frame*frame_tolerancia_colisao;
                }

                // Aceleração ---------------
                if ( rodovia_frame_agg[rodovia][ultimo_frame_str][placa].contains("Velocidade") ){

                double t_1 = inner_hash[frame][ placa ][ 1 ];

				double vel_1 = rodovia_frame_agg[rodovia][frame][placa]["Velocidade"];; // Estaremos olhando apenas para o valor Y do eixo
				
                double vel_0 = rodovia_frame_agg[rodovia][ultimo_frame_str][placa]["Velocidade"]; 

                double diff_frame = frame_num - ultimo_frame;

                rodovia_frame_agg[rodovia][frame][placa]["Aceleração"] = (vel_1 - vel_0) / diff_frame ;
                
				double acel_1 = rodovia_frame_agg[rodovia][frame][placa]["Aceleração"];
				
				// s = s_0 + v_0*t + a*t²/2
				double delta_t = diff_frame*frame_tolerancia_colisao;
				pos_prevista = t_1 + vel_1*delta_t + acel_1*std::pow(delta_t, 2)/2;
				}
				
				rodovia_frame_agg[rodovia][frame][placa]["Posição Prevista"] = pos_prevista;

				std::vector<std::string> riscos;
				
				rodovia_frame_agg[rodovia][frame][placa]["Risco Colisão"] = riscos;}
                // Acidente ---------------
            for(auto carro = inner_hash[  frame ].begin(); carro != inner_hash[  frame ].end(); ++carro){
                
				for(auto carro2 = inner_hash[  frame ].begin(); carro2 != inner_hash[  frame ].end(); ++carro2){
                    if (carro.key() == carro2.key()){ continue; }
                    //checa se após o dado número de frames um carro ultrapassará o outro (assim, colidindo)
                    if( carro.value()[0] == carro2.value()[0]){
							double pos = carro.value()[0];
							double pos2 = carro2.value()[0];
							std::string placa = carro.key();
							double pos_prevista = rodovia_frame_agg[rodovia][frame][placa]["Posição Prevista"];
							std::string placa2 = carro2.key();
							double pos_prevista2 = rodovia_frame_agg[rodovia][frame][placa2]["Posição Prevista"];
							if ((pos > pos2 and pos_prevista < pos_prevista2)
							or (pos < pos2 and pos_prevista > pos_prevista2)){
								(rodovia_frame_agg[rodovia][frame][placa]["Risco Colisão"]).push_back(placa2);								
						}
                    }

                }

            }
			ultimo_frame = frame_num;
            ultimo_index = frame_index;
            }

        if( !waiting){
        std::string filename_output = std::string("./output/") + rodovia + std::string("_output_") + std::to_string(contador) + std::string(".txt");

        std::ofstream file(filename_output);

        file << rodovia_frame_agg;

        file.close();

        std::cout << "arquivo " << filename_output <<  " salvo" << std::endl;

        contador++;

        rodovia_frame_agg[rodovia] = json {};
        }
		sleep(1.5);
        
    }        
}



int main() {

    int NUM_THREADS = 2;

    //Lista dos nomes das rodovias
    std::vector<std::string> rodovias;

    //Carregando dados de parametros
	json parametros;
    std::string parameters_json = std::string("./mock/parametros.json");
	std::ifstream file_parameters_json(parameters_json, std::ios::in);
    if (file_parameters_json.is_open()) {
            file_parameters_json >> parametros;
        }else{
            std::cout << "Não encontrado o arquivo " << parameters_json << std::endl;
        }
	
	  for (auto& parametros_rodovia : parametros.items()) {
        rodovias.push_back(parametros_rodovia.key());
  }


    //Criado o que contará com os dados agregados
    json hash_agg;
    std::vector<std::thread> threads_files(NUM_THREADS);
    std::vector<std::thread> threads_calculations(NUM_THREADS);
    std::vector< std::vector<int> > frames_indexes(NUM_THREADS);
    std::vector<std::mutex> mutexes(NUM_THREADS); 

    for (int l = 0; l < NUM_THREADS; l++) {
              threads_files[l] = std::thread(read_aggregate, &hash_agg, &frames_indexes, rodovias, l, &mutexes);
    }


    for (int l = 0; l < NUM_THREADS; l++) {
              threads_calculations[l] = std::thread(calcula_carros, &hash_agg, &frames_indexes, rodovias, l, &mutexes, &parametros);
    }
    

    sleep(15); // sleep simples para testes -----------------------------------------------------------------


    return 0;

}
