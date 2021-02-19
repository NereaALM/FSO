#!/bin/bash

# Trobar la temperatura màxima d’un fitxer “.tem”
# qualsevol, utilitzant “sort -n”

# Parametre d'entrada: nom del fitxer
# Format: Entre cometes dobles si el nom te espais

# Format del fitxer.tem:
# 1998	min	mig	max
# gen	-4.2	6.3	15.3
# feb	-2.2	7.1	19.6
# mar	-3.5	10.2	22.7

sort -k4 -n "$1" | tail -1 | cut -d'	' -f4
