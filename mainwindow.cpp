#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProcess>
#include <windows.h>
#include <QTime>

QString version = "V1.0";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(this->sizeHint());
    setWindowTitle("ChiaMint_"+version);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sleep(int sec)
{
    /*一个起到延迟作用的函数，
     * 防止用户在需要等待的时间里面乱点，*/

    QTime reachTime = QTime::currentTime().addMSecs(sec);
    while(QTime::currentTime()<reachTime)
        QApplication::processEvents(QEventLoop::AllEvents,100);
}

void MainWindow::on_pushButtonUUID_clicked()
{
    /*通过后台调用CMD指令：start https://www.uuidgenerator.net/
     让用户从这个网站获取UUID*/

    int waitTinme = 10000;
    QString UUIDwebsite = "https://www.uuidgenerator.net/";

    ui->pushButtonUUID->setEnabled(false);
    QProcess cmd(this);
    QStringList arguments;
    arguments<<"/c"<<"start"<<UUIDwebsite;
    cmd.start("cmd.exe",arguments);
    cmd.waitForStarted(waitTinme);
    cmd.waitForFinished(waitTinme);
    sleep(waitTinme);
    ui->pushButtonUUID->setEnabled(true);
}

