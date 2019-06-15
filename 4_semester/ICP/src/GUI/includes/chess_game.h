#ifndef GAME
#define GAME
#include <QtCore>
#include <QtWidgets>
#include <QObject>
#include "board.h"
#include "game_menu.h"

/**
*@author Jaromir Hradil, xhradi15@fit.vutbr.cz
*/


/**
*Class for creating tab manager window
*/
class chess_game : public QWidget
{

  ///Marking class as QTObject to develop signals
  Q_OBJECT


  public:
      //Constructor
      chess_game(QApplication * app);
      //Destructor
      ~ chess_game();

  private slots:

  /**
  *Private slot method for closing tabs
  */
        void  slotCloseTab(int index);

  /**
  *Private slot method for adding tabs
  */
        void  addTab();



  private:

    QWidget * tabForm;
    QApplication * application; //application pointer
    QPushButton *tabButton; //new tab button
    QTabWidget *tabWidget;//Tab window
    int count = 1;


    //Creating new tab
    void setUpNewTab();
    //Creating tab widget
    void setUpTabWidget();



};

#endif
