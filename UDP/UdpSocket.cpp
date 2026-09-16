#include "UdpSocket.hpp"

#include <cerrno>
#include <limits>
#include <stdexcept>
#include <system_error>
#include <unistd.h>

UdpSocket::UdpSocket(int porta)
{
    if (porta < 1 || porta > std::numeric_limits<std::uint16_t>::max()) {
        throw std::invalid_argument("a porta deve estar entre 1 e 65535");
    }

    _fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (_fd < 0) {
        throw std::system_error(errno, std::generic_category(), "Erro ao criar socket");
    }

    sockaddr_in endereco{};
    endereco.sin_family = AF_INET;
    endereco.sin_addr.s_addr = htonl(INADDR_ANY);
    endereco.sin_port = htons(static_cast<std::uint16_t>(porta));

    if (bind(_fd, reinterpret_cast<const sockaddr*>(&endereco), sizeof(endereco)) < 0) {
        const std::error_code erro(errno, std::generic_category());
        close(_fd);
        _fd = -1;
        throw std::system_error(erro, "Erro no bind");
    }

    sockaddr_in endereco_real{};
    socklen_t tamanho = sizeof(endereco_real);
    if (getsockname(_fd, reinterpret_cast<sockaddr*>(&endereco_real), &tamanho) < 0) {
        const std::error_code erro(errno, std::generic_category());
        close(_fd);
        _fd = -1;
        throw std::system_error(erro, "Erro no getsockname");
    }

    _port = ntohs(endereco_real.sin_port);
}

UdpSocket::~UdpSocket()
{
    if (_fd >= 0) {
        close(_fd);
    }
}

UdpSocket::UdpSocket(UdpSocket&& outro) noexcept
    : _fd(outro._fd), _port(outro._port)
{
    outro._fd = -1;
    outro._port = 0;
}

UdpSocket& UdpSocket::operator=(UdpSocket&& outro) noexcept
{
    if (this == &outro) {
        return *this;
    }

    if (_fd >= 0) {
        close(_fd);
    }

    _fd = outro._fd;
    _port = outro._port;
    outro._fd = -1;
    outro._port = 0;
    return *this;
}

ssize_t UdpSocket::receive(char* buffer, std::size_t tamanho,
                           sockaddr_in& cliente, socklen_t& tamanho_cliente) const
{
    const ssize_t bytes = recvfrom(_fd, buffer, tamanho, 0,
                                   reinterpret_cast<sockaddr*>(&cliente), &tamanho_cliente);
    if (bytes < 0) {
        throw std::system_error(errno, std::generic_category(), "Erro no recvfrom");
    }

    return bytes;
}

void UdpSocket::send(const std::string& mensagem, const sockaddr_in& cliente,
                     socklen_t tamanho_cliente) const
{
    const ssize_t bytes = sendto(_fd, mensagem.data(), mensagem.size(), 0,
                                 reinterpret_cast<const sockaddr*>(&cliente), tamanho_cliente);
    if (bytes < 0) {
        throw std::system_error(errno, std::generic_category(), "Erro no sendto");
    }
}

std::uint16_t UdpSocket::port() const noexcept
{
    return _port;
}
