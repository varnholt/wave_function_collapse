#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "config.h"
#include "wavefunctioncollapse.h"

#include <QButtonGroup>

namespace
{
Tile::Direction current_direction = Tile::Direction::North;
std::optional<int32_t> current_tile_index;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->_compatible_tiles->setSelectMultiple(true);

    connect(
        ui->_generate,
        &QPushButton::pressed,
        this,
        &MainWindow::generate
    );

    connect(
        ui->_tiled_image,
        &TiledTextureWidget::tileSelected,
        this,
        [this](int32_t index){

            Config::instance().addTile(index);
            showSelectedTile(index);
            updatedSelectedTiles();
        }
    );

    connect(
        ui->_compatible_tiles,
        &TiledTextureWidget::tileSelectionUpdated,
        this,
        [](const std::set<int32_t>& indices){
            if (!current_tile_index.has_value())
            {
                return;
            }
            auto& tile = Config::instance().getTile(current_tile_index.value());
            tile.setCompatibleTiles(current_direction, indices);
        }
    );

    connect(
        ui->_dir_north,
        &QToolButton::pressed,
        this,
        [this](){
        current_direction = Tile::Direction::North;
        updatedSelectedTiles();
    });

    connect(
        ui->_dir_east,
        &QToolButton::pressed,
        this,
        [this](){
        current_direction = Tile::Direction::East;
        updatedSelectedTiles();
    });

    connect(
        ui->_dir_south,
        &QToolButton::pressed,
        this,
        [this](){
        current_direction = Tile::Direction::South;
        updatedSelectedTiles();
    });

    connect(
        ui->_dir_west,
        &QToolButton::pressed,
        this,
        [this](){
        current_direction = Tile::Direction::West;
        updatedSelectedTiles();
    });

    auto dir_button_group = new QButtonGroup(this);
    dir_button_group->setExclusive(true);
    dir_button_group->addButton(ui->_dir_north);
    dir_button_group->addButton(ui->_dir_east);
    dir_button_group->addButton(ui->_dir_south);
    dir_button_group->addButton(ui->_dir_west);

    connect(
        ui->_action_load,
        &QAction::triggered,
        [](){
        Config::instance().load();
    });

    connect(
        ui->_action_save,
        &QAction::triggered,
        [](){
        Config::instance().save();
    });
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::showSelectedTile(int32_t index)
{
    current_tile_index = index;

    const auto& image = ui->_tiled_image->texture();
    const auto cols = 5;
    const auto tile_width = 16;

    const auto x = (index % cols) * tile_width;
    const auto y = (index / cols) * tile_width;

    const auto copy = image.copy(x, y, tile_width, tile_width);
    ui->_tile_label->setPixmap(QPixmap::fromImage(copy));
}


void MainWindow::updatedSelectedTiles()
{
    if (!current_tile_index.has_value())
    {
        return;
    }

    auto& tile = Config::instance().getTile(current_tile_index.value());
    ui->_compatible_tiles->setSelectedTiles(tile._compatible_tiles[static_cast<int32_t>(current_direction)]);
}


void MainWindow::unitTest()
{
    // 0: {74}, {1}, {5}, {74}
    // 1: {74}, {74}, {6}, {0}
    // 5: {0}, {6}, {74}, {74}
    // 6: {1}, {74}, {74}, {5}
    // 74: {5, 6}, {0, 5}, {0, 1}, {1, 6}

    // +---+---+---+---+
    // |###|###|###|###|
    // +---+---+---+---+
    // |###|/0-|-1\|###|
    // +---+---+---+---+
    // |###|\5_|_6/|###|
    // +---+---+---+---+
    // |###|###|###|###|
    // +---+---+---+---+

    QImage texture;
    texture.load("texture.png");

    auto& config = Config::instance();
    config._grid_size._x = 32;
    config._grid_size._y = 32;
    config._texture_size._x = texture.width();
    config._texture_size._y = texture.height();

    Tile t0(0, {std::set<int32_t>{74}, std::set<int32_t>{1}, std::set<int32_t>{5}, std::set<int32_t>{74}});
    Tile t1(1, {std::set<int32_t>{74}, std::set<int32_t>{74}, std::set<int32_t>{6}, std::set<int32_t>{0}});
    Tile t5(5, {std::set<int32_t>{0}, std::set<int32_t>{6}, std::set<int32_t>{74}, std::set<int32_t>{74}});
    Tile t6(6, {std::set<int32_t>{1}, std::set<int32_t>{74}, std::set<int32_t>{74}, std::set<int32_t>{5}});
    Tile t74(74, {std::set<int32_t>{5, 6, 74}, std::set<int32_t>{0, 5, 74}, std::set<int32_t>{0, 1, 74}, std::set<int32_t>{1, 6, 74}});

    std::vector<Tile> tiles{t0, t1, t5, t6, t74};

    std::srand(std::time(0));
    Tile::instance_counter = 0;
    Grid grid(config._grid_size._x, config._grid_size._y, tiles);
    grid.run();

    ui->_tile_grid->setTexture(texture);
    ui->_tile_grid->setGrid(grid._size, grid.readGrid());
}


void MainWindow::generate()
{
    QImage texture;
    texture.load("texture.png");

    auto& config = Config::instance();
    config._grid_size._x = 32;
    config._grid_size._y = 32;
    config._texture_size._x = texture.width();
    config._texture_size._y = texture.height();

    std::srand(std::time(0));
    Tile::instance_counter = 0;
    Grid grid(config._grid_size._x, config._grid_size._y, config._tiles);
    grid.run();

    ui->_tile_grid->setTexture(texture);
    ui->_tile_grid->setGrid(grid._size, grid.readGrid());
}

