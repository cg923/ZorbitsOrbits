// Copyright 2017 Happy Rock Studios

#ifndef CINEMATIC_H
#define CINEMATIC_H

#include "JollyLava/State.h"
#include "JollyLava/TextureManager.h"

class Game;

class Cinematic : public jl::State
{
public:
    Cinematic(std::string name,
              jl::Game* game,
              std::string filePath,
              std::string returnName = "None",
              double duration = -1.0);

    void initializeConcrete();

    void processInput(sf::Event event);

    void update();

    void draw();

    void fadeOut();

    void fadeIn();

    void pauseConcrete();

    void unpauseConcrete();

    void pauseChronometers();

    void resumeChronometers();

    void finish();

private:
    jl::TextureDesc::Ptr    _desc;
    sf::Sprite              _sprite;

    std::string     _filePath;
    double          _duration;
    bool            _waitForButtonPress;
    std::string     _returnName;

    bool                    _fadeOut;
    bool                    _fadeIn;
    sftools::Chronometer    _fadeTimer;

};
#endif // CINEMATIC_H
