/**
* @author Matúš Škuta <xskuta04@stud.fit.vutbr.cz>
*/
#include "../includes/ChessGame.h"

void ChessGame::createBoard()
{
  // Create board 8x8
  for (int x = 0; x < BOARD_SIZE; x++)
  {
    for (int y = 0; y < BOARD_SIZE; y++)
    {
      // Calculate the color of Square
      bool white = ((x + y) % 2) == 0 ? false : true;

      // Create new Square
      board[x][y] = new Square(x, y, white);
    }
  }
}

void ChessGame::clearBoard()
{
  // Delete allocated memory
  for (int x = 0; x < BOARD_SIZE; x++)
  {
    for (int y = 0; y < BOARD_SIZE; y++)
    {
      // Free each allocated Square
      delete board[x][y];
    }
  }
}


std::string ChessGame::GetNotation()
{
  // Are we in the index ?
  if (stringIndex < chessNotations.size())
    return chessNotations[stringIndex++];
  // Not in the index
  stringIndex = 0;
  return "";
}

/*
* Construtor of class ChessGame
*/
ChessGame::ChessGame()
{
  // Create board
  createBoard();
  // Initialize 4 moves
  for (int i = 0; i < MOVES_SIZE; i++)
  {
    moves[i] = NULL;
  }
}

/*
* Deconstructor of class ChessGame
*/
ChessGame::~ChessGame()
{
  // Delte all elements on board
  clearBoard();

  // Delete all elements from chessMoves
  deleteElements(0);

  chessNotations.clear();
}

/*
* Initialize the board with figures
*/
void ChessGame::placeFigures()
{
  // Place white figures

  // Tower
  board[0][0]->placeFigure(new Figure(W_ROOK));
  board[7][0]->placeFigure(new Figure(W_ROOK));

  // Horse
  board[1][0]->placeFigure(new Figure(W_KNIGHT));
  board[6][0]->placeFigure(new Figure(W_KNIGHT));

  // Shooter
  board[2][0]->placeFigure(new Figure(W_BISHOP));
  board[5][0]->placeFigure(new Figure(W_BISHOP));

  // Queen
  board[3][0]->placeFigure(new Figure(W_QUEEN));

  // King
  board[4][0]->placeFigure(new Figure(W_KING));

  // Place white pawns
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    board[i][1]->placeFigure(new Figure(W_PAWN));
  }

  // Place black figures

  // Tower
  board[0][7]->placeFigure(new Figure(B_ROOK));
  board[7][7]->placeFigure(new Figure(B_ROOK));

  // Horse
  board[1][7]->placeFigure(new Figure(B_KNIGHT));
  board[6][7]->placeFigure(new Figure(B_KNIGHT));

  // Shooter
  board[2][7]->placeFigure(new Figure(B_BISHOP));
  board[5][7]->placeFigure(new Figure(B_BISHOP));

  // Queen
  board[3][7]->placeFigure(new Figure(B_QUEEN));

  // King
  board[4][7]->placeFigure(new Figure(B_KING));

  // Place black pawns
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    board[i][6]->placeFigure(new Figure(B_PAWN));
  }
}

/*
* Function for sorting ChessMoves
* @mv1 First ChessMove to be compared.
* @mv2 Second ChessMove that we will be comparing with.
* @return Boolean if the mv1 is lesser that mv2
*/
bool compareChessMovesL(ChessMove *mv1,ChessMove *mv2)
{
  return (mv1->index < mv2->index);
}

/*
* Function for sorting ChessMoves
* @mv1 First ChessMove to be compared.
* @mv2 Second ChessMove that we will be comparing with.
* @return Boolean if the mv1 is higher that mv2
*/
bool compareChessMovesH(ChessMove *mv1,ChessMove *mv2)
{
  return (mv1->index > mv2->index);
}

bool ChessGame::SaveGame(std::string filepath)
{
  // Try to open file
  std::ofstream outfile;
  outfile.open(filepath);

  // Check if file is open
  if (!outfile.is_open())
    return false;

  // Loop through all chessMoves and append it into file
  for (unsigned long int i = 0; i < chessMoves.size(); i++)
  {
    outfile << chessMoves[i]->move << '\n';
  }

  // Close file
  outfile.close();

  // Return success
  return true;
}

void ChessGame::GenerateStringNotations()
{
  // Clear string vector
  chessNotations.clear();

  // Loop through all
  for (unsigned long int i = 0; i < chessMoves.size(); i++)
  {
    // Create string vector
    std::vector<std::string> tmp_vec = {};
    // Reserve vector for 3 values
    tmp_vec.reserve(3);

    // Create variables for splitting string
    std::string tmp = "";
    std::stringstream ss(chessMoves[i]->move);

    // Split by " "
    while (getline(ss,tmp,' '))
    {
      // Push splitted string
      tmp_vec.push_back(tmp);
    }

    // Push into notations
    chessNotations.push_back(tmp_vec[1]);
    chessNotations.push_back(tmp_vec[2]);
  }

  // Set index of string to 0, so when we are returning we are returning all
  stringIndex = 0;
}

/*
* @filepath Path to the file
* @return Will return 0 if everything is okay or error number based on what hapen
*/
int ChessGame::CreateGame(std::string filepath)
{
  // Variables
  std::string line = "";
  std::ifstream infile;
  Parser parser = Parser();
  ChessMove *chm = NULL;

  // Set this to true if any error occurs
  lastIndex = true;

  // Open file
  infile.open(filepath);

  // Is file open ?
  if (!infile.is_open())
  {
    placeFigures();
    GenerateStringNotations();
    return FILE_ERROR;
  }

  // Load line by line
  while (getline(infile,line))
  {
    // Create new ChessMove
    chm = new ChessMove;

    // Save line and set userInut to false
    chm->move = line;

    // Parse loaded line
    if (!parser.Parse(line,false))  // We are parsing file input
    {
      // Free allocated memory close file and exit
      delete chm;
      infile.close();
      return PARSER_ERROR;
    }

    // Set index to ChessMove that will be used to sort
    parser.setIndex(chm);

    // Get all white moves from line
    if (!parser.generateMove(&(chm->white)))
    {
      // Free allocated memory close file and exit
      delete chm;
      infile.close();
      return PARSER_ERROR;
    }

    // Get all black moves from line
    if (!parser.generateMove(&(chm->black)))
    {
      // Free allocated memory close file and exit
      delete chm;
      infile.close();
      return PARSER_ERROR;
    }

    // Push move in the vector
    chessMoves.push_back(chm);
  }

  // Sort vertex by index
  sort(chessMoves.begin(), chessMoves.end(), compareChessMovesL);

  // Check if indexes are correct
  for (unsigned long int i = 0; i < chessMoves.size(); i++)
  {
    // Check each index with index
    if (chessMoves[i]->index != (i + 1))
    {
      // Close file and return error
      infile.close();
      return PARSER_ERROR;
    }
  }

  // Place figures in the starting positions
  placeFigures();

  lastIndex = false;

  // Generate strings from notations
  GenerateStringNotations();

  // Close file and return 0
  infile.close();
  return 0;
}

/**
* Function that restart whole game
*/
void ChessGame::RestartGame()
{
  // Reset values
  currIndex = 0;
  currWhite = true;
  lastIndex = false;

  // Clear all generated moves
  clearMoves();

  // Clear board
  clearBoard();

  // Create new board
  createBoard();

  // Spawn figures
  placeFigures();
}

int ChessGame::AddUserInput(std::string str)
{
  // Empty string
  if (str.size() == 0)
    return 0;

  // Split string by EOL
  std::vector<std::string> lines = {};
  std::stringstream ss(str);
  std::string tmp = "";
  ChessMove *chm = NULL;
  Parser parser = Parser();

  // Split by every new line
  while (getline(ss,tmp,'\n'))
  {
    // Add split string into vector of strings
    lines.push_back(tmp);
  }

  // Delete all checkMoves from this index
  deleteElements(currIndex);

  // Loop through every line
  for (std::string line : lines)
  {
    // Create new ChessMove
    chm = new ChessMove;

    // Save line
    chm->move = std::to_string(chessMoves.size() + 1) + ". " + line;
    chm->userInput = true;

    // Parse loaded line,
    if (!parser.Parse(line,true)) // True means we are parsing user input
    {
      // Free chessmove and return error
      delete chm;
      return PARSER_ERROR;
    }

    // Set index to ChessMove
    chm->index = chessMoves.size();

    // Get all white moves from line
    if (!parser.generateMove(&(chm->white)))
    {
      // Free ChessMove and return error
      delete chm;
      return PARSER_ERROR;
    }

    // Get all black moves from line
    if (!parser.generateMove(&(chm->black)))
    {
      // Free ChessMove and return error
      delete chm;
      return PARSER_ERROR;
    }

    // Push move in the vector
    chessMoves.push_back(chm);
  }

  // Generate string notations
  GenerateStringNotations();

  // We added elements so there should be something
  lastIndex = false;

  // Return success
  return 0;
}

/*
* @from Removes all element from this number to the end
*/
void ChessGame::deleteElements(int from)
{
  // Remove all elements from backward to forward
  for (int i = chessMoves.size() - 1; i >= from; i--)
  {
    // Get pointer to the last element
    ChessMove *tmp = chessMoves[i];

    // Pop from vector
    chessMoves.pop_back();

    // Free
    delete tmp;
  }
}

/*
* @mv Structure containing move
*/
bool ChessGame::basicMoveCheck(MoveSet *mv)
{
  // Just to be safe
  if (mv == NULL)
    // Nothing to be checked error
    return false;

  // When one of from coordinates is lower than 0 that mean, it is not set
  if (mv->from_x < 0 || mv->from_y < 0)
    return false;

  // When one of to coordinates is lower than 0 that mean, it is not set
  if (mv->to_x < 0 || mv->to_y < 0)
    return false;

  // Is it the rigth figure ?
  if (!board[mv->from_x][mv->from_y]->isFig(mv->fig))
    return false;

  // Is there any figure ?
  if (board[mv->to_x][mv->to_y]->figure == NULL)
    return true;

  // These should not be equal
  if (board[mv->from_x][mv->from_y]->isWhiteFigure() == board[mv->to_x][mv->to_y]->isWhiteFigure())
    return false;

  // Everything is okay
  return true;
}

bool ChessGame::BishopMove(MoveSet *mv, bool whiteFig)
{
  int fx = mv->from_x;
  int fy = mv->from_y;

  int tx = mv->to_x;
  int ty = mv->to_y;

  // Check only the path
  int x_sub = std::abs(fx - tx);
  int y_sub = std::abs(fy - ty);

  // We are moving in X so these values should be equal
  if (x_sub != y_sub)
    return false;

  // Check if we are moving on the
  if (x_sub == 0)
    return false;

  // Do the basic check
  if (!basicMoveCheck(mv))
    return false;

  // Calculate where to move
  int x_way = (fx < tx) ? 1 : -1;
  int y_way = (fy < ty) ? 1 : -1;

  // Current coordinates
  int x_coord = fx + x_way;
  int y_coord = fy + y_way;

  // Check until we are on the same coordinates as we needs to be
  while (x_coord != tx)
  {
    // If there is some figure on the way error
    if (board[x_coord][y_coord]->figure != NULL)
      return false;

    // Move by the given index
    x_coord += x_way;
    y_coord += y_way;
  }


  // If there is figure to be kicked
  if (board[x_coord][y_coord]->figure != NULL)
  {
    // Check if the figure is kickable
    bool white = (whiteFig && board[x_coord][y_coord]->isBlackFigure());
    bool black = (!whiteFig && board[x_coord][y_coord]->isWhiteFigure());

    // The figure needs to be kickable
    if (white || black)
    {
      // Kickable create kickable move
      moves[0] = new Move;
      moves[0]->x = x_coord;
      moves[0]->y = y_coord;
      moves[0]->kick = true;
      moves[0]->check = mv->check;
      moves[0]->mate = mv->mate;
    }
  }

  // Create kick of my figure
  moves[1] = new Move;
  moves[1]->x = fx;
  moves[1]->y = fy;
  moves[1]->kick = true;
  moves[1]->check = mv->check;
  moves[1]->mate = mv->mate;

  // Create move of my figure
  moves[2] = new Move;
  moves[2]->x = tx;
  moves[2]->y = ty;
  moves[2]->kick = false;
  moves[2]->fig = mv->fig;
  moves[2]->check = mv->check;
  moves[2]->mate = mv->mate;

  // Return success
  return true;
}

bool ChessGame::KingMove(MoveSet *mv, bool whiteFig)
{
  int fx = mv->from_x;
  int fy = mv->from_y;

  int tx = mv->to_x;
  int ty = mv->to_y;

  // Check only the path
  int x_sub = std::abs(fx - tx);
  int y_sub = std::abs(fy - ty);

  // X should be 0 - 1
  if (x_sub < 0 || x_sub > 1)
    return false;

  // Y shoudl be 0 - 1
  if (y_sub < 0 || y_sub > 1)
    return false;

  // Check if we are moving on the same
  if (x_sub == 0 && y_sub == 0)
    return false;

  // Do the basic check
  if (!basicMoveCheck(mv))
    return false;

  // If there is figure
  if (board[tx][ty]->figure != NULL)
  {
    // Check if is kickable
    bool white = (whiteFig && board[tx][ty]->isBlackFigure());
    bool black = (!whiteFig && board[tx][ty]->isWhiteFigure());

    // The figure needs to be kickable
    if (white || black)
    {
      // Kickable create kickable move
      moves[0] = new Move;
      moves[0]->x = tx;
      moves[0]->y = ty;
      moves[0]->kick = true;
      moves[0]->check = mv->check;
      moves[0]->mate = mv->mate;
    }
  }

  // Create kick of my figure
  moves[1] = new Move;
  moves[1]->x = fx;
  moves[1]->y = fy;
  moves[1]->kick = true;
  moves[1]->check = mv->check;
  moves[1]->mate = mv->mate;

  // Create move of my figure
  moves[2] = new Move;
  moves[2]->x = tx;
  moves[2]->y = ty;
  moves[2]->kick = false;
  moves[2]->fig = mv->fig;
  moves[2]->check = mv->check;
  moves[2]->mate = mv->mate;

  // Return success
  return true;
}

bool ChessGame::KnightMove(MoveSet *mv, bool whiteFig)
{
  int fx = mv->from_x;
  int fy = mv->from_y;

  int tx = mv->to_x;
  int ty = mv->to_y;

  // Check only the path
  int x_sub = std::abs(fx - tx);
  int y_sub = std::abs(fy - ty);

  // Check if we are moving on the same square
  if (x_sub == 0 && y_sub == 0)
    return false;

  // X can be in interval 1 - 2
  if (x_sub > 2 || x_sub < 1)
    return false;

  // Y can be in interval 1 - 2
  if (y_sub > 2 || y_sub < 1)
    return false;

  // Horrizontal L
  if (x_sub == 1 && y_sub != 2)
    return false;

  // Vertical L
  if (x_sub == 2 && y_sub != 1)
    return false;

  // Do the basic check
  if (!basicMoveCheck(mv))
    return false;

  // If there is figure
  if (board[tx][ty]->figure != NULL)
  {
    // Check if is kickable
    bool white = (whiteFig && board[tx][ty]->isBlackFigure());
    bool black = (!whiteFig && board[tx][ty]->isWhiteFigure());

    // The figure needs to be kickable
    if (white || black)
    {
      // Kickable create kickable move
      moves[0] = new Move;
      moves[0]->x = tx;
      moves[0]->y = ty;
      moves[0]->kick = true;
      moves[0]->check = mv->check;
      moves[0]->mate = mv->mate;
    }
  }

  // Create kick of my figure
  moves[1] = new Move;
  moves[1]->x = fx;
  moves[1]->y = fy;
  moves[1]->kick = true;
  moves[1]->check = mv->check;
  moves[1]->mate = mv->mate;

  // Create move of my figure
  moves[2] = new Move;
  moves[2]->x = tx;
  moves[2]->y = ty;
  moves[2]->kick = false;
  moves[2]->fig = mv->fig;
  moves[2]->check = mv->check;
  moves[2]->mate = mv->mate;

  // Return success
  return true;
}

bool ChessGame::PawnMove(MoveSet *mv, bool whiteFig)
{
  int fx = mv->from_x;
  int fy = mv->from_y;

  int tx = mv->to_x;
  int ty = mv->to_y;

  // Check only the path
  int x_sub = abs(tx - fx);
  int y_sub = (ty - fy); // This needs to be + because we are moving only up

  // x_sub can only be <0,1>
  if (x_sub < 0 || x_sub > 1)
    return false;

  // abs(y_sub) can only be <1,2>
  if (abs(y_sub) < 1 || abs(y_sub) > 2)
    return false;

  // Changing into figure needs to be set
  if (((whiteFig && ty == 7) || (!whiteFig && ty == 0)) && mv->figChange == NO_FIGURE)
    return false;

  // White pawn figure
  if (whiteFig)
  {
    // When we are moving normally we can only move by 1
    if (fy != 1 && y_sub != 1)
      return false;

    // When we are moving from start location of white pawn
    // We can move 1 or 2
    if (fy == 1 && (y_sub < 1 || y_sub > 2))
      return false;
  }
  // Black pawn figure
  else
  {
    // When we are moving normally we can only move by 1
    if (fy != 6 && y_sub != -1)
      return false;

    // When we are moving from start location of black pawn
    // We can move 1 or 2
    if (fy == 1 && (y_sub > -1 || y_sub < -2))
      return false;
  }

  // If we are moving on X we can only when kicking others
  if (x_sub == 1 && (board[tx][ty]->figure == NULL))
    return false;

  // We cant kick before us
  if (x_sub == 0 && (board[tx][ty]->figure != NULL))
    return false;

  // Do the basic check
  if (!basicMoveCheck(mv))
    return false;

  // If there is figure to kick
  if (board[tx][ty]->figure != NULL)
  {
    bool white = (whiteFig && board[tx][ty]->isBlackFigure());
    bool black = (!whiteFig && board[tx][ty]->isWhiteFigure());

    // The figure needs to be kickable
    if (white || black)
    {
      // Kickable create kickable move
      moves[0] = new Move;
      moves[0]->x = tx;
      moves[0]->y = ty;
      moves[0]->kick = true;
      moves[0]->check = mv->check;
      moves[0]->mate = mv->mate;
    }
  }

  // Create kick of my figure
  moves[1] = new Move;
  moves[1]->x = fx;
  moves[1]->y = fy;
  moves[1]->kick = true;
  moves[1]->check = mv->check;
  moves[1]->mate = mv->mate;

  // Create move of my figure
  moves[2] = new Move;
  moves[2]->x = tx;
  moves[2]->y = ty;
  moves[2]->kick = false;
  moves[2]->check = mv->check;
  moves[2]->mate = mv->mate;

  // Set figure based on the position of the figure
  if ((whiteFig && ty == 7) || (!whiteFig && ty == 0))
  {
    // Set what we are changing into
    moves[2]->fig = mv->figChange;
  }
  else
  {
    // Set as pawn
    moves[2]->fig = mv->fig;
  }

  // Return success
  return true;
}

bool ChessGame::QueenMove(MoveSet *mv)
{
  int fx = mv->from_x;
  int fy = mv->from_y;

  int tx = mv->to_x;
  int ty = mv->to_y;

  // Check only the path
  int x_sub = std::abs(fx - tx);
  int y_sub = std::abs(fy - ty);

  // Trying to move on the same square
  if (x_sub == 0 && y_sub == 0)
    return false;

  // One of them needs to be 0 or they needs to be the same
  if ((x_sub != y_sub) && (x_sub != 0 && y_sub != 0))
    return false;

  // Do the basic check
  if (!basicMoveCheck(mv))
    return false;

  // Calculate where to move
  int x_way = (fx == tx) ? 0 : ((fx < tx) ? 1 : -1);
  int y_way = (fy == ty) ? 0 : ((fy < ty) ? 1 : -1);

  // Current coordinates
  int x_coord = fx + x_way;
  int y_coord = fy + y_way;

  // Check until we are on the same coordinates as we needs to be
  while (x_coord != tx || y_coord != ty)
  {
    // If there is some figure on the way error
    if (board[x_coord][y_coord]->figure != NULL)
      return false;

    // Set coordinates based on indexes
    x_coord += x_way;
    y_coord += y_way;
  }

  // If there is figure
  if (board[x_coord][y_coord]->isKickable(mv->fig))
  {
    // Kickable create kickable move
    moves[0] = new Move;
    moves[0]->x = x_coord;
    moves[0]->y = y_coord;
    moves[0]->kick = true;
    moves[0]->check = mv->check;
    moves[0]->mate = mv->mate;
  }

  // Create kick of my figure
  moves[1] = new Move;
  moves[1]->x = fx;
  moves[1]->y = fy;
  moves[1]->kick = true;
  moves[1]->check = mv->check;
  moves[1]->mate = mv->mate;

  // Create move of my figure
  moves[2] = new Move;
  moves[2]->x = tx;
  moves[2]->y = ty;
  moves[2]->kick = false;
  moves[2]->fig = mv->fig;
  moves[2]->check = mv->check;
  moves[2]->mate = mv->mate;

  // Return success
  return true;
}

bool ChessGame::RookMove(MoveSet *mv)
{
  int fx = mv->from_x;
  int fy = mv->from_y;

  int tx = mv->to_x;
  int ty = mv->to_y;

  // Check only the path
  int x_sub = std::abs(fx - tx);
  int y_sub = std::abs(fy - ty);

  // Trying to move on the same square
  if (x_sub == 0 && y_sub == 0)
    return false;

  // One of them needs to be 0
  if (x_sub != 0 && y_sub != 0)
    return false;

  // Do the basic check
  if (!basicMoveCheck(mv))
    return false;

  // Calculate where to move
  int x_way = (fx == tx) ? 0 : ((fx < tx) ? 1 : -1);
  int y_way = (fy == ty) ? 0 : ((fy < ty) ? 1 : -1);

  // Current coordinates
  int x_coord = fx + x_way;
  int y_coord = fy + y_way;

  // Check until we are on the same coordinates as we needs to be
  while (x_coord != tx || y_coord != ty)
  {
    // If there is some figure on the way error
    if (board[x_coord][y_coord]->figure != NULL)
    {
      return false;
    }

    // Set coordinates based on indexes
    x_coord += x_way;
    y_coord += y_way;
  }

  // If there is figure
  if (board[x_coord][y_coord]->isKickable(mv->fig))
  {
    // Kickable create kickable move
    moves[0] = new Move;
    moves[0]->x = x_coord;
    moves[0]->y = y_coord;
    moves[0]->kick = true;
    moves[0]->check = mv->check;
    moves[0]->mate = mv->mate;
  }

  // Create kick of my figure
  moves[1] = new Move;
  moves[1]->x = fx;
  moves[1]->y = fy;
  moves[1]->kick = true;
  moves[1]->check = mv->check;
  moves[1]->mate = mv->mate;

  // Create move of my figure
  moves[2] = new Move;
  moves[2]->x = tx;
  moves[2]->y = ty;
  moves[2]->kick = false;
  moves[2]->fig = mv->fig;
  moves[2]->check = mv->check;
  moves[2]->mate = mv->mate;

  // Return success
  return true;
}


/*
* @mv Structure containing move
*/
bool ChessGame::calculateMove(MoveSet *mv)
{
  // Check if both to are set
  if (mv->to_x < 0 || mv->to_y < 0)
    return false;

  // Based on figure calculate and check if the move is possible
  switch (mv->fig)
  {
    case W_BISHOP:
    case B_BISHOP:
      return BishopMove(mv,(mv->fig == W_BISHOP));

    case W_KING:
    case B_KING:
      return KingMove(mv,(mv->fig == W_KING));

    case W_KNIGHT:
    case B_KNIGHT:
      return KnightMove(mv,(mv->fig == W_KNIGHT));

    case W_PAWN:
    case B_PAWN:
      return PawnMove(mv,(mv->fig == W_PAWN));

    case W_QUEEN:
    case B_QUEEN:
      return QueenMove(mv);

    case W_ROOK:
    case B_ROOK:
      return RookMove(mv);

    // No figure no moves to be checked return error
    default:
      return false;
  }
}

/*
*
*/
bool ChessGame::CreateNextMoves()
{
  // Check if moves exist
  if (!lastMove)
    // If they exist, do nothing
    return true;

  // Are there any chess moves left ?
  if (lastIndex || currIndex >= chessMoves.size())
  {
    // We are generating error so we clear all moves for no memory leaks
    clearMoves();
    moves[0] = new Move;
    moves[0]->error_code = NO_MORE_MOVES;
    return false;
  }

  // Pick MoveSet based on current color
  MoveSet* mv = (currWhite) ? &(chessMoves[currIndex]->white) : &(chessMoves[currIndex]->black);
  // Increment only when the color is black
  currIndex = (!currWhite) ? currIndex + 1 : currIndex;
  // Always pick the other color
  currWhite = !currWhite;
  lastIndex = false;

  // Did we reached the end ?
  if ((long unsigned int)currIndex == chessMoves.size())
    lastIndex = true; // Set last to true

  // Try to look for both X and Y
  if (mv->from_x < 0 && mv->from_y < 0)
  {
    bool found = false;
    for (int x = 0; x < BOARD_SIZE; x++)
    {
      for (int y = 0; y < BOARD_SIZE; y++)
      {
        mv->from_x = x;
        mv->from_y = y;

        if (calculateMove(mv))
        {
          clearMoves();
          found = true;
        }

        if (found)
          break;
      }
      if (found)
        break;
    }
  }
  // Y is set
  else if (mv->from_x < 0)
  {
    // look through X
    for (mv->from_x = 0; mv->from_x < BOARD_SIZE; mv->from_x++)
    {
      // If not the figure skip
      if (!board[mv->from_x][mv->from_y]->isFig(mv->fig))
        continue;

      // Calculate if move is possible
      if (calculateMove(mv))
      {
        clearMoves();
        break;
      }
    }
  }
  // X is set
  else if (mv->from_y < 0)
  {
    // look through X
    for (mv->from_y = 0; mv->from_y < BOARD_SIZE; mv->from_y++)
    {
      // If not the figure skip
      if (!board[mv->from_x][mv->from_y]->isFig(mv->fig))
        continue;

      // Calculate if move is possible
      if (calculateMove(mv))
      {
        clearMoves();
        break;
      }
    }
  }

  // Calculate if move is possible
  if (!calculateMove(mv))
  {
    // Error occured clear moves
    clearMoves();

    // Reset current index to previous value
    if (currWhite)
      currIndex = (currIndex == 0) ? 0 : currIndex - 1;
    // Switch color back
    currWhite = !currWhite;

    // Wrong move create and return error to the GUI
    moves[0] = new Move;
    moves[0]->error_code = WRONG_MOVE;
    return false;
  }

  lastMove = false;
  return true;
}

bool ChessGame::UndoUserInput()
{
  // Nothing to delete
  if (chessMoves.size() == 0)
    return true;

  // Not user input nothing to delete
  if (!chessMoves[chessMoves.size() - 1]->userInput)
    return true;

  // Dont delete what is under our feet
  if ((currIndex == (chessMoves.size() - 1) && !currWhite) || (currIndex == chessMoves.size() && currWhite))
    return false;

  // Delete user input
  deleteElements(chessMoves.size() - 1);

  // Create new string notations after deleting some
  GenerateStringNotations();

  // Return success
  return true;
}

int ChessGame::RedoUserInput()
{
  int index = -1;
  // Find userInput
  for (unsigned long int i = 0; i < chessMoves.size(); i++)
  {
    if (chessMoves[i]->userInput)
    {
      index = i;
      break;
    }
  }

  // Nothing found return it
  if (index < 0)
    return index;

  // Delete all elements
  deleteElements(index);

  GenerateStringNotations();

  // Ok we found it return the correct index
  return index * 2 + 1;
}



void ChessGame::doTheMove(Move* mv)
{
  // If there is some error do nothing
  if (mv->error_code != 0)
    return;

  // If the coordinates are not set do nothing
  if (mv->x < 0 || mv->y < 0)
    return;

  // Kick ?
  if (mv->kick)
  {
    // Remove figure
    board[mv->x][mv->y]->removeFigure();
    return;
  }

  // Spawn figure
  board[mv->x][mv->y]->placeFigure(new Figure(mv->fig));
}

void ChessGame::clearMoves()
{
  // Loop through array of moves
  for (int i = 0; i < MOVES_SIZE; i++)
  {
    // If the move is not NULL
    if (moves[i] != NULL)
      // Free it
      delete moves[i];
  }
}

/*
* Return next move one by one
*/
Move* ChessGame::getNextMove()
{
  // Tmp pointer
  Move *tmp = NULL;

  // Try to create moves
  if (!CreateNextMoves())
  {
    tmp = moves[0];
    moves[0] = NULL;
    // If something failed the first will be set as error return that
    return tmp;
  }

  // Loop through all created moves and return them
  for (int i = 0; i < MOVES_SIZE; i++)
  {
    // Check what move is next
    if (moves[i] != NULL)
    {
      tmp = moves[i];
      moves[i] = NULL;
      doTheMove(tmp); // Do the same move that will be done in the GUI
      return tmp;
    }
  }
  // Set last move true
  lastMove = true;
  return NULL;
}
