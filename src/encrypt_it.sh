#!/usr/bin/env bash
# Uso: ./encrypt_it.sh <rot> "<frase>"
rot="$1"; shift
text="$*"

caesar() {
  local txt="$1" r="$2" out="" c o base code
  for ((i=0; i<${#txt}; i++)); do
    c="${txt:$i:1}"
    code=$(printf "%d" "'$c")
    if [[ $code -ge 65 && $code -le 90 ]]; then
      base=65; o=$(( (code-base + r) % 26 + base ))
      out+=$(printf "\\$(printf '%03o' "$o")")
    elif [[ $code -ge 97 && $code -le 122 ]]; then
      base=97; o=$(( (code-base + r) % 26 + base ))
      out+=$(printf "\\$(printf '%03o' "$o")")
    else
      out+="$c"
    fi
  done
  printf "%s\n" "$out"
}

caesar "$text" "$rot"
