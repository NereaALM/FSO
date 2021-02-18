#!/bin/bash

# Dins el directori “Dt” (DadesTemperatura), ajuntar
# tots els fitxers d’una població determinada en un sol
# fitxer

# Parametre d'entrada: nom de població

find ./DadesTemperatura -name "$1.tem" > directoris

while IFS= read -r line
do
	echo "$line"
	
	$dir = 
	$file = 
	
	cd $dir
	cat $file >> ../../
	
	cd ../..
done < directoris

rm directoris
