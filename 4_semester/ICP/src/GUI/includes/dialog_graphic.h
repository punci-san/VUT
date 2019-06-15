#include <QtCore>
#include <QtWidgets>
#include <QObject>

/**
*@author Jaromir Hradil, xhradi15@fit.vutbr.cz
*/



/**
*Class for creation of dialog menu interface
*/
class dialogGraphic
{

  public:
      /**
      *Public method for creation of dialog menu graphic
      */
      static void  setUpTabListDialog(QPlainTextEdit * userInSubmit, QListWidget * movetList, QLabel * inputTextLabel, QLabel * moveListLabel);

      /**
      *Public method for creation of dialog button graphic
      */
      static void setUpUserInputButtons(QLabel * userMoveLabel, QPushButton * submitButton, QPushButton * redoButton, QPushButton * undoButton, QPushButton * saveButton);

};
