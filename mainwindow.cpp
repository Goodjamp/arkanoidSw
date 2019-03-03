#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include "stdint.h"
#include "QDebug"

#include "arkanoidsetings.h"
/*

    QList<QPushButton *> myObj;
    QPushButton *platform;
    QPushButton *ball;

    bool initGame(void);
    int32_t   platformX;
    int32_t   platformY;

*/

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->pushButton->setGeometry(200,
                                200,
                                9,
                                9);
    ui->pushButton->setStyleSheet("background-color: rgb(0, 0, 0);"
                                  "border: none; "
                                  "border-radius: 4px;");
    initGame();
}

bool MainWindow::initGame(void)
{
    int32_t shiftBorder = BALL_RADIUS;
    //set platform position
    platformX      = 0;
    platformSpeed  = 10;
    platform = new QPushButton(this);
    platform->setGeometry(shiftBorder ,
                          AREA_HEIGHT -  PLATFORM_HEIGHT + shiftBorder,
                          PLATFORM_WIDTH - shiftBorder * 2,
                          PLATFORM_HEIGHT - shiftBorder * 2);
    platform->setStyleSheet("background-color: rgb(0, 255, 64);"
                            "border-color:  rgb(0, 0, 255); ");
    platform->setVisible(true);
    platform->setEnabled(false);

    //calculate and set obj position. User could set owen algorithm
    int32_t dh=(AREA_WIDTH - (int32_t)(AREA_WIDTH/OBJ_WIDTH)*OBJ_WIDTH) / 2;
    int32_t objPerLine = (int32_t)(AREA_WIDTH/OBJ_WIDTH);

    int32_t y = shiftBorder;

    for(int32_t k = 0; k < OBJ_QYANTITY; k += objPerLine) {
        for(int32_t n = 0; n < objPerLine; n++ ) {
            myObj.push_back(new QPushButton(this));
            myObj.back()->setGeometry(dh + shiftBorder + OBJ_WIDTH * n,
                                      y + shiftBorder,
                                      OBJ_WIDTH - 2 * shiftBorder,
                                      OBJ_HEIGHT - 2 * shiftBorder);
            myObj.back()->setStyleSheet("background-color: rgb(200, 100, 180);"
                                        "border: none; "
                                        "border-radius: 4px;");
            myObj.back()->setVisible(true);
            myObj.back()->setEnabled(false);
        }
        y += OBJ_HEIGHT;
    }

    return true;
}


void MainWindow::updatePlatformPos(bool leftShift)
{
    if(leftShift) {
        if(platformX - platformSpeed < 0) {
            return;
        }
        platformX -= platformSpeed;
        platform->move(platform->pos().x() - platformSpeed, platform->pos().y());
    } else {
        if(platformX + platformSpeed + PLATFORM_WIDTH > AREA_WIDTH) {
            return;
        }
        platformX += platformSpeed;
        platform->move(platform->pos().x() + 10, platform->pos().y());
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->type()==QEvent::KeyPress) {
        QKeyEvent* key = static_cast<QKeyEvent*>(event);
        switch(key->key()) {
        case Qt::Key_Comma:
            updatePlatformPos(true);
            break;
        case Qt::Key_Period:
            updatePlatformPos(false);
            break;
        default: break;
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{

}
