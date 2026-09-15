#include "MessageParser.hpp"

#include <iostream>
#include <stdexcept>

Mensagem::Mensagem(const std::string& json_str)
{
    const nlohmann::json json = nlohmann::json::parse(json_str);

    if (!json.is_object()) {
        throw std::runtime_error("A mensagem JSON deve ser um objeto");
    }

    if (!json.contains("tipo") || !json.at("tipo").is_string()) {
        throw std::runtime_error("A mensagem deve conter o campo string 'tipo'");
    }

    if (json.contains("valor")) 
    {
        valor = json.at("valor");
    } 
    else if (json.contains("val")) 
    {
        valor = json.at("val");
    } 
    else 
    {
        throw std::runtime_error("A mensagem deve conter o campo 'valor'");
    }

    tipo = json.at("tipo").get<std::string>();

    if (tipo == "int" && !valor.is_number_integer()) 
    {
        throw std::runtime_error("O campo 'valor' deve ser inteiro para tipo 'int'");
    }
    if (tipo == "char" && (!valor.is_string() || valor.get<std::string>().size() != 1)) 
    {
        throw std::runtime_error("O campo 'valor' deve conter um único caractere");
    }
    if (tipo == "string" && !valor.is_string()) 
    {
        throw std::runtime_error("O campo 'valor' deve ser string para tipo 'string'");
    }
}

std::string Mensagem::toJson() const
{
    return nlohmann::json{{"tipo", tipo}, {"val", valor}}.dump();
}

void Mensagem::imprimir() const
{
    std::cout << "  tipo: \"" << tipo << "\" | valor: " << valor.dump() << "\n";
}

void ProcessadorMensagem::processar(Mensagem& msg)
{
    if (msg.tipo == "int") 
    {
        soma(msg);
    } 
    else if (msg.tipo == "char") 
    {
        inverterCaixa(msg);
    } 
    else if (msg.tipo == "string") 
    {
        inverterString(msg);
    } 
    else 
    {
        throw std::runtime_error("Tipo desconhecido: " + msg.tipo);
    }
}

void ProcessadorMensagem::soma(Mensagem& msg)
{
    long long valor = msg.valor.get<long long>();

    valor++;

    msg.valor = valor;
}

void ProcessadorMensagem::inverterCaixa(Mensagem& msg)
{
    std::string s;

    s = msg.valor.get<std::string>();


    if (s.size() != 1) {
        std::cout << "deve ter exatamente 1 caractere, size recebido :  "<< s.size();
    }

    char c = s[0];

    if (std::islower(static_cast<unsigned char>(c))) 
    {
        c = static_cast<char>(std::toupper(c));
    } 
    else if (std::isupper(static_cast<unsigned char>(c))) 
    {
        c = static_cast<char>(std::tolower(c));
    }

    msg.valor = std::string(1, c);
}

void ProcessadorMensagem::inverterString(Mensagem& msg)
{
    std::string s = msg.valor.get<std::string>();

    std::reverse(s.begin(), s.end());

    msg.valor = s;
}
