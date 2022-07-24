#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "config.h"
#include "wavefunctioncollapse.h"

#include <iostream>
#include <QButtonGroup>
#include <QFileDialog>

namespace
{
Tile::Direction current_direction = Tile::Direction::North;
std::optional<int32_t> current_tile_index;


Tile::Direction oppositeDirection(Tile::Direction dir)
{
    switch (dir)
    {
        case Tile::Direction::North:
            return Tile::Direction::South;
        case Tile::Direction::East:
            return Tile::Direction::West;
        case Tile::Direction::South:
            return Tile::Direction::North;
        case Tile::Direction::West:
            return Tile::Direction::East;
    }

    // things went
    return Tile::Direction::South;
}

}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle(tr("Wave Function Collapse Editor 1.0"));

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
            ui->_tile_bias->setValue(Config::instance().getTile(index)._bias * 100.0f);
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

            // compability is bidirectional, so also add the current tile to the others
            for (auto other_tile_index : indices)
            {
                Config::instance().addTile(other_tile_index);
                auto& other_tile = Config::instance().getTile(other_tile_index);
                other_tile.addCompatibleTile(oppositeDirection(current_direction), current_tile_index.value());
            }
        }
    );

    connect(
        ui->_tile_bias,
        &QSlider::sliderMoved,
        this,
        [](int32_t value){
            const auto bias = value * 0.01f;
            auto& tile = Config::instance().getTile(current_tile_index.value());
            tile._bias = bias;
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
        this,
        [this](){
        const auto path = QFileDialog::getOpenFileName(this, tr("project path"), QString(), "*.json");
        if (path.isEmpty())
        {
            return;
        }
        auto& config = Config::instance();
        Config::instance().load(path.toStdString());
        ui->_use_biases->setChecked(config._use_bias);
        ui->_grid_width->setText(QString::number(config._grid_size._x));
        ui->_grid_width->setText(QString::number(config._grid_size._y));
        ui->_tile_size->setText(QString::number(config._tile_size));
        loadTexture();
    });

    connect(
        ui->_action_save,
        &QAction::triggered,
        [](){
        Config::instance().save();
    });

    connect(
        ui->_action_save_as,
        &QAction::triggered,
        this,
        [this](){
        const auto path = QFileDialog::getOpenFileName(this, tr("project path"), QString(), "*.json");
        if (path.isEmpty())
        {
            return;
        }
        Config::instance()._project_file_path = path.toStdString();
        Config::instance().save();
    });

    connect(
        ui->_action_load_texture,
        &QAction::triggered,
        this,
        [this](){
            const auto path = QFileDialog::getOpenFileName(this, tr("texture path"));
            if (path.isEmpty())
            {
                return;
            }

            Config::instance()._texture_path = path.toStdString();
            loadTexture();
        }
    );
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::loadTexture()
{
    auto& config = Config::instance();
    QImage texture;
    texture.load(QString::fromStdString(config._texture_path));
    config._texture_size._x = texture.width();
    config._texture_size._y = texture.height();
    ui->_tile_grid->setTexture(texture);
    ui->_tiled_image->setTexture(texture);
    ui->_compatible_tiles->setTexture(texture);
}


void MainWindow::showSelectedTile(int32_t index)
{
    current_tile_index = index;

    const auto& image = ui->_tiled_image->texture();
    const auto cols = Config::instance().textureColumnCount();
    const auto tile_width = Config::instance()._tile_size;

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
    auto& config = Config::instance();
    config._tile_size = ui->_tile_size->text().toInt();
    config._grid_size._x = ui->_grid_width->text().toInt();
    config._grid_size._y = ui->_grid_height->text().toInt();
    config._use_bias = ui->_use_biases->isChecked();

    std::srand(std::time(0));
    Tile::instance_counter = 0;

    auto succesful = false;
    int32_t collapsed_slot_count = 0;

    while (!succesful)
    {
        ui->_tile_grid->clearPositionedTiles();

        Grid grid(config._grid_size._x, config._grid_size._y, config._tiles);
        grid._use_bias = config._use_bias;
        grid._tile_collapsed_callback = [this](const Vector2D& pos, int32_t index){
            ui->_tile_grid->addPositionedTile(pos, index);
            qApp->processEvents();
        };

        grid.run();

        succesful = !grid._given_up;

        if (grid._collapsed_slot_count > collapsed_slot_count)
        {
            collapsed_slot_count = grid._collapsed_slot_count;
            std::cout << collapsed_slot_count << std::endl;
        }

        if (succesful)
        {
            ui->_tile_grid->setGrid(grid._size, grid.readGrid());
        }
    }
}

