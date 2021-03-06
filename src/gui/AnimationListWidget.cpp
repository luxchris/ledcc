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
#include "AnimationListWidget.hpp"
#include "Animation.hpp"
#include "Animations.hpp"
// Third Party Includes
#include "aqp.hpp"
#include "alt_key.hpp"

// Qt includes
#include <QKeyEvent>
#include <QAction>

// Debug Includes
#ifdef _DEBUG_
#include <QDebug>
#endif

AnimationListWidget::AnimationListWidget(QWidget *parent):
    ListWidget(parent)
{
    setDropIndicatorShown(false);
    setMovement(Static);
    setDefaultDropAction(Qt::IgnoreAction);
    setEditTriggers(DoubleClicked | EditKeyPressed);
    setDragDropMode(DragOnly);
    setDragEnabled(true);
    setAcceptDrops(false);
    setSortingEnabled(true);
    createActions();

    connect( m_addToPlaylistAction, &QAction::triggered, this, &AnimationListWidget::addSelectedItemsToPlaylist);
}

AnimationListWidget::~AnimationListWidget()
{

}

void AnimationListWidget::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Enter:
        addSelectedItemsToPlaylist();
        break;
    case Qt::Key_Return:
        addSelectedItemsToPlaylist();
        break;
    case Qt::Key_Up:
        ListWidget::keyPressEvent(e);
        break;
    case Qt::Key_Down:
        ListWidget::keyPressEvent(e);
        break;
    case Qt::Key_Escape:
        ListWidget::keyPressEvent(e);
        break;
    case Qt::Key_Space:
        break;
    default:
        e->ignore();
        break;
    }
    e->accept();
}

void AnimationListWidget::translateItemToolTip()
{
    selectAllItems();
    QList<QListWidgetItem*> list = selectedItems();
    for (int i = 0; i < list.count(); i++) {
        dynamic_cast<AnimationItem*>(list.at(i))->createAnimationTooltipAsRichText();
    }
}


void AnimationListWidget::insertAnimationItems(const QList<AnimationItem *> *animation)
{
    foreach (AnimationItem *i, *animation)
        addItem(i);
}

void AnimationListWidget::createActions()
{
    m_addToPlaylistAction = createAction(tr("&Add to Playlist"));
    addAction(m_addToPlaylistAction);
}


void AnimationListWidget::addSelectedItemsToPlaylist()
{
    Q_EMIT addToPlaylist(selectedItems());
}





