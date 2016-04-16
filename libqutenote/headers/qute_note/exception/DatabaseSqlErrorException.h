#ifndef __LIB_QUTE_NOTE__EXCEPTION__DATABASE_SQL_ERROR_EXCEPTION_H
#define __LIB_QUTE_NOTE__EXCEPTION__DATABASE_SQL_ERROR_EXCEPTION_H

#include <qute_note/exception/IQuteNoteException.h>

namespace qute_note {

class QUTE_NOTE_EXPORT DatabaseSqlErrorException: public IQuteNoteException
{
public:
    explicit DatabaseSqlErrorException(const QString & message);

protected:
    virtual const QString exceptionDisplayName() const Q_DECL_OVERRIDE;
};

}

#endif // __LIB_QUTE_NOTE__EXCEPTION__DATABASE_SQL_ERROR_EXCEPTION_H
