#include "includes/board.h"




/**
*Class constructor
*/
game_board :: game_board(QWidget * parent) : QGraphicsScene(parent)
{
  //Setting graphic elements
  this->setSceneRect(0,0, 800, 800);
  this->setUpPositions();
  //Setting graphic elements
  this->scene = new QGraphicsView(this, parent);
  this->scene->setEnabled(false);
  this->scene->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  this->scene->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  //Setting graphic elements
  this->scene->setGeometry(QRect(80, 2, 805, 805));
  this->setNewGame();

}

/**
*Class destructor
*/
game_board :: ~ game_board()
{
  for(int i = 0; i < 8; i++)///Deleting all squares
  {
    for(int j = 0; j < 8; j++)
    {
      delete this->positionChessBlocks[i][j];
    }

  }

}




void game_board :: spawnFigure(int toX, int toY, FigureType figure)
{
  ///Creating new pixmap for figures
  QGraphicsPixmapItem * figure_to_spawn = new QGraphicsPixmapItem(this->positionChessBlocks[toX][toY]);

  ///Setting appropriate figure image
  switch (figure)
  {
    case W_KING://Spawning king
        figure_to_spawn->setPixmap(QPixmap("src/GUI/img/w_king.png"));
      break;

    case W_QUEEN://Spawning queen
        figure_to_spawn->setPixmap(QPixmap("src/GUI/img/w_queen.png"));
      break;

    case W_ROOK://Spawning rook
        figure_to_spawn->setPixmap(QPixmap("src/GUI/img/w_rook.png"));
      break;

    case W_BISHOP://Spawning bishop
        figure_to_spawn->setPixmap(QPixmap("src/GUI/img/w_bishop.png"));
      break;

    case W_KNIGHT://Spawning knight
        figure_to_spawn->setPixmap(QPixmap("src/GUI/img/w_knight.png"));
      break;

    case W_PAWN://Spawning pawn
        figure_to_spawn->setPixmap(QPixmap("src/GUI/img/w_pawn.png"));
      break;

    case B_KING://Spawning king
        figure_to_spawn->setPixmap(QPixmap("src/GUI/img/b_king.png"));
      break;

    case B_QUEEN://Spawning queen
        figure_to_spawn->setPixmap(QPixmap("src/GUI/img/b_queen.png"));
      break;

    case B_ROOK://Spawning rook
        figure_to_spawn->setPixmap(QPixmap("src/GUI/img/b_rook.png"));
      break;

    case B_BISHOP://Spawning bishop
        figure_to_spawn->setPixmap(QPixmap("src/GUI/img/b_bishop.png"));
      break;

    case B_KNIGHT://Spawning knight
        figure_to_spawn->setPixmap(QPixmap("src/GUI/img/b_knight.png"));
      break;

    case B_PAWN://Spawning pawn
        figure_to_spawn->setPixmap(QPixmap("src/GUI/img/b_pawn.png"));
      break;

    default:
      break;

  }

    //Setting position of figure to specific coordinates
    figure_to_spawn->setPos(15 + (100*toX),15 + (100*(-toY+7)));

}


void game_board :: deleteFigure(int fromX, int fromY)
{
  //Figure list pointer iterator
  QList <QGraphicsItem *> figure_list = this->positionChessBlocks[fromX][fromY]->childItems();
  if(!figure_list.empty())  ///Deleting figure
  {
    delete figure_list[0];
  }

}

void game_board :: setNewGame()
{

      //Deleting all figure
      for(int i = 0; i < 8; i++)
      {
        for(int j = 0; j < 8; j++)
        {
          this->deleteFigure(i,j);
        }
      }

      ///white
      ///Spawning white figures on theit places
      this->spawnFigure(0,0,W_ROOK);
      this->spawnFigure(7,0,W_ROOK);
      this->spawnFigure(1,0,W_KNIGHT);
      this->spawnFigure(6,0,W_KNIGHT);
      this->spawnFigure(2,0,W_BISHOP);
      this->spawnFigure(5,0,W_BISHOP);
      this->spawnFigure(3,0,W_QUEEN);
      this->spawnFigure(4,0,W_KING);

      ///Spawning pawn front line
      for(int i = 0;i<8;i++)
      {
        this->spawnFigure(i,1,W_PAWN);
      }

    ///Spawning white figures on theit places
      ///black
      this->spawnFigure(0,7,B_ROOK);
      this->spawnFigure(7,7,B_ROOK);
      this->spawnFigure(1,7,B_KNIGHT);
      this->spawnFigure(6,7,B_KNIGHT);
      this->spawnFigure(2,7,B_BISHOP);
      this->spawnFigure(5,7,B_BISHOP);
      this->spawnFigure(3,7,B_QUEEN);
      this->spawnFigure(4,7,B_KING);

      ///Spawning pawn front line
      for(int i = 0;i<8;i++)
      {
        this->spawnFigure(i,6,B_PAWN);
      }

}



void game_board :: setUpPositions()
{
  ///Coordinates
  qreal x = 0;
  qreal y = 700;
  qreal w = 100;
  qreal h = 100;
  int color = BLACK;//color



  ///Color brushes
  const QPen tmp_pen;
  const QBrush black_brush("#986504", Qt::SolidPattern);
  const QBrush white_brush("#eeaf4c", Qt::SolidPattern);


  //Creating squares on chessboard
  for(int i = 0; i < 8 ;i++)
  {
      ///Setting color
      if(i%2 != 0)
      {
        color = WHITE;
      }
      else
      {
        color = BLACK;
      }


      for(int j = 0;j < 8; j++)
      {
         if(color == WHITE)///Creating square
         {
           this->positionChessBlocks[i][j] =  this->addRect(x, y, w, h, tmp_pen, white_brush);
           color = BLACK;
         }
         else///Creating square
         {
           this->positionChessBlocks[i][j] = this->addRect(x, y, w, h, tmp_pen, black_brush);
           color = WHITE;
         }
          y -= 100;
      }

      x += 100;
      y = 700;
  }



}
