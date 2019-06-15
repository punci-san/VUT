#!/bin/bash
#Generátor AutoTest skriptů

CESTA="./testiky_bad/"
BAD="1"

soubory=$(ls -1 | grep ".txt" | cut -d "." -f 1)
#soubory=$(ls -1 | grep ".txt" | cut -d "." -f 1 | tr "\n" " ")

if [ "${soubory}" == "" ]
then
	echo nic k delani
	exit 0
fi

for i in $soubory
do
	echo "Spoustim pro ${i}"

	echo '#!/bin/bash' > "${i}.sh"
	echo '#Vygenerováno AutoTest skriptem' >> "${i}.sh"
	echo "NAZEV=${i}" >> "${i}.sh"
	echo "CESTA=${CESTA}" >> "${i}.sh"

	cat "./defautl_generator.sh" >> "${i}.sh"

	ocekavanyRet=0

	hlavicka=$(head -n 1 ${i}.txt)
	hlavicka_hl=$(echo "$hlavicka" | head -c 10 | grep '###retval=')

	if [ "$hlavicka_hl" == "###retval=" ]
	then
		ocekavanyRet=$(echo "$hlavicka" | cut -d "=" -f 2)
		echo "očkáváná návratová hodnota $ocekavanyRet"
		BAD=0
	fi

	if [ "$BAD" -eq "1" ]
	then
		echo '#####' 				>> "${i}.sh"
		echo 'if [ "$RETVAL" -eq "0" ]' 	>> "${i}.sh"
		echo 'then' 				>> "${i}.sh"
		echo '    echo "${NAZEV} Selhal"' 	>> "${i}.sh"
		echo 'else' 				>> "${i}.sh"
		echo '    echo "${NAZEV} Prošel"' 	>> "${i}.sh"
		echo 'fi' 				>> "${i}.sh"
		echo 'cd ..' 				>> "${i}.sh"
		echo 'exit $RETVAL' 			>> "${i}.sh"
	else
		echo '#####'  					>> "${i}.sh"
		echo 'if [ "$RETVAL" -eq' " $ocekavanyRet ]" 	>> "${i}.sh"
		echo 'then' 					>> "${i}.sh"
		echo '    echo "${NAZEV} Prošel"' 		>> "${i}.sh"
		echo 'else' 					>> "${i}.sh"
		echo '    echo "očekáván návratový kód "' "$ocekavanyRet" 		>> "${i}.sh"
		echo '    echo "${NAZEV} Selhal"' 		>> "${i}.sh"
		echo 'fi' 					>> "${i}.sh"
		echo 'cd ..' 					>> "${i}.sh"
		echo 'exit $RETVAL' 				>> "${i}.sh"
	fi

	chmod +x "${i}.sh"
	mv "${i}.txt" "${i}.rb"
	#echo "presun ${i}.txt" "${i}.rb"
	echo "${CESTA}${i}.sh" >> ../testy

done


