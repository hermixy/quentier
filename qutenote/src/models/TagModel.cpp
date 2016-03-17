#include "TagModel.h"
#include <qute_note/logging/QuteNoteLogger.h>
#include <limits>

// Limit for the queries to the local storage
#define TAG_LIST_LIMIT (100)

#define NUM_TAG_MODEL_COLUMNS (3)

namespace qute_note {

TagModel::TagModel(LocalStorageManagerThreadWorker & localStorageManagerThreadWorker,
                   QObject * parent) :
    QAbstractItemModel(parent),
    m_data(),
    m_fakeRootItem(Q_NULLPTR),
    m_listTagsOffset(0),
    m_listTagsRequestId(),
    m_tagItemsNotYetInLocalStorageUids(),
    m_addTagRequestIds(),
    m_updateTagRequestIds(),
    m_expungeTagRequestIds(),
    m_findTagRequestIds()
{
    createConnections(localStorageManagerThreadWorker);
    requestTagsList();
}

TagModel::~TagModel()
{
    delete m_fakeRootItem;
}

Qt::ItemFlags TagModel::flags(const QModelIndex & index) const
{
    Qt::ItemFlags indexFlags = QAbstractItemModel::flags(index);
    if (!index.isValid()) {
        return indexFlags;
    }

    indexFlags |= Qt::ItemIsSelectable;
    indexFlags |= Qt::ItemIsEnabled;

    if (index.column() == Columns::Dirty) {
        return indexFlags;
    }

    if (index.column() == Columns::Synchronizable)
    {
        QModelIndex parentIndex = index;

        while(true) {
            TagModelItem * item = itemForIndex(parentIndex);
            if (!item) {
                break;
            }

            if (item == m_fakeRootItem) {
                break;
            }

            if (item->isSynchronizable()) {
                return indexFlags;
            }

            parentIndex = parentIndex.parent();
        }
    }

    indexFlags |= Qt::ItemIsEditable;

    return indexFlags;
}

QVariant TagModel::data(const QModelIndex & index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    int columnIndex = index.column();
    if ((columnIndex < 0) || (columnIndex >= NUM_TAG_MODEL_COLUMNS)) {
        return QVariant();
    }

    TagModelItem * item = itemForIndex(index);
    if (!item) {
        return QVariant();
    }

    if (item == m_fakeRootItem) {
        return QVariant();
    }

    Columns::type column;
    switch(columnIndex)
    {
    case Columns::Name:
        column = Columns::Name;
        break;
    case Columns::Synchronizable:
        column = Columns::Synchronizable;
        break;
    case Columns::Dirty:
        column = Columns::Dirty;
        break;
    default:
        return QVariant();
    }

    switch(role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
    case Qt::ToolTipRole:
        return dataText(*item, column);
    case Qt::AccessibleTextRole:
    case Qt::AccessibleDescriptionRole:
        return dataAccessibleText(*item, column);
    default:
        return QVariant();
    }
}

QVariant TagModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    switch(section)
    {
    case Columns::Name:
        return QVariant(QT_TR_NOOP("Name"));
    case Columns::Synchronizable:
        return QVariant(QT_TR_NOOP("Synchronizable"));
    case Columns::Dirty:
        return QVariant(QT_TR_NOOP("Dirty"));
    default:
        return QVariant();
    }
}

int TagModel::rowCount(const QModelIndex & parent) const
{
    if (parent.isValid() && (parent.column() != Columns::Name)) {
        return 0;
    }

    TagModelItem * parentItem = itemForIndex(parent);
    return (parentItem ? parentItem->numChildren() : 0);
}

int TagModel::columnCount(const QModelIndex & parent) const
{
    if (parent.isValid() && (parent.column() != Columns::Name)) {
        return 0;
    }

    return NUM_TAG_MODEL_COLUMNS;
}

QModelIndex TagModel::index(int row, int column, const QModelIndex & parent) const
{
    if (!m_fakeRootItem || (row < 0) || (column < 0) || (column >= NUM_TAG_MODEL_COLUMNS) ||
        (parent.isValid() && (parent.column() != Columns::Name)))
    {
        return QModelIndex();
    }

    TagModelItem * parentItem = itemForIndex(parent);
    if (!parentItem) {
        return QModelIndex();
    }

    TagModelItem * item = parentItem->childAtRow(row);
    if (!item) {
        return QModelIndex();
    }

    return createIndex(row, column, item);
}

QModelIndex TagModel::parent(const QModelIndex & index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }

    TagModelItem * childItem = itemForIndex(index);
    if (!childItem) {
        return QModelIndex();
    }

    TagModelItem * parentItem = childItem->parent();
    if (!parentItem) {
        return QModelIndex();
    }

    if (parentItem == m_fakeRootItem) {
        return QModelIndex();
    }

    TagModelItem * grandParentItem = parentItem->parent();
    if (!grandParentItem) {
        return QModelIndex();
    }

    int row = grandParentItem->rowForChild(parentItem);
    if (row < 0) {
        return QModelIndex();
    }

    return createIndex(row, Columns::Name, parentItem);
}

bool TagModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant & value, int role)
{
    Q_UNUSED(section)
    Q_UNUSED(orientation)
    Q_UNUSED(value)
    Q_UNUSED(role)
    return false;
}

bool TagModel::setData(const QModelIndex & modelIndex, const QVariant & value, int role)
{
    if (role != Qt::EditRole) {
        return false;
    }

    if (!modelIndex.isValid()) {
        return false;
    }

    if (modelIndex.column() == Columns::Dirty) {
        emit notifyError(QT_TR_NOOP("The \"dirty\" flag can't be set manually"));
        return false;
    }

    TagModelItem * item = itemForIndex(modelIndex);
    if (!item) {
        return false;
    }

    if (item == m_fakeRootItem) {
        return false;
    }

    TagModelItem itemCopy = *item;
    switch(modelIndex.column())
    {
    case Columns::Name:
        itemCopy.setName(value.toString());
        break;
    case Columns::Synchronizable:
        {
            if (itemCopy.isSynchronizable()) {
                emit notifyError(QT_TR_NOOP("Can't make already synchronizable tag not synchronizable"));
                return false;
            }

            bool dirty = itemCopy.isDirty();
            dirty |= (value.toBool() != itemCopy.isSynchronizable());
            itemCopy.setSynchronizable(value.toBool());
            break;
        }
    default:
        return false;
    }

    TagDataByLocalUid & index = m_data.get<ByLocalUid>();
    auto it = index.find(itemCopy.localUid());
    if (Q_UNLIKELY(it == index.end())) {
        QString error = QT_TR_NOOP("Internal error: can't find item in TagModel by its local uid");
        QNWARNING(error);
        emit notifyError(error);
        return false;
    }

    index.replace(it, itemCopy);

    emit dataChanged(modelIndex, modelIndex);

    Tag tag;
    tag.setLocalUid(itemCopy.localUid());
    tag.setName(itemCopy.name());
    tag.setLocal(!itemCopy.isSynchronizable());
    tag.setDirty(itemCopy.isDirty());

    QUuid requestId = QUuid::createUuid();

    auto notYetSavedItemIt = m_tagItemsNotYetInLocalStorageUids.find(itemCopy.localUid());
    if (notYetSavedItemIt != m_tagItemsNotYetInLocalStorageUids.end())
    {
        Q_UNUSED(m_addTagRequestIds.insert(requestId));
        emit addTag(tag, requestId);

        QNTRACE("Emitted the request to add the tag to local storage: id = " << requestId
                << ", tag: " << tag);

        Q_UNUSED(m_tagItemsNotYetInLocalStorageUids.erase(notYetSavedItemIt))
    }
    else
    {
        Q_UNUSED(m_updateTagRequestIds.insert(requestId));
        emit updateTag(tag, requestId);

        QNTRACE("Emitted the request to update the tag in the local storage: id = " << requestId
                << ", tag: " << tag);
    }

    return true;
}

bool TagModel::insertRows(int row, int count, const QModelIndex & parent)
{
    if (!m_fakeRootItem) {
        m_fakeRootItem = new TagModelItem;
    }

    TagModelItem * parentItem = (parent.isValid()
                                 ? itemForIndex(parent)
                                 : m_fakeRootItem);
    if (!parentItem) {
        return false;
    }

    TagDataByLocalUid & index = m_data.get<ByLocalUid>();

    beginInsertRows(parent, row, row + count - 1);
    for(int i = 0; i < count; ++i)
    {
        TagModelItem item;
        QString localUid = QUuid::createUuid().toString();
        localUid.chop(1);
        localUid.remove(0, 1);
        item.setLocalUid(localUid);
        Q_UNUSED(m_tagItemsNotYetInLocalStorageUids.insert(localUid))

        item.setName(tr("New tag"));
        item.setDirty(true);
        item.setParent(parentItem);

        Q_UNUSED(index.insert(item))
    }
    endInsertRows();

    return true;
}

bool TagModel::removeRows(int row, int count, const QModelIndex & parent)
{
    if (!m_fakeRootItem) {
        return false;
    }

    TagModelItem * parentItem = (parent.isValid()
                                 ? itemForIndex(parent)
                                 : m_fakeRootItem);
    if (!parentItem) {
        return false;
    }

    for(int i = 0; i < count; ++i)
    {
        TagModelItem * item = parentItem->childAtRow(row + i);
        if (!item) {
            continue;
        }

        if (item->isSynchronizable()) {
            QString error = QT_TR_NOOP("Can't remove synchronizable tag");
            QNINFO(error);
            emit notifyError(error);
            return false;
        }

        if (hasSynchronizableChildren(item)) {
            QString error = QT_TR_NOOP("Can't remove tag with synchronizable children");
            QNINFO(error);
            emit notifyError(error);
            return false;
        }
    }

    TagDataByLocalUid & index = m_data.get<ByLocalUid>();

    beginRemoveRows(parent, row, row + count - 1);
    for(int i = 0; i < count; ++i)
    {
        TagModelItem * item = parentItem->takeChild(row);
        if (!item) {
            continue;
        }

        Tag tag;
        tag.setLocalUid(item->localUid());

        QUuid requestId = QUuid::createUuid();
        Q_UNUSED(m_expungeTagRequestIds.insert(requestId))
        emit expungeTag(tag, requestId);
        QNTRACE("Emitted the request to expunge the tag from the local storage: request id = "
                << requestId << ", tag local uid: " << item->localUid());

        auto it = index.find(item->localUid());
        Q_UNUSED(index.erase(it))
    }
    endRemoveRows();

    return true;
}

void TagModel::onAddTagComplete(Tag tag, QUuid requestId)
{
    // TODO: implement
    Q_UNUSED(tag)
    Q_UNUSED(requestId)
}

void TagModel::onAddTagFailed(Tag tag, QString errorDescription, QUuid requestId)
{
    // TODO: implement
    Q_UNUSED(tag)
    Q_UNUSED(errorDescription)
    Q_UNUSED(requestId)
}

void TagModel::onUpdateTagComplete(Tag tag, QUuid requestId)
{
    // TODO: implement
    Q_UNUSED(tag)
    Q_UNUSED(requestId)
}

void TagModel::onUpdateTagFailed(Tag tag, QString errorDescription, QUuid requestId)
{
    // TODO: implement
    Q_UNUSED(tag)
    Q_UNUSED(errorDescription)
    Q_UNUSED(requestId)
}

void TagModel::onFindTagComplete(Tag tag, QUuid requestId)
{
    // TODO: implement
    Q_UNUSED(tag)
    Q_UNUSED(requestId)
}

void TagModel::onFindTagFailed(Tag tag, QString errorDescription, QUuid requestId)
{
    // TODO: implement
    Q_UNUSED(tag)
    Q_UNUSED(errorDescription)
    Q_UNUSED(requestId)
}

void TagModel::onListTagsComplete(LocalStorageManager::ListObjectsOptions flag,
                                  size_t limit, size_t offset,
                                  LocalStorageManager::ListTagsOrder::type order,
                                  LocalStorageManager::OrderDirection::type orderDirection,
                                  QString linkedNotebookGuid, QList<Tag> foundTags, QUuid requestId)
{
    // TODO: implement
    Q_UNUSED(flag)
    Q_UNUSED(limit)
    Q_UNUSED(offset)
    Q_UNUSED(order)
    Q_UNUSED(orderDirection)
    Q_UNUSED(linkedNotebookGuid)
    Q_UNUSED(foundTags)
    Q_UNUSED(requestId)
}

void TagModel::onListTagsFailed(LocalStorageManager::ListObjectsOptions flag,
                                size_t limit, size_t offset,
                                LocalStorageManager::ListTagsOrder::type order,
                                LocalStorageManager::OrderDirection::type orderDirection,
                                QString linkedNotebookGuid, QString errorDescription, QUuid requestId)
{
    // TODO: implement
    Q_UNUSED(flag)
    Q_UNUSED(limit)
    Q_UNUSED(offset)
    Q_UNUSED(order)
    Q_UNUSED(orderDirection)
    Q_UNUSED(linkedNotebookGuid)
    Q_UNUSED(errorDescription)
    Q_UNUSED(requestId)
}

void TagModel::onDeleteTagComplete(Tag tag, QUuid requestId)
{
    // TODO: implement
    Q_UNUSED(tag)
    Q_UNUSED(requestId)
}

void TagModel::onDeleteTagFailed(Tag tag, QString errorDescription, QUuid requestId)
{
    // TODO: implement
    Q_UNUSED(tag)
    Q_UNUSED(errorDescription)
    Q_UNUSED(requestId)
}

void TagModel::onExpungeTagComplete(Tag tag, QUuid requestId)
{
    // TODO: implement
    Q_UNUSED(tag)
    Q_UNUSED(requestId)
}

void TagModel::onExpungeTagFailed(Tag tag, QString errorDescription, QUuid requestId)
{
    // TODO: implement
    Q_UNUSED(tag)
    Q_UNUSED(errorDescription)
    Q_UNUSED(requestId)
}

void TagModel::createConnections(LocalStorageManagerThreadWorker & localStorageManagerThreadWorker)
{
    QNDEBUG("TagModel::createConnections");

    // Local signals to localStorageManagerThreadWorker's slots
    QObject::connect(this, QNSIGNAL(TagModel,addTag,Tag,QUuid),
                     &localStorageManagerThreadWorker, QNSLOT(LocalStorageManagerThreadWorker,onAddTagRequest,Tag,QUuid));
    QObject::connect(this, QNSIGNAL(TagModel,updateTag,Tag,QUuid),
                     &localStorageManagerThreadWorker, QNSLOT(LocalStorageManagerThreadWorker,onUpdateTagRequest,Tag,QUuid));
    QObject::connect(this, QNSIGNAL(TagModel,findTag,Tag,QUuid),
                     &localStorageManagerThreadWorker, QNSLOT(LocalStorageManagerThreadWorker,onFindTagRequest,Tag,QUuid));
    QObject::connect(this, QNSIGNAL(TagModel,listTags,LocalStorageManager::ListObjectsOptions,size_t,size_t,
                                    LocalStorageManager::ListTagsOrder::type,LocalStorageManager::OrderDirection::type,QString,QUuid),
                     &localStorageManagerThreadWorker, QNSLOT(LocalStorageManagerThreadWorker,onListTagsRequest,LocalStorageManager::ListObjectsOptions,
                                                              size_t,size_t,LocalStorageManager::ListTagsOrder::type,
                                                              LocalStorageManager::OrderDirection::type,QString,QUuid));
    QObject::connect(this, QNSIGNAL(TagModel,deleteTag,Tag,QUuid),
                     &localStorageManagerThreadWorker, QNSLOT(LocalStorageManagerThreadWorker,onDeleteTagRequest,Tag,QUuid));
    QObject::connect(this, QNSIGNAL(TagModel,expungeTag,Tag,QUuid),
                     &localStorageManagerThreadWorker, QNSLOT(LocalStorageManagerThreadWorker,onExpungeTagRequest,Tag,QUuid));

    // localStorageManagerThreadWorker's signals to local slots
    QObject::connect(&localStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,addTagComplete,Tag,QUuid),
                     this, QNSLOT(TagModel,onAddTagComplete,Tag,QUuid));
    QObject::connect(&localStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,addTagFailed,Tag,QString,QUuid),
                     this, QNSLOT(TagModel,onAddTagFailed,Tag,QString,QUuid));
    QObject::connect(&localStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,updateTagComplete,Tag,QUuid),
                     this, QNSLOT(TagModel,onUpdateTagComplete,Tag,QUuid));
    QObject::connect(&localStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,updateTagFailed,Tag,QString,QUuid),
                     this, QNSLOT(TagModel,onUpdateTagFailed,Tag,QString,QUuid));
    QObject::connect(&localStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,findTagComplete,Tag,QUuid),
                     this, QNSLOT(TagModel,onFindTagComplete,Tag,QUuid));
    QObject::connect(&localStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,findTagFailed,Tag,QString,QUuid),
                     this, QNSLOT(TagModel,onFindTagFailed,Tag,QString,QUuid));
    QObject::connect(&localStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,listTagsComplete,LocalStorageManager::ListObjectsOptions,
                                                                size_t,size_t,LocalStorageManager::ListTagsOrder::type,LocalStorageManager::OrderDirection::type,
                                                                QString,QList<Tag>,QUuid),
                     this, QNSLOT(TagModel,onListTagsComplete,LocalStorageManager::ListObjectsOptions,size_t,size_t,
                                  LocalStorageManager::ListTagsOrder::type,LocalStorageManager::OrderDirection::type,QString,QList<Tag>,QUuid));
    QObject::connect(&localStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,listTagsFailed,LocalStorageManager::ListObjectsOptions,
                                                                size_t,size_t,LocalStorageManager::ListTagsOrder::type,LocalStorageManager::OrderDirection::type,
                                                                QString,QUuid),
                     this, QNSLOT(TagModel,onListTagsFailed,LocalStorageManager::ListObjectsOptions,size_t,size_t,
                                  LocalStorageManager::ListTagsOrder::type,LocalStorageManager::OrderDirection::type,QString,QUuid));
    QObject::connect(&localStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,deleteTagComplete,Tag,QUuid),
                     this, QNSLOT(TagModel,onDeleteTagComplete,Tag,QUuid));
    QObject::connect(&localStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,deleteTagFailed,Tag,QString,QUuid),
                     this, QNSLOT(TagModel,onDeleteTagFailed,Tag,QString,QUuid));
    QObject::connect(&localStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,expungeTagComplete,Tag,QUuid),
                     this, QNSLOT(TagModel,onExpungeTagComplete,Tag,QUuid));
    QObject::connect(&localStorageManagerThreadWorker, QNSIGNAL(LocalStorageManagerThreadWorker,expungeTagFailed,Tag,QString,QUuid),
                     this, QNSLOT(TagModel,onExpungeTagFailed,Tag,QString,QUuid));
}

void TagModel::requestTagsList()
{
    // TODO: implement
}

void TagModel::onTagAddedOrUpdated(const Tag & tag, bool * pAdded)
{
    // TODO: implement
    Q_UNUSED(tag)
    Q_UNUSED(pAdded)
}

QVariant TagModel::dataText(const TagModelItem & item, const Columns::type column) const
{
    switch(column)
    {
    case Columns::Name:
        return QVariant(item.name());
    case Columns::Synchronizable:
        return QVariant(item.isSynchronizable());
    case Columns::Dirty:
        return QVariant(item.isDirty());
    default:
        return QVariant();
    }
}

QVariant TagModel::dataAccessibleText(const TagModelItem & item, const Columns::type column) const
{
    QVariant textData = dataText(item, column);
    if (textData.isNull()) {
        return QVariant();
    }

    QString accessibleText = QT_TR_NOOP("Tag: ");

    switch(column)
    {
    case Columns::Name:
        accessibleText += QT_TR_NOOP("name is ") + textData.toString();
        break;
    case Columns::Synchronizable:
        accessibleText += (textData.toBool() ? "synchronizable" : "not synchronizable");
        break;
    case Columns::Dirty:
        accessibleText += (textData.toBool() ? "dirty" : "not dirty");
        break;
    default:
        return QVariant();
    }

    return QVariant(accessibleText);
}

TagModelItem * TagModel::itemForIndex(const QModelIndex & index) const
{
    if (!index.isValid()) {
        return m_fakeRootItem;
    }

    TagModelItem * item = reinterpret_cast<TagModelItem*>(index.internalPointer());
    if (item) {
        return item;
    }

    return m_fakeRootItem;
}

bool TagModel::hasSynchronizableChildren(TagModelItem * item) const
{
    if (item->isSynchronizable()) {
        return true;
    }

    QList<TagModelItem*> children = item->children();
    for(auto it = children.begin(), end = children.end(); it != end; ++it) {
        if (hasSynchronizableChildren(*it)) {
            return true;
        }
    }

    return false;
}

} // namespace qute_note