#include "Server.hpp"

#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>

ServidorUdp::ServidorUdp(int porta) : _socket()
{
    if (porta < 1 || porta > std::numeric_limits<std::uint16_t>::max()) {
        throw std::invalid_argument("a porta deve estar entre 1 e 65535");
    }

    _socket.bindSocket(static_cast<std::uint16_t>(porta));
    
    _mostrarInfo();
}

void ServidorUdp::processarUmaMensagem()
{
    char buffer[2048];

    sockaddr_in cli_addr{};
    socklen_t calen = sizeof(cli_addr);

    std::cout << "Aguardando datagrama JSON...\n";

    ssize_t bytes = _socket.receive(buffer, sizeof(buffer) - 1, cli_addr, calen);

    buffer[bytes] = '\0';
    std::string json_recebido(buffer);

    std::cout << "Recebido de " << inet_ntoa(cli_addr.sin_addr) << ":" << ntohs(cli_addr.sin_port) << "\n";
    std::cout << "JSON de input: " << json_recebido << "\n";

    Mensagem msg = MessageCodec::decode(json_recebido);

    Mensagem resposta_msg = _processador.processar(msg);

    std::string resposta = MessageCodec::encode(resposta_msg);
    
    std::cout << "Resposta JSON: " << resposta << "\n";

    _socket.send(resposta, cli_addr, calen);

    std::cout << "Resposta enviada com sucesso.\n";
}

void ServidorUdp::loop()
{
    std::cout << "Iniciando loop " << std::endl;

    while (true) 
    {
        try 
        {
            std::cout << std::endl;
            processarUmaMensagem();
            std::cout << std::endl;
        } 
        catch (const std::exception& e) 
        {
            std::cerr << "Erro: " << e.what() << "\n";
        }
    }
}

void ServidorUdp::_mostrarInfo()
{
    std::cout << "Servidor iniciado na porta " << _socket.port() << "\n\n";
}
