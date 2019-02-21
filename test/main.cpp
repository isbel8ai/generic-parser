/**
 * @file
 * @author Isbel Ochoa Izquierdo
 */

#include <QFile>
#include <QDomDocument>
#include <QDebug>
#include <iostream>
#include <QTime>

#include <parsermanager.h>

int main() {

    QFile dataFile("../data/testdata.txt");
    if (!dataFile.open(QIODevice::ReadOnly | QIODevice::Text))
         return 1;

    QString data(dataFile.readAll());
    dataFile.close();
    ParserManager parsingManager(QDir("../config/generic-parser"));

    QTime start = QTime::currentTime();
    std::cout << parsingManager.toDom(&data).toString().toStdString()
            << std::endl;
    std::cout << "Time: " << start.msecsTo(QTime::currentTime()) << " ms"
            << std::endl;
    return 0;
}

