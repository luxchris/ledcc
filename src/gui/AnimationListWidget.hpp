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
#ifndef ANIMATIONLISTWIDGET_HPP
#define ANIMATIONLISTWIDGET_HPP

#include "ListWidget.hpp"
#include "AnimationItem.hpp"
/**
 * @brief Handles all available animations
 *
 *
 */
class AnimationListWidget : public ListWidget
{
    Q_OBJECT

public:
    explicit AnimationListWidget(QWidget* parent = Q_NULLPTR);
    virtual ~AnimationListWidget();
    void translateItemToolTip();
Q_SIGNALS:
    void addToPlaylist(QList<QListWidgetItem *> selected);

protected:
    void keyPressEvent(QKeyEvent * e);
public Q_SLOTS:
    void insertAnimationItems(const QList<AnimationItem *> *animation);
    void addSelectedItemsToPlaylist();

private:
    void createActions();
    QAction *m_addToPlaylistAction;
};

#endif // ANIMATIONLISTWIDGET_HPP
