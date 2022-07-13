#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dialogmetahelp.h"

class QStandardItemModel;
class QJsonObject;
class CMDCommand;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
    void sleep(int sec);
    void readCSV(const QString & path);
    bool makeJson(int row,const QString &path);
    bool saveJsonFile(const QJsonObject &obj, const QString &path);
    bool makeLicense();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonUUID_clicked();

    void on_pushButtonCSV_clicked();

    void on_pushButtonMake_clicked();

    void on_pushButtonResult_clicked();

    void on_pushButtonMetaHelp_clicked();

    void on_pushButtonDeamon_clicked();

    void on_pushButtonPictureFile_clicked();

    void on_pushButtonMetaFile_clicked();

    void on_pushButtonNFTID_clicked();

    void on_pushButtonMint_clicked();

    void on_downLoadPercentage(qint64,qint64,int,int);
    void on_downLoadfinished(int row,int colunm);
    void on_downFail(int row,int colunm);
    void on_hashfinishde(int row,int colunm,const QString hash);

    void on_pushButtonMakeCLI_clicked();

    void on_pushButtonMintCheckCLI_clicked();

private:
    Ui::MainWindow *ui;
    QStandardItemModel *MetaModel;
    QStandardItemModel *MintModel;
    DialogMetaHelp * dialogMetaHelp = NULL;
    CMDCommand *cmd;

};
#endif // MAINWINDOW_H
