#!/bin/bash
#Mazač bordelu po AutoTest skriptech

echo "Mažu všechny logy"



cd testiky_bad

soubory=$(ls -1 | grep ".rb" | cut -d "." -f 1)

for i in $soubory 
do
    echo "Mažu $i"
    rm -rf "$i"
done

cd ..
cd testiky_ok

soubory=$(ls -1 | grep ".rb" | cut -d "." -f 1)

for i in $soubory 
do
    echo "Mažu $i"
    rm -rf "$i"
done

cd ..
cd testiky

rm -rf MAKE

cd ..

rm -rf logy

echo "HOTOVO"



