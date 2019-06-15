/**
* @author Matúš Škuta <xskuta04@stud.fit.vutbr.cz>
*/
#include "../includes/movement/Figure.h"

/**
* Constructor
* @param fig Construct figure with given type
*/
Figure::Figure(FigureType fig)
{
  // Set type of figure
  type = fig;
}

/**
* Deconstructor
*/
Figure::~Figure()
{
  // Nothing to do here
}
