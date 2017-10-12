// Copyright(c) 2017 Happy Rock Studios

#ifndef COGHUD_H
#define COGHUD_H

#include <sstream>

#include "JollyLava/HUD.h"

class ZorbitLevel;

class CogHUD : public jl::HUD {
public:

    typedef fwk::Ptr<CogHUD> Ptr;

    CogHUD(ZorbitLevel * level, sf::Vector2f pos);

    void update();

    void scale(sf::Vector2f scaleFactor);

    void draw();

    template <typename t_Time>
    static std::string timeString(t_Time time);

private:
    int _cogCount;

    sf::Text        _cogText;
    sf::Text        _timerText;

    sf::Vector2f _scale;
};

#endif




