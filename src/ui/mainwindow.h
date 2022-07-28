#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
   MainWindow(QWidget* parent = nullptr);
   ~MainWindow();

private:
   Ui::MainWindow* ui;

   void loadTexture();

private slots:
   void unitTest();
   void generate();
   void updatedSelectedTiles();
   void showSelectedTile(int32_t);
};
#endif  // MAINWINDOW_H
