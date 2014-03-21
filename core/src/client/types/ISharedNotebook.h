#ifndef __QUTE_NOTE__CLIENT__TYPES__I_SHARED_NOTEBOOK_H
#define __QUTE_NOTE__CLIENT__TYPES__I_SHARED_NOTEBOOK_H

#include <tools/Printable.h>
#include <tools/TypeWithError.h>

namespace evernote {
namespace edam {
QT_FORWARD_DECLARE_CLASS(SharedNotebook)
}
}

namespace qute_note {

class ISharedNotebook: public Printable,
                       public TypeWithError
{
public:
    ISharedNotebook();
    virtual ~ISharedNotebook();

    bool hasId() const;
    qint64 id() const;
    void setId(const qint64 id);

    bool hasUsedId() const;
    qint32 userId() const;
    void setUserId(const qint32 userId);

    const bool hasNotebookGuid() const;
    const QString notebookGuid() const;
    void setNotebookGuid(const QString & notebookGuid);

    const bool hasEmail() const;
    const QString email() const;
    void setEmail(const QString & email);

    bool hasCreationTimestamp() const;
    qint64 creationTimestamp() const;
    void setCreationTimestamp(const qint64 timestamp);

    bool hasModificationTimestamp() const;
    qint64 modificationTimestamp() const;
    void setModificationTimestamp(const qint64 timestamp);

    bool hasShareKey() const;
    const QString shareKey() const;
    void setShareKey(const QString & shareKey);

    bool hasUsername() const;
    const QString username() const;
    void setUsername(const QString & username);

    bool hasPrivilegeLevel() const;
    qint8 privilegeLevel() const;
    void setPrivilegeLevel(const qint8 privilegeLevel);

    bool hasAllowPreview() const;
    bool allowPreview() const;
    void setAllowPreview(const bool allowPreview);

    bool hasReminderNotifyEmail() const;
    bool reminderNotifyEmail() const;
    void setReminderNotifyEmail(const bool notifyEmail);

    bool hasReminderNotifyApp() const;
    bool reminderNotifyApp() const;
    void setReminderNotifyApp(const bool notifyApp);

protected:
    ISharedNotebook(const ISharedNotebook & other);
    ISharedNotebook & operator=(const ISharedNotebook & other);

    virtual evernote::edam::SharedNotebook & GetEnSharedNotebook() = 0;
    virtual const evernote::edam::SharedNotebook & GetEnSharedNotebook() const = 0;

private:
    virtual QTextStream & Print(QTextStream & strm) const override;
};

} // namespace qute_note

#endif // __QUTE_NOTE__CLIENT__TYPES__I_SHARED_NOTEBOOK_H
