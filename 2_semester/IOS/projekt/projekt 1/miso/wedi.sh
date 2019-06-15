#!/bin/sh

POSIXLY_CORRECT=yes


if [ ! $WEDI_RC ]; then #kontrola stringu ci nie je prazdna

  echo "Premenna WEDI_RC nie je nastavena "
  exit 1
fi

if [ ! -e "$WEDI_RC" ]; then
  #statements
  WEDI_RC_CESTA=$(echo "$WEDI_RC" | rev | cut -d '/' | rev)
  WEDI_RC_CESTA=$(echo "$WEDI_RC" | grep  -Po ".*?(?=$WEDI_RC_CESTA)")
  if [ ! -e "$WEDI_RC_CESTA" ]; then
    mkdir -p "$WEDI_RC_CESTA"
  fi
  touch "$WEDI_RC"
fi

# 0 argumentov
# pwd, posledne editovany subor
if [ "$#" = "0" ]; then
  # Vyhladavanie vyhovujuceho suboru
  ADRESAR=$(pwd) #cesta
  VYCUC=$(cat "$WEDI_RC" | grep "$ADRESAR/ ") #vypis obsahu ktore potrebujem
  if [ -z "$VYCUC" ]; then
    echo "Nenasiel som v subore wedirc ziadne subory."
    exit 1
  fi
  FILE=$( echo "$VYCUC" | \
  {
    GOODFILE="empty"
    GOODDATE=0
    while read line
    do
      DATE=$(echo "$line" | cut -d ' ' -f1 | cut -d '-' -f1)$(echo "$line" | cut -d ' ' -f1 | cut -d '-' -f2)$(echo "$line" | cut -d ' ' -f1 | cut -d '-' -f3)$(echo "$line" | cut -d ' ' -f2 | cut -d ':' -f1)$(echo "$line" | cut -d ' ' -f2 | cut -d ':' -f2)$(echo "$line" | cut -d ' ' -f2 | cut -d ':' -f3)
      #      GOODDATE   <   DATE  cas a datum
      if [ "$GOODDATE" -lt "$DATE" ]; then
        GOODDATE="$DATE"
        GOODFILE=$(echo "$line" | cut -d ' ' -f4) #ulozi mi nazov suboru
      fi

    done
    echo "$GOODFILE"

  })

  echo "$FILE"

  if [ $EDITOR ]; then # OTvaranie suboru

    $EDITOR "$FILE"
  elif [ $VISUAL ]; then

    $VISUAL "$FILE"
  else
    vi "$FILE"
  fi

  DATE="$(date +%Y-%m-%d) $(date +%H:%M:%S)" # formatovanie datumu a casu
  TIMESFILEOPEN=$(cat "$WEDI_RC" | grep "$ADRESAR/ $FILE" | cut -d ' ' -f5)
  #vyber zo suboru ktory porovnava cestu a nazov
  echo "$TIMESFILEOPEN"
  exit 0
  if [ -z "$TIMESFILEOPEN" ]; then
    # nenasli sme nic appendni na koniec
    STRINGTOUPLOAD="1 $ADRESAR/ $FILE $DATE"
    echo "$STRINGTOUPLOAD" >> $WEDI_RC
  else
    # updatuje
    TIMESFILEOPEN=$(($TIMESFILEOPEN+1))
    WEDI_RCCONTENT=$(cat "$WEDI_RC" | grep -v "$ADRESAR/ $FILE")
    # obsahuje vsetko okrem nasho riadku ktory updatujeme
    echo "$WEDI_RCCONTENT" > $WEDI_RC  #prepise
    STRINGTOUPLOAD="$TIMESFILEOPEN $ADRESAR/ $FILE $DATE"
    echo "$STRINGTOUPLOAD" >> $WEDI_RC #da na koniec
  fi
  exit 0

fi

if [ "$#" = 1 ]; then


  if [ "$1" = "-m" ]; then
    ADRESAR=$(pwd)
    VYCUC=$(cat "$WEDI_RC" | grep "$ADRESAR/ ")
    FILE=$(echo "$VYCUC" | \
    {
      GOODFILE="empty"
      TIMESOPENED=0
      while read line
      do
        TIMESOPENEDLINE=$(echo "$line" | cut -d ' ' -f2)
        if [ "$TIMESOPENED" -lt "$TIMESOPENEDLINE" ]; then
          TIMESOPENED=$TIMESOPENEDLINE
          GOODFILE=$(echo "$line" | cut -d ' ' -f4)
          #statements

        fi
        #statements
      done
      echo "$GOODFILE"

    })
  if [ $EDITOR ]; then # OTvaranie suboru

    $EDITOR "$FILE"
  elif [ $VISUAL ]; then

    $VISUAL "$FILE"
  else
    vi "$FILE"
  fi

  DATE="$(date +%Y-%m-%d) $(date +%H:%M:%S)" # formatovanie datumu a casu
  TIMESFILEOPEN=$(cat "$WEDI_RC" | grep "$ADRESAR/ $FILE" | cut -d ' ' -f2)
  #vyber zo suboru ktory porovnava cestu a nazov
  if [ -z "$TIMESFILEOPEN" ]; then
    # nenasli sme nic appendni na koniec
    STRINGTOUPLOAD="1 $ADRESAR/ $FILE $DATE"
    echo "$STRINGTOUPLOAD" >> $WEDI_RC
  else
    # updatuje
    TIMESFILEOPEN=$(($TIMESFILEOPEN+1))
    WEDI_RCCONTENT=$(cat "$WEDI_RC" | grep -v "$ADRESAR/ $FILE")
    # obsahuje vsetko okrem nasho riadku ktory updatujeme
    echo "$WEDI_RCCONTENT" > $WEDI_RC #prepise
    STRINGTOUPLOAD="$TIMESFILEOPEN $ADRESAR/ $FILE $DATE"
    echo "$STRINGTOUPLOAD" >> $WEDI_RC #da na koniec
  fi

  exit 0
  fi

    if [ "$1" = "-l" ]; then
    ADRESAR=$(pwd)/
    VYCUC=$(cat "$WEDI_RC" | grep "$ADRESAR ")
    SUBORY=$( echo "$VYCUC" |  \
    {
      while read line
      do
        echo .$( echo "$line" | cut -d ' ' -f4)
      done
    })
    echo "$SUBORY"
    exit 0
  fi

  if [ -d "$1" ]; then
    ADRESAR=$(readlink -f "$1")
    echo "$ADRESAR"

    VYCUC=$(cat "$WEDI_RC" | grep "$ADRESAR/ ")
    FILE=$( echo "$VYCUC" | \
    {
      GOODFILE="empty"
      GOODDATE=0
      while read line
      do
        DATE=$(echo "$line" | cut -d ' ' -f1 | cut -d '-' -f1)$(echo "$line" | cut -d ' ' -f1 | cut -d '-' -f2)$(echo "$line" | cut -d ' ' -f1 | cut -d '-' -f3)$(echo "$line" | cut -d ' ' -f2 | cut -d ':' -f1)$(echo "$line" | cut -d ' ' -f2 | cut -d ':' -f2)$(echo "$line" | cut -d ' ' -f2 | cut -d ':' -f3)
        #      GOODDATE   <   DATE
        if [ "$GOODDATE" -lt "$DATE" ]; then
          GOODDATE="$DATE"
          GOODFILE=$(echo "$line" | cut -d ' ' -f4)
        fi

      done
      echo "$GOODFILE"

    })

    # OTvaranie suboru
    if [ $EDITOR ]; then

      $EDITOR "$FILE"
    elif [ $VISUAL ]; then

      $VISUAL "$FILE"
    else
      vi "$FILE"
    fi

    DATE=$("date +%Y-%m-%d")$("date +%H:%M:%S")
    TIMESFILEOPEN=$(cat "$WEDI_RC" | grep "$ADRESAR/ $FILE" | cut -d ' ' -f2)
    if [ -z "$TIMESFILEOPEN" ]; then
      # nenasli sme nic appendni na koniec
      STRINGTOUPLOAD="1 $ADRESAR/ $FILE $DATE"
      echo "$STRINGTOUPLOAD" >> $WEDI_RC
    else
      TIMESFILEOPEN=$(($TIMESFILEOPEN+1))
      WEDI_RCCONTENT=$(cat "$WEDI_RC" | grep -v "$ADRESAR/ $FILE")
      echo "$WEDI_RCCONTENT" > $WEDI_RC
      STRINGTOUPLOAD="$TIMESFILEOPEN $ADRESAR/ $FILE $DATE"
      echo "$STRINGTOUPLOAD" >> $WEDI_RC
    fi
    exit 0
  fi

  if [ -f "$1" ]; then
    ADRESAR=$(readlink -f "$1")
    if [ $EDITOR ]; then

      $EDITOR "$FILE"
    elif [ $VISUAL ]; then

      $VISUAL "$FILE"
    else
      vi "$FILE"
    fi

    FILE=$(echo "$ADRESAR" | rev | cut -d '/' -f1 | rev)
    ADRESAR=$( echo "$ADRESAR" | grep -Po ".*?(?=$FILE)")

    DATE="$(date +%Y-%m-%d) $(date +%H:%M:%S)"
    TIMESFILEOPEN=$(cat "$WEDI_RC" | grep "$ADRESAR $FILE" | cut -d ' ' -f2)
    if [ -z "$TIMESFILEOPEN" ]; then
      # nenasli sme nic appendni na koniec
      STRINGTOUPLOAD="1 $ADRESAR $FILE $DATE"
      echo "$STRINGTOUPLOAD" >> $WEDI_RC
    else
      TIMESFILEOPEN=$(($TIMESFILEOPEN+1))
      WEDI_RCCONTENT=$(cat "$WEDI_RC" | grep -v "$ADRESAR $FILE")
      echo "$WEDI_RCCONTENT" > $WEDI_RC
      STRINGTOUPLOAD="$TIMESFILEOPEN $ADRESAR $FILE $DATE"
      echo "$STRINGTOUPLOAD" >> $WEDI_RC
    fi
    exit 0

  fi
fi

if  [ "$#" = "2" ]; then
  #statements
  if [ "$1" = "-m" ]; then
    if [ -e "$2" ]; then
      ADRESAR=$(readlink -f "$2")
      VYCUC=$(cat "$WEDI_RC" | grep "$ADRESAR/ ")
      FILE=$(echo "$VYCUC" | \
      {
        GOODFILE="empty"
        TIMESOPENED=0
        while read line
        do
          TIMESOPENEDLINE=$(echo "$line" | cut -d ' ' -f2)
          if [ "$TIMESOPENED" -lt "$TIMESOPENEDLINE" ]; then
            TIMESOPENED=$TIMESOPENEDLINE
            GOODFILE=$(echo "$line" | cut -d ' ' -f4)
            #statements

          fi
          #statements
        done
        echo "$GOODFILE"

      })
    if [ $EDITOR ]; then # OTvaranie suboru

      $EDITOR "$FILE"
    elif [ $VISUAL ]; then

      $VISUAL "$FILE"
    else
      vi "$FILE"
    fi

    DATE="$(date +%Y-%m-%d) $(date +%H:%M:%S)" # formatovanie datumu a casu
    TIMESFILEOPEN=$(cat "$WEDI_RC" | grep "$ADRESAR/ $FILE" | cut -d ' ' -f2)
    #vyber zo suboru ktory porovnava cestu a nazov
    if [ -z "$TIMESFILEOPEN" ]; then
      # nenasli sme nic appendni na koniec
      STRINGTOUPLOAD="1 $ADRESAR/ $FILE $DATE"
      echo "$STRINGTOUPLOAD" >> $WEDI_RC
    else
      # updatuje
      TIMESFILEOPEN=$(($TIMESFILEOPEN+1))
      WEDI_RCCONTENT=$(cat "$WEDI_RC" | grep -v "$ADRESAR/ $FILE")
      # obsahuje vsetko okrem nasho riadku ktory updatujeme
      echo "$WEDI_RCCONTENT" > $WEDI_RC #prepise
      STRINGTOUPLOAD="$TIMESFILEOPEN $ADRESAR/ $FILE $DATE"
      echo "$STRINGTOUPLOAD" >> $WEDI_RC #da na koniec
    fi

    exit 0
      #statements
    fi
  fi

  if [ "$1" = "-l" ]; then
    if [ -e "$2" ]; then
      ADRESAR=$(readlink -f "$2")/
      VYCUC=$(cat "$WEDI_RC" | grep "$ADRESAR ")
      SUBORY=$( echo "$VYCUC" |  \
      {
        while read line
        do
          echo .$( echo "$line" | cut -d ' ' -f4)
        done
      })
      echo "$SUBORY"
      exit 0
    fi
  fi

  if [ "$1" = "-a" ]; then
    ADRESAR=$(pwd)/
    VYCUC=$(cat "$WEDI_RC" | grep "$ADRESAR ")
    GOODDATE=$(echo "$2" | cut -d '-' -f1)$(echo "$2" | cut -d '-' -f2)$(echo "$2" | cut -d '-' -f3)
    SUBORY=$( echo "$VYCUC" |  \
    {
      while read line
      do
        LINEDATE=$(echo "$line" | cut -d ' ' -f1 | cut -d '-' -f1)$(echo "$line" | cut -d ' ' -f1 | cut -d '-' -f2)$(echo "$line" | cut -d ' ' -f1 | cut -d '-' -f3)
        if [ "$GOODDATE" -lt "$LINEDATE"  ]; then
          echo ".$(echo "$line" | cut -d ' ' -f4)"

        fi
      done
    })
    if [ -z "$SUBORY" ]; then
      echo "Ziadne subory na vypis po $2"
      exit 1
    fi
    echo "$SUBORY"
    exit 0
    #statements
  fi

  if [ "$1" = "-b" ]; then
    ADRESAR=$(pwd)/
    VYCUC=$(cat "$WEDI_RC" | grep "$ADRESAR ")
    GOODDATE=$(echo "$2" | cut -d '-' -f1)$(echo "$2" | cut -d '-' -f2)$(echo "$2" | cut -d '-' -f3)
    SUBORY=$( echo "$VYCUC" |  \
    {
      while read line
      do
        LINEDATE=$(echo "$line" | cut -d ' ' -f1 | cut -d '-' -f1)$(echo "$line" | cut -d ' ' -f1 | cut -d '-' -f2)$(echo "$line" | cut -d ' ' -f1 | cut -d '-' -f3)
        if [ "$GOODDATE" -gt "$LINEDATE"  ]; then
          echo ".$(echo "$line" | cut -d ' ' -f4)"

        fi
      done
    })
    if [ -z "$SUBORY" ]; then
      echo "Ziadne subory na vypis po $2"
      exit 1
    fi
    echo "$SUBORY"
    exit 0
  fi
fi

if [ "$#" = 3 ]; then
  if [ "$1" = "-a" ]; then
    if  [ ! -e "$3" ]; then
      echo "ADRESAR $3 neexistuje"
      exit 1
    fi

    ADRESAR=$(readlink -f "$3")/
    VYCUC=$(cat "$WEDI_RC" | grep "$ADRESAR ")
    GOODDATE=$(echo "$2" | cut -d '-' -f1)$(echo "$2" | cut -d '-' -f2)$(echo "$2" | cut -d '-' -f3)
    SUBORY=$( echo "$VYCUC" |  \
    {
      while read line
      do
        LINEDATE=$(echo "$line" | cut -d ' ' -f1 | cut -d '-' -f1)$(echo "$line" | cut -d ' ' -f1 | cut -d '-' -f2)$(echo "$line" | cut -d ' ' -f1 | cut -d '-' -f3)
        if [ "$GOODDATE" -lt "$LINEDATE"  ]; then
          echo ".$(echo "$line" | cut -d ' ' -f4)"

        fi
      done
    })
    if [ -z "$SUBORY" ]; then
      echo "Ziadne subory na vypis po $2"
      exit 1
    fi
    echo "$SUBORY"
    exit 0

  fi

  if [ "$1" = "-b" ]; then
    if [ ! -e "$3" ]; then
      echo "ADRESAR $3 neexistuje "
      exit 1
    fi

    ADRESAR=$(readlink -f "$3")/
    VYCUC=$(cat "$WEDI_RC" | grep "$ADRESAR ")
    GOODDATE=$(echo "$2" | cut -d '-' -f1)$(echo "$2" | cut -d '-' -f2)$(echo "$2" | cut -d '-' -f3)
    SUBORY=$( echo "$VYCUC" |  \
    {
      while read line
      do
        LINEDATE=$(echo "$line" | cut -d ' ' -f1 | cut -d '-' -f1)$(echo "$line" | cut -d ' ' -f1 | cut -d '-' -f2)$(echo "$line" | cut -d ' ' -f1 | cut -d '-' -f3)
        if [ "$GOODDATE" -gt "$LINEDATE"  ]; then
          echo ".$(echo "$line" | cut -d ' ' -f4)"

        fi
      done
    })
    if [ -z "$SUBORY" ]; then
      echo "Ziadne subory na vypis po $2"
      exit 1
    fi
    echo "$SUBORY"
    exit 0
  fi

fi

if [ "$#" -gt 3 ]; then
  echo "zle zadane argumenty"
  exit 1
fi
