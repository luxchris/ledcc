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
#ifndef RANDOMZLIFT_HPP
#define RANDOMZLIFT_HPP

#include "Animation.hpp"



class RandomZLift : public Animation
{
    Q_OBJECT
    Q_PROPERTY(quint16 iterations READ getIterations WRITE setIterations)
public:
    explicit RandomZLift(const quint16 &iterations = 10, const quint16 &speed = 100,
                         const QString &name = "Random Z Lift",QObject *parent = Q_NULLPTR);

    quint16 getIterations() const
    {
        return m_iterations;
    }

public Q_SLOTS:
    virtual void createAnimation();
    void setIterations(quint16 &iterations)
    {
        if (m_iterations != iterations)
            m_iterations = iterations;
    }

private:
    quint16 m_iterations;
};

#endif // RANDOMZLIFT_HPP
