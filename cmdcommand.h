#ifndef CMDCOMMAND_H
#define CMDCOMMAND_H

#include <QObject>
#include <QtNetwork>

class QProcess;

class CMDCommand : public QObject
{
    Q_OBJECT
public:
    explicit CMDCommand(QObject *parent = nullptr);

public:
    bool setWorkDir(const QString & dir);
    bool setFingerprint(const QString &f);
    bool setNFTID(const QString & i);
    bool setUris(const QStringList &u); //利用循环处理
    bool setmetadataUris(const QStringList &mu);//利用循环处理
    bool setLicenseUris(const QString &lu); //简化代码量，减少出错率，同样在循环中处理
    bool setRoyaltyAddress(const QString &ra);
    bool setRoyaltyPercentage(const QString &rp);
    bool setTargetAddress(const QString &ta);
    bool setSeriesNumber(const QString &sn);
    bool setSeriesTotal(const QString &st);
    int run();

private:
    void wait(int sec);
    bool downLoad(const QString & urlSpace);
    QString getSha256(const QByteArray &array) const;

signals:
    void downLoadPercentage(qint64,qint64,int,int);
    void downLoadfinished(int,int);
    void downFail(int,int);
    void hashfinishde(int,int,QString);

private slots:
    void on_downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void on_finished();

private:
    QNetworkReply * reply;
    QNetworkAccessManager networkManager;
    QProcess *cmd;
    QStringList arg;//CMD参数
    QStringList universalArg;//CMD全局参数备份
    QString workDir; //工作环境
    QString fingerprint; //指纹
    QString NFTID; //NFT钱包ID
    QStringList uris; //图片链接列表
    QStringList metadataUris; //元数据链接列表
    QString licenseUris; //许可证列表
    QString royaltyAddress; //版税地址
    QString royaltyPercentage; //版税百分比
    QString targetAddress; //目标地址
    QString seriesNumber; //版本号
    QString seriesTotal; // 总版本号
    bool downLoadState = false;
    int currentRow; //代表列表中的文件
    int currentColumn;//0代表文件，1代表图片哈希，2代表元数据哈希，3代表许可证哈希
};

#endif // CMDCOMMAND_H
