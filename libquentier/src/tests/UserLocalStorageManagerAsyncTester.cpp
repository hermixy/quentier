#include "UserLocalStorageManagerAsyncTester.h"
#include <quentier/local_storage/LocalStorageManagerThreadWorker.h>
#include <quentier/logging/QuentierLogger.h>
#include <QThread>

namespace quentier {
namespace test {

UserLocalStorageManagerAsyncTester::UserLocalStorageManagerAsyncTester(QObject * parent) :
    QObject(parent),
    m_state(STATE_UNINITIALIZED),
    m_pLocalStorageManagerThreadWorker(Q_NULLPTR),
    m_pLocalStorageManagerThread(Q_NULLPTR),
    m_userId(3),
    m_initialUser(),
    m_foundUser(),
    m_modifiedUser()
{}

UserLocalStorageManagerAsyncTester::~UserLocalStorageManagerAsyncTester()
{
    // NOTE: shouldn't attempt to delete m_pLocalStorageManagerThread as Qt's parent-child system
    // should take care of that
    if (m_pLocalStorageManagerThread) {
        m_pLocalStorageManagerThread->quit();
        m_pLocalStorageManagerThread->wait();
    }

    if (m_pLocalStorageManagerThreadWorker) {
        delete m_pLocalStorageManagerThreadWorker;
    }
}

void UserLocalStorageManagerAsyncTester::onInitTestCase()
{
    QString username = "UserLocalStorageManagerAsyncTester";
    bool startFromScratch = true;
    bool overrideLock = false;

    if (m_pLocalStorageManagerThread) {
        delete m_pLocalStorageManagerThread;
        m_pLocalStorageManagerThread = Q_NULLPTR;
    }

    if (m_pLocalStorageManagerThreadWorker) {
        delete m_pLocalStorageManagerThreadWorker;
        m_pLocalStorageManagerThreadWorker = Q_NULLPTR;
    }

    m_state = STATE_UNINITIALIZED;

    m_pLocalStorageManagerThread = new QThread(this);
    m_pLocalStorageManagerThreadWorker = new LocalStorageManagerThreadWorker(username, m_userId, startFromScratch, overrideLock);
    m_pLocalStorageManagerThreadWorker->moveToThread(m_pLocalStorageManagerThread);

    createConnections();

    m_pLocalStorageManagerThread->start();
}

void UserLocalStorageManagerAsyncTester::onWorkerInitialized()
{
    m_initialUser.setUsername("fakeusername");
    m_initialUser.setId(m_userId);
    m_initialUser.setEmail("Fake user email");
    m_initialUser.setName("Fake user name");
    m_initialUser.setTimezone("Europe/Moscow");
    m_initialUser.setPrivilegeLevel(qevercloud::PrivilegeLevel::NORMAL);
    m_initialUser.setCreationTimestamp(3);
    m_initialUser.setModificationTimestamp(3);
    m_initialUser.setActive(true);

    QString errorDescription;
    if (!m_initialUser.checkParameters(errorDescription)) {
        QNWARNING("Found invalid user: " << m_initialUser << ", error: " << errorDescription);
        emit failure(errorDescription);
        return;
    }

    m_state = STATE_SENT_ADD_REQUEST;
    emit addUserRequest(m_initialUser);
}

void UserLocalStorageManagerAsyncTester::onGetUserCountCompleted(int count, QUuid requestId)
{
    Q_UNUSED(requestId)

    QString errorDescription;

#define HANDLE_WRONG_STATE() \
    else { \
        errorDescription = "Internal error in UserLocalStorageManagerAsyncTester: " \
                           "found wrong state"; \
        emit failure(errorDescription); \
    }

    if (m_state == STATE_SENT_GET_COUNT_AFTER_UPDATE_REQUEST)
    {
        if (count != 1) {
            errorDescription = "GetUserCount returned result different from the expected one (1): ";
            errorDescription += QString::number(count);
            emit failure(errorDescription);
            return;
        }

        m_modifiedUser.setLocal(false);
        m_modifiedUser.setDeletionTimestamp(13);
        m_state = STATE_SENT_DELETE_REQUEST;
        emit deleteUserRequest(m_modifiedUser);
    }
    else if (m_state == STATE_SENT_GET_COUNT_AFTER_EXPUNGE_REQUEST)
    {
        if (count != 0) {
            errorDescription = "GetUserCount returned result different from the expected one (0): ";
            errorDescription += QString::number(count);
            emit failure(errorDescription);
            return;
        }

        emit success();
    }
    HANDLE_WRONG_STATE();
}

void UserLocalStorageManagerAsyncTester::onGetUserCountFailed(QString errorDescription, QUuid requestId)
{
    QNWARNING(errorDescription << ", requestId = " << requestId);
    emit failure(errorDescription);
}

void UserLocalStorageManagerAsyncTester::onAddUserCompleted(UserWrapper user, QUuid requestId)
{
    Q_UNUSED(requestId)

    QString errorDescription;

    if (m_state == STATE_SENT_ADD_REQUEST)
    {
        if (m_initialUser != user) {
            errorDescription = "Internal error in UserLocalStorageManagerAsyncTester: "
                               "user in onAddUserCompleted doesn't match the original UserWrapper";
            QNWARNING(errorDescription);
            emit failure(errorDescription);
            return;
        }

        m_foundUser = UserWrapper();
        m_foundUser.setId(user.id());

        m_state = STATE_SENT_FIND_AFTER_ADD_REQUEST;
        emit findUserRequest(m_foundUser);
    }
    HANDLE_WRONG_STATE();
}

void UserLocalStorageManagerAsyncTester::onAddUserFailed(UserWrapper user, QString errorDescription, QUuid requestId)
{
    QNWARNING(errorDescription << ", requestId = " << requestId << ", user: " << user);
    emit failure(errorDescription);
}

void UserLocalStorageManagerAsyncTester::onUpdateUserCompleted(UserWrapper user, QUuid requestId)
{
    Q_UNUSED(requestId)

    QString errorDescription;

    if (m_state == STATE_SENT_UPDATE_REQUEST)
    {
        if (m_modifiedUser != user) {
            errorDescription = "Internal error in UserLocalStorageManagerAsyncTester: "
                               "user in onUpdateUserCompleted slot doesn't match "
                               "the original modified UserWrapper";
            QNWARNING(errorDescription);
            emit failure(errorDescription);
            return;
        }

        m_state = STATE_SENT_FIND_AFTER_UPDATE_REQUEST;
        emit findUserRequest(m_foundUser);
    }
    HANDLE_WRONG_STATE();
}

void UserLocalStorageManagerAsyncTester::onUpdateUserFailed(UserWrapper user, QString errorDescription, QUuid requestId)
{
    QNWARNING(errorDescription << ", requestId = " << requestId << ", user: " << user);
    emit failure(errorDescription);
}

void UserLocalStorageManagerAsyncTester::onFindUserCompleted(UserWrapper user, QUuid requestId)
{
    Q_UNUSED(requestId)

    QString errorDescription;

    if (m_state == STATE_SENT_FIND_AFTER_ADD_REQUEST)
    {
        if (user != m_initialUser) {
            errorDescription = "Added and found users in local storage don't match";
            QNWARNING(errorDescription << ": UserWrapper added to LocalStorageManager: " << m_initialUser
                      << "\nUserWrapper found in LocalStorageManager: " << user);
            emit failure(errorDescription);
            return;
        }

        // Ok, found user is good, updating it now
        m_modifiedUser = m_initialUser;
        m_modifiedUser.setUsername(m_initialUser.username() + "_modified");
        m_modifiedUser.setName(m_initialUser.name() + "_modified");

        m_state = STATE_SENT_UPDATE_REQUEST;
        emit updateUserRequest(m_modifiedUser);
    }
    else if (m_state == STATE_SENT_FIND_AFTER_UPDATE_REQUEST)
    {
        if (user != m_modifiedUser) {
            errorDescription = "Updated and found users in local storage don't match";
            QNWARNING(errorDescription << ": UserWrapper updated in LocalStorageManager: " << m_modifiedUser
                      << "\nUserWrapper found in LocalStorageManager: " << user);
            emit failure(errorDescription);
            return;
        }

        m_state = STATE_SENT_GET_COUNT_AFTER_UPDATE_REQUEST;
        emit getUserCountRequest();
    }
    else if (m_state == STATE_SENT_FIND_AFTER_EXPUNGE_REQUEST)
    {
        errorDescription = "Error: found user which should have been expunged from local storage";
        QNWARNING(errorDescription << ": UserWrapper expunged from LocalStorageManager: " << m_modifiedUser
                  << "\nUserWrapper found in LocalStorageManager: " << user);
        emit failure(errorDescription);
        return;
    }
    HANDLE_WRONG_STATE();
}

void UserLocalStorageManagerAsyncTester::onFindUserFailed(UserWrapper user, QString errorDescription, QUuid requestId)
{
    if (m_state == STATE_SENT_FIND_AFTER_EXPUNGE_REQUEST) {
        m_state = STATE_SENT_GET_COUNT_AFTER_EXPUNGE_REQUEST;
        emit getUserCountRequest();
        return;
    }

    QNWARNING(errorDescription << ", requestId = " << requestId << ", user: " << user);
    emit failure(errorDescription);
}

void UserLocalStorageManagerAsyncTester::onDeleteUserCompleted(UserWrapper user, QUuid requestId)
{
    Q_UNUSED(requestId)

    QString errorDescription;

    if (m_modifiedUser != user) {
        errorDescription = "Internal error in UserLocalStorageManagerAsyncTester: "
                           "user in onDeleteUserCompleted slot doesn't match "
                           "the original deleted UserWrapper";
        QNWARNING(errorDescription << "; original deleted user: " << m_modifiedUser << ", user: " << user);
        emit failure(errorDescription);
        return;
    }

    m_modifiedUser.setLocal(true);
    m_state = STATE_SENT_EXPUNGE_REQUEST;
    emit expungeUserRequest(m_modifiedUser);
}

void UserLocalStorageManagerAsyncTester::onDeleteUserFailed(UserWrapper user, QString errorDescription, QUuid requestId)
{
    QNWARNING(errorDescription << ", requestId = " << requestId << ", user: " << user);
    emit failure(errorDescription);
}

void UserLocalStorageManagerAsyncTester::onExpungeUserCompleted(UserWrapper user, QUuid requestId)
{
    Q_UNUSED(requestId)

    QString errorDescription;

    if (m_modifiedUser != user) {
        errorDescription = "Internal error in UserLocalStorageManagerAsyncTester: "
                           "user in onExpungeUserCompleted slot doesn't match "
                           "the original expunged UserWrapper";
        QNWARNING(errorDescription);
        emit failure(errorDescription);
    }

    m_state = STATE_SENT_FIND_AFTER_EXPUNGE_REQUEST;
    emit findUserRequest(m_foundUser);
}

void UserLocalStorageManagerAsyncTester::onExpungeUserFailed(UserWrapper user, QString errorDescription, QUuid requestId)
{
    QNWARNING(errorDescription << ", requestId = " << requestId << ", user: " << user);
    emit failure(errorDescription);
}

void UserLocalStorageManagerAsyncTester::createConnections()
{
    QObject::connect(m_pLocalStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,failure,QString),
                     this, QNSIGNAL(UserLocalStorageManagerAsyncTester,failure,QString));

    QObject::connect(m_pLocalStorageManagerThread, QNSIGNAL(QThread,started),
                     m_pLocalStorageManagerThreadWorker, QNSLOT(LocalStorageManagerThreadWorker,init));
    QObject::connect(m_pLocalStorageManagerThread, QNSIGNAL(QThread,finished),
                     m_pLocalStorageManagerThread, QNSLOT(QThread,deleteLater));

    QObject::connect(m_pLocalStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,initialized),
                     this, QNSLOT(UserLocalStorageManagerAsyncTester,onWorkerInitialized));

    // Request --> slot connections
    QObject::connect(this, QNSIGNAL(UserLocalStorageManagerAsyncTester,getUserCountRequest,QUuid),
                     m_pLocalStorageManagerThreadWorker, QNSLOT(LocalStorageManagerThreadWorker,onGetUserCountRequest,QUuid));
    QObject::connect(this, QNSIGNAL(UserLocalStorageManagerAsyncTester,addUserRequest,UserWrapper,QUuid),
                     m_pLocalStorageManagerThreadWorker, QNSLOT(LocalStorageManagerThreadWorker,onAddUserRequest,UserWrapper,QUuid));
    QObject::connect(this, QNSIGNAL(UserLocalStorageManagerAsyncTester,updateUserRequest,UserWrapper,QUuid),
                     m_pLocalStorageManagerThreadWorker, QNSLOT(LocalStorageManagerThreadWorker,onUpdateUserRequest,UserWrapper,QUuid));
    QObject::connect(this, QNSIGNAL(UserLocalStorageManagerAsyncTester,findUserRequest,UserWrapper,QUuid),
                     m_pLocalStorageManagerThreadWorker, QNSLOT(LocalStorageManagerThreadWorker,onFindUserRequest,UserWrapper,QUuid));
    QObject::connect(this, QNSIGNAL(UserLocalStorageManagerAsyncTester,deleteUserRequest,UserWrapper,QUuid),
                     m_pLocalStorageManagerThreadWorker, QNSLOT(LocalStorageManagerThreadWorker,onDeleteUserRequest,UserWrapper,QUuid));
    QObject::connect(this, QNSIGNAL(UserLocalStorageManagerAsyncTester,expungeUserRequest,UserWrapper,QUuid),
                     m_pLocalStorageManagerThreadWorker, QNSLOT(LocalStorageManagerThreadWorker,onExpungeUserRequest,UserWrapper,QUuid));

    // Slot <-- result connections
    QObject::connect(m_pLocalStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,getUserCountComplete,int,QUuid),
                     this, QNSLOT(UserLocalStorageManagerAsyncTester,onGetUserCountCompleted,int,QUuid));
    QObject::connect(m_pLocalStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,getUserCountFailed,QString,QUuid),
                     this, QNSLOT(UserLocalStorageManagerAsyncTester,onGetUserCountFailed,QString,QUuid));
    QObject::connect(m_pLocalStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,addUserComplete,UserWrapper,QUuid),
                     this, QNSLOT(UserLocalStorageManagerAsyncTester,onAddUserCompleted,UserWrapper,QUuid));
    QObject::connect(m_pLocalStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,addUserFailed,UserWrapper,QString,QUuid),
                     this, QNSLOT(UserLocalStorageManagerAsyncTester,onAddUserFailed,UserWrapper,QString,QUuid));
    QObject::connect(m_pLocalStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,updateUserComplete,UserWrapper,QUuid),
                     this, QNSLOT(UserLocalStorageManagerAsyncTester,onUpdateUserCompleted,UserWrapper,QUuid));
    QObject::connect(m_pLocalStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,updateUserFailed,UserWrapper,QString,QUuid),
                     this, QNSLOT(UserLocalStorageManagerAsyncTester,onUpdateUserFailed,UserWrapper,QString,QUuid));
    QObject::connect(m_pLocalStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,findUserComplete,UserWrapper,QUuid),
                     this, QNSLOT(UserLocalStorageManagerAsyncTester,onFindUserCompleted,UserWrapper,QUuid));
    QObject::connect(m_pLocalStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,findUserFailed,UserWrapper,QString,QUuid),
                     this, QNSLOT(UserLocalStorageManagerAsyncTester,onFindUserFailed,UserWrapper,QString,QUuid));
    QObject::connect(m_pLocalStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,deleteUserComplete,UserWrapper,QUuid),
                     this, QNSLOT(UserLocalStorageManagerAsyncTester,onDeleteUserCompleted,UserWrapper,QUuid));
    QObject::connect(m_pLocalStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,deleteUserFailed,UserWrapper,QString,QUuid),
                     this, QNSLOT(UserLocalStorageManagerAsyncTester,onDeleteUserFailed,UserWrapper,QString,QUuid));
    QObject::connect(m_pLocalStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,expungeUserComplete,UserWrapper,QUuid),
                     this, QNSLOT(UserLocalStorageManagerAsyncTester,onExpungeUserCompleted,UserWrapper,QUuid));
    QObject::connect(m_pLocalStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,expungeUserFailed,UserWrapper,QString,QUuid),
                     this, QNSLOT(UserLocalStorageManagerAsyncTester,onExpungeUserFailed,UserWrapper,QString,QUuid));
}

#undef HANDLE_WRONG_STATE

} // namespace test
} // namespace quentier
