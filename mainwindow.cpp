#include "mainwindow.h"
#include "ui_mainwindow.h"

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

