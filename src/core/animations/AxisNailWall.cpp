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
#include "AxisNailWall.hpp"



AxisNailWall::AxisNailWall(const quint16 &speed, const Draw::Axis &axis, const Draw::Direction &direction, const QString &name, QObject *parent):
    Animation(speed,name,parent),
    m_axis(axis),
    m_direction(direction)
{
}


void AxisNailWall::createAnimation()
{
    QVector<quint8> destination(CUBE_ARRAY_SIZE);
    QVector<quint8> position(CUBE_ARRAY_SIZE);

    fillCubeArray(0x00);

    if (m_direction == Draw::Forward)
        setPlane(m_axis, 0);
    else
        setPlane(m_axis, 7);

    if(m_abort)
        return;
    waitMs(speed() * 3);

    for (quint8 i = 0; i < CUBE_ARRAY_SIZE; i++)
        destination[i] = qrand() % CUBE_SIZE;

    for (quint8 i = 0; i < CUBE_SIZE; i++)
    {
        for (quint8 px = 0; px < CUBE_ARRAY_SIZE; px++)
        {
            if (position[px] < destination[px])
                position[px]++;
        }
        drawPositionAxis(m_axis, position, m_direction);
        if(m_abort)
            return;
        waitMs(speed());
    }

    for (quint8 i = 0; i < CUBE_ARRAY_SIZE; i++)
    {
        destination[i] = CUBE_SIZE - 1;
    }
    if(m_abort)
        return;
    waitMs(speed() * 10);

    for (quint8 i = 0; i < CUBE_SIZE; i++)
    {
        for (quint8 px = 0; px < CUBE_ARRAY_SIZE; px++)
        {
            if (position[px] < destination[px])
                position[px]++;
            else if (position[px] > destination[px])
                position[px]--;
        }
        drawPositionAxis(m_axis, position, m_direction);
        if(m_abort)
            return;
        waitMs(speed());
    }
    Q_EMIT done();
}

