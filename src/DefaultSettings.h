#ifndef QUENTIER_DEFAULT_SETTINGS_H
#define QUENTIER_DEFAULT_SETTINGS_H

#include "SystemTrayIconManager.h"
#include <QtGlobal>

#define DEFAULT_SHOW_SYSTEM_TRAY_ICON (true)
#define DEFAULT_CLOSE_TO_SYSTEM_TRAY (true)
#define DEFAULT_MINIMIZE_TO_SYSTEM_TRAY (false)

#define DEFAULT_START_MINIMIZED_TO_SYSTEM_TRAY (false)

#define DEFAULT_SHOW_NOTE_THUMBNAILS (true)

#ifdef Q_WS_MAC
#define DEFAULT_TRAY_ICON_KIND QStringLiteral("dark")
#else
#define DEFAULT_TRAY_ICON_KIND QStringLiteral("colored")
#endif

#ifdef Q_WS_MAC
#define DEFAULT_SINGLE_CLICK_TRAY_ACTION (SystemTrayIconManager::TrayActionDoNothing)
#else
#define DEFAULT_SINGLE_CLICK_TRAY_ACTION (SystemTrayIconManager::TrayActionShowContextMenu)
#endif

#define DEFAULT_MIDDLE_CLICK_TRAY_ACTION (SystemTrayIconManager::TrayActionShowHide)
#define DEFAULT_DOUBLE_CLICK_TRAY_ACTION (SystemTrayIconManager::TrayActionDoNothing)

#define DEFAULT_REMOVE_EMPTY_UNEDITED_NOTES (true)
#define DEFAULT_EDITOR_CONVERT_TO_NOTE_TIMEOUT (500)
#define DEFAULT_EXPUNGE_NOTE_TIMEOUT (500)

#define DEFAULT_DOWNLOAD_NOTE_THUMBNAILS (true)
#define DEFAULT_DOWNLOAD_INK_NOTE_IMAGES (true)

#define DEFAULT_RUN_SYNC_EACH_NUM_MINUTES (15)

#endif // QUENTIER_DEFAULT_SETTINGS_H
