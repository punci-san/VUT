
MAKERETVAL=""
RETVAL=""


#echo "$NAZEV"

cd "$CESTA"
[ -d "$NAZEV" ] || mkdir "${NAZEV}"
cd ..


#make clean
#make SBERAC > "${CESTA}/${NAZEV}/MAKE-stdout.txt" 2> "${CESTA}/${NAZEV}/MAKE-stderr.txt"
#MAKERETVAL=$(echo $?)

#if [ "$MAKERETVAL" -eq "0" ]
#then
#    echo "Make hotov"
#else
#    echo "Make ret $MAKERETVAL"
#    echo "${NAZEV} Selhal"
#    cd ..
#    exit $MAKERETVAL
#fi


./KOMP < "${CESTA}${NAZEV}.rb" > "${CESTA}/${NAZEV}/stdout.txt" 2> "${CESTA}/${NAZEV}/stderr.txt"
RETVAL=$(echo $?)

if [ -f "./SBERAC_DEBUG.txt" ]
then
	cp "./SBERAC_DEBUG.txt" "${CESTA}/${NAZEV}/SBERAC_DEBUG.txt"
fi

cat "${CESTA}/${NAZEV}/stderr.txt"

echo "Navratový kód ${RETVAL}"



