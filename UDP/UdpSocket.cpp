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
    sockaddr_in endereco{};

    endereco.sin_family = AF_INET;

    endereco.sin_addr.s_addr = htonl(INADDR_ANY);

    endereco.sin_port = htons(porta);

    if (bind(_fd, (struct sockaddr*)(&endereco), sizeof(endereco)) < 0) 
    {
        throw std::runtime_error("Erro no bind");
    }

    socklen_t tamanho = sizeof(endereco);

    if (getsockname(_fd, reinterpret_cast<sockaddr*>(&endereco), &tamanho) < 0) 
    {
        throw std::runtime_error("Erro no getsockname");
    }

    _port = ntohs(endereco.sin_port);
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
    const ssize_t bytes = recvfrom(_fd, buffer, tamanho, 0,reinterpret_cast<sockaddr*>(&cliente), &tamanho_cliente);
    
    if (bytes < 0) 
    {
        throw std::runtime_error("Erro no recvfrom");
    }

    return bytes;
}

void UdpSocket::send(const std::string& mensagem, const sockaddr_in& cliente,
                     socklen_t tamanho_cliente) const
{
    const ssize_t bytes = sendto(_fd, mensagem.data(), mensagem.size(), 0,reinterpret_cast<const sockaddr*>(&cliente), tamanho_cliente);
    
    if (bytes < 0) 
    {
        throw std::runtime_error("Erro no sendto");
    }
}

std::uint16_t UdpSocket::port() const noexcept
{
    return _port;
}
