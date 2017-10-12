// Copyright(c) 2017 Happy Rock Studios

#include "BossHUD.h"
#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "Zorbit.h"
#include "ZBoss.h"

BossHUD::BossHUD(std::string bossName, ZorbitLevel * level, sf::Vector2f pos) : jl::HUD("BossHealth", level, pos)
{
    _visible = false;
    _bossName = bossName;
    _currentScale = 1;

    _portraitDesc = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/hud/portraits.png",
            sf::Vector2f(249, 0), 800, 800);

    // Determine boss name and portrait
    if(bossName.compare("TutorialBoss") == 0)
    {
        _nameText.setString("Big Glob");
        _portraitSprite.setTextureRect(sf::IntRect(0, 0, 99, 99));
    }
    else if(bossName.compare("VexBoss") == 0)
    {
        _nameText.setString("Vex");
        _portraitSprite.setTextureRect(sf::IntRect(100, 0, 99, 99));
    }
    else if(bossName.compare("FunzoBoss") == 0)
    {
        _nameText.setString("Funzo");
        _portraitSprite.setTextureRect(sf::IntRect(200, 0, 99, 99));
    }
    else if(bossName.compare("ElvisBoss") == 0)
    {
        _nameText.setString("Elvix");
        _portraitSprite.setTextureRect(sf::IntRect(300, 0, 99, 99));
    }
    else if(bossName.compare("StrongoBoss") == 0)
    {
        _nameText.setString("Strongo");
        _portraitSprite.setTextureRect(sf::IntRect(0, 100, 99, 99));
    }
    else if(bossName.compare("KrawlurBoss") == 0)
    {
        _nameText.setString("Krawlur");
        _portraitSprite.setTextureRect(sf::IntRect(100, 100, 99, 99));
    }
    else if(bossName.compare("GlobBoss") == 0)
    {
        _nameText.setString("King Globulon");
        _portraitSprite.setTextureRect(sf::IntRect(200, 100, 99, 99));
    }
    //else throw fwk::InternalException("Invalid boss name");

    _textureDesc = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/hud/bosshealth.png",
            sf::Vector2f(298, 0), 597, 63); // 298, 31

    _sprite.setTexture(*_textureDesc->texture());
    _sprite.setOrigin(_textureDesc->origin());
    _sprite.setTextureRect(sf::IntRect(0, 0, 597, 34));

    _healthSprite.setTexture(*_textureDesc->texture());
    _healthSprite.setOrigin(_textureDesc->origin());
    _healthSprite.setTextureRect(sf::IntRect(0, 34, 498, 29));
    _healthSprite.setPosition(_displayPosition + sf::Vector2f(94, 2));

    _portraitSprite.setTexture(*_portraitDesc->texture());
    _portraitSprite.setOrigin(_portraitDesc->origin());

    _nameText.setFont(*this->level()->game()->font("foo"));
    _nameText.setCharacterSize(60);
    _nameText.setFillColor(sf::Color::Yellow);
    _nameText.setOutlineColor(sf::Color::Black);
    _nameText.setOutlineThickness(3);
    _nameText.setPosition(displayPosition().x - 62, displayPosition().y - 44);
    //_nameText.setString(_bossName);
}

void BossHUD::update()
{
    HUD::update();

    _portraitSprite.setPosition(_displayPosition + sf::Vector2f(-24, -40));

    if(level()->entityMan()->entity(_bossName)
       && static_cast<ZBoss*>(level()->entityMan()->entity(_bossName).ptr())->currentHealth() <= 0)
       _visible = false;
}

void BossHUD::reset()
{
    HUD::reset();

    _visible = false;
    _healthSprite.setTextureRect(sf::IntRect(0, 34, 498, 29));
}

void BossHUD::updateHealth()
{
    if(level()->entityMan()->entity(_bossName)
       && static_cast<ZBoss*>(level()->entityMan()->entity(_bossName).ptr())->currentHealth() > 0)
    {
        //float increment = 498 / static_cast<ZBoss*>(level()->entityMan()->entity(_bossName).ptr())->maxHealth();
        sf::IntRect currentRect = _healthSprite.getTextureRect();

        _healthSprite.setTextureRect(sf::IntRect(currentRect.left,
                                                currentRect.top,
                                                498 * static_cast<ZBoss*>(level()->entityMan()->entity(_bossName).ptr())->currentHealth() / static_cast<ZBoss*>(level()->entityMan()->entity(_bossName).ptr())->maxHealth(),
                                                //currentRect.width - increment, // 498 * incremenet
                                                currentRect.height));
    }
}

void BossHUD::draw()
{
    HUD::draw();
    level()->game()->renderWindow()->draw(_healthSprite);
    level()->game()->renderWindow()->draw(_nameText);
    level()->game()->renderWindow()->draw(_portraitSprite);
}




