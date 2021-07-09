/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>


#include "Thing.h"
#include "Game.h"

Thing Thing::NOTHING;

void Thing::set(ID n_x, ID n_y, ID n_id) {
    x = n_x;
    y = n_y;
    id = n_id;
    Game& game = Game::getGame();
    game.setCell(x,y,id);
}

void Thing::applyMove() {
    Game& game = Game::getGame();

    if (dx!=0 || dy!=0) {

        ID x2 = x.asInt32()+dx;
        ID y2 = y.asInt32()+dy;

        if (game.getCell(x2,y2)==0) {
            game.setCell(x,y,0);
            x = x2;
            y = y2;
            game.setCell(x,y,id);
            printf("Implemented move for %ld\n", id.asInt32());
        } else {
            printf("Ignored blocked move\n");
        }

        dx = dy = 0;
    }
}
