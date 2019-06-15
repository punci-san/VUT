#ifndef MESSAGE
#define MESSAGE
#include <QtCore>
#include <QtWidgets>
#include <QObject>


/**
*@author Jaromir Hradil, xhradi15@fit.vutbr.cz
*/



/**
*Class fo creation custom QWidget window for pick menu mode
*/
class messageWindow : public QWidget
{


public:

      //Window pointer
      QWidget *modeSelectWindow;
      QLabel *modeSelectLabel;
      //Buttons
      QPushButton *autoSelect;
      QPushButton *manualSelect;
      //Constructor
      messageWindow(QWidget * parent);



};

#endif
