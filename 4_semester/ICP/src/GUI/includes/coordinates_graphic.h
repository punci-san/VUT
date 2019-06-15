#include <QtCore>
#include <QtWidgets>
#include <QObject>

/**
*@author Jaromir Hradil, xhradi15@fit.vutbr.cz
*/




/**
*Class for setting all graphic elelments for coordinates on chessboard
*/

class coordinatesGraphic
{

 public:

   /**
   * Public method for creation of new graphic interface and elements
   */
    static void drawCoordinates(QLabel * letterCoordinates, QLabel * numCoor1, QLabel * numCoor2, QLabel * numCoor3, QLabel *numCoor4, QLabel * numCoor5, QLabel * numCoor6, QLabel * numCoor7, QLabel * numCoor8);

};
