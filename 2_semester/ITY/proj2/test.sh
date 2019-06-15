#!/bin/sh

POSIXLY_CORRECT=yes


if [ ! $WEDI_RC ]; then
  #statements
  echo "WEDI_RC variable is not set"
  exit 1
fi
WEDI_RC=$(readlink -f "$WEDI_RC")

if [ ! -e "$WEDI_RC/" ]; then
  #statements
  mkdir -p "$WEDI_RC"
fi

if [ ! -e "$WEDI_RC/WEDI_RC" ]; then
  #statements
  touch "$WEDI_RC/WEDI_RC"
fi
#echo "WEDI_RC variable is set"
WEDI_RC="$WEDI_RC/WEDI_RC"

# 0 argumentov
# pwd, posledne editovany subor
if [ "$#" = "0" ]; then
  # Vyhladavanie vyhovujuceho suboru
  WORKINGDIRECTORY=`pwd`
  VYCUC=`cat "$WEDI_RC" | grep "$WORKINGDIRECTORY/ "`
  FILE=$( echo "$VYCUC" | \
  {
    GOODFILE="empty"
    GOODDATE=0
    while read line
    do
      DATE=$(echo "$line" | cut -d ' ' -f4 | cut -d '-' -f1)`echo "$line" | cut -d ' ' -f4 | cut -d '-' -f2``echo "$line" | cut -d ' ' -f4 | cut -d '-' -f3``echo "$line" | cut -d ' ' -f5 | cut -d ':' -f1``echo "$line" | cut -d ' ' -f5 | cut -d ':' -f2``echo "$line" | cut -d ' ' -f5 | cut -d ':' -f3`
      #      GOODDATE   <   DATE
      if [ "$GOODDATE" -lt "$DATE" ]; then
        GOODDATE="$DATE"
        GOODFILE=`echo "$line" | cut -d ' ' -f3`
      fi
      #statements
    done
    echo "$GOODFILE"

  })

  # OTvaranie suboru
  if [ $EDITOR ]; then
    #statements
    $EDITOR "$FILE"
  elif [ $VISUAL ]; then
    #statements
    $VISUAL "$FILE"
  else
    vi "$FILE"
  fi

  # date
  DATE="`date +%Y-%m-%d` `date +%H:%M:%S`"
  TIMESFILEOPEN=`cat "$WEDI_RC" | grep "$WORKINGDIRECTORY/ $FILE" | cut -d ' ' -f1`
  if [ -z "$TIMESFILEOPEN" ]; then
    # nenasli sme nic appendni na koniec
    STRINGTOUPLOAD="1 $WORKINGDIRECTORY/ $FILE $DATE"
    echo "$STRINGTOUPLOAD" >> $WEDI_RC
  else
    TIMESFILEOPEN=$(($TIMESFILEOPEN+1))
    WEDI_RCCONTENT=`cat "$WEDI_RC" | grep -v "$WORKINGDIRECTORY/ $FILE"`
    echo "$WEDI_RCCONTENT" > $WEDI_RC
    STRINGTOUPLOAD="$TIMESFILEOPEN $WORKINGDIRECTORY/ $FILE $DATE"
    echo "$STRINGTOUPLOAD" >> $WEDI_RC
  fi
  exit 0
fi

if [ "$#" = "1" ]; then
  # -m
  if [ "$1" = "-m" ]; then
    #statements
    echo "Otvaram najviac editovany subor v tomto directory"
    WORKINGDIRECTORY=$(pwd)
    VYCUC=$(cat "$WEDI_RC" | grep "$WORKINGDIRECTORY/ ")
    FILE=$( echo "$VYCUC" | \
    {
      GOODFILE="empty"
      TIMESOPENED=0
      while read line
      do
        TIMESOPENEDLINE=$(echo "$line" | cut -d ' ' -f1)
        if [ "$TIMESOPENED" -lt "$TIMESOPENEDLINE" ]; then
          TIMESOPENED=$TIMESOPENEDLINE
          GOODFILE=$(echo "$line" | cut -d ' ' -f3)
        fi
      done
      echo "$GOODFILE"
    })
    echo "$FILE"

    # OTvaranie suboru
    if [ $EDITOR ]; then
      #statements
      $EDITOR "$FILE"
    elif [ $VISUAL ]; then
      #statements
      $VISUAL "$FILE"
    else
      vi "$FILE"
    fi

    DATE="`date +%Y-%m-%d` `date +%H:%M:%S`"
    TIMESFILEOPEN=`cat "$WEDI_RC" | grep "$WORKINGDIRECTORY/ $FILE" | cut -d ' ' -f1`
    if [ -z "$TIMESFILEOPEN" ]; then
      # nenasli sme nic appendni na koniec
      STRINGTOUPLOAD="1 $WORKINGDIRECTORY/ $FILE $DATE"
      echo "$STRINGTOUPLOAD" >> $WEDI_RC
    else
      TIMESFILEOPEN=$(($TIMESFILEOPEN+1))
      WEDI_RCCONTENT=`cat "$WEDI_RC" | grep -v "$WORKINGDIRECTORY/ $FILE"`
      echo "$WEDI_RCCONTENT" > $WEDI_RC
      STRINGTOUPLOAD="$TIMESFILEOPEN $WORKINGDIRECTORY/ $FILE $DATE"
      echo "$STRINGTOUPLOAD" >> $WEDI_RC
    fi

    exit 0
  fi

  if [ "$1" = "-l" ]; then
    #statements
    WORKINGDIRECTORY=$(pwd)/
    VYCUC=$(cat "$WEDI_RC" | grep "$WORKINGDIRECTORY ")
    SUBORY=$( echo "$VYCUC" | \
    {
      while read line
      do
        echo .`echo "$line" | cut -d ' ' -f3`
      done

    })
    echo "$SUBORY"
    exit 0
  fi

  if [ -d "$1" ]; then
    WORKINGDIRECTORY=$(readlink -f "$1")

    VYCUC=`cat "$WEDI_RC" | grep "$WORKINGDIRECTORY/ "`
    FILE=$( echo "$VYCUC" | \
    {
      GOODFILE="empty"
      GOODDATE=0
      while read line
      do
        DATE=`echo "$line" | cut -d ' ' -f4 | cut -d '-' -f1``echo "$line" | cut -d ' ' -f4 | cut -d '-' -f2``echo "$line" | cut -d ' ' -f4 | cut -d '-' -f3``echo "$line" | cut -d ' ' -f5 | cut -d ':' -f1``echo "$line" | cut -d ' ' -f5 | cut -d ':' -f2``echo "$line" | cut -d ' ' -f5 | cut -d ':' -f3`
        #      GOODDATE   <   DATE
        if [ "$GOODDATE" -lt "$DATE" ]; then
          GOODDATE="$DATE"
          GOODFILE=`echo "$line" | cut -d ' ' -f3`
        fi
        #statements
      done
      echo "$GOODFILE"

    })
    echo "$FILE"

    # OTvaranie suboru
    if [ $EDITOR ]; then
      #statements
      $EDITOR "$FILE"
    elif [ $VISUAL ]; then
      #statements
      $VISUAL "$FILE"
    else
      vi "$FILE"
    fi

    DATE="`date +%Y-%m-%d` `date +%H:%M:%S`"
    TIMESFILEOPEN=`cat "$WEDI_RC" | grep "$WORKINGDIRECTORY/ $FILE" | cut -d ' ' -f1`
    if [ -z "$TIMESFILEOPEN" ]; then
      # nenasli sme nic appendni na koniec
      STRINGTOUPLOAD="1 $WORKINGDIRECTORY/ $FILE $DATE"
      echo "$STRINGTOUPLOAD" >> $WEDI_RC
    else
      TIMESFILEOPEN=$(($TIMESFILEOPEN+1))
      WEDI_RCCONTENT=`cat "$WEDI_RC" | grep -v "$WORKINGDIRECTORY/ $FILE"`
      echo "$WEDI_RCCONTENT" > $WEDI_RC
      STRINGTOUPLOAD="$TIMESFILEOPEN $WORKINGDIRECTORY/ $FILE $DATE"
      echo "$STRINGTOUPLOAD" >> $WEDI_RC
    fi
    exit 0

  fi

  if [ -f "$1" ]; then
    WORKINGDIRECTORY=$(readlink -f "$1")

    # OTvaranie suboru
    if [ $EDITOR ]; then
      #statements
      $EDITOR "$FILE"
    elif [ $VISUAL ]; then
      #statements
      $VISUAL "$FILE"
    else
      vi "$FILE"
    fi

    FILE=$(echo "$WORKINGDIRECTORY" | rev | cut -d '/' -f1 | rev)
    WORKINGDIRECTORY=$( echo "$WORKINGDIRECTORY" | grep -Po ".*?(?=$FILE)")

    DATE="`date +%Y-%m-%d` `date +%H:%M:%S`"
    TIMESFILEOPEN=`cat "$WEDI_RC" | grep "$WORKINGDIRECTORY $FILE" | cut -d ' ' -f1`
    if [ -z "$TIMESFILEOPEN" ]; then
      # nenasli sme nic appendni na koniec
      STRINGTOUPLOAD="1 $WORKINGDIRECTORY $FILE $DATE"
      echo "$STRINGTOUPLOAD" >> $WEDI_RC
    else
      #echo "$TIMESFILEOPEN"
      TIMESFILEOPEN=$(($TIMESFILEOPEN+1))
      WEDI_RCCONTENT=`cat "$WEDI_RC" | grep -v "$WORKINGDIRECTORY $FILE"`
      echo "$WEDI_RCCONTENT" > $WEDI_RC
      STRINGTOUPLOAD="$TIMESFILEOPEN $WORKINGDIRECTORY $FILE $DATE"
      echo "$STRINGTOUPLOAD" >> $WEDI_RC
    fi
    exit 0
  fi
fi

if [ "$#" = "2" ]; then
  #statements
  if [ "$1" = "-m" ]; then
    if [ -e "$2" ]; then
      WORKINGDIRECTORY=$(readlink -f "$2")

      VYCUC=$(cat "$WEDI_RC" | grep "$WORKINGDIRECTORY/ ")
      FILE=$( echo "$VYCUC" | \
      {
        GOODFILE="empty"
        TIMESOPENED=0
        while read line
        do
          TIMESOPENEDLINE=$(echo "$line" | cut -d ' ' -f1)
          if [ "$TIMESOPENED" -lt "$TIMESOPENEDLINE" ]; then
            TIMESOPENED=$TIMESOPENEDLINE
            GOODFILE=$(echo "$line" | cut -d ' ' -f3)
          fi
        done
        echo "$GOODFILE"
      })

      # OTvaranie suboru
      if [ $EDITOR ]; then
        #statements
        $EDITOR "$FILE"
      elif [ $VISUAL ]; then
        #statements
        $VISUAL "$FILE"
      else
        vi "$FILE"
      fi

      DATE="`date +%Y-%m-%d` `date +%H:%M:%S`"
      TIMESFILEOPEN=`cat "$WEDI_RC" | grep "$WORKINGDIRECTORY/ $FILE" | cut -d ' ' -f1`
      if [ -z "$TIMESFILEOPEN" ]; then
        # nenasli sme nic appendni na koniec
        STRINGTOUPLOAD="1 $WORKINGDIRECTORY/ $FILE $DATE"
        echo "$STRINGTOUPLOAD" >> $WEDI_RC
      else
        TIMESFILEOPEN=$(($TIMESFILEOPEN+1))
        WEDI_RCCONTENT=`cat "$WEDI_RC" | grep -v "$WORKINGDIRECTORY/ $FILE"`
        echo "$WEDI_RCCONTENT" > $WEDI_RC
        STRINGTOUPLOAD="$TIMESFILEOPEN $WORKINGDIRECTORY/ $FILE $DATE"
        echo "$STRINGTOUPLOAD" >> $WEDI_RC
      fi

      exit 0

    fi
  fi

  if [ "$1" = "-l" ]; then
    if [ -e "$2" ]; then
      #statements
      WORKINGDIRECTORY=$(readlink -f "$2")/
      VYCUC=$(cat "$WEDI_RC" | grep "$WORKINGDIRECTORY ")
      SUBORY=$( echo "$VYCUC" | \
      {
        while read line
        do
          echo .`echo "$line" | cut -d ' ' -f3`
        done
      })
      echo "$SUBORY"
      exit 0
    fi
  fi

  if [ "$1" = "-a" ]; then
    WORKINGDIRECTORY=$(pwd)/
    VYCUC=$(cat "$WEDI_RC" | grep "$WORKINGDIRECTORY ")
    GOODDATE=$(echo "$2" | cut -d '-' -f1)$(echo "$2" | cut -d '-' -f2)$(echo "$2" | cut -d '-' -f3)
    SUBORY=$( echo "$VYCUC" | \
    {
      while read line
      do
        LINEDATE=$(echo "$line" | cut -d ' ' -f4 | cut -d '-' -f1)$(echo "$line" | cut -d ' ' -f4 | cut -d '-' -f2)$(echo "$line" | cut -d ' ' -f4 | cut -d '-' -f3)
        if [ "$GOODDATE" -lt "$LINEDATE" ]; then
          echo ".$(echo "$line" | cut -d ' ' -f3)"
        fi
      done
    })

    if [ -z "$SUBORY" ]; then
      echo "Ziadne subory na vypisanie po $2"
      exit 1
    fi
    echo "$SUBORY"
    exit 0
  fi

  if [ "$1" = "-b" ]; then
    WORKINGDIRECTORY=$(pwd)/
    VYCUC=$(cat "$WEDI_RC" | grep "$WORKINGDIRECTORY ")
    GOODDATE=$(echo "$2" | cut -d '-' -f1)$(echo "$2" | cut -d '-' -f2)$(echo "$2" | cut -d '-' -f3)
    SUBORY=$( echo "$VYCUC" | \
    {
      while read line
      do
        LINEDATE=$(echo "$line" | cut -d ' ' -f4 | cut -d '-' -f1)$(echo "$line" | cut -d ' ' -f4 | cut -d '-' -f2)$(echo "$line" | cut -d ' ' -f4 | cut -d '-' -f3)
        if [ "$GOODDATE" -gt "$LINEDATE" ]; then
          echo ".$(echo "$line" | cut -d ' ' -f3)"
        fi
      done
    })

    if [ -z "$SUBORY" ]; then
      echo "Ziadne subory na vypisanie po $2"
      exit 1
    fi
    echo "$SUBORY"
    exit 0
  fi
fi

if [ "$#" = 3 ]; then
  if [ "$1" = "-a" ]; then
    if [ ! -e "$3" ]; then
      echo "Adresar $3 neexistuje"
      exit 1
    fi
    WORKINGDIRECTORY=$(readlink -f "$3")/
    VYCUC=$(cat "$WEDI_RC" | grep "$WORKINGDIRECTORY ")
    GOODDATE=$(echo "$2" | cut -d '-' -f1)$(echo "$2" | cut -d '-' -f2)$(echo "$2" | cut -d '-' -f3)
    SUBORY=$( echo "$VYCUC" | \
    {
      while read line
      do
        LINEDATE=$(echo "$line" | cut -d ' ' -f4 | cut -d '-' -f1)$(echo "$line" | cut -d ' ' -f4 | cut -d '-' -f2)$(echo "$line" | cut -d ' ' -f4 | cut -d '-' -f3)
        if [ "$GOODDATE" -lt "$LINEDATE" ]; then
          echo ".$(echo "$line" | cut -d ' ' -f3)"
        fi
      done
    })

    if [ -z "$SUBORY" ]; then
      echo "Ziadne subory na vypisanie po $2"
      exit 1
    fi
    echo "$SUBORY"
    exit 0
  fi

  if [ "$1" = "-b" ]; then
    if [ ! -e "$3" ]; then
      echo "Adresar $3 neexistuje."
      exit 1
    fi
    WORKINGDIRECTORY=$(readlink -f "$3")/
    VYCUC=$(cat "$WEDI_RC" | grep "$WORKINGDIRECTORY ")
    GOODDATE=$(echo "$2" | cut -d '-' -f1)$(echo "$2" | cut -d '-' -f2)$(echo "$2" | cut -d '-' -f3)
    SUBORY=$( echo "$VYCUC" | \
    {
      while read line
      do
        LINEDATE=$(echo "$line" | cut -d ' ' -f4 | cut -d '-' -f1)$(echo "$line" | cut -d ' ' -f4 | cut -d '-' -f2)$(echo "$line" | cut -d ' ' -f4 | cut -d '-' -f3)
        if [ "$GOODDATE" -gt "$LINEDATE" ]; then
          echo ".$(echo "$line" | cut -d ' ' -f3)"
        fi
      done
    })

    if [ -z "$SUBORY" ]; then
      echo "Ziadne subory na vypisanie po $2"
      exit 1
    fi
    echo "$SUBORY"
    exit 0
  fi
fi

if [ "$#" -gt 3 ]; then
  #statements
  echo "Zle zadane argumenty."
  exit 1
fi
