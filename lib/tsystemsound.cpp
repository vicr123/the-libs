/****************************************
 *
 *   INSERT-PROJECT-NAME-HERE - INSERT-GENERIC-NAME-HERE
 *   Copyright (C) 2019 Victor Tran
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
#include "tsystemsound.h"

#include <QDir>
#include <QMediaPlayer>
#include <QDirIterator>
#include "tapplication.h"

tSystemSound::tSystemSound(QObject *parent) : QObject(parent)
{

}

#ifdef T_OS_UNIX_NOT_MAC
    QSettings::Format desktopFormat = QSettings::InvalidFormat;

    QSettings::Format getDesktopFormat() {
        if (desktopFormat == QSettings::InvalidFormat) {
            desktopFormat = QSettings::registerFormat("desktop", [](QIODevice &device, QSettings::SettingsMap &map) -> bool {
                QString group;
                while (!device.atEnd()) {
                    QString line = device.readLine().trimmed();
                    if (line.startsWith("[") && line.endsWith("]")) {
                        group = line.mid(1, line.length() - 2);
                    } else {
                        QString key = line.left(line.indexOf("="));
                        QString value = line.mid(line.indexOf("=") + 1);
                        map.insert(group + "/" + key, value);
                    }
                }
                return true;
            }, [](QIODevice &device, const QSettings::SettingsMap &map) -> bool {
                return false;
            }, Qt::CaseInsensitive);
        }

        return desktopFormat;
    }

    QStringList findSoundThemeDirs(QString soundThemeName) {
        //Search for the sound in the sound theme
        QStringList themeSearchPaths = {
            QDir::homePath() + "/.local/share/sounds",
            "/usr/share/sounds"
        };

        QStringList folderSearches;
        for (QString searchPath : themeSearchPaths) {
            QDir dir(searchPath);
            for (QString folderName : dir.entryList(QDir::Dirs)) {
                QDir themeDir(dir.absoluteFilePath(folderName));
                if (themeDir.exists("index.theme")) {
                    //Read in the index.theme file
                    QSettings themeFile(themeDir.absoluteFilePath("index.theme"), getDesktopFormat());
                    if (themeFile.value("Sound Theme/Name").toString() == soundThemeName || (soundThemeName == "freedesktop" && themeDir.dirName() == "freedesktop")) {
                        //We found the sound theme
                        QStringList soundThemeFolders;
                        soundThemeFolders.append(themeDir.absolutePath());

                        if (soundThemeName == "freedesktop") return soundThemeFolders; //freedesktop sound theme does not inherit anything
                        if (themeFile.contains("Sound Theme/Inherits")) {
                            soundThemeFolders.append(findSoundThemeDirs(themeFile.value("Sound Theme/Inherits").toString()));
                        } else {
                            soundThemeFolders.append(findSoundThemeDirs("freedesktop"));
                        }

                        return soundThemeFolders;
                    }
                }
            }
        }

        //We haven't found the sound theme
        if (soundThemeName == "freedesktop") {
            return {};
        } else {
            //Fall back to freedesktop
            return {findSoundThemeDirs("freedesktop")};
        }
    }

    QString soundFilePath(QString name, QString themePath) {
        QDirIterator iterator(themePath, QDirIterator::Subdirectories);
        while (iterator.hasNext()) {
            iterator.next();
            QFileInfo info = iterator.fileInfo();
            if (QStringList({"ogg", "oga", "wav"}).contains(info.suffix()) && info.completeBaseName() == name) {
                //We got it!
                return iterator.filePath();
            }
        }

        if (name.contains("-")) {
            //Chop the name at the last "-" and try again
            return soundFilePath(name.left(name.lastIndexOf("-")), themePath);
        }

        return "";
    }

    tSystemSound* tSystemSound::play(QString soundName) {
        return play(soundName, 1);
    }

    tSystemSound* tSystemSound::play(QString soundName, qreal volume) {
        QString soundPath = soundLocation(soundName);
        if (soundPath != "") {
            tSystemSound* engine = new tSystemSound();

            QMediaPlayer* player = new QMediaPlayer(engine, QMediaPlayer::LowLatency);
            player->setAudioRole(QAudio::NotificationRole);
            player->setMedia(QUrl::fromLocalFile(soundPath));
            player->setVolume(qRound(QAudio::convertVolume(volume, QAudio::LogarithmicVolumeScale, QAudio::LinearVolumeScale) * 100));
            connect(player, &QMediaPlayer::stateChanged, engine, [=](QMediaPlayer::State state) {
                if (state == QMediaPlayer::StoppedState) {
                    //We're done here
                    emit engine->done();

                    player->deleteLater();
                    engine->deleteLater();
                }
            });
            player->play();

            return engine;
        } else {
            return nullptr;
        }
    }

    QString tSystemSound::soundLocation(QString soundName) {
        QSettings settings("theSuite", "ts-qtplatform");
        QString soundTheme = settings.value("sound/theme", "Contemporary").toString();

        //Get the inheritance tree of the sound theme
        QStringList soundThemeDirs = findSoundThemeDirs(soundTheme);

        //Find the required sound
        for (QString soundTheme : soundThemeDirs) {
            QString soundPath = soundFilePath(soundName, soundTheme);
            if (soundPath != "") return soundPath;
        }
        return "";
    }

    bool tSystemSound::isSoundEnabled(QString soundName) {
        QSettings settings("theSuite", "ts-qtplatform");
        settings.beginGroup("sound");
        return settings.value("snd-" + soundName, true).toBool();
    }

    void tSystemSound::setSoundEnabled(QString soundName, bool isEnabled) {
        QSettings settings("theSuite", "ts-qtplatform");
        settings.beginGroup("sound");
        settings.setValue("snd-" + soundName, isEnabled);
    }
#elif defined(Q_OS_MAC)
tSystemSound* tSystemSound::play(QString soundName, qreal volume) {
    if (soundName == "bell") tApplication::beep();
    return nullptr;
}

QString tSystemSound::soundLocation(QString soundName) {
    return "";
}

bool tSystemSound::isSoundEnabled(QString soundName) {
    if (soundName == "bell") return true;
    return false;
}

void tSystemSound::setSoundEnabled(QString soundName, bool isEnabled) {
    //do nothing for now
}
#else
    tSystemSound* tSystemSound::play(QString soundName, qreal volume) {
        return nullptr;
    }

    QString tSystemSound::soundLocation(QString soundName) {
        return "";
    }
    bool tSystemSound::isSoundEnabled(QString soundName) {
        return false;
    }

    void tSystemSound::setSoundEnabled(QString soundName, bool isEnabled) {
        //do nothing for now
    }
#endif
