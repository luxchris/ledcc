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

#ifndef XMLPLAYLISTREADER_HPP
#define XMLPLAYLISTREADER_HPP
#include <QList>

/* Forward declarations */
#include "AnimationItem.hpp"
class QXmlStreamReader;
class QXmlStreamAttributes;

class XmlPlaylistReader
{
public:
    QList<AnimationItem*> readAnimationPlaylist(const QString &xmlPlaylist);
private:
    AnimationItem* parseAnimation(QXmlStreamReader *xmlReader);
    static int readAnimationProperties(QXmlStreamAttributes *attributes, AnimationItem::Options *options, AnimationItem *animationItem);
    static void cleanUpOnError(QList<AnimationItem *> *animationItemList);
};

#endif // XMLPLAYLISTREADER_HPP
