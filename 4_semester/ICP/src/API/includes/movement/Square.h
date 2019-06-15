/**
* @author Matúš Škuta <xskuta04@stud.fit.vutbr.cz>
*/
#if !defined(Square_header)
#define Square_header

#include <string>
#include "Figure.h"
#include "../Globals.h"

/**
*
* This class represents one chess square that can hold one figure and imlements
* functions that work with that given figure.
*
*/
class Square
{
private:
  /// Color of square
  bool white = false;

  /// Coordinates of square
  int x = 0;
  int y = 0;

  /// Check if given fig is white
  bool isWhite(FigureType fig);

  /// Check if given fig is black
  bool isBlack(FigureType fig);

public:
  /// Pointer to given figure
  Figure *figure = NULL;

  /**
  *
  * Constructor
  *
  * @param _x X position on the board.
  * @param _y Y position on the board.
  * @param _white Boolean telling us if the square is white or black.
  */
  Square (int _x, int _y, bool _white);

  /**
  *
  * Deconstructor
  *
  */
  virtual ~Square ();

  /**
  * Check if this square contain given figure.
  *
  * @param fig Type of figure to be checked.
  * @return true if given figure is on this square, false if not.
  *
  */
  bool isFig(FigureType fig);

  /**
  * Check if the figure on this square is kickable by given figure.
  *
  * @param fig Figure
  * @return true if figure on this square can be kicked by the given figure, false otherwise.
  */
  bool isKickable(FigureType fig);

  /**
  * Check if figure on this square is white.
  * @return true if figure on this square is white, false if there is no figure or the figure is black.
  */
  bool isWhiteFigure();


  /**
  * Check if figure on this square is black.
  * @return true if figure on this square is black, false if there is no figure or the figure is white.
  */
  bool isBlackFigure();

  /**
  * Place figure on this square.
  * @param fig Figure that will be placed on this square.
  * @return true if the figure could be placed on this square, false if square already contain another figure.
  *
  */
  bool placeFigure(Figure *fig);

  /**
  * Remove figure from this square.
  * @return true if there is some figure to be removed, false if there is no figure.
  */
  bool removeFigure();
};

#endif // MACRO
