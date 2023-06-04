#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <unistd.h>
#include <chrono>
#include <filesystem>
#include <mutex>
#include "API/API.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// int getIndex(std::vector<std::string> myList, std::string rodovia) {
//     for (size_t i = 0; i < myList.size(); ++i) {
//         if (myList[i] == rodovia) {
//             return static_cast<int>(i);  // Return index as int
//         }
//     } 
//     return -1;  // Return -1 if target is not found
// }

// void update_data(std::string new_string_json,
//                      json * hash_agg, std::vector<std::vector<int>> * frames_indexes,
//                     std::vector<std::string> rodovias,
//                     std::vector<std::mutex> * mutexes, json * batches_adm){
    
//     json_file = nlohmann::json::parse(new_string_json);
//     int file_number = json_file["number"];
//     std::string rodovia = json_file["name"];
//     int index = getIndex(rodovias, name_rodovia)

// 	(*batches_adm)[rodovia][std::to_string(file_number)]["start"] = json_file["now"];
//     std::vector<int> new_indexes{ std::istream_iterator<int>(file), std::istream_iterator<int>() };
			
// 	int primeiro = new_indexes.front();
// 	(*batches_adm)[rodovia][std::to_string(file_number)]["Primeiro Frame"] = std::to_string(primeiro);
// 	int ultimo = new_indexes.back();
// 	(*batches_adm)[rodovia][std::to_string(file_number)]["Ultimo Frame"] = std::to_string(ultimo);

//     if (file_number == 0)
//         {
//         (*mutexes)[index].lock(); 
//         (*hash_agg)[rodovia] = json_file["data"];

//         (*frames_indexes)[index] = new_indexes;
//          // Deve ser adicionado aqui
//         //ou deve ter um mutex próprio?

//         (*mutexes)[index].unlock();

//         }
//         else
//         {
//         (*mutexes)[index].lock();

//         (*hash_agg)[rodovia].update(json_file["data"]); // Impede a leitura do arquivo enquanto este for atualizado
                        
//         (*frames_indexes)[index].insert((*frames_indexes)[index].end(), new_indexes.begin(), new_indexes.end());
//          // Deve ser adicionado aqui
//         //ou deve ter um mutex próprio?
                        
//         (*mutexes)[index].unlock();
//         }
// }

json Message_reader(std::queue<string> * my_queue, std::mutex * thread_mutex){
    
    std::string rodovia_json = "";

    while(rodovia_json == ""){

    thread_mutex->lock();
    
        if (!my_queue->empty()){
            rodovia_json = my_queue->front();
            my_queue->pop();
        }

    thread_mutex->unlock();

    }
    return json::parse(rodovia_json);
}

json lerArquivoSimula(json mensagem){

    return mensagem["data"];
    
}

std::vector<int> lerArquivoIndices(json mensagem, json * batches_adm) {//, int i, json * batches_adm){


    //std::string file_name = rodovia + "_" + std::to_string(i);
    //std::string file_name_txt = std::string("./delta-time/") + file_name + std::string(".txt");
    
    //Parse da mensagem


    int num = mensagem["number"];
    std::string rodovia = mensagem["name"];

    std::string i = std::to_string(num);

    
    (*batches_adm)[rodovia][i]["start"] = mensagem["now"];


    // std::string[] numbers = mensagem["delta_time"];

    //std::vector<string> numbers(mensagem["delta_time"].begin(), mensagem["delta_time"].end());


    std::vector<string> numbers = mensagem["delta_time"].get<std::vector<string>>();

    std::vector<int> ints;
    
    ints.reserve(numbers.size()); // reserva espaço para o mesmo número de elementos
    

    // opção 1: usar um loop for
    for (const auto& s : numbers) {
    try {
        ints.push_back(std::stoi(s)); // tenta converter a string para um int e adiciona no vetor
    }
    catch (std::invalid_argument& e) {
        std::cout << "String inválida: " << s << "\n"; // trata o erro
    }
    }
    

	int primeiro = ints.front();
	(*batches_adm)[rodovia][i]["Primeiro Frame"] = std::to_string(primeiro);
	int ultimo = ints.back();
	(*batches_adm)[rodovia][i]["Ultimo Frame"] = std::to_string(ultimo);
    

	return ints;

}

// std::vector<int> lerArquivoIndices(json mensagem, json * batches_adm) {//, int i, json * batches_adm){

//     //std::string file_name = rodovia + "_" + std::to_string(i);
//     //std::string file_name_txt = std::string("./delta-time/") + file_name + std::string(".txt");
    
//     //Parse da mensagem

//     int i = mensagem["number"];
//     std::string rodovia = mensagem["name"];
    
//     (*batches_adm)[rodovia][std::to_string(i)]["start"] = mensagem["now"];
//     std::vector<int> numbers = mensagem["delta_time"];
			
// 	int primeiro = numbers.front();
// 	(*batches_adm)[rodovia][std::to_string(i)]["Primeiro Frame"] = std::to_string(primeiro);
// 	int ultimo = numbers.back();
// 	(*batches_adm)[rodovia][std::to_string(i)]["Ultimo Frame"] = std::to_string(ultimo);

// 	return numbers;

// }

void read_aggregate(json * hash_agg, std::vector<std::vector<int>> * frames_indexes,
                    std::vector<std::string> rodovias, int index,
                    std::vector<std::mutex> * mutexes, json * batches_adm,
                     std::mutex * thread_mutex, std::queue<string> * my_queue){

    // Start by gathering the data from .txt file
    int file_number = 0;
    std::string rodovia = rodovias[index];

    // while( file_number < 3){ // Commented for checks and debugs
    while( true ){

    //PASSAR AQUI STRING DO gRPC ----------------------------------------------------------
    
    //separar os dois

    if (my_queue->empty() == false){
        continue;
    }
    json mensagem = Message_reader(my_queue, thread_mutex);

    json json_file = lerArquivoSimula(mensagem);
    std::vector<int> new_indexes = lerArquivoIndices(mensagem, batches_adm);

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

    sleep(2.5); // Para não deixar o processo muito rápido

    file_number++;

    }

}

std::string getBatch(std::string rodovia, std::string frame, json * batches_adm){

    int frame_int = std::stoi(frame);
    json inner_batch = (*batches_adm)[rodovia];
    int ultimo_batch = (*batches_adm)[rodovia]["Ultimo"];

    int batch = (*batches_adm)[rodovia]["Ultimo"];
    
    while(true)
    {   std::string ultimo = inner_batch[std::to_string(batch)]["Ultimo Frame"];
        std::string primeiro = inner_batch[std::to_string(batch)]["Primeiro Frame"];
        if (std::stoi(ultimo) < frame_int){
            batch++;}
        else if (std::stoi(primeiro) <= frame_int){
            (*batches_adm)[rodovia]["Ultimo"] = batch;
            
            return std::to_string(batch);
		}
	}
}


void calcula_carros(json * hash_agg, std::vector<std::vector<int>> * frames_indexes,
                    std::vector<std::string> rodovias, int index,
                    std::vector<std::mutex> * mutexes, json * parametros,
                    json * batches_adm, APIQueue * apiQueue){
        
    std::string rodovia = rodovias[index];

    json rodovia_frame_agg = json {};
    
    rodovia_frame_agg[rodovia] = json {};

    rodovia_frame_agg["api"] = json {};

    int ultimo_index = 0; // inicializamos esta variável que ajudará a iterar

    int ultimo_frame = 0;

    int contador = 0;

    double frame_num = 0;
	
	// número de frames no qual consideraremos como risco de colisão
	// caso seja previsto que os carros batam após esse número de frames
	int frame_tolerancia_colisao = 60;

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

                // Requisição da API
                if (!rodovia_frame_agg["api"].contains(placa)){

                    bool request = (*apiQueue).push(placa);
                    if (request){

                        json search_result = (*apiQueue).search();
                        rodovia_frame_agg["api"][placa] = search_result;

                    }
                }
                
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
                    
                    std::string placa = carro.key();
                    double pos = carro.value()[1];
                    rodovia_frame_agg[rodovia][frame][placa]["Batido"] = false;
                    double pos_prevista = rodovia_frame_agg[rodovia][frame][placa]["Posição Prevista"];
                    
                    for(auto carro2 = inner_hash[  frame ].begin(); carro2 != inner_hash[  frame ].end(); ++carro2){
                        if (carro.key() == carro2.key()){ continue; }
                        //checa se após o dado número de frames um carro ultrapassará o outro (assim, colidindo)
                            if( carro.value()[0] == carro2.value()[0]){
                                    double pos2 = carro2.value()[1];
                                    double pos_prevista = rodovia_frame_agg[rodovia][frame][placa]["Posição Prevista"];
                                    std::string placa2 = carro2.key();
                                    double pos_prevista2 = rodovia_frame_agg[rodovia][frame][placa2]["Posição Prevista"];
                                    if ( (pos > pos2 and pos_prevista < pos_prevista2) 
                                    
                                        or (pos < pos2 and pos_prevista > pos_prevista2)

                                        or (std::abs(pos-pos2) <= (*parametros)[rodovia]["tamanhoCarro"])

                                        or (std::abs(pos_prevista-pos_prevista2) <= (*parametros)[rodovia]["tamanhoCarro"])   ){

                                        (rodovia_frame_agg[rodovia][frame][placa]["Risco Colisão"]).push_back(placa2);
                                        if (rodovia_frame_agg[rodovia][frame][placa].contains("Velocidade")
                                            and 
                                            rodovia_frame_agg[rodovia][frame][placa]["Velocidade"] == 0)
                                            {
                                                rodovia_frame_agg[rodovia][frame][placa]["Batido"] = true;
                                            }
                                }
                            }
                    }

                    std::string str_start = (*batches_adm)[rodovia][getBatch(rodovia, frame, batches_adm)]["start"];
                    // Parse the string into a std::tm structure
                    std::tm tm = {};
                    std::istringstream iss(str_start);
                    iss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S.");

                    // Extract the milliseconds value from the stream
                    int microseconds;
                    iss >> microseconds;

                    // Convert the std::tm structure to a std::chrono::time_point
                    auto start = std::chrono::system_clock::from_time_t(std::mktime(&tm)) + std::chrono::microseconds(microseconds);
                    std::time_t time_t_now = std::chrono::system_clock::to_time_t(start);
                    
                    
                    auto end = std::chrono::high_resolution_clock::now();
                    time_t_now = std::chrono::system_clock::to_time_t(end);

                    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
                    rodovia_frame_agg[rodovia][frame][placa]["Tempo de Execução"] = duration.count();



            }

			ultimo_frame = frame_num;
            ultimo_index = frame_index;
            }

        if( !waiting){
        std::string filename_output = std::string("./output/") + rodovia + std::string("_output_") + std::to_string(contador) + std::string(".json");

        std::ofstream file(filename_output);

        file << rodovia_frame_agg;

        file.close();

        std::cout << "arquivo " << filename_output <<  " salvo" << std::endl;

        contador++;

        rodovia_frame_agg[rodovia] = json {};

        rodovia_frame_agg["api"] = json {};
        }
		sleep(0.75);
        
    }   
}
