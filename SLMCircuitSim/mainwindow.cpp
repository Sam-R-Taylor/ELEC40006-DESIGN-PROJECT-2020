#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    QFile file("Output.txt");

    if (!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this,"Title","File Not Open");

    }
    QTextStream out(&file);
    QString text = ui->plainTextEdit_2->toPlainText() ;
    out << text ;
    file.flush(); //Flushes stream inside out
    file.close();
}

void MainWindow::on_pushButton_2_clicked()
{
    QFile file("Input.txt");

    if (!file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this,"Title","File Not Open");

    }
    QTextStream in(&file);
    QString text = in.readAll();
    ui->plainTextEdit_2->setPlainText(text);
    file.close();
}
