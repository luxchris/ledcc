#ifndef AXISNAILWALL_HPP
#define AXISNAILWALL_HPP

#include "Animation.hpp"

class AxisNailWall : public Animation
{
    Q_OBJECT
    Q_PROPERTY(bool invert READ getInvert WRITE setInvert)
    Q_PROPERTY(Axis axis READ getAxis WRITE setAxis)

public:
    explicit AxisNailWall(const u_int16_t &speed = 70,
                          const Axis &axis = X_AXIS,
                          const bool invert = false,
                          const QString &name = "Axis Nail Wall",
                          QObject *parent = 0);

    bool getInvert() const
    {
        return m_invert;
    }

    Axis getAxis() const
    {
        return m_axis;
    }
signals:

public Q_SLOTS:
    virtual void createAnimation();

    void setInvert(const bool invert)
    {
        if(m_invert != invert)
            m_invert = invert;
    }
    void setAxis(const Axis axis)
    {
        if(m_axis != axis)
        m_axis = axis;
    }
private:
    Axis m_axis;
    bool m_invert;

};

#endif // AXISNAILWALL_HPP
