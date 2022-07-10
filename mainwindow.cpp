#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProcess>
#include <windows.h>
#include <QTime>
#include <QFileDialog>
#include <QMessageBox>

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

void MainWindow::readCSV(const QString & path)
{
    qDebug()<<path;
    qDebug()<<"23点10分,明天继续撸代码";
}

void MainWindow::on_pushButtonCSV_clicked()
{
    /*点击后将让用户选择自己的元数据表格，
     * 这个表格是用另外一款图片合成软件创建的
     * https://github.com/Jianghuchengphilip/Generate-NFT.git
     * 除了提示表格文件的路径之外，还需要调用readCSV函数来读取这个表格内容*/

    QString csvFile = QFileDialog::getOpenFileName(this,tr("打开元数据表格")
                                                   ,QDir::currentPath(),
                                                   tr("CSV文件(*.csv)"));
    if(csvFile.isEmpty()){
        QMessageBox::information(this,"提示","获取失败");
    }else{
        ui->lineEditCSV->setText(csvFile);
    }

}

