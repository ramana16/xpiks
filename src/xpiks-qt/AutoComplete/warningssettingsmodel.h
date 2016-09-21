#ifndef WARNINGSSETTINGSMODEL_H
#define WARNINGSSETTINGSMODEL_H

#include <QJsonDocument>
#include <QJsonArray>
#include "../Models/abstractconfigupdatermodel.h"

namespace AutoComplete {
    class WarningsSettingsModel:
        public Models::AbstractConfigUpdaterModel
    {
    Q_OBJECT

    public:
        WarningsSettingsModel();
        void initializeConfigs();

        const QString &getAllowedFilenameCharacters() const { return m_AllowedFilenameCharacters; }
        double getMinMegapixels() const { return m_MinMegapixels; }
        int getMaxKeywordsCount() const { return m_MaxKeywordsCount; }
        int getMaxDescriptionLength() const { return m_MaxDescriptionLength; }

    signals:
        void warningsSettingsUpdated();

        // AbstractConfigUpdaterModel interface

    protected:
        virtual void processRemoteConfig(const QJsonDocument &remoteDocument, bool overwriteLocal);
        virtual bool parseConfig(const QJsonDocument &document);

        // CompareValuesJson interface

    public:
        virtual int operator ()(const QJsonObject &val1, const QJsonObject &val2);

    private:
        QString m_AllowedFilenameCharacters;
        double m_MinMegapixels;
        int m_MaxKeywordsCount;
        int m_MaxDescriptionLength;
    };
}
#endif // WARNINGSSETTINGSMODEL_H
