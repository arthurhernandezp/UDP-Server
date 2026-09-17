#pragma once

#include <iostream>
#include <arpa/inet.h>

#include "MessageParser.hpp"
#include "UdpSocket.hpp"

class ServidorUdp
{
public:
    ServidorUdp(int porta);

    void processarDatagrama();

    void loop();

private:
    UdpSocket _socket;

    ProcessadorMensagem _processador;
};