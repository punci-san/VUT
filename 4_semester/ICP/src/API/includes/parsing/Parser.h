/**
* @author Matúš Škuta <xskuta04@stud.fit.vutbr.cz>
*/
#if !defined(Parser_header)
#define Parser_header

#include <iostream> // Cout, Cin
#include <vector>   // Vector
#include <string>   // string
#include <sstream>  // stringstream
#include <regex>    // Regex
#include "../Globals.h"

/// Regex for parsing sigle move in notation
#define NOTATION_REGEX "^(K|D|V|S|J|p){0,1}([a-h]|[1-8]){0,1}(x){0,1}([a-h][1-8]){0,1}([a-h][1-8])(K|D|V|S|J){0,1}(\\+|#){0,1}"

/**
* @author Matúš Škuta <xskuta04@stud.fit.vutbr.cz>
*/

/**
*
* Used for checking given input and saving move into structures.
*
*/
class Parser
{
private:
  std::string whiteMove = "";
  std::string blackMove = "";
  int indexMove;

public:
  /**
  * Constructor
  */
  Parser();

  /**
  * Deconstructor
  */
  ~Parser();

  /**
  * Function that will parse line and will set private variables.
  * @param line String containing line loaded from file or userinput.
  * @param user Boolean telling us if the input was given by user or from file.
  * @return false if parsing of user input failed or true if no error occured.
  */
  bool Parse(std::string line,bool user);

  /**
  * This function is called twice. First it sets, white move second time it sets black move.
  * @param ms Moveset pointer that will be set to given values.
  * @return false if
  */
  bool generateMove(MoveSet* ms);

  /**
  * Set index of structure based on index from file. This index
  * will be later used for sorting purposes.
  * @param chm Structure containing black and white move, given line
  * and index.
  */
  void setIndex(ChessMove* chm);

};

#endif // MACRO
