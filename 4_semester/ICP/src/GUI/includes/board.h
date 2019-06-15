#ifndef BOARD
#define BOARD
#include <QtCore>
#include <QtWidgets>
#include <QObject>
#include <iostream>
#include "../../API/includes/Globals.h"

#define BLACK 0
#define WHITE 1

/**
*@author Jaromir Hradil, xhradi15@fit.vutbr.cz
*/




/**
*Class for creating a chess board and managing chess figures
*/
class game_board : public QGraphicsScene
{

  public:
    /**
    *Constructor of class
    */
    game_board(QWidget * parent);

    /**
    *Destructo of class
    */
    ~ game_board();

    /**
    *Public method for spawning figures
    */
    void spawnFigure(int toX, int toY, FigureType figure);

    /**
    *Public method for deleting figures
    */
    void deleteFigure(int fromX, int fromY);

    /**
    *Public method for deleting figures
    */
    void setNewGame();


  private:

    //Graphic view for board
      QGraphicsView * scene;
      QGraphicsRectItem * positionChessBlocks[8][8];//Array of pointers to squares on board

      /**
      *Private method for setting positions
      */
      void setUpPositions();



};

#endif
