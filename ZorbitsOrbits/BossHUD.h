// Copyright(c) 2017 Happy Rock Studios

#ifndef BOSSHUD_H
#define BOSSHUD_H

#include <sstream>

#include "JollyLava/HUD.h"

class ZorbitLevel;
class ZBoss;

class BossHUD : public jl::HUD {
public:

    typedef fwk::Ptr<BossHUD> Ptr;

    BossHUD(std::string bossName, ZorbitLevel * level, sf::Vector2f pos);

    void update();

    void draw();

    void updateHealth();

    void reset();

private:
    jl::TextureDesc::Ptr _portraitDesc;
    sf::Sprite _portraitSprite;
    sf::Sprite _healthSprite;
    float _currentScale;

    std::string _bossName;
    std::map<std::string, sf::Font>   _fonts;
    sf::Text        _nameText;

};

#endif




