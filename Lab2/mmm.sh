#!/bin/bash

# Feu un script que donat un fitxer de temperatures
# ‘.tem’, calculi la temperatura mínima, mitja i màxima
# de tot l’any (./mmm poblacio.tem)

# Format del fitxer.tem:
# 1999	min	mig	max
# feb	2.6	11.1	21.8
# mar	5.6	13.1	21.4
# abr	6.0	15.0	23.3

# Mínim:
echo -e "Mínim:\t$(sort -k2 -n "$1" | head -2 | tail -1 |  cut -f2 -d"	")"

# Mig:
nlines=$(wc -l "$1" | cut -d' ' -f1)
let mig=nlines/2+1
echo -e "Mig:\t$(sort -k3 -n "$1" | head -"$mig" | tail -1 |  cut -f3 -d"	")"

# Màxim:
echo -e "Màxim:\t$(sort -k4 -n "$1" | tail -1 |  cut -f4 -d"	")"
