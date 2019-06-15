#include "c402.h"

int main(int argc, char const *argv[])
{
  tBTNodePtr TempTree;
  BTInit(&TempTree);
  BTDisposeTree(&TempTree);
  return 0;
}
