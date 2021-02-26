#!/bin/bash

# Feu un script que compari els arxius continguts en
# dos directoris i, quan detecti el mateix nom d’arxiu
# als dos directoris, copiï la versió més nova (data
# d’actualització més recent) sobre la versió antiga 
#
# (./ sinc dir1 dir2)

ls "$1" > arxius_dir1

while IFS= read -r line
do 
	arxR=$(find "$2" -name "$line")
	
	if [ -f "$arxR" ]
	then
		date1=$(ls -l "$1$line" | tr -s ' ' | cut -f6-8 -d' ')
		date2=$(ls -l "$arxR" | tr -s ' ' | cut -f6-8 -d' ')
		
		date1=$(date -d "$date1" +"%Y%m%d")
		date2=$(date -d "$date2" +"%Y%m%d")
		
		if [ "$date1" -gt "$date2" ]
		then
			cp -f "$arxR" "$1$line" 
			echo "L'arxiu $arxR ha sigut actualitzat"
			
		elif [ "$date1" -lt "$date2" ]
		then
			cp -f "$1$line" "$arxR"
			echo "L'arxiu $1$line ha sigut actualitzat"
		fi
	fi

done < arxius_dir1

rm arxius_dir1
