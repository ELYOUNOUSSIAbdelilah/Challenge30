#include "mainwindo.h"
#include "ui_mainwindo.h"

MainWindo::MainWindo(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindo)
{
    ui->setupUi(this);
}

MainWindo::~MainWindo()
{
    delete ui;
}
