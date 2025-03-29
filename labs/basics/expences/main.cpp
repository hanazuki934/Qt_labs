//
// Created by andrei on 3/12/25.
//
#include <QApplication>
#include "expences.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ExpenceApp window;
    window.show();

    return app.exec();
}
