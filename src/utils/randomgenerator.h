#ifndef RANDOMGENERATOR_H
#define RANDOMGENERATOR_H
#include <QtCore/QRandomGenerator>
#include "defines.h"

class RandomGenerator
{
public:
    RandomGenerator();
    static QString generateFromString(const int &length, const QString &str = RANDOM_STRING);
};

#endif // RANDOMGENERATOR_H
