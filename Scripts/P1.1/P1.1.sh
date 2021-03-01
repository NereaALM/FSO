#!/bin/bash

# Crida: ./P1.1.sh [lmin_nom] [dies_pass] 

# Paràmetres d'entrada:
#	longitud mínima de nom d'usuari (opcional)
#	número màxim de dies entre canvis de contrasenya (opcional)

# Comprovacions:
# 1. quins usuaris 1 que tenen un nom massa curt,
# 2. comprovarà quins usuaris poden executar amb permisos elevats (sudoers),
# 3. quins usuaris fa massa temps que van canviar la seva contrasenya,
# 4. quins ftxers tenen el permís d’execució per a els altres usuaris (others),
# 5. quins ftxers tenen el bit SETUID activat,
# 6. quins ftxers d’arxivat (.tar o .tgz) contenen ftxers amb el bit X activat.

lmin_nom=$1
dies_pass=$2

if [ -z "$lmin_nom" ]
then
	$lmin_nom=4
fi

if [ -z "$dies_pass" ]
then
	$dies_pass=300
fi


echo $lmin_nom
echo $dies_pass
