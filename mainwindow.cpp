#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QtCore>
#include <fstream>
#include <cstdio>
#include <string>
#include <sstream>
#include <iostream>
#include <Parser.hpp>
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

void MainWindow::makeplotVoltage(){
    std::string CurrentStr;
    std::fstream myfile;
    std::string str;
    std::stringstream ss;
    myfile.open("output_voltage.txt");
    QVector<double> yData, xData;

    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
    ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);

    while (std::getline(myfile, str))
    {
        ss << str;
        std::getline(ss,CurrentStr,',');
        xData.push_back(atof(CurrentStr.c_str()));

        int PlotSelect = (ui->spinBox->value());
        for(int i =0; i<PlotSelect ;i++){
            std::getline(ss,CurrentStr,',');
        }

        std::getline(ss,CurrentStr,',');
        yData.push_back(atof(CurrentStr.c_str()));
        ui->customPlot->graph(0)->setData(xData, yData);
        ss.str("");
        ui->customPlot->replot();

    }
    double xmin = *std::min_element(xData.constBegin(), xData.constEnd());
    double xmax = *std::max_element(xData.constBegin(), xData.constEnd());
    double ymin = *std::min_element(yData.constBegin(), yData.constEnd());
    double ymax = *std::max_element(yData.constBegin(), yData.constEnd());
    ui->customPlot->xAxis->setRange(xmin - (xmin/2), xmax + (xmax/2));
    ui->customPlot->yAxis->setRange(ymin - (ymin/2), ymax + (ymax/2));
    ui->customPlot->replot();


    /*
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
     */

}

void MainWindow::makeplotCurrent(){
    std::string CurrentStr;
    std::fstream myfile;
    std::string str;
    std::stringstream ss;
    myfile.open("output_current.txt");
    QVector<double> yData, xData;

    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
    ui->customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);

    while (std::getline(myfile, str))
    {
        ss << str;
        std::getline(ss,CurrentStr,',');
        xData.push_back(atof(CurrentStr.c_str()));

        int PlotSelect = (ui->spinBox->value());
        for(int i =0; i<PlotSelect ;i++){
            std::getline(ss,CurrentStr,',');
        }

        std::getline(ss,CurrentStr,',');
        yData.push_back(atof(CurrentStr.c_str()));
        ui->customPlot->graph(0)->setData(xData, yData);
        ss.str("");
        ui->customPlot->replot();

    }
    double xmin = *std::min_element(xData.constBegin(), xData.constEnd());
    double xmax = *std::max_element(xData.constBegin(), xData.constEnd());
    double ymin = *std::min_element(yData.constBegin(), yData.constEnd());
    double ymax = *std::max_element(yData.constBegin(), yData.constEnd());
    ui->customPlot->xAxis->setRange(xmin - (xmin/2), xmax + (xmax/2));
    ui->customPlot->yAxis->setRange(ymin - (ymin/2), ymax + (ymax/2));
    ui->customPlot->replot();


    /*
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
     */

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
    QFileInfo fileInfo(file);
    QString filedir = fileInfo.absoluteFilePath();
    string dir = filedir.toLocal8Bit().constData();
    std::fstream ifs(dir);
    std::fstream &input = ifs;
    parse_input(input);
    file.close();
}

void MainWindow::on_pushButton_3_clicked()
{
    if ((ui->comboBox->currentIndex()) == 0  ){
        MainWindow::makeplotVoltage();
    }

    else{
        MainWindow::makeplotCurrent();

    }

}
