#!/bin/sh
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

OK="${GREEN} OK ${NC}"
FAILED="${RED} FAILED ${NC}"

FILES="./tests/*"

# Zavolame make nech mame najnovsiu verziu
make

# Kazdy test co je skontrolujeme ci je OK
for f in $FILES.src
do
  FILENAME=$(echo "$f" | sed "s/.*\///")
  RESULT=$(cat $f".out" | cut -b 1)
  ./main < $f > "Projekt.code"
  CMD_RESULT=$?
  if [ $CMD_RESULT -eq $RESULT ]
  then
    echo "${FILENAME} \t\t ${OK}"
  else
    echo "${FILENAME} \t\t ${FAILED}"
  fi
done
