#include <QtCore>
#include <QtWidgets>
#include <QObject>

/**
*@author Jaromir Hradil, xhradi15@fit.vutbr.cz
*/


/**
*Class for setting all graphic elelments for speed box menu and play box menu
*
*/
class speedMenuGraphic
{
  public:
    /**
    *Public method for creating a new graphic interface and elements of speed box menu
    */
    static void setUpSpeedMenu(QGroupBox * speedBox, QRadioButton * speedButton1, QRadioButton * speedButton2, QRadioButton * speedButton3, QRadioButton * speedButton4, QRadioButton * speedButton5);
    /**
    *Public method for creating a new graphic interface and elements of play box menu
    */
    static void setUpPlayMenu(QGroupBox * playDirectionBox, QRadioButton * directionButtonB, QRadioButton * directionButtonF);

};
