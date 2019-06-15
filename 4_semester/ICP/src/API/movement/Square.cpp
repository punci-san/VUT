/**
* @author Matúš Škuta <xskuta04@stud.fit.vutbr.cz>
*/
#include "../includes/movement/Square.h"

/**
*
* Constructor
*
* @param _x X position on the board.
* @param _y Y position on the board.
* @param _white Boolean telling us if the square is white or black.
*/
Square::Square(int _x, int _y, bool _white)
{
  // Set private variables of this class
  x = _x;
  y = _y;
  white = _white;
}

/**
*
* Deconstructor
*
*/
Square::~Square()
{
  // If there is figure
  if (figure != NULL)
    // Delete it
    delete figure;
}

/**
* Check if figure on this square is white.
* @return true if figure on this square is white, false if there is no figure or the figure is black.
*/
bool Square::isWhiteFigure()
{
  // No figure
  if (figure == NULL)
    // Return error
    return false;

  // Check if figure is one of the white ones
  if (isWhite(figure->type))
    // White figure
    return true;

  // Figure is not white
  return false;
}

/**
* Check if figure on this square is black.
* @return true if figure on this square is black, false if there is no figure or the figure is white.
*/
bool Square::isBlackFigure()
{
  // No figure
  if (figure == NULL)
    // No figure
    return false;

  // Check if figure is one of the black ones
  if (isBlack(figure->type))
    // Black figure
    return true;

  // White figure
  return false;
}

/**
* Check if given fig is black.
*
* @param fig FigureType to be checked if it is black.
* @return true if FigureType is black, false if not.
*/
bool Square::isBlack(FigureType fig)
{
  switch (fig)
  {
    // Check with each black type
    case B_KING:
    case B_QUEEN:
    case B_ROOK:
    case B_BISHOP:
    case B_KNIGHT:
    case B_PAWN:
      // Return black
      return true;
      break;
    default:
      // Return white
      return false;
      break;
  }
}

/**
* Check if given fig is white.
*
* @param fig FigureType to be checked if it is white.
* @return true if FigureType is white, false if not.
*/
bool Square::isWhite(FigureType fig)
{
  switch (fig)
  {
    // Check each white figure
    case W_KING:
    case W_QUEEN:
    case W_ROOK:
    case W_BISHOP:
    case W_KNIGHT:
    case W_PAWN:
      // Return is white
      return true;
      break;
    default:
      // Return is black
      return false;
      break;
  }
}

/**
* Check if this square contain given figure.
*
* @param fig Type of figure to be checked.
* @return true if given figure is on this square, false if not.
*
*/
bool Square::isFig(FigureType fig)
{
  // No figure on this square
  if (figure == NULL)
    // No figure
    return false;

  // Return boolean telling us if the given figure is on this square.
  return figure->type == fig;
}

/**
* Check if the figure on this square is kickable by given figure.
*
* @param fig Figure
* @return true if figure on this square can be kicked by the given figure, false otherwise.
*/
bool Square::isKickable(FigureType fig)
{
  // No figure cant be kicked
  if (figure == NULL)
    return false;

  // Figure is white and square contain black figure
  if (isWhite(fig) && isBlackFigure())
    // Kickable
    return true;

  // Figure is black and square contain white figure
  if (isBlack(fig) && isWhiteFigure())
    // Kickable
    return true;

  // Non kickable
  return false;
}

/**
* Place figure on this square.
* @param fig Figure that will be placed on this square.
* @return true if the figure could be placed on this square, false if square already contain another figure.
*
*/
bool Square::placeFigure(Figure *fig)
{
  // There is already figure on this square
  if (figure != NULL)
    // Can't be placed
    return false;

  // Place figure
  figure = fig;
  return true;
}

/**
* Remove figure from this square.
* @return true if there is some figure to be removed, false if there is no figure.
*/
bool Square::removeFigure()
{
  // There is no figure to be removed
  if (figure == NULL)
    return false;

  // Remove figure
  delete figure;
  figure = NULL;
  return true;
}
