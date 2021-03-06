/*
 * Copyright 2016 Dmitry Ivanov
 *
 * This file is part of Quentier.
 *
 * Quentier is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * Quentier is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Quentier. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef QUENTIER_TESTS_MODEL_TEST_NOTEBOOK_MODEL_TEST_HELPER_H
#define QUENTIER_TESTS_MODEL_TEST_NOTEBOOK_MODEL_TEST_HELPER_H

#include <quentier/local_storage/LocalStorageManagerAsync.h>

namespace quentier {

QT_FORWARD_DECLARE_CLASS(NotebookModel)
QT_FORWARD_DECLARE_CLASS(NotebookModelItem)

class NotebookModelTestHelper: public QObject
{
    Q_OBJECT
public:
    explicit NotebookModelTestHelper(LocalStorageManagerAsync * pLocalStorageManagerAsync,
                                     QObject * parent = Q_NULLPTR);

Q_SIGNALS:
    void failure(ErrorString errorDescription);
    void success();

public Q_SLOTS:
    void test();

private Q_SLOTS:
    void onAddNotebookFailed(Notebook notebook, ErrorString errorDescription, QUuid requestId);
    void onUpdateNotebookFailed(Notebook notebook, ErrorString errorDescription, QUuid requestId);
    void onFindNotebookFailed(Notebook notebook, ErrorString errorDescription, QUuid requestId);
    void onListNotebooksFailed(LocalStorageManager::ListObjectsOptions flag, size_t limit, size_t offset,
                               LocalStorageManager::ListNotebooksOrder::type order,
                               LocalStorageManager::OrderDirection::type orderDirection,
                               QString linkedNotebookGuid, ErrorString errorDescription, QUuid requestId);
    void onExpungeNotebookFailed(Notebook notebook, ErrorString errorDescription, QUuid requestId);

private:
    bool checkSorting(const NotebookModel & model, const NotebookModelItem * item) const;
    void notifyFailureWithStackTrace(ErrorString errorDescription);

    struct LessByName
    {
        bool operator()(const NotebookModelItem * lhs, const NotebookModelItem * rhs) const;
    };

    struct GreaterByName
    {
        bool operator()(const NotebookModelItem * lhs, const NotebookModelItem * rhs) const;
    };

private:
    LocalStorageManagerAsync *   m_pLocalStorageManagerAsync;
};

} // namespace quentier

#endif // QUENTIER_TESTS_MODEL_TEST_NOTEBOOK_MODEL_TEST_HELPER_H
