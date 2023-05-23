/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the L icense.
 *
 */

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <unistd.h>
#include <filesystem>
#include <mutex>
#include "API/API.hpp"
#include <nlohmann/json.hpp>

// #include "helper.h"

#include <grpc/grpc.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <grpcpp/server_context.h>
#ifdef BAZEL_BUILD
#include "examples/protos/route_guide.grpc.pb.h"
#else
#include "route_guide.grpc.pb.h"
#endif
#include <queue>
#include <iostream>
#include <memory>
#include <string>
// #include "functions.hpp"
#include "func.hpp"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using hello::HelloRequest;
using hello::HelloResponse;
using hello::HelloService;
using json = nlohmann::json;

// how to acess queue
// my_queue.front();
// my_queue.pop();
std::queue<std::string> my_queue;
void trat_json(string j){
  my_queue.push(j);
}


class HelloServiceImpl final : public HelloService::Service {
  Status SayHello(ServerContext* context, const HelloRequest* request,
                  HelloResponse* reply) override {
    std::string j = request->name();
    trat_json(j);
    //std::cout << "Recebido: " << j << std::endl; // imprime a string recebida no console
    reply->set_message("Olá " + j);
    return Status::OK;
  }
};

void RunServer() {
  std::string server_address("0.0.0.0:50051");
  HelloServiceImpl service;

  ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&service);
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Servidor escutando em " << server_address << std::endl;
  server->Wait();
}

int main(int argc, char** argv) {

    int NUM_THREADS = 0;

     //Lista dos nomes das rodovias
    std::vector<std::string> rodovias;

    APIQueue apiQueue(10);

    //Carregando dados de parametros
	json parametros;
  json batches_adm;
  std::string parameters_json = std::string("../../mock/parametros.json");
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

    std::mutex thread_mutex;
  
    for (int l = 0; l < NUM_THREADS; l++) {
              threads_files[l] = std::thread(read_aggregate, &hash_agg, &frames_indexes, rodovias, l, &mutexes, &batches_adm, &thread_mutex, &my_queue);
    }
    //  std::mutex * thread_mutex, std::queue<string> * my_queue


    for (int l = 0; l < NUM_THREADS; l++) {
              threads_calculations[l] = std::thread(calcula_carros, &hash_agg, &frames_indexes,
                                                            rodovias, l, &mutexes, &parametros, &batches_adm, &apiQueue);
    }

  //std::thread server_listening;
  //server_listening = std::thread(RunServer);

  RunServer();

//  while (true){
//      continue;
//  }

  return 0;
}

