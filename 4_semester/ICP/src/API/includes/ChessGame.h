/**
* @author Matúš Škuta <xskuta04@stud.fit.vutbr.cz>
*/
#if !defined(ChessGame_header)
#define ChessGame_header

#define BOARD_SIZE 8
#define MOVES_SIZE 4

#include <fstream>  // Files
#include <iostream> // Cout, Cin
#include <vector>   // vector
#include <memory>   // shared_ptr

#include "movement/Square.h"
#include "Globals.h"
#include "parsing/Parser.h"
#include "movement/Figure.h"

/**
* @author Matúš Škuta <xskuta04@stud.fit.vutbr.cz>
*/

/**
 * Implements API functionality for GUI application
 *
 *  ChessGame is used for internal representation of game.
 *  Calculations for each move, if the move is possible or
 *  finding the right figure that can do the given move.
 *
 */
class ChessGame
{

private:
  Square *board[BOARD_SIZE][BOARD_SIZE];
  std::vector<std::string> chessNotations = {};
  Move *moves[MOVES_SIZE];

  /// chessMoves in internal notation
  std::vector<ChessMove*> chessMoves = {};

  unsigned long int currIndex = 0;
  unsigned long int stringIndex = 0;
  bool currWhite = true;
  bool lastMove = true;
  bool lastIndex = false;

  bool CreateNextMoves();
  bool CreatePrevMoves();

  // Calculate move for this move
  bool calculateMove(MoveSet *mv);
  void clearMoves();

  // Place figures
  void placeFigures();

  // Do the same move as is done in GUI
  void doTheMove(Move *mv);

  // Check figures from and to
  bool basicMoveCheck(MoveSet *mv);

  // Figure movement
  bool BishopMove(MoveSet *mv,bool whiteFig);
  bool KingMove(MoveSet *mv,bool whiteFig);
  bool KnightMove(MoveSet *mv,bool whiteFig);
  bool PawnMove(MoveSet *mv,bool whiteFig);
  bool QueenMove(MoveSet *mv);
  bool RookMove(MoveSet *mv);

  void deleteElements(int from);

  void GenerateStringNotations();

  void createBoard();
  void clearBoard();

public:
  /// Constructor of class ChessGame
  ChessGame();

  /// Deconstructor of class ChessGame
  ~ChessGame();

  /**
  * Create new game and load notations from given file.
  *
  * @param filepath String containing path to file.
  * @return 0 if no error occured or error type.
  *
  */
  int CreateGame(std::string filepath);

  /**
  * Destroy current game and create new game without loading a file.
  */
  void RestartGame();

  /**
  * Return one move in notation to GUI.
  * @return String containing one move in notation or empty string.
  */
  std::string GetNotation();

  /**
  * Get user input from GUI and add him to the moveset.
  * @param str String containing user input.
  * @return 0 if no error occured or error type.
  */
  int AddUserInput(std::string str);

  /**
  * Return structure Move containing instructions for GUI.
  *
  * @return Structure Move containing instructions.
  */
  Move *getNextMove();

  /**
  * Removes one user input
  */
  bool UndoUserInput();

  /**
  * Removes all user input
  */
  int RedoUserInput();

  /**
  * Save the notations into file.
  * @param filepath Path to the file where the notations will be saved.
  * @return true if no error occured or false if something went wrong with writting to the file.
  */
  bool SaveGame(std::string filepath);

  void printBoard()
  {
    for (int y = BOARD_SIZE -1; y >= 0; y--)
    {
      for (int x = 0; x < BOARD_SIZE; x++)
      {
        if (board[x][y]->figure == NULL)
        {
          printf("   ");
          continue;
        }

        switch (board[x][y]->figure->type)
        {
          case W_KING:
          case B_KING:
            printf(" K ");
            break;

          case W_QUEEN:
          case B_QUEEN:
            printf(" D ");
            break;

          case W_ROOK:
          case B_ROOK:
            printf(" V ");
            break;

          case W_BISHOP:
          case B_BISHOP:
            printf(" S ");
            break;

          case W_KNIGHT:
          case B_KNIGHT:
            printf(" J ");
            break;

          case W_PAWN:
          case B_PAWN:
            printf(" p ");
            break;

          default:
            printf(" X ");
            break;
        }
      }
      printf("\n");
    }
  }
};

#endif // MACRO
