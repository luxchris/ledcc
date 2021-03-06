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
#include "WireBoxCornerShrinkGrow.hpp"



WireBoxCornerShrinkGrow::WireBoxCornerShrinkGrow(const quint16 &iterations, const quint16 &speed, const quint8 &rotate, const quint8 &flip, const QString &name, QObject *parent):
    Animation(speed,name,parent),
    m_iterations(iterations),
    m_rotate(rotate),
    m_flip(flip)
{
}

void WireBoxCornerShrinkGrow::createAnimation()
{
    for (quint8 i = 0; i < CUBE_SIZE; i++)
                createWireBoxCorner(i % 4, i & 0x04);
    Q_EMIT done();
}

void WireBoxCornerShrinkGrow::createWireBoxCorner(const quint8 rotate, const quint8 flip)
{
    quint8 xyz = 0;
    for (quint16 j = 0; j < m_iterations; j++)
    {
        for (quint8 i = 0; i < CUBE_SIZE * 2; i++)
        {
            xyz = CUBE_SIZE - 1 - i;
            if (i > CUBE_SIZE - 1)
                xyz = i - CUBE_SIZE;
            fillCubeArray(0x00);
            boxWireframe(0, 0, 0, xyz, xyz, xyz);

            if (flip > 0)
                mirrorZ();
            if (rotate == 1 || rotate == 3)
                mirrorY();
            if (rotate == 2 || rotate == 3)
                mirrorX();
            if(m_abort)
                return;
            waitMs(speed());
        }
    }
}

QStringList& WireBoxCornerShrinkGrow::getAnimationPropertiesAsPlainText(const AnimationItem *item)
{
    list.clear();

    list.append(name());
    list.append(QString("Speed:%1").arg(item->getOptions()->m_speed));
    list.append( QString( "Iterations:%1" ).arg( item->getOptions()->m_iteration ) );

    return list;
}
