#!/bin/bash

# Obtenir un llistat del directori actual, mostrant només
# la informació del propietari, el grup, la mida i el nom
# de cada fitxer

ls -l | tr -s ' ' >fbase
cat fbase | cut -d' ' -f3 >f3
cat fbase | cut -d' ' -f4 >f4
cat fbase | cut -d' ' -f5 >f5
cat fbase | cut -d' ' -f9 >f9
paste f3 f4 f5 f9
rm fbase f3 f4 f5 f9
