#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dialogmetahelp.h"

class QStandardItemModel;
class QJsonObject;

extern QString version;

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

private:
    Ui::MainWindow *ui;
    QStandardItemModel *MetaModel;
    QStandardItemModel *MintModel;
    DialogMetaHelp * dialogMetaHelp = NULL;

};
#endif // MAINWINDOW_H
