/*
 * SPDX-FileCopyrightText: 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * SPDX-FileCopyrightText: 2006-2010 RobotCub Consortium
 * SPDX-License-Identifier: BSD-3-Clause
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
