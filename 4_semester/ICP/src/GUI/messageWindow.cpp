#include "includes/messageWindow.h"


/**
*@author Jaromir Hradil, xhradi15@fit.vutbr.cz
*/



/**
* Creates new class with appropriate elements
* @param parent Pointer to parent widget
*/
messageWindow :: messageWindow(QWidget * parent) : QWidget(parent)


{
  //Creating font for graphic interface
      //Generated by QtCreator
  QFont font5;
  font5.setFamily(QStringLiteral("Abyssinica SIL"));
  font5.setPointSize(15);
  font5.setBold(true);
  font5.setItalic(false);
  font5.setWeight(75);


  //Setting up widget and adjusting graphic elements
  this->setGeometry(QRect(390, 400, 311, 231));
  this->setStyleSheet(QStringLiteral("border-color: rgb(46, 52, 54);"));
  this->activateWindow();

  //Setting up button and adjusting graphic elements
      //Generated by QtCreator
  this->manualSelect = new QPushButton(this);
  this->manualSelect->setText("Manual");
  this->manualSelect->setGeometry(QRect(170, 160, 96, 35));
  this->manualSelect->setStyleSheet(QLatin1String("QPushButton\n""{\n""   background-color:rgb(233, 185, 110)\n""}"));

  //Setting up label and adjusting graphic elements
      //Generated by QtCreator
  this->modeSelectLabel = new QLabel(this);
  this->modeSelectLabel->setText("Pick game mode");
  this->modeSelectLabel->setEnabled(true);
  this->modeSelectLabel->setGeometry(QRect(50, 70, 231, 41));
  this->modeSelectLabel->setFont(font5);
  this->modeSelectLabel->setAlignment(Qt::AlignCenter);

  //Setting up button and adjusting graphic elements
      //Generated by QtCreator
  this->autoSelect = new QPushButton(this);
  this->autoSelect->setText("Automatic");
  this->autoSelect->setGeometry(QRect(60, 160, 96, 35));
  this->autoSelect->setStyleSheet(QLatin1String("QPushButton\n""{\n""   background-color:rgb(233, 185, 110)\n""}"));



}
