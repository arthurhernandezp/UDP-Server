#!/usr/bin/env python3
"""Valida o servidor UDP usando casos de protocolo reais."""

import argparse
import json
import socket
import subprocess
import sys
import time
from pathlib import Path


class FalhaValidacao(Exception):
    pass


def porta_livre():
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.bind(("127.0.0.1", 0))
        return sock.getsockname()[1]


def enviar(sock, porta, mensagem, esperar_resposta=True):
    payload = json.dumps(mensagem).encode("utf-8")
    sock.sendto(payload, ("127.0.0.1", porta))

    if not esperar_resposta:
        return None

    try:
        resposta, _ = sock.recvfrom(2048)
    except socket.timeout as erro:
        raise FalhaValidacao(
            f"timeout aguardando resposta para {mensagem!r}"
        ) from erro

    try:
        return json.loads(resposta.decode("utf-8"))
    except (UnicodeDecodeError, json.JSONDecodeError) as erro:
        raise FalhaValidacao(f"resposta invalida: {resposta!r}") from erro


def esperar_servidor(sock, porta):
    limite = time.monotonic() + 3
    ultima_falha = None

    while time.monotonic() < limite:
        try:
            resposta = enviar(sock, porta, {"tipo": "int", "val": 0})
            if resposta == {"tipo": "int", "val": 1}:
                return
        except (OSError, FalhaValidacao) as erro:
            ultima_falha = erro
        time.sleep(0.05)

    raise FalhaValidacao(f"servidor nao respondeu: {ultima_falha}")


def validar(porta):
    casos = [
        ({"tipo": "int", "val": 41}, {"tipo": "int", "val": 42}),
        ({"tipo": "char", "val": "A"}, {"tipo": "char", "val": "a"}),
        ({"tipo": "string", "valor": "udp"}, {"tipo": "string", "val": "pdu"}),
    ]

    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.settimeout(0.8)
        esperar_servidor(sock, porta)

        for entrada, esperado in casos:
            resposta = enviar(sock, porta, entrada)
            if resposta != esperado:
                raise FalhaValidacao(
                    f"para {entrada!r}, esperado {esperado!r}, recebido {resposta!r}"
                )
            print(f"OK: {entrada} -> {resposta}")

        enviar(sock, porta, {"tipo": "int", "val": "invalido"}, False)
        try:
            sock.recvfrom(2048)
        except socket.timeout:
            print("OK: mensagem invalida foi rejeitada")
        else:
            raise FalhaValidacao("mensagem invalida recebeu uma resposta")

        resposta = enviar(sock, porta, {"tipo": "int", "val": 9})
        if resposta != {"tipo": "int", "val": 10}:
            raise FalhaValidacao(
                f"servidor nao continuou ativo apos erro: {resposta!r}"
            )
        print("OK: servidor continuou ativo apos erro")


def iniciar_servidor(caminho, porta):
    return subprocess.Popen(
        [str(caminho), str(porta)],
        stdout=subprocess.DEVNULL,
        stderr=subprocess.DEVNULL,
    )


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--server",
        type=Path,
        default=Path("build/server"),
        help="caminho do executavel (padrao: build/server)",
    )
    parser.add_argument(
        "--port", type=int, help="porta do servidor ja em execucao"
    )
    parser.add_argument(
        "--no-start",
        action="store_true",
        help="nao inicia o executavel; valida um servidor existente",
    )
    args = parser.parse_args()

    if args.no_start and args.port is None:
        parser.error("--no-start exige --port")

    processo = None
    porta = args.port or porta_livre()

    try:
        if not args.no_start:
            if not args.server.is_file():
                raise FalhaValidacao(f"executavel nao encontrado: {args.server}")
            processo = iniciar_servidor(args.server, porta)

        validar(porta)
        print(f"Validacao concluida com sucesso na porta {porta}.")
        return 0
    except (FalhaValidacao, OSError) as erro:
        print(f"FALHA: {erro}", file=sys.stderr)
        return 1
    finally:
        if processo is not None:
            processo.terminate()
            try:
                processo.wait(timeout=2)
            except subprocess.TimeoutExpired:
                processo.kill()
                processo.wait()


if __name__ == "__main__":
    raise SystemExit(main())
