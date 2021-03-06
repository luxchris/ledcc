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
#include "Draw.hpp"
#include <string.h>
#include <QTime>
#include <QtGlobal>

#include "LookUpTables.hpp"


Draw::Draw(QObject *parent):
    QObject(parent)
{
    QTime time = QTime::currentTime();
    cubeFrame =  CubeArray(CUBE_SIZE,QVector<quint8>(CUBE_SIZE)) ;
    cubeFrameTemp =  CubeArray(CUBE_SIZE,QVector<quint8>(CUBE_SIZE)) ;
    qsrand(static_cast<uint>(time.msecsSinceStartOfDay()));
    qRegisterMetaType<CubeArray>("CubeArray");
}

Draw::~Draw()
{
    cubeFrame.clear();
    cubeFrameTemp.clear();
}

void Draw::setBixel(const quint8 x, const  quint8 y, const quint8 z)
{
    if (inRange(x, y, z))
        cubeFrame[z][y] |= ( 0x01 << x );
}

void Draw::setTempBixel(const quint8 x, const quint8 y, const quint8 z)
{
    if (inRange(x, y, z))
        cubeFrameTemp[z][y] |= ( 0x01 << x );
}

void Draw::clearBixel(const quint8 x, const quint8 y, const quint8 z)
{
    if (inRange(x, y, z))
        cubeFrame[z][y] &= ~( 0x01 << x );
}

void Draw::clearTempBixel(const quint8 x, const quint8 y, const quint8 z)
{
    if (inRange(x, y, z))
        cubeFrameTemp[z][y] &= ~( 0x01 << x );
}

Draw::BixelState Draw::getBixelState(const quint8 x, const quint8 y, const quint8 z)
{
    if (inRange(x, y, z)) {
        if (cubeFrame[z][y] & ( 0x01 << x ))
            return ON;
        else
            return OFF;
    } else {
        return OFF;
    }
}

void Draw::flipBixels(const quint8 x, const quint8 y, const quint8 z)
{
    if (inRange(x, y, z))
        cubeFrame[z][y] ^= ( 0x01 << x );
}

void Draw::alterBixel(const quint8 x, const quint8 y, const quint8 z, const BixelState state)
{
    if (state)
        setBixel(x, y, z);
    else
        clearBixel(x, y, z);
}

bool Draw::inRange(const quint8 x, const quint8 y, const quint8 z)
{
    if (x < CUBE_SIZE && y < CUBE_SIZE && z < CUBE_SIZE)
        return true;
    return false;
}

void Draw::shift(const Axis axis, const Direction direction)
{
    quint8 i, x, y, ii, iii;
    for (i = 0; i < CUBE_SIZE; i++) {
        if (direction)
            ii = IT_CUBE_SIZE - i;
        else
            ii = i;

        for (x = 0; x < CUBE_SIZE; x++) {
            for (y = 0; y < CUBE_SIZE; ++y) {
                if (direction)
                    iii = ii - 1;
                else
                    iii = ii + 1;

                switch (axis) {
                case X_AXIS:
                    alterBixel(ii, y, x, getBixelState(iii, y, x));
                    break;
                case Y_AXIS:
                    alterBixel(x, ii, y, getBixelState(x, iii, y));
                    break;
                case Z_AXIS:
                    alterBixel(x, y, ii, getBixelState(x, y, iii));
                    break;
                default:
                    break;
                }
            }
        }
    }
    if (direction) {
        i = 0;
    } else {
        i = IT_CUBE_SIZE;
    }

    for (x = 0; x < CUBE_SIZE; x++) {
        for (y = 0; y < CUBE_SIZE; y++) {
            switch (axis) {
            case X_AXIS:
                clearBixel(i, y, x);
                break;
            case Y_AXIS:
                clearBixel(x, i, y);
                break;
            case Z_AXIS:
                clearBixel(x, y, i);
                break;
            default:
                break;
            }
        }
    }
}

void Draw::checkArgumentOrder(quint8 from, quint8 to, quint8 *newStartPoint, quint8 *newEndPoint)
{
    if (from > to) {
        quint8 tmp;
        tmp = from;
        from = to;
        to = tmp;
    }
    *newStartPoint = from;
    *newEndPoint = to;
}

void Draw::drawPositionAxis(const Axis axis, const QVector<quint8> &position, const Direction direction)
{
    quint8 k = 0;

    fillCubeArray(0x00);
    for (quint8 x = 0; x < CUBE_SIZE; x++)
    {
        for (quint8 y = 0; y < CUBE_SIZE; y++)
        {
            if (direction) /* Forward */
                k = position[x * CUBE_SIZE + y];
            else
                k = CUBE_SIZE - 1 - position[x * CUBE_SIZE + y];

            switch (axis)
            {
            case X_AXIS:
                setBixel(k, y, x);
                break;
            case Y_AXIS:
                setBixel(x, k, y);
                break;
            case Z_AXIS:
                setBixel(x, y, k);
                break;
            default:
                break;
            }
        }
    }
}
//TODO:: Currently this is optimized for 32bit Processors, we should also add one for 64Bit
//       processors and identify the processor size within the cmake files, to set a flag which
//       of those should be compiled.
quint8 Draw::flipByte(const quint8 byte)
{
    quint8 b = byte;
    b = ( ( ( b * 0x0802LU ) & 0x22110LU ) | ( ( b * 0x8020LU ) & 0x88440LU ) ) * 0x10101LU >> 16;
    return b;
}

void Draw::setPlaneZ(const quint8 z)
{
    quint8 i;
    if (z < CUBE_SIZE) {
        for (i = 0; i < CUBE_SIZE; i++)
            cubeFrame[z][i] = 0xff;
    }
}

void Draw::clearPlaneZ(const quint8 z)
{
    quint8 i;
    if (z < CUBE_SIZE) {
        for (i = 0; i < CUBE_SIZE; i++)
            cubeFrame[z][i]  = 0x00;
    }
}

void Draw::setPlaneX(const quint8 x)
{
    quint8 z, y;
    if (x < CUBE_SIZE) {
        for (z = 0; z < CUBE_SIZE; z++) {
            for (y = 0; y < CUBE_SIZE; y++)
                cubeFrame[z][y] |= ( 1 << x );
        }
    }
}

void Draw::clearPlaneX(const quint8 x)
{
    quint8 z, y;
    if (x < CUBE_SIZE) {
        for (z = 0; z < CUBE_SIZE; z++) {
            for (y = 0; y < CUBE_SIZE; y++)
                cubeFrame[z][y] &= ~( 1 << x );
        }
    }
}

void Draw::setPlaneY(const quint8 y)
{
    quint8 z;
    if (y < CUBE_SIZE) {
        for (z = 0; z < CUBE_SIZE; z++)
            cubeFrame[z][y] = 0xFF;
    }
}

void Draw::clearPlaneY(const quint8 y)
{
    quint8 z;
    if (y < CUBE_SIZE) {
        for (z = 0; z < CUBE_SIZE; z++)
            cubeFrame[z][y] = 0x00;
    }
}

void Draw::setPlane(const Axis axis, const quint8 i)
{
    switch (axis) {
    case X_AXIS:
        setPlaneX(i);
        break;
    case Y_AXIS:
        setPlaneY(i);
        break;
    case Z_AXIS:
        setPlaneZ(i);
        break;
    default:
        break;
    }
}

void Draw::clearPlane(const Axis axis, const quint8 i)
{
    switch (axis) {
    case X_AXIS:
        clearPlaneX(i);
        break;
    case Y_AXIS:
        clearPlaneY(i);
        break;
    case Z_AXIS:
        clearPlaneZ(i);
        break;
    default:
        break;
    }
}

void Draw::boxWireframe(quint8 x1, quint8 y1, quint8 z1, quint8 x2, quint8 y2, quint8 z2)
{
    quint8 iy = 0;
    quint8 iz = 0;

    checkArgumentOrder(x1, x2, &x1, &x2);
    checkArgumentOrder(y1, y2, &y1, &y2);
    checkArgumentOrder(z1, z2, &z1, &z2);

    // Lines along X axis
    cubeFrame[z1][y1] = byteline(x1, x2);
    cubeFrame[z1][y2] = byteline(x1, x2);
    cubeFrame[z2][y1] = byteline(x1, x2);
    cubeFrame[z2][y2] = byteline(x1, x2);

    // Lines along Y axis
    for (iy = y1; iy <= y2; iy++) {
        setBixel(x1, iy, z1);
        setBixel(x1, iy, z2);
        setBixel(x2, iy, z1);
        setBixel(x2, iy, z2);
    }

    // Lines along Z axis
    for (iz = z1; iz <= z2; iz++) {
        setBixel(x1, y1, iz);
        setBixel(x1, y2, iz);
        setBixel(x2, y1, iz);
        setBixel(x2, y2, iz);
    }
}

void Draw::boxFilled(quint8 x1, quint8 y1, quint8 z1, quint8 x2, quint8 y2, quint8 z2)
{
    quint8 iy;
    quint8 iz;

    checkArgumentOrder(x1, x2, &x1, &x2);
    checkArgumentOrder(y1, y2, &y1, &y2);
    checkArgumentOrder(z1, z2, &z1, &z2);

    for (iz = z1; iz <= z2; iz++) {
        for (iy = y1; iy <= y2; iy++) {
            cubeFrame[iz][iy] |= byteline(x1, x2);
        }
    }
}

void Draw::boxWalls(quint8 x1, quint8 y1, quint8 z1, quint8 x2, quint8 y2, quint8 z2)
{
    quint8 iy = 0;
    quint8 iz = 0;

    checkArgumentOrder(x1, x2, &x1, &x2);
    checkArgumentOrder(y1, y2, &y1, &y2);
    checkArgumentOrder(z1, z2, &z1, &z2);

    for (iz = z1; iz <= z2; iz++) {
        for (iy = y1; iy <= y2; iy++) {
            if (iy == y1 || iy == y2 || iz == z1 || iz == z2) {
                cubeFrame[iz][iy] = byteline(x1, x2);
            } else {
                cubeFrame[iz][iy] |= ( ( 0x01 << x1 ) | ( 0x01 << x2 ) );
            }
        }
    }
}

void Draw::mirrorX()
{
    quint8 y = 0;
    quint8 z = 0;
    for (z = 0; z < CUBE_SIZE; z++) {
        for (y = 0; y < CUBE_SIZE; y++) {
            cubeFrameTemp[z][y] = flipByte(cubeFrame[z][y]);
        }
    }
    cubeFrame = cubeFrameTemp;
}

void Draw::mirrorY()
{
    quint8 z = 0;
    quint8 y = 0;
    for (z = 0; z < CUBE_SIZE; z++) {
        for (y = 0; y < CUBE_SIZE; y++) {
            cubeFrameTemp[z][CUBE_SIZE - y - 1] = cubeFrame[z][y];
        }
    }
    cubeFrame = cubeFrameTemp;
}

void Draw::mirrorZ()
{
    quint8 y = 0;
    quint8 z = 0;
    for (z = 0; z < CUBE_SIZE; z++) {
        for (y = 0; y < CUBE_SIZE; y++) {
            cubeFrameTemp[CUBE_SIZE - z - 1][y] = cubeFrame[z][y];
        }
    }
    cubeFrame = cubeFrameTemp;
}

void Draw::fillTempCubeArray(const quint8 &pattern)
{
    for(quint8 i = 0; i < CUBE_SIZE; i++)
        cubeFrameTemp[i].fill(pattern);
}

void Draw::fillCubeArray(const quint8 &pattern)
{
    for(quint8 i = 0; i < CUBE_SIZE; i++)
        cubeFrame[i].fill(pattern);
}

quint8 Draw::byteline(const quint8 &start, const quint8 &end)
{
    return ( ( 0xff << start ) & ~( 0xff << ( end + 1 ) ) );
}

void Draw::tmpCubeToCube()
{
    cubeFrame = cubeFrameTemp;
}

void Draw::fontGetChar(quint8 chr, quint8 dst[5])
{
    quint8 i=0;
    chr -= 32; // bitmap starts at ascii 32 (space)

    for (i = 0; i < 5; i++)
        dst[i] = lookUpTable[(chr*5)+i];
}


float Draw::distance2d(float x1, float y1, float x2, float y2)
{
    return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}


float Draw::distance3d(float x1, float y1, float z1, float x2, float y2, float z2)
{
    return  sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) + (z1-z2)*(z1-z2));
}


void Draw::line3d(int x1, int y1, int z1, int x2, int y2, int z2)
{
    int i, dx, dy, dz, l, m, n, x_inc, y_inc, z_inc,
            err_1, err_2, dx2, dy2, dz2;
    int pixel[3];
    pixel[0] = x1;
    pixel[1] = y1;
    pixel[2] = z1;
    dx = x2 - x1;
    dy = y2 - y1;
    dz = z2 - z1;
    x_inc = (dx < 0) ? -1 : 1;
    l = abs(dx);
    y_inc = (dy < 0) ? -1 : 1;
    m = abs(dy);
    z_inc = (dz < 0) ? -1 : 1;
    n = abs(dz);
    dx2 = l << 1;
    dy2 = m << 1;
    dz2 = n << 1;
    if ((l >= m) && (l >= n)) {
        err_1 = dy2 - l;
        err_2 = dz2 - l;
        for (i = 0; i < l; i++) {
            //PUT_PIXEL(pixel);
            setBixel(pixel[0],pixel[1],pixel[2]);
            //printf("Setting %i %i %i \n", pixel[0],pixel[1],pixel[2]);
            if (err_1 > 0) {
                pixel[1] += y_inc;
                err_1 -= dx2;
            }
            if (err_2 > 0) {
                pixel[2] += z_inc;
                err_2 -= dx2;
            }
            err_1 += dy2;
            err_2 += dz2;
            pixel[0] += x_inc;
        }
    } else if ((m >= l) && (m >= n)) {
        err_1 = dx2 - m;
        err_2 = dz2 - m;
        for (i = 0; i < m; i++) {
            //PUT_PIXEL(pixel);
            setBixel(pixel[0],pixel[1],pixel[2]);
            //printf("Setting %i %i %i \n", pixel[0],pixel[1],pixel[2]);
            if (err_1 > 0) {
                pixel[0] += x_inc;
                err_1 -= dy2;
            }
            if (err_2 > 0) {
                pixel[2] += z_inc;
                err_2 -= dy2;
            }
            err_1 += dx2;
            err_2 += dz2;
            pixel[1] += y_inc;
        }
    } else {
        err_1 = dy2 - n;
        err_2 = dx2 - n;
        for (i = 0; i < n; i++) {
            setBixel(pixel[0],pixel[1],pixel[2]);
            //printf("Setting %i %i %i \n", pixel[0],pixel[1],pixel[2]);
            //PUT_PIXEL(pixel);
            if (err_1 > 0) {
                pixel[1] += y_inc;
                err_1 -= dz2;
            }
            if (err_2 > 0) {
                pixel[0] += x_inc;
                err_2 -= dz2;
            }
            err_1 += dy2;
            err_2 += dx2;
            pixel[2] += z_inc;
        }
    }
    setBixel(pixel[0],pixel[1],pixel[2]);
    //printf("Setting %i %i %i \n", pixel[0],pixel[1],pixel[2]);
    //PUT_PIXEL(pixel);
}
