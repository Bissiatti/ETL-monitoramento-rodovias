#include <iostream>
#include <string>
#include <sstream>
#include <queue>

using namespace std;

struct Veicle
{
    string plate_code;
    string name;
    string year;
    string model;
};

class API 
{
    public:
        API(){

        }
        ~API(){
            
        }

        void query_veicle(string plate_code){
            std::string command = "python3 API.py " + plate_code;
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

    Veicle search(){
        Veicle result;
        while (!this->queue.empty()){
            string plate_code = this->queue.front();
            this->queue.pop();
            this->lenQueue--;
            API api;
            api.query_veicle(plate_code);
            string name = api.get_name();
            string year = api.get_year();
            string model = api.get_model();

            result.plate_code = plate_code;
            result.name = name;
            result.year = year;
            result.model = model;

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


int main(int argc, char const *argv[])
{
    APIQueue apiQueue(10);
    apiQueue.push("SUR2K34");
    apiQueue.push("COL3H45");

    Veicle veicle = apiQueue.search();

    cout << veicle.plate_code << endl;
    cout << veicle.name << endl;
    cout << veicle.year << endl;
    cout << veicle.model << endl;
}