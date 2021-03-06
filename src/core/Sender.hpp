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
#ifndef SENDER_HPP
#define SENDER_HPP
#include <QThread>
#include "Global.hpp"
#include "animations/Draw.hpp"
#include "SettingsDialog.hpp"

class QSerialPort;

class Sender : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool portOpen READ portOpen WRITE setPortOpen NOTIFY portOpenChanged)
public:
    explicit Sender(QObject *parent = Q_NULLPTR);
    ~Sender();

    bool m_abort;
    bool portOpen() const
    {
        return m_portOpen;
    }

Q_SIGNALS:
    void portOpened(const QString &param);
    void portClosed(const QString &param);
    void portError(const QString &param);
    void closePort(const QString &param);
    void portOpenChanged(bool arg);

public Q_SLOTS:
    void sendAnimation(const Draw::CubeArray &d);
    void openCloseSerialPort(/*const SerialSettings *s*/);
    void closeSerialPort();

    void setPortOpen(bool arg)
    {
        if (m_portOpen != arg) {
            m_portOpen = arg;
            emit portOpenChanged(arg);
        }
    }

private:
    QSerialPort *m_serial;
    QString m_param;
    SerialSettings *m_port;
private:
    bool openSerialPort();
    bool checkPortSettings();
    Q_DISABLE_COPY(Sender)
    bool m_portOpen;
};

#endif // SENDER_HPP
