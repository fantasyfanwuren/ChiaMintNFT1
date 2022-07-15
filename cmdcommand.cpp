#include "cmdcommand.h"
#include <QProcess>
#include <QEventLoop>
#include <QTimer>
#include <QUrl>
#include <QCryptographicHash>



CMDCommand::CMDCommand(QObject *parent)
    : QObject{parent}
{
    cmd = new QProcess(this);
    universalArg<<"/c"<<"chia wallet nft mint";
}


bool CMDCommand::setWorkDir(const QString & dir)
{
    if(dir.isEmpty())
        return false;
    workDir = dir;
    cmd->setWorkingDirectory(workDir);
    return true;
}


bool CMDCommand::setFingerprint(const QString &f)
{
    /*
     * 设置指纹
     */
    if(f.isEmpty())
        return false;
    fingerprint = f;
    universalArg<<"-f"<<fingerprint;
    return true;

}


bool CMDCommand::setNFTID(const QString & i)
{
    /*
     * 设置NFT钱包ID
     */
    if(i.isEmpty())
        return false;
    NFTID = i;
    universalArg<<"-i"<<NFTID;
    return true;
}


bool CMDCommand::setUris(const QStringList &u)
{
    /*
     * 设置图片链接集合
     */
    if(u.isEmpty())
        return false;
    uris = u;
    return true;
}


bool CMDCommand::setmetadataUris(const QStringList &mu)
{
    /*
     * 设置元数据链接集合
     */
    if(mu.isEmpty())
        return false;
    metadataUris = mu;
    return true;
}


bool CMDCommand::setLicenseUris(const QString &lu)
{
    /*
     * 设置许可证
     */
    if(lu.isEmpty())
        return false;
    licenseUris = lu;
    universalArg<<"-lu"<<licenseUris;
    return true;
}


bool CMDCommand::setRoyaltyAddress(const QString &ra)
{
    /*
     * 设置版税接受地址
     */
    if(ra.isEmpty())
        return false;
    royaltyAddress = ra;
    universalArg<<"-ra"<<royaltyAddress;
    return true;
}


bool CMDCommand::setRoyaltyPercentage(const QString &rp)
{
    /*
     * 设置版税比例
     */
    if(rp.isEmpty())
        return false;
    royaltyPercentage = rp;
    universalArg<<"-rp"<<royaltyPercentage;
    return true;
}


bool CMDCommand::setTargetAddress(const QString &ta)
{
    /*
     * 设置目标地址
     */
    if(ta.isEmpty())
        return false;
    targetAddress = ta;
    universalArg<<"-ta"<<targetAddress;
    return true;
}


bool CMDCommand::setSeriesNumber(const QString &sn)
{
    /*
     * 设置版本号
     */
    if(sn.isEmpty())
        return false;
    seriesNumber = sn;
    universalArg<<"-sn"<<seriesNumber;
    return true;
}


bool CMDCommand::setSeriesTotal(const QString &st)
{
    /*
     * 设置总版本号
     */
    if(st.isEmpty())
        return false;
    seriesTotal = st;
    universalArg<<"-st"<<seriesTotal;
    return true;
}

void CMDCommand::on_downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    /*
     * 通过信号downLoadPercentage，向GUI发送下载过程的各项信息，方便在GUI显示
     */
    emit downLoadPercentage(bytesReceived,bytesTotal,currentRow,currentColumn);
}


QString CMDCommand::getSha256(const QByteArray &array) const
{
    /*
     * 一个计算哈希的小函数
     */
    QCryptographicHash hash(QCryptographicHash::Sha256);
        hash.addData(array);  // 添加数据到加密哈希值
        QByteArray result = hash.result();  // 返回最终的哈希值
        QString strSha256 = result.toHex();
        return strSha256;
}

void CMDCommand::on_finished()
{
    /*
     * 下载完成后将根据currentColumn的值确定是下载的图片、元数据或者许可证
     * 计算哈希，并对arg进行更新
     * 发出哈希计算完成信号，将currentRow,currentColumn和哈希值发送出去，方便界面显示
     * 清空下载请求指针
     */
    emit downLoadfinished(currentRow,currentColumn);
    downLoadState = true;
    QByteArray array = reply->readAll();
    QString sha256 = getSha256(array);
    emit hashfinishde(currentRow,currentColumn,sha256);
    //0代表文件，1代表图片哈希，2代表元数据哈希，3代表许可证哈希
    if(currentColumn==1){
        arg<<"-nh"<<sha256;
    }else if(currentColumn == 2){
        arg<<"-mh"<<sha256;
    }else if(currentColumn == 3){
        arg<<"-lh"<<sha256;
    }
    reply->deleteLater();
    downLoadState = false;
}

void CMDCommand::wait(int sec)
{
    /*
     * 一个延迟小函数，方便输出下载超时信息
     */
    for(int i=0;i<sec;++i){
            QEventLoop eventloop;
            QTimer::singleShot(1000, &eventloop, SLOT(quit()));
            eventloop.exec();
        }
}


bool CMDCommand::downLoad(const QString & urlSpace)
{
    /* 根据具体的链接文本，对链接进行下载，
     * 若下载成功，链接槽函数on_finished()
     * 下载过程链接槽函数on_downloadProgress(qint64, qint64);
     * 若失败将发送downFail(currentRow,currentColumn)
     */
    if(urlSpace.isEmpty()){
        emit downFail(currentRow,currentColumn);
        return false;
    }
    QUrl newUrl = QUrl(urlSpace);
    if(!newUrl.isValid()){
        emit downFail(currentRow,currentColumn);
        return false;
    }
    qDebug()<<urlSpace;
    downLoadState = true;
    reply = networkManager.get(QNetworkRequest(newUrl));
    connect(reply,SIGNAL(finished()),this,SLOT(on_finished()));
    connect(reply,SIGNAL(downloadProgress(qint64, qint64))
                ,this,SLOT(on_downloadProgress(qint64, qint64)));
    int timeout = 30;
    for(int i=0;i<timeout;++i){
            wait(1);
            if(downLoadState == false){
                break;
            }
            if(i==timeout-1){
                emit downFail(currentRow,currentColumn);
                downLoadState = false;
                return false;
            }
    }

    return true;

}


int CMDCommand::makeCLI()
{
    /* 在进行run之前，必须提前进行所有的set功能，完成对universalArg的初始化
     * 然后进行 row循环，内嵌column循环，
     * 在row循环中，需要对arg清空并赋值 universalArg
     * column循环中从1到3；分别为：1代表图片哈希，2代表元数据哈希，3代表许可证哈希
     *
     * 要点1：图片，元数据的列表级其他universalArg在外部创建
     * 要点2：要时刻控制 currentRow,currentColumn两个变量，方便信号的输出和对全局的把控
     * 返回值：0成功，1图片无法下载，2元数据无法下载，3许可证无法下载
     */
    result.clear();
    //for(currentRow = 0;currentRow<uris.count();++currentRow){
    for(currentRow = 0;currentRow<uris.count();++currentRow){
        arg.clear();
        arg = universalArg;
        qDebug()<<"初始化的arg:"<<arg;
        currentColumn = 1;//图片
        arg<<"-u"<<uris.at(currentRow);
        if(!downLoad(uris.at(currentRow)))
            return 1;

        currentColumn = 2; //元数据
        arg<<"-mu"<<metadataUris.at(currentRow);
        if(!downLoad(metadataUris.at(currentRow)))
            return 2;

        currentColumn = 3; //许可证
        if(!downLoad(licenseUris))
            return 3;
        //执行cmd命名
        result<<arg;
    }
    return 0;
}

QString CMDCommand::checkCLI()
{
    QString checkString;
    for(int i=0;i<result.count();++i){
        for(int j=1;j<result.at(i).count();++j){
            checkString+=result.at(i).at(j)+" ";
        }
        checkString+="\n\n";
    }
    return checkString;
}


void CMDCommand::run()
{
    QString outPut;
    int number = result.count();
    for(int i=0;i<number;++i){

        while (true) {
            cmd->start("cmd.exe",result.at(i));
            cmd->waitForStarted();
            cmd->waitForFinished();
            outPut = QString::fromLocal8Bit(cmd->readAllStandardOutput());
            qDebug()<<outPut;
            if(outPut.contains("NFT minted Successfully")){
                emit mintMessage(i,"铸造成功");
                break;
            }else{
                emit mintMessage(i,"等待上次铸造同步...");
            }
            wait(30);
        }

    }
}

