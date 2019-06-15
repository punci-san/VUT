#include <QtCore>
#include <QtWidgets>
#include <QObject>

/**
*@author Jaromir Hradil, xhradi15@fit.vutbr.cz
*/


/**
*Class for creating graphic of move menu
*/
class moveMenuGraphic
{
  public:
    /**
    *Public method creating graphic of move menu
    */
    static void setUpMoveMenu(QLabel * whiteMoveLabel, QLabel * blackMoveLabel, QGroupBox * gameMoveGroup, QPushButton * prevMoveButton, QPushButton * nextMoveButton);

};
