#include <iostream>
#include <string>
#include <sstream>

int main() {
    std::string plate_code = "URU0A12";
    std::string command = "python3 API.py " + plate_code;
    FILE* stream = popen(command.c_str(), "r");
    char output[256];
    fgets(output, sizeof(output), stream);
    pclose(stream);
    std::stringstream ss(output);
    std::string name, year, model;
    ss >> name >> year >> model;
    std::cout << "Name: " << name << ", Year: " << year << ", Model: " << model << std::endl;
    return 0;
}