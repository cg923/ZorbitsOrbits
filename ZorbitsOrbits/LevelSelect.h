// Copyright (c) 2017 Happy Rock Studios

#ifndef LEVEL_SELECT_H
#define LEVEL_SELECT_H

#include "JollyLava/Screen.h"

class ZorbitsOrbits;

enum LevelName_SelectScreen
{
    none_s,
    tutorial_s,
    highway_s,
    wondercog_s,
    vegas_s,
    junkyard_s,
    jungle_s,
    roboruins_s,
    boss_s,
    frog_s
};

struct LevelStats
{
    std::string                 name;
    LevelName_SelectScreen      id;
    std::string                 hardestDifficultyCompleted;
    int                         fastestTimeMinutes;
    int                         fastestTimeSeconds;
    int                         fastestTimeMilliseconds;
    int                         cogsFound;
    int                         totalCogs;
    bool                        happyRockFound;
};

/**
 * @brief Sreen where the player can select a level.
 * */
class LevelSelectScreen : public jl::Screen
{
public:

    typedef fwk::Ptr<LevelSelectScreen> Ptr;

    /**
     * @param game The game for which this is the level select menu.
     * */
    LevelSelectScreen(ZorbitsOrbits * game);
    void initializeConcrete();
    void reset();
    void load();
    void save();
    void play();

    // -------------------------------------------------
    void processInput(sf::Event sfevent);
    void moveLeft();
    void moveRight();
    void update();

    // -------------------------------------------------
    void highlightBeatenLevels();
    void unhighlightUnbeatenLevels();
    sf::Vector2f scale() const { return _scale; }
    void draw();

    // -------------------------------------------------

    LevelStats loadStats(std::string levelName);
    void resetStats(std::string levelName);
    void saveStats(std::string levelName);

    // --------------------------------------------------

    void setStat(std::string levelName, std::string stat, std::string value);
    void setDifficultyIfBetter(std::string levelName, std::string newDif);
    void setCogsIfBetter(std::string levelName, int cogs);
    void setTimeIfFaster(std::string levelName, std::string value);
	int totalCogs(std::string levelName);

    // ---------------------------------------------------

    std::string generateDifficultyText(std::string levelName);
    std::string generateTimeText(std::string levelName);
    std::string generateTimeText2(std::string levelName);
    std::string generateCogsText(std::string levelName);
    std::string generateHrText(std::string levelName);
    int determineLastLevelBeaten();

    // ---------------------------------------------------

    void unlockAllLevels();
    int totalCogsCollected();
	int totalHappyRocksCollected();

    // ---------------------------------------------------

    void pauseConcrete() {}
    void unpauseConcrete() {}

    void powerDown();

    void printStats();
    void printStats(std::string levelName);

private:
    friend class ScoreScreen;

    std::map<std::string, LevelStats>   _levelStats;
    std::map<std::string, int>          _cogNumbers;
    int                                 _currentLevel;
    int                                 _lastLevelPlayed;
    int                                 _lastLevelBeaten;
    bool                                _moveLeft;
    bool                                _moveRight;
    int                                 _mLCounter;
    int                                 _mRCounter;
    int                                 _nextX;
    int                                 _previousX;
    bool                                _canSelect;
    bool                                _chooseDifficulty;
    bool                                _storyMode;

    sf::Text    _levelNameText;
    sf::Text    _difficultyText;
    sf::Text    _timeText;
    sf::Text    _cogsText;
    sf::Text    _hrText;
    sf::Text    _confirmText;
    sf::Text    _backText;
    sf::Text    _storyModeText;
    sf::Text    _frogText;
    sf::Text    _curDifficulty;
    sf::Text    _diffChooseText;

    sf::Vector2f            _scale;

    jl::TextureDesc::Ptr     _arrowDesc;
    sf::Sprite              _leftArrowSprite;
    sf::Sprite              _rightArrowSprite;
    sf::Sprite              _leftArrowSprite2;
    sf::Sprite              _rightArrowSprite2;

    jl::TextureDesc::Ptr    _buttonsDesc;
    sf::Sprite              _confirmSprite;
    sf::Sprite              _backSprite;
    sf::Sprite              _storyModeSprite;

    jl::TextureDesc::Ptr    _tutScreenDesc;
    sf::Sprite              _tutScreenSprite;
    jl::TextureDesc::Ptr    _higScreenDesc;
    sf::Sprite              _higScreenSprite;
    jl::TextureDesc::Ptr    _wonScreenDesc;
    sf::Sprite              _wonScreenSprite;
    jl::TextureDesc::Ptr    _junkScreenDesc;
    sf::Sprite              _junkScreenSprite;
    jl::TextureDesc::Ptr    _jungScreenDesc;
    sf::Sprite              _jungScreenSprite;
    jl::TextureDesc::Ptr    _vegScreenDesc;
    sf::Sprite              _vegScreenSprite;
    jl::TextureDesc::Ptr    _robScreenDesc;
    sf::Sprite              _robScreenSprite;
    jl::TextureDesc::Ptr    _bosScreenDesc;
    sf::Sprite              _bosScreenSprite;
    jl::TextureDesc::Ptr    _frogScreenDesc;
    sf::Sprite              _frogScreenSprite;
    jl::TextureDesc::Ptr    _frogDesc;
    sf::Sprite              _frogSprite;

    jl::TextureDesc::Ptr    _blackDesc;
    sf::Sprite              _blackSprite;

    jl::TextureDesc::Ptr    _unlockDesc;
    sf::Sprite              _unlockSprite;

    jl::TextureDesc::Ptr    _bgDesc;
    sf::Sprite              _bgSprite;

    std::string encrypt(std::string toEncrypt);
};

#endif


