#!/bin/sh

# TOTO JE MOJE UPRAVENA VERZE, VSECHNY KOMENTARE JSEM PREPSAL A PROVEDL DALSI ZMENY
# ZDE JSOU PROMENNE, KTERE JSEM PREJMENOVAL
# start_edit => editovat
# path => cesta
# filesInPath => souboryAdresare
# file => soubor
# fileName => nazevSouboru
# inputDate => datum
# fileDate => pomocna
# name => temp (spousta dalsich promennych byla prejmenovana na 'temp')
# resultNames => vysledek

# tail -r => tac

editovat() { # funkce spusti editor a zapise do logu
    #tvoje:

    #editor=${EDITOR-${VISUAL-vi}}
    #$editor "$1"

    if [ -z "$EDITOR"]
    then
        if [ -z "$VISUAL" ]
            then
                vi "$1"
            else
                $VISUAL "$1"
        fi
        else
            $EDITOR "$1"
    fi
    ret=$?
    soubor=$1
    echo "${soubor##*/}:$(realpath "${soubor}"):$(date +%Y-%m-%d)" >> $WEDI
    echo "Vse probehlo v poradku, ukoncuji"
    exit $ret
}

#----------------END-OF-FUNCTIONS------------------------
cesta=${WEDI_RC?"ERROR:WEDI_RC není nastaveno"}
if ! [ -f $cesta ]
then # pokud neexistuje cesta do WEDI_RC, vytovri ji
    mkdir -p "$(dirname "$cesta")" # tvoje: && touch "$cesta"
    touch "$cesta"
fi
WEDI=`realpath "$WEDI_RC"`

#tvoje:  which realpath >/dev/null
echo $(realpath "$0")>/dev/null

if [ $? -ne 0 ]
then
    echo "ERROR: Doslo k chybe pri pouziti realpath">&2
    exit 1
fi

# pokud je skriptu zadano 4 a vice argumentu, program hlasi chybu
if [ $# -ge 4 ]
then
    echo "ERROR: Prilis mnoho arumentu" >&2
    exit 2
fi

# pripad spusteni bez argumentu
if [ $# -eq 0 ]
then
    # nalezeni naposled editovaneho souboru v soucasnem adresari a jeho spusteni

    souboryAdresare=`tac $WEDI | grep "$(realpath .)/[^/]*$"`

    for soubor in $souboryAdresare
    do
        nazevSouboru=`echo $soubor | cut -d':' -f1`
        if [ -f $nazevSouboru ]
        then
            editovat $nazevSouboru
        fi
    done
    echo "ERROR: V tomto adresari nebyl timto skriptem editovan ani jeden soubor" >&2
    exit 3

# pripad spusteni s adresarem jako jedinym argumentem
elif [ -d "$1" ] && [ $# -eq 1 ]
then
    cesta=$1
    souboryAdresare=`tac $WEDI | grep "$(realpath $cesta)/[^/]*$"`
    for soubor in $souboryAdresare
    do
        nazevSouboru=`echo $soubor | cut -d':' -f2`
        if [ -f $nazevSouboru ]
        then
            editovat $nazevSouboru
        fi
    done
    echo "ERROR: Ve vybranem adresari nebyl timto skriptem editovan ani jeden soubor" >&2
    exit 4

# pripad, kdy byl zadan pouze soubor
elif [ -f "$1" ] && [ $# -eq 1 ]
then
    editovat $1
fi

# tvuj kod:  (prepsal jsem potom i podminky od nich se odvijejici)
#while getopts :a:b:ml o
#do case "$o" in
#    a) option=""$option"a";;
#    b) option=""$option"b";;
#    m) option=""$option"m";;
#    l) option=""$option"l";;
#    *) echo "WRONG OPTIONS: Use options a/b with a date parameter, or m or l, or file ,or directory." >&2;;
#    esac
#done

#if [ "${#option}" -gt 1 ]
#then
#    echo "WRONG OPTIONS: Use only one option" >&2
#    exit 1
#fi
if [ $1 = "-a" ]
then
    if [ $# -eq 1 ]
    then
        echo "ERROR: Nebylo zadano datum" >&2
        exit 8
    fi

    echo "$2" | grep -qE "[0-9]{4}-[0-9]{2}-[0-9]{2}"
    if [ $? -ne 0 ]
    then
        echo "ERROR: Datum bylo zadano ve spatnem formatu" >&2
        exit 9
    fi

    # odstraneni '-' z promenne datum
    datum=`echo "$2" | sed "s/-//g"`

    if [ $# -eq 2 ]
    then
        cesta=`realpath "."`
    elif [ $# -eq 3 ]
    then
        cesta=`realpath "$3"`
    fi

    souboryAdresare=`grep "$cesta/[^/]*$" "$WEDI"`
    for soubor in $souboryAdresare
    do
        # ziskani data, kdy byl dany soubor editovan
        pomocna=`echo $soubor |cut -d':' -f3 | sed "s/-//g"`
        if [ $pomocna -ge $datum ]
        then
            vysledek="$vysledek `echo .$soubor |cut -d':' -f1`"
        fi
    done

    for temp in `echo $vysledek`
    do
        echo $temp
    done | sort | uniq

#-----------------------------END-A------------------------------
elif [ $1 = "-b" ]
then
        if [ $# -eq 1 ]
    then
        echo "ERROR: Nebylo zadano datum" >&2
        exit 10
    fi

    echo "$2" | grep -qE "[0-9]{4}-[0-9]{2}-[0-9]{2}"
    if [ $? -ne 0 ]
    then
        echo "ERROR: Datum bylo zadano ve spatnem formatu" >&2
        exit 11
    fi

    # odstraneni '-' z promenne datum
    datum=`echo "$2" | sed "s/-//g"`

    if [ $# -eq 2 ]
    then
        cesta=`realpath "."`
    elif [ $# -eq 3 ]
    then
        cesta=`realpath "$3"`                                             # POZOR!!! u predchoziho pouziti byl syntax: cesta=`realpath "$3"`
    fi

    souboryAdresare=`grep "$cesta/[^/]*$" "$WEDI"`
    for soubor in $souboryAdresare
    do
        pomocna=`echo $soubor |cut -d':' -f3 | sed "s/-//g"`
        if [ $pomocna -lt $datum ]
        then
            vysledek="$vysledek `echo .$soubor |cut -d':' -f1`"
        fi
    done

   for name in `echo $vysledek`
    do
        echo $name
    done | sort | uniq

#-----------------------------END-B------------------------------
# pripad, kdy byl pouzit argument -m
elif [ $1 = "-m" ]
then
    if [ $# -eq 1 ]
    then
        cesta=`realpath "."`
    elif [ $# -eq 2 ]
    then
        cesta=$(realpath "$2")
   elif [ $# -gt 2 ]
    then
        echo "ERROR: Chybny pocet argumentu" >&2
        exit 5
    fi

    souboryAdresare=`grep "$cesta/[^/]*$" "$WEDI" | cut -d':' -f2 | sort | uniq -c | sort -r`
    for temp in `echo $souboryAdresare`
    do
        if [ -f $temp ]
        then
            editovat $temp
        fi
    done

# osetreni prikazu pro vypsani vsech souboru v adresari, ktere byly editovany
elif [ $1 = "-l" ]
then
    if [ $# -eq 1 ]
    then
        cesta=`realpath "."`
    elif [ $# -eq 2 ]
    then
        cesta=$(realpath "$2")
     elif [ $# -gt 2 ]
    then
        echo "ERROR: Chybny pocet argumentu" >&2
        exit 6
    fi
    souboryAdresare=`grep "$cesta/[^/]*$" "$WEDI" | cut -d':' -f1 | sort | uniq`

    for temp in $souboryAdresare
    do
        echo .$temp
    done

#-----------------------------END-L------------------------------
# pokud neexistuje cesta, vytvori ji
elif [ $# -eq 1 ]
then
        if ! [ -d "$(dirname "$1")" ]
        then
            mkdir -p "$(dirname "$1")"
        fi

    editovat $1
else
    echo "ERROR: Doslo k chybe pri zadavani vstupu." >&2
    exit 7
fi
