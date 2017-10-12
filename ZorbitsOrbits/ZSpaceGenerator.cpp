// Copyright (c) 2017 Happy Rock Studios

#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "ZSpaceGenerator.h"

ZSpaceGenerator::ZSpaceGenerator(jl::State * level) : _level(level)
{
        // Create textures for various space objects for this level
    _star1Tex = _level->game()->textureMan()->textureDescIs("resources/zorbitsorbits/space/star1.png",
                                                            sf::Vector2f(16, 16), 32, 32);

    _star2Tex = _level->game()->textureMan()->textureDescIs("resources/zorbitsorbits/space/star2.png",
                                                            sf::Vector2f(16, 16), 32, 32);

    _star3Tex = _level->game()->textureMan()->textureDescIs("resources/zorbitsorbits/space/star3.png",
                                                            sf::Vector2f(25, 25), 50, 50);

    _star4Tex = _level->game()->textureMan()->textureDescIs("resources/zorbitsorbits/space/star4.png",
                                                            sf::Vector2f(12, 12), 25, 25);

    if(!static_cast<ZorbitsOrbits*>(_level->game())->_sillyMode)
    {
    _planet1Tex = _level->game()->textureMan()->textureDescIs("resources/zorbitsorbits/space/planet1.png",
                                                                sf::Vector2f(140, 107), 281, 214);

    _planet2Tex = _level->game()->textureMan()->textureDescIs("resources/zorbitsorbits/space/planet2.png",
                                                                sf::Vector2f(124, 82), 248, 165);

    _planet3Tex = _level->game()->textureMan()->textureDescIs("resources/zorbitsorbits/space/planet3.png",
                                                                sf::Vector2f(400, 300), 800, 600);

    _planet4Tex = _level->game()->textureMan()->textureDescIs("resources/zorbitsorbits/space/planet4.png",
                                                                sf::Vector2f(75, 45), 150, 91);

    _planet5Tex = _level->game()->textureMan()->textureDescIs("resources/zorbitsorbits/space/planet5.png",
                                                                sf::Vector2f(221, 181), 442, 362);
    }
    else
    {
    _planet1Tex = _level->game()->textureMan()->textureDescIs("resources/zorbitsorbits/space/p1s.png",
                                                                sf::Vector2f(170, 256   ), 341, 512);

    _planet2Tex = _level->game()->textureMan()->textureDescIs("resources/zorbitsorbits/space/p2s.png",
                                                                sf::Vector2f(122, 122), 244, 244);

    _planet3Tex = _level->game()->textureMan()->textureDescIs("resources/zorbitsorbits/space/p3s.png",
                                                                sf::Vector2f(284, 192), 568, 385);

    _planet4Tex = _level->game()->textureMan()->textureDescIs("resources/zorbitsorbits/space/p4s.png",
                                                                sf::Vector2f(322, 135), 466, 271);

    _planet5Tex = _level->game()->textureMan()->textureDescIs("resources/zorbitsorbits/space/p5s.png",
                                                                sf::Vector2f(450, 211), 900, 423);
    }

    _planet6Tex = _level->game()->textureMan()->textureDescIs("resources/zorbitsorbits/space/planet6.png",
                                                                sf::Vector2f(320, 181), 640, 362);

    _planet7Tex = _level->game()->textureMan()->textureDescIs("resources/zorbitsorbits/space/planet7.png",
                                                                sf::Vector2f(320, 181), 640, 362);

    _planet8Tex = _level->game()->textureMan()->textureDescIs("resources/zorbitsorbits/space/planet8.png",
                                                                sf::Vector2f(360, 288), 720, 576);

    _planet9Tex = _level->game()->textureMan()->textureDescIs("resources/zorbitsorbits/space/planet9.png",
                                                                sf::Vector2f(1000, 1000), 2000, 2000);

    _planet10Tex = _level->game()->textureMan()->textureDescIs("resources/zorbitsorbits/space/planet10.png",
                                                                sf::Vector2f(1000, 1000), 2000, 2000);

    _planet11Tex = _level->game()->textureMan()->textureDescIs("resources/zorbitsorbits/space/planet11.png",
                                                                sf::Vector2f(1000, 1000), 2000, 2000);
    // Generate sprites randomly from these textures.

    srand(time(0));

    int stars = rand() % 2000 + 3000;  // Generate between 500 and 700 stars

    for(int iii = 1; iii < stars; iii++)
    {
        int whichStar = rand() % 4 + 1;
        sf::Sprite sprite;

        switch(whichStar)
        {
            case 1:
                sprite.setTexture(*_star1Tex->texture());
                sprite.setOrigin(_star1Tex->origin());
                sprite.setTextureRect(_star1Tex->rect());
                break;
            case 2:
                sprite.setTexture(*_star2Tex->texture());
                sprite.setOrigin(_star2Tex->origin());
                sprite.setTextureRect(_star2Tex->rect());
                break;
            case 3:
                sprite.setTexture(*_star3Tex->texture());
                sprite.setOrigin(_star3Tex->origin());
                sprite.setTextureRect(_star3Tex->rect());
                break;
            case 4:
                sprite.setTexture(*_star4Tex->texture());
                sprite.setOrigin(_star4Tex->origin());
                sprite.setTextureRect(_star4Tex->rect());
                break;
            default:
                break;
        }

        sprite.setPosition(rand() % 30000 - 5000, rand() % 30000 - 5000);
        sprite.setRotation(rand() % 360);
        float scale = rand() % 1 + 0.5f;
        sprite.scale(scale, scale);

        _sprites.push_back(sprite);
    }

    // Planets, etc.
    int planets = rand() % 10 + 20;  // Generate between 10 and 15 planets

    for(int iii = 1; iii < planets; iii++)
    {
        int whichPlanet = rand() % 11 + 1;
        sf::Sprite sprite2;

        switch(whichPlanet)
        {
            case 1:
                sprite2.setTexture(*_planet1Tex->texture());
                sprite2.setOrigin(_planet1Tex->origin());
                sprite2.setTextureRect(_planet1Tex->rect());
                break;
            case 2:
                sprite2.setTexture(*_planet2Tex->texture());
                sprite2.setOrigin(_planet2Tex->origin());
                sprite2.setTextureRect(_planet2Tex->rect());
                break;
            case 3:
                sprite2.setTexture(*_planet3Tex->texture());
                sprite2.setOrigin(_planet3Tex->origin());
                sprite2.setTextureRect(_planet3Tex->rect());
                break;
            case 4:
                sprite2.setTexture(*_planet4Tex->texture());
                sprite2.setOrigin(_planet4Tex->origin());
                sprite2.setTextureRect(_planet4Tex->rect());
                break;
            case 5:
                sprite2.setTexture(*_planet5Tex->texture());
                sprite2.setOrigin(_planet5Tex->origin());
                sprite2.setTextureRect(_planet5Tex->rect());
                break;
            case 6:
                sprite2.setTexture(*_planet6Tex->texture());
                sprite2.setOrigin(_planet6Tex->origin());
                sprite2.setTextureRect(_planet6Tex->rect());
                break;
            case 7:
                sprite2.setTexture(*_planet7Tex->texture());
                sprite2.setOrigin(_planet7Tex->origin());
                sprite2.setTextureRect(_planet7Tex->rect());
                break;
            case 8:
                sprite2.setTexture(*_planet8Tex->texture());
                sprite2.setOrigin(_planet8Tex->origin());
                sprite2.setTextureRect(_planet8Tex->rect());
                break;
            case 9:
                sprite2.setTexture(*_planet9Tex->texture());
                sprite2.setOrigin(_planet9Tex->origin());
                sprite2.setTextureRect(_planet9Tex->rect());
                break;
            case 10:
                sprite2.setTexture(*_planet10Tex->texture());
                sprite2.setOrigin(_planet10Tex->origin());
                sprite2.setTextureRect(_planet10Tex->rect());
                break;
            case 11:
                sprite2.setTexture(*_planet11Tex->texture());
                sprite2.setOrigin(_planet11Tex->origin());
                sprite2.setTextureRect(_planet11Tex->rect());
                break;
            default:
                break;
        }

        sprite2.setPosition(rand() % 20000 - 500, rand() % 20000 - 500);
        sprite2.setRotation(rand() % 360);
        float scale = rand() % 1 + 1.5f;
        sprite2.scale(scale, scale);

        _sprites.push_back(sprite2);
    }
}

void ZSpaceGenerator::draw()
{
    if(_level->name().compare("Credits") !=0 )
    {
        sf::View view = static_cast<ZorbitLevel*>(_level)->camera()->getView();
        _level->game()->renderWindow()->setView(view);
    }

    std::list<sf::Sprite>::iterator itr;
    for(itr = _sprites.begin(); itr != _sprites.end(); itr++)
    {
       _level->game()->renderWindow()->draw(*itr);
    }
}
