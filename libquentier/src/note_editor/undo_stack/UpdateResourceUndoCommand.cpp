#include "UpdateResourceUndoCommand.h"
#include "../NoteEditor_p.h"
#include <quentier/logging/QuentierLogger.h>

namespace quentier {

UpdateResourceUndoCommand::UpdateResourceUndoCommand(const Resource & resourceBefore, const Resource & resourceAfter,
                                                     NoteEditorPrivate & noteEditorPrivate, QUndoCommand * parent) :
    INoteEditorUndoCommand(noteEditorPrivate, parent),
    m_resourceBefore(resourceBefore),
    m_resourceAfter(resourceAfter)
{
    init();
}

UpdateResourceUndoCommand::UpdateResourceUndoCommand(const Resource & resourceBefore, const Resource & resourceAfter,
                                                     NoteEditorPrivate & noteEditorPrivate, const QString & text, QUndoCommand * parent) :
    INoteEditorUndoCommand(noteEditorPrivate, text, parent),
    m_resourceBefore(resourceBefore),
    m_resourceAfter(resourceAfter)
{
    init();
}

UpdateResourceUndoCommand::~UpdateResourceUndoCommand()
{}

void UpdateResourceUndoCommand::undoImpl()
{
    QNDEBUG(QStringLiteral("UpdateResourceUndoCommand::undoImpl"));

    m_noteEditorPrivate.replaceResourceInNote(m_resourceBefore);
    m_noteEditorPrivate.updateFromNote();
}

void UpdateResourceUndoCommand::redoImpl()
{
    QNDEBUG(QStringLiteral("UpdateResourceUndoCommand::redoImpl"));

    m_noteEditorPrivate.replaceResourceInNote(m_resourceAfter);
    m_noteEditorPrivate.updateFromNote();
}

void UpdateResourceUndoCommand::init()
{
    setText(tr("Edit attachment"));
}

} // namespace quentier
