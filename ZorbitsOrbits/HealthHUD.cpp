// Copyright(c) 2017 Happy Rock Studios

#include <sstream>

#include "HealthHUD.h"
#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "Zorbit.h"

HealthHUD::HealthHUD(ZorbitLevel * level, sf::Vector2f pos) : jl::HUD("ZorbitHealth",
            level, pos)
{
    _textureDesc = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/hud/health.png",
            sf::Vector2f(0, 0), 300, 80);

    _livesDesc = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/hud/lives.png",
            sf::Vector2f(0, 0), 48, 23);

    _sprite.setTexture(*_textureDesc->texture());
    _sprite.setOrigin(_textureDesc->origin());
    _sprite.setTextureRect(_textureDesc->rect());

    _livesSprite.setTexture(*_livesDesc->texture());
    _livesSprite.setOrigin(_livesDesc->origin());
    _livesSprite.setTextureRect(_livesDesc->rect());

    _lives.setFont(*this->level()->game()->font("foo"));
    _lives.setCharacterSize(24);
    _lives.setColor(sf::Color::Yellow);
    _lives.setPosition(_sprite.getPosition() + sf::Vector2f(178, 42));
    _livesSprite.setPosition(_sprite.getPosition() + sf::Vector2f(128, 48));
    _lives.setString("");
}

void HealthHUD::update()
{
    HUD::update();

    int spriteFrame = 10 - static_cast<Zorbit*>(level()->entityMan()->entity("Zorbit").ptr())->currentHealth();
    int sideStep = spriteFrame % 4;

    sf::IntRect rect = _sprite.getTextureRect();
    rect.left = rect.width * sideStep;

    if(spriteFrame <= 3)
        rect.top = 0;

    if(spriteFrame > 3 && spriteFrame <= 7)
        rect.top  = rect.height;

    if(spriteFrame > 7)
        rect.top = rect.height * 2;

    if(spriteFrame >= 10)
        rect.left = rect.width;

    if (static_cast<Zorbit*>(level()->entityMan()->entity("Zorbit").ptr())->currentHealth() == 0)
        rect = sf::IntRect(603, 160, 300, 80);

    _sprite.setTextureRect(rect);

    switch(level()->game()->settings()->currentLives())
    {
    case -1:
        _lives.setString("8");
        _lives.setRotation(90);
        _lives.setPosition(_sprite.getPosition() + sf::Vector2f(204, 52));
        break;
    case -2:
        _lives.setString("0");
        break;
    default:
        std::stringstream ss;
        ss << level()->game()->settings()->currentLives();
        _lives.setString(ss.str());
        break;
    }
}

void HealthHUD::scale(sf::Vector2f scaleFactor)
{
    HUD::scale(scaleFactor);

    _livesSprite.scale(scaleFactor);
    _lives.scale(scaleFactor);
}
void HealthHUD::draw()
{
    level()->game()->renderWindow()->draw(_lives);
    level()->game()->renderWindow()->draw(_livesSprite);
    HUD::draw();
}

void HealthHUD::setSpriteToZero()
{
    _sprite.setTextureRect(sf::IntRect(900, 240, 300, 80));
}



