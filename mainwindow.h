#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QList>
#include <QEvent>
#include <QTimer>
#include <QPainter>

extern "C" {
#include "arkonoidEngeen/arkanoidEngeen.h"
}
#include "arkanoidsetings.h"

typedef enum{
    GAME_PLAY,
    GAME_WAIT_START,
    GAME_PAUSE
}GameStatus;

typedef enum{
    EVENT_PLATFORM_MOVE_LEFT,
    EVENT_PLATFORM_MOVE_RIGHT,
    EVENT_GAME_START,
    EVENT_GAME_STEP,
}KeyEven;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event);
private:
    Ui::MainWindow *ui;

    QTimer *updateTimer;
    QLine  *myLine;

    GameStatus gameStatus;
    QPushButton *myObj[OBJ_QYANTITY];
    QPushButton *platform;
    QPushButton *ball;
    uint32_t   platformX;
    uint32_t   platformY;
    uint32_t   ballX;
    uint32_t   ballY;
    uint32_t   platformSpeed;
    uint32_t   gameTimeMs;

    bool initGame(void);
    void keyEventPos(KeyEven keyEven);
    void updateState(void);
    void updateaBallDirection(void);

    // Arkanoid engeen elements descriptions
    ElementDescr elementDesc[OBJ_QYANTITY];
    ArkanoidH arkanoidH;
    // direction of ball
    int32_t dirX;
    int32_t dirY;
    float  lineDirectionAngle;
    float  arkanoidDirectionAngle;
    //Current game state
    int32_t xBall_0;
    int32_t xBall_1;
    int32_t yBall_0;
    int32_t yBall_1;
protected:
    void paintEvent(QPaintEvent *event);
};

#endif // MAINWINDOW_H
