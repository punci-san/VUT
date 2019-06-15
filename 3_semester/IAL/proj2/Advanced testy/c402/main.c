#include "c402.h"

int main(int argc, char const *argv[])
{
  tBTNodePtr TempTree;
  BTInit(&TempTree);

  BTInsert(&TempTree,5);
  BTInsert(&TempTree,6);
  BTInsert(&TempTree,3);

  BTInsert(&TempTree,5);


  BTDisposeTree(&TempTree);
  return 0;
}
