#ifndef JSONMERGE_TESTS_H
#define JSONMERGE_TESTS_H

#include <QtTest/QTest>

class JsonMergeTests: public QObject
{
    Q_OBJECT
private slots:
    void mergeArraysOfObjectsTest();
    void mergeArraysOfStringsTest();
    void mergeOverwriteTest();
};

#endif // JSONMERGE_TESTS_H
