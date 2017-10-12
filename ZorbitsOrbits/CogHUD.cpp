// Copyright(c) 2017 Happy Rock Studios

#include "CogHUD.h"
#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "Zorbit.h"

CogHUD::CogHUD(ZorbitLevel * level, sf::Vector2f pos) : jl::HUD("ZorbitCog",
            level, pos)
{

    _textureDesc = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/hud/cog.png",
            sf::Vector2f(0, 0), 415, 71);

    _sprite.setTexture(*_textureDesc->texture());
    _sprite.setOrigin(_textureDesc->origin());
    _sprite.setTextureRect(_textureDesc->rect());

    _cogCount = 0;
    _scale = sf::Vector2f(1,1);

    // Cog count:
    _cogText.setFont(*this->level()->game()->font("foo"));
    _cogText.setCharacterSize(60);
    _cogText.setColor(sf::Color::Yellow);
    _cogText.setPosition(displayPosition().x + 68, displayPosition().y);

    std::stringstream ss;
    ss << _cogCount;
    _cogText.setString(ss.str());

    // Timer:
    _timerText.setFont(*this->level()->game()->font("foo"));
    _timerText.setCharacterSize(40);
    _timerText.setColor(sf::Color::Yellow);
    _timerText.setPosition(displayPosition().x + 20, displayPosition().y + 6);

    ss.str("");
    ss << timeString(static_cast<ZorbitLevel*>(this->level())->levelTime());
    _timerText.setString(ss.str());
}

void CogHUD::update()
{
    HUD::update();

    _cogCount = static_cast<Zorbit*>(level()->entityMan()->entity("Zorbit").ptr())->currentCogs();

    if(_cogCount < 10)
    {
        _cogText.setPosition(displayPosition().x + 276*_scale.x, displayPosition().y - 1);
    }
    else if (_cogCount < 100)
    {
        _cogText.setPosition(displayPosition().x + 250*_scale.x, displayPosition().y - 1);
    }
    else
    {
        _cogText.setPosition(displayPosition().x + 224*_scale.x, displayPosition().y - 1);
    }

    std::stringstream ss;
    ss << _cogCount;
    _cogText.setString(ss.str());

    ss.str("");
    ss << timeString(static_cast<ZorbitLevel*>(this->level())->levelTime());
    _timerText.setString(ss.str());
}

void CogHUD::scale(sf::Vector2f scaleFactor)
{
    _scale = scaleFactor;
    HUD::scale(scaleFactor);
    _cogText.scale(scaleFactor);
    _timerText.scale(scaleFactor);
}

void CogHUD::draw()
{
    HUD::draw();
    level()->game()->renderWindow()->draw(_cogText);
    level()->game()->renderWindow()->draw(_timerText);
}

// using a template in case we receive sf::Time or an sf::Time wrapper class.
template <typename t_Time>
std::string CogHUD::timeString(t_Time time)
{
    int minutes = 0;
    sf::Time minsAccrued = time;

    while(int(minsAccrued.asSeconds()) >= 60)
    {
        minutes++;
        minsAccrued -= sf::seconds(60);
    }

    std::stringstream ss;
    int seconds = int(minsAccrued.asSeconds());
    if( seconds >= 10)
        ss << minutes << ":" << seconds;
    else
        ss << minutes << ":0" << seconds;

    if (((time.asMilliseconds() - (1000 * int(time.asSeconds()))) / 10) >= 10)
        ss << ":" << (time.asMilliseconds() - (1000 * int(time.asSeconds()))) / 10 ;
    else
        ss << ":0" << (time.asMilliseconds() - (1000 * int(time.asSeconds()))) / 10 ;

    return ss.str();
}




