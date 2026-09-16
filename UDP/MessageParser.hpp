#pragma once

#include <nlohmann/json.hpp>

#include <string>

class Mensagem
{
public:
    Mensagem(std::string tipo, nlohmann::json valor);

    const std::string& tipo() const noexcept;
    const nlohmann::json& valor() const noexcept;

private:
    std::string _tipo;
    nlohmann::json _valor;
};

class MessageCodec
{
public:
    static Mensagem decode(const std::string& json_str);
    static std::string encode(const Mensagem& mensagem);
};

class ProcessadorMensagem
{
public:
    Mensagem processar(const Mensagem& msg) const;

private:
    nlohmann::json soma(const nlohmann::json& valor) const;
    nlohmann::json inverterCaixa(const nlohmann::json& valor) const;
    nlohmann::json inverterString(const nlohmann::json& valor) const;
};