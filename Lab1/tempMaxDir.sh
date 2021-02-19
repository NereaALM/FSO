#!/bin/bash

# Trobar la temperatura màxima de tots els fitxer “.tem”
# que hi hagin dins l’arbre de subdirectoris del directori
# actual de treball

find ./DadesTemperatura -name "*.tem" > directoris

while IFS= read -r line
do 
	cat "$line" >> "temperatures.tem"
	
done < directoris

./maxTemp.sh temperatures.tem

rm directoris temperatures.tem
