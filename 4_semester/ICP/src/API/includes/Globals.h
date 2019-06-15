/**
* @author Matúš Škuta <xskuta04@stud.fit.vutbr.cz>
*/
#if !defined(Globals_header)
#define Globals_header

#include <string>
/**
*
* Enumerator containing types of figures that can be found on the board.
*
*/
enum FigureType
{
  NO_FIGURE,  //  0
  W_KING,     //  1
  W_QUEEN,    //  2
  W_ROOK,     //  3
  W_BISHOP,   //  4
  W_KNIGHT,   //  5
  W_PAWN,     //  6
  B_KING,     //  7
  B_QUEEN,    //  8
  B_ROOK,     //  9
  B_BISHOP,   //  10
  B_KNIGHT,   //  11
  B_PAWN,     //  12
};

/**
*
* Structure that is send between API and GUI
*
*/
typedef struct Move
{
  /// Remove or spawn picture on given x,y coordinates.
  bool kick = false;
  /// Coordinates for a-h squares
  int x = -1;
  /// Coordinates for 1-8 squares
  int y = -1;
  /// FigureType is type of figure to be spawned
  FigureType fig = NO_FIGURE;
  /// Is check
  bool check = false;
  /// Is mate
  bool mate = false;
  /// Integer containing error code to be displayed
  int error_code = 0;
}Move;

/**
* Structure holding one move notation
*/
typedef struct MoveSet
{
  /// Coordinates a-h from where we are moving.
  int from_x = -1;
  /// Coordinates 1-8 from where we are moving.
  int from_y = -1;

  /// Coordinates a-h to where we are moving.
  int to_x = -1;
  /// Coordinates 1-8 to where we are moving.
  int to_y = -1;
  /// What type of figure are we
  FigureType fig = NO_FIGURE;
  /// What type of figure we want to change, used only for pawn.
  FigureType figChange = NO_FIGURE;
  /// When pawn reaches the other side of the board he is allowed to switch his figure
  bool pawn_switch = false;
  /// Was check set.
  bool check = false;
  /// Was mate set.
  bool mate = false;
}MoveSet;

/**
* Structure to hold one line of notation
*/
typedef struct ChessMove
{
  /// Index of notation
  unsigned long int index;
  /// String containing given notation
  std::string move;
  /// Structure containing white move
  MoveSet white;
  /// Structure containing black move
  MoveSet black;
  /// Was the move given to us by user ?
  bool userInput = false;
}ChessMove;

/// Return if permission error, file couldn't be oppened, or written to
#define FILE_ERROR 1
/// If given syntax was wrong
#define PARSER_ERROR 2
/// If given move could not be done
#define WRONG_MOVE 3
/// If there are no more moves to be done
#define NO_MORE_MOVES 4

#endif // MACRO
