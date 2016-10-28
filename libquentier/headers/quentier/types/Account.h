#ifndef LIB_QUENTIER_TYPES_ACCOUNT_H
#define LIB_QUENTIER_TYPES_ACCOUNT_H

#include <quentier/utility/Printable.h>
#include <quentier/utility/Qt4Helper.h>
#include <QString>
#include <QSharedDataPointer>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <qt5qevercloud/QEverCloud.h>
#else
#include <qt4qevercloud/QEverCloud.h>
#endif

namespace quentier {

QT_FORWARD_DECLARE_CLASS(AccountData)

/**
 * @brief The Account class encapsulates some details about the account: its name,
 * whether it is local or synchronized to Evernote and for the latter case -
 * some additional details like upload limit etc.
 */
class Account: public Printable
{
public:
    struct Type
    {
        enum type
        {
            Local = 0,
            Evernote
        };
    };

    struct EvernoteAccountType
    {
        enum type
        {
            Free = 0,
            Plus,
            Premium,
            Business
        };
    };

public:
    explicit Account(const QString & name, const Type::type type,
                     const qevercloud::UserID userId = -1,
                     const EvernoteAccountType::type evernoteAccountType = EvernoteAccountType::Free);
    Account(const Account & other);
    Account & operator=(const Account & other);
    virtual ~Account();

    /**
     * @return username for either local or Evernote account
     */
    QString name() const;

    /**
     * @return user id for Evernote accounts, -1 for local accounts (as the concept of user id is not defined for local accounts)
     */
    qevercloud::UserID id() const;

    /**
     * @return the type of the account: either local of Evernote
     */
    Type::type type() const;

    /**
     * @return the type of the Evernote account; if applied to free account, returns "Free"
     */
    EvernoteAccountType::type evernoteAccountType() const;

    void setEvernoteAccountType(const EvernoteAccountType::type evernoteAccountType);

    qint32 mailLimitDaily() const;
    qint64 noteSizeMax() const;
    qint64 resourceSizeMax() const;
    qint32 linkedNotebookMax() const;
    qint32 noteCountMax() const;
    qint32 notebookCountMax() const;
    qint32 tagCountMax() const;
    qint32 noteTagCountMax() const;
    qint32 savedSearchCountMax() const;
    qint32 noteResourceCountMax() const;
    void setEvernoteAccountLimits(const qevercloud::AccountLimits & limits);

    virtual QTextStream & print(QTextStream & strm) const Q_DECL_OVERRIDE;

private:
    QSharedDataPointer<AccountData> d;
};

} // namespace quentier

#endif // LIB_QUENTIER_TYPES_ACCOUNT_H