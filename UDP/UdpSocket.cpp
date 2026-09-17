#include "UdpSocket.hpp"

#include <stdexcept>
#include <unistd.h>

UdpSocket::UdpSocket()
{
    _fd = socket(AF_INET, SOCK_DGRAM, 0);

    if (_fd < 0) 
    {
        throw std::runtime_error("Erro ao criar socket");
    }
}

void UdpSocket::bindSocket(std::uint16_t porta)
{
    sockaddr_in srv_addr{};

    srv_addr.sin_family = AF_INET;

    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY); 

    srv_addr.sin_port = htons(porta);

    if (bind(_fd, (struct sockaddr*)(&srv_addr), sizeof(srv_addr)) < 0) 
    {
        throw std::runtime_error("Erro no bind");
    }

    socklen_t srv_addr_size = sizeof(srv_addr);

    if (getsockname(_fd, (struct sockaddr*)(&srv_addr), &srv_addr_size) < 0) 
    {
        throw std::runtime_error("Erro no getsockname");
    }

    _port = ntohs(srv_addr.sin_port);
}

UdpSocket::~UdpSocket()
{
    if (_fd >= 0) 
    {
        close(_fd);
    }
}

ssize_t UdpSocket::receive(char* buffer, std::size_t tamanho,
                           sockaddr_in& cliente, socklen_t& tamanho_cliente) const
{
    const ssize_t bytes = recvfrom(_fd, buffer, tamanho, 0,(struct sockaddr*)(&cliente), &tamanho_cliente);
    
    if (bytes < 0) 
    {
        throw std::runtime_error("Erro no recvfrom");
    }

    return bytes;
}

void UdpSocket::send(const std::string& mensagem, const sockaddr_in& cliente,
                     socklen_t tamanho_cliente) const
{
    const ssize_t bytes = sendto(_fd, mensagem.data(), mensagem.size(), 0,(struct sockaddr*)(&cliente), tamanho_cliente);
    
    if (bytes < 0) 
    {
        throw std::runtime_error("Erro no sendto");
    }
}

std::uint16_t UdpSocket::port() const
{
    return _port;
}
