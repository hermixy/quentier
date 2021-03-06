#ifndef QUENTIER_MODELS_COLUMN_CHANGE_REROUTER_H
#define QUENTIER_MODELS_COLUMN_CHANGE_REROUTER_H

#include <quentier/utility/Macros.h>
#include <QAbstractItemModel>

/**
 * @brief The ColumnChangeRerouter catches the dataChanged signal from the model and emits its own dataChanged
 * signal with the same row and parent item but with different column
 */
class ColumnChangeRerouter: public QObject
{
    Q_OBJECT
public:
    explicit ColumnChangeRerouter(const int columnFrom, const int columnTo,
                                  QObject * parent = Q_NULLPTR);

    void setModel(QAbstractItemModel * model);

Q_SIGNALS:

// NOTE: don't attempt to use QT_VERSION_CHECK here: Qt4 doesn't expand macro functions during moc run which leads
// to the check not working and the code not building
    void dataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight
#if QT_VERSION < 0x050000
                            );
#else
                            , const QVector<int> & roles = QVector<int>());
#endif

private Q_SLOTS:
    void onModelDataChanged(const QModelIndex & topLeft, const QModelIndex & bottomRight
#if QT_VERSION < 0x050000
                            );
#else
                            , const QVector<int> & roles = QVector<int>());
#endif

private:
    int     m_columnFrom;
    int     m_columnTo;
};

#endif // QUENTIER_MODELS_COLUMN_CHANGE_REROUTER_H
