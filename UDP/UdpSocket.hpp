#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>

class UdpSocket
{
public:
    UdpSocket();
    
    ~UdpSocket();

    void bindSocket(std::uint16_t porta);

    ssize_t receive(char* buffer, std::size_t tamanho,sockaddr_in& cliente, socklen_t& tamanho_cliente) const;
    
    void send(const std::string& mensagem, const sockaddr_in& cliente, socklen_t tamanho_cliente) const;
    
    std::uint16_t port() const;

private:
    int _fd{-1};
    std::uint16_t _port{0};
};
