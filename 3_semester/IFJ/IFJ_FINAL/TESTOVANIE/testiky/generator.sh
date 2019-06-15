#!/bin/bash
#Generátor AutoTest skriptů

CESTA="./testiky/"

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
	interpretr=0
	inter_aktiv=0

	hlavicka=$(head -n 1 ${i}.txt)
	hlavicka_hl=$(echo "$hlavicka" | head -c 10 | grep '###retval=')

	if [ "$hlavicka_hl" == "###retval=" ]
	then
		ocekavanyRet=$(echo "$hlavicka" | cut -d "=" -f 2)
		echo "očkáváná návratová hodnota $ocekavanyRet"
	else
		echo "NENI ZADANA NAVRATOVA HODNOTA"
		exit 1
	fi

	hlavicka=$(head -n 2 ${i}.txt | tail -n 1)
	hlavicka_hl=$(echo "$hlavicka" | head -c 14 | grep '###interpretr=')

	if [ "$hlavicka_hl" == "###interpretr=" ]
	then
		interpretr=$(echo "$hlavicka" | cut -d "=" -f 2)
		echo "očkáváná návratová hodnota interpretru $interpretr"
		inter_aktiv=1
	fi

	
	if [ "$inter_aktiv" -eq "0" ]
	then
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
	else
		echo '#spouštění interprestru'	>> "${i}.sh"
		echo './ic18int "${CESTA}/${NAZEV}/stdout.txt" > ${CESTA}/${NAZEV}/interout.txt 2> ${CESTA}/${NAZEV}/intererr.txt' >> "${i}.sh"
		echo 'RETINTER=$(echo $?)'			>> "${i}.sh"
		echo 'echo "Navratový kód interpretru ${RETINTER}"'		>> "${i}.sh"
		echo '#####'  					>> "${i}.sh"
		echo 'if [ "$RETVAL" -eq' " $ocekavanyRet ]" 	>> "${i}.sh"
		echo 'then' 					>> "${i}.sh"
		echo '    if [ "$RETINTER" -eq' " $interpretr ]" 	>> "${i}.sh"
		echo '    then' 					>> "${i}.sh"
		echo '          echo "${NAZEV} Prošel"' 		>> "${i}.sh"
		echo '    else' 					>> "${i}.sh"
		echo '          echo "očekáván návratový interpretru kód "' "$interpretr" 		>> "${i}.sh"
		echo '          echo "${NAZEV} Selhal"' 	>> "${i}.sh"
		echo '    fi' 					>> "${i}.sh"
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


