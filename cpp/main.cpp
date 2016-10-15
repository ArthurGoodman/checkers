#include <QCoreApplication>
#include <QString>

#include "engine.h"

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);

    Engine engine(argc > 1 ? QString(argv[1]).toInt() : 2);

    return app.exec();
}
