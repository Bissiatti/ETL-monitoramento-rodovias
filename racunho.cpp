#include <iostream>
#include <fstream>

using namespace std;

int main()
{
    ifstream arquivo("exemplo.txt"); // abre o arquivo exemplo.txt para leitura
    
    if (arquivo.is_open()) // verifica se o arquivo foi aberto corretamente
    {
        string linha;
        while (getline(arquivo, linha)) // lê cada linha do arquivo
        {
            cout << linha << endl; // imprime a linha no console
        }
        
        arquivo.close(); // fecha o arquivo
    }
    else
    {
        cout << "Não foi possível abrir o arquivo" << endl;
    }
    
    return 0;
}
