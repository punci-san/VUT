#!/bin/sh

./ic18int test.IFJcode18
RETVAL=$?
[ $RETVAL -eq 0 ] && echo "\nSuccess"
[ $RETVAL -ne 0 ] && echo "\nFailure"
echo $RETVAL
