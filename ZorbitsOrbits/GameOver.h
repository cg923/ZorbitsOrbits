// Copyright (c) 2017 Happy Rock Studios

#ifndef GAME_OVER_H
#define GAME_OVER_H

#include "JollyLava/Screen.h"

class GameOver : public jl::Screen
{
public:

    typedef fwk::Ptr<GameOver> Ptr;

    GameOver(ZorbitsOrbits * game);
    void initializeConcrete();
    void reset();
    // -------------------------------------------------
    void processInput(sf::Event sfevent);
    void update();
    // -------------------------------------------------
    void draw();
    // -------------------------------------------------
    void pauseConcrete();
    void unpauseConcrete();
    void cleanUp() {}

private:
    jl::TextureDesc::Ptr    _gameOverDesc;
    sf::Sprite              _gameOverSprite;
    sftools::Chronometer    _timer;
    bool _ending;
    bool _finished;
};

#endif




