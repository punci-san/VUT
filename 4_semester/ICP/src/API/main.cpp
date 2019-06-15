#include "includes/ChessGame.h"

int main(int argc, char const *argv[])
{
  ChessGame* game = new ChessGame();

  int tmp = game->CreateGame("examples/notace2.txt");

  for (int i = 0; i < 2; i++)
  {
    Move *tmp = game->getNextMove();
    while (tmp != NULL)
    {

      if (tmp->error_code != 0)
      {
        printf("ERROR : %d\n", tmp->error_code);
        delete tmp;
        delete game;
        return 0;
      }

      delete tmp;

      tmp = game->getNextMove();
    }
  }


  game->AddUserInput("a4 b5\na5 b4\na6 b3\n");

  int index = game->RedoUserInput();

  game->RestartGame();

  for (int i = 0; i < index; i++)
  {
    Move *tmp = game->getNextMove();
    while (tmp != NULL)
    {

      if (tmp->error_code != 0)
      {
        printf("ERROR : %d\n", tmp->error_code);
        delete tmp;
        delete game;
        return 0;
      }

      delete tmp;

      tmp = game->getNextMove();
    }
  }

  std::string str = game->GetNotation();
  while (str.size() != 0)
  {
    std::cout << str << '\n';
    str = game->GetNotation();
  }

  game->printBoard();

  delete game;
  return 0;
}
