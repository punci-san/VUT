#include "c401.h"

int main(int argc, char *argv[])
{
  tBSTNodePtr test;
  BSTInit(&test);
  BSTInsert(&test,'H',8);
  BSTInsert(&test,'A',8);
  if (test != NULL)
  {
    printf("%c\n", test->Key);
    printf("%d\n", test->BSTNodeCont);
    printf("------------------------\n");

    printf("%c\n", test->LPtr->Key);
    printf("%d\n", test->LPtr->BSTNodeCont);
  }
  BSTDispose(&test);
  if (test == NULL)
  {
    printf("Vyprazdil sa dobre\n");
  }
  return 0;
}
