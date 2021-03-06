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
#include "StringFly.hpp"


StringFly::StringFly(const quint16 &speed, const QString &string, const QString &name, QObject *parent) :
    Animation(speed,name,parent),
    m_sToDisplay(string)
{
}

void StringFly::createAnimation()
{
    quint8 x = 0;
    quint8 z = 0;
    quint8 i = 0;
    quint8 chr[5];

    for(int j = 0; j < m_sToDisplay.length(); j++)
    {
        fontGetChar(m_sToDisplay.at(j).toLatin1(), chr);

        // Put a character on the back of the cube
        for (x = 0; x < 5; x++)
        {
            for (z = 0; z < 8; z++)
            {
                if ((chr[x] & (0x80 >> z)))
                {
                    setBixel(x + 2, 7, z);
                }
            }
        }
        // Shift the entire contents of the cube forward by 6 steps
        // before placing the next character
        for (i = 0; i < 6; i++)
        {
            if(m_abort)
                return;
            waitMs(speed());
            shift(Y_AXIS, Backward);
        }
    }
    // Shift the last character out of the cube.
    for (i = 0; i < 8; i++)
    {
        if(m_abort)
            return;
        waitMs(speed());
        shift(Y_AXIS, Backward);
    }
    Q_EMIT done();
}
