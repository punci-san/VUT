/**
* @author Matúš Škuta <xskuta04@stud.fit.vutbr.cz>
*/
#if !defined(Figure_header)
#define Figure_header

#include "../Globals.h"

#include <string>

/**
* Figure structure containing type of figure.
*/
class Figure
{
private:
public:
  /// Type of figure
  FigureType type;

  /**
  * Constructor
  * @param fig Construct figure with given type
  */
  Figure(FigureType fig);

  /**
  * Deconstructor
  */
  virtual ~Figure ();

};

#endif // MACRO
