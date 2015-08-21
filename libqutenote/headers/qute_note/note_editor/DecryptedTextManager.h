#ifndef __LIB_QUTE_NOTE__NOTE_EDITOR__DECRYPTED_TEXT_MANAGER_H
#define __LIB_QUTE_NOTE__NOTE_EDITOR__DECRYPTED_TEXT_MANAGER_H

#include <QtGlobal>

namespace qute_note {

QT_FORWARD_DECLARE_CLASS(DecryptedTextManagerPrivate)

class DecryptedTextManager
{
public:
    DecryptedTextManager();

    void addEntry(const QString & hash, const QString & decryptedText,
                  const bool rememberForSession, const QString & passphrase,
                  const QString & cipher, const size_t keyLength);

    bool findDecryptedText(const QString & passphrase, QString & decryptedText,
                           bool & rememberForSession) const;

    bool rehashDecryptedText(const QString & originalHash, const QString & newDecryptedText,
                             QString & newHash);

private:
    Q_DISABLE_COPY(DecryptedTextManager)

    DecryptedTextManagerPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(DecryptedTextManager)
};

} // namespace qute_note

#endif // __LIB_QUTE_NOTE__NOTE_EDITOR__DECRYPTED_TEXT_MANAGER_H
