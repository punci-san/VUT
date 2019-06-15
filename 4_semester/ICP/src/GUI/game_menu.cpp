#include "includes/game_menu.h"


/**
*@author Jaromir Hradil, xhradi15@fit.vutbr.cz
*/



/**
*Contructor of class
*@param parent Serves to connect class to tab widget because of interactions from both sides
*/

game_menu :: game_menu(QWidget * parent) : QWidget(parent)
{

   //Setting graphic settings to specific widgets

     //Setting restart button graphic elements
  resetGraphic :: setUpResetButton(this->restartButton);
     //Setting speed menu graphic elements
  speedMenuGraphic :: setUpSpeedMenu(this->speedBox, this->speedButton1, this->speedButton2, this->speedButton3, this->speedButton4, this->speedButton5);
     //Setting play menu graphic elements
  speedMenuGraphic :: setUpPlayMenu(this->playDirectionBox, this->directionButtonB, this->directionButtonF);
     //Setting game mode graphic elements
  modeMenuGraphic :: setUpGameModeMenu(this->gameModGroup, this->autoButton, this->manualButton);
     //Setting move menu graphic elements
  moveMenuGraphic :: setUpMoveMenu(this->whiteMoveLabel, this->blackMoveLabel, this->gameMoveGroup, this->prevMoveButton, this->nextMoveButton);
     //Setting chessboard graphic elements
  coordinatesGraphic :: drawCoordinates(this->letterCoordinates, this->numCoor1, this->numCoor2, this->numCoor3, this->numCoor4, this->numCoor5, this->numCoor6, this->numCoor7, this->numCoor8);
     //Setting dialog menu graphic elements
  dialogGraphic :: setUpTabListDialog(this->userInSubmit, this->moveList, this->inputTextLabel, this->moveListLabel);
     //Setting user input buttons graphic elements
  dialogGraphic :: setUpUserInputButtons(this->userMoveLabel,this->submitButton, this->redoButton, this->undoButton, this->saveButton);
     //Creating new chess board for new game
  new_board = new game_board(this);

   //Creating a mode picker
  this->modeElector = new messageWindow(this);
   //Creating mode picker buttons
  this->autoModeMess = modeElector->autoSelect;
  this->manModeMess = modeElector->manualSelect;

   //Connectin buttons to slots to  make the work
  this->setUpButtonFunctionality();

  //Disable all buttons and inputs
  this->disableAll();

}




void game_menu :: handleManMode()
{
  //Deleting init menu pick
  this->manModeMess->disconnect();
  delete this->modeElector;

  //Asking for input file
  QUrl url("/home/");
  QUrl pathString = QFileDialog :: getOpenFileUrl(this,"Open file ", url);

  //Getting filepath
  QString filename = pathString.path();

  std :: string notation = "";

  //Sending file to API to check syntax
  int res = this->gameSet->CreateGame(filename.toStdString());

  //possible cases of parser
  switch (res)
  {
    case FILE_ERROR:

      QMessageBox :: about(this,"Wrong file","Your input file is not valid");
      break;

    case PARSER_ERROR:

      QMessageBox :: about(this,"Parser error","Your input file has not valid form");
      break;

    default:
      //Inserting items from parsed file to our list log
      this->insertListItems();
      break;

  }
  //Enabling manual
  this->enableManual();


}

void game_menu :: handleAutoMode()
{
  //Enable auto control application and
  this->enableAuto();
  this->autoModeMess->disconnect();//Disconnecting mode picker from slot function
  delete this->modeElector;

  //Getting filepath
  QUrl url("/home/");
  QUrl pathString = QFileDialog :: getOpenFileUrl(this,"Open file", url);

  QString filename = pathString.path();

  std :: string notation = "";

  //Sending file to API to check syntax
  int res = this->gameSet->CreateGame(filename.toStdString());
  //possible cases of parser
  switch (res)
  {
    case FILE_ERROR:

      QMessageBox :: about(this,"Wrong file","Your input file is not valid, switching to manual mod");
      this->enableManual();//Something went wrong, switching to manual

      break;
    case PARSER_ERROR:

      QMessageBox :: about(this,"Parser error","Your input file has not valid form, switching to manual mod");
      this->enableManual();//Something went wrong, switching to manual

      break;

    default:

    //Enabling auto mode control
      this->enableAuto();
      //Inserting items from parsed file to our list log
      this->insertListItems();


  }

}

void game_menu :: handleRestartButton()
{
  //Restarting figures
   this->move = -1;
   this->gameSet->RestartGame();//Restarting API
   this->new_board->setNewGame();
   this->highlightCurrentMove();//Highilighting new position
   this->setTurn();
}

void game_menu :: handleNextMoveButton()
{ //Executing move
  int res = this->executeMove();
  //Something went wrong
  if(res != 0)
  {
    this->highlightCurrentMove();
    return;
  }
  //All is ok, incrementing moves
  this->move++;
  this->setTurn();//Set right turn
  this->highlightCurrentMove();

  //Check report
  if(this->check)
  {
    QMessageBox :: about(this,"Notification","Check");
    this->check = 0;
  }
  //Mate report
  if(this->mate)
  {
    QMessageBox :: about(this,"Notification","Check mate");
    this->mate = 0;
  }


}


void game_menu :: handlePreviousMoveButton()
{
  int res = 0;
  this->new_board->setNewGame();//Restarting figures
  this->gameSet->RestartGame();//Restarting API

  //Executing move
  for(int i = -1; i < this->move - 1; i++)
  {
      res = this->executeMove();

      //Something went wrong
      if(res != 0)
      {
        this->highlightCurrentMove();
        return;
      }

      //Reporting check
      if(this->check) //move must be desired
      {
        if(i == this->move - 2)
        {
          QMessageBox :: about(this,"Notification","Check");
        }
        this->check = 0;
      }

      //Reporting mate
      else if(this->mate)//move must be desired
      {
        if(i == this->move - 2)
        {
          QMessageBox :: about(this,"Notification","Check mate");
        }
        this->mate = 0;
      }



  }
  //If there is still space to go back we go
  if(this->move > -1)
  {
    this->move--;
  }
  //If we are at the beginning we end
  else
  {
    QMessageBox :: about(this,"Warning","No more moves left");
  }
  //Setting turn and highilighting move
  this->setTurn();
  this->highlightCurrentMove();


}


void game_menu :: handleManualButton()
{
  //Stopping any running timer
  this->autoMoveTimer->stop();
  this->switchToManual();//Setting speed menu to pause
  this->enableManual();//Enabling manul control
}

/**
*Creating chess movement interaction based on communication with API
*@return  Returns indicator of success of operations, when there is error, 3 is f invalid move and 4 is for no more moves left. 0 is OK
*/
int game_menu :: executeMove()
{
  Move * figureMove = NULL;//Pointer for move structure


  //While we are getting repsons from API other than NULL, we still have moves to manage
    while((figureMove = this->gameSet->getNextMove()) != NULL)
    {

      switch(figureMove->error_code)
      {
          case 0:
            //API is indicating, that selected figure needs to go
            if(figureMove->kick)
            {
              this->new_board->deleteFigure(figureMove->x,figureMove->y);
            }
            else//Spawning figure at place, where it should be
            {
              this->new_board->spawnFigure(figureMove->x,figureMove->y,figureMove->fig);
            }
            //Mate is in game
            if(figureMove->mate)
            {
              this->mate = 1;
            }
            //Check mate was obtained
            else if(figureMove->check)
            {
              this->check = 1;
            }

            break;
          case 3://Something went wrong, message is displayed

            QMessageBox :: about(this,"Warning","Invalid move");
            break;

          case 4://Something went wrong, message is displayed

            QMessageBox :: about(this,"Warning","No more moves left");
            break;
      }
      //If something went wrong, we have to delete structure earlier than leaving a loop
      if(figureMove->error_code != 0)
      {
        delete figureMove;
        return figureMove->error_code;
      }

      delete figureMove;
    }



    return 0;

}




void game_menu :: handleAutoButton()
{  //Enabling auto mode
  this->enableAuto();
}



void game_menu :: handleClickedItem(QListWidgetItem * move)
{
  int jump = this->moveList->row(move); //Clicked item by user

  int res = 0;
   //Restarting whole game and moving to the required point
  this->new_board->setNewGame();
  this->gameSet->RestartGame();

   //Executing required moves
  for(int i = 0; i <= jump; i++)
  {
    res = this->executeMove();
     //Check if something went wrong
    if(res != 0)
    {
      this->move = i-1;
      this->setTurn();   //Setting turn and highilighting move
      this->highlightCurrentMove();
      return;
    }

    //Reporting check
    if(this->check) ///Move must be desired
    {
      if(i == jump)
      {
        QMessageBox :: about(this,"Notification","Check");
      }
      this->check = 0;

    }
    //Reporting mate
    else if(this->mate)
    {
      if(i == jump)///Move must be desired
      {
        QMessageBox :: about(this,"Notification","Check mate");
      }
      this->mate = 0;

    }



  }

   //Highilighting selected item
  this->move = jump;
  this->highlightCurrentMove();
  this->setTurn();

}



void game_menu :: handleSubmitButton()
{
   //Getting user input and clearing inout window
  QString userInput = this->userInSubmit->toPlainText();
  this->userInSubmit->clear();

   //Sending input to API to control
  int res = this->gameSet->AddUserInput(userInput.toStdString());
     //Check if something went wrong
  if(res != 0)
  {
    QMessageBox :: about(this,"Warning","Wrong user input");
    return;
  }
   //List is empty
  if(this->moveList->count() == 0 )
  {
    this->index_before_input = -1;
  }
  else
  {  //We look for current item and that we select
    QListWidgetItem * item = this->moveList->currentItem();
     //Saving all
    this->index_before_input = this->moveList->row(item);

    if(this->index_before_input % 2 == 0 && this->move > -1)
    {
      QMessageBox :: about(this,"Warning","Only on black moves it is possible to substitute moves");
      return;
    }

     //Deleting all items
    for(int i = this->moveList->count() - 1; i > -1; i--)
    {
      item = this->moveList->takeItem(i);
      this->moveList->removeItemWidget(item);
      delete item;
    }

  }

   //Inserting new items
  this->insertListItems();
   //if we are at the beginning, whe do not highlight
  if(this->index_before_input == -1 || this->move == -1)
  {
    return;
  }

  this->moveList->setCurrentItem(this->moveList->item(this->index_before_input));

}


void game_menu :: handleSaveButton()
{  //Asking for save file destination
  QUrl url("/home/");
  QUrl pathString = QFileDialog :: getSaveFileUrl(this,"Save file ", url);

  QString filePath = pathString.path();

  bool res = gameSet->SaveGame(filePath.toStdString());
     //Check if something went wrong
  if(!res)
  {
    QMessageBox :: about(this,"Error","Saving failed");
  }
  else //Saving was successful
  {
    QMessageBox :: about(this,"Report","Saving successful");
  }

}


void game_menu :: handleRedoButton()
{
  //Setting index to jump
  int redoIndex = this->gameSet->RedoUserInput();

  //Pointer for list items
  QListWidgetItem * item = NULL;
  //Nothing to do
  if(redoIndex < 0)
  {
    return;
  }
  //Begging of game
  if(redoIndex == 0)
  {
    this->move = -1;
    this->setTurn(); //Set right turn

    this->highlightCurrentMove();
    this->new_board->setNewGame();
    this->gameSet->RestartGame();

    //Deleting all items
    for(int i = this->moveList->count() - 1; i > -1; i--)
    {
     item = moveList->takeItem(i);
     moveList->removeItemWidget(item);
     delete item;
    }


    return;
  }
  //Restarting game
  this->new_board->setNewGame();//Restarting figures
  this->gameSet->RestartGame();
  this->move = redoIndex - 1;


  //Deleting all items
  for(int i = this->moveList->count() - 1; i > -1; i--)
  {
   item = this->moveList->takeItem(i);
   this->moveList->removeItemWidget(item);
   delete item;
  }

  //Inserting new items
  this->insertListItems();
  //if we are at the beginning, whe do not highlight

  if(this->move == -1)
  {
   return;
  }

  this->moveList->setCurrentItem(this->moveList->item(this->move));

  int res = 0;

  //Executing move
  for(int i = -1; i < this->move - 1; i++)
  {
    res = this->executeMove();
    //Something went wrong
    if(res != 0)
    {
      this->highlightCurrentMove();
      return;
    }

    //Reporting check
    if(this->check) //move must be desired
    {
      if(i == this->move - 2)
      {
        QMessageBox :: about(this,"Notification","Check");
      }
      this->check = 0;
    }
    //Reporting mate
    else if(this->mate )//move must be desired
    {
      if(i == this->move - 2)
      {
        QMessageBox :: about(this,"Notification","Check mate");
      }
      this->mate = 0;
    }

  }

  //Setting turn and highilighting move
  this->setTurn();
  this->highlightCurrentMove();
}



void game_menu :: handleUndoButton()
{

  //Pointer for list items
  QListWidgetItem * item = NULL;


    while(1)
    {
      //Nothing to do
      if(!this->gameSet->UndoUserInput())
      {
        handlePreviousMoveButton();

        continue;
      }
      break;
    }

    //Deleting all items
    for(int i = this->moveList->count() - 1; i > -1; i--)
    {
      item = this->moveList->takeItem(i);
      this->moveList->removeItemWidget(item);
      delete item;
    }

    //Inserting new items
    this->insertListItems();


}



void game_menu :: handleSpeedButton1()
{
   //Setting spped parameter and calling timer
  this->speed = PAUSE;
  this->setTimer();
}

void game_menu :: handleSpeedButton2()
{ //Setting spped parameter and calling timer
  this->speed =  HALF_SEC;
  this->setTimer();
}

void game_menu :: handleSpeedButton3()
{ //Setting spped parameter and calling timer
  this->speed = ONE_SEC;
  this->setTimer();
}

void game_menu :: handleSpeedButton4()
{ //Setting spped parameter and calling timer
   this->speed =  TWO_SEC;
   this->setTimer();
}

void game_menu :: handleSpeedButton5()
{ //Setting spped parameter and calling timer
    this->speed =  THREE_SEC;
    this->setTimer();
}

void game_menu :: handleDirectionButtonB()
{
   this->direction =  BACKWARDS; //Setting direction parameter
}

void game_menu :: handleDirectionButtonF()
{
   this->direction =  FORWARD; //Setting direction parameter
}



void game_menu :: setUpButtonFunctionality()
{

       //Creating connetions between button and events to make game possible to play
    connect(this->restartButton, SIGNAL (clicked()),this, SLOT (handleRestartButton()));
    connect(this->nextMoveButton, SIGNAL (clicked()),this, SLOT (handleNextMoveButton()));
       //Creating connetions between button and events to make game possible to play
    connect(this->manualButton, SIGNAL (clicked()),this, SLOT (handleManualButton()));
    connect(this->autoButton, SIGNAL (clicked()),this, SLOT (handleAutoButton()));
       //Creating connetions between button and events to make game possible to play
    connect(this->prevMoveButton, SIGNAL (clicked()),this, SLOT (handlePreviousMoveButton()));
    connect(this->moveList, SIGNAL(itemClicked(QListWidgetItem*)),this, SLOT(handleClickedItem(QListWidgetItem*)));
       //Creating connetions between button and events to make game possible to play
    connect(this->submitButton, SIGNAL(clicked()),this, SLOT(handleSubmitButton()));
    connect(this->saveButton, SIGNAL(clicked()),this, SLOT(handleSaveButton()));
    connect(this->undoButton, SIGNAL(clicked()),this, SLOT(handleUndoButton()));
    connect(this->redoButton, SIGNAL(clicked()),this, SLOT(handleRedoButton()));

       //Creating connetions between button and events to make game possible to play
    connect(this->autoModeMess,SIGNAL(clicked()), this, SLOT(handleAutoMode()));
    connect(this->manModeMess,SIGNAL(clicked()), this, SLOT(handleManMode()));

       //Creating connetions between button and events to make game possible to play
    connect(this->speedButton1, SIGNAL(clicked()),this, SLOT(handleSpeedButton1()));
    connect(this->speedButton2,SIGNAL(clicked()), this, SLOT(handleSpeedButton2()));
    connect(this->speedButton3,SIGNAL(clicked()), this, SLOT(handleSpeedButton3()));
    connect(this->speedButton4, SIGNAL(clicked()),this, SLOT(handleSpeedButton4()));
    connect(this->speedButton5,SIGNAL(clicked()), this, SLOT(handleSpeedButton5()));

       //Creating connetions between button and events to make game possible to play
    connect(this->directionButtonB,SIGNAL(clicked()), this, SLOT(handleDirectionButtonB()));
    connect(this->directionButtonF,SIGNAL(clicked()), this, SLOT(handleDirectionButtonF()));

     //Creating a connection between a timer and slot to catch timeouts and executing automoves
    connect(this->autoMoveTimer,SIGNAL(timeout()), this, SLOT(playDirection()));

}


void game_menu :: highlightCurrentMove()
{

    if(this->move == -1)
    {  //If we are at the begging whe deselect all items
      if(this->moveList->count() != 0)
      {
        this->moveList->setCurrentItem(this->moveList->item(0));
        this->moveList->setItemSelected(this->moveList->item(0), false);
      }

      return;
    }
     //     //Highilighting current move on the move list
    this->moveList->setCurrentItem(this->moveList->item(this->move));

}


void game_menu :: insertListItems()
{
   //Getting string literasls from API to insert the into the list
  std :: string item = this->gameSet->GetNotation();
  QString listLiteral = "";

   //Brushes to color the items
  const QBrush black_brush("black", Qt::SolidPattern);
  const QBrush white_brush("white", Qt::SolidPattern);

   //We always put new items into the list
  this->insertLastIndex = -1;

  while(item.size() != 0)
  {
     //Transforming literal to QString
    listLiteral = QString :: fromStdString(item);
    moveList->addItem(listLiteral); //Adding item
    this->insertLastIndex++;

    QListWidgetItem * insertedItem = this->moveList->item(this->insertLastIndex);
     //Coloring the item
    if(this->insertLastIndex % 2 == 1)
    {
      insertedItem->setBackground(black_brush);
      insertedItem->setForeground(white_brush);
    }
     //Getting other literal
    item = this->gameSet->GetNotation();
  }

}



void game_menu :: setTurn()
{
     //Setting turn to white
    if((this->move%2) == 1 || this->move == -1)
    {
      this->whiteMoveLabel->setEnabled(true);
      this->blackMoveLabel->setEnabled(false);
    }
    else //Setting turn to black
    {
      this->whiteMoveLabel->setEnabled(false);
      this->blackMoveLabel->setEnabled(true);
    }

}



void game_menu :: timeOutForward()
{
   //Executing automatically another move
  int res = this->executeMove();

  if(res != 0) //Nothing else possible to do, we stop
  {
    this->highlightCurrentMove();
    this->autoMoveTimer->stop();
    this->switchToManual();
    return;
  }
   //Highilighting move and setting other turn
  this->move++;
  this->setTurn();
  this->highlightCurrentMove();

  //Check report
  if(this->check)
  {
    QMessageBox :: about(this,"Notification","Check");
    this->autoMoveTimer->stop();
    this->switchToManual();
    this->check = 0;
  }
  //Mate report
  if(this->mate)
  {
    QMessageBox :: about(this,"Notification","Check mate");
    this->autoMoveTimer->stop();
    this->switchToManual();
    this->mate = 0;
  }


}


void game_menu :: timeOutBackwards()
{
  int res = 0;
  this->new_board->setNewGame(); //Restarting game to get to the past point
  this->gameSet->RestartGame();

  for(int i = -1; i < this->move - 1; i++)
  {   //Executing automatically another move
    res = this->executeMove();

    if(res != 0) //Nothing else possible to do, we stop
    {
      this->highlightCurrentMove();
      this->autoMoveTimer->stop();
      this->switchToManual();
      return;
    }

    //Reporting check
    if(this->check) //move must be desired
    {
      if(i == this->move - 2)
      {
        QMessageBox :: about(this,"Notification","Check");
        this->autoMoveTimer->stop();
        this->switchToManual();
      }
      this->check = 0;
    }
    //Reporting mate
    else if(this->mate )//move must be desired
    {
      if(i == this->move - 2)
      {
        QMessageBox :: about(this,"Notification","Check mate");
        this->autoMoveTimer->stop();
        this->switchToManual();
      }
      this->mate = 0;
    }

  }

  if(this->move > -1) //We can still continue
  {
    this->move--;
  }
  else
  { //Nothing else possible to do, we stop
    QMessageBox :: about(this,"Warning","No more moves left");
    this->autoMoveTimer->stop();
    this->switchToManual();
  }
   //Highilighting turn
  this->setTurn();
  this->highlightCurrentMove();

}



void game_menu :: playDirection()
{
  switch (this->direction)
  {
    case FORWARD://calling playing forward

      this->timeOutForward();
      break;

    case BACKWARDS://calling playing backwards

      this->timeOutBackwards();
      break;
  }


}



void game_menu :: setTimer()
{
   //Stopping timer, if it is running
  this->autoMoveTimer->stop();

  switch(this->speed)
  {
    case PAUSE:

       //Just leave, nothing to do here
      return;

      break;

    case HALF_SEC:
       //Setting timeout to one sec
      this->autoMoveTimer->setInterval(500);
      break;

    case ONE_SEC:
       //Setting timeout to two secs
      this->autoMoveTimer->setInterval(1000);
      break;

    case TWO_SEC:
       //Setting timeout to three secs
      this->autoMoveTimer->setInterval(2000);
      break;

    case THREE_SEC:
       //Setting timeout to four secs
      this->autoMoveTimer->setInterval(3000);
      break;

  }
       //Starting timer with new timeout
      this->autoMoveTimer->start();
}


void game_menu :: disableAll()
{
   //Disabling all controls
  this->userInSubmit->setEnabled(false);
  this->submitButton->setEnabled(false);
   //Disabling all controls
  this->undoButton->setEnabled(false);
  this->redoButton->setEnabled(false);
  this->saveButton->setEnabled(false);
   //Disabling all controls
  this->speedBox->setEnabled(false);
  this->gameModGroup->setEnabled(false);
  this->gameMoveGroup->setEnabled(false);
   //Disabling all controls
  this->restartButton->setEnabled(false);
  this->moveList->setEnabled(false);
  this->playDirectionBox->setEnabled(false);
}

void game_menu :: enableManual()
{
   //Enabling manual controls
  this->userInSubmit->setEnabled(true);
  this->submitButton->setEnabled(true);
   //Enabling manual controls
  this->undoButton->setEnabled(true);
  this->redoButton->setEnabled(true);
  this->saveButton->setEnabled(true);
   //Enabling manual controls
  this->speedBox->setEnabled(false);
  this->gameModGroup->setEnabled(true);
  this->gameMoveGroup->setEnabled(true);
   //Enabling manual controls
  this->restartButton->setEnabled(true);
  this->moveList->setEnabled(true);
  this->playDirectionBox->setEnabled(false);
}

void game_menu :: enableAuto()
{
   //Enabling auto controls
  this->userInSubmit->setEnabled(false);
  this->submitButton->setEnabled(false);
   //Enabling auto controls
  this->undoButton->setEnabled(false);
  this->redoButton->setEnabled(false);
  this->saveButton->setEnabled(false);
   //Enabling auto controls
  this->speedBox->setEnabled(true);
  this->gameModGroup->setEnabled(true);
  this->gameMoveGroup->setEnabled(false);
   //Enabling auto controls
  this->restartButton->setEnabled(false);
  this->moveList->setEnabled(false);
  this->playDirectionBox->setEnabled(true);
}


void game_menu :: switchToManual()
{  //Setting speed box to paused state
  this->speedButton1->setChecked(true);
  this->speedButton2->setChecked(false);
  this->speedButton3->setChecked(false);
  this->speedButton4->setChecked(false);
  this->speedButton5->setChecked(false);
}
