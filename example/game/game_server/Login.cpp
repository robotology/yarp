/*
 * Copyright: (C) 2010 RobotCub Consortium
 * Author: Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 */

#include "Login.h"

#include "Game.h"

Login::Login() {
}


bool Login::apply(const char *name, const char *key) {
    Game& game = Game::getGame();

    Thing& rthing = game.newThing();
    base_id = rthing.getID();

    return (base_id.asInt()!=-1);
}


Thing& Login::getThing() {
    Game& game = Game::getGame();
    return game.getThing(base_id);
}

void Login::shutdown() {
    if (base_id.asInt()!=-1) {
        Game& game = Game::getGame();
        game.killThing(base_id);
        base_id = -1;
    }
}

