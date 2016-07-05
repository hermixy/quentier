#ifndef LIB_QUENTIER_EXCEPTION_SHARED_NOTEBOOK_ADAPTER_ACCESS_EXCEPTION_H
#define LIB_QUENTIER_EXCEPTION_SHARED_NOTEBOOK_ADAPTER_ACCESS_EXCEPTION_H

#include <quentier/exception/IQuentierException.h>

namespace quentier {

class QUENTIER_EXPORT SharedNotebookAdapterAccessException: public IQuentierException
{
public:
    explicit SharedNotebookAdapterAccessException(const QString & message);

protected:
    virtual const QString exceptionDisplayName() const Q_DECL_OVERRIDE;
};

} // namespace quentier

#endif // LIB_QUENTIER_EXCEPTION_SHARED_NOTEBOOK_ADAPTER_ACCESS_EXCEPTION_H