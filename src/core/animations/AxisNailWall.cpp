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



AxisNailWall::AxisNailWall(const u_int16_t &speed, const Draw::Axis &axis, const Draw::Direction &direction, const QString &name, QObject *parent):
    Animation(speed,name,parent),
    m_axis(axis),
    m_direction(direction)
{
}


void AxisNailWall::createAnimation()
{
    QVector<u_int8_t> destination(CUBE_ARRAY_SIZE);
    QVector<u_int8_t> position(CUBE_ARRAY_SIZE);


    fillCubeArray(0x00);

    if (Draw::Forward )
        setPlane(m_axis, 0);
    else
        setPlane(m_axis, 7);

    if(m_abort)
        return;
    waitMs(getSpeed() * 3);

    for (u_int8_t i = 0; i < CUBE_ARRAY_SIZE; i++)
        destination[i] = qrand() % CUBE_SIZE;

    for (u_int8_t i = 0; i < CUBE_SIZE; i++)
    {
        for (u_int8_t px = 0; px < CUBE_ARRAY_SIZE; px++)
        {
            if (position[px] < destination[px])
                position[px]++;
        }
        drawPositionAxis(m_axis, position, m_direction);
        if(m_abort)
            return;
        waitMs(getSpeed());
    }

    for (u_int8_t i = 0; i < CUBE_ARRAY_SIZE; i++)
    {
        destination[i] = CUBE_SIZE - 1;
    }
    if(m_abort)
        return;
    waitMs(getSpeed() * 10);

    for (u_int8_t i = 0; i < CUBE_SIZE; i++)
    {
        for (u_int8_t px = 0; px < CUBE_ARRAY_SIZE; px++)
        {
            if (position[px] < destination[px])
                position[px]++;
            else if (position[px] > destination[px])
                position[px]--;
        }
        drawPositionAxis(m_axis, position, m_direction);
        if(m_abort)
            return;
        waitMs(getSpeed());
    }
    Q_EMIT done();
}



void AxisNailWall::createAnimationTooltipAsRichText(AnimationItem *item)
{
    QString itemToolTip;
    setItemToolTipNameSpeed(&itemToolTip, item);

    if(item->getOptions()->m_axis == X_AXIS)
        itemToolTip.append("Axis: X-Axis<br>");
    else if(item->getOptions()->m_axis == Y_AXIS)
        itemToolTip.append("Axis: Y-Axis<br>");
    else
        itemToolTip.append("Axis: Z-Axis<br>");
    itemToolTip.append( QString("Direction: %1").arg(item->getOptions()->m_direction == Draw::Forward ? "Forward" : "Backward"));

    item->setToolTip ( itemToolTip );
}



QStringList& AxisNailWall::getAnimationPropertiesAsPlainText(const AnimationItem *item)
{
    list.clear();

    list.append(getName());
    list.append(QString("Speed:%1").arg( item->getOptions()->m_speed));
    if( item->getOptions()->m_axis == X_AXIS )
        list.append( QString("Axis:X_AXIS") );
    else if( item->getOptions()->m_axis == Y_AXIS )
        list.append( QString("Axis:Y_AXIS") );
    else
        list.append( QString("Axis:Z_AXIS") );
    list.append( QString("Direction:%1").arg( item->getOptions()->m_direction == Draw::Forward ? "Forward" : "Backward"));

    return list;
}
