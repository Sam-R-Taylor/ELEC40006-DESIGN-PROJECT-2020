#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
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

void MainWindow::makeplot(){

    // generate some data:
    QVector<double> x(101), y(101); // initialize with entries 0..100
    for (int i=0; i<101; ++i)
    {
      x[i] = i/50.0 - 1; // x goes from -1 to 1
      y[i] = x[i]*x[i]; // let's plot a quadratic function
    }
    // create graph and assign data to it:
    ui->customPlot->addGraph();
     ui->customPlot->graph(0)->setData(x, y);
    // give the axes some labels:
     ui->customPlot->xAxis->setLabel("Time");
     ui->customPlot->yAxis->setLabel("Voltage");
    // set axes ranges, so we see all data:
     ui->customPlot->xAxis->setRange(-1, 1);
     ui->customPlot->yAxis->setRange(0, 1);
     ui->customPlot->replot();

}


void MainWindow::on_pushButton_2_clicked()
{
    QFile file(QFileDialog::getOpenFileName(this,"Select Netlist File","C://"));

    if (!file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this,"Title","File Not Open");

    }
    QTextStream in(&file);
    QString text = in.readAll();
    ui->plainTextEdit_2->setPlainText(text);
    file.close();
}

void MainWindow::on_pushButton_3_clicked()
{
    MainWindow::makeplot();
}
