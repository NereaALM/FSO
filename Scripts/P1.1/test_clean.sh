#!/bin/bash

# Aquest script neteja del PC les modificacions fetes per test.sh
# Es necessari executarlo amb permissos de sudo

# 1. quins usuaris tenen un nom massa curt
# 2. comprovarà quins usuaris poden executar amb permisos elevats (sudoers)
# 3. quins usuaris fa massa temps que van canviar la seva contrasenya
userdel aaa
userdel aba
userdel aaaaaaaa
userdel bbbbbbbb
echo -e "Usuaris esborrats\n"

# 4. quins ftxers tenen el permís d’execució per a els altres usuaris (others)
# 5. quins ftxers tenen el bit SETUID activat
rm file{1..3} 
echo -e "Fitxers esborrats\n"

# 6. quins ftxers d’arxivat (.tar o .tgz) contenen ftxers amb el bit X activat
rm othersX.tar
rm not_othersX.tar
rm othersX.tgz
rm not_othersX.tgz
echo -e "Tars esborrats\n"
