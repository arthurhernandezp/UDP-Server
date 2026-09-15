#include "Server.hpp"

#include <cstring>
#include <iostream>
#include <stdexcept>

ServidorUdp::ServidorUdp(int porta) : _porta(porta), _sock(-1)
{
    _createSocket();
    _bind();
    _mostrarInfo();
}

ServidorUdp::~ServidorUdp()
{
    if (_sock >= 0) 
    {
        close(_sock);
    }
}

void ServidorUdp::processarUmaMensagem()
{
    char buffer[2048];
    sockaddr_in cli_addr{};
    socklen_t calen = sizeof(cli_addr);

    std::cout << "Aguardando datagrama JSON...\n";

    ssize_t bytes = recvfrom(_sock, buffer, sizeof(buffer) - 1, 0,
                             (struct sockaddr*)(&cli_addr), &calen);
    if (bytes < 0) 
    {
        throw std::runtime_error("Erro no recvfrom");
    }

    buffer[bytes] = '\0';
    std::string json_recebido(buffer);

    std::cout << "Recebido de " << inet_ntoa(cli_addr.sin_addr) << ":"
              << ntohs(cli_addr.sin_port) << "\n";
    std::cout << "JSON de input: " << json_recebido << "\n";

    Mensagem msg(json_recebido);
    msg.imprimir();
    _processador.processar(msg);

    std::string resposta = msg.toJson();
    std::cout << "Resposta JSON: " << resposta << "\n";

    if (sendto(_sock, resposta.c_str(), resposta.size(), 0,
               (struct sockaddr*)(&cli_addr), calen) < 0) {
        throw std::runtime_error("Erro no sendto");
    }

    std::cout << "Resposta enviada com sucesso.\n";
}

void ServidorUdp::loop()
{
    std::cout << "Iniciando loop " << std::endl;

    while (true) {
        try {
            std::cout << std::endl;
            processarUmaMensagem();
            std::cout << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Erro: " << e.what() << "\n";
        }
    }
}

void ServidorUdp::_createSocket()
{
    _sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (_sock < 0) 
    {
       throw std::runtime_error("Erro ao criar socket");
    }
}

void ServidorUdp::_bind()
{
    std::memset(&srv_addr_, 0, sizeof(srv_addr_));             /*clear sockaddr structure*/
    srv_addr_.sin_family = AF_INET;                            /* familia Internet */
    srv_addr_.sin_addr.s_addr = INADDR_ANY;                    /* se liga ao IP local */
    srv_addr_.sin_port = htons(static_cast<uint16_t>(_porta)); /*acerta byte order para a rede */

    if (bind(_sock, (struct sockaddr*)(&srv_addr_), sizeof(srv_addr_)) < 0) {
        close(_sock);
        _sock = -1;
        throw std::runtime_error("Erro no bind");
    }
}

void ServidorUdp::_mostrarInfo()
{
    sockaddr_in addr{};
    socklen_t len = sizeof(addr);
    getsockname(_sock, (struct sockaddr*)(&addr), &len);

    std::cout << "Servidor iniciado na porta " << ntohs(addr.sin_port) << "\n\n";
}
