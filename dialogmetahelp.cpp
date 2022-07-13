#include "dialogmetahelp.h"
#include "ui_dialogmetahelp.h"

DialogMetaHelp::DialogMetaHelp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMetaHelp)
{
    ui->setupUi(this);
    setWindowTitle("Meta帮助文档");

}

DialogMetaHelp::~DialogMetaHelp()
{
    delete ui;
}

void DialogMetaHelp::setText(const QString &text)
{
    ui->plainTextEdit->setPlainText(text);
}

void DialogMetaHelp::on_pushButton_2_clicked()
{
    close();
}

void DialogMetaHelp::setTitle(const QString &title)
{
    setWindowTitle(title);
}

