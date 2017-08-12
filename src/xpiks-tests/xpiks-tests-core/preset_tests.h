#ifndef PRESETTESTS_H
#define PRESETTESTS_H

#include <QObject>
#include <QtTest/QtTest>

class PresetTests: public QObject
{
    Q_OBJECT
private slots:
    void expandFromPresetTrivial();
    void expandFromPresetWithDuplicates();
    void appendFromPresetTrivial();
    void appendFromPresetWithDuplicates();
    void findPresetByNameTest();
    void strictFindPresetByNameTest();
    void findPresetWithLongNamesByNameTest();
    void addPresetKeywordsWithDuplicatesTest();
};

#endif // PRESETTESTS_H
