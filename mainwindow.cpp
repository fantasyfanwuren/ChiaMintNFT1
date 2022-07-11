#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProcess>
#include <windows.h>
#include <QTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QStandardItemModel>

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

    int waitTime = 10000;
    QString UUIDwebsite = "https://www.uuidgenerator.net/";

    ui->pushButtonUUID->setEnabled(false);
    QProcess cmd(this);
    QStringList arguments;
    arguments<<"/c"<<"start"<<UUIDwebsite;
    cmd.start("cmd.exe",arguments);
    cmd.waitForStarted(waitTime);
    cmd.waitForFinished(waitTime);
    sleep(waitTime);
    ui->pushButtonUUID->setEnabled(true);
}

void MainWindow::readCSV(const QString & path)
{
    /* 读取Meta.csv文件
     * 将其内容提取到ui->tableViewMeta中 */

    MetaModel = new QStandardItemModel;
    QStandardItem * tempItem;
    QFile file(path);
    if(file.open(QIODevice::ReadOnly)){
        QTextStream out(&file);
        QStringList LineText = out.readAll().split("\r\n");//读取每一行
        LineText.removeLast();
        for(int i=0;i<LineText.count();++i){
            QStringList text = LineText.at(i).split(',');//将每一行分开
            if(i==0){//设置表头
                text<<"运行状况";
                MetaModel->setHorizontalHeaderLabels(text);
            }else{//设置表格内容
                text<<"待生成";
                for(int j=0;j<text.count();++j){
                    tempItem = new QStandardItem(text.at(j));
                    MetaModel->setItem(i-1,j,tempItem);
                }
            }
        }
    }

    ui->tableViewMeta->setModel(MetaModel);
    ui->tableViewMeta->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void MainWindow::on_pushButtonCSV_clicked()
{
    /* 点击后将让用户选择自己的元数据表格，
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
        readCSV(csvFile);
    }

}


void MainWindow::makeJson(QStandardItemModel * theModel,const QString &path)
{
    /* 将gui中的meta表格生成json文件
     * 保存在path文件夹中*/
    //qDebug()<<theModel;
    qDebug()<<path;

}

void MainWindow::on_pushButtonMake_clicked()
{
    /* 在程序所在目录创建一个<Meta>文件夹
     * 创建每一行数据对应的元数据文件
     * 保存元数据文件，并以表格第一列作为名称 */

    ui->pushButtonMake->setEnabled(false);

    QString metaPath = QDir::currentPath() + "/Meta";
    QDir metaDir(metaPath);
    if(metaDir.exists()){//若文件夹已经存在就删除掉
        if(!metaDir.removeRecursively()){
            QMessageBox::information(this,"提示","已存在的Meta文件夹无法删除");
            ui->pushButtonMake->setEnabled(true);
            return;
        }
    }

    if(!metaDir.mkdir(metaPath)){//无论文件夹存在与否，都需要创建新文件夹
        QMessageBox::information(this,"提示","无法创建Meta目录");
        ui->pushButtonMake->setEnabled(true);
        return;
    }

    makeJson(MetaModel,metaPath);
    ui->pushButtonMake->setEnabled(true);

}

