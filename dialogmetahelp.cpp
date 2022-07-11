#include "dialogmetahelp.h"
#include "ui_dialogmetahelp.h"

DialogMetaHelp::DialogMetaHelp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogMetaHelp)
{
    ui->setupUi(this);
}

DialogMetaHelp::~DialogMetaHelp()
{
    delete ui;
}
