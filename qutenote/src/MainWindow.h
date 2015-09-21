#ifndef __QUTE_NOTE__MAINWINDOW_H
#define __QUTE_NOTE__MAINWINDOW_H

#include <qute_note/types/Notebook.h>
#include <qute_note/types/Note.h>
#include <qute_note/utility/Qt4Helper.h>

#include <QtCore>

#if QT_VERSION >= 0x050000
#include <QtWidgets/QMainWindow>
#else
#include <QMainWindow>
#endif
#include <QTextListFormat>

namespace Ui {
class MainWindow;
}

QT_FORWARD_DECLARE_CLASS(QUrl)

namespace qute_note {
QT_FORWARD_DECLARE_CLASS(NoteEditor)
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget * pParentWidget = Q_NULLPTR);
    virtual ~MainWindow();

private:
    void connectActionsToEditorSlots();
    void connectEditorSignalsToSlots();

public Q_SLOTS:
    void onSetStatusBarText(QString message, const int duration = 0);

private Q_SLOTS:
    void onNoteTextBoldToggled();
    void onNoteTextItalicToggled();
    void onNoteTextUnderlineToggled();
    void onNoteTextStrikethroughToggled();
    void onNoteTextAlignLeftAction();
    void onNoteTextAlignCenterAction();
    void onNoteTextAlignRightAction();
    void onNoteTextAddHorizontalLineAction();
    void onNoteTextIncreaseIndentationAction();
    void onNoteTextDecreaseIndentationAction();
    void onNoteTextInsertUnorderedListAction();
    void onNoteTextInsertOrderedListAction();

    void onNoteChooseTextColor();
    void onNoteChooseSelectedTextColor();

    void onNoteTextSpellCheckToggled() { /* TODO: implement */ }
    void onNoteTextInsertToDoCheckBoxAction();

    void onNoteTextInsertTableDialogAction();
    void onNoteTextInsertTable(int rows, int columns, double width, bool relativeWidth);

    void onShowNoteSource();
    void onSetTestNoteWithEncryptedData();
    void onSetTestNoteWithResources();

    void onNoteEditorHtmlUpdate(QString html);
    void onNoteEditorError(QString error);

    // Slots used to reflect the change of formatting for the piece of text being the one currently pointed to by the text cursor in the note editor
    void onNoteEditorBoldStateChanged(bool state);
    void onNoteEditorItalicStateChanged(bool state);
    void onNoteEditorUnderlineStateChanged(bool state);
    void onNoteEditorStrikethroughStateChanged(bool state);
    void onNoteEditorAlignLeftStateChanged(bool state);
    void onNoteEditorAlignCenterStateChanged(bool state);
    void onNoteEditorAlignRightStateChanged(bool state);
    void onNoteEditorInsideOrderedListStateChanged(bool state);
    void onNoteEditorInsideUnorderedListStateChanged(bool state);
    void onNoteEditorInsideTableStateChanged(bool state);

private:
    void checkThemeIconsAndSetFallbacks();
    void updateNoteHtmlView(QString html);

    bool consumerKeyAndSecret(QString & consumerKey, QString & consumerSecret, QString & error);

private:
    Ui::MainWindow * m_pUI;
    QWidget * m_currentStatusBarChildWidget;
    qute_note::NoteEditor * m_pNoteEditor;
    QString m_lastNoteEditorHtml;

    qute_note::Notebook    m_testNotebook;
    qute_note::Note        m_testNote;
};

#endif // __QUTE_NOTE__MAINWINDOW_H
