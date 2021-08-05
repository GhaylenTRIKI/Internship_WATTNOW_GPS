#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QLabel>
#include "sunrise.h"
#include <iostream>
#include <QPixmap>

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
     // Sunrise sunrise(16.141, -22.904); // Boa Vista, Cabo Verde


   /*
    * Hannover
      Position: 52° 23' 12" N,  9° 41' 52" E
    */



     Sunrise sunrise(ui->lineEdit_latitude->text().toInt(), ui->lineEdit_longitude->text().toInt());



    ui->label_sunrise->setText("SUNRISE: "+sunrise.sunrise(d).toString());
    ui->label_noon->setText("NOON: "+sunrise.noon(d).toString());
    ui->label_sunset->setText("SUNSET: "+sunrise.sunset(d).toString());
}

