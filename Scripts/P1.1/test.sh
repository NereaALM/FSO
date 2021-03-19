#!/bin/bash

# Aquest script fa les tasques necessàries per els jocs de proves dels scripts P1.1.sh i P1.2.py
# Es necessari executarlo amb permissos de sudo


# 1. quins usuaris tenen un nom massa curt
adduser --disabled-password --gecos "" aaa
adduser --disabled-password --gecos "" aba
adduser --disabled-password --gecos "" aaaaaaaa
adduser --disabled-password --gecos "" bbbbbbbb

echo -e "Usuaris creats\n"


# 2. comprovarà quins usuaris poden executar amb permisos elevats (sudoers)
adduser aaa sudo
adduser aba sudo

echo -e "Usuaris sudoers afegits\n"


# 3. quins usuaris fa massa temps que van canviar la seva contrasenya
line_number=$(grep -n "^aaa:" /etc/shadow | cut -d: -f1)

days=$(grep -n "^aaa:" /etc/shadow | cut -d: -f4)
let days=$days-500

line_content_ini=$(grep -n "^aaa:" /etc/shadow | cut -d: -f2-3)
line_content_end=$(grep -n "^aaa:" /etc/shadow | cut -d: -f5-10)

line_content="$line_content_ini:$days:$line_content_end"

sed -i "$line_number s/.*/$line_content/" /etc/shadow

echo -e "/etc/shadow modificat afegint $line_content\n"

# 4. quins ftxers tenen el permís d’execució per a els altres usuaris (others)
touch file{1..3}
echo -e "Fitxers 1,2 i 3 creats\n"

chmod o+x file2
echo -e "file2 té permiís d'execució a others\n"

# 5. quins ftxers tenen el bit SETUID activat
chmod u+s file3
echo -e "file té SETUID activat\n"


# 6. quins ftxers d’arxivat (.tar o .tgz) contenen ftxers amb el bit X activat
tar -czvf othersX.tar file2
tar -czvf not_othersX.tar file1
tar -czvf othersX.tgz file2
tar -czvf not_othersX.tgz file1
echo -e "Tars creats:\nothersX.tar i othersX.tgz contenen file2\nnot_othersX.tar i not_othersX.tgz contenen file1\n"
