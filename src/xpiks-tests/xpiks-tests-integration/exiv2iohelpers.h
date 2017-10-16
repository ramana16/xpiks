#ifndef EXIV2IOHELPERS_H
#define EXIV2IOHELPERS_H

#include <QString>
#include <QStringList>

class Exiv2InitHelper
{
public:
    Exiv2InitHelper();
    ~Exiv2InitHelper();
};

struct BasicMetadata {
    QString m_Title;
    QString m_Description;
    QStringList m_Keywords;
};

namespace Models {
    class ArtworkMetadata;
}

void readMetadata(const QString &filepath, BasicMetadata &importResult);
void readMetadataEx(const QString &filepath, BasicMetadata &xmpMetadata, BasicMetadata &exifMetadata, BasicMetadata &iptcMetadata);

#endif // EXIV2IOHELPERS_H
