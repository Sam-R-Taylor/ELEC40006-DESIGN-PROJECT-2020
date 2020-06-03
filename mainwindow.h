#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <fstream>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void addPoint(double x , double y);
    void clearData();
    void plot();

private slots:

    void on_pushButton_2_clicked();

    void makeplotVoltage();
    void on_pushButton_3_clicked();
    void makeplotCurrent();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
