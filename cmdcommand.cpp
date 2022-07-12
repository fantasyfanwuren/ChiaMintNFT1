#include "cmdcommand.h"
#include <QProcess>

CMDCommand::CMDCommand(QObject *parent)
    : QObject{parent}
{
    cmd = new QProcess(this);
    arg<<"/c"<<"chia wallet nft mint";
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
    if(f.isEmpty())
        return false;
    fingerprint = f;
    arg<<"-f"<<fingerprint;
    return true;

}


bool CMDCommand::setNFTID(const QString & i)
{
    if(i.isEmpty())
        return false;
    NFTID = i;
    arg<<"-i"<<NFTID;
    return true;
}


bool CMDCommand::setUris(const QStringList &u)
{
    if(u.isEmpty())
        return false;
    uris = u;
    return true;
}


bool CMDCommand::setmetadataUris(const QStringList &mu)
{
    if(mu.isEmpty())
        return false;
    metadataUris = mu;
    return true;
}


bool CMDCommand::setLicenseUris(const QString &lu)
{
    if(lu.isEmpty())
        return false;
    licenseUris = lu;
    arg<<"-lu"<<licenseUris;
    return true;
}


bool CMDCommand::setRoyaltyAddress(const QString &ra)
{
    if(ra.isEmpty())
        return false;
    royaltyAddress = ra;
    arg<<"-ra"<<royaltyAddress;
    return true;
}


bool CMDCommand::setRoyaltyPercentage(const QString &rp)
{
    if(rp.isEmpty())
        return false;
    royaltyPercentage = rp;
    arg<<"-rp"<<royaltyPercentage;
    return true;
}


bool CMDCommand::setTargetAddress(const QString &ta)
{
    if(ta.isEmpty())
        return false;
    targetAddress = ta;
    arg<<"-ta"<<targetAddress;
    return true;
}


bool CMDCommand::setSeriesNumber(const QString &sn)
{
    if(sn.isEmpty())
        return false;
    seriesNumber = sn;
    arg<<"-sn"<<seriesNumber;
    return true;
}


bool CMDCommand::setSeriesTotal(const QString &st)
{
    if(st.isEmpty())
        return false;
    seriesTotal = st;
    arg<<"-st"<<seriesTotal;
    return true;
}

