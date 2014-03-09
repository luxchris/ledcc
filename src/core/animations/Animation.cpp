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
#include "Animation.hpp"
#include <QTimer>


/**
 * @brief
 *
 * @param x
 * @param y
 * @param z
 */
void Animation::sendBixelZ(u_int8_t x, u_int8_t y, u_int8_t z)
{
    u_int8_t ii = 0;
    for (u_int8_t i = 0; i < CUBE_SIZE; i++)
    {
        if (z == ( CUBE_SIZE - 1))
        {
            ii = ( CUBE_SIZE - 1) - i;
            clearBixel(x, y, ii + 1);
        }
        else
        {
            ii = i;
            clearBixel(x, y, ii - 1);
        }
        setBixel(x, y, ii);
        waitMs(m_speed);
    }
}


/**
 * @brief If the number at QVector position "x" is greater or lower
 *        than the number in the destination QVector, the new position
 *        gets either decrimented or incremented by one.
 *
 * @param destination
 * @param position
 * @param axe
 */
void Animation::effectZUpDownMove(QVector<u_int8_t> &destination, QVector<u_int8_t> &position, Axis axis)
{
    for (u_int8_t px = 0; px < CUBE_ARRAY_SIZE; px++)
    {
        if (position[px] < destination[px])
            position[px]++;
        else if(position[px] > destination[px])
            position[px]--;

    }
//    fillCubeArray(0x00);
//    waitMs(10);
    drawPositionAxis(axis, position, true);
}


/**
 * @brief
 *
 * @param time
 */
void Animation::waitMs(const u_int16_t &time)
{
    Q_EMIT sendData(cubeFrame);
    thread()->msleep(time);
}
