#include "HTMLCleaner.h"
#include <logging/QuteNoteLogger.h>
#include <tidy.h>
#include <tidybuffio.h>
#include <stdio.h>
#include <errno.h>

namespace qute_note {

class HTMLCleaner::Impl
{
public:
    Impl() :
        m_tidyOutput(),
        m_tidyErrorBuffer(),
        m_tidyDoc(tidyCreate())
    {}

    ~Impl()
    {
        tidyBufFree(&m_tidyOutput);
        tidyBufFree(&m_tidyErrorBuffer);
        tidyRelease(m_tidyDoc);
    }

    bool convertHtml(const QString & html, const TidyOptionId outputFormat,
                     QString & output, QString & errorDescription);

    TidyBuffer  m_tidyOutput;
    TidyBuffer  m_tidyErrorBuffer;
    TidyDoc     m_tidyDoc;
};

HTMLCleaner::HTMLCleaner() :
    m_impl(new HTMLCleaner::Impl)
{}

HTMLCleaner::~HTMLCleaner()
{
    delete m_impl;
}

bool HTMLCleaner::htmlToXml(const QString & html, QString & output, QString & errorDescription)
{
    QNDEBUG("HTMLCleaner::htmlToXml");
    QNTRACE("html = " << html);

    return m_impl->convertHtml(html, TidyXmlOut, output, errorDescription);
}

bool HTMLCleaner::htmlToXhtml(const QString & html, QString & output, QString & errorDescription)
{
    QNDEBUG("HTMLCleaner::htmlToXhtml");
    QNTRACE("html = " << html);

    return m_impl->convertHtml(html, TidyXhtmlOut, output, errorDescription);
}

bool HTMLCleaner::cleanupHtml(QString & html, QString & errorDescription)
{
    QNDEBUG("HTMLCleaner::cleanupHtml");
    QNTRACE("html = " << html);

    return m_impl->convertHtml(html, TidyHtmlOut, html, errorDescription);
}

bool HTMLCleaner::Impl::convertHtml(const QString & html, const TidyOptionId outputFormat, QString & output, QString & errorDescription)
{
    int rc = -1;
    Bool ok = tidyOptSetBool(m_tidyDoc, outputFormat, yes);
    QNTRACE("tidyOptSetBool: output format: ok = " << (ok ? "true" : "false"));
    if (ok) {
        ok = tidyOptSetBool(m_tidyDoc, TidyPreserveEntities, yes);
        QNTRACE("tidyOptSetBool: preserve entities = yes: ok = " << (ok ? "true" : "false"));
    }

    if (ok) {
        ok = tidyOptSetInt(m_tidyDoc, TidyMergeDivs, no);
        QNTRACE("tidyOptSetInt: merge divs = no: ok = " << (ok ? "true" : "false"));
    }

    if (ok) {
        ok = tidyOptSetInt(m_tidyDoc, TidyMergeSpans, no);
        QNTRACE("tidyOptSetInt: merge spans = no: ok = " << (ok ? "true" : "false"));
    }

    if (ok) {
        ok = tidyOptSetBool(m_tidyDoc, TidyMergeEmphasis, no);
        QNTRACE("tidyOptSetBool: merge emphasis = no: ok = " << (ok ? "true" : "false"));
    }

    if (ok) {
        ok = tidyOptSetBool(m_tidyDoc, TidyDropEmptyElems, no);
        QNTRACE("tidyOptSetBool: drop empty elemens = no: ok = " << (ok ? "true" : "false"));
    }

    if (ok) {
        rc = tidySetErrorBuffer(m_tidyDoc, &m_tidyErrorBuffer);
        QNTRACE("tidySetErrorBuffer: rc = " << rc);
    }

    if (rc >= 0) {
        rc = tidyParseString(m_tidyDoc, html.toLocal8Bit().constData());
        QNTRACE("tidyParseString: rc = " << rc);
    }

    if (rc >= 0) {
        rc = tidyCleanAndRepair(m_tidyDoc);
        QNTRACE("tidyCleanAndRepair: rc = " << rc);
    }

    if (rc >= 0) {
        rc = tidyRunDiagnostics(m_tidyDoc);
        QNTRACE("tidyRunDiagnostics: rc = " << rc);
    }

    if (rc > 1) {
        int forceOutputRc = tidyOptSetBool(m_tidyDoc, TidyForceOutput, yes);
        QNTRACE("tidyOptSetBool (force output): rc = " << forceOutputRc);
        if (!forceOutputRc) {
            rc = -1;
        }
    }

    if (rc >= 0) {
        rc = tidySaveBuffer(m_tidyDoc, &m_tidyOutput);
        QNTRACE("tidySaveBuffer: rc = " << rc);
    }

    if (rc >= 0)
    {
        if (rc > 0) {
            QNINFO("Tidy diagnostics: " << QByteArray(reinterpret_cast<const char*>(m_tidyErrorBuffer.bp),
                                                      static_cast<int>(m_tidyErrorBuffer.size)));
        }

        output.resize(0);
        output.append(QByteArray(reinterpret_cast<const char*>(m_tidyOutput.bp),
                                 static_cast<int>(m_tidyOutput.size)));
        return true;
    }

    errorDescription = QT_TR_NOOP("tidy-html5 error: ");
    errorDescription.append(QByteArray(reinterpret_cast<const char*>(m_tidyErrorBuffer.bp),
                                       static_cast<int>(m_tidyErrorBuffer.size)));
    QNINFO(errorDescription);
    return false;
}

} // namespace qute_note