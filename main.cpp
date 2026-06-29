#include "mainwindow.h"
#include "databasemanager.h"
#include "soundmanager.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/images/icon.ico"));
     SoundManager::instance();

    if (DatabaseManager::connectDatabase()) {
        DatabaseManager::createTables();
        DatabaseManager::insertTestQuestions();
    }

    MainWindow w;
    w.show();

    return a.exec();
}