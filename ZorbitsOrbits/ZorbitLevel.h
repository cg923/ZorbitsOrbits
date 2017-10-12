// Copyright(c) 2017 Happy Rock Studios

#ifndef ZORBITLEVEL_H
#define ZORBITLEVEL_H

// JollyLava
#include "JollyLava/Level.h"

class ZSpaceGenerator;
class ScoreScreen;

class ZorbitLevel : public jl::Level
{
public:

    typedef fwk::Ptr<ZorbitLevel> Ptr;

    ZorbitLevel(std::string levelName, std::string filePath, jl::Game * game, int currentHealth = 10) :
        jl::Level(levelName, game),
        _filePath(filePath),
        _checkPoint("None"),
        _ctrl(false),
        _grappleDown(false),
        _bossName(""),
        _bossPos(b2Vec2()),
        _bossAngle(0),
        _happyRock(false),
        _healthToSet(currentHealth)
    {
    }

    void reset();

    void processInputConcrete(sf::Event event);

    void handleKeyboard(sf::Event event);

    void togglePause();

    void pauseConcrete();

    void unpauseConcrete();

    std::vector<string> mapLayerNames() const
    {
        std::vector<string> layerNames;
        layerNames.push_back("Layer3");
        layerNames.push_back("Layer2");
        layerNames.push_back("Layer1");

        return layerNames;
    }

    void populate();

    void generateBoss();

    void update();

    void draw();

    void handleEvent(std::string actor, std::string action, std::string value);

    void customObjectNew(std::string name,
                        std::string type,
                        std::string layer,
                        int x,
                        int y,
                        float angle,
                        std::string extra,
                        std::string extra2,
                        std::string extra3);

    void setCheckPoint(std::string input);

    std::string checkPoint() const { return _checkPoint; }

    b2Vec2 checkPointPosition() const;

    b2Vec2 checkPointPosition(int checkpoint) const;

    void stopInput();

    void resumeInput();

    void foundHR();

    void resetHR();

    float distanceToZorbit(b2Vec2 from);

    void generateFadeTexture();

    sf::Time levelTime() { return _timer.getElapsedTime(); }

    sf::Time subTimer() { return _subTimer.getElapsedTime(); }

    void finish();

    void print();

    std::vector<string> layerNames() { return _layerNames; }

    float _boingerFactor;

    bool bullocked() const { return _bullocked; }

    void bullockedIs(bool value);

    void firstTouchIs(bool value) { _firstTouch = value; }

    void wholeLevelBeatenIs(bool value) { _wholeLevelBeaten = value; }

    void placePart(b2Vec2 position, float angle);

protected:
    void onZeroReferences() const;

private:
    sftools::Chronometer    _timer;
    sftools::Chronometer    _subTimer;
    int                     _healthToSet;

    ZSpaceGenerator*        _spaceGen;

    std::string             _filePath;
    std::string             _checkPoint;

    std::vector<string>     _layerNames;

    // Boss
    std::string             _bossName;
    b2Vec2                  _bossPos;
    float                   _bossAngle;

    // Keyboard
    bool                    _ctrl;
    bool                    _joyRight;
    bool                    _joyLeft;
    bool                    _grappleDown;

    // HR
    bool                    _happyRock;
    b2Vec2                  _happyRockPos;
    float                   _happyRockAngle;

    sf::Sprite           _resetSprite;
    bool                 _bullocked;
    bool                    _canBullock;

    bool                 _firstTouch;
    bool                 _wholeLevelBeaten;

    b2Vec2               _jungleWaterPos;

    ScoreScreen*        _scoreScreen;

    b2Body*             _partBody;
    sf::Sprite          _partSprite;
    bool                _partPlaced;
    bool                _partMove;
};

#endif


