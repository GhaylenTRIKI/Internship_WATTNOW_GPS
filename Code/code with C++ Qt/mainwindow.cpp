#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include "sunrise.h"
#include <iostream>
#include <QPixmap>
#include <sstream>
#include <string>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QPixmap pix("D:/ACADEMIQUE/stage/stage WATTNOW/code/picture.jpg");
    ui->label_Pic->setPixmap(pix.scaled(200,200,Qt::KeepAspectRatio));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    QDate d;
     using namespace std;
       d = ui->dateEdit->date();

     cout << d.toString().toStdString() << endl;


     Sunrise sunrise(ui->lineEdit_latitude->text().toDouble(), ui->lineEdit_longitude->text().toDouble(), ui->lineEdit_elevation->text().toDouble());

     double longit = ui->lineEdit_longitude->text().toDouble();
     cout << "longit= "<<longit<<endl;

     ostringstream  ss1;
     double timez= sunrise.timezone(longit);
     cout << "timezone= "<<timez<<endl;



     ss1<<timez;
     string d_str1 = ss1.str();


    ui->label_sunrise->setText("SUNRISE: "+sunrise.sunrise(d).toString()+ " UTC "+QString::fromUtf8(d_str1.c_str()));
    ui->label_noon->setText("NOON: "+sunrise.noon(d).toString()+ " UTC "+QString::fromUtf8(d_str1.c_str()));
    ui->label_sunset->setText("SUNSET: "+sunrise.sunset(d).toString()+ " UTC "+QString::fromUtf8(d_str1.c_str()));




}

