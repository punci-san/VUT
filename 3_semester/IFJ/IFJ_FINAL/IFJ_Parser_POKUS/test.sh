#!/bin/sh
RED=''
GREEN=''
NC=''

OK="${GREEN} OK ${NC}"
FAILED="${RED} FAILED ${NC}"

FILES="./tests/lexsynsem/*"
RUNTIMEFILES="./tests/runtimetests/*"
RUNTIMEFILESOUT="./tests/runtimeouts/"

TEST_OK=0
TEST_NUM=0

# Compiler tests
for f in $FILES.src
do
  TEST_NUM=`expr $TEST_NUM + 1`
  FILENAME=$(echo "$f" | sed "s/.*\///")
  RESULT=$(cat $f".out" | cut -b 1)
  ./main < $f > "${RUNTIMEFILESOUT}${FILENAME}.code"
  CMD_RESULT=$?
  if [ $CMD_RESULT -eq $RESULT ]
  then
    TEST_OK=`expr $TEST_OK + 1`
    echo "${FILENAME} \t\t ${OK} ${CMD_RESULT}"
  else
    echo "${FILENAME} \t\t ${FAILED} ${CMD_RESULT}"
  fi
done

# Runtime tests
for f in $RUNTIMEFILES.src
do
  TEST_NUM=`expr $TEST_NUM + 1`
  FILENAME=$(echo "$f" | sed "s/.*\///")
  RESULT=$(cat $f".out" | cut -b 1)
  ./main < $f > "${RUNTIMEFILESOUT}${FILENAME}.code"
  CMD_RESULT=$?
  if [ $CMD_RESULT -eq "0" ]
    then
    chmod 777 "${RUNTIMEFILESOUT}${FILENAME}.code"
    ./ic18int "${RUNTIMEFILESOUT}${FILENAME}.code"
    CMD_RESULT=$?
    if [ $CMD_RESULT -eq $RESULT ]
    then
      TEST_OK=`expr $TEST_OK + 1`
      echo "${FILENAME} \t\t ${OK} ${CMD_RESULT}"
    else
      echo "${FILENAME} \t\t ${FAILED} ${CMD_RESULT}"
    fi
  else
    if [ $CMD_RESULT -eq $RESULT ]
    then
      TEST_OK=`expr $TEST_OK + 1`
      echo "${FILENAME} \t\t ${OK} ${CMD_RESULT}"
    else
      echo "${FILENAME} \t\t ${FAILED} ${CMD_RESULT}"
    fi
  fi
done

echo "$TEST_NUM : $TEST_OK"
echo $(( 100 * $TEST_OK/$TEST_NUM )) | sed 's/..$/.&/'
