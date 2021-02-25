#!/bin/bash

# Feu un script que substitueixi els espais en blanc per
# un ‘_’ en els noms dels fitxers continguts dins d’un
# directori (./sblanc dir)

# El directori ha d'estar dins de l'actual

for file in $(ls "$1")
do
	echo $file
	cat "./$1/$file" | tr ' ' '_'
done
