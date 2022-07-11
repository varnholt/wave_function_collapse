#include "mainwindow.h"

#include "wavefunctioncollapse.h"

// #include <QApplication>


int main(int argc, char *argv[])
{
    srand(57784);

//    int runCount = 5000;
//    auto t1 = std::chrono::high_resolution_clock::now();

//    for (int i = 0; i < runCount; i++)
//    {
//        Model model(10, 10);
//        model.Run();
//    }
//    auto t2 = std::chrono::high_resolution_clock::now();
//    auto time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);

//    printf("Time per solve : %2.4f seconds\n", float(time_span.count()) / float(runCount));


    Model model(100, 25);
    model.run();
    model.dump();


//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();
//    return a.exec();
}
