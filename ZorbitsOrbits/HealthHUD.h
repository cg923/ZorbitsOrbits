// Copyright(c) 2017 Happy Rock Studios

#ifndef HEALTHHUD_H
#define HEALTHHUD_H

#include <sstream>

#include "JollyLava/HUD.h"

class ZorbitLevel;

class HealthHUD : public jl::HUD {
public:

    typedef fwk::Ptr<HealthHUD> Ptr;

    HealthHUD(ZorbitLevel * level, sf::Vector2f pos);

    void update();

    void scale(sf::Vector2f scaleFactor);

    void draw();

    void setSpriteToZero();

private:
    sf::Text                _lives;
    sf::Sprite              _livesSprite;
    jl::TextureDesc::Ptr    _livesDesc;
};

#endif



