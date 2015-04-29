#ifndef __QUTE_NOTE__EVERNOTE_CLIENT__ENML_CONVERTER_H
#define __QUTE_NOTE__EVERNOTE_CLIENT__ENML_CONVERTER_H

#include <tools/Linkage.h>
#include <tools/qt4helper.h>
#include <QSet>
#include <QString>

QT_FORWARD_DECLARE_CLASS(QuteNoteTextEdit)
QT_FORWARD_DECLARE_CLASS(QTextFragment)
QT_FORWARD_DECLARE_CLASS(QTextCharFormat)
QT_FORWARD_DECLARE_CLASS(QDomElement)

namespace qevercloud {
QT_FORWARD_DECLARE_STRUCT(Note)
QT_FORWARD_DECLARE_STRUCT(Resource)
}

namespace qute_note {

QT_FORWARD_DECLARE_CLASS(Note)
QT_FORWARD_DECLARE_CLASS(HTMLCleaner)

class QUTE_NOTE_EXPORT ENMLConverter
{
public:
    ENMLConverter();
    virtual ~ENMLConverter();

    bool htmlToNoteContent(const QString & html, Note & note, QString & errorDescription) const;
    bool noteContentToHtml(const Note & note, QString & html, QString & errorDescription) const;

    bool validateEnml(const QString & enml) const;

    bool richTextToNoteContent(const QuteNoteTextEdit & noteEditor, QString & ENML,
                               QString & errorDescription) const;

    bool noteToRichText(const qevercloud::Note & note, QuteNoteTextEdit & noteEditor,
                        QString & errorMessage) const;

    bool noteContentToPlainText(const QString & noteContent, QString & plainText,
                                QString & errorMessage) const;

    bool noteContentToListOfWords(const QString & noteContent, QStringList & listOfWords,
                                  QString & errorMessage, QString * plainText = nullptr) const;

    static QStringList plainTextToListOfWords(const QString & plainText);

private:
    ENMLConverter(const ENMLConverter & other) Q_DECL_DELETE;
    ENMLConverter & operator=(const ENMLConverter & other) Q_DECL_DELETE;

private:
    void fillTagsLists();
    bool encodeFragment(const QTextFragment & fragment, QString & encodedFragment,
                        QString & errorMessage) const;
    const QString domElementToRawXML(const QDomElement & elem) const;

    bool isForbiddenXhtmlTag(const QString & tagName) const;
    bool isForbiddenXhtmlAttribute(const QString & attributeName) const;
    bool isEvernoteSpecificXhtmlTag(const QString & tagName) const;
    bool isAllowedXhtmlTag(const QString & tagName) const;

    static int indexOfResourceByHash(const QList<qevercloud::Resource> & resources,
                                     const QByteArray & hash);

    bool addEnMediaFromCharFormat(const QTextCharFormat & format, QString & ENML,
                                  QString & errorDescription) const;

private:
    mutable HTMLCleaner *   m_pHtmlCleaner;
    QSet<QString> m_forbiddenXhtmlTags;
    QSet<QString> m_forbiddenXhtmlAttributes;
    QSet<QString> m_evernoteSpecificXhtmlTags;
    QSet<QString> m_allowedXhtmlTags;
};

} // namespace qute_note

#endif // __QUTE_NOTE__EVERNOTE_CLIENT__ENML_CONVERTER_H
