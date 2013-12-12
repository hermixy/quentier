#include "ResourceMetadata.h"
#include "Note.h"
#include "../tools/EmptyDataElementException.h"

namespace qute_note {

class ResourceMetadataPrivate
{
public:
    ResourceMetadataPrivate(const Note & note, const QString & resourceMimeType,
                            const QString & dataHash, const std::size_t dataSize,
                            const std::size_t width, const std::size_t height);
    ResourceMetadataPrivate(const ResourceMetadataPrivate & other);
    ResourceMetadataPrivate & operator=(const ResourceMetadataPrivate & other);

    Guid         m_noteGuid;
    QString      m_resourceMimeType;
    QString      m_resourceDataHash;
    std::size_t  m_resourceDataSize;
    std::size_t  m_resourceDisplayWidth;
    std::size_t  m_resourceDisplayHeight;

private:
    ResourceMetadataPrivate() = delete;
};

ResourceMetadata::ResourceMetadata(const Note & note, const QString & resourceMimeType,
                                   const QString & dataHash, const std::size_t dataSize,
                                   const std::size_t width, const std::size_t height) :
    d_ptr(new ResourceMetadataPrivate(note, resourceMimeType, dataHash, dataSize,
                                      width, height))
{}

ResourceMetadata::ResourceMetadata(const ResourceMetadata & other) :
    d_ptr((other.d_func() == nullptr) ? nullptr : new ResourceMetadataPrivate(*(other.d_func())))
{}

ResourceMetadata::~ResourceMetadata()
{}

ResourceMetadataPrivate::ResourceMetadataPrivate(const Note & note,
                                                 const QString & resourceMimeType,
                                                 const QString &dataHash,
                                                 const std::size_t dataSize,
                                                 const std::size_t width,
                                                 const std::size_t height) :
    m_noteGuid(note.guid()),
    m_resourceMimeType(resourceMimeType),
    m_resourceDataHash(dataHash),
    m_resourceDataSize(dataSize),
    m_resourceDisplayWidth(width),
    m_resourceDisplayHeight(height)
{
    if (note.isEmpty()) {
        throw EmptyDataElementException("Attempt to create ResourceMetadata object for empty Note");
    }
}

ResourceMetadataPrivate::ResourceMetadataPrivate(const ResourceMetadataPrivate & other) :
    m_noteGuid(other.m_noteGuid),
    m_resourceMimeType(other.m_resourceMimeType),
    m_resourceDataHash(other.m_resourceDataHash),
    m_resourceDataSize(other.m_resourceDataSize),
    m_resourceDisplayWidth(other.m_resourceDisplayWidth),
    m_resourceDisplayHeight(other.m_resourceDisplayHeight)
{}

}
