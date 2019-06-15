#!/bin/bash
#AutoTest skripty
Autor="ZmatenejStrejda"

STRANKA=./logy/projekt.html
SOUBOR=./logy/log.txt
PROSEL=0
SELHAL=0
DATUM=$(date "+%Y-%m-%d_%H:%M:%S")
TESTOVY_SOUBOR=$(cat ./testy)

CESTA_EXPORT=$(echo "./logy/${DATUM}")

TERMINAL=ANO


if ! [ -d ./logy ]
then    if [ -e ./logy ]
            then echo "logy už existuje" >&2
                exit 1
        fi
    mkdir logy
    mkdir ./logy/html
    echo "vytvořeno logy a logy/html" >&2
fi



printf '<html>\n   <head>\n      <meta charset="utf-8">\n       <title>Projekt testy</title>\n   </head>\n<body style="background-color:black;">\n' > $STRANKA
printf "<font color="white">\n\n" >> $STRANKA
echo "<p><b>Projekt Testy</b></p>" >> $STRANKA
echo "<p>" $(date "+%Y-%m-%d . . . %H:%M:%S") "<br></p><br>" >> $STRANKA

######## soubor
echo "$DATUM" > "$SOUBOR"
echo "" >> $SOUBOR
####### terminal
echo "$DATUM"
echo ""
#################

tisk(){
    echo "<br>$1" >> $STRANKA
    echo "$1" >> $SOUBOR ##########
}
tisk_zele(){
    echo "<br> <font color=\"#00ff00\">$1</font>" >> $STRANKA
    #echo -e "\e[92m$1\e[0m" >> $SOUBOR ##############
}
tisk_cerv(){
    echo "<br> <font color=\"#ff0000\">$1</font>" >> $STRANKA
    #echo -e "\e[31m$1\e[0m" >> $SOUBOR ############
}
tisk_tes(){
    PROGRAM="$1"
    VYSTUP="$($PROGRAM)"
    VYSTUP2=$VYSTUP ################
    CHYBA=$(printf "$VYSTUP" | grep "Prošel")
    if [ -z "$CHYBA" ]; 
        then (( SELHAL += 1 ))
        else (( PROSEL += 1 ))
    fi
    VYSTUP=$(printf "$VYSTUP" | sed ':a;N;$!ba;s/\n/\n<br>/g')
    VYSTUP=$(printf "$VYSTUP" | sed 's|Selhal|<font color="#ff0000"><b>Selhal</b></font>|')
    VYSTUP=$(printf "$VYSTUP" | sed 's|Prošel|<font color="#00ff00"><b>Prošel</b></font>|')
    echo "<p>$PROGRAM<br>$VYSTUP</p><br>" >> $STRANKA
    
    #######################
    VYSTUP2=$(printf "$VYSTUP2" | sed 's|Selhal|\\e[31;1mSelhal\\e[0m|')
    VYSTUP2=$(printf "$VYSTUP2" | sed 's|Prošel|\\e[92;1mProšel\\e[0m|')
    echo "$PROGRAM" >> $SOUBOR
    echo -e "$VYSTUP2" >> $SOUBOR
    echo "" >> $SOUBOR
    ############# terminal
    if [ "$TERMINAL" == "ANO" ];then
        echo "$PROGRAM" 
        echo -e "$VYSTUP2" 
        echo "" 
    fi
}
###########################################################################################################
###########################################################################################################
#######################  ZDE SPUSTET TESTY ################################x
#tisk "jooo"
#tisk_zele "žlutá"
#tisk_cerv "modrá"


#tisk_tes "./test01.sh"

#tisk_tes "./test02.sh"

#tisk_tes "./test02.sh"

##forcyklus z nějakýho souboru nebo souborů se budou načítat cesty s testům
if [ -z "$TESTOVY_SOUBOR" ]; then 
    echo "nenalezeny žádné testy" >&2
    exit 3
fi

SLOZKA=$(pwd)
for i in $TESTOVY_SOUBOR
do
        cd $SLOZKA
    	echo "Zapinam $i"
	tisk_tes "$i"
done


###########################################################################################################
# při chybě by to mohlo vytvořit(a pak smazat) soubor který by se objevil ve vimu???
# mazání duplikovaných logů
# do složky s datumem přidat logy všech testů
# přidávání výstupů do druhého souboru s bavičkama pro shell 
###########################################################################################################
if [ $SELHAL -eq "0" ]
    then tisk_zele "<b>Prošlo $PROSEL z $PROSEL </b>"
         echo -e "\e[92;1mProšlo $PROSEL z $PROSEL\e[0m" >> $SOUBOR
         echo -e "\e[92;1mProšlo $PROSEL z $PROSEL\e[0m" 

    else tisk_cerv "<b>Prošlo $PROSEL z $((( SELHAL + PROSEL ))) </b>"
        echo -e "\e[31;1mProšlo $PROSEL z $((( SELHAL + PROSEL )))\e[0m" >> $SOUBOR
        echo -e "\e[31;1mProšlo $PROSEL z $((( SELHAL + PROSEL )))\e[0m" 

fi

#echo "<p>" $(date "+%Y-%m-%d . . . %H:%M:%S") "<br></p><br>" >> $STRANKA
#DATUM=$(date "+%Y-%m-%d_%H:%M:%S")

if [ $SELHAL -eq "0" ]
    then    echo -e "\e[92;1mAlles Gute\e[0m"
    else    echo -e "\e[31;1mKde asi soudruzi udělali chybu?\e[0m"
            cp "$STRANKA" "./logy/html/test_${DATUM}.html"
            ############
            mkdir "./logy/$DATUM"
            cp "$SOUBOR" "./logy/${DATUM}/log"
fi

printf "\n</font>\n" >> $STRANKA
printf "\n</body>\n<html>\n" >> $STRANKA
if [ "$Autor" != "ZmatenejStrejda" ]; then 
echo -e "\e[31;1m                                                                                                                     
                                                                                                                     
                                                                                                                     
                                                                                                                     
                                                                                                                     
                                                                                                                     
                               '. ;l .;               .,,.    .,:lolc,                                               
                               ;d :Nd.KKdc,.'',,...:kXNNNNNXXNNNNNNNNNKl;d:.                                         
                               .Kd.KNKkO0NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNxdNX:                                        
                                ,X0xNNNNK0KNNNN0NNNNNNNNNNNNNNNNNNNNNNNN0xNNOcl                                      
                                 '0NNKkNN0k0NNNKxXNNNN0KNNNNNNNNNNNNNNNNNNNXoKO                                      
                                   ;kX0dONN0kk0NKo0NNNoOXxNNNNNNNNNNNNNNNNNKXX;                                      
                                     .;lc;lxO0dlldldkX0lkdNXkdolllcccclloxdo:.                                       
                                   .';:clllc:;;:ll:;,;c,.;:;:lllllllllllll:,.                                        
                                 .:llllllllllllllllllllc;',clllllllllllllllllc;.                                     
                       .'clclc;.:lllllllllllllllllllllllllc';llllllllllllcccccll; ',.                                
                    'o0NNNNNNK;:lllllllllllllc:;;;;;;;;;:lll;'llllll:;;;;;;;;;;;;''KN0c                              
                 .l0NNX0XNNKl,clllllllllll:;;;:clllllllc;,,,c.:llllcclllllllllllllc;:lKO.                            
       .;.     ,dXN0kkONKOkkc;llllllllllc::llllllllllllllllc,.,llllllllllllllllllllllc,xk                            
         ;kOxoxxxxkOXNXKKNXl;lllllllllllllllllllllllc::::::clc.llllllllllc;;;;;;;;;;;;c;Nd                           
        .  ,dKNNNNNNNNNXOx,;lllllllllllllllllllc;,,;;;;,,,,,,,.llllllll:;;:;;;;;;;;;;,,:0Nk;,'                       
         okxolxkkk0NXK0O0o;lllllllllllllllllc;;;;;;;;;;;;;;;;,.,lllll:;;;,'',,'',,,;;;;,;NNX0:                       
          .l0NNNNNNNX0Ok,;lllllllcc::clllllc;;,,',,'.,;......:c..ll, .coxk.  ;K.      ol'lNXk.                       
          :OxxolkkOXNNNK,cllll:::::::;cllc;',cxkKd   kx      ,MK.ll',;cxWX      .;    OMO:X0X,                       
         ,0lllo0XNXKOxl;clllllllllllllc:;':KMMMMM.     ,Od   'Kc.llc.c;;;c      ;l.  'dl.xNKc                        
         dNNNNKkkkxo.cllllllllc:;;;::::::cc;cokKWd     .:.  .',',lll;.c:;;;,,''.''',;;;'cXXO.                        
        cXNKKNNNNNNo;lllllll:;;:lllllllllll;;;:;;:,....',,,,,;.,lllll;':llc:;;;;;;;;;;;'KXd.                         
    .,cdoxO0NNNNNN0,llllllllcllllllll:lllllllc;;;;;;;;;;;;:cl,,lllllllc,,:llllllllllc,.c;.                           
       .kNNNNNNNXd;clllllllllllllllllc;:clllllllllllllllll:,,:llllllllllc,'cllc:;;;,                                 
     .lXNNNXONXd;;clllllllllllllllllllllcllllllllllllc:;,;;cllllllllllllll;:llllllll:                                
    :KOKNNXdX0:c:'lllllllllllllllllllllllllllllllc::;;:clllllllllllllllllllllllllllll.                               
  .xk'cX0Nd0O,cl,,llllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllc.                               
  x,  0dKXoO,lll;.llllllllllllllllllllllllllllllllllllllllcc::;;;;;;;;,,,,,,,,,,'''..''.                             
 .   ;NcN0:,lllll.:lllllllllllllllllllc;'''''''',,,'''''''..'''',,,,,,,,,,,,,,,,,,,,,,,,.                            
     :K.oK :lllllc';lllllllllllllllll:.',,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,',,,,,,,,,''...                             
     ,o;,:;;,llllll;;cllllllllllllllc.,,,,,,,'..''''...........................'...'',,,                             
     ':,,:,.';lllllllclllllllllllllll.',,,,,,,,''''',,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,.                             
      .:llllc';lllcllllllllllllllllllc'.',,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,'.                              
       .llllll,,cl;,lllll:,llllllllllllc;,''''.........'''''''''''''''''''''''''',,,,                                
       'lllllllc:cl:,:cllc.clllllllllllllllllllllllllllllllllllllllllllllllllllllllll.                               
       .llllllllcllllcllll:'cllllllllllllllllllllllllllllllllllllllllllllll:llllllll,                                
        'lllllll;'lllllllllc',clllllllllllllllllllllllllllllllllllllllllll,:llllll:.                                 
        .llllllll,'clllllllllc;;:lllllllllllllllllllllllllllllllllllllll:,:llllll'                                   
        'lllllllll:',cllllll:'clllllllllllllllllllllllllllllllllllllll;,;cllllll,                                    
      .,clllllllllllc,,,:clll:';llllllllllllllllllllllllllllllllllc;,,:lllllll:.                                     
   ..;cllllllllllllllllc::cllll;,,:llllllllllllllllllllllc::;;;;;;;:llllllc;'.                                       
;:cllllllllllllllllllllllllllllllc;,,,;:clllllllllllllllllccclllllllllc,,,.                                          
llllllllllllllllllllllllllllllllllllllc:;:cllllllllllllllllllllllllll,'clllc,..                                      
lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllc';lllllllll:,.                                   
lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllc;;clllllllllllll;                                  
lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllll:                                 
lllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllllc."
echo ""
echo -e "\e[31;1mTy niggo, změnil si autora :D"
echo ""
fi
##deduplikátor
#./deduplikator.sh
######

exit 0

