#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QList>
#include <QEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

protected:
    void keyPressEvent(QKeyEvent *event);
private:
    Ui::MainWindow *ui;

    QList<QPushButton *> myObj;
    QPushButton *platform;
    QPushButton *ball;
    int32_t   platformX;
    int32_t   platformSpeed;

    bool initGame(void);
    void updatePlatformPos(bool leftShift);

};

#endif // MAINWINDOW_H
