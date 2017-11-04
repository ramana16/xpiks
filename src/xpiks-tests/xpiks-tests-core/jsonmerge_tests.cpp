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

#define PLAN_NAME_KEY QLatin1String("name")
#define PROPERTY_TYPE_KEY QLatin1String("propertyType")
#define PROPERTY_NAME_KEY QLatin1String("propertyName")
#define COLUMN_NAME_KEY QLatin1String("columnName")

struct PlanNameJsonComparer: public Helpers::CompareValuesJson
{
    virtual int operator()(const QJsonObject &val1, const QJsonObject &val2) {
        if (val1.contains(PLAN_NAME_KEY) && val2.contains(PLAN_NAME_KEY)) {
            const QString planName1 = val1.value(PLAN_NAME_KEY).toString();
            const QString planName2 = val2.value(PLAN_NAME_KEY).toString();

            int result = QString::compare(planName1, planName2);
            return result;
        } else if (val1.contains(PROPERTY_TYPE_KEY) && val2.contains(PROPERTY_TYPE_KEY)) {
            const QString columnName1 = val1.value(COLUMN_NAME_KEY).toString();
            const QString columnName2 = val2.value(COLUMN_NAME_KEY).toString();

            const int type1 = val1.value(PROPERTY_TYPE_KEY).toInt(-1);
            const int type2 = val2.value(PROPERTY_TYPE_KEY).toInt(-1);

            if ((type1 != -1) && (type1 == type2) && (columnName1 == columnName2)) {
                return 0;
            }
        }

        return 1;
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

    PlanNameJsonComparer comparer;
    Helpers::mergeJson(remoteDoc, localDoc, false, comparer);

    qDebug() << "local" << localDoc.toJson(QJsonDocument::Compact);
    qDebug() << "merged" << mergedDoc.toJson(QJsonDocument::Compact);

    QCOMPARE(localDoc, mergedDoc);
}

void JsonMergeTests::mergeSelfTest() {
    const char *oneJson = R"JSON(
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
                                          },
                                          {
                                              "columnName": "Categories",
                                              "propertyName": "Empty",
                                              "propertyType": 0
                                          },
                                          {
                                              "columnName": "Editorial",
                                              "propertyName": "Empty",
                                              "propertyType": 0
                                          }
                                      ]
                                  },
                                  {
                                      "issystem": true,
                                      "name": "Pond5 Video *",
                                      "properties": [
                                          {
                                              "columnName": "ClipId",
                                              "propertyName": "Empty",
                                              "propertyType": 0
                                          },
                                          {
                                              "columnName": "OriginalFilename",
                                              "propertyName": "Filename",
                                              "propertyType": 1
                                          },
                                          {
                                              "columnName": "Keywords",
                                              "propertyName": "Keywords",
                                              "propertyType": 4
                                          }
                                      ]
                                  }
                              ]
                          }
)JSON";

    QByteArray localJsonData(oneJson);
    QByteArray remoteJsonData(oneJson);
    QByteArray mergedJsonData(oneJson);

    QJsonParseError error;
    QJsonDocument localDoc = QJsonDocument::fromJson(localJsonData, &error); QCOMPARE(error.error, QJsonParseError::NoError);
    QJsonDocument remoteDoc = QJsonDocument::fromJson(remoteJsonData, &error); QCOMPARE(error.error, QJsonParseError::NoError);
    QJsonDocument mergedDoc = QJsonDocument::fromJson(mergedJsonData, &error); QCOMPARE(error.error, QJsonParseError::NoError);

    QCOMPARE(localDoc, mergedDoc);

    PlanNameJsonComparer comparer;
    Helpers::mergeJson(remoteDoc, localDoc, false, comparer);

    auto localJson = localDoc.toJson(QJsonDocument::Compact);
    auto remoteJson = mergedDoc.toJson(QJsonDocument::Compact);

    QCOMPARE(localJson, remoteJson);

    //qDebug() << "local" << localJson;
    //qDebug() << "merged" << remoteJson;

    // does not work
    // QCOMPARE(localDoc, mergedDoc);
}
