#include "jsonmerge_tests.h"
#include <QJsonDocument>
#include "../../xpiks-qt/Helpers/jsonhelper.h"
#include "../../xpiks-qt/Helpers/comparevaluesjson.h"

struct NameJsonComparer: public Helpers::CompareValuesJson
{
    virtual int operator()(const QJsonObject &val1, const QJsonObject &val2) {
        return (val1["name"] == val2["name"]) ? 0 : -1;
    }
};

void JsonMergeTests::mergeArraysOfObjectsTest() {
    const char *localJson = R"JSON(
                             {
                                 "stocks_ftp": [
                                     {
                                         "name": "Bigstockphoto",
                                         "ftp": "ftp://bigstockphoto"
                                     },
                                     {
                                         "name": "Canstockphoto",
                                         "ftp": "ftp://ftp.canstockphoto"
                                     },
                                     {
                                         "name": "Depositphotos",
                                         "ftp": "ftp://ftp.depositphotos"
                                     },
                                     {
                                         "name": "Missing one",
                                         "ftp": "ftp://bigstockphoto.com"
                                     }
                                 ],
                                 "version": 0
                             }
)JSON";

    const char *remoteJson = R"JSON(
                             {
                                 "stocks_ftp": [
                                     {
                                         "ftp": "ftp://bigstockphoto.com",
                                         "name": "Bigstockphoto"
                                     },
                                     {
                                         "ftp": "ftp://ftp.canstockphoto.com",
                                         "name": "Canstockphoto"
                                     },
                                     {
                                         "ftp": "ftp://ftp.depositphotos.com",
                                         "name": "Depositphotos"
                                     },
                                     {
                                         "ftp": "ftp://ftp.shutterstock.com",
                                         "name": "Brand new one"
                                     }
                                 ],
                                 "version": 1
                             }
)JSON";

    const char *mergedJson = R"JSON(
                           {
                               "stocks_ftp": [
                                   {
                                       "ftp": "ftp://bigstockphoto.com",
                                       "name": "Bigstockphoto"
                                   },
                                   {
                                       "ftp": "ftp://ftp.canstockphoto.com",
                                       "name": "Canstockphoto"
                                   },
                                   {
                                       "ftp": "ftp://ftp.depositphotos.com",
                                       "name": "Depositphotos"
                                   },
                                   {
                                       "ftp": "ftp://bigstockphoto.com",
                                       "name": "Missing one"
                                   },
                                   {
                                       "ftp": "ftp://ftp.shutterstock.com",
                                       "name": "Brand new one"
                                   }
                               ],
                               "version": 1
                           }
)JSON";

    QByteArray localJsonData(localJson);
    QByteArray remoteJsonData(remoteJson);
    QByteArray mergedJsonData(mergedJson);

    QJsonParseError error;
    QJsonDocument localDoc = QJsonDocument::fromJson(localJsonData, &error); QCOMPARE(error.error, QJsonParseError::NoError);
    QJsonDocument remoteDoc = QJsonDocument::fromJson(remoteJsonData, &error); QCOMPARE(error.error, QJsonParseError::NoError);
    QJsonDocument mergedDoc = QJsonDocument::fromJson(mergedJsonData, &error); QCOMPARE(error.error, QJsonParseError::NoError);

    NameJsonComparer comparer;
    Helpers::mergeJson(remoteDoc, localDoc, false, comparer);

    QCOMPARE(localDoc, mergedDoc);
}

void JsonMergeTests::mergeArraysOfStringsTest() {
    const char *localJson = R"JSON(
                            {
                                "ftp_list": [
                                        "ftp://bigstockphoto.com",
                                        "ftp://ftp.canstockphoto.com",
                                        "ftp://ftp.depositphotos.com"
                                ],
                                "version": 0
                            }
)JSON";

    const char *remoteJson = R"JSON(
                             {
                                 "ftp_list": [
                                        "ftp://bigstockphoto.com",
                                        "ftp://ftp.canstockphoto.com__",
                                        "ftp://ftp.depositphotos.com__"
                                 ],
                                 "version": 1
                             }
)JSON";

    const char *mergedJson = R"JSON(
                             {
                                 "ftp_list": [
                                         "ftp://bigstockphoto.com",
                                         "ftp://ftp.canstockphoto.com",
                                         "ftp://ftp.canstockphoto.com__",
                                         "ftp://ftp.depositphotos.com",
                                         "ftp://ftp.depositphotos.com__"
                                 ],
                                 "version": 1
                             }
)JSON";

    QByteArray localJsonData(localJson);
    QByteArray remoteJsonData(remoteJson);
    QByteArray mergedJsonData(mergedJson);

    QJsonParseError error;
    QJsonDocument localDoc = QJsonDocument::fromJson(localJsonData, &error); QCOMPARE(error.error, QJsonParseError::NoError);
    QJsonDocument remoteDoc = QJsonDocument::fromJson(remoteJsonData, &error); QCOMPARE(error.error, QJsonParseError::NoError);
    QJsonDocument mergedDoc = QJsonDocument::fromJson(mergedJsonData, &error); QCOMPARE(error.error, QJsonParseError::NoError);

    NameJsonComparer comparer;
    Helpers::mergeJson(remoteDoc, localDoc, false, comparer);

    // QCOMPARE(localDoc, mergedDoc); -- do the actual check below

    QJsonObject goldObject = mergedDoc.object();
    QJsonObject testObject = localDoc.object();

    QJsonArray goldArray = goldObject["ftp_list"].toArray();
    QJsonArray testArray = testObject["ftp_list"].toArray();

    QSet<QString> goldSet;
    QSet<QString> testSet;
    int goldSize = goldArray.size();
    int testSize = testArray.size();

    for (int i = 0; i < goldSize; i++) {
        goldSet.insert(goldArray[i].toString());
    }

    for (int i = 0; i < testSize; i++) {
        testSet.insert(testArray[i].toString());
    }

    QCOMPARE(testSet, goldSet);
}

void JsonMergeTests::mergeOverwriteTest() {
    const char *localJson = R"JSON(
                             {
                                 "stocks_ftp": [
                                     {
                                         "name": "Bigstockphoto",
                                         "ftp": "ftp://bigstockphoto"
                                     },
                                     {
                                         "name": "Canstockphoto",
                                         "ftp": "ftp://ftp.canstockphoto"
                                     },
                                     {
                                         "name": "Depositphotos",
                                         "ftp": "ftp://ftp.depositphotos"
                                     },
                                     {
                                         "name": "Missing one",
                                         "ftp": "ftp://bigstockphoto.com"
                                     }
                                 ],
                                 "version": 0
                             }
)JSON";

    const char *remoteJson = R"JSON(
                             {
                                 "ftp_list": [
                                         "ftp://bigstockphoto.com",
                                         "ftp://ftp.canstockphoto.com",
                                         "ftp://ftp.canstockphoto.com__",
                                         "ftp://ftp.depositphotos.com",
                                         "ftp://ftp.depositphotos.com__"
                                 ],
                                 "version": 1
                             }
)JSON";

    const char *mergedJson = remoteJson;

    QByteArray localJsonData(localJson);
    QByteArray remoteJsonData(remoteJson);
    QByteArray mergedJsonData(mergedJson);

    QJsonParseError error;
    QJsonDocument localDoc = QJsonDocument::fromJson(localJsonData, &error); QCOMPARE(error.error, QJsonParseError::NoError);
    QJsonDocument remoteDoc = QJsonDocument::fromJson(remoteJsonData, &error); QCOMPARE(error.error, QJsonParseError::NoError);
    QJsonDocument mergedDoc = QJsonDocument::fromJson(mergedJsonData, &error); QCOMPARE(error.error, QJsonParseError::NoError);

    NameJsonComparer comparer;
    Helpers::mergeJson(remoteDoc, localDoc, true, comparer);

    QCOMPARE(localDoc, mergedDoc);
}

void JsonMergeTests::mergeExistingElementsTest() {
    const char *localJson = R"JSON(
                            {
                                "plans": [
                                    {
                                        "name": "test plan 1",
                                        "properties": [
                                            {
                                                "propertyName": "Empty",
                                                "propertyType": 0
                                            }
                                        ]
                                    },
                                    {
                                        "issystem": true,
                                        "name": "Shutterstock Video *",
                                        "properties": [
                                            {
                                                "columnName": "Filename",
                                                "propertyName": "Filename",
                                                "propertyType": 1
                                            },
                                            {
                                                "columnName": "Description",
                                                "propertyName": "Description",
                                                "propertyType": 3
                                            }
                                        ]
                                    }
                                ]
                            }
)JSON";

    const char *remoteJson = R"JSON(
                             {
                                 "plans": [
                                     {
                                         "issystem": true,
                                         "name": "Shutterstock Video *",
                                         "properties": [
                                             {
                                                 "columnName": "Filename",
                                                 "propertyName": "Filename",
                                                 "propertyType": 1
                                             },
                                             {
                                                 "columnName": "Description",
                                                 "propertyName": "Description",
                                                 "propertyType": 3
                                             }
                                         ]
                                     }
                                 ]
                             }
)JSON";

    const char *mergedJson = R"JSON(
                             {
                                 "plans": [
                                     {
                                         "name": "test plan 1",
                                         "properties": [
                                             {
                                                 "propertyName": "Empty",
                                                 "propertyType": 0
                                             }
                                         ]
                                     },
                                     {
                                         "issystem": true,
                                         "name": "Shutterstock Video *",
                                         "properties": [
                                             {
                                                 "columnName": "Filename",
                                                 "propertyName": "Filename",
                                                 "propertyType": 1
                                             },
                                             {
                                                 "columnName": "Description",
                                                 "propertyName": "Description",
                                                 "propertyType": 3
                                             }
                                         ]
                                     }
                                 ]
                             }
)JSON";

    QByteArray localJsonData(localJson);
    QByteArray remoteJsonData(remoteJson);
    QByteArray mergedJsonData(mergedJson);

    QJsonParseError error;
    QJsonDocument localDoc = QJsonDocument::fromJson(localJsonData, &error); QCOMPARE(error.error, QJsonParseError::NoError);
    QJsonDocument remoteDoc = QJsonDocument::fromJson(remoteJsonData, &error); QCOMPARE(error.error, QJsonParseError::NoError);
    QJsonDocument mergedDoc = QJsonDocument::fromJson(mergedJsonData, &error); QCOMPARE(error.error, QJsonParseError::NoError);

    NameJsonComparer comparer;
    Helpers::mergeJson(remoteDoc, localDoc, false, comparer);

    QCOMPARE(localDoc, mergedDoc);
}
