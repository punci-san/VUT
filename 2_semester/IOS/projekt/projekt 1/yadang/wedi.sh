#!/bin/bash
POSIXLY_CORRECT=yes
PROGRAM=""
if [ ! -z "$WEDI_RC" ]; then
  WEDI_RC=`readlink -f "$WEDI_RC"`
  if [ ! -e "$WEDI_RC" ]; then
    DIRECTORY=`echo "$WEDI_RC" | rev | cut -d '/' -f1 | rev`
    DIRECTORY=`echo "$WEDI_RC" | grep -Po ".*?(?=$DIRECTORY)"`
    mkdir -p "$DIRECTORY"
    touch $WEDI_RC
  fi
else
  echo "Variable doesn't exist."
  exit 1
fi

if [ ! -z "$EDITOR" ]; then
  PROGRAM="$EDITOR"
elif [ ! -z "$VISUAL" ]; then
  PROGRAM="$EDITOR"
else
  PROGRAM="vi"
fi

if [ -z "$1" ]; then
  DIRECTORY="$(pwd)"
  cat "$WEDI_RC"
  CONTENT=$(cat "$WEDI_RC" | grep "$DIRECTORY/ ")
  if [ -z "$CONTENT" ]; then
    echo "Unable to find any File in $DIRECTORY that exist in the LOG"
    exit 1
  fi

  FILE=$(echo "$CONTENT" | \
  {
    NEWEST_FILE_DATE=0
    NEWEST_FILE_NAME=0
    NEWEST_FILE_DIR=0
    OPEN_COUNT=0
  while read LINE
  do
    DATE=$(echo "$LINE" | cut -d " " -f4)
    DATE+=$(echo "$LINE" | cut -d " " -f5)
    DATE+=$(echo "$LINE" | cut -d " " -f6)
    DATE+=$(echo "$LINE" | cut -d " " -f7)
    DATE+=$(echo "$LINE" | cut -d " " -f8)
    DATE+=$(echo "$LINE" | cut -d " " -f9)
    if [ "$NEWEST_FILE_DATE" -lt "$DATE" ]; then
      NEWEST_FILE_DATE=$DATE
      OPEN_COUNT=$(echo "$LINE" | cut -d " " -f1)
      NEWEST_FILE_DIR=$(echo "$LINE" | cut -d " " -f2)
      NEWEST_FILE_NAME=$(echo "$LINE" | cut -d " " -f3)
    fi
  done
  echo "$OPEN_COUNT $NEWEST_FILE_DIR $NEWEST_FILE_NAME"
  })
  echo $FILE
  DATE=$(date +%Y" "%m" "%d" "%H" "%M" "%S)
  OPEN_COUNT=$(echo "$FILE" | cut -d " " -f1)
  FILE_NAME=$(echo "$FILE" | cut -d " " -f3)
  FILE_DIR=$(echo "$FILE" | cut -d " " -f2)
  $PROGRAM $FILE
  CONTENT=$(cat "$WEDI_RC")
  LOG_EDIT=$(echo "$CONTENT" | \
  {
  NEWEST_FILE_DIR=0
  NEWEST_FILE_NAME=0
  LINE_NUM=1
  while read LINE
    do
      NEWEST_FILE_DIR=$(echo "$LINE" | cut -d " " -f2)
      NEWEST_FILE_NAME=$(echo "$LINE" | cut -d " " -f3)
      if [[ ( "$FILE_NAME" = "$NEWEST_FILE_NAME" ) && ( "$FILE_DIR" = "$NEWEST_FILE_DIR" ) ]]; then
        sed -i "$LINE_NUM d" $WEDI_RC
      fi
      LINE_NUM=$(($LINE_NUM+1))
    done
    OPEN_COUNT=$((OPEN_COUNT+1))
    echo "$OPEN_COUNT $FILE_DIR $FILE_NAME $DATE" >> $WEDI_RC
  })
fi

if [[ ( "$1" = "-m" ) || ( "$1" = "-l" ) ]]; then
  ARG=$1

  if [ "$ARG" = "-m" ]; then
  if [ ! -e "$2" ]; then
    DIRECTORY="$(pwd)"
    CONTENT=$(cat "$WEDI_RC")
    if [ -z "$CONTENT" ]; then
      echo "Unable to find any File in $DIRECTORY that exist in the LOG"
      exit 1
    fi
    FILE=$(echo "$CONTENT" | grep "$DIRECTORY/ " | \
    {
      HI_OPEN_COUNT=0
      HI_FILE_NAME=0
      HI_FILE_DIR=0
      while read LINE
      do
        OPEN_COUNT=$(echo "$LINE" | cut -d " " -f1)
        FILE_DIR=$(echo "$LINE" | cut -d " " -f2)
        FILE_NAME=$(echo "$LINE" | cut -d " " -f3)
        if [ "$HI_OPEN_COUNT" -lt "$OPEN_COUNT" ]; then
          HI_OPEN_COUNT=$OPEN_COUNT
          HI_FILE_NAME=$FILE_NAME
          HI_FILE_DIR=$FILE_DIR
        fi
      done
      echo "$HI_OPEN_COUNT $HI_FILE_DIR $HI_FILE_NAME"
    })
    DATE=$(date +%Y" "%m" "%d" "%H" "%M" "%S)
    OPEN_COUNT=$(echo "$FILE" | cut -d " " -f1)
    FILE_NAME=$(echo "$FILE" | cut -d " " -f3)
    FILE_DIR=$(echo "$FILE" | cut -d " " -f2)
    $PROGRAM $FILE_NAME
    LOG_EDIT=$(echo "$CONTENT" | \
    {
      LINE_NUM=1
      while read LINE
      do
        EXIST_FILE_DIR=$(echo "$LINE" | cut -d " " -f2)
        EXIST_FILE_NAME=$(echo "$LINE" | cut -d " " -f3)
        if [[ ( "$FILE_NAME" = "$EXIST_FILE_NAME" ) && ( "$FILE_DIR" = "$EXIST_FILE_DIR" ) ]]; then
          sed -i "$LINE_NUM d" $WEDI_RC
        fi
        LINE_NUM=$(($LINE_NUM+1))
      done
      OPEN_COUNT=$((OPEN_COUNT+1))
      echo "$OPEN_COUNT $FILE_DIR $FILE_NAME $DATE" >> $WEDI_RC
    })
    exit 0
  fi

    if [ -e "$2" ]; then
      DIRECTORY=$2
      CONTENT=$(cat "$WEDI_RC")
      if [ -z "$CONTENT" ]; then
        echo "Unable to find any File in $DIRECTORY that exist in the LOG"
        exit 1
      fi
      FILE=$(echo "$CONTENT" | grep "$DIRECOTRY/ " | \
      {
        HI_OPEN_COUNT=0
        HI_FILE_NAME=0
        HI_FILE_DIR=0
        while read LINE
        do
          OPEN_COUNT=$(echo "$LINE" | cut -d " " -f1)
          FILE_DIR=$(echo "$LINE" | cut -d " " -f2)
          FILE_NAME=$(echo "$LINE" | cut -d " " -f3)
          if [ "$HI_OPEN_COUNT" -lt "$OPEN_COUNT" ]; then
            HI_OPEN_COUNT=$OPEN_COUNT
            HI_FILE_NAME=$FILE_NAME
            HI_FILE_DIR=$FILE_DIR
          fi
        done
        echo "$HI_OPEN_COUNT $HI_FILE_DIR $HI_FILE_NAME"
      })
      DATE=$(date +%Y" "%m" "%d" "%H" "%M" "%S)
      OPEN_COUNT=$(echo "$FILE" | cut -d " " -f1)
      FILE_NAME=$(echo "$FILE" | cut -d " " -f3)
      FILE_DIR=$(echo "$FILE" | cut -d " " -f2)
      START="$PROGRAM $DIRECTORY/$FILE_NAME"
      $START
      LOG_EDIT=$(echo "$CONTENT" | \
      {
        LINE_NUM=1
        while read LINE
        do
          EXIST_FILE_DIR=$(echo "$LINE" | cut -d " " -f2)
          EXIST_FILE_NAME=$(echo "$LINE" | cut -d " " -f3)
          if [[ ( "$FILE_NAME" = "$EXIST_FILE_NAME" ) && ( "$FILE_DIR" = "$EXIST_FILE_DIR" ) ]]; then
            sed -i "$LINE_NUM d" $WEDI_RC
          fi
          LINE_NUM=$(($LINE_NUM+1))
        done
        OPEN_COUNT=$((OPEN_COUNT+1))
        echo "$OPEN_COUNT $FILE_DIR $FILE_NAME $DATE" >> $WEDI_RC
      })
    fi
  fi

  if [ "$ARG" = "-l" ]; then

    if [ ! -e "$2" ]; then
      DIRECTORY="$(pwd)"
      CONTENT=$(cat "$WEDI_RC" | grep "$DIRECTORY/ ")
      if [ -z "$CONTENT" ]; then
        echo "Unable to find any File in $DIRECTORY that exist in the LOG"
        exit 1
      fi
      FILE=$(echo "$CONTENT" | \
      {
        while read LINE
        do
          echo .$(echo "$LINE" | cut -d " " -f3)
        done
      })
      echo $FILE
      exit 0
    fi

    if [ -e "$2" ]; then
      DIRECTORY=$2
      CONTENT=$(cat "$WEDI_RC" | grep "$DIRECTORY/ ")
      if [ -z "$CONTENT" ]; then
        echo "Unable to find any File in $DIRECTORY that exist in the LOG"
        echo "Don't put / sign in the end of the directory link"
        exit 1
      fi
      FILE=$(echo "$CONTENT" | \
      {
        while read LINE
        do
          echo $(echo "$LINE" | cut -d " " -f3)
        done
      })
      echo $FILE
      exit 0
    fi
  fi
fi
if [[ ( "$1" = "-b" ) || ( "$1" = "-a" ) ]]; then
  ARG=$1
  ARG_DATE=$2
  DIRECTORY="$(pwd)"
  GIVEN_DATE=$(echo "$ARG_DATE" | cut -d "-" -f1)
  GIVEN_DATE+=$(echo "$ARG_DATE" | cut -d "-" -f2)
  GIVEN_DATE+=$(echo "$ARG_DATE" | cut -d "-" -f3)
  if [ "$ARG" = "-b" ]; then
    if [ -z "$2" ]; then
      echo "No date given"
      exit 1
    fi
    if [ ! -e "$3" ]; then
      CONTENT=$(cat "$WEDI_RC" | grep "$DIRECTORY/ ")
      if [ -z "$CONTENT" ]; then
        echo "Unable to find any File in $DIRECTORY that exist in the LOG"
        echo "Don't put / sign in the end of the directory link"
        exit 1
      fi
      FILE=$(echo "$CONTENT" | \
      {
        while read LINE
        do
          DATE=$(echo "$LINE" | cut -d " " -f4)
          DATE+=$(echo "$LINE" | cut -d " " -f5)
          DATE+=$(echo "$LINE" | cut -d " " -f6)
          if [[ "$DATE" < "$GIVEN_DATE" ]]; then
            echo .$(echo "$LINE" | cut -d " " -f3)
          fi
        done
      })
      echo $FILE
      exit 0
    fi

    if [ -e "$3" ]; then
      DIRECTORY=$3
      CONTENT=$(cat "$WEDI_RC" | grep "$DIRECTORY/ ")
      if [ -z "$CONTENT" ]; then
        echo "Unable to find any File in $DIRECTORY that exist in the LOG"
        echo "Don't put / sign in the end of the directory link"
        exit 1
      fi
      FILE=$(echo "$CONTENT" | \
      {
        while read LINE
        do
          DATE=$(echo "$LINE" | cut -d " " -f4)
          DATE+=$(echo "$LINE" | cut -d " " -f5)
          DATE+=$(echo "$LINE" | cut -d " " -f6)
          if [[ "$DATE" < "$GIVEN_DATE" ]]; then
            echo .$(echo "$LINE" | cut -d " " -f3)
          fi
        done
      })
      echo $FILE
      exit 0
    fi
  fi

  if [ "$ARG" = "-a" ]; then
    if [ -z "$2" ]; then
      echo "No date given"
      exit 1
    fi
    if [ ! -e "$3" ]; then
      CONTENT=$(cat "$WEDI_RC" | grep "$DIRECTORY/ ")
      if [ -z "$CONTENT" ]; then
        echo "Unable to find any File in $DIRECTORY that exist in the LOG"
        echo "Don't put / sign in the end of the directory link"
        exit 1
      fi
      FILE=$(echo "$CONTENT" | \
      {
        while read LINE
        do
          DATE=$(echo "$LINE" | cut -d " " -f4)
          DATE+=$(echo "$LINE" | cut -d " " -f5)
          DATE+=$(echo "$LINE" | cut -d " " -f6)
          if [[ "$DATE" > "$GIVEN_DATE" ]]; then
            echo .$(echo "$LINE" | cut -d " " -f3)
          fi
        done
      })
      echo $FILE
      exit 0
    fi

    if [ -e "$3" ]; then
      DIRECTORY=$3
      CONTENT=$(cat "$WEDI_RC" | grep "$DIRECTORY/ ")
      if [ -z "$CONTENT" ]; then
        echo "Unable to find any File in $DIRECTORY that exist in the LOG"
        echo "Don't put / sign in the end of the directory link"
        exit 1
      fi
      FILE=$(echo "$CONTENT" | \
      {
        while read LINE
        do
          DATE=$(echo "$LINE" | cut -d " " -f4)
          DATE+=$(echo "$LINE" | cut -d " " -f5)
          DATE+=$(echo "$LINE" | cut -d " " -f6)
          if [[ "$DATE" > "$GIVEN_DATE" ]]; then
            echo .$(echo "$LINE" | cut -d " " -f3)
          fi
        done
      })
      echo $FILE
      exit 0
    fi
  fi
fi
if [ "$#" -eq 1 ]; then
  ARG=$1

  if [[ -f $ARG ]]; then
    DIRECTORY="$(pwd)"
    DATE=$(date +%Y" "%m" "%d" "%H" "%M" "%S)
    FILE_NAME=$ARG
    $PROGRAM $FILE_NAME
    CONTENT=$(cat "$WEDI_RC")
    LOG_EDIT=$(echo "$CONTENT" | \
    {
      FOUND=0
      LINE_NUM=1
      while read LINE
       do
         EXIST_FILE_DIR=$(echo "$LINE" | cut -d " " -f2)
         EXIST_FILE_NAME=$(echo "$LINE" | cut -d " " -f3)
         if [[ ( "$FILE_NAME" = "$EXIST_FILE_NAME" ) && ( "$DIRECTORY/" = "$EXIST_FILE_DIR" ) ]]; then
           OPEN_COUNT=$(echo "$LINE" | cut -d " " -f1)
           MATCHED_FILE_NAME=$EXIST_FILE_NAME
           MATCHED_FILE_DIR=$EXIST_FILE_DIR
           sed -i "$LINE_NUM d" $WEDI_RC
           FOUND=1
         fi
         LINE_NUM=$(($LINE_NUM+1))
       done
       OPEN_COUNT=$((OPEN_COUNT+1))
       if [[ $FOUND -eq 1 ]]; then
         echo "$OPEN_COUNT $MATCHED_FILE_DIR $MATCHED_FILE_NAME $DATE" >> $WEDI_RC
       fi
       echo "$FOUND"
    })
    if [[ $LOG_EDIT -eq 0 ]]; then
      echo "1 $DIRECTORY/ $FILE_NAME $DATE" >> $WEDI_RC
    fi
    exit 0
  fi

  if [[ -d $ARG ]]; then
    DIRECTORY=$ARG
    CONTENT=$(cat "$WEDI_RC" | grep "$DIRECTORY/ ")
    if [ -z "$CONTENT" ]; then
      echo "Unable to find any File in $DIRECTORY that exist in the LOG"
      echo "Don't put / sign in the end of the directory link"
      exit 1
    fi
    FILE=$(echo "$CONTENT" | \
    {
    NEWEST_FILE_DATE=0
    while read LINE
    do
      DATE=$(echo "$LINE" | cut -d " " -f4)
      DATE+=$(echo "$LINE" | cut -d " " -f5)
      DATE+=$(echo "$LINE" | cut -d " " -f6)
      DATE+=$(echo "$LINE" | cut -d " " -f7)
      DATE+=$(echo "$LINE" | cut -d " " -f8)
      DATE+=$(echo "$LINE" | cut -d " " -f9)
      if [ "$NEWEST_FILE_DATE" -lt "$DATE" ]; then
        NEWEST_FILE_DATE=$DATE
        OPEN_COUNT=$(echo "$LINE" | cut -d " " -f1)
        NEWEST_FILE_DIR=$(echo "$LINE" | cut -d " " -f2)
        NEWEST_FILE_NAME=$(echo "$LINE" | cut -d " " -f3)
      fi
    done
    echo "$OPEN_COUNT $NEWEST_FILE_DIR $NEWEST_FILE_NAME"
    })
    DATE=$(date +%Y" "%m" "%d" "%H" "%M" "%S)
    OPEN_COUNT=$(echo "$FILE" | cut -d " " -f1)
    FILE_NAME=$(echo "$FILE" | cut -d " " -f3)
    FILE_DIR=$(echo "$FILE" | cut -d " " -f2)
    START="$PROGRAM $DIRECTORY/$FILE_NAME"
    $START
    CONTENT=$(cat "$WEDI_RC")
    LOG_EDIT=$(echo "$CONTENT" | \
    {
    LINE_NUM=1
    while read LINE
      do
        NEWEST_FILE_DIR=$(echo "$LINE" | cut -d " " -f2)
        NEWEST_FILE_NAME=$(echo "$LINE" | cut -d " " -f3)
        if [[ ( "$FILE_NAME" = "$NEWEST_FILE_NAME" ) && ( "$FILE_DIR" = "$NEWEST_FILE_DIR" ) ]]; then
          sed -i "$LINE_NUM d" $WEDI_RC
        fi
        LINE_NUM=$(($LINE_NUM+1))
      done
      OPEN_COUNT=$((OPEN_COUNT+1))
      echo "$OPEN_COUNT $FILE_DIR $FILE_NAME $DATE" >> $WEDI_RC
    })
    exit 0
  fi
  echo "Wrong arguemnts"
  exit 1
fi
