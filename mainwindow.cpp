#include "mainwindow.h"
#include "ui_mainwindow.h"


#include "wavefunctioncollapse.h"


void MainWindow::generate()
{
    int32_t width = ui->_width->text().toInt();
    int32_t height = ui->_height->text().toInt();

    std::vector<Tile> tiles;

    srand(57784);
    Grid model(width, height, tiles);
    model.run();
    model.dump();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(
        ui->_generate,
        &QPushButton::pressed,
        this,
        &MainWindow::unitTest
    );
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::unitTest()
{
    Tile t0(0);
    Tile t1(1);
    Tile t5(5);
    Tile t6(6);
    Tile t74(74);

    // 0: {74}, {1}, {5}, {74}
    // 1: {74}, {74}, {6}, {0}
    // 5: {0}, {6}, {74}, {74}
    // 6: {1}, {74}, {74}, {5}
    // 74: {5, 6}, {0, 5}, {0, 1}, {1, 6}

    t0.setCompatibleTiles({std::set<int32_t>{74}, std::set<int32_t>{1}, std::set<int32_t>{5}, std::set<int32_t>{74}});
    t1.setCompatibleTiles({std::set<int32_t>{74}, std::set<int32_t>{74}, std::set<int32_t>{6}, std::set<int32_t>{0}});
    t5.setCompatibleTiles({std::set<int32_t>{0}, std::set<int32_t>{6}, std::set<int32_t>{74}, std::set<int32_t>{74}});
    t6.setCompatibleTiles({std::set<int32_t>{1}, std::set<int32_t>{74}, std::set<int32_t>{74}, std::set<int32_t>{5}});
    t74.setCompatibleTiles({std::set<int32_t>{5, 6}, std::set<int32_t>{0, 5}, std::set<int32_t>{0, 1}, std::set<int32_t>{1, 6}});

    std::vector<Tile> tiles{t0, t1, t5, t6, t74};

    srand(57784);
    Grid model(2, 2, tiles);
    model.run();
    model.dump();
}

