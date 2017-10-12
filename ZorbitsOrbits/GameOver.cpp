// Copyright (c) 2017 Happy Rock Studios

#include <sstream>
#include <string>

#include "ZorbitsOrbits.h"
#include "GameOver.h"

GameOver::GameOver(ZorbitsOrbits * game) : Screen("GameOver", game)
{
}

void GameOver::initializeConcrete()
{
    game()->soundMan()->playMusic("gameover", false);
    _gameOverDesc   = game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/gameover.png", sf::Vector2f(400, 300), 800, 600);
    _gameOverSprite.setTexture(*_gameOverDesc->texture());
    _gameOverSprite.setOrigin(_gameOverDesc->origin());
    _gameOverSprite.setTextureRect(_gameOverDesc->rect());
    _gameOverSprite.setPosition(game()->renderWindow()->getView().getCenter().x,
                             game()->renderWindow()->getView().getCenter().y);
    _gameOverSprite.setColor(sf::Color(255,255,255,0));

    _ending = false;
    _finished = false;
    _timer.resume();
}

void GameOver::reset()
{
    _timer.reset(true);
    _ending = false;
    _finished = false;
}

void GameOver::processInput(sf::Event sfevent)
{
    // Keyboard
    if (sfevent.type == sf::Event::KeyPressed)
    {
        if (_gameOverSprite.getColor().a == 255)
            _ending = true;
    }
    // Joystick
    if (sfevent.type == sf::Event::JoystickButtonPressed)
    {
        if (_gameOverSprite.getColor().a == 255)
            _ending = true;
    }
}

void GameOver::update()
{
    if(!_ending)
    {
        if (_gameOverSprite.getColor().a <= 252)
            _gameOverSprite.setColor(sf::Color(255,255,255,_gameOverSprite.getColor().a+3));

    }
    if(_ending)
    {
        if (_gameOverSprite.getColor().a > 0)
            _gameOverSprite.setColor(sf::Color(255,255,255,_gameOverSprite.getColor().a-3));
        else _finished = true;
    }

    if(_finished)
    {
        game()->stateMan()->activeStateIs("mainmenu");
        game()->settings()->difficultyIs(game()->settings()->difficulty());
        game()->soundMan()->playMusic("zorbit");
        static_cast<ZorbitsOrbits*>(game())->gameIsOver(false);
    }
}

void GameOver::draw()
{
    game()->renderWindow()->draw(_gameOverSprite);
}

void GameOver::pauseConcrete()
{
    _timer.pause();
}

void GameOver::unpauseConcrete()
{
    _timer.resume();
}

