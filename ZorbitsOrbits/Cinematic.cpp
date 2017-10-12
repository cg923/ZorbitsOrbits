// Copyright 2017 Happy Rock Studios

#include "Cinematic.h"
#include "ZorbitsOrbits.h"
#include "LoadingScreen.h"

Cinematic::Cinematic(std::string name,
                     jl::Game* game,
                     std::string filePath,
                     std::string returnName,
                     double duration) : State(name, game),
                                        _filePath(filePath),
                                        _duration(duration),
                                        _returnName(returnName)
{
}

void Cinematic::initializeConcrete()
{
    _desc = this->game()->textureMan()->textureDescIs(_filePath,
            sf::Vector2f(0, 0), 1920, 1080);
    _sprite.setTexture(*_desc->texture());
    _sprite.setOrigin(_desc->origin());
    _sprite.setTextureRect(_desc->rect());
    _sprite.setPosition(0,0);
    _sprite.setColor(sf::Color(255,255,255,0));

    if(game()->renderWindow()->getSize().x < 1920)
        _sprite.scale(float(game()->renderWindow()->getSize().x) / 1920.0f, float(game()->renderWindow()->getSize().y) / 1080.0f);

    if(_duration <= 0) _waitForButtonPress = true;
    else _waitForButtonPress = false;

    _clock.resume();
    _fadeTimer.resume();

    _fadeOut = false;
    fadeIn();
}

void Cinematic::processInput(sf::Event event)
{
    if(!_waitForButtonPress) return;

    if (event.type == sf::Event::KeyPressed)
    {
        fadeOut();
    }
    if (event.type == sf::Event::JoystickButtonPressed)
    {
        fadeOut();
    }
}

void Cinematic::update()
{
    if(!_waitForButtonPress)
    {
        if(_clock.getElapsedTime().asSeconds() >= _duration * 4 / 5)
        {
            fadeOut();
        }
    }

    if(_fadeIn)
    {
        double tempDuration = _duration;
        if(tempDuration <= 0) tempDuration = 5;

        if(_fadeTimer.getElapsedTime().asSeconds() <= tempDuration / 5)
        {
            if(_sprite.getColor().a + 10 <= 255)
                _sprite.setColor(sf::Color(255,255,255,_sprite.getColor().a + 10));
            else _sprite.setColor(sf::Color(255,255,255,255));
        }
        else _fadeIn = false;
    }

    if(_fadeOut)
    {
        _fadeIn = false;

        double tempDuration = _duration;
        if(tempDuration <= 0) tempDuration = 5;

        if(_fadeTimer.getElapsedTime().asSeconds() <= tempDuration / 5)
        {
            if(_sprite.getColor().a - 10 >= 0)
                _sprite.setColor(sf::Color(255,255,255,_sprite.getColor().a - 10));
            else _sprite.setColor(sf::Color(255,255,255,0));
        }
        if(_sprite.getColor().a <= 0)
        {
            _fadeOut = false;
            finish();
        }
    }
}

void Cinematic::draw()
{
    game()->renderWindow()->draw(_sprite);
}

void Cinematic::fadeIn()
{
    _fadeIn = true;
    _fadeTimer.reset(true);
}

void Cinematic::fadeOut()
{
    _fadeOut = true;
    _fadeTimer.reset(true);
}

void Cinematic::pauseConcrete()
{
    pauseChronometers();
}

void Cinematic::unpauseConcrete()
{
    resumeChronometers();
}

void Cinematic::pauseChronometers()
{
    _clock.pause();
    _fadeTimer.pause();
}

void Cinematic::resumeChronometers()
{
    _clock.resume();
    _fadeTimer.resume();
}

void Cinematic::finish()
{
    if(_returnName.compare("None") == 0)
    {
        std::string nextLevel = game()->nextLevelName();
        if(nextLevel.compare("reset") != 0)
        {
            static_cast<LoadingScreen*>(game()->stateMan()->state("LoadingScreen").ptr())->levelNameIs(nextLevel);
            game()->stateMan()->queueState("LoadingScreen");
        }
        else
        {
            static_cast<LoadingScreen*>(game()->stateMan()->state("LoadingScreen").ptr())->levelNameIs("reset");
            static_cast<ZorbitsOrbits*>(game())->currentLevelIs(uninitialized);
        }
        game()->stateMan()->deactivateAndFinishState(name());
    }
    else
    {
        game()->stateMan()->activeStateIs(_returnName);
        game()->stateMan()->stateDel(name());
    }
}
