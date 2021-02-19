#!/bin/bash

# Dins el directori “Dt” (DadesTemperatura), ajuntar
# tots els fitxers d’una població determinada en un sol
# fitxer

# Paràmetre d'entrada: nom de població 
# Format del paràmetre: entre cometes dobles o amb barres invertides als espais

find ./DadesTemperatura -name "$1.tem" > directoris

while IFS= read -r line
do 
	cat "$line" >> "$1.tem"
	
done < directoris

rm directoris
