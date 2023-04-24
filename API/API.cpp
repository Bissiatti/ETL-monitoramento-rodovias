#include <iostream>
#include <string>
#include <sstream>
#include <queue>
#include <nlohmann/json.hpp>


using namespace std;

class API 
{
    public:
        API(){

        }
        ~API(){
            
        }

        void query_veicle(string plate_code){
            std::string command = "python3 api/API.py " + plate_code;
            FILE* stream = popen(command.c_str(), "r");
            char output[256];
            fgets(output, sizeof(output), stream);
            pclose(stream);
            std::stringstream ss(output);
            std::string name, year, model;
            ss >> name >> year >> model;
            
            this->name = name;
            this->year = year;
            this->model = model;

        }

        string get_name(){
            return this->name;
        }

        string get_year(){
            return this->year;
        }

        string get_model(){
            return this->model;
        }

    private:
        string name;
        string year;
        string model;  
};

class APIQueue
{
private:
    std::queue<string> queue;
    int maxLenQueue = maxLenQueue;
    int lenQueue = 0;
public:
    APIQueue(int lenQueue);
    ~APIQueue();

    void push(string plate_code){
        if (this->lenQueue < this->maxLenQueue){
            this->queue.push(plate_code);
            this->lenQueue++;
        }
    }

    nlohmann::json search(){
        nlohmann::json result = nlohmann::json {};
        while (!this->queue.empty()){
            string plate_code = this->queue.front();
            this->queue.pop();
            this->lenQueue--;
            API api;
            api.query_veicle(plate_code);
            string name = api.get_name();
            string year = api.get_year();
            string model = api.get_model();

            result["placa"] = plate_code;
            result["nome"] = name;
            result["ano"] = year;
            result["modelo"] = model;

        }
        return result;
    }
    
};

APIQueue::APIQueue(int maxLenQueue)
{
    this->maxLenQueue = maxLenQueue;
    this->lenQueue = 0;
}

APIQueue::~APIQueue()
{

};
