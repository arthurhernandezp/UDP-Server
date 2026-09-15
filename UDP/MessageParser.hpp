#pragma once

#include <nlohmann/json.hpp>

#include <string>

class Mensagem
{
public:
    std::string tipo;
    nlohmann::json valor;

    Mensagem(const std::string& json_str);

    Mensagem() = default;

    std::string toJson() const;

    void imprimir() const;
};

class ProcessadorMensagem 
{
public:
    void processar(Mensagem& msg);

private:
    void soma(Mensagem& msg);
    void inverterCaixa(Mensagem& msg);
    void inverterString(Mensagem& msg);
};