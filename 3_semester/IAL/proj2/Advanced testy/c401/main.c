#include "c401.h"



int main(int argc, char const *argv[])
{
  tBSTNodePtr TempTree;
  BSTInit(&TempTree);

  BSTInsert(&TempTree,'h',5);
  BSTInsert(&TempTree,'k',6);
  BSTInsert(&TempTree,'g',7);
  BSTInsert(&TempTree,'r',7);
  BSTInsert(&TempTree,'f',9);
  BSTInsert(&TempTree,'e',7);
  BSTInsert(&TempTree,'g',9);
  BSTInsert(&TempTree,'h',7);
  BSTInsert(&TempTree,'a',9);
  BSTInsert(&TempTree,'j',7);
  BSTInsert(&TempTree,'k',9);
  BSTInsert(&TempTree,'l',7);
  BSTInsert(&TempTree,'x',9);
  BSTInsert(&TempTree,'n',7);
  BSTInsert(&TempTree,'o',9);
  BSTInsert(&TempTree,'q',7);
  BSTInsert(&TempTree,'q',9);

  BSTDelete(&TempTree,'a');
  BSTDelete(&TempTree,'r');

  int i;
  if ((BSTSearch(TempTree,'r',&i)) == TRUE)
  {
    printf("%d\n", i);
  }


  BSTDispose(&TempTree);
  return 0;
}
