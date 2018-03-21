/*
 * Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
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

        ID x2 = x.asInt()+dx;
        ID y2 = y.asInt()+dy;
    
        if (game.getCell(x2,y2)==0) {
            game.setCell(x,y,0);    
            x = x2;
            y = y2;
            game.setCell(x,y,id);
            printf("Implemented move for %ld\n", id.asInt());
        } else {
            printf("Ignored blocked move\n");
        }

        dx = dy = 0;
    }
}
  

