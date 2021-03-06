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
#include "Wall.hpp"
#include <QDebug>


Wall::Wall(const quint16 &speed, const Draw::Axis &axis, const Draw::Direction &direction, const QString &name, QObject *parent):
    Animation(speed,name,parent),
    m_direction(direction),
    m_axis(axis)
{
}

void Wall::createAnimation()
{
    quint8 i = 0;

    fillCubeArray(0x00);

    if (m_direction)
        setPlane(m_axis, 0);
    else
        setPlane(m_axis, 7);

    for (i = 0; i < CUBE_SIZE ; i++)
    {
        if(m_abort)
            return;
        waitMs(speed());
        shift(m_axis, m_direction);
    }
    Q_EMIT done();
}

