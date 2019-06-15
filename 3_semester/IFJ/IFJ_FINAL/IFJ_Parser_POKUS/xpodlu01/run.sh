#!/bin/sh
make
valgrind ./main < Projekt.ifj > Projekt.code
RETVAL=$?
echo "\nErr code: $RETVAL"
echo "-------------------------------------------------------------\n"

if [ $RETVAL -eq 0 ]
then
  ./ic18int Projekt.code
  RETVAL=$?
  echo "\n-------------------------------------------------------------\n"
  echo "\nErr code: $RETVAL"
fi
