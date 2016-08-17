#ifndef ADDSPELLINGWORDTESTS_H
#define ADDSPELLINGWORDTESTS_H

#include <QObject>
#include <QtTest/QtTest>
#include "../../xpiks-qt/Common/hold.h"

class AddSpellingWordTests:
    public QObject
{
Q_OBJECT

private slots:
    void addWordSignalTest();

private:
    Common::Hold m_FakeHold;
};

#endif // ADDSPELLINGWORDTESTS_H
