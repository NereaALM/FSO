#!/bin/bash

# Feu un script que donat un fitxer de temperatures
# ‘.tem’, calculi la temperatura mínima, mitja i màxima
# de tot l’any (./mmm poblacio.tem)

# TO DO
# Agafar columna de minim endreçarla buscar primera pos
# Agafar columna de mig endreçarla i agafar mig pos
# Agafar columna de max endreçar i agafar ultim pos

# Ordenar numeros ascendentment

cat $1 | 

echo "$@" | tr " " "\n" | sort -g > fitxerAux
	
# Mínim
echo -e "Mínim:\t$(cat fitxerAux | head -1 fitxerAux)"

# Mig
let mig=$#/2+1
echo -e "Mig:\t$(cat fitxerAux | head -"$mig" fitxerAux | tail -1)"

# Màxim
echo -e "Màxim:\t$(cat fitxerAux | tail -1)"

rm fitxerAux



sort -k4 -n "nomFitxer" | tail -1 | cut -d'	' -f4
cat fa | cut -d' ' -f9 >fb 
