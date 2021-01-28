/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * Copyright (C) 2006-2010 RobotCub Consortium
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

#include "Login.h"

#include "Game.h"

Login::Login() {
}


bool Login::apply(const char *name, const char *key) {
    Game& game = Game::getGame();

    Thing& rthing = game.newThing();
    base_id = rthing.getID();

    return (base_id.asInt32()!=-1);
}


Thing& Login::getThing() {
    Game& game = Game::getGame();
    return game.getThing(base_id);
}

void Login::shutdown() {
    if (base_id.asInt32()!=-1) {
        Game& game = Game::getGame();
        game.killThing(base_id);
        base_id = -1;
    }
}
