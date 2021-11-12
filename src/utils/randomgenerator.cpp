#include "randomgenerator.h"
#include <QDebug>
RandomGenerator::RandomGenerator()
{

}

QString RandomGenerator::generateFromString(const int &length, const QString &srcString)
{
    if(length <= 0 || srcString.length() <= 0){
        return QString();
    }
    QString retString;
    retString.resize(length);
#if QT_VERSION >= QT_VERSION_CHECK(5,10,0)
    for(int i = 0; i < length; i++){
        int len = QRandomGenerator::securelySeeded().bounded(0,srcString.length());
        retString[i] = srcString.at(len);
    }
#else
    QTime t;
    t = QTime::currentTime();
    qsrand(t.msec() + t.second()*1000);
    for(int i = 0; i < length; i++){
        int len = qrand() % srcString.length();
        retString[i] = srcString.at(len);
    }
#endif
    return retString;
}
