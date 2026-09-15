#pragma once

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "MessageParser.hpp"
#include <cstring>
#include <stdexcept>
#include <string>

class ServidorUdp
{
public:
    ServidorUdp(int porta);
    ~ServidorUdp();

    void processarUmaMensagem();
    void loop();

private:
    int _porta; // porta a ser utilizada pelo servidor
    int _sock;

    sockaddr_in srv_addr_{};
    ProcessadorMensagem _processador;

    void _createSocket();
    void _bind();
    void _mostrarInfo();
};