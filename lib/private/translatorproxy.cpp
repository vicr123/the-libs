#include "translatorproxy.h"

struct TranslatorProxyPrivate {
    QTranslator* primaryTranslator;

    static QStringList numberExtensions;
    static QString accentReplacements;
};

QStringList TranslatorProxyPrivate::numberExtensions {
    "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten",
    "eleven", "twelve", "thirteen", "fourteen", "fifteen", "sixteen", "seventeen", "eighteen", "nineteen", "twenty"
};

QString TranslatorProxyPrivate::accentReplacements =
    QStringLiteral("AÅBβCÇDĐEÊGĜHĤIÎJĴKĶLĻMḾNÑOÖPÞRŔSŠTŢUÛVvWŴYÝZaåcçdðeéffgĝhĥiîjĵkķlļmḿnñoöpþrŕsštţuûvVwŵyýzƶ");

TranslatorProxy::TranslatorProxy(QTranslator* parentTranslator, QObject* parent)
    : QTranslator(parent) {
    d = new TranslatorProxyPrivate();
    d->primaryTranslator = parentTranslator;
}

TranslatorProxy::~TranslatorProxy() {
    delete d;
}

QString TranslatorProxy::translate(const char* context, const char* sourceText, const char* disambiguation, int n) const {
    QString translatedString = d->primaryTranslator->translate(context, sourceText, disambiguation, n);

    if (qEnvironmentVariable("THELIBS_ENABLE_PSEUDOTRANSLATION") == "1") {
        if (translatedString.isEmpty()) {
            translatedString = sourceText;
            if (translatedString.contains("%n")) translatedString.replace("%n", QString::number(n));
        }

        int extensionLength = translatedString.length() * 0.7;
        QStringList allExtensions = d->numberExtensions;
        QStringList usedExtensions;
        while (extensionLength > 0) {
            QString str = allExtensions.takeFirst();
            usedExtensions.append(str);
            extensionLength -= str.length() + 1;

            if (allExtensions.isEmpty()) allExtensions = d->numberExtensions;
        }

        QString accentedString;
        for (QChar character : qAsConst(translatedString)) {
            int index = d->accentReplacements.indexOf(character);
            if (index == -1) {
                accentedString.append(character);
            } else {
                accentedString.append(d->accentReplacements.at(index + 1));
            }
        }

        translatedString = QStringLiteral("[%2 %1]").arg(usedExtensions.join(" "), accentedString);
    }

    return translatedString;
}

bool TranslatorProxy::isEmpty() const {
    return d->primaryTranslator->isEmpty();
}
