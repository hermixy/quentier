#include "CoreTester.h"
#include "SerializationTests.h"
#include "LocalStorageManagerTests.h"
#include <tools/IQuteNoteException.h>
#include <client/local_storage/LocalStorageManager.h>
#include <client/types/SavedSearch.h>
#include <client/types/LinkedNotebook.h>
#include <client/types/Tag.h>
#include <client/types/ResourceWrapper.h>
#include <client/types/Notebook.h>
#include <client/types/SharedNotebookWrapper.h>
#include <client/types/UserWrapper.h>
#include <client/Serialization.h>
#include <QtTest/QTest>

namespace qute_note {
namespace test {

CoreTester::CoreTester(QObject * parent) :
    QObject(parent)
{}

CoreTester::~CoreTester()
{}

#define TEST(component) \
    void CoreTester::serializationTest##component() \
    { \
        QString error; \
        bool res = Test##component##Serialization(error); \
        QVERIFY2(res == true, error.toStdString().c_str()); \
    }

TEST(BusinessUserInfo)
TEST(PremiumInfo)
TEST(Accounting)
TEST(UserAttributes)
TEST(NoteAttributes)
TEST(ResourceAttributes)

#undef TEST

void CoreTester::localStorageManagerIndividualSavedSearchTest()
{
    try
    {
        const bool startFromScratch = true;
        LocalStorageManager localStorageManager("CoreTesterFakeUser", 0, startFromScratch);

        SavedSearch search;
        search.setGuid("00000000-0000-0000-c000-000000000046");
        search.setUpdateSequenceNumber(1);
        search.setName("Fake saved search name");
        search.setQuery("Fake saved search query");
        search.setQueryFormat(1);
        search.setIncludeAccount(true);
        search.setIncludeBusinessLinkedNotebooks(false);
        search.setIncludePersonalLinkedNotebooks(true);

        QString error;
        bool res = TestSavedSearchAddFindUpdateExpungeInLocalStorage(search, localStorageManager, error);
        QVERIFY2(res == true, qPrintable(error));
    }
    catch(IQuteNoteException & exception) {
        QFAIL(qPrintable("Caught exception: " + exception.errorMessage()));
    }
}

void CoreTester::localStorageManagerIndividualLinkedNotebookTest()
{
    try
    {
        const bool startFromScratch = true;
        LocalStorageManager localStorageManager("CoreTesterFakeUser", 0, startFromScratch);

        LinkedNotebook linkedNotebook;
        linkedNotebook.setGuid("00000000-0000-0000-c000-000000000046");
        linkedNotebook.setUpdateSequenceNumber(1);
        linkedNotebook.setShareName("Fake linked notebook share name");
        linkedNotebook.setUsername("Fake linked notebook username");
        linkedNotebook.setShardId("Fake linked notebook shard id");
        linkedNotebook.setShareKey("Fake linked notebook share key");
        linkedNotebook.setUri("Fake linked notebook uri");
        linkedNotebook.setNoteStoreUrl("Fake linked notebook note store url");
        linkedNotebook.setWebApiUrlPrefix("Fake linked notebook web api url prefix");
        linkedNotebook.setStack("Fake linked notebook stack");
        linkedNotebook.setBusinessId(1);

        QString error;
        bool res = TestLinkedNotebookAddFindUpdateExpungeInLocalStorage(linkedNotebook, localStorageManager, error);
        QVERIFY2(res == true, error.toStdString().c_str());
    }
    catch(IQuteNoteException & exception) {
        QFAIL(QString("Caught exception: " + exception.errorMessage()).toStdString().c_str());
    }
}

void CoreTester::localStorageManagerIndividualTagTest()
{
    try
    {
        const bool startFromScratch = true;
        LocalStorageManager localStorageManager("CoreTesterFakeUser", 0, startFromScratch);

        Tag tag;
        tag.setGuid("00000000-0000-0000-c000-000000000046");
        tag.setUpdateSequenceNumber(1);
        tag.setName("Fake tag name");

        QString error;
        bool res = TestTagAddFindUpdateExpungeInLocalStorage(tag, localStorageManager, error);
        QVERIFY2(res == true, error.toStdString().c_str());
    }
    catch(IQuteNoteException & exception) {
        QFAIL(QString("Caught exception: " + exception.errorMessage()).toStdString().c_str());
    }
}

void CoreTester::localStorageManagerIndividualResourceTest()
{
    try
    {
        const bool startFromScratch = true;
        LocalStorageManager localStorageManager("CoreTesterFakeUser", 0, startFromScratch);

        Notebook notebook;
        notebook.setGuid("00000000-0000-0000-c000-000000000047");
        notebook.setUpdateSequenceNumber(1);
        notebook.setName("Fake notebook name");
        notebook.setCreationTimestamp(1);
        notebook.setModificationTimestamp(1);

        QString error;
        bool res = localStorageManager.AddNotebook(notebook, error);
        QVERIFY2(res == true, qPrintable(error));

        Note note;
        note.setGuid("00000000-0000-0000-c000-000000000046");
        note.setUpdateSequenceNumber(1);
        note.setTitle("Fake note title");
        note.setContent("Fake note content");
        note.setCreationTimestamp(1);
        note.setModificationTimestamp(1);
        note.setActive(true);
        note.setNotebookGuid(notebook.guid());

        error.clear();
        res = localStorageManager.AddNote(note, error);
        QVERIFY2(res == true, qPrintable(error));

        ResourceWrapper resource;
        resource.setGuid("00000000-0000-0000-c000-000000000046");
        resource.setUpdateSequenceNumber(1);
        resource.setNoteGuid(note.guid());
        resource.setDataBody("Fake resource data body");
        resource.setDataSize(resource.dataBody().size());
        resource.setDataHash("Fake hash      1");

        resource.setRecognitionDataBody("Fake resource recognition data body");
        resource.setRecognitionDataSize(resource.recognitionDataBody().size());
        resource.setRecognitionDataHash("Fake hash      2");

        resource.setMime("text/plain");
        resource.setWidth(1);
        resource.setHeight(1);

        evernote::edam::ResourceAttributes resourceAttributes;

        resourceAttributes.sourceURL = "Fake resource source URL";
        resourceAttributes.__isset.sourceURL = true;

        resourceAttributes.timestamp = 1;
        resourceAttributes.__isset.timestamp = true;

        resourceAttributes.latitude = 0.0;
        resourceAttributes.__isset.latitude = true;

        resourceAttributes.longitude = 0.0;
        resourceAttributes.__isset.longitude = true;

        resourceAttributes.altitude = 0.0;
        resourceAttributes.__isset.altitude = true;

        resourceAttributes.cameraMake = "Fake resource camera make";
        resourceAttributes.__isset.cameraMake = true;

        resourceAttributes.cameraModel = "Fake resource camera model";
        resourceAttributes.__isset.cameraModel = true;

        QByteArray serializedResourceAttributes = GetSerializedResourceAttributes(resourceAttributes);

        resource.setResourceAttributes(serializedResourceAttributes);

        error.clear();
        res = TestResourceAddFindUpdateExpungeInLocalStorage(resource, localStorageManager, error);
        QVERIFY2(res == true, error.toStdString().c_str());
    }
    catch(IQuteNoteException & exception) {
        QFAIL(QString("Caught exception: " + exception.errorMessage()).toStdString().c_str());
    }
}

void CoreTester::localStorageManagedIndividualNoteTest()
{
    try
    {
        const bool startFromScratch = true;
        LocalStorageManager localStorageManager("CoreTesterFakeUser", 0, startFromScratch);

        Notebook notebook;
        notebook.setGuid("00000000-0000-0000-c000-000000000047");
        notebook.setUpdateSequenceNumber(1);
        notebook.setName("Fake notebook name");
        notebook.setCreationTimestamp(1);
        notebook.setModificationTimestamp(1);

        QString error;
        bool res = localStorageManager.AddNotebook(notebook, error);
        QVERIFY2(res == true, qPrintable(error));

        Note note;
        note.setGuid("00000000-0000-0000-c000-000000000046");
        note.setUpdateSequenceNumber(1);
        note.setTitle("Fake note title");
        note.setContent("Fake note content");
        note.setCreationTimestamp(1);
        note.setModificationTimestamp(1);
        note.setActive(true);
        note.setNotebookGuid(notebook.guid());

        evernote::edam::NoteAttributes noteAttributes;
        noteAttributes.subjectDate = 1;
        noteAttributes.__isset.subjectDate = true;
        noteAttributes.latitude = 1.0;
        noteAttributes.__isset.latitude = true;
        noteAttributes.longitude = 1.0;
        noteAttributes.__isset.longitude = true;
        noteAttributes.altitude = 1.0;
        noteAttributes.__isset.altitude = true;
        noteAttributes.author = "author";
        noteAttributes.__isset.author = true;
        noteAttributes.source = "source";
        noteAttributes.__isset.source = true;
        noteAttributes.sourceURL = "source URL";
        noteAttributes.__isset.sourceURL = true;
        noteAttributes.sourceApplication = "source application";
        noteAttributes.__isset.sourceApplication = true;
        noteAttributes.shareDate = 2;
        noteAttributes.__isset.shareDate = true;

        QByteArray serializedNoteAttributes = GetSerializedNoteAttributes(noteAttributes);
        note.setNoteAttributes(serializedNoteAttributes);

        res = localStorageManager.AddNote(note, error);
        QVERIFY2(res == true, qPrintable(error));

        Tag tag;
        tag.setGuid("00000000-0000-0000-c000-000000000048");
        tag.setUpdateSequenceNumber(1);
        tag.setName("Fake tag name");

        res = localStorageManager.AddTag(tag, error);
        QVERIFY2(res == true, qPrintable(error));

        res = localStorageManager.LinkTagWithNote(tag, note, error);
        QVERIFY2(res == true, qPrintable(error));

        note.addTagGuid(tag.guid());

        ResourceWrapper resource;
        resource.setGuid("00000000-0000-0000-c000-000000000049");
        resource.setUpdateSequenceNumber(1);
        resource.setFreeAccount(true);
        resource.setNoteGuid(note.guid());
        resource.setDataBody("Fake resource data body");
        resource.setDataSize(resource.dataBody().size());
        resource.setDataHash("Fake hash      1");
        resource.setMime("text/plain");
        resource.setWidth(1);
        resource.setHeight(1);
        resource.setRecognitionDataBody("Fake resource recognition data body");
        resource.setRecognitionDataSize(resource.recognitionDataBody().size());
        resource.setRecognitionDataHash("Fake hash      2");

        note.addResource(resource);

        res = localStorageManager.UpdateNote(note, error);
        QVERIFY2(res == true, qPrintable(error));

        res = TestNoteFindUpdateDeleteExpungeInLocalStorage(note, localStorageManager, error);
        QVERIFY2(res == true, qPrintable(error));
    }
    catch(IQuteNoteException & exception) {
        QFAIL(qPrintable("Caught exception: " + exception.errorMessage()));
    }
}

void CoreTester::localStorageManagerIndividualNotebookTest()
{
    try
    {
        const bool startFromScratch = true;
        LocalStorageManager localStorageManager("CoreTesterFakeUser", 0, startFromScratch);

        Notebook notebook;
        notebook.setGuid("00000000-0000-0000-c000-000000000047");
        notebook.setUpdateSequenceNumber(1);
        notebook.setName("Fake notebook name");
        notebook.setCreationTimestamp(1);
        notebook.setModificationTimestamp(1);
        notebook.setDefaultNotebook(true);
        notebook.setPublishingUri("Fake publishing uri");
        notebook.setPublishingOrder(1);
        notebook.setPublishingAscending(true);
        notebook.setPublishingPublicDescription("Fake public description");
        notebook.setPublished(true);
        notebook.setStack("Fake notebook stack");
        notebook.setBusinessNotebookDescription("Fake business notebook description");
        notebook.setBusinessNotebookPrivilegeLevel(1);
        notebook.setBusinessNotebookRecommended(true);
        // NotebookRestrictions
        notebook.setCanReadNotes(true);
        notebook.setCanCreateNotes(true);
        notebook.setCanUpdateNotes(true);
        notebook.setCanExpungeNotes(false);
        notebook.setCanShareNotes(true);
        notebook.setCanEmailNotes(true);
        notebook.setCanSendMessageToRecipients(true);
        notebook.setCanUpdateNotebook(true);
        notebook.setCanExpungeNotebook(false);
        notebook.setCanSetDefaultNotebook(true);
        notebook.setCanSetNotebookStack(true);
        notebook.setCanPublishToPublic(true);
        notebook.setCanPublishToBusinessLibrary(false);
        notebook.setCanCreateTags(true);
        notebook.setCanUpdateTags(true);
        notebook.setCanExpungeTags(false);
        notebook.setCanSetParentTag(true);
        notebook.setCanCreateSharedNotebooks(true);
        notebook.setUpdateWhichSharedNotebookRestrictions(1);
        notebook.setExpungeWhichSharedNotebookRestrictions(1);

        QString error;
        bool res = localStorageManager.AddNotebook(notebook, error);
        QVERIFY2(res == true, qPrintable(error));

        SharedNotebookWrapper sharedNotebook;
        sharedNotebook.setId(1);
        sharedNotebook.setUserId(1);
        sharedNotebook.setNotebookGuid(notebook.guid());
        sharedNotebook.setEmail("Fake shared notebook email");
        sharedNotebook.setCreationTimestamp(1);
        sharedNotebook.setModificationTimestamp(1);
        sharedNotebook.setShareKey("Fake shared notebook share key");
        sharedNotebook.setUsername("Fake shared notebook username");
        sharedNotebook.setPrivilegeLevel(1);
        sharedNotebook.setAllowPreview(true);
        sharedNotebook.setReminderNotifyEmail(true);
        sharedNotebook.setReminderNotifyApp(false);

        notebook.addSharedNotebook(sharedNotebook);

        Note note;
        note.setGuid("00000000-0000-0000-c000-000000000049");
        note.setUpdateSequenceNumber(1);
        note.setTitle("Fake note title");
        note.setContent("Fake note content");
        note.setCreationTimestamp(1);
        note.setModificationTimestamp(1);
        note.setActive(true);
        note.setNotebookGuid(notebook.guid());

        res = localStorageManager.AddNote(note, error);
        QVERIFY2(res == true, qPrintable(error));

        Tag tag;
        tag.setGuid("00000000-0000-0000-c000-000000000048");
        tag.setUpdateSequenceNumber(1);
        tag.setName("Fake tag name");

        res = localStorageManager.AddTag(tag, error);
        QVERIFY2(res == true, qPrintable(error));

        res = localStorageManager.LinkTagWithNote(tag, note, error);
        QVERIFY2(res == true, qPrintable(error));

        note.addTagGuid(tag.guid());

        res = TestNotebookFindUpdateDeleteExpungeInLocalStorage(notebook, localStorageManager, error);
        QVERIFY2(res == true, qPrintable(error));
    }
    catch(IQuteNoteException & exception) {
        QFAIL(QString("Caught exception: " + exception.errorMessage()).toStdString().c_str());
    }
}

void CoreTester::localStorageManagedIndividualUserTest()
{
    try
    {
        const bool startFromScratch = true;
        LocalStorageManager localStorageManager("CoreTesterFakeUser", 0, startFromScratch);

        UserWrapper user;
        user.setId(1);
        user.setUsername("fake_user_username");
        user.setEmail("fake_user _mail");
        user.setName("fake_user_name");
        user.setTimezone("fake_user_timezone");
        user.setPrivilegeLevel(1);
        user.setCreationTimestamp(2);
        user.setModificationTimestamp(3);
        user.setActive(true);

        auto & userAttributes = user.userAttributes();
        userAttributes.defaultLocationName = "fake_default_location_name";
        userAttributes.__isset.defaultLocationName = true;
        userAttributes.defaultLatitude = 1.0;
        userAttributes.__isset.defaultLatitude = true;
        userAttributes.defaultLongitude = 2.0;
        userAttributes.__isset.defaultLongitude = true;
        userAttributes.preactivation = false;
        userAttributes.__isset.preactivation = true;
        std::vector<std::string> viewedPromotions;
        viewedPromotions.push_back("Viewed promotion 1");
        viewedPromotions.push_back("Viewed promotion 2");
        viewedPromotions.push_back("Viewed promotion 3");
        userAttributes.viewedPromotions = viewedPromotions;
        userAttributes.__isset.viewedPromotions = true;
        userAttributes.incomingEmailAddress = "fake_incoming_email_address";
        userAttributes.__isset.incomingEmailAddress = true;
        std::vector<std::string> recentEmailAddresses;
        recentEmailAddresses.push_back("recent_email_address_1");
        recentEmailAddresses.push_back("recent_email_address_2");
        userAttributes.recentMailedAddresses = recentEmailAddresses;
        userAttributes.__isset.recentMailedAddresses = true;
        userAttributes.comments = "Fake comments";
        userAttributes.__isset.comments = true;
        userAttributes.dateAgreedToTermsOfService = 1;
        userAttributes.__isset.dateAgreedToTermsOfService = true;
        userAttributes.maxReferrals = 3;
        userAttributes.__isset.maxReferrals = true;
        userAttributes.refererCode = "fake_referer_code";
        userAttributes.__isset.refererCode = true;
        userAttributes.sentEmailDate = 5;
        userAttributes.__isset.sentEmailDate = true;
        userAttributes.sentEmailCount = 4;
        userAttributes.__isset.sentEmailCount = true;
        userAttributes.dailyEmailLimit = 2;
        userAttributes.__isset.dailyEmailLimit = true;
        userAttributes.emailOptOutDate = 6;
        userAttributes.__isset.emailOptOutDate = true;
        userAttributes.partnerEmailOptInDate = 7;
        userAttributes.__isset.partnerEmailOptInDate = true;
        userAttributes.preferredLanguage = "ru";
        userAttributes.__isset.preferredLanguage = true;
        userAttributes.preferredCountry = "Russia";
        userAttributes.__isset.preferredCountry = true;
        userAttributes.clipFullPage = true;
        userAttributes.__isset.clipFullPage = true;
        userAttributes.twitterUserName = "fake_twitter_username";
        userAttributes.__isset.twitterUserName = true;
        userAttributes.twitterId = "fake_twitter_id";
        userAttributes.__isset.twitterId = true;
        userAttributes.groupName = "fake_group_name";
        userAttributes.__isset.groupName = true;
        userAttributes.recognitionLanguage = "ru";
        userAttributes.__isset.recognitionLanguage = true;
        userAttributes.referralProof = "I_have_no_idea_what_this_means";
        userAttributes.__isset.referralProof = true;
        userAttributes.educationalDiscount = false;
        userAttributes.__isset.educationalDiscount = true;
        userAttributes.businessAddress = "fake_business_address";
        userAttributes.__isset.businessAddress = true;
        userAttributes.hideSponsorBilling = true;
        userAttributes.__isset.hideSponsorBilling = true;
        userAttributes.taxExempt = true;
        userAttributes.__isset.taxExempt = true;
        userAttributes.useEmailAutoFiling = true;
        userAttributes.__isset.useEmailAutoFiling = true;
        userAttributes.reminderEmailConfig = evernote::edam::ReminderEmailConfig::DO_NOT_SEND;
        userAttributes.__isset.reminderEmailConfig = true;

        user.setHasAttributes(true);

        auto & businessUserInfo = user.businessUserInfo();
        businessUserInfo.businessId = 1;
        businessUserInfo.__isset.businessId = true;
        businessUserInfo.businessName = "Fake business name";
        businessUserInfo.__isset.businessName = true;
        businessUserInfo.role = evernote::edam::BusinessUserRole::NORMAL;
        businessUserInfo.__isset.role = true;
        businessUserInfo.email = "Fake business email";
        businessUserInfo.__isset.email = true;

        user.setHasBusinessUserInfo(true);

        auto & accounting = user.accounting();
        accounting.uploadLimit = 1000;
        accounting.__isset.uploadLimit = true;
        accounting.uploadLimitEnd = 9;
        accounting.__isset.uploadLimitEnd = true;
        accounting.uploadLimitNextMonth = 1200;
        accounting.__isset.uploadLimitNextMonth = true;
        accounting.premiumServiceStatus = evernote::edam::PremiumOrderStatus::PENDING;
        accounting.__isset.premiumServiceStatus = true;
        accounting.premiumOrderNumber = "Fake premium order number";
        accounting.__isset.premiumOrderNumber = true;
        accounting.premiumCommerceService = "Fake premium commerce service";
        accounting.__isset.premiumCommerceService = true;
        accounting.premiumServiceStart = 8;
        accounting.__isset.premiumServiceStart = true;
        accounting.premiumServiceSKU = "Fake code associated with the purchase";
        accounting.__isset.premiumServiceSKU = true;
        accounting.lastSuccessfulCharge = 7;
        accounting.__isset.lastSuccessfulCharge = true;
        accounting.lastFailedCharge = 5;
        accounting.__isset.lastFailedCharge = true;
        accounting.lastFailedChargeReason = "No money, no honey";
        accounting.__isset.lastFailedChargeReason = true;
        accounting.nextPaymentDue = 12;
        accounting.__isset.nextPaymentDue = true;
        accounting.premiumLockUntil = 11;
        accounting.__isset.premiumLockUntil = true;
        accounting.updated = 10;
        accounting.__isset.updated = true;
        accounting.premiumSubscriptionNumber = "Fake premium subscription number";
        accounting.__isset.premiumSubscriptionNumber = true;
        accounting.lastRequestedCharge = 9;
        accounting.__isset.lastRequestedCharge = true;
        accounting.currency = "USD";
        accounting.__isset.currency = true;
        accounting.unitPrice = 100;
        accounting.__isset.unitPrice = true;
        accounting.unitDiscount = 2;
        accounting.__isset.unitDiscount = true;
        accounting.nextChargeDate = 12;
        accounting.__isset.nextChargeDate = true;

        user.setHasAccounting(true);

        auto & premiumInfo = user.premiumInfo();
        premiumInfo.currentTime = 1;
        premiumInfo.premium = false;
        premiumInfo.premiumRecurring = false;
        premiumInfo.premiumExpirationDate = 11;
        premiumInfo.__isset.premiumExpirationDate = true;
        premiumInfo.premiumExtendable = true;
        premiumInfo.premiumPending = true;
        premiumInfo.premiumCancellationPending = false;
        premiumInfo.canPurchaseUploadAllowance = true;
        premiumInfo.sponsoredGroupName = "Fake sponsored group name";
        premiumInfo.__isset.sponsoredGroupName = true;
        premiumInfo.premiumUpgradable = true;
        premiumInfo.__isset.premiumUpgradable = true;

        user.setHasPremiumInfo(true);

        QString error;
        bool res = TestUserAddFindUpdateDeleteExpungeInLocalStorage(user, localStorageManager, error);
        QVERIFY2(res == true, qPrintable(error));
    }
    catch(IQuteNoteException & exception) {
        QFAIL(qPrintable("Caught exception: " + exception.errorMessage()));
    }
}

void CoreTester::localStorageManagerListAllSavedSearchesTest()
{
    try
    {
        const bool startFromScratch = true;
        LocalStorageManager localStorageManager("CoreTesterFakeUser", 0, startFromScratch);

        QString error;

        size_t nSearches = 5;
        std::vector<SavedSearch> searches;
        searches.reserve(nSearches);
        for(size_t i = 0; i < nSearches; ++i)
        {
            searches.push_back(SavedSearch());
            SavedSearch & search = searches.back();

            search.setGuid("00000000-0000-0000-c000-00000000000" + QString::number(i+1));
            search.setUpdateSequenceNumber(i);
            search.setName("SavedSearch #" + QString::number(i));
            search.setQuery("Fake saved search query #" + QString::number(i));
            search.setQueryFormat(1);
            search.setIncludeAccount(true);
            search.setIncludeBusinessLinkedNotebooks(true);
            search.setIncludePersonalLinkedNotebooks(true);

            bool res = localStorageManager.AddSavedSearch(search, error);
            QVERIFY2(res == true, qPrintable(error));
        }

        std::vector<SavedSearch> foundSearches;

        bool res = localStorageManager.ListAllSavedSearches(foundSearches, error);
        QVERIFY2(res == true, qPrintable(error));

        size_t numFoundSearches = foundSearches.size();
        if (numFoundSearches != nSearches) {
            QFAIL(qPrintable("Error: number of saved searches in the result of LocalStorageManager::ListAllSavedSearches (" +
                             QString::number(numFoundSearches) + ") does not match the original number of added saved searches (" +
                             QString::number(nSearches) + ")"));
        }

        for(size_t i = 0; i < numFoundSearches; ++i)
        {
            const SavedSearch & foundSearch = foundSearches.at(i);
            const auto it = std::find(searches.cbegin(), searches.cend(), foundSearch);
            if (it == searches.cend()) {
                QFAIL("One of saved searches from the result of LocalStorageManager::ListAllSavedSearches "
                      "was not found in the list of original searches");
            }
        }
    }
    catch(IQuteNoteException & exception) {
        QFAIL(qPrintable("Caught exception: " + exception.errorMessage()));
    }
}

void CoreTester::localStorageManagerListAllTagsTest()
{
    try
    {
        const bool startFromScratch = true;
        LocalStorageManager localStorageManager("CoreTesterFakeUser", 0, startFromScratch);

        QString error;

        size_t nTags = 5;
        std::vector<Tag> tags;
        tags.reserve(nTags);
        for(size_t i = 0; i < nTags; ++i)
        {
            tags.push_back(Tag());
            Tag & tag = tags.back();

            tag.setGuid("00000000-0000-0000-c000-00000000000" + QString::number(i+1));
            tag.setUpdateSequenceNumber(i);
            tag.setName("Tag name #" + QString::number(i));

            if (i != 0) {
                tag.setParentGuid(tags.at(i-1).guid());
            }

            bool res = localStorageManager.AddTag(tag, error);
            QVERIFY2(res == true, qPrintable(error));
        }

        std::vector<Tag> foundTags;

        bool res = localStorageManager.ListAllTags(foundTags, error);
        QVERIFY2(res == true, qPrintable(error));

        size_t numFoundTags = foundTags.size();
        if (numFoundTags != nTags) {
            QFAIL(qPrintable("Error: number of tags in the result of LocalStorageManager::ListAllTags (" +
                             QString::number(numFoundTags) + ") does not match the original number of added tags (" +
                             QString::number(nTags) + ")"));
        }

        for(size_t i = 0; i < numFoundTags; ++i)
        {
            const Tag & foundTag = foundTags.at(i);
            const auto it = std::find(tags.cbegin(), tags.cend(), foundTag);
            if (it == tags.cend()) {
                QFAIL("One of tags from the result of LocalStorageManager::ListAllTags "
                      "was not found in the list of original tags");
            }
        }
    }
    catch(IQuteNoteException & exception) {
        QFAIL(qPrintable("Caught exception: " + exception.errorMessage()));
    }
}

void CoreTester::localStorageManagerListAllLinkedNotebooksTest()
{
    try
    {
        const bool startFromScratch = true;
        LocalStorageManager localStorageManager("CoreTesterFakeUser", 0, startFromScratch);

        QString error;

        size_t nLinkedNotebooks = 5;
        std::vector<LinkedNotebook> linkedNotebooks;
        linkedNotebooks.reserve(nLinkedNotebooks);
        for(size_t i = 0; i < nLinkedNotebooks; ++i)
        {
            linkedNotebooks.push_back(LinkedNotebook());
            LinkedNotebook & linkedNotebook = linkedNotebooks.back();

            linkedNotebook.setGuid("00000000-0000-0000-c000-00000000000" + QString::number(i+1));
            linkedNotebook.setUpdateSequenceNumber(i);
            linkedNotebook.setShareName("Linked notebook share name #" + QString::number(i));
            linkedNotebook.setUsername("Linked notebook username #" + QString::number(i));
            linkedNotebook.setShardId("Linked notebook shard id #" + QString::number(i));
            linkedNotebook.setShareKey("Linked notebook share key #" + QString::number(i));
            linkedNotebook.setUri("Linked notebook uri #" + QString::number(i));
            linkedNotebook.setNoteStoreUrl("Linked notebook note store url #" + QString::number(i));
            linkedNotebook.setWebApiUrlPrefix("Linked notebook web api url prefix #" + QString::number(i));
            linkedNotebook.setStack("Linked notebook stack #" + QString::number(i));
            linkedNotebook.setBusinessId(1);

            bool res = localStorageManager.AddLinkedNotebook(linkedNotebook, error);
            QVERIFY2(res == true, qPrintable(error));
        }

        std::vector<LinkedNotebook> foundLinkedNotebooks;

        bool res = localStorageManager.ListAllLinkedNotebooks(foundLinkedNotebooks, error);
        QVERIFY2(res == true, qPrintable(error));

        size_t numFoundLinkedNotebooks = foundLinkedNotebooks.size();
        if (numFoundLinkedNotebooks != nLinkedNotebooks) {
            QFAIL(qPrintable("Error: number of linked notebooks in the result of LocalStorageManager::ListAllLinkedNotebooks (" +
                             QString::number(numFoundLinkedNotebooks) + ") does not match the original number of added linked notebooks (" +
                             QString::number(nLinkedNotebooks) + ")"));
        }

        for(size_t i = 0; i < numFoundLinkedNotebooks; ++i)
        {
            const LinkedNotebook & foundLinkedNotebook = foundLinkedNotebooks.at(i);
            const auto it = std::find(linkedNotebooks.cbegin(), linkedNotebooks.cend(), foundLinkedNotebook);
            if (it == linkedNotebooks.cend()) {
                QFAIL("One of linked notebooks from the result of LocalStorageManager::ListAllLinkedNotebooks "
                      "was not found in the list of original linked notebooks");
            }
        }
    }
    catch(IQuteNoteException & exception) {
        QFAIL(qPrintable("Caught exception: " + exception.errorMessage()));
    }
}

void CoreTester::localStorageManagerListAllSharedNotebooksTest()
{
    try
    {
        const bool startFromScratch = true;
        LocalStorageManager localStorageManager("CoreTesterFakeUser", 0, startFromScratch);

        Notebook notebook;
        notebook.setGuid("00000000-0000-0000-c000-000000000000");
        notebook.setUpdateSequenceNumber(1);
        notebook.setName("Fake notebook name");
        notebook.setCreationTimestamp(1);
        notebook.setModificationTimestamp(1);
        notebook.setDefaultNotebook(true);
        notebook.setPublished(false);
        notebook.setStack("Fake notebook stack");


        size_t numSharedNotebooks = 5;
        std::vector<SharedNotebookWrapper> sharedNotebooks;
        sharedNotebooks.reserve(numSharedNotebooks);
        for(size_t i = 0; i < numSharedNotebooks; ++i)
        {
            sharedNotebooks.push_back(SharedNotebookWrapper());
            SharedNotebookWrapper & sharedNotebook = sharedNotebooks.back();

            sharedNotebook.setId(i);
            sharedNotebook.setUserId(i);
            sharedNotebook.setNotebookGuid(notebook.guid());
            sharedNotebook.setEmail("Fake shared notebook email #" + QString::number(i));
            sharedNotebook.setCreationTimestamp(i+1);
            sharedNotebook.setModificationTimestamp(i+1);
            sharedNotebook.setShareKey("Fake shared notebook share key #" + QString::number(i));
            sharedNotebook.setUsername("Fake shared notebook username #" + QString::number(i));
            sharedNotebook.setPrivilegeLevel(1);
            sharedNotebook.setAllowPreview(true);
            sharedNotebook.setReminderNotifyEmail(true);
            sharedNotebook.setReminderNotifyApp(false);

            notebook.addSharedNotebook(sharedNotebook);
        }

        QString error;
        bool res = localStorageManager.AddNotebook(notebook, error);
        QVERIFY2(res == true, qPrintable(error));

        std::vector<SharedNotebookWrapper> foundSharedNotebooks;

        res = localStorageManager.ListAllSharedNotebooks(foundSharedNotebooks, error);
        QVERIFY2(res == true, qPrintable(error));

        size_t numFoundSharedNotebooks = foundSharedNotebooks.size();
        if (numFoundSharedNotebooks != numSharedNotebooks) {
            QFAIL(qPrintable("Error: number of shared notebooks in the result of LocalStorageManager::ListAllSharedNotebooks (" +
                             QString::number(numFoundSharedNotebooks) + ") does not match the original number of added shared notebooks (" +
                             QString::number(numSharedNotebooks) + ")"));
        }

        for(size_t i = 0; i < numFoundSharedNotebooks; ++i)
        {
            const SharedNotebookWrapper & foundSharedNotebook = foundSharedNotebooks.at(i);
            const auto it = std::find(sharedNotebooks.cbegin(), sharedNotebooks.cend(), foundSharedNotebook);
            if (it == sharedNotebooks.cend()) {
                QFAIL("One of shared notebooks from the result of LocalStorageManager::ListAllSharedNotebooks "
                      "was not found in the list of original shared notebooks");
            }
        }
    }
    catch(IQuteNoteException & exception) {
        QFAIL(qPrintable("Caught exception: " + exception.errorMessage()));
    }
}

void CoreTester::localStorageManagerListAllTagsPerNoteTest()
{
    try
    {
        const bool startFromScratch = true;
        LocalStorageManager localStorageManager("CoreTesterFakeUser", 0, startFromScratch);

        Notebook notebook;
        notebook.setGuid("00000000-0000-0000-c000-000000000047");
        notebook.setUpdateSequenceNumber(1);
        notebook.setName("Fake notebook name");
        notebook.setCreationTimestamp(1);
        notebook.setModificationTimestamp(1);

        QString error;
        bool res = localStorageManager.AddNotebook(notebook, error);
        QVERIFY2(res == true, qPrintable(error));

        Note note;
        note.setGuid("00000000-0000-0000-c000-000000000046");
        note.setUpdateSequenceNumber(1);
        note.setTitle("Fake note title");
        note.setContent("Fake note content");
        note.setCreationTimestamp(1);
        note.setModificationTimestamp(1);
        note.setActive(true);
        note.setNotebookGuid(notebook.guid());

        res = localStorageManager.AddNote(note, error);
        QVERIFY2(res == true, qPrintable(error));

        size_t numTags = 5;
        std::vector<Tag> tags;
        tags.reserve(numTags);
        for(size_t i = 0; i < numTags; ++i)
        {
            tags.push_back(Tag());
            Tag & tag = tags.back();

            tag.setGuid("00000000-0000-0000-c000-00000000000" + QString::number(i+1));
            tag.setUpdateSequenceNumber(i);
            tag.setName("Tag name #" + QString::number(i));

            res = localStorageManager.AddTag(tag, error);
            QVERIFY2(res == true, qPrintable(error));

            res = localStorageManager.LinkTagWithNote(tag, note, error);
            QVERIFY2(res == true, qPrintable(error));
        }

        Tag tagNotLinkedWithNote;
        tagNotLinkedWithNote.setGuid("00000000-0000-0000-c000-000000000045");
        tagNotLinkedWithNote.setUpdateSequenceNumber(9);
        tagNotLinkedWithNote.setName("Tag not linked with note");

        res = localStorageManager.AddTag(tagNotLinkedWithNote, error);
        QVERIFY2(res == true, qPrintable(error));

        std::vector<Tag> foundTags;

        res = localStorageManager.ListAllTagsPerNote(note.guid(), foundTags, error);
        QVERIFY2(res == true, qPrintable(error));

        size_t numFoundTags = foundTags.size();
        if (numFoundTags != numTags) {
            QFAIL(qPrintable("Error: number of tags in the result of LocalStorageManager::ListAllTagsPerNote (" +
                             QString::number(numFoundTags) + ") does not match the original number of added tags (" +
                             QString::number(numTags) + ")"));
        }

        for(size_t i = 0; i < numFoundTags; ++i)
        {
            const Tag & foundTag = foundTags.at(i);
            const auto it = std::find(tags.cbegin(), tags.cend(), foundTag);
            if (it == tags.cend()) {
                QFAIL("One of tags from the result of LocalStorageManager::ListAllTagsPerNote "
                      "was not found in the list of original tags");
            }
        }

        const auto it = std::find(foundTags.cbegin(), foundTags.cend(), tagNotLinkedWithNote);
        if (it != foundTags.cend()) {
            QFAIL("Found tag not linked with testing note in the result of LocalStorageManager::ListAllTagsPerNote");
        }
    }
    catch(IQuteNoteException & exception) {
        QFAIL(qPrintable("Caught exception: " + exception.errorMessage()));
    }
}

void CoreTester::localStorageManagerListAllNotesPerNotebookTest()
{
    try
    {
        const bool startFromScratch = true;
        LocalStorageManager localStorageManager("CoreTesterFakeUser", 0, startFromScratch);

        Notebook notebook;
        notebook.setGuid("00000000-0000-0000-c000-000000000047");
        notebook.setUpdateSequenceNumber(1);
        notebook.setName("Fake notebook name");
        notebook.setCreationTimestamp(1);
        notebook.setModificationTimestamp(1);

        QString error;
        bool res = localStorageManager.AddNotebook(notebook, error);
        QVERIFY2(res == true, qPrintable(error));

        Notebook notebookNotLinkedWithNotes;
        notebookNotLinkedWithNotes.setGuid("00000000-0000-0000-c000-000000000048");
        notebookNotLinkedWithNotes.setUpdateSequenceNumber(1);
        notebookNotLinkedWithNotes.setName("Fake notebook not linked with notes name name");
        notebookNotLinkedWithNotes.setCreationTimestamp(1);
        notebookNotLinkedWithNotes.setModificationTimestamp(1);

        res = localStorageManager.AddNotebook(notebookNotLinkedWithNotes, error);
        QVERIFY2(res == true, qPrintable(error));

        size_t numNotes = 5;
        std::vector<Note> notes;
        notes.reserve(numNotes);
        for(size_t i = 0; i < numNotes; ++i)
        {
            notes.push_back(Note());
            Note & note = notes.back();

            note.setGuid("00000000-0000-0000-c000-00000000000" + QString::number(i+1));
            note.setUpdateSequenceNumber(i+1);
            note.setTitle("Fake note title #" + QString::number(i));
            note.setContent("Fake note content #" + QString::number(i));
            note.setCreationTimestamp(i+1);
            note.setModificationTimestamp(i+1);
            note.setActive(true);
            note.setNotebookGuid(notebook.guid());

            res = localStorageManager.AddNote(note, error);
            QVERIFY2(res == true, qPrintable(error));
        }

        std::vector<Note> foundNotes;
        res = localStorageManager.ListAllNotesPerNotebook(notebook.guid(), foundNotes, error);
        QVERIFY2(res == true, qPrintable(error));

        size_t numFoundNotes = foundNotes.size();
        if (numFoundNotes != numNotes) {
            QFAIL(qPrintable("Error: number of notes in the result of LocalStorageManager::ListAllNotesPerNotebook (" +
                             QString::number(numFoundNotes) + ") does not match the original number of added notes (" +
                             QString::number(numNotes) + ")"));
        }

        for(size_t i = 0; i < numFoundNotes; ++i)
        {
            const Note & foundNote = foundNotes.at(i);
            const auto it = std::find(notes.cbegin(), notes.cend(), foundNote);
            if (it == notes.cend()) {
                QFAIL("One of notes from the result of LocalStorageManager::ListAllNotesPerNotebook "
                      "was not found in the list of original notes");
            }
        }

        foundNotes.clear();
        res = localStorageManager.ListAllNotesPerNotebook(notebookNotLinkedWithNotes.guid(),
                                                          foundNotes, error);
        QVERIFY2(res == true, qPrintable(error));

        if (foundNotes.size() != 0) {
            QFAIL(qPrintable("Found non-zero number of notes in the result of LocalStorageManager::ListAllNotesPerNotebook "
                             "called with guid of notebook not containing any notes (found " +
                             QString::number(foundNotes.size()) + " notes)"));
        }
    }
    catch(IQuteNoteException & exception) {
        QFAIL(qPrintable("Caught exception: " + exception.errorMessage()));
    }
}

void CoreTester::localStorageManagerListAllNotebooksTest()
{
    try
    {
        const bool startFromScratch = true;
        LocalStorageManager localStorageManager("CoreTesterFakeUser", 0, startFromScratch);

        QString error;

        size_t numNotebooks = 5;
        std::vector<Notebook> notebooks;
        for(size_t i = 0; i < numNotebooks; ++i)
        {
            notebooks.push_back(Notebook());
            Notebook & notebook = notebooks.back();

            notebook.setGuid("00000000-0000-0000-c000-00000000000" + QString::number(i+1));
            notebook.setUpdateSequenceNumber(i+1);
            notebook.setName("Fake notebook name #" + QString::number(i+1));
            notebook.setCreationTimestamp(i+1);
            notebook.setModificationTimestamp(i+1);

            notebook.setDefaultNotebook(true);
            notebook.setPublishingUri("Fake publishing uri #" + QString::number(i+1));
            notebook.setPublishingOrder(1);
            notebook.setPublishingAscending(true);
            notebook.setPublishingPublicDescription("Fake public description");
            notebook.setPublished(true);
            notebook.setStack("Fake notebook stack");
            notebook.setBusinessNotebookDescription("Fake business notebook description");
            notebook.setBusinessNotebookPrivilegeLevel(1);
            notebook.setBusinessNotebookRecommended(true);
            // NotebookRestrictions
            notebook.setCanReadNotes(true);
            notebook.setCanCreateNotes(true);
            notebook.setCanUpdateNotes(true);
            notebook.setCanExpungeNotes(false);
            notebook.setCanShareNotes(true);
            notebook.setCanEmailNotes(true);
            notebook.setCanSendMessageToRecipients(true);
            notebook.setCanUpdateNotebook(true);
            notebook.setCanExpungeNotebook(false);
            notebook.setCanSetDefaultNotebook(true);
            notebook.setCanSetNotebookStack(true);
            notebook.setCanPublishToPublic(true);
            notebook.setCanPublishToBusinessLibrary(false);
            notebook.setCanCreateTags(true);
            notebook.setCanUpdateTags(true);
            notebook.setCanExpungeTags(false);
            notebook.setCanSetParentTag(true);
            notebook.setCanCreateSharedNotebooks(true);
            notebook.setUpdateWhichSharedNotebookRestrictions(1);
            notebook.setExpungeWhichSharedNotebookRestrictions(1);

            SharedNotebookWrapper sharedNotebook;
            sharedNotebook.setId(i+1);
            sharedNotebook.setUserId(i+1);
            sharedNotebook.setNotebookGuid(notebook.guid());
            sharedNotebook.setEmail("Fake shared notebook email #" + QString::number(i+1));
            sharedNotebook.setCreationTimestamp(i+1);
            sharedNotebook.setModificationTimestamp(i+1);
            sharedNotebook.setShareKey("Fake shared notebook share key #" + QString::number(i+1));
            sharedNotebook.setUsername("Fake shared notebook username #" + QString::number(i+1));
            sharedNotebook.setPrivilegeLevel(1);
            sharedNotebook.setAllowPreview(true);
            sharedNotebook.setReminderNotifyEmail(true);
            sharedNotebook.setReminderNotifyApp(false);

            notebook.addSharedNotebook(sharedNotebook);

            bool res = localStorageManager.AddNotebook(notebook, error);
            QVERIFY2(res == true, qPrintable(error));
        }

        std::vector<Notebook> foundNotebooks;
        bool res = localStorageManager.ListAllNotebooks(foundNotebooks, error);
        QVERIFY2(res == true, qPrintable(error));

        size_t numFoundNotebooks = foundNotebooks.size();
        if (numFoundNotebooks != numNotebooks) {
            QFAIL(qPrintable("Error: number of notebooks in the result of LocalStorageManager::ListAllNotebooks (" +
                             QString::number(numFoundNotebooks) + ") does not match the original number of added notebooks (" +
                             QString::number(numNotebooks) + ")"));
        }

        for(size_t i = 0; i < numFoundNotebooks; ++i)
        {
            const Notebook & foundNotebook = foundNotebooks.at(i);
            const auto it = std::find(notebooks.cbegin(), notebooks.cend(), foundNotebook);
            if (it == notebooks.cend()) {
                QFAIL("One of notebooks from the result of LocalStorageManager::ListAllNotebooks "
                      "was not found in the list of original notebooks");
            }
        }
    }
    catch(IQuteNoteException & exception) {
        QFAIL(QString("Caught exception: " + exception.errorMessage()).toStdString().c_str());
    }
}

}
}
