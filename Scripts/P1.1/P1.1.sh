#!/bin/bash

# Crida: ./P1.1.sh [lmin_name] [d_pass] 

# Paràmetres d'entrada:
#	longitud mínima de nom d'usuari (opcional)
#	número màxim de dies entre canvis de contrasenya (opcional)

# Comprovacions:
# 1. quins usuaris tenen un nom massa curt,
# 2. comprovarà quins usuaris poden executar amb permisos elevats (sudoers),
# 3. quins usuaris fa massa temps que van canviar la seva contrasenya,
# 4. quins ftxers tenen el permís d’execució per a els altres usuaris (others),
# 5. quins ftxers tenen el bit SETUID activat,
# 6. quins ftxers d’arxivat (.tar o .tgz) contenen ftxers amb el bit X activat.


# Lectura de paràmetres
case $# in
	0)
		lmin_name=4
		d_pass=300
	;;
	1)
		lmin_name=$1
		d_pass=300
	;;
	2)
		lmin_name=$1
		d_pass=$2
	;;
	*)
		echo -e "\e[31mError: número de paràmetres incorrecte\e[0m"
		exit 1
	;;
esac


echo "1. Usuaris amb nom massa curt:"

user_array=($(cut -d: -f1 /etc/passwd))

for uname in "${user_array[@]}"
do
	if [ ${#uname} -lt $lmin_name ]
	then
		echo -e "$uname\t${#uname}"
	fi
done


echo -e "\n2. Usuaris amb permisos elevats d'execució:"
echo -e "$(grep '^sudo:.*$' /etc/group | cut -d: -f4)"


echo -e "\n3. Usuaris amb contrasenya massa antiga:"

if [ "$EUID" -ne 0 ]
then
	echo -e "\e[31mError: cal ser root per veure aquesta informació\e[0m"
else
	str=$(cat /etc/shadow | cut -d: -f1,3)
	echo "$str"
fi











































