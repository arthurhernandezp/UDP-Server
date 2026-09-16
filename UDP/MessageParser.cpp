#include "MessageParser.hpp"

#include <algorithm>
#include <cctype>
#include <stdexcept>

Mensagem::Mensagem(std::string tipo, nlohmann::json valor)
    : _tipo(std::move(tipo)), _valor(std::move(valor))
{
    if (_tipo == "int" && !_valor.is_number_integer())
    {
        throw std::runtime_error("O campo 'valor' deve ser inteiro para tipo 'int'");
    }
    if (_tipo == "char" && (!_valor.is_string() || _valor.get<std::string>().size() != 1))
    {
        throw std::runtime_error("O campo 'valor' deve conter um único caractere");
    }
    if (_tipo == "string" && !_valor.is_string())
    {
        throw std::runtime_error("O campo 'valor' deve ser string para tipo 'string'");
    }
}

const std::string& Mensagem::tipo() const noexcept
{
    return _tipo;
}

const nlohmann::json& Mensagem::valor() const noexcept
{
    return _valor;
}

Mensagem MessageCodec::decode(const std::string& json_str)
{
    const nlohmann::json json = nlohmann::json::parse(json_str);
    if (!json.is_object())
    {
        throw std::runtime_error("A mensagem JSON deve ser um objeto");
    }

    if (!json.contains("tipo") || !json.at("tipo").is_string())
        throw std::runtime_error("A mensagem deve conter o campo string 'tipo'");

    if (!json.contains("valor") && !json.contains("val"))
        throw std::runtime_error("A mensagem deve conter o campo 'valor'");

    const auto& valor = json.contains("valor") ? json.at("valor") : json.at("val");
    return Mensagem(json.at("tipo").get<std::string>(), valor);
}

std::string MessageCodec::encode(const Mensagem& mensagem)
{
    return nlohmann::json{{"tipo", mensagem.tipo()}, {"val", mensagem.valor()}}.dump();
}

Mensagem ProcessadorMensagem::processar(const Mensagem& msg) const
{
    if (msg.tipo() == "int")
        return Mensagem(msg.tipo(), soma(msg.valor()));
    if (msg.tipo() == "char")
        return Mensagem(msg.tipo(), inverterCaixa(msg.valor()));
    if (msg.tipo() == "string")
        return Mensagem(msg.tipo(), inverterString(msg.valor()));

    throw std::runtime_error("Tipo desconhecido: " + msg.tipo());
}

 nlohmann::json ProcessadorMensagem::soma(const nlohmann::json& valor) const
{
    return valor.get<long long>() + 1;
}

nlohmann::json ProcessadorMensagem::inverterCaixa(const nlohmann::json& valor) const
{
    std::string s = valor.get<std::string>();

    char c = s[0];

    if (std::islower(static_cast<unsigned char>(c))) 
    {
        c = static_cast<char>(std::toupper(c));
    } 
    else if (std::isupper(static_cast<unsigned char>(c))) 
    {
        c = static_cast<char>(std::tolower(c));
    }

    return std::string(1, c);
}

nlohmann::json ProcessadorMensagem::inverterString(const nlohmann::json& valor) const
{
    std::string s = valor.get<std::string>();

    std::reverse(s.begin(), s.end());

    return s;
}
