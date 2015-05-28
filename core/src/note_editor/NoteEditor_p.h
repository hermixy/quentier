#ifndef __QUTE_NOTE__CORE__NOTE_EDITOR__NOTE_EDITOR_PRIVATE_H
#define __QUTE_NOTE__CORE__NOTE_EDITOR__NOTE_EDITOR_PRIVATE_H

#include "NoteEditor.h"
#include <QObject>

QT_FORWARD_DECLARE_CLASS(QByteArray)
QT_FORWARD_DECLARE_CLASS(QMimeType)
QT_FORWARD_DECLARE_CLASS(QImage)

namespace qute_note {


class NoteEditorPrivate: public QObject
{
    Q_OBJECT
public:
    explicit NoteEditorPrivate(NoteEditor & noteEditor);

    void execJavascriptCommand(const QString & command);
    void execJavascriptCommand(const QString & command, const QString & args);

    void setNote(const Note & note);
    const Note * getNote() const;

    bool isModified() const;

    void onDropEvent(QDropEvent * pEvent);
    void dropFile(QString & filepath);
    void attachResourceToNote(const QByteArray & data, const QString & dataHash, const QMimeType & mimeType);

    template <typename T>
    QString composeHtmlTable(const T width, const T singleColumnWidth, const int rows, const int columns,
                             const bool relative, const size_t tableId);

    void insertImage(const QByteArray & data,  const QString & dataHash, const QMimeType & mimeType);
    void insertToDoCheckbox();

    void setFont(const QFont & font);
    void setFontHeight(const int height);
    void setFontColor(const QColor & color);
    void setBackgroundColor(const QColor & color);
    void insertHorizontalLine();
    void changeIndentation(const bool increase);
    void insertBulletedList();
    void insertNumberedList();
    void insertFixedWidthTable(const int rows, const int columns, const int widthInPixels);
    void insertRelativeWidthTable(const int rows, const int columns, const double relativeWidth);

public:
    // TODO: deprecate these methods in favour of NoteEditorPluginFactory
    void addResourceInserterForMimeType(const QString & mimeTypeName,
                                        INoteEditorResourceInserter * pResourceInserter);

    bool removeResourceInserterForMimeType(const QString & mimeTypeName);

    bool hasResourceInsertedForMimeType(const QString & mimeTypeName) const;

Q_SIGNALS:
    void notifyError(QString error);

private Q_SLOTS:
    void onNoteLoadFinished(bool ok);

private:
    QString     m_jQuery;
    QString     m_resizableColumnsPlugin;
    QString     m_onFixedWidthTableResize;

    Note *      m_pNote;
    bool        m_modified;
    QHash<QString, INoteEditorResourceInserter*>    m_noteEditorResourceInserters;
    size_t      m_lastFreeId;

    NoteEditor * const q_ptr;
    Q_DECLARE_PUBLIC(NoteEditor)
};

} // namespace qute_note

void __initNoteEditorResources();

#endif // __QUTE_NOTE__CORE__NOTE_EDITOR__NOTE_EDITOR_PRIVATE_H
