#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cmdcommand.h"

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
#include <QClipboard>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //setFixedSize(this->sizeHint());
    setWindowTitle("ChiaMint_"+QApplication::applicationVersion());
    setMinimumSize(sizeHint());
    resize(sizeHint());
    ui->pushButtonMetaHelp->setIcon(QIcon(":/ChiaMintNFT1.ico"));
    ui->pushButtonMintHelp->setIcon(QIcon(":/ChiaMintNFT1.ico"));
    ui->pushButtonMetaVideo->setIcon(QIcon(":/vidio.ico"));
    ui->pushButtonMintVideo->setIcon(QIcon(":/vidio.ico"));
    ui->pushButtonMake->setIcon(QIcon(":/start.ico"));
    ui->pushButtonMint->setIcon(QIcon(":/start.ico"));
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
const QString MainWindow::nameByLength(int i)
{
    QString result;
    QString orignalName = QString::number(i);
    result = orignalName;
    int len = ui->spinBoxTotalNumber->value();
    for(int i=0;i<len-orignalName.count();++i)
        result = "0"+result;
    qDebug()<<"/"+result+".json";
    return "/"+result+".json";


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
        QString name = nameByLength(i);
        QString savePath = metaPath + name;
        QStandardItem * tempitem = MetaModel->item(i,MetaModel->columnCount()-1);
        if(makeJson(i,savePath)){
            tempitem->setText("成功");
        }else{
            tempitem->setText("失败");
            failNumber++;
        }
        //随着程序的进行，对滚动条动态跟随
        ui->tableViewMeta->scrollTo(MetaModel->index(i,0));
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
    ui->pushButtonNFTID->setEnabled(false);
    ui->pushButtonNFTID->setText("查看中..");
    QProcess process(this);
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    process.setWorkingDirectory(ui->lineEditDeamon->text());
    QStringList arg;
    process.start("cmd.exe", arg<<"/c"<<"chia wallet show"<<"-f"<<ui->lineEditFinger->text());
    process.waitForStarted();
    process.waitForFinished();
    QMessageBox::information(this,"信息提醒",QString::fromLocal8Bit(process.readAllStandardOutput()));
    process.kill();
    ui->pushButtonNFTID->setEnabled(true);
    ui->pushButtonNFTID->setText("查看ID");

}

void MainWindow::on_pushButtonNFTIDMake_clicked()
{

    QString finger = ui->lineEditFinger->text();
    QString deamonPath = ui->lineEditDeamon->text();
    QString name = ui->lineEditName->text();
    if(deamonPath.isEmpty() or finger.isEmpty() or name.isEmpty()){
        QMessageBox::information(this,"警告","必须设置以下内容：\n"
                                           "Chia deamon(本页面上方)\n"
                                           "钱包指纹(本页面下方)\n"
                                           "名称（上个页面中部）\n"
                                           "才能使用该功能！");
        return;
    }
    ui->pushButtonNFTIDMake->setEnabled(false);
    ui->pushButtonNFTIDMake->setText("正在创建..");

    QProcess process(this);
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    process.setWorkingDirectory(deamonPath);
    QStringList arg;

    // chia wallet did create -n My_DID
    //创建DID
    arg<<"/c"<<"chia wallet did create"<<"-n"<<name<<"-f"<<finger;
    process.start("cmd.exe", arg);
    process.waitForStarted();
    process.waitForFinished();
    QString outPut = QString::fromLocal8Bit(process.readAllStandardOutput());
    qDebug()<<outPut;
    int position = outPut.indexOf("did:chia:");
    QString DID = outPut.mid(position,68);
    qDebug()<<DID;
    arg.clear();

    /* chia wallet nft create -f 4288332900
     * -di did:chia:17jvhl9z8zj6jma2uxk4mqj22p90hfpf29svlvlyalu8ksyefsvpql7f403
     * -n "DID-linked NFT Wallet" */
    //创建NFT钱包
    arg<<"/c"<<"chia wallet nft create"<<"-n"<<name+"_NFT_Wallet"<<"-f"<<finger
      <<"-di"<<DID;
    qDebug()<<arg;
    process.start("cmd.exe", arg);
    process.waitForStarted();
    process.waitForFinished();
    outPut = QString::fromLocal8Bit(process.readAllStandardOutput());
    qDebug()<<outPut;
    arg.clear();

    //查看DID
    process.start("cmd.exe", arg<<"/c"<<"chia wallet show"<<"-f"<<ui->lineEditFinger->text());
    process.waitForStarted();
    process.waitForFinished();
    QMessageBox::information(this,"信息提醒",QString::fromLocal8Bit(process.readAllStandardOutput()));
    process.kill();
    ui->pushButtonNFTIDMake->setEnabled(true);
    ui->pushButtonNFTIDMake->setText("创建NFT钱包");

}


void MainWindow::on_downLoadPercentage(qint64 bytesReceived,qint64 bytesTotal,int row ,int column)
{
    Q_UNUSED(bytesTotal);
    //QStandardItem *item = new QStandardItem("正在下载："+percentage);
    qDebug()<<"gui bytesReceived"<<bytesReceived;
    QStandardItem *item = MintModel->item(row,column);
    item->setText("正在下载："+QString::number(bytesReceived));
    //随着程序的进行，对滚动条动态跟随
    ui->tableViewMint->scrollTo(MintModel->index(row,column));
}

void MainWindow::on_downLoadfinished(int row,int column)
{
    QStandardItem *item = MintModel->item(row,column);
    item->setText("下载完成");
    //随着程序的进行，对滚动条动态跟随
    ui->tableViewMint->scrollTo(MintModel->index(row,column));
}

void MainWindow::on_downFail(int row,int column)
{
    QStandardItem *item = MintModel->item(row,column);
    item->setText("下载失败");
    //随着程序的进行，对滚动条动态跟随
    ui->tableViewMint->scrollTo(MintModel->index(row,column));
}


void MainWindow::on_hashfinishde(int row,int column,const QString hash)
{
    QStandardItem *item = MintModel->item(row,column);
    item->setText(hash);
    //随着程序的进行，对滚动条动态跟随
    ui->tableViewMint->scrollTo(MintModel->index(row,column));
}




void MainWindow::on_pushButtonMakeCLI_clicked()
{

    if(ui->lineEditPictureFile->text().isEmpty()){
        QMessageBox::information(this,"提醒","请设置图片集合");
        return;
    }
    if(ui->lineEditPictureLink->text().isEmpty()){
        QMessageBox::information(this,"提醒","请设置图片集链接");
        return;
    }
    if(ui->lineEditMetaFile->text().isEmpty()){
        QMessageBox::information(this,"提醒","请设置元数据Json集合");
        return;
    }
    if(ui->lineEditMetaLink->text().isEmpty()){
        QMessageBox::information(this,"提醒","请设置元数据Json集链接");
        return;
    }
    ui->pushButtonMakeCLI->setEnabled(false);
    ui->pushButtonMakeCLI->setText("正在生成...");
    //创建一个CMDCommand对象,并设置工作环境（必选）
    cmd = new CMDCommand(this);
    if(!cmd->setWorkDir(ui->lineEditDeamon->text())){
        QMessageBox::information(this,"提醒","请设置Chia Deamon");
        return;
    }

    //添加图片列表（必选）
    QDir pictureDir(ui->lineEditPictureFile->text());
    QStringList pictureList = pictureDir.entryList(QDir::Files);
    QStringList pictureUris;
    for(int i=0;i<pictureList.count();++i)
        pictureUris<<ui->lineEditPictureLink->text()+pictureList.at(i);
    cmd->setUris(pictureUris);

    //添加元数据列表（必选）
    QDir metaDir(ui->lineEditMetaFile->text());
    QStringList metaList = metaDir.entryList(QDir::Files);
    QStringList metaUris;
    for(int i=0;i<metaList.count();++i){
        metaUris<<ui->lineEditMetaLink->text() + metaList.at(i);
    }
    cmd->setmetadataUris(metaUris);

    //许可证的设置(选填)
    if(!ui->lineEditLicenceLink->text().isEmpty()){
        cmd->setLicenseUris(ui->lineEditLicenceLink->text());
    }

    //设置指纹（必选）
    if(ui->lineEditFinger->text().isEmpty()){
        QMessageBox::information(this,"提醒","请设置指纹");
        return;
    }
    cmd->setFingerprint(ui->lineEditFinger->text());

    //设置NFT钱包ID(必选)
    if(ui->lineEditNFTID->text().isEmpty()){
        QMessageBox::information(this,"提醒","请设置NFT钱包ID");
        return;
    }
    cmd->setNFTID(ui->lineEditNFTID->text());

    //设置目标地址（必选）
    if(ui->lineEditTargetAddress->text().isEmpty()){
        QMessageBox::information(this,"提醒","请设置目标地址");
        return;
    }
    cmd->setTargetAddress(ui->lineEditTargetAddress->text());

    //设置版税地址（必选）
    if(ui->lineEditTaxAddress->text().isEmpty()){
        QMessageBox::information(this,"提醒","请设置版税地址");
        return;
    }
    cmd->setRoyaltyAddress(ui->lineEditTaxAddress->text());

    //设置总版本号(选填)
    if(!ui->lineEditTotalVersion->text().isEmpty()){
        cmd->setSeriesTotal(ui->lineEditTotalVersion->text());
    }
    //设置当前版本号（选填）
    if(!ui->lineEditCurrentVersion->text().isEmpty()){
        cmd->setSeriesNumber(ui->lineEditCurrentVersion->text());
    }
    //设置版税
    int royalty = ui->spinBoxTaxRate->value();
    cmd->setRoyaltyPercentage(QString::number(royalty));

    //信号与槽的链接，用于gui的显示
    MintModel = new QStandardItemModel;
    MintModel->setHorizontalHeaderLabels(
                QStringList()<<"图片名称"<<"图片哈希"<<"元数据哈希"<<"许可证哈希"<<"执行");
    for(int i=0;i<pictureList.count();++i){
        QStandardItem *theItem = new QStandardItem(pictureList.at(i));
        MintModel->setItem(i,0,theItem);
        for(int j=1;j<=4;j++){
            QStandardItem *aItem = new QStandardItem("");
            MintModel->setItem(i,j,aItem);
        }
    }
    ui->tableViewMint->setModel(MintModel);
    ui->tableViewMint->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(cmd,SIGNAL(downLoadPercentage(qint64,qint64,int,int))
            ,this,SLOT(on_downLoadPercentage(qint64,qint64,int,int)));
    connect(cmd,SIGNAL(downLoadfinished(int,int))
            ,this,SLOT(on_downLoadfinished(int,int)));
    connect(cmd,SIGNAL(downFail(int,int))
            ,this,SLOT(on_downFail(int,int)));
    connect(cmd,SIGNAL(hashfinishde(int,int,QString))
            ,this,SLOT(on_hashfinishde(int,int,QString)));
    int whenOut = cmd->makeCLI();
    if(whenOut == 0){
        QMessageBox::information(this,"提醒","已生成");
        ui->pushButtonMakeCLI->setText("命令列表已生成");
    }else{
        QMessageBox::information(this,"提醒","网络链接不稳定或链接填写错误");
        ui->pushButtonMakeCLI->setText("生成指令");
        ui->pushButtonMakeCLI->setEnabled(true);
    }
    ui->pushButtonMintCheckCLI->setEnabled(true);


}


void MainWindow::on_pushButtonMintCheckCLI_clicked()
{
    checkDialog = new DialogMetaHelp;
    checkDialog->setAttribute(Qt::WA_DeleteOnClose);
    checkDialog->setTitle("检查指令");
    checkDialog->setText(cmd->checkCLI());
    checkDialog->show();
    ui->pushButtonMint->setEnabled(true);
}


void MainWindow::on_mintMessage(int row,QString str)
{
    QStandardItem * theItem = MintModel->item(row,MintModel->columnCount()-1);
    theItem->setText(str);
    MintModel->setItem(row,MintModel->columnCount()-1,theItem);
    //随着程序的进行，对滚动条动态跟随
    ui->tableViewMint->scrollTo(MintModel->index(row,MintModel->columnCount()-1));
}
void MainWindow::on_pushButtonMint_clicked()
{
    ui->pushButtonMint->setEnabled(false);
    connect(cmd,SIGNAL(mintMessage(int,QString))
            ,this,SLOT(on_mintMessage(int,QString)));
    cmd->run();
}

void MainWindow::on_pushButtonMintVideo_clicked()
{
    QMessageBox::information(this,"提醒","视频教程正在制作。\n"
                                       "怕走丢，关注B站UP：中国数字党\n"
                                       "已将地址复制到您的剪切版");
    QClipboard *board = QApplication::clipboard();
    board->setText("https://space.bilibili.com/407407190");
}


void MainWindow::on_pushButtonMetaVideo_clicked()
{
    QMessageBox::information(this,"提醒","视频教程正在制作。\n"
                                       "怕走丢，关注B站UP：中国数字党\n"
                                       "已将地址复制到您的剪切版");
    QClipboard *board = QApplication::clipboard();
    board->setText("https://space.bilibili.com/407407190");
}







void MainWindow::on_pushButtonMintHelp_clicked()
{
    QString path =":/help/MintHelp.txt";
    QFile file(path);
    if(!file.exists()){
        QMessageBox::information(this,"提示","未找到MintHelp帮助文档");
        return;
    }
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        QMessageBox::information(this,"提示","MintHelp帮助文档打开失败");
        return;
    }
    QString helpText = file.readAll();
    file.close();
    dialogMetaHelp = new DialogMetaHelp;
    dialogMetaHelp->setAttribute(Qt::WA_DeleteOnClose);
    //Qt::WindowFlags flags = dialogMetaHelp->windowFlags();
    //dialogMetaHelp->setWindowFlags(flags|Qt::WindowStaysOnTopHint);
    dialogMetaHelp->setText(helpText);
    dialogMetaHelp->setWindowTitle("MintHelp帮助文档");
    dialogMetaHelp->show();
}

