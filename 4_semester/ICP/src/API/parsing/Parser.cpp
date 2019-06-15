/**
* @author Matúš Škuta <xskuta04@stud.fit.vutbr.cz>
*/
#include "../includes/parsing/Parser.h"

/**
* Split given string into vector of strings.
*
* @param str String that will be split.
* @param separator Character that will be used as separator.
* @return Vector of strings containing parts of the string.
*/
std::vector<std::string> split_string(std::string str,char separator)
{
  // Create vector
  std::vector<std::string> res = {};
  std::stringstream ss(str); //

  // Temporaly string to hold returned string
  std::string tmp = "";

  // Loop until you can split the string
  while(getline(ss, tmp, separator))
  {
    // Each split string push into string vector res
    res.push_back(tmp);
  }

  // Return vector
  return res;
}

/**
* Check given index against regex.
*
* @param str String containing index that will be checked.
* @return true if regex mateched, false otherwise.
*/
bool checkIndex(std::string str)
{
  // Match regex
  if (!regex_match(str,std::regex("[1-9]([0-9])*\\.")))
    return false;

  // Match ok
  return true;
}

/**
* Check given notation against regex.
*
* @param str String containing move of white or black chess piece.
* @return true if regex matched, false otherwise.
*/
bool checkChessMove(std::string str)
{
  // Match regex
  if (regex_match(str,std::regex(NOTATION_REGEX)))
    return true;

  // Match failed
  return false;
}

/**
* Return FigureType based on given character and color.
*
* @param ch String containing 1 character
* @param isWhite Boolean telling us which player we are working with
* @return FigureType that the given ch represents.
*/
FigureType figureType(std::string ch,bool isWhite)
{
  // Nothing ?
  if (ch.size() == 0)
    return (isWhite) ? W_PAWN : B_PAWN;

  // King
  if (ch[0] == 'K')
    return (isWhite) ? W_KING : B_KING;
  // Queen
  else if (ch[0] == 'D')
    return(isWhite) ? W_QUEEN : B_QUEEN;
  // Rook
  else if (ch[0] == 'V')
    return (isWhite) ? W_ROOK : B_ROOK;
  // Bishop
  else if (ch[0] == 'S')
    return (isWhite) ? W_BISHOP : B_BISHOP;
  // Knight
  else if (ch[0] == 'J')
    return (isWhite) ? W_KNIGHT : B_KNIGHT;
  // p or nothing is pawn
  else
    return (isWhite) ? W_PAWN : B_PAWN;
}

/**
* Set figure type based on char
*
* @param mv Pointer to struct MoveSet that will be altered.
* @param ch Character that is checked if it is one of the figure.
* @param isWhite Bool telling us if the piece is white or black
*/
void setFigure(MoveSet *mv,std::string ch,bool isWhite)
{
  // Set figure based on what type of character is in ch
  mv->fig = figureType(ch,isWhite);
}

/**
* Set coordinates of our starting position
*
* @param mv Pointer to struct MoveSet that will be altered.
* @param move String containing 2 characters that will tell us position or one in other case.
*/
void setFrom(MoveSet *mv,std::string move)
{
  // Only 1 char set
  if (move.size() == 1)
  {
    // It is the first coordinate
    if ('a' <= move[0] && move[0] <= 'h')
    {
      // Set X
      mv->from_x = (int)(move[0] - 'a');
    }
    // It is the second coordinate
    else
    {
      // Set Y
      mv->from_y = (int)(move[0] - '0' - 1);
    }
  }
  // Two characters set
  else if (move.size() == 2)
  {
    // Convert the two characters into coordinates
    mv->from_x = (int)(move[0] - 'a');
    mv->from_y = (int)(move[1] - '0' - 1);
  }
  // Nothing
}

/**
* Set coordinates of our final position.
*
* @param mv Pointer to struct MoveSet that will be altered.
* @param move String containing 2 characters that will tell us position or one in other case.
*/
void setTo(MoveSet *mv,std::string move)
{
  // Two characters given
  if (move.size() == 2)
  {
    // Convert into coordinates
    mv->to_x = (int)(move[0] - 'a');
    mv->to_y = (int)(move[1] - '0' - 1);
  }
  // Nothing
  else
  {
    // Set negative values so we know this are not coordinates
    mv->to_x = -1;
    mv->to_y = -1;
  }
}

/**
* Set figure type that we want to change into.
*
* @param mv Pointer to struct MoveSet that will be altered.
* @param ch Character that is checked if it is one of the figure.
* @param isWhite Bool telling us if the piece is white or black
*/
void setChangeTo(MoveSet *mv,std::string str,bool isWhite)
{
  // No characters given
  if (str.size() == 0)
    // Do nothing
    return;

  // Set figChange based on str
  mv->figChange = figureType(str,isWhite);
}

/**
* Set check and mate booleans based on given string.
*
* @param mv Pointer to struct MoveSet that will be altered.
* @param str Character that is checked if it is one of the symbols.
*/
void setCheckMate(MoveSet *mv,std::string str)
{
  // Nothing skip
  if (str.size() == 0)
    return;

  // Check
  if (str[0] == '+')
    mv->check = true;
  // Mate
  else
  {
    mv->check = true;
    mv->mate = true;
  }
}

/**
* Constructor
*
*/
Parser::Parser()
{
}

/**
* Deconstructor
*
*/
Parser::~Parser()
{

}

/**
* Set index of structure based on index from file. This index
* will be later used for sorting purposes.
* @param chm Structure containing black and white move, given line
* and index.
*/
void Parser::setIndex(ChessMove *chm)
{
  // Set index
  chm->index = indexMove;
}

/**
* Function that will parse line and will set private variables.
* @param line String containing line loaded from file or userinput.
* @param user Boolean telling us if the input was given by user or from file.
* @return false if parsing of user input failed or true if no error occured.
*/
bool Parser::Parse(std::string line,bool user)
{
  // Split given line
  std::vector<std::string> vec = split_string(line,' ');

  // If we are dealing with user input
  if (user)
  {
    // Size of items should be 2
    if (vec.size() != 2)
      return false;

    // We are not checking index but insert there empty string
    vec.insert(vec.begin(),"");
  }
  // We are dealing with file input
  else
  {
    // If there are not enough strings
    if (vec.size() != 3)
      // Error
      return false;

    // Check index
    if (!checkIndex(vec[0]))
      return false;

    // Convert str to int
    indexMove = std::stoi(vec[0]);
  }

  // Check move of white chess piece
  if (!checkChessMove(vec[1]))
    return false;

  // Save white move
  whiteMove = vec[1];

  // Check move of black chess piece
  if (!checkChessMove(vec[2]))
    return false;

  // Save black move
  blackMove = vec[2];

  // Syntax is okay
  return true;
}

/**
* This function is called twice. First it sets, white move second time it sets black move.
* @param ms Moveset pointer that will be set to given values.
* @return false if
*/
bool Parser::generateMove(MoveSet* ms)
{
  // String holder
  std::string str = "";
  bool white = true;

  // We are making white move
  if (whiteMove.size() > 0)
  {
    // Get the white move
    str = whiteMove;
    whiteMove = ""; // Clear the white move
  }
  // We are making black move
  else if (blackMove.size() > 0)
  {
    // Get the black move
    str = blackMove;
    // Clear the black move
    blackMove = "";
    // Tell us that we are dealing with black move
    white = false;
  }
  // No more moves
  else
  {
    return false;
  }

  // Create vector for 7 groups
  std::vector<std::string> str_reg_groups;
  str_reg_groups.reserve(7);

  // string match
  std::smatch m;

  // regex
  std::regex e(NOTATION_REGEX);

  // Get all groups and push them into vector
  while (std::regex_search (str,m,e))
  {
    for (auto x:m)
      str_reg_groups.push_back(x);
    str = m.suffix().str();
  }

  // Set structure based on first group
  setFigure(ms,str_reg_groups[1],white);

  // Set from only one row or column
  setFrom(ms,str_reg_groups[2]);

  // Skip kick

  // Set from x,y
  setFrom(ms,str_reg_groups[4]);

  // Set to x,y
  setTo(ms,str_reg_groups[5]);

  // Set figChange
  setChangeTo(ms,str_reg_groups[6],white);

  // Set check/mate/
  setCheckMate(ms,str_reg_groups[7]);

  // No problem
  return true;
}
