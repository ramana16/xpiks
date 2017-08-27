#include "stringhelpers_tests.h"
#include "../../xpiks-qt/Helpers/stringhelper.h"
#include <QStringList>

void StringHelpersTests::splitEmptyStringTest() {
    QString text = "";
    QStringList result;
    Helpers::splitText(text, result);

    QVERIFY(result.isEmpty());
}

void StringHelpersTests::splitStringWithSpacesTest() {
    QString text = "    . , : -";
    QStringList result;
    Helpers::splitText(text, result);

    QVERIFY(result.isEmpty());
}

void StringHelpersTests::splitOneWordTest() {
    QString text = "oneword";
    QStringList result;
    Helpers::splitText(text, result);

    QCOMPARE(result.length(), 1);
    QCOMPARE(result[0], text);
}

void StringHelpersTests::splitOneWordWithSpacesTest() {
    QString text = "  oneword    ";
    QStringList result;
    Helpers::splitText(text, result);

    QCOMPARE(result.length(), 1);
    QCOMPARE(result[0], text.trimmed());
}

void StringHelpersTests::splitSeveralWordsWithSpacesTest() {
    QString text = "  OneworD  and Another anD   one more   ";
    QStringList result;
    Helpers::splitText(text, result);

    QStringList expected;
    expected << "OneworD" << "and" << "Another" << "anD" << "one" << "more";

    QCOMPARE(result, expected);
}

void StringHelpersTests::splitOneLetterWordTest() {
    QString text = "  OneworD b    and Another c anD a  one more   ";
    QStringList result;
    Helpers::splitText(text, result);

    QStringList expected;
    expected << "OneworD" << "b" << "and" << "Another" << "c" << "anD" << "a" << "one" << "more";

    QCOMPARE(result, expected);
}

void StringHelpersTests::splitSeveralWordsWithPunctuationTest() {
    QString text = "  OneworD: & and,Anot$her.anD @ - one more...   ";
    QStringList result;
    Helpers::splitText(text, result);

    QStringList expected;
    expected << "OneworD" << "and" << "Anot$her" << "anD" << "one" << "more";

    QCOMPARE(result, expected);
}

void StringHelpersTests::replaceWholeSmokeTest() {
    QString text = "Word";
    QString replaced = Helpers::replaceWholeWords(text, "word", "Bob");
    QCOMPARE(replaced, QLatin1String("Bob"));
}

void StringHelpersTests::replaceWholeTrivialTest() {
    QString text = "a whole  Word  heWordre";
    QString replaced = Helpers::replaceWholeWords(text, "Word", "Bob");
    QCOMPARE(replaced, QLatin1String("a whole  Bob  heWordre"));
}

void StringHelpersTests::replaceWholeTrivialCaseSensitiveTest() {
    QString text = "a whwordole  Word word heWordre";
    QString replaced = Helpers::replaceWholeWords(text, "word", "Bob", Qt::CaseSensitive);
    QCOMPARE(replaced, QLatin1String("a whwordole  Word Bob heWordre"));
}

void StringHelpersTests::replaceWholePrefixTest() {
    QString text = "Word whole  Words heWordre";
    QString replaced = Helpers::replaceWholeWords(text, "Word", "Bob");
    QCOMPARE(replaced, QLatin1String("Bob whole  Words heWordre"));
}

void StringHelpersTests::replaceWholeSuffixTest() {
    QString text = "Word whole Words word";
    QString replaced = Helpers::replaceWholeWords(text, "word", "Bob", Qt::CaseSensitive);
    QCOMPARE(replaced, QLatin1String("Word whole Words Bob"));
}

void StringHelpersTests::replaceWholeWithPunctuationTest() {
    QString text = "Word whole, Words word";
    QString replaced = Helpers::replaceWholeWords(text, "whole", "Bob");
    QCOMPARE(replaced, QLatin1String("Word Bob, Words word"));
}

void StringHelpersTests::replaceWholeWithSpaceTest() {
    QString text = "Word whole  Words word";
    QString replaced = Helpers::replaceWholeWords(text, "whole ", "Bob");
    QCOMPARE(replaced, QLatin1String("Word Bob Words word"));
}

void StringHelpersTests::replaceWholeWithCommaTest() {
    QString text = "Word whole;, Words word";
    QString replaced = Helpers::replaceWholeWords(text, "whole;", "Bob");
    QCOMPARE(replaced, QLatin1String("Word Bob, Words word"));
}

void StringHelpersTests::replaceWholeNoCaseHitTest() {
    QString text = "Word inWord and the Wordend";
    QString replaced = Helpers::replaceWholeWords(text, "word", "Bob", Qt::CaseSensitive);
    QCOMPARE(replaced, text);
}

void StringHelpersTests::replaceWholeNoHitTest() {
    QString text = "Word inWord and the Wordend";
    QString replaced = Helpers::replaceWholeWords(text, "whole", "Bob");
    QCOMPARE(replaced, text);
}

void StringHelpersTests::switcherHashTest() {
    QCOMPARE(Helpers::switcherHash(""), quint32(3820012610));
    QCOMPARE(Helpers::switcherHash("-"), quint32(963895330));
    QCOMPARE(Helpers::switcherHash("a"), quint32(3398926610));
    QCOMPARE(Helpers::switcherHash("0"), quint32(1609362278));
    QCOMPARE(Helpers::switcherHash("foo"), quint32(740734059));

    QCOMPARE(Helpers::switcherHash("261aad35-2cce-41e3-afed-ae97831eb2be"), quint32(3867831621));
    QCOMPARE(Helpers::switcherHash("31f8f3e7-b003-481a-b0c9-6afd1a07daea"), quint32(809399339));
    QCOMPARE(Helpers::switcherHash("5c2bd3c9-a063-4974-9fd7-542c0ec5e225"), quint32(1798162191));
    QCOMPARE(Helpers::switcherHash("48eaeb10-1d03-4020-a747-604616cedf28"), quint32(535900959));
    QCOMPARE(Helpers::switcherHash("dc16d9fe-61d6-4564-931b-650e42fcf443"), quint32(3282680053));

}
