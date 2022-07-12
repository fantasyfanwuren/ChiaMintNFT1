#ifndef CMDCOMMAND_H
#define CMDCOMMAND_H

#include <QObject>

class QProcess;

class CMDCommand : public QObject
{
    Q_OBJECT
public:
    explicit CMDCommand(QObject *parent = nullptr);

private:
    bool setWorkDir(const QString & dir);
    bool setFingerprint(const QString &f);
    bool setNFTID(const QString & i);
    bool setUris(const QStringList &u);
    bool setmetadataUris(const QStringList &mu);
    bool setLicenseUris(const QString &lu);
    bool setRoyaltyAddress(const QString &ra);
    bool setRoyaltyPercentage(const QString &rp);
    bool setTargetAddress(const QString &ta);
    bool setSeriesNumber(const QString &sn);
    bool setSeriesTotal(const QString &st);

signals:
private:
    QProcess *cmd;
    QStringList arg;//CMD参数
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
};

#endif // CMDCOMMAND_H
