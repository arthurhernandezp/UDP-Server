#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include "UDP/Server.hpp"

int main(int argc, char ** argv)
{
    if(argc < 2)
    {
        std::cout << "Uso: " << argv[0] << " <porta>" << std::endl;
        return 1;
    }
    try {
        int porta = std::stoi(argv[1]);
        ServidorUdp servidor(porta);
        servidor.loop();

    } catch (const std::exception& e) {
        std::cerr << "Erro: " << e.what() << '\n';
        return 1;
    }

    return 0;
}