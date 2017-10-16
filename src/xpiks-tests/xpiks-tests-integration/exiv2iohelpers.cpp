#include "exiv2iohelpers.h"

#include <QVector>
#include <QTextCodec>
#include <QDateTime>
#include <QImageReader>
#include <QStringList>
#include <sstream>
#include <string>

#include "../../xpiks-qt/Models/artworkmetadata.h"
#include "../../xpiks-qt/Models/imageartwork.h"
#include "../../xpiks-qt/Common/defines.h"
#include "../../xpiks-qt/Helpers/stringhelper.h"

#ifdef Q_OS_WIN
#include <Windows.h>
#endif

#include <exiv2/exiv2.hpp>
#include <exiv2/xmp.hpp>

#define X_DEFAULT QString::fromLatin1("x-default")

#define XMP_DESCRIPTION "Xmp.dc.description"
#define XMP_PS_HEADLINE "Xmp.photoshop.Headline"
#define XMP_TITLE "Xmp.dc.title"
#define XMP_PS_DATECREATED "Xmp.photoshop.DateCreated"
#define XMP_KEYWORDS "Xmp.dc.subject"

#define IPTC_DESCRIPTION "Iptc.Application2.Caption"
#define IPTC_TITLE "Iptc.Application2.ObjectName"
#define IPTC_KEYWORDS "Iptc.Application2.Keywords"
#define IPTC_CHARSET "Iptc.Envelope.CharacterSet"

#define EXIF_USERCOMMENT "Exif.Photo.UserComment"
#define EXIF_DESCRIPTION "Exif.Image.ImageDescription"
#define EXIF_PHOTO_DATETIMEORIGINAL "Exif.Photo.DateTimeOriginal"
#define EXIF_IMAGE_DATETIMEORIGINAL "Exif.Image.DateTimeOriginal"

Exiv2InitHelper::Exiv2InitHelper() {
    Exiv2::XmpParser::initialize();
}

Exiv2InitHelper::~Exiv2InitHelper() {
    Exiv2::XmpParser::terminate();
}

// ----------------------------------------------------------------------

// helper from libkexiv2
bool isUtf8(const char * const buffer) {
    int i, n;
    register unsigned char c;
    bool gotone = false;

    if (!buffer) {
        return true;
    }

    // character never appears in text
#define F 0
    // character appears in plain ASCII text
#define T 1
    // character appears in ISO-8859 text
#define I 2
    // character appears in non-ISO extended ASCII (Mac, IBM PC)
#define X 3

    static const unsigned char text_chars[256] =
    {
        //                  BEL BS HT LF    FF CR
        F, F, F, F, F, F, F, T, T, T, T, F, T, T, F, F,  // 0x0X
        //                              ESC
        F, F, F, F, F, F, F, F, F, F, F, T, F, F, F, F,  // 0x1X
        T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  // 0x2X
        T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  // 0x3X
        T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  // 0x4X
        T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  // 0x5X
        T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  // 0x6X
        T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, F,  // 0x7X
        //            NEL
        X, X, X, X, X, T, X, X, X, X, X, X, X, X, X, X,  // 0x8X
        X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,  // 0x9X
        I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  // 0xaX
        I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  // 0xbX
        I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  // 0xcX
        I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  // 0xdX
        I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  // 0xeX
        I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I   // 0xfX
    };

    for (i = 0; (c = buffer[i]); ++i) {
        if ((c & 0x80) == 0) {
            // 0xxxxxxx is plain ASCII

            // Even if the whole file is valid UTF-8 sequences,
            // still reject it if it uses weird control characters.

            if (text_chars[c] != T) {
                return false;
            }

        }
        else if ((c & 0x40) == 0) {
            // 10xxxxxx never 1st byte
            return false;
        }
        else {
            // 11xxxxxx begins UTF-8
            int following = 0;

            if ((c & 0x20) == 0) {
                // 110xxxxx
                following = 1;
            }
            else if ((c & 0x10) == 0) {
                // 1110xxxx
                following = 2;
            }
            else if ((c & 0x08) == 0) {
                // 11110xxx
                following = 3;
            }
            else if ((c & 0x04) == 0) {
                // 111110xx
                following = 4;
            }
            else if ((c & 0x02) == 0) {
                // 1111110x
                following = 5;
            }
            else {
                return false;
            }

            for (n = 0; n < following; ++n) {
                i++;

                if (!(c = buffer[i])) { goto done; }

                if ((c & 0x80) == 0 || (c & 0x40)) {
                    return false;
                }
            }

            gotone = true;
        }
    }

done:
    return gotone;   // don't claim it's UTF-8 if it's all 7-bit.
}

#undef F
#undef T
#undef I
#undef X

// copy-paste code from libkexiv2
QString detectEncodingAndDecode(const std::string &value) {
    if (value.empty()) {
        return QString();
    }

    if (isUtf8(value.c_str())) {
        return QString::fromUtf8(value.c_str());
    }

    // Utf8 has a pretty unique byte pattern.
    // Thats not true for ASCII, it is not possible
    // to reliably autodetect different ISO-8859 charsets.
    // So we can use either local encoding, or latin1.

    //TODO: KDE4PORT: check for regression of #134999 (very probably no regression!)
    return QString::fromLocal8Bit(value.c_str());
}

// ----------------------------------------------------------------------

QStringList decomposeKeyword(const QString &keyword) {
    return keyword.split(',', QString::SkipEmptyParts);
}

QString getIptcCharset(Exiv2::IptcData &iptcData) {
    QString iptcCharset = "";

    try {
        const char *charsetPtr = iptcData.detectCharset();
        if (charsetPtr != NULL) {
            iptcCharset = QString::fromLatin1(charsetPtr).toUpper();
        }
    }
    catch (Exiv2::Error &e) {
        LOG_WARNING << "Exiv2 error:" << e.what();
    }

    return iptcCharset;
}

bool getXmpLangAltValue(Exiv2::XmpData &xmpData, const char *propertyName,
                        const QString &langAlt, QString &resultValue) {
    bool anyFound = false;

    Exiv2::XmpKey key(propertyName);
    Exiv2::XmpData::iterator it = xmpData.findKey(key);
    if ((it != xmpData.end()) && (it->typeId() == Exiv2::langAlt)) {
        const Exiv2::LangAltValue &value = static_cast<const Exiv2::LangAltValue &>(it->value());

        QString anyValue;

        Exiv2::LangAltValue::ValueType::const_iterator it2 = value.value_.begin();
        Exiv2::LangAltValue::ValueType::const_iterator end = value.value_.end();
        for (; it2 != end; ++it2) {
            QString lang = QString::fromUtf8(it2->first.c_str());

            if (langAlt == lang) {
                QString text = QString::fromUtf8(it2->second.c_str()).trimmed();
                if (!text.isEmpty()) {
                    anyFound = true;
                    resultValue = text.trimmed();
                    break;
                }
            }

            if (anyValue.isEmpty()) {
                QString text = QString::fromUtf8(it2->second.c_str());
                anyValue = text.trimmed();
            }
        }

        if (!anyFound && !anyValue.isEmpty()) {
            anyFound = true;
            resultValue = anyValue;
        }
    }

    return anyFound;
}

bool getXmpDescription(Exiv2::XmpData &xmpData, const QString &langAlt, QString &description) {
    bool anyFound = false;

    try {
        anyFound = getXmpLangAltValue(xmpData, XMP_DESCRIPTION, langAlt, description);

        if (!anyFound || description.isEmpty()) {
            Exiv2::XmpKey psKey(XMP_PS_HEADLINE);
            Exiv2::XmpData::iterator xmpIt = xmpData.findKey(psKey);
            if (xmpIt != xmpData.end()) {
                const Exiv2::XmpTextValue &value = static_cast<const Exiv2::XmpTextValue &>(xmpIt->value());
                QString headline = QString::fromUtf8(value.value_.c_str()).trimmed();

                if (!headline.isEmpty()) {
                    anyFound = true;
                    description = headline;
                }
            }
        }
    }
    catch (Exiv2::Error &e) {
        LOG_WARNING << "Exiv2 error:" << e.what();
        anyFound = false;
    }
    catch (...) {
        LOG_WARNING << "Exception";
        anyFound = false;
#ifdef QT_DEBUG
        throw;
#endif
    }

    return anyFound;
}

bool getXmpTitle(Exiv2::XmpData &xmpData, const QString &langAlt, QString &title) {
    bool anyFound = false;

    try {
        anyFound = getXmpLangAltValue(xmpData, XMP_TITLE, langAlt, title);
    }
    catch (Exiv2::Error &e) {
        LOG_WARNING << "Exiv2 error:" << e.what();
        anyFound = false;
    }
    catch (...) {
        LOG_WARNING << "Exception";
        anyFound = false;
#ifdef QT_DEBUG
        throw;
#endif
    }

    return anyFound;
}

bool getXmpTagStringBag(Exiv2::XmpData &xmpData, const char *propertyName, QStringList &bag) {
    bool found = false;

    Exiv2::XmpKey key(propertyName);
    Exiv2::XmpData::iterator it = xmpData.findKey(key);

    if ((it != xmpData.end()) && (it->typeId() == Exiv2::xmpBag)) {
        found = true;
        int count = it->count();
        bag.reserve(count);

        if (count == 1) {
            QString bagValue = QString::fromUtf8(it->toString(0).c_str());
            if (bagValue.contains(',')) {
                LOG_DEBUG << "processing legacy saved keywords";
                bag += decomposeKeyword(bagValue);
            } else {
                bag.append(bagValue);
            }
        } else {
            for (int i = 0; i < count; i++) {
                QString bagValue = QString::fromUtf8(it->toString(i).c_str());
                bag.append(bagValue);
            }
        }
    }

    return found;
}

bool getXmpDateTime(Exiv2::XmpData &xmpData, QDateTime &dateTime) {
    bool anyFound = false;

    try {
        Exiv2::XmpKey psKey(XMP_PS_DATECREATED);
        Exiv2::XmpData::iterator xmpIt = xmpData.findKey(psKey);

        if (xmpIt != xmpData.end()) {
            dateTime = QDateTime::fromString(QString::fromLatin1(xmpIt->toString().c_str()), Qt::ISODate);
            anyFound = dateTime.isValid();
        }
    }
    catch (Exiv2::Error &e) {
        LOG_WARNING << "Exiv2 error:" << e.what();
        anyFound = false;
    }
    catch (...) {
        LOG_WARNING << "Exception";
        anyFound = false;
#ifdef QT_DEBUG
        throw;
#endif
    }

    return anyFound;
}

bool getXmpKeywords(Exiv2::XmpData &xmpData, QStringList &keywords) {
    bool anyFound = false;

    try {
        anyFound = getXmpTagStringBag(xmpData, XMP_KEYWORDS, keywords);
    }
    catch (Exiv2::Error &e) {
        LOG_WARNING << "Exiv2 error:" << e.what();
        anyFound = false;
    }
    catch (...) {
        LOG_WARNING << "Exception";
        anyFound = false;
#ifdef QT_DEBUG
        throw;
#endif
    }

    return anyFound;
}

bool getIptcString(Exiv2::IptcData &iptcData, const char *propertyName, bool isIptcUtf8, QString &resultValue) {
    bool anyFound = false;

    Exiv2::IptcKey key(propertyName);

    Exiv2::IptcData::iterator it = iptcData.findKey(key);
    if (it != iptcData.end()) {
        std::ostringstream os;
        os << *it;
        std::string str = os.str();

        QString value;

        if (isIptcUtf8 || isUtf8(str.c_str())) {
            value = QString::fromUtf8(str.c_str()).trimmed();
        } else {
            value = QString::fromLocal8Bit(str.c_str()).trimmed();
        }

        if (!value.isEmpty()) {
            resultValue = value;
            anyFound = true;
        }
    }

    return anyFound;
}

bool getIptcDescription(Exiv2::IptcData &iptcData, bool isIptcUtf8, QString &description) {
    bool anyFound = false;

    try {
        anyFound = getIptcString(iptcData, IPTC_DESCRIPTION, isIptcUtf8, description);
    }
    catch (Exiv2::Error &e) {
        LOG_WARNING << "Exiv2 error:" << e.what();
        anyFound = false;
    }
    catch (...) {
        LOG_WARNING << "Exception";
        anyFound = false;
#ifdef QT_DEBUG
        throw;
#endif
    }

    return anyFound;
}

bool getIptcTitle(Exiv2::IptcData &iptcData, bool isIptcUtf8, QString &title) {
    bool anyFound = false;

    try {
        anyFound = getIptcString(iptcData, IPTC_TITLE, isIptcUtf8, title);
    }
    catch (Exiv2::Error &e) {
        LOG_WARNING << "Exiv2 error:" << e.what();
        anyFound = false;
    }
    catch (...) {
        LOG_WARNING << "Exception";
        anyFound = false;
#ifdef QT_DEBUG
        throw;
#endif
    }

    return anyFound;
}

bool getIptcKeywords(Exiv2::IptcData &iptcData, bool isIptcUtf8, QStringList &keywords) {
    bool anyAdded = false;

    try {
        QString keywordsTagName = QString::fromLatin1(IPTC_KEYWORDS);

        for (Exiv2::IptcData::iterator it = iptcData.begin(); it != iptcData.end(); ++it) {
            QString key = QString::fromLocal8Bit(it->key().c_str());

            if (key == keywordsTagName) {
                QString tag;
                if (isIptcUtf8) {
                    tag = QString::fromUtf8(it->toString().c_str());
                } else {
                    tag = QString::fromLocal8Bit(it->toString().c_str());
                }

                keywords.append(tag);
                anyAdded = true;
            }
        }

        if (keywords.length() == 1 && keywords[0].contains(',')) {
            LOG_DEBUG << "processing legacy saved keywords";
            QString composite = keywords[0];
            keywords.clear();
            keywords += decomposeKeyword(composite);
        }
    }
    catch (Exiv2::Error &e) {
        LOG_WARNING << "Exiv2 error:" << e.what();
        anyAdded = false;
    }
    catch (...) {
        LOG_WARNING << "Exception";
        anyAdded = false;
#ifdef QT_DEBUG
        throw;
#endif
    }

    return anyAdded;
}

QString getExifCommentValue(Exiv2::ExifData &exifData, const char *propertyName) {
    QString result;

    Exiv2::ExifKey key(propertyName);
    Exiv2::ExifData::iterator it = exifData.findKey(key);
    if (it != exifData.end()) {
        const Exiv2::Exifdatum& exifDatum = *it;

        std::string comment;
        std::string charset;

        comment = exifDatum.toString();

        // libexiv2 will prepend "charset=\"SomeCharset\" " if charset is specified
        // Before conversion to QString, we must know the charset, so we stay with std::string for a while
        if (comment.length() > 8 && comment.substr(0, 8) == "charset=") {
            // the prepended charset specification is followed by a blank
            std::string::size_type pos = comment.find_first_of(' ');

            if (pos != std::string::npos) {
                // extract string between the = and the blank
                charset = comment.substr(8, pos-8);
                // get the rest of the string after the charset specification
                comment = comment.substr(pos+1);
            }
        }

        if (charset == "\"Unicode\"") {
            result = QString::fromUtf8(comment.data());
        }
        else if (charset == "\"Jis\"") {
            QTextCodec* const codec = QTextCodec::codecForName("JIS7");
            result = codec->toUnicode(comment.c_str());
        }
        else if (charset == "\"Ascii\"") {
            result = QString::fromLatin1(comment.c_str());
        } else {
            result = detectEncodingAndDecode(comment);
        }
    }

    return result;
}

bool getExifDescription(Exiv2::ExifData &exifData, QString &description) {
    bool foundDesc = false;

    try {
        QString value = getExifCommentValue(exifData, EXIF_DESCRIPTION).trimmed();

        if (value.isEmpty()) {
            value = getExifCommentValue(exifData, EXIF_USERCOMMENT).trimmed();
        }

        if (!value.isEmpty()) {
            description = value;
            foundDesc = true;
        }
    }
    catch (Exiv2::Error &e) {
        LOG_WARNING << "Exiv2 error:" << e.what();
        foundDesc = false;
    }
    catch (...) {
        LOG_WARNING << "Exception";
        foundDesc = false;
#ifdef QT_DEBUG
        throw;
#endif
    }

    return foundDesc;
}

QString retrieveDescription(Exiv2::XmpData &xmpData, Exiv2::ExifData &exifData, Exiv2::IptcData &iptcData,
                            bool isIptcUtf8) {
    QString description;
    bool success = false;
    success = getXmpDescription(xmpData, X_DEFAULT, description);
    success = success || getIptcDescription(iptcData, isIptcUtf8, description);
    success = success || getExifDescription(exifData, description);
    return description;
}

QString retrieveTitle(Exiv2::XmpData &xmpData, Exiv2::ExifData &exifData, Exiv2::IptcData &iptcData,
                      bool isIptcUtf8) {
    QString title;
    bool success = false;
    success = getXmpTitle(xmpData, X_DEFAULT, title);
    success = success || getIptcTitle(iptcData, isIptcUtf8, title);
    Q_UNUSED(exifData);
    return title;
}

QStringList retrieveKeywords(Exiv2::XmpData &xmpData, Exiv2::ExifData &exifData, Exiv2::IptcData &iptcData,
                             bool isIptcUtf8) {
    QStringList keywords;
    bool success = false;
    success = getXmpKeywords(xmpData, keywords);
    success = success || getIptcKeywords(iptcData, isIptcUtf8, keywords);
    Q_UNUSED(exifData);
    return keywords;
}

void readMetadata(const QString &filepath, BasicMetadata &importResult) {
#if defined(Q_OS_WIN)
    Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(filepath.toStdWString());
#else
    Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(filepath.toStdString());
#endif
    Q_ASSERT(image.get() != NULL);
    image->readMetadata();

    Exiv2::XmpData &xmpData = image->xmpData();
    Exiv2::ExifData &exifData = image->exifData();
    Exiv2::IptcData &iptcData = image->iptcData();

    QString iptcEncoding = getIptcCharset(iptcData).toUpper();
    bool isIptcUtf8 = (iptcEncoding == QLatin1String("UTF-8")) || (iptcEncoding == QLatin1String("UTF8"));

    importResult.m_Description = retrieveDescription(xmpData, exifData, iptcData, isIptcUtf8);
    importResult.m_Title = retrieveTitle(xmpData, exifData, iptcData, isIptcUtf8);
    importResult.m_Keywords = retrieveKeywords(xmpData, exifData, iptcData, isIptcUtf8);
}

void readMetadataEx(const QString &filepath, BasicMetadata &xmpMetadata, BasicMetadata &exifMetadata, BasicMetadata &iptcMetadata) {
#if defined(Q_OS_WIN)
    Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(filepath.toStdWString());
#else
    Exiv2::Image::AutoPtr image = Exiv2::ImageFactory::open(filepath.toStdString());
#endif
    Q_ASSERT(image.get() != NULL);
    image->readMetadata();

    Exiv2::XmpData &xmpData = image->xmpData();
    Exiv2::ExifData &exifData = image->exifData();
    Exiv2::IptcData &iptcData = image->iptcData();

    QString iptcEncoding = getIptcCharset(iptcData).toUpper();
    bool isIptcUtf8 = (iptcEncoding == QLatin1String("UTF-8")) || (iptcEncoding == QLatin1String("UTF8"));

    getXmpDescription(xmpData, X_DEFAULT, xmpMetadata.m_Description);
    getXmpTitle(xmpData, X_DEFAULT, xmpMetadata.m_Title);
    getXmpKeywords(xmpData, xmpMetadata.m_Keywords);

    getExifDescription(exifData, exifMetadata.m_Description);

    getIptcDescription(iptcData, isIptcUtf8, iptcMetadata.m_Description);
    getIptcTitle(iptcData, isIptcUtf8, iptcMetadata.m_Title);
    getIptcKeywords(iptcData, isIptcUtf8, iptcMetadata.m_Keywords);
}
