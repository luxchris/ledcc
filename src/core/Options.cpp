#include "Options.hpp"

using namespace animations;
Options::Options(const QString &text, const u_int16_t &speed, const u_int16_t &delay,
                 const u_int16_t &leds, const u_int16_t &iteration, const Draw::Direction &direction,
                 const Draw::Axis &axis):
    m_text(text),
    m_speed(speed),
    m_delay(delay),
    m_leds(leds),
    m_iteration(iteration),
    m_direction(direction),
    m_axis(axis)
{

}
