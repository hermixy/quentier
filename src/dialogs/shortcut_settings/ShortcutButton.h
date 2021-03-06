/*
 * Copyright 2017 Dmitry Ivanov
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

/**
 * The contents of this file were heavily inspired by the source code of Qt Creator,
 * a Qt and C/C++ IDE by The Qt Company Ltd., 2016. That source code is licensed under GNU GPL v.3
 * license with two permissive exceptions although they don't apply to this derived work.
 */

#ifndef QUENTIER_DIALOGS_SHORTCUT_SETTINGS_SHORTCUT_BUTTON_H
#define QUENTIER_DIALOGS_SHORTCUT_SETTINGS_SHORTCUT_BUTTON_H

#include <quentier/utility/Macros.h>
#include <QPushButton>
#include <QKeySequence>

namespace quentier {

class ShortcutButton: public QPushButton
{
    Q_OBJECT
public:
    ShortcutButton(QWidget * parent = Q_NULLPTR);

    virtual QSize sizeHint() const Q_DECL_OVERRIDE;

Q_SIGNALS:
    void keySequenceChanged(const QKeySequence & sequence);

protected:
    virtual bool eventFilter(QObject * pWatched, QEvent * pEvent) Q_DECL_OVERRIDE;

private:
    void updateText();

private Q_SLOTS:
    void handleToggleChange(bool toggleChange);

private:
    QString     m_uncheckedText;
    QString     m_checkedText;
    mutable int m_preferredWidth;
    int         m_key[4];
    int         m_keyNum;
};

} // namespace quentier

#endif // QUENTIER_DIALOGS_SHORTCUT_SETTINGS_SHORTCUT_BUTTON_H
