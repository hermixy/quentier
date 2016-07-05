#ifndef LIB_QUENTIER_EXCEPTION_RESOURCE_LOCAL_FILE_STORAGE_FOLDER_NOT_FOUND_EXCEPTION_H
#define LIB_QUENTIER_EXCEPTION_RESOURCE_LOCAL_FILE_STORAGE_FOLDER_NOT_FOUND_EXCEPTION_H

#include <quentier/exception/IQuentierException.h>

namespace quentier {

class QUENTIER_EXPORT ResourceLocalFileStorageFolderNotFoundException: public IQuentierException
{
public:
    explicit ResourceLocalFileStorageFolderNotFoundException(const QString & message);

protected:
    virtual const QString exceptionDisplayName() const Q_DECL_OVERRIDE;
};

} // namespace quentier

#endif // LIB_QUENTIER_EXCEPTION_RESOURCE_LOCAL_FILE_STORAGE_FOLDER_NOT_FOUND_EXCEPTION_H