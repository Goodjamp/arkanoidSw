#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include "stdint.h"
#include "QDebug"
#include "QPainter"

extern "C" {
#include "arkonoidEngeen/arkanoidEngeen.h"
}

#include "arkanoidsetings.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->setGeometry(20, 20, AREA_WIDTH, AREA_HEIGHT);
    ui->setupUi(this);

    updateTimer = new QTimer();
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::updateState);
    updateTimer->setInterval(FRAME_RATE_MS);
    //updateTimer->start();
    xBall_0 = 0;
    yBall_1 = 0;
    xBall_0 = 0;
    yBall_1 = 0;
    directionAngle = M_PI / 2;
    dirX = ballX + static_cast<int32_t>(100 * cosf(directionAngle));
    dirY = ballY - static_cast<int32_t>(100 * sinf(directionAngle));

    initGame();
    update();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter  myPainter(this);
    myPainter.drawLine(ballX, ballY, dirX, dirY);
}

void MainWindow::updateState(void)
{
    gameTimeMs += FRAME_RATE_MS;
    arkanoidUpdate(arkanoidH, gameTimeMs);
    xBall_0 = xBall_1;
    yBall_0 = yBall_1;
    arkanoidGetBallPos(arkanoidH, &xBall_1, &yBall_1);
    this->update();
    ball->move(xBall_1, yBall_1);
    for(uint16_t k = 0; k < OBJ_QYANTITY; k++) {
        if(elementDesc[k].isPresent) {
            myObj[k]->setVisible(true);
        }else {
            myObj[k]->setVisible(false);
        }
    }
}

bool MainWindow::initGame(void)
{
    gameStatus = GAME_WAIT_START;
    //init game status and variables
    uint32_t shiftBorder = BALL_RADIUS;
    platformSpeed  = BALL_INIT_SPEED;
    gameTimeMs     = 0;
    platformX      = AREA_WIDTH / 2 - PLATFORM_WIDTH / 2;
    platformY      = AREA_HEIGHT -  PLATFORM_HEIGHT;
    ballX          = platformX + PLATFORM_WIDTH / 2;
    ballY          = AREA_HEIGHT -  PLATFORM_HEIGHT;
    // init platform
    platform = new QPushButton(this);
    platform->setGeometry(platformX + BALL_RADIUS,
                          platformY + BALL_RADIUS,
                          PLATFORM_WIDTH - BALL_RADIUS * 2,
                          PLATFORM_HEIGHT - BALL_RADIUS * 2);
    platform->setStyleSheet("background-color: rgb(0, 255, 64);"
                            "border-color:  rgb(0, 0, 255); ");
    platform->setVisible(true);
    platform->setEnabled(false);

    // init ball
    ball = new QPushButton(this);
    ball->setGeometry(ballX - BALL_RADIUS,
                      ballY - BALL_RADIUS,
                      BALL_RADIUS * 2,
                      BALL_RADIUS * 2);
    ball->setStyleSheet("background-color: rgb(0, 0, 0);"
                                  "border: none; "
                                  "border-radius: 4px;");

    //calculate and set obj position. User could set own algorithm
    int32_t dh=(AREA_WIDTH - (int32_t)(AREA_WIDTH/OBJ_WIDTH)*OBJ_WIDTH) / 2;
    int32_t objPerLine = (int32_t)(AREA_WIDTH/OBJ_WIDTH);

    int32_t y = 0;
    int32_t n;

    for(int32_t k = 0; k < OBJ_QYANTITY; k++) {
        n = k % objPerLine;

        myObj[k] = new QPushButton(this);
        myObj[k]->setGeometry(dh + shiftBorder + OBJ_WIDTH * n,
                              y + shiftBorder,
                              OBJ_WIDTH - 2 * shiftBorder,
                              OBJ_HEIGHT - 2 * shiftBorder);
        myObj[k]->setStyleSheet("background-color: rgb(200, 100, 180);"
                                "border: none; "
                                "border-radius: 4px;");
        myObj[k]->setVisible(true);
        myObj[k]->setEnabled(false);

        this->elementDesc[k].x1 = dh + OBJ_WIDTH * n;
        this->elementDesc[k].y1 = y;
        this->elementDesc[k].x2 = dh + OBJ_WIDTH * n + OBJ_WIDTH;
        this->elementDesc[k].y2 = y + OBJ_HEIGHT;
        if(n == (objPerLine - 1)) {
            y += OBJ_HEIGHT;
        }
    }
    updateaBallDirection();

    ArkanoidConfig arkanoidConfig;
    arkanoidConfig.elementList     = elementDesc;
    arkanoidConfig.elementQuantity = OBJ_QYANTITY;
    arkanoidConfig.platformPosX    = 0;
    arkanoidConfig.platformPosY    = AREA_HEIGHT - PLATFORM_HEIGHT;
    arkanoidConfig.platformHeight  = PLATFORM_HEIGHT;
    arkanoidConfig.platformWidth   = PLATFORM_WIDTH;
    arkanoidConfig.areaHeight      = AREA_HEIGHT;
    arkanoidConfig.areaWidth       = AREA_WIDTH;
    arkanoidConfig.ballSpeed       = BALL_INIT_SPEED;

    arkanoidH = arkanoidInit(arkanoidConfig);
    gameStatus = GAME_WAIT_START;
    return true;
}

void  MainWindow::updateaBallDirection(void)
{
    dirX = ballX + static_cast<int32_t>(100 * cosf(directionAngle));
    dirY = ballY - static_cast<int32_t>(100 * sinf(directionAngle));
}

void MainWindow::keyEventPos(KeyEven keyEven)
{
    switch(keyEven) {
    case EVENT_PLATFORM_MOVE_LEFT:
        if(gameStatus == GAME_WAIT_START) {
            if((directionAngle + 0.1) < M_PI) {
                directionAngle += 0.1;
            }
            break;
        }

        if(platformX < platformSpeed) {
            return;
        }
        platformX -= platformSpeed;
        platform->move(platform->pos().x() - platformSpeed, platform->pos().y());
        if(gameStatus == GAME_WAIT_START) {
            ballX -=  platformSpeed;
        }
        break;
    case EVENT_PLATFORM_MOVE_RIGHT:
        if(gameStatus == GAME_WAIT_START) {
            if((directionAngle - 0.1) > 0) {
                directionAngle -= 0.1;
            }
            break;
        }

        if(platformX + platformSpeed + PLATFORM_WIDTH > AREA_WIDTH) {
            return;
        }
        platformX += platformSpeed;
        platform->move(platform->pos().x() + platformSpeed, platform->pos().y());
        if(gameStatus == GAME_WAIT_START) {
            ballX +=  platformSpeed;
        }
        break;
    case EVENT_GAME_START:
        gameStatus = GAME_PLAY;
        updateTimer->start();
        break;
    }

    if(gameStatus == GAME_WAIT_START) {
        //ball->move(ballX - BALL_RADIUS, ballY - BALL_RADIUS);
        //arkanoidSetPlatformPos(arkanoidH, ballX, ballY);
        updateaBallDirection();
        this->update();
    }
    arkanoidSetPlatformPos(arkanoidH, platformX, platformY);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->type()==QEvent::KeyPress) {
        QKeyEvent* key = static_cast<QKeyEvent*>(event);
        switch(key->key()) {
        case Qt::Key_Comma:
            keyEventPos(EVENT_PLATFORM_MOVE_LEFT);
            break;
        case Qt::Key_Period:
            keyEventPos(EVENT_PLATFORM_MOVE_RIGHT);
            break;
        case Qt::Key_Q:
            keyEventPos(EVENT_GAME_START);
            break;
        default: break;
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
