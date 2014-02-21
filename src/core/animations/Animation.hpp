#ifndef ANIMATION_HPP
#define ANIMATION_HPP
#include "Draw.hpp"
//#include <QVector>

class Animation : public Draw
{
public:
    explicit  Animation(const u_int16_t &speed, const QString &name,QObject *parent = Q_NULLPTR):
        Draw(parent),
        m_speed(speed),
        m_name(name){
    }

    void setName(const QString &name){
        if(m_name.compare(name) == 0){
            m_name = name;
        }
    }
    QString getName(void) const{
        return m_name;
    }
    void setSpeed(const u_int16_t &speed){
        if(m_speed != speed){
            m_speed = speed;
        }
    }
    u_int16_t getSpeed(void) const{
        return m_speed;
    }

    void sendBixelZ(u_int8_t x, u_int8_t y, u_int8_t z, u_int16_t speed);
    void effectZUpDownMove(QVector<u_int8_t> &destination,
            QVector<u_int8_t> &position, Axis axe);
    virtual void createAnimation(void) = 0;
    void waitMs(const u_int16_t &time);
private:
    u_int16_t m_speed;
    QString m_name;
};

#endif // ANIMATION_HPP