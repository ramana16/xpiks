#ifndef DUPLICATESEARCH_TESTS_H
#define DUPLICATESEARCH_TESTS_H

#include <QObject>
#include <QtTest/QtTest>

class DuplicateSearchTests: public QObject
{
    Q_OBJECT
private slots:
    void simpleTest();
    void distanceTest();
    void severalwordsinkeywordTest();
};

#endif // DUPLICATESEARCH_TESTS_H
