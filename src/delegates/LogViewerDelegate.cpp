#include "LogViewerDelegate.h"
#include "../models/LogViewerModel.h"
#include "../models/LogViewerFilterModel.h"
#include <QFontMetrics>
#include <QPainter>
#include <cmath>

namespace quentier {

LogViewerDelegate::LogViewerDelegate(QObject * parent) :
    AbstractStyledItemDelegate(parent),
    m_margin(0.1),
    m_widestLogLevelName(QStringLiteral("Warning")),
    m_sampleDateTimeString(QStringLiteral("26/09/2017 19:31:23:457")),
    m_sampleSourceFileLineNumberString(QStringLiteral("99999"))
{}

QWidget * LogViewerDelegate::createEditor(QWidget * pParent,
                                          const QStyleOptionViewItem & option,
                                          const QModelIndex & index) const
{
    Q_UNUSED(pParent)
    Q_UNUSED(option)
    Q_UNUSED(index)
    return Q_NULLPTR;
}

void LogViewerDelegate::paint(QPainter * pPainter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    if (paintImpl(pPainter, option, index)) {
        return;
    }

    AbstractStyledItemDelegate::paint(pPainter, option, index);
}

QSize LogViewerDelegate::sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    // NOTE: this method is called many times when QHeaderView::resizeSections(QHeaderView::ResizeToContents)
    // it has to be very fast, otherwise the performance is complete crap
    // so there are some shortcuts and missing checks which should normally be here

    switch(index.column())
    {
    case LogViewerModel::Columns::Timestamp:
        return stringSizeHint(option, index, m_sampleDateTimeString);
    case LogViewerModel::Columns::SourceFileLineNumber:
        return stringSizeHint(option, index, m_sampleSourceFileLineNumberString);
    case LogViewerModel::Columns::LogLevel:
        return stringSizeHint(option, index, m_widestLogLevelName);
    }

    // If we haven't returned yet, either the index is invalid or we are dealing
    // with either log entry column or source file name column

    if (Q_UNLIKELY(!index.isValid())) {
        return AbstractStyledItemDelegate::sizeHint(option, index);
    }

    const LogViewerFilterModel * pFilterModel = qobject_cast<const LogViewerFilterModel*>(index.model());
    if (Q_UNLIKELY(!pFilterModel)) {
        return AbstractStyledItemDelegate::sizeHint(option, index);
    }

    const LogViewerModel * pModel = qobject_cast<const LogViewerModel*>(pFilterModel->sourceModel());
    if (Q_UNLIKELY(!pModel)) {
        return AbstractStyledItemDelegate::sizeHint(option, index);
    }

    QModelIndex sourceIndex = pFilterModel->mapToSource(index);
    if (Q_UNLIKELY(!sourceIndex.isValid())) {
        return AbstractStyledItemDelegate::sizeHint(option, index);
    }

    int row = sourceIndex.row();
    const LogViewerModel::Data * pDataEntry = pModel->dataEntry(row);
    if (Q_UNLIKELY(!pDataEntry)) {
        return AbstractStyledItemDelegate::sizeHint(option, index);
    }

    if (index.column() == LogViewerModel::Columns::SourceFileName) {
        return stringSizeHint(option, index, pDataEntry->m_sourceFileName);
    }

    QFontMetrics fontMetrics(option.font);

    QSize size;
    size.setWidth(static_cast<int>(std::floor(fontMetrics.width(QStringLiteral("w")) *
                                              (pDataEntry->m_logEntryMaxNumCharsPerLine + 2 + m_margin) + 0.5)));
    size.setHeight(static_cast<int>(std::floor(fontMetrics.height() *
                                               (pDataEntry->m_numLogEntryLines + 1 + m_margin) + 0.5)));
    return size;
}

QSize LogViewerDelegate::stringSizeHint(const QStyleOptionViewItem & option, const QModelIndex & index, const QString & content) const
{
    QSize result;

    QFontMetrics fontMetrics(option.font);
    int contentWidth = static_cast<int>(std::floor(fontMetrics.width(content) * (1.0 + m_margin) + 0.5));
    int headerWidth = columnNameWidth(option, index);
    result.setWidth(std::max(contentWidth, headerWidth));
    result.setHeight(static_cast<int>(std::floor(fontMetrics.height() * (1.0 + m_margin) + 0.5)));

    return result;
}

bool LogViewerDelegate::paintImpl(QPainter * pPainter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    if (Q_UNLIKELY(!pPainter)) {
        return false;
    }

    if (Q_UNLIKELY(!index.isValid())) {
        return false;
    }

    const LogViewerFilterModel * pFilterModel = qobject_cast<const LogViewerFilterModel*>(index.model());
    if (Q_UNLIKELY(!pFilterModel)) {
        return false;
    }

    const LogViewerModel * pModel = qobject_cast<const LogViewerModel*>(pFilterModel->sourceModel());
    if (Q_UNLIKELY(!pModel)) {
        return false;
    }

    QModelIndex sourceIndex = pFilterModel->mapToSource(index);
    if (Q_UNLIKELY(!sourceIndex.isValid())) {
        return false;
    }

    int row = sourceIndex.row();
    const LogViewerModel::Data * pDataEntry = pModel->dataEntry(row);
    if (Q_UNLIKELY(!pDataEntry)) {
        return false;
    }

    pPainter->save();
    pPainter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    if (option.state & QStyle::State_Selected) {
        pPainter->fillRect(option.rect, option.palette.highlight());
        pPainter->setPen(option.palette.highlightedText().color());
    }
    else {
        // TODO: fill background with color specific to the log level
        pPainter->setPen(option.palette.windowText().color());
    }

    QTextOption textOption(Qt::Alignment(Qt::AlignLeft | Qt::AlignTop));
    textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);

    QRect adjustedRect = option.rect.adjusted(2, 2, -2, -2);

    int column = sourceIndex.column();
    switch(column)
    {
    case LogViewerModel::Columns::Timestamp:
        {
            const QDateTime & timestamp = pDataEntry->m_timestamp;
            QDate date = timestamp.date();
            QTime time = timestamp.time();
            QString printedTimestamp = date.toString(Qt::DefaultLocaleShortDate);
            printedTimestamp += QStringLiteral(" ");
            printedTimestamp += time.toString(QStringLiteral("HH:mm:ss:zzz"));
            pPainter->drawText(adjustedRect, printedTimestamp, textOption);
        }
        break;
    case LogViewerModel::Columns::SourceFileName:
        {
            QTextOption textOption(Qt::Alignment(Qt::AlignLeft | Qt::AlignTop));
            textOption.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
            pPainter->drawText(adjustedRect, pDataEntry->m_sourceFileName, textOption);
        }
        break;
    case LogViewerModel::Columns::SourceFileLineNumber:
        pPainter->drawText(adjustedRect, QString::number(pDataEntry->m_sourceFileLineNumber),
                           textOption);
        break;
    case LogViewerModel::Columns::LogLevel:
        pPainter->drawText(adjustedRect, LogViewerModel::logLevelToString(pDataEntry->m_logLevel),
                           textOption);
        break;
    case LogViewerModel::Columns::LogEntry:
        {
            pPainter->drawText(adjustedRect, pDataEntry->m_logEntry, textOption);
        }
        break;
    default:
        break;
    }

    pPainter->restore();
    return true;
}

} // namespace quentier