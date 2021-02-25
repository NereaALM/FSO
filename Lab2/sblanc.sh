#!/bin/bash

# Feu un script que substitueixi els espais en blanc per
# un ‘_’ en els noms dels fitxers continguts dins d’un
# directori (./sblanc dir)

for file in $(ls "$1")
do
	echo $file
	cat "$file" | tr ' ' '_'
done
