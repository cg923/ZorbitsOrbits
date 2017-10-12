//Copyright 2017 Happyrock Studios

#ifndef SCORE_SCREEN_H
#define SCORE_SCREEN_H

#include "JollyLava/Chronometer.h"

class ZorbitLevel;

class ScoreScreen
{
public:
    ScoreScreen(ZorbitLevel* level);

    void setStats(std::string levelName, std::string difficulty, std::string cogs, std::string time, bool happyRock);

    void update();

    void draw();

    void finish() { _finished = true; }

    bool fadeIn() { return _fadeIn; }

    bool active() { return _active; }

    void activeIs(bool input) { _active = input; }

    bool totallyFinished() { return _totallyFinished; }

    bool isTimeARecord(std::string levelName, std::string time);

    bool isDifficultyARecord(std::string levelName, std::string difficulty);

    void powerDown() {}

private:
    sf::Sprite _newRecordSprite1;
    bool _difficultyRecord;
    sf::Sprite _newRecordSprite2;
    bool _timeRecord;
    sf::Sprite _newRecordSprite3;
    bool _cogsRecord;
    sf::Sprite _newRecordSprite4;
    bool _happyRecord;

    sf::Text                            _levelNameText;
    std::string                         _levelName;

    sf::Text                            _difficultyText;
    std::string                         _difficulty;

    sf::Text                            _cogsText;
    std::string                         _cogs;

    sf::Text                            _timeText;
    std::string                         _time;

    sf::Text                            _happyRockText;
    bool                                _happyRock;

    sf::Text                            _confirmText;
    sf::Sprite                          _confirmSprite;

    sf::Sprite                          _fadeSprite;
    sftools::Chronometer                _fadeTimer;

    bool                                _fadeIn;
    sftools::Chronometer                _fancyTimer;
    bool                                _finished;
    bool                                _active;
    bool                                _totallyFinished;

    ZorbitLevel*                        _level;
};
#endif // SCORE_SCREEN_H
