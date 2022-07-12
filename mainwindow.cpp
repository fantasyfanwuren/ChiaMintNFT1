#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QProcess>
#include <windows.h>
#include <QTime>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QStandardItemModel>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDesktopServices>

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

bool MainWindow::saveJsonFile(const QJsonObject &obj, const QString &path)
{
    QJsonDocument doc(obj);
    QByteArray docByte = doc.toJson();
    QFile fp(path);
    if (!fp.open(QIODevice::WriteOnly)){
        QMessageBox::information(this,"错误","写入Json失败！");
        return false;
    }
    fp.write(docByte);
    fp.close();
    return true;

}

bool MainWindow::makeJson(int row,const QString &path)
{
    /* 将gui中的meta表格生成json文件
     * 保存在path文件夹中*/

    QJsonObject obj{
        {"format", "CHIP-0007"},
    };
    //"name": "Chia Friends #1",
    QString name = ui->lineEditName->text()+
            " #"+MetaModel->item(row,0)->text();
    obj.insert("name",name);

    //"description": "10,000 eco-bit NFTs on Chia, from Chia."
    QString description = ui->lineEditDescription->text();
    obj.insert("description",description);

    // "sensitive_content": false
    obj.insert("sensitive_content",false);

    //"collection": {}
    QJsonObject collection;
        //"name": "Chia Friends", "id": "517B1E97-F1AF-4824-A7B9-8D85E281D7B8",
    collection.insert("name",ui->lineEditName->text());
    collection.insert("id",ui->lineEditUUID->text());
        //"attributes": []
    QJsonArray attributes{
        QJsonObject{
            {"type","description"},
            {"value",description}
        }
    };
    QString website = ui->lineEditWebsite->text();
    QString discord = ui->lineEditDiscord->text();
    QString twitter = ui->lineEditTwitter->text();

    if(!website.isEmpty()){
        QJsonObject websiteObj{
            {"type","website"},
            {"value",website}
        };
        attributes.append(websiteObj);
    }

    if(!discord.isEmpty()){
        QJsonObject discordObj{
            {"type","discord"},
            {"value",discord}
        };
        attributes.append(discordObj);
    }

    if(!twitter.isEmpty()){
        QJsonObject twitterObj{
            {"type","twitter"},
            {"value",twitter}
        };
        attributes.append(twitterObj);
    }

    collection.insert("attributes",attributes);
    obj.insert("collection",collection);

    //"attributes": []
    QJsonArray theAttributes;
    /* 在attributes中添加每个用户自定义属性，
     * 首先获取表头
     * 再获取所在行的内容 */
    for(int i=1;i<MetaModel->columnCount()-1;++i){
        QString trait_type = MetaModel->horizontalHeaderItem(i)->text();
        QString value = MetaModel->item(row,i)->text();
        QJsonObject tempObj{
            {"trait_type",trait_type},
            {"value",value}
        };
        theAttributes.append(tempObj);
    }
    obj.insert("attributes",theAttributes);

    if(!saveJsonFile(obj,path))
        return false;
    else
        return true;

}

void MainWindow::on_pushButtonMake_clicked()
{
    /* 在程序所在目录创建一个<Meta>文件夹
     * 创建每一行数据对应的元数据文件
     * 保存元数据文件，并以表格第一列作为名称
     * 在成语所在目录创建一个<License>文件夹
     * 创建License.txt,并根据模板修改 */

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
    int failNumber = 0;
    for(int i=0;i<MetaModel->rowCount();++i){
        QString savePath = metaPath + QString::asprintf("/%d,json",i);
        QStandardItem * tempitem = MetaModel->item(i,MetaModel->columnCount()-1);
        if(makeJson(i,savePath)){
            tempitem->setText("成功");
        }else{
            tempitem->setText("失败");
            failNumber++;
        }
    }
    if(failNumber==0 and makeLicense()){
        QMessageBox::information(this,"提示","元数据Json与许可证Json生成已完成");
    }else{
        QMessageBox::information(this,"警告","元数据Json与许可证Json生成过程存在失败情况");
    }

    ui->pushButtonMake->setEnabled(true);
    ui->pushButtonResult->setEnabled(true);

}
bool MainWindow::makeLicense()
{

    QString licensePath = QDir::currentPath() + "/License";
    QDir licenseDir(licensePath);
    if(licenseDir.exists()){//若文件夹已经存在就删除掉
        if(!licenseDir.removeRecursively()){
            QMessageBox::information(this,"提示","已存在的License文件夹无法删除");
            ui->pushButtonMake->setEnabled(true);
            return false;
        }
    }

    if(!licenseDir.mkdir(licensePath)){//无论文件夹存在与否，都需要创建新文件夹
        QMessageBox::information(this,"提示","无法创建License目录");
        ui->pushButtonMake->setEnabled(true);
        return false;
    }

    QString licenseTextPath = licensePath +"/License.txt";
    QFile licensefile(licenseTextPath);
    QFile licensefileTemplate(":/License/License.txt");
    if(!licensefileTemplate.open(QIODevice::ReadOnly|QIODevice::Text)){
        QMessageBox::information(this,"提示","获取许可证模板失败");
        return false;
    }
    QString templateText = licensefileTemplate.readAll();
    licensefileTemplate.close();

    QString licenseText = templateText.replace("MyNFTNAME"
                                               ,ui->lineEditName->text());


    if(!licensefile.open(QIODevice::WriteOnly|QIODevice::Text)){
        QMessageBox::information(this,"提示","创建许可证文件失败");
        return false;
    }
    QTextStream tempStream(&licensefile);
    tempStream<<licenseText;
    licensefile.close();
    return true;
}

void MainWindow::on_pushButtonResult_clicked()
{

    ui->pushButtonResult->setEnabled(false);
    int waitTime = 3000;
    QString path = QDir::currentPath() + "/Meta";
    QDir metaDir(path);
    if(metaDir.exists()){
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));//支持中文路径
    }else{
        QMessageBox::information(this,"提示","未生成Meta文件夹，无法查看");
    }
    sleep(waitTime);
    ui->pushButtonResult->setEnabled(true);
}


void MainWindow::on_pushButtonMetaHelp_clicked()
{
    QString path =":/help/MetaHelp.txt";
    QFile file(path);
    if(!file.exists()){
        QMessageBox::information(this,"提示","未找到MetaHelp帮助文档");
        return;
    }
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        QMessageBox::information(this,"提示","MetaHelp帮助文档打开失败");
        return;
    }
    QString helpText = file.readAll();
    file.close();
    dialogMetaHelp = new DialogMetaHelp;
    dialogMetaHelp->setAttribute(Qt::WA_DeleteOnClose);
    //Qt::WindowFlags flags = dialogMetaHelp->windowFlags();
    //dialogMetaHelp->setWindowFlags(flags|Qt::WindowStaysOnTopHint);
    dialogMetaHelp->setText(helpText);
    dialogMetaHelp->show();
}


void MainWindow::on_pushButtonDeamon_clicked()
{
    QString deamonPath = QFileDialog::getExistingDirectory(this,"选择文件夹","");
    if(deamonPath.isEmpty()){
        return;
    }
    ui->lineEditDeamon->setText(deamonPath);
}



void MainWindow::on_pushButtonPictureFile_clicked()
{
    QString picturePath = QFileDialog::getExistingDirectory(this,"选择文件夹","");
    if(!picturePath.isEmpty())
        ui->lineEditPictureFile->setText(picturePath);
}



void MainWindow::on_pushButtonMetaFile_clicked()
{
    QString metaPath = QFileDialog::getExistingDirectory(this,"选择文件夹","");
    if(!metaPath.isEmpty())
        ui->lineEditMetaFile->setText(metaPath);
}


void MainWindow::on_pushButtonLicenceFile_clicked()
{
    QString licensePath = QFileDialog::getExistingDirectory(this,"选择文件夹","");
    if(!licensePath.isEmpty())
        ui->lineEditLicenceFile->setText(licensePath);
}


void MainWindow::on_pushButtonNFTID_clicked()
{
    /* 首先确认用户是否设置了环境和指纹
     * 然后执行 chia wallet show 指令
     * 过程中可能需要选择钱包，需要考虑这一点。*/
    QString finger = ui->lineEditFinger->text();
    QString deamonPath = ui->lineEditDeamon->text();
    if(deamonPath.isEmpty()){
        QMessageBox::information(this,"警告","必须设置Chia deamon才能使用该功能！");
        return;
    }
    if(finger.isEmpty()){
        QMessageBox::information(this,"警告","必须设置钱包指纹才能使用该功能！");
        return;
    }
    QProcess process(0);
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    process.setWorkingDirectory(ui->lineEditDeamon->text());
    QStringList arg;
    process.start("cmd.exe", arg<<"/c"<<"chia wallet show"<<"-f"<<ui->lineEditFinger->text());
    process.waitForStarted();
    process.waitForFinished();
    QMessageBox::information(this,"信息提醒",QString::fromLocal8Bit(process.readAllStandardOutput()));
    process.kill();

}

