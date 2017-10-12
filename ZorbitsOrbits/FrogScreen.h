//Copyright 2017 Happyrock Studios

#ifndef FROG_SCREEN_H
#define FROG_SCREEN_H

#include "JollyLava/Screen.h"

class ZorbitsOrbits;

enum CheatSelection
{
    Silly,
    Fast,
    Vex
};

class FrogScreen : public jl::Screen
{
public:
    FrogScreen(ZorbitsOrbits * game);

    void initializeConcrete();

    void processInput(sf::Event event);

    void nextSelection();

    void prevSelection();

    void switchActivation();

    void pauseConcrete() {}

    void unpauseConcrete() {}

    void generateFrogText();

    void update();

    void draw();

    void powerDown() {}

private:
    CheatSelection _currentSelection;

    sf::Vector2f _scale;

    sf::Sprite  _bgSprite;
    sf::Sprite  _backSprite;

    sf::Text    _cogsCollectedText;
    sf::Text    _welcomeText;
    sf::Text    _backText;
    sf::Text    _sillyText;
    sf::Text    _on1Text;
    sf::Text    _off1Text;
    sf::Text    _fastText;
    sf::Text    _on2Text;
    sf::Text    _off2Text;
    sf::Text    _vexText;
    sf::Text    _on3Text;
    sf::Text    _off3Text;
    sf::Text    _frogEText;

    int         _next;
    int         _previous;
    int         _nextX;
    int         _previousX;

    int         _cogsCollected;

};
#endif // FROG_SCREEN_H

