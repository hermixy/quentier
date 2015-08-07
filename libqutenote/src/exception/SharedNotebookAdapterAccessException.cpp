#include <qute_note/exception/SharedNotebookAdapterAccessException.h>

namespace qute_note {

SharedNotebookAdapterAccessException::SharedNotebookAdapterAccessException(const QString & message) :
    IQuteNoteException(message)
{}

const QString SharedNotebookAdapterAccessException::exceptionDisplayName() const
{
    return "SharedNotebookAdapterAccessException";
}

} // namespace qute_note