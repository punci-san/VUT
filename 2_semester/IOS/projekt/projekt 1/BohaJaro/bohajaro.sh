#!/bin/sh
POSIXLY_CORRECT=yes
if [ ! $WEDI_RC ]; then
  #statements
  echo "WEDI_RC premenna nie je zadana."
  exit 1
fi
WEDI_RC=`readlink -f "$WEDI_RC"`

WEDIRC_CESTA=`echo $WEDI_RC | rev | cut -d "/" -f1 | rev `

WEDIRC_CESTA=`echo "$WEDI_RC" | grep -Po ".*?(?=$WEDIRC_CESTA)" `


if [ ! -e "$WEDIRC_CESTA" ]; then
  mkdir -p "$WEDIRC_CESTA"
fi
if [ ! -e "$WEDI_RC" ]; then
  #echo "Vytvaram subor"
  touch "$WEDI_RC"
fi





# Nastavime editor
if [ $EDITOR ]; then
  #
  OTVARANIE_SUBORU=$EDITOR
elif [ $VISUAL ]; then
  #
  OTVARANIE_SUBORU=$VISUAL
else
  OTVARANIE_SUBORU="vi"
fi

#Â 0 argumentov
#Â pwd, posledne editovany subor
if [ "$#" = "0" ]; then
  #Â Vyhladavanie vyhovujuceho suboru
  AKTUALNYADRESAR=`pwd`
  OBSAHSUBORU=`cat "$WEDI_RC" | grep "$AKTUALNYADRESAR/ "`
  if [ -z "$OBSAHSUBORU" ]; then
    echo "Nic na otvorenie."
    exit 1
  fi
  SUBOR=$( echo "$OBSAHSUBORU" | \
  {
    VYHOVUJUCISUBOR="empty"
    NAJPOSLEDNEJSIDATUM=0
    while read riadok
    do
      DATUM=`echo $riadok | cut -d " " -f3 | cut -d "." -f1`    #Â roky
      DATUM=$DATUM`echo $riadok | cut -d " " -f3 | cut -d "." -f2` #Â mesiace
      DATUM=$DATUM`echo $riadok | cut -d " " -f3 | cut -d "." -f3` #Â dni
      DATUM=$DATUM`echo $riadok | cut -d " " -f4 | cut -d ":" -f1` #Â hodiny
      DATUM=$DATUM`echo $riadok | cut -d " " -f4 | cut -d ":" -f2` # minuty
      DATUM=$DATUM`echo $riadok | cut -d " " -f4 | cut -d ":" -f3` #Â sekundy
      #      GOODDATE   <   DATE
      if [ "$NAJPOSLEDNEJSIDATUM" -lt "$DATUM" ]; then
        NAJPOSLEDNEJSIDATUM="$DATUM"
        VYHOVUJUCISUBOR=`echo "$riadok" | cut -d " " -f1`
      fi
      #statements
    done
    if [ "$VYHOVUJUCISUBOR" = "empty" ]; then
      echo "0"
    else
      echo "$VYHOVUJUCISUBOR"
    fi

  })

  #Â ak sme nic nenasli vyhod error
  if [ "$SUBOR" = "0" ]; then
    echo "Ziadny subor na otvorenie."
    exit 1
  fi

  #Â nazov programu + nazov suboru na spustenie
  $OTVARANIE_SUBORU "$SUBOR"
  if [ ! "$?" = 0 ]; then
    echo "Problem pri otvarani suboru."
    exit 1
  fi

  DATUM=`date +%Y.%m.%d`
  CAS=`date +%H:%M:%S`
  OBSAHSUBORU=` cat $WEDI_RC | grep "$SUBOR $AKTUALNYADRESAR/ "`
  if [ ! -z "$OBSAHSUBORU" ]; then
    POCETOTVORENI=`echo "$OBSAHSUBORU" | cut -d " " -f6`
    POCETOTVORENI=$(($POCETOTVORENI+1))
    LOGRIADKU=" $SUBOR $AKTUALNYADRESAR/ $DATUM $CAS $POCETOTVORENI"
    OBSAHSUBORU=`cat $WEDI_RC | grep -v "$SUBOR $AKTUALNYADRESAR/ "`
    echo "$OBSAHSUBORU" > $WEDI_RC
    echo "$LOGRIADKU" >> $WEDI_RC
  fi
  exit $?
fi

# 1 argument
if [ "$#" = "1" ]; then
    if [ "$1" = "-m" ]; then
      AKTUALNYADRESAR=`pwd`
      OBSAHSUBORU=`cat $WEDI_RC | grep "$AKTUALNYADRESAR/ "`
      if [ -z "$OBSAHSUBORU" ]; then
        echo "Subor neobsahuje ziadny subory v $AKTUALNYADRESAR."
        exit 1
      fi
      SUBOR=$( echo "$OBSAHSUBORU" | \
      {
        VYHOVUJUCISUBOR="empty"
        NAJVIACSPUSTENY=0
        while read riadok
        do
          POCETSPUSTENI=` echo "$riadok" | cut -d " " -f5`
          #      GOODDATE   <   DATE
          if [ "$NAJVIACSPUSTENY" -lt "$POCETSPUSTENI" ]; then
            NAJVIACSPUSTENY="$POCETSPUSTENI"
            VYHOVUJUCISUBOR=`echo "$riadok" | cut -d " " -f1`
          fi
          #statements
        done

        if [ "$VYHOVUJUCISUBOR" = "empty" ]; then
          echo "0"
        else
          echo "$VYHOVUJUCISUBOR"
        fi
      })

      if [ "$SUBOR" = 0 ]; then
        echo "Ziadny vyhovujuci subor."
        exit 1
      fi

      #Â nazov programu + nazov suboru na spustenie
      $OTVARANIE_SUBORU "$SUBOR"
      if [ ! "$?" = 0 ]; then
        echo "Problem pri otvarani suboru."
        exit 1
      fi

      DATUM=`date +%Y.%m.%d`
      CAS=`date +%H:%M:%S`
      OBSAHSUBORU=` cat $WEDI_RC | grep " $SUBOR $AKTUALNYADRESAR/ "`
      if [ ! -z "$OBSAHSUBORU" ]; then
        POCETOTVORENI=`echo "$OBSAHSUBORU" | cut -d " " -f6`
        POCETOTVORENI=$(($POCETOTVORENI+1))
        LOGRIADKU=" $SUBOR $AKTUALNYADRESAR/ $DATUM $CAS $POCETOTVORENI"
        OBSAHSUBORU=`cat $WEDI_RC | grep -v "$SUBOR $AKTUALNYADRESAR/ "`
        echo "$OBSAHSUBORU" > $WEDI_RC
        echo "$LOGRIADKU" >> $WEDI_RC
      else
        LOGRIADKU=" $SUBOR $AKTUALNYADRESAR/ $DATUM $CAS 1"
        echo "$LOGRIADKU" >> $WEDI_RC
      fi
      exit $?

    elif [ "$1" = "-l" ]; then
      OBSAHSUBORU=`cat "$WEDI_RC" | grep "$AKTUALNYADRESAR/ "`
      SUBORY=$( echo "$OBSAHSUBORU" | \
      {
        while read riadok
        do
          echo .`echo "$riadok" | cut -d " " -f1`
        done
      })
      if [ "$SUBORY" = "." ]; then
        echo "Ziadne subory na vypisanie."
        exit 1
      fi
      echo "$SUBORY"
      exit 0
    elif [ -d "$1" ]; then
      AKTUALNYADRESAR=`readlink -f "$1"`
      OBSAHSUBORU=`cat "$WEDI_RC" | grep "$AKTUALNYADRESAR/ "`
      if [ -z "$OBSAHSUBORU" ]; then
        echo "Log neobsahuje ziadny subory v $AKTUALNYADRESAR."
        exit 1
      fi
      SUBOR=$( echo "$OBSAHSUBORU" | \
      {
        VYHOVUJUCISUBOR="empty"
        NAJPOSLEDNEJSIDATUM=0
        while read riadok
        do
          DATUM=`echo $riadok | cut -d " " -f3 | cut -d "." -f1`    #Â roky
          DATUM=$DATUM`echo $riadok | cut -d " " -f3 | cut -d "." -f2` #Â mesiace
          DATUM=$DATUM`echo $riadok | cut -d " " -f3 | cut -d "." -f3` #Â dni
          DATUM=$DATUM`echo $riadok | cut -d " " -f4 | cut -d ":" -f1` #Â hodiny
          DATUM=$DATUM`echo $riadok | cut -d " " -f4 | cut -d ":" -f2` # minuty
          DATUM=$DATUM`echo $riadok | cut -d " " -f4 | cut -d ":" -f3` #Â sekundy
          #      GOODDATE   <   DATE
          if [ "$NAJPOSLEDNEJSIDATUM" -lt "$DATUM" ]; then
            NAJPOSLEDNEJSIDATUM="$DATUM"
            VYHOVUJUCISUBOR=`echo "$riadok" | cut -d " " -f1`
          fi
          #statements
        done

        if [ "$VYHOVUJUCISUBOR" = "empty" ]; then
          echo "0"
        else
          echo "$VYHOVUJUCISUBOR"
        fi

      })

      #Â nazov programu + nazov suboru na spustenie
      $OTVARANIE_SUBORU "$SUBOR"
      if [ ! "$?" = 0 ]; then
        echo "Problem pri otvarani suboru."
        exit 1
      fi

      DATUM=`date +%Y.%m.%d`
      CAS=`date +%H:%M:%S`
      OBSAHSUBORU=` cat $WEDI_RC | grep "$SUBOR $AKTUALNYADRESAR/ "`
      if [ ! -z "$OBSAHSUBORU" ]; then
        POCETOTVORENI=`echo "$OBSAHSUBORU" | cut -d " " -f6`
        POCETOTVORENI=$(($POCETOTVORENI+1))
        LOGRIADKU=" $SUBOR $AKTUALNYADRESAR/ $DATUM $CAS $POCETOTVORENI"
        OBSAHSUBORU=`cat $WEDI_RC | grep -v "$SUBOR $AKTUALNYADRESAR/ "`
        echo "$OBSAHSUBORU" > $WEDI_RC
        echo "$LOGRIADKU" >> $WEDI_RC
      fi
      exit $?

    elif [ -f "$1" ]; then
      AKTUALNYADRESAR=`readlink -f "$1"`

      #Â nazov programu + nazov suboru na spustenie
      $OTVARANIE_SUBORU "$AKTUALNYADRESAR"
      if [ ! "$?" = 0 ]; then
        echo "Problem pri otvarani suboru."
        exit 1
      fi

      SUBOR=`echo "$AKTUALNYADRESAR" | rev | cut -d '/' -f1 | rev `
      AKTUALNYADRESAR=$( echo "$AKTUALNYADRESAR" | grep -Po ".*?(?=$SUBOR)")

      DATUM=`date +%Y.%m.%d`
      CAS=`date +%H:%M:%S`
      OBSAHSUBORU=` cat $WEDI_RC | grep "$SUBOR $AKTUALNYADRESAR "`

      if [ ! -z "$OBSAHSUBORU" ]; then
        POCETOTVORENI=`echo "$OBSAHSUBORU" | cut -d " " -f5`
        POCETOTVORENI=$(($POCETOTVORENI+1))
        LOGRIADKU=" $SUBOR $AKTUALNYADRESAR $DATUM $CAS $POCETOTVORENI"
        OBSAHSUBORU=`cat $WEDI_RC | grep -v "$SUBOR $AKTUALNYADRESAR "`
        echo "$OBSAHSUBORU" > $WEDI_RC
        echo "$LOGRIADKU" >> $WEDI_RC
      else
        LOGRIADKU=" $SUBOR $AKTUALNYADRESAR $DATUM $CAS 1"
        echo "$LOGRIADKU" >> $WEDI_RC
      fi
      exit $?
    else
      echo "Zle zadane argumenty"
      exit 1
    fi
fi

#Â 2 argumenty
if [ "$#" = "2" ]; then
  if [ -d "$2" ]; then
    if [ "$1" = "-l" ]; then
      AKTUALNYADRESAR=`readlink -f "$2"`
      OBSAHSUBORU=`cat "$WEDI_RC" | grep "$AKTUALNYADRESAR/ "`
      SUBORY=$( echo "$OBSAHSUBORU" | \
      {
        while read riadok
        do
          echo .`echo "$riadok" | cut -d " " -f1`
        done
      })
      if [ "$SUBORY" = "." ]; then
        echo "Ziadne subory na vypisanie."
        exit 1
      fi
      echo "$SUBORY"
      exit 0
    elif [ "$1" = "-m" ]; then
      AKTUALNYADRESAR=`readlink -f "$2"`
      OBSAHSUBORU=`cat $WEDI_RC | grep "$AKTUALNYADRESAR/ "`
      if [ -z "$OBSAHSUBORU" ]; then
        echo "Log neobsahuje ziadny subory v $AKTUALNYADRESAR."
        exit 1
      fi
      SUBOR=$( echo "$OBSAHSUBORU" | \
      {
        VYHOVUJUCISUBOR="empty"
        NAJVIACSPUSTENY=0
        while read riadok
        do
          POCETSPUSTENI=` echo "$riadok" | cut -d " " -f5`
          #      GOODDATE   <   DATE
          if [ "$NAJVIACSPUSTENY" -lt "$POCETSPUSTENI" ]; then
            NAJVIACSPUSTENY="$POCETSPUSTENI"
            VYHOVUJUCISUBOR=`echo "$riadok" | cut -d " " -f1`
          fi
          #statements
        done

        if [ "$VYHOVUJUCISUBOR" = "empty" ]; then
          echo "0"
        else
          echo "$VYHOVUJUCISUBOR"
        fi
      })

      if [ "$SUBOR" = 0 ]; then
        echo "Ziadny vyhovujuci subor."
        exit 1
      fi

      #Â nazov programu + nazov suboru na spustenie
      $OTVARANIE_SUBORU "$SUBOR"
      if [ ! "$?" = 0 ]; then
        echo "Problem pri otvarani suboru."
        exit 1
      fi

      DATUM=`date +%Y.%m.%d`
      CAS=`date +%H:%M:%S`
      OBSAHSUBORU=` cat $WEDI_RC | grep "$SUBOR $AKTUALNYADRESAR/ "`
      if [ ! -z "$OBSAHSUBORU" ]; then
        POCETOTVORENI=`echo "$OBSAHSUBORU" | cut -d " " -f6`
        POCETOTVORENI=$(($POCETOTVORENI+1))
        LOGRIADKU=" $SUBOR $AKTUALNYADRESAR/ $DATUM $CAS $POCETOTVORENI"
        OBSAHSUBORU=`cat $WEDI_RC | grep -v "$SUBOR $AKTUALNYADRESAR/ "`
        echo "$OBSAHSUBORU" > $WEDI_RC
        echo "$LOGRIADKU" >> $WEDI_RC
      else
        LOGRIADKU=" $SUBOR $AKTUALNYADRESAR/ $DATUM $CAS 1"
        echo "$LOGRIADKU" >> $WEDI_RC
      fi
      exit $?
    fi
  elif [ "$1" = "-a" ]; then
    AKTUALNYADRESAR=`pwd`
    DATUMNAPOROVNANIE=`echo "$2" | cut -d "-" -f1`
    DATUMNAPOROVNANIE=$DATUMNAPOROVNANIE`echo "$2" | cut -d "-" -f2`
    DATUMNAPOROVNANIE=$DATUMNAPOROVNANIE`echo "$2" | cut -d "-" -f3`
    OBSAHSUBORU=`cat $WEDI_RC | grep "$AKTUALNYADRESAR/ "`
    if [ -z "$OBSAHSUBORU" ]; then
      echo "Log neobsahuje subory z adresara $AKTUALNYADRESAR/"
      exit 1
    fi
    SUBORY=$( echo "$OBSAHSUBORU" | \
    {
      while read riadok
      do
        DATUM=`echo $riadok | cut -d " " -f3 | cut -d "." -f1`    #Â roky
        DATUM=$DATUM`echo $riadok | cut -d " " -f3 | cut -d "." -f2` #Â mesiace
        DATUM=$DATUM`echo $riadok | cut -d " " -f3 | cut -d "." -f3` #Â dni
        #      GOODDATE   <   DATE
        if [ "$DATUMNAPOROVNANIE" -lt "$DATUM" ]; then
          echo .`echo "$riadok" | cut -d " " -f1`
        fi
        #statements
      done
    })

    if [ -z "$SUBORY" ]; then
      echo "Nic na vypisanie."
      exit 1
    fi

    echo "$SUBORY"
    exit $?
  elif [ "$1" = "-b" ]; then
    AKTUALNYADRESAR=`pwd`
    DATUMNAPOROVNANIE=`echo "$2" | cut -d "-" -f1`
    DATUMNAPOROVNANIE=$DATUMNAPOROVNANIE`echo "$2" | cut -d "-" -f2`
    DATUMNAPOROVNANIE=$DATUMNAPOROVNANIE`echo "$2" | cut -d "-" -f3`
    OBSAHSUBORU=`cat $WEDI_RC | grep "$AKTUALNYADRESAR/ "`
    if [ -z "$OBSAHSUBORU" ]; then
      echo "Subor neobsahuje subory v $AKTUALNYADRESAR."
      exit 1
    fi
    SUBORY=$( echo "$OBSAHSUBORU" | \
    {
      while read riadok
      do
        DATUM=`echo $riadok | cut -d " " -f3 | cut -d "." -f1`    #Â roky
        DATUM=$DATUM`echo $riadok | cut -d " " -f3 | cut -d "." -f2` #Â mesiace
        DATUM=$DATUM`echo $riadok | cut -d " " -f3 | cut -d "." -f3` #Â dni
        #      GOODDATE   <   DATE
        if [ "$DATUM" -lt "$DATUMNAPOROVNANIE" ]; then
          echo .`echo "$riadok" | cut -d " " -f1`
        fi
        #statements
      done
    })

    if [ -z "$SUBORY" ]; then
      echo "Nic na vypisanie."
      exit 1
    fi

    echo "$SUBORY"
    exit $?
  fi
fi

if [ "$#" = "3" ]; then
  if [ -d "$3" ]; then
    if [ "$1" = "-a" ]; then
      AKTUALNYADRESAR=`readlink -f "$3"`
      DATUMNAPOROVNANIE=`echo "$2" | cut -d "-" -f1`
      DATUMNAPOROVNANIE=$DATUMNAPOROVNANIE`echo "$2" | cut -d "-" -f2`
      DATUMNAPOROVNANIE=$DATUMNAPOROVNANIE`echo "$2" | cut -d "-" -f3`
      OBSAHSUBORU=`cat $WEDI_RC | grep "$AKTUALNYADRESAR/ "`
      if [ -z "$OBSAHSUBORU" ]; then
          echo "Log neobsahuje subory v $AKTUALNYADRESAR."
          exit 1
      fi
      SUBORY=$( echo "$OBSAHSUBORU" | \
      {
        while read riadok
        do
          DATUM=`echo $riadok | cut -d " " -f3 | cut -d "." -f1`    #Â roky
          DATUM=$DATUM`echo $riadok | cut -d " " -f3 | cut -d "." -f2` #Â mesiace
          DATUM=$DATUM`echo $riadok | cut -d " " -f3 | cut -d "." -f3` #Â dni
          #      GOODDATE   <   DATE
          if [ "$DATUMNAPOROVNANIE" -lt "$DATUM" ]; then
            echo .`echo "$riadok" | cut -d " " -f1`
          fi
          #statements
        done
      })

      if [ -z "$SUBORY" ]; then
        echo "Nic na vypisanie."
        exit 1
      fi

      echo "$SUBORY"
      exit $?
    elif [ "$1" = "-b" ]; then
      AKTUALNYADRESAR=`readlink -f "$3"`
      DATUMNAPOROVNANIE=`echo "$2" | cut -d "-" -f1`
      DATUMNAPOROVNANIE=$DATUMNAPOROVNANIE`echo "$2" | cut -d "-" -f2`
      DATUMNAPOROVNANIE=$DATUMNAPOROVNANIE`echo "$2" | cut -d "-" -f3`
      OBSAHSUBORU=`cat $WEDI_RC | grep "$AKTUALNYADRESAR/ "`
      if [ -z "$OBSAHSUBORU" ]; then
        echo "Log neobsahuje subory v $AKTUALNYADRESAR."
        exit 1
      fi
      SUBORY=$( echo "$OBSAHSUBORU" | \
      {
        while read riadok
        do
          DATUM=`echo $riadok | cut -d " " -f3 | cut -d "." -f1`    #Â roky
          DATUM=$DATUM`echo $riadok | cut -d " " -f3 | cut -d "." -f2` #Â mesiace
          DATUM=$DATUM`echo $riadok | cut -d " " -f3 | cut -d "." -f3` #Â dni
          #      GOODDATE   <   DATE
          if [ "$DATUM" -lt "$DATUMNAPOROVNANIE" ]; then
            echo .`echo "$riadok" | cut -d " " -f1`
          fi
          #statements
        done
      })

      if [ -z "$SUBORY" ]; then
        echo "Nic na vypisanie."
        exit 1
      fi

      echo "$SUBORY"
      exit $?
    fi
    #statements
  fi
  echo "Zle zadane argumenty"
  exit 1
fi

if [ "$#" -gt 3 ]; then
  echo "Zle zadane argumenty"
fi