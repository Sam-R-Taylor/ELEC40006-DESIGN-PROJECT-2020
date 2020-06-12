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
#include <vector>
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
    ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle::ssDot);
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
    ui->customPlot->axisRect(0)->axis(QCPAxis::atLeft)->rescale();
    ui->customPlot->replot();

}

void MainWindow::makeplotCurrent(){
    std::string CurrentStr;
    std::fstream myfile;
    std::string str;
    std::stringstream ss;
    myfile.open("output_current.txt");
    QVector<double> yData, xData;

    ui->customPlot->addGraph();
    ui->customPlot->graph(0)->setScatterStyle(QCPScatterStyle::ssDot);
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
    ui->customPlot->axisRect(0)->axis(QCPAxis::atLeft)->rescale();
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

void MainWindow::on_spinBox_editingFinished()
{
    std::fstream myfile;
    vector<string> vect;
    myfile.open("output_current.txt");
    std::stringstream ss;
    string str;
    string CurrentStr;
    std::getline(myfile, str);
    ss << str;
    while (std::getline(ss,CurrentStr,',')){
       vect.push_back(CurrentStr);
    }

//FINISH ME

    int PlotSelect = (ui->spinBox->value());
    QString text2 = QString::fromStdString(vect.at(PlotSelect));
    ui->plainTextEdit->setPlainText(text2);

}


void MainWindow::on_spinBox_valueChanged(int arg1)
{

}

