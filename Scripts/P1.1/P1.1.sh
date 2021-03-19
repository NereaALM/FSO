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
		dmax_pass=300
	;;
	2)
		if [ $1 = "-n" ]
		then
			lmin_name=$2
			dmax_pass=300
		elif [ $1 = "-p" ]
		then
			lmin_name=4
			dmax_pass=$2
		else
			echo -e "\e[31mError: paràmetre incorrecte.\nEscriu -n pel mínim de caracters del nom i -p pel màxim de dies sense canvis a la contrasenya\e[0m"
			exit 1
		fi
	;;
	4)
		if [ $1 = "-n" ] && [ $3 = "-p" ]
		then
			lmin_name=$2
			dmax_pass=$4
		elif [ $1 = "-p" ] && [ $3 = "-n" ]
		then
			lmin_name=$4
			dmax_pass=$2
		else
			echo -e "\e[31mError: paràmetre incorrecte.\nEscriu -n pel mínim de caracters del nom i -p pel màxim de dies sense canvis a la contrasenya\e[0m"
			exit 1
		fi
	;;
	*)
		echo -e "\e[31mError: número de paràmetres incorrecte\e[0m"
		exit 1
	;;
esac


echo "1. Usuaris amb nom massa curt:"
# Lectura de fitxer /etc/passwd
user_array=($(cut -d: -f1 /etc/passwd))

 # Evaluació de longituds de noms
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
# Calcul data actual -300 dies
dmax_pass=$(date --date="-$dmax_pass days" +%Y%m%d)

# Array amb usuari:data en dies desde 1970
ud_pass_array=($(sudo cat /etc/shadow | cut -d: -f1,3))

for ud_pass in "${ud_pass_array[@]}"
do
	# data en segons desde 1970		
	let d_pass=$(echo $ud_pass | cut -d: -f2)*24*60*60
	# data última modificació contrasenya
	d_pass=$(date -d"@$d_pass" +%Y%m%d)
		
	if [ $d_pass -lt $dmax_pass ]
	then
		echo $ud_pass | cut -d: -f1
	fi
done


ls -l -R $HOME > ls_aux

echo -e "\n4. Fitxers executables per altres:"
cat ls_aux | grep -e '^-[r-][w-][x-][r-][w-][x-][r-][w-][x]' | tr -s ' ' | cut -d' ' -f9


echo -e "\n5. Fitxers amb el bit SETUID activat:"
cat ls_aux | grep -e '^-[r-][w-][sS][r-][w-][x-][r-][w-][x-]' | tr -s ' ' | cut -d' ' -f9

rm ls_aux


echo -e "\n6. Fitxers d'arxivat que contenen fitxers amb bit X activat:"
tar_names=$(ls -R $HOME | grep -e '[*]*.tar' -e '[*]*.tgz')

for cfile in $tar_names
do
	path=$(find $HOME -name "$cfile" -print 2>/dev/null)
	
	tar -tzvf $path > tar_content
	
	is_printed_cfile=0
	while IFS= read -r line && [[ $is_printed_cfile -eq 0 ]]
	do
		# Si el fitxer es executable i no s'ha imprès a pantalla, imprimim cfile(pare comprimit)
		if [[ "$(echo "$line" | cut -d' ' -f1)" == *x* ]] && [ $is_printed_cfile -eq 0 ]
		then
			echo $cfile
			is_printed_cfile=1
		fi
	done < tar_content
	
done

if [ -f tar_content ] 
then
	rm tar_content
fi



