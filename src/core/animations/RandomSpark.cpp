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
#include "RandomSpark.hpp"
#include "RandomSparkFlash.hpp"



RandomSpark::RandomSpark(const quint16 &speed, const quint16 &iterations, const quint16 &sparks,
                         const QString &name, QObject *parent):
    Animation(speed,name,parent),
    m_sparks(sparks),
    m_iterations(iterations)
{

}

void RandomSpark::createAnimation()
{
    for (quint16 i = 0; i < m_sparks; i++)
    {
        createSparks(i);
    }
    for (quint16 i = m_sparks; i >= 1; i--)
    {
        createSparks(i);
    }
    Q_EMIT done();
}



void RandomSpark::createSparks(const quint16 &leds)
{
    for (quint16 i = 0; i < m_iterations; i++)
    {
        for (quint16 b = 0; b < leds; b++)
        {
            setBixel(qrand() % CUBE_SIZE, qrand() % CUBE_SIZE,
                    qrand() % CUBE_SIZE);
        }
        if(m_abort)
            return;
        waitMs(speed());
        fillCubeArray(0x00);
    }
}
