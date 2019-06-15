#include <QApplication>
#include "includes/chess_game.h"
#include <iostream>


/**
*@author Jaromir Hradil, xhradi15@fit.vutbr.cz
*/



int main(int argc, char *argv[])
{

    //Instance of Qt application for GUI execution
    QApplication app(argc, argv);
    //Creating instance of the main window
    chess_game window(&app);
    //Showing  main window
    window.show();
    //Running GUI
    return app.exec();
}
