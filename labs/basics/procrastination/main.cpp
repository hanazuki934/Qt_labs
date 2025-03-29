//
// Created by andrei on 3/12/25.
//
#include <QApplication>
#include "procrastination.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    TicketApp window;
    window.show();

    return app.exec();
}
