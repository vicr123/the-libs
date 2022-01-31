#ifndef TRANSLATORPROXY_H
#define TRANSLATORPROXY_H

#include <QTranslator>

struct TranslatorProxyPrivate;
class TranslatorProxy : public QTranslator {
        Q_OBJECT
    public:
        explicit TranslatorProxy(QTranslator* parentTranslator, QObject* parent = nullptr);
        ~TranslatorProxy();

    signals:

    private:
        TranslatorProxyPrivate* d;

        // QTranslator interface
    public:
        QString translate(const char* context, const char* sourceText, const char* disambiguation, int n) const override;
        bool isEmpty() const override;
};

#endif // TRANSLATORPROXY_H
