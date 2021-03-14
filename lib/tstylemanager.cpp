/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2021 Victor Tran
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * *************************************/
#include "tstylemanager.h"

#include <tapplication.h>

struct tStyleManagerPrivate {
    tStyleManager::Style currentStyle = tStyleManager::System;
    tStyleManager::Platforms overrideOnPlatforms = tStyleManager::Windows | tStyleManager::MacOS | tStyleManager::OtherPlatform;
};

tStyleManager::tStyleManager(QObject* parent) : QObject(parent) {
    d = new tStyleManagerPrivate();
}

void tStyleManager::updateStyle() {
    if (isOverridingStyle()) {
        //Override the styles!
        QApplication::setStyle(QStyleFactory::create("contemporary"));

        QIcon::setThemeSearchPaths({
            "/usr/share/icons", //Linux
            "/share/icons", //Flatpak
            tApplication::applicationDirPath() + "\\icons", //Windows
            tApplication::macOSBundlePath() + "/Contents/Resources/icons" //macOS
        });
        QIcon::setThemeName("contemporary-icons");

        QPalette pal = QApplication::palette();

        //Get the accent colour
        QSettings accentDetection("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\DWM", QSettings::NativeFormat);
        QColor accentCol;
        if (accentDetection.contains("ColorizationColor")) {
            accentCol = QColor::fromRgb(QRgb(accentDetection.value("ColorizationColor").toInt() & 0x00FFFFFF));
            if (d->currentStyle == ContemporaryLight) {
                pal.setColor(QPalette::Button, accentCol.lighter(150));
            } else {
                pal.setColor(QPalette::Button, accentCol);
            }
        } else {
            accentCol = QColor::fromRgb((d->currentStyle == ContemporaryLight ? 0xC400C8FF : 0xC4003296) & 0x00FFFFFF);
            pal.setColor(QPalette::Button, accentCol);
        }

        if (d->currentStyle == ContemporaryLight) {
            pal.setColor(QPalette::ButtonText, QColor(0, 0, 0));
            pal.setColor(QPalette::Highlight, accentCol.lighter(125));
            pal.setColor(QPalette::HighlightedText, QColor(0, 0, 0));
            pal.setColor(QPalette::Disabled, QPalette::Button, accentCol.darker(200));
            pal.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(0, 0, 0));

            pal.setColor(QPalette::Window, QColor(210, 210, 210));
            pal.setColor(QPalette::Base, QColor(210, 210, 210));
            pal.setColor(QPalette::AlternateBase, QColor(210, 210, 210));
            pal.setColor(QPalette::WindowText, QColor(0, 0, 0));
            pal.setColor(QPalette::Text, QColor(0, 0, 0));
            pal.setColor(QPalette::ToolTipText, QColor(0, 0, 0));

            pal.setColor(QPalette::Disabled, QPalette::WindowText, QColor(100, 100, 100));
        } else {
            pal.setColor(QPalette::ButtonText, QColor(255, 255, 255));
            pal.setColor(QPalette::Highlight, accentCol.lighter(125));
            pal.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
            pal.setColor(QPalette::Disabled, QPalette::Button, accentCol.darker(200));
            pal.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(150, 150, 150));

            pal.setColor(QPalette::Window, QColor(40, 40, 40));
            pal.setColor(QPalette::Base, QColor(40, 40, 40));
            pal.setColor(QPalette::AlternateBase, QColor(60, 60, 60));
            pal.setColor(QPalette::WindowText, QColor(255, 255, 255));
            pal.setColor(QPalette::Text, QColor(255, 255, 255));
            pal.setColor(QPalette::ToolTipText, QColor(255, 255, 255));

            pal.setColor(QPalette::Disabled, QPalette::WindowText, QColor(150, 150, 150));
        }

        QApplication::setPalette(pal);
        QApplication::setPalette(pal, "QDockWidget");
        QApplication::setPalette(pal, "QToolBar");
    }
}

tStyleManager* tStyleManager::instance() {
    static tStyleManager* instance = new tStyleManager();
    return instance;
}

void tStyleManager::setOverrideStyleForApplication(tStyleManager::Style style) {
    instance()->d->currentStyle = style;
    instance()->updateStyle();
}

void tStyleManager::setOverrideStyleOnPlatforms(tStyleManager::Platforms platforms) {
    instance()->d->overrideOnPlatforms = platforms;
    instance()->updateStyle();
}

bool tStyleManager::isOverridingStyle() {
#if defined(Q_OS_WIN)
    return instance()->d->overrideOnPlatforms & Windows;
#elif defined(Q_OS_MAC)
    return instance()->d->overrideOnPlatforms & MacOS;
#else
    QString desktop = qEnvironmentVariable("XDG_CURRENT_DESKTOP");
    if (desktop == "thedesk") {
        return instance()->d->overrideOnPlatforms & TheDesk;
    } else {
        return instance()->d->overrideOnPlatforms & OtherPlatform;
    }
#endif
}
