// /*
//  *
//  * Copyright 2015 gRPC authors.
//  *
//  * Licensed under the Apache License, Version 2.0 (the "License");
//  * you may not use this file except in compliance with the License.
//  * You may obtain a copy of the License at
//  *
//  *     http://www.apache.org/licenses/LICENSE-2.0
//  *
//  * Unless required by applicable law or agreed to in writing, software
//  * distributed under the License is distributed on an "AS IS" BASIS,
//  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  * See the License for the specific language governing permissions and
//  * limitations under the License.
//  *
//  */

// #include <algorithm>
// #include <chrono>
// #include <cmath>
// #include <iostream>
// #include <memory>
// #include <string>

// #include "helper.h"

// #include <grpc/grpc.h>
// #include <grpcpp/security/server_credentials.h>
// #include <grpcpp/server.h>
// #include <grpcpp/server_builder.h>
// #include <grpcpp/server_context.h>
// #ifdef BAZEL_BUILD
// #include "examples/protos/route_guide.grpc.pb.h"
// #else
// #include "route_guide.grpc.pb.h"
// #endif

// #include <iostream>
// #include <memory>
// #include <string>

// // #include <grpcpp/grpcpp.h>

// // #include "routeguide.grpc.pb.h"

// using grpc::Server;
// using grpc::ServerBuilder;
// using grpc::ServerContext;
// using grpc::Status;
// using routeguide::Greeter;
// using routeguide::HelloReply;
// using routeguide::HelloRequest;

// // Classe que implementa o serviço Greeter definido no proto
// class GreeterServiceImpl final : public Greeter::Service {
//   Status SayHello(ServerContext* context, const HelloRequest* request,
//                   HelloReply* reply) override {
//     std::string prefix("Olá ");
//     reply->set_message(prefix + request->name());
//     return Status::OK;
//   }
// };

// void RunServer() {
//   std::string server_address("0.0.0.0:50051");
//   GreeterServiceImpl service;

//   ServerBuilder builder;
//   // Escuta no endereço e porta especificados
//   builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
//   // Registra o serviço implementado
//   builder.RegisterService(&service);
//   // Cria e inicia o servidor
//   std::unique_ptr<Server> server(builder.BuildAndStart());
//   std::cout << "Servidor escutando em " << server_address << std::endl;

//   // Espera até que o servidor seja encerrado
//   server->Wait();
// }

// int main(int argc, char** argv) {
//   RunServer();

//   return 0;
// }


int main(int argc, char** argv) {
  std::cout << "Hello World!" << std::endl;
  return 0;
}