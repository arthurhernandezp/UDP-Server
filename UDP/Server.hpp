#pragma once

#include <iostream>
#include <arpa/inet.h>
#include "MessageParser.hpp"
#include "UdpSocket.hpp"

class ServidorUdp
{
public:
    ServidorUdp(int porta);

    void processarUmaMensagem();
    void loop();

private:
    void _mostrarInfo();

private:
    UdpSocket _socket;
    ProcessadorMensagem _processador;
};