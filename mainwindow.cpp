#include "mainwindow.h"
#include "ui_mainwindow.h"


#include "wavefunctioncollapse.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(
        ui->_generate,
        &QPushButton::pressed,
        this,
        [this](){
            int32_t width = ui->_width->text().toInt();
            int32_t height = ui->_height->text().toInt();

            srand(57784);
            Grid model(width, height);
            model.run();
            model.dump();
        }
    );
}

MainWindow::~MainWindow()
{
    delete ui;
}

