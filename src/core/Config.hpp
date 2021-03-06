/**
 * Copyright (C) 2014  Christian Schwarzgruber <christiandev9@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <QObject>
#include <QScopedPointer>
#include <QVariant>

namespace Settings{
    const QString MainWindowGeometrySettings("MainWindowGeometry");
    const QString AnimationOptionPreviewGeometry("AnimationOptionPreviewGeometry");
    const QString AnimationListSplitterState("AnimationListSplitterState");
    const QString IsMainToolbarHidden("IsMainToolbarHidden");
    const QString IsAnimationToolbarHidden("IsAnimationToolbarHidden");
    const QString IsHelpToolbarHidden("IsHelpToolbarHidden");
    const QString IsAnimationOptionPreviewHidden("IsAnimationOptionPreviewHidden");
    const QString SettingsBaudeRate("Settings/Port/baude");
    const QString SettingsParity("Settings/Port/parity");
    const QString SettingsDataBits("Settings/Port/databits");
    const QString SettingsStopBits("Settings/Port/stopbits");
    const QString SettingsFlowControl("Settings/Port/flowcontrol");
    const QString LastSavePath("LastSavePath");
    const QString LastOpenPath("LastOpenPath");
    const QString FileDialogState("FileDialogState");
    const QString FileDialogHistorySave("FileDialogHistorySave");
    const QString FileDialogHistoryOpen("FileDialogHistoryOpen");
    }

class QSettings;

class Config : public QObject
{
    Q_OBJECT

public:
    ~Config();
    QVariant get(const QString &key);
    QVariant get(const QString &key, const QVariant &defaultValue);
    void set(const QString &key, const QVariant &value);

    static Config *instance();
    static void createConfigFromFile(QString &file);
    static void createTempFileInstance();
signals:
private:
    explicit Config(QObject *parent);
    Config(const QString &fileName, QObject* parent);
    void init(const QString &fileName);
    static Config *m_instance;

    QScopedPointer<QSettings> m_settings;
    QHash<QString, QVariant> m_defaults;

    Q_DISABLE_COPY(Config)
};

inline Config *config()
{
    return Config::instance();
}

#endif // CONFIG_HPP
