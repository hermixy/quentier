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

#include "SetupStartAtLogin.h"
#include "../SettingsNames.h"
#include "../DefaultSettings.h"
#include "../utility/StartAtLogin.h"
#include <quentier/utility/ApplicationSettings.h>
#include <quentier/logging/QuentierLogger.h>
#include <quentier/types/ErrorString.h>

namespace quentier {

void setupStartQuentierAtLogin()
{
    QNDEBUG(QStringLiteral("setupStartQuentierAtLogin"));

    ApplicationSettings appSettings;
    appSettings.beginGroup(START_AUTOMATICALLY_AT_LOGIN_SETTINGS_GROUP_NAME);
    if (appSettings.contains(SHOULD_START_AUTOMATICALLY_AT_LOGIN)) {
        QNDEBUG(QStringLiteral("Start automatically at login setting is present within settings, nothing to do"));
        appSettings.endGroup();
        return;
    }

    QNDEBUG(QStringLiteral("Start automatically at login setting is not present, will set it to the default value"));

    bool shouldStartAutomaticallyAtLogin = DEFAULT_SHOULD_START_AUTOMATICALLY_AT_LOGIN_OPTION;
    if (!shouldStartAutomaticallyAtLogin) {
        QNDEBUG(QStringLiteral("Should not start automatically at login by default"));
        appSettings.endGroup();
        return;
    }

    StartQuentierAtLoginOption::type option = DEFAULT_START_AUTOMATICALLY_AT_LOGIN_OPTION;

    ErrorString errorDescription;
    bool res = setStartQuentierAtLoginOption(shouldStartAutomaticallyAtLogin, errorDescription, option);
    if (Q_UNLIKELY(!res)) {
        QNWARNING(QStringLiteral("Failed to set Quentier to start automatically at login: ") << errorDescription);
        appSettings.endGroup();
        return;
    }

    appSettings.setValue(SHOULD_START_AUTOMATICALLY_AT_LOGIN, shouldStartAutomaticallyAtLogin);
    appSettings.setValue(START_AUTOMATICALLY_AT_LOGIN_OPTION, option);
    appSettings.endGroup();
}

} // namespace quentier
