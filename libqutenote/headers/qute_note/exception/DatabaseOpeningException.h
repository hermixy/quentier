#ifndef __LIB_QUTE_NOTE__EXCEPTION__DATABASE_OPENING_EXCEPTION_H
#define __LIB_QUTE_NOTE__EXCEPTION__DATABASE_OPENING_EXCEPTION_H

#include <qute_note/exception/IQuteNoteException.h>

namespace qute_note {

class QUTE_NOTE_EXPORT DatabaseOpeningException: public IQuteNoteException
{
public:
    explicit DatabaseOpeningException(const QString & message);

protected:
    virtual const QString exceptionDisplayName() const Q_DECL_OVERRIDE;
};

}

#endif // __LIB_QUTE_NOTE__EXCEPTION__DATABASE_OPENING_EXCEPTION_H
