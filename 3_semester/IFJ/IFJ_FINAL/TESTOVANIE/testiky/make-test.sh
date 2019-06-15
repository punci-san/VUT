#!/bin/bash
NAZEV=MAKE
CESTA=./testiky/

MAKERETVAL=""
RETVAL=""


echo "$NAZEV"

cd "$CESTA"
[ -d "$NAZEV" ] || mkdir "${NAZEV}"
cd ..


make clean
make > "${CESTA}/${NAZEV}/MAKE-stdout.txt" 2> "${CESTA}/${NAZEV}/MAKE-stderr.txt"
MAKERETVAL=$(echo $?)

if [ "$MAKERETVAL" -eq "0" ]
then
    echo "Make hotov"
    echo "${NAZEV} Prošel"
else
    echo "Make ret $MAKERETVAL"
    echo "${NAZEV} Selhal"
    cd ..
    exit $MAKERETVAL
fi

exit $MAKERETVAL
