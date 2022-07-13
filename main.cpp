#include "mainwindow.h"

#include "wavefunctioncollapse.h"

// #include <QApplication>


int main(int /*argc*/, char* /*argv*/ [])
{
    srand(57784);


    Grid model(100, 25);
    model.run();
    model.dump();


//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();
//    return a.exec();
}
