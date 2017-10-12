// Copyright (c) 2017 Happy Rock Studios

#ifndef CREDITS_H
#define CREDITS_H

#include <queue>

#include "JollyLava/Screen.h"

class ZSpaceGenerator;

class Credits : public jl::Screen
{
public:

    typedef fwk::Ptr<Credits> Ptr;

    Credits(ZorbitsOrbits * game);
    void initializeConcrete();
    void reset();
    // -------------------------------------------------
    void processInput(sf::Event sfevent);
    void update();
    // -------------------------------------------------
    void draw();
    // -------------------------------------------------
    void pauseConcrete() {}
    void unpauseConcrete() {}
    void powerDown();

    void repositionZorbit();

    void generateCredits();
    sf::Text formattedText(std::string str, float yCoord);

private:
    jl::TextureDesc::Ptr    _zorbitsOrbitsDesc;
    sf::Sprite              _zorbitSprite;
    jl::TextureDesc::Ptr    _zorbitDesc;
    sf::Sprite              _zSprite;
    sf::Vector2f            _zorbitTrajectory;
    jl::TextureDesc::Ptr    _fadeDesc;
    sf::Sprite              _fadeSprite;
    sftools::Chronometer               _timer;
    sftools::Chronometer             _fadeTimer;
    sftools::Chronometer             _nonResettingTimer;
    sftools::Chronometer             _zorbitTimer;
    bool                    _ending;
    bool                    _finished;

    std::list<sf::Text>     _credits;

    ZSpaceGenerator* _space;
};

#endif



