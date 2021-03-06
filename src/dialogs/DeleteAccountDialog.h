/*
 * Copyright 2018 Dmitry Ivanov
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

#ifndef QUENTIER_DIALOGS_DELETE_ACOUNT_DIALOG_H
#define QUENTIER_DIALOGS_DELETE_ACOUNT_DIALOG_H

#include <quentier/utility/Macros.h>
#include <quentier/types/Account.h>
#include <QDialog>
#include <QPointer>

namespace Ui {
class DeleteAccountDialog;
}

namespace quentier {

QT_FORWARD_DECLARE_CLASS(AccountModel)

class DeleteAccountDialog : public QDialog
{
    Q_OBJECT
public:
    explicit DeleteAccountDialog(const Account & account, AccountModel & model,
                                 QWidget * parent = Q_NULLPTR);
    virtual ~DeleteAccountDialog();

private Q_SLOTS:
    void onConfirmationLineEditTextEdited(const QString & text);

private:
    virtual void accept() Q_DECL_OVERRIDE;

private:
    void setStatusBarText(const QString & text);

private:
    Ui::DeleteAccountDialog *   m_pUi;
    Account         m_account;
    AccountModel &  m_model;
};

} // namespace quentier

#endif // QUENTIER_DIALOGS_DELETE_ACOUNT_DIALOG_H
