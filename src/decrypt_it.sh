#!/usr/bin/env bash
# Uso: ./decrypt_it.sh <rot> "<frase>"
rot="$1"; shift
text="$*"

# Desencriptar = rotación negativa
./encrypt_it.sh $(( (26 - (rot % 26)) % 26 )) "$text"
