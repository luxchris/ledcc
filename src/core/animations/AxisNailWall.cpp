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


//TODO:: Change bool invert with Draw::Direction

AxisNailWall::AxisNailWall(const u_int16_t &speed, const Draw::Axis &axis, const bool invert, const QString &name, QObject *parent):
    Animation(speed,name,parent),
    m_axis(axis),
    m_invert(invert)
{
}


void AxisNailWall::createAnimation()
{
    QVector<u_int8_t> destination(CUBE_ARRAY_SIZE);
    QVector<u_int8_t> position(CUBE_ARRAY_SIZE);


    fillCubeArray(0x00);

    if (m_invert)
        setPlane(m_axis, 7);
    else
        setPlane(m_axis, 0);
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
        drawPositionAxis(m_axis, position, m_invert);
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
        drawPositionAxis(m_axis, position, m_invert);
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

    if(item->getOptions()->axis == X_AXIS)
        itemToolTip.append("Axis: X-Axis<br>");
    else if(item->getOptions()->axis == Y_AXIS)
        itemToolTip.append("Axis: Y-Axis<br>");
    else
        itemToolTip.append("Axis: Z-Axis<br>");
    itemToolTip.append(item->getOptions()->invert == true ? "Invert: Yes" : "Invert: No");

    item->setToolTip ( itemToolTip );
}



QStringList& AxisNailWall::getAnimationPropertiesAsPlainText(const AnimationItem *item)
{
    list.clear();

    list.append(getName());
    list.append(QString("Speed:%1").arg( item->getOptions()->speed));
    if( item->getOptions()->axis == X_AXIS )
        list.append( QString("Axis:X_AXIS") );
    else if( item->getOptions()->axis == Y_AXIS )
        list.append( QString("Axis:Y_AXIS") );
    else
        list.append( QString("Axis:Z_AXIS") );
    list.append( QString("Invert:%1").arg( item->getOptions()->invert == true ? "Yes" : "No"));

    return list;
}
