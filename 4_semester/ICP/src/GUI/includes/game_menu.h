#ifndef MENU
#define MENU
#include <QtCore>
#include <QtWidgets>
#include <QObject>
#include <iostream>
#include <unistd.h>
#include "chess_game.h"
#include "board.h"
#include "reset_graphic.h"
#include "modebox_graphic.h"
#include "speedbox_graphic.h"
#include "movebox_graphic.h"
#include "coordinates_graphic.h"
#include "dialog_graphic.h"
#include "messageWindow.h"
#include "../../API/includes/ChessGame.h"
#include "../../API/includes/Globals.h"
#define PAUSE 0
#define HALF_SEC 1
#define ONE_SEC 2
#define TWO_SEC 3
#define THREE_SEC 4
#define FORWARD 6
#define BACKWARDS 7


/**
*@author Jaromir Hradil, xhradi15@fit.vutbr.cz
*/


/**
* A class to display tab and control every interaction within tab
*
*/
class game_menu : public QWidget
{
  ///Marking class as QTObject to develop signals
  Q_OBJECT

  public:

    /**
    *Constructor for class instance and creation all requiered attributes
    *
    */
    game_menu(QWidget * parent);

  private slots:


    /**
    *Private slot method for handling click signal emition in game mode menu
    */
    void handleManMode();



    /**
    *Private slot method for handling click signal emition in game mode menu
    */
    void handleAutoMode();


    /**
    *Private slot method for handling click signal emition for restart button to execute other methods
    */
    void handleRestartButton();

    /**
    *Private slot method for handling click signal emition for next move button to execute next move on the board
    */
    void handleNextMoveButton();

    /**
    *Private slot method for handling click signal emition for next move button to execute previous move on the board
    */
    void handlePreviousMoveButton();

    /**
    *Private slot method for handling click signal emition for manual move button to switch other graphic components to manual mode
    */
    void handleManualButton();

    /**
    *Private slot method for handling click signal emition for auto move button to switch other graphic components to automatic mode
    */
    void handleAutoButton();

    /**
    *Private slot method for handling click signal emition for clicking on any item in move list structure. It executes operations to jump on specific index of game move
    */
    void handleClickedItem(QListWidgetItem*);

    /**
    *Private slot method for handling click signal emition for clicking on submit button. It clears user input field and sends its value to analyze
    */
    void handleSubmitButton();

    /**
    *Private slot method for handling click signal emition for clicking on submit button. It clears user input field and sends its value to analyze
    */
    void handleUndoButton();

    /**
    *Private slot method for handling click signal emition for clicking on submit button. It clears user input field and sends its value to analyze
    */
    void handleRedoButton();

    /**
    *Private slot method for handling click signal emition for clicking on save button. It saves current game set.
    */
    void handleSaveButton();

    /**
    *Private slot method for handling click signal emition for clicking on speed button. It sets automatic play game delay.
    */
    void handleSpeedButton1();

    /**
    *Private slot method for handling click signal emition for clicking on speed button. It sets automatic play game delay.
    */
    void handleSpeedButton2();

    /**
    *Private slot method for handling click signal emition for clicking on speed button. It sets automatic play game delay.
    */
    void handleSpeedButton3();

    /**
    *Private slot method for handling click signal emition for clicking on speed button. It sets automatic play game delay.
    */
    void handleSpeedButton4();

    /**
    *Private slot method for handling click signal emition for clicking on speed button. It sets automatic play game delay.
    */
    void handleSpeedButton5();

    /**
    *Private slot method for handling click signal emition for clicking on play direction button. It plays game backwards.
    */
    void handleDirectionButtonB();

    /**
    *Private slot method for handling click signal emition for clicking on play direction button. It plays game forward.
    */
    void handleDirectionButtonF();

    /**
    *Private slot method for handling calling play direction methods to play game in required order.
    */
    void playDirection();

  private:

    //Restart button
    QPushButton *restartButton = new QPushButton(this); /*!< a restart button widget*/

    QGraphicsView *graphicsView;/*!< a restart button widget*/

    //Speedbox and buttons
    QGroupBox *speedBox = new QGroupBox(this);
    QRadioButton *speedButton1 = new QRadioButton(this->speedBox);
    QRadioButton *speedButton2 = new QRadioButton(this->speedBox);
    QRadioButton *speedButton3 = new QRadioButton(this->speedBox);
    QRadioButton *speedButton4 = new QRadioButton(this->speedBox);
    QRadioButton *speedButton5 = new QRadioButton(this->speedBox);
    //Playbox and buttons
    QGroupBox *playDirectionBox = new QGroupBox(this);
    QRadioButton *directionButtonB = new QRadioButton(this->playDirectionBox);
    QRadioButton *directionButtonF = new QRadioButton(this->playDirectionBox);

    //Gamemode box and buttons
    QGroupBox *gameModGroup = new QGroupBox(this);
    QPushButton *autoButton = new QPushButton(this->gameModGroup);
    QPushButton *manualButton = new QPushButton(this->gameModGroup);

    QLabel *moveListLabel = new QLabel(this);
    //Movebox and buttons
    QGroupBox *gameMoveGroup = new QGroupBox(this);
    QPushButton *prevMoveButton = new QPushButton(this->gameMoveGroup);
    QPushButton *nextMoveButton = new QPushButton(this->gameMoveGroup);
    //Turn labels
    QLabel *whiteMoveLabel = new QLabel(this);
    QLabel *blackMoveLabel = new QLabel(this);
    QLabel *newTabLabel = new QLabel(this);
    //Input and list fields
    QPlainTextEdit *userInSubmit = new QPlainTextEdit(this);
    QLabel *inputTextLabel = new QLabel(this);
    QListWidget *moveList = new QListWidget(this);

    QLabel * userMoveLabel = new QLabel(this);
    //Dialog buttons
    QPushButton *submitButton = new QPushButton(this);
    QPushButton *undoButton = new QPushButton(this);
    QPushButton *redoButton = new QPushButton(this);
    QPushButton *saveButton = new QPushButton(this);
    //Pick menu buttons
    QPushButton *autoModeMess = NULL;
    QPushButton *manModeMess = NULL;
    //Coordiante
    QLabel *letterCoordinates = new QLabel(this);
    QLabel *numCoor1 = new QLabel(this);
    QLabel *numCoor2 = new QLabel(this);
    QLabel *numCoor3 = new QLabel(this);
    QLabel *numCoor4 = new QLabel(this);
    QLabel *numCoor5 = new QLabel(this);
    QLabel *numCoor6 = new QLabel(this);
    QLabel *numCoor7 = new QLabel(this);
    QLabel *numCoor8 = new QLabel(this);
    //new chess board
    ChessGame * gameSet = new ChessGame;

    //Timer
    QTimer * autoMoveTimer = new QTimer(this);

    //Message window
    messageWindow * modeElector = NULL;
    //Gameflow attrbutes
    int move = -1;
    int direction = FORWARD;
    int speed = PAUSE;
    int insertLastIndex = -1;

    int index_before_input = -1;

    //Attributes for message display
    int check = 0;
    int mate = 0;


    game_board * new_board;



    /**
    *Private method for forw
    */
    void autoMovePlay();
    /**
    *Private method for forward button to play automatic game moves forward
    */
    void setUpButtonFunctionality();
    /**
    *Private method for forward button to play automatic game moves forward
    */
    void disableAll();
    /**
    *Private method for forward button to play automatic game moves forward
    */
    void enableManual();
    /**
    *Private method for forward button to play automatic game moves forward
    */
    void enableAuto();
    /**
    *Private method for forward button to play automatic game moves forward
    */
    void highlightCurrentMove();
    /**
    *Private method for forward button to play automatic game moves forward
    */
    void setTurn();
    /**
    *Private method for forward button to play automatic game moves forward
    */
    void setTimer();
    /**
    *Private method for forward button to play automatic game moves forward
    */
    void insertListItems();
    /**
    *Private method for forward button to play automatic game moves forward
    */
    int executeMove();

    /**
    *Private method for forward button to play automatic game moves forward
    */
    void timeOutForward();

    /**
    *Private method for play backwards button to play automatic game moves backwards
    */
    void timeOutBackwards();


    /**
    *Private method for setting right speed play menu
    */
    void switchToManual();

};
#endif
