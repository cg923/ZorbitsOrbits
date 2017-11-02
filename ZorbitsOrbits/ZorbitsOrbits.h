// Copyright(c) 2017 Happy Rock Studios

#ifndef ZORBITSORBITS_H
#define ZORBITSORBITS_H

// JollyLava
#include "JollyLava/Game.h"

class CSteamAchievements;
class LevelSelectScreen;

enum LevelName
{
    uninitialized,
    tutorial,
    highway,
    wondercog,
    vegas,
    junkyard,
    jungle,
    roboruins,
    boss,
    cinematic
};

enum GameModes
{
    classic,
    level_select
};

enum Status
{
    playing,
    not_playing
};

enum EAchievements { 
	ACH_ALL_HAPPY = 0, 
	ACH_ALL_COGS = 1, 
	ACH_EASY = 2,
	ACH_MEDIUM = 3, 
	ACH_HARD = 4,
	ACH_VERY_HARD = 5,
	ACH_INSANE = 6,
}; 

class ZorbitsOrbits : public jl::Game
{
public:

    ZorbitsOrbits(): jl::Game("Zorbits Orbits", sf::VideoMode::getDesktopMode(), 60.0f){}

    void initializeStates();

	void gameLoopConcrete();

    void tearDownConcrete();

    void loadNextLevel() {}

    void setLevelsForPlay()
    {
        //_levelNames.push_back("tc1");
        //_levelNames.push_back("tc2");
        //_levelNames.push_back("tc3");
        //_levelNames.push_back("tc4");
        //_levelNames.push_back("tc5");
        //_levelNames.push_back("tc6");
        _levelNames.push_back("tutorial");
        //_levelNames.push_back("hc1");
        //_levelNames.push_back("hc2");
        //_levelNames.push_back("hc3");
        //_levelNames.push_back("hc4");
        _levelNames.push_back("highway");
        _levelNames.push_back("wondercog");
        _levelNames.push_back("vegas");
        _levelNames.push_back("junkyard");
        _levelNames.push_back("jungle");
        //_levelNames.push_back("jc1");
        //_levelNames.push_back("jc2");
        //_levelNames.push_back("jc3");
        //_levelNames.push_back("jc4");
        _levelNames.push_back("roboruins");
        //_levelNames.push_back("rc1");
        //_levelNames.push_back("rc2");
        //_levelNames.push_back("rc3");
        //_levelNames.push_back("rc4");
        //_levelNames.push_back("rc5");
        //_levelNames.push_back("rc6");
        //_levelNames.push_back("rc7");
        _levelNames.push_back("boss");
        //_levelNames.push_back("bc1");
        //_levelNames.push_back("bc2");
        _levelNames.push_back("reset");
    }

    void currentLevelIs(LevelName input) { _currentLevel = input; }

    void clearLevelNames() { _levelNames.clear();   }

	void setAchievement(std::string achievement);

    void foundHR(std::string level) { _happyRocks[level] = true; }

    void unFoundHR(std::string level) { _happyRocks[level] = false;}

    void currentHealthIs(int health) { _currentHealth = health; }

    int currentHealth() const { return _currentHealth; }

    bool vSyncIs(bool input)
    {
        _verticalSync = input;
        _rw->setVerticalSyncEnabled(input);
    }

    bool loseLife()
    {
        if(settings()->difficulty().compare("Easy") == 0) // Infinite lives.
            return true;
        if(settings()->difficulty().compare("Very Hard") == 0)
            return false;
        if(settings()->difficulty().compare("Insane") == 0)
            return false;

        settings()->currentLivesIs(settings()->currentLives() - 1);

        if(settings()->currentLives() == 0)
            return false;

        return true;
    }

    void gameIsOver(bool value);

    LevelSelectScreen* levelSelectScreen();

    // Game-wide variables
    LevelName _currentLevel;

    bool gameOver() const { return _gameOver; }

    GameModes currentGameMode() const { return _currentGameMode; }

    void currentGameModeIs(GameModes mode) { _currentGameMode = mode; }

    Status currentStatus() const { return _currentStatus; }

    void currentStatusIs(Status status) { _currentStatus = status; }

    // Cheats
    bool                                _godMode;
    bool                                _sillyMode;
    bool                                _fastMode;
    bool                                _vex;

private:
    bool                                _gameOver;
	int									_currentCogs;
	const int							TOTAL_COGS = 887;
    int                                 _currentHealth;
    std::map<std::string, bool>         _happyRocks;
    GameModes                           _currentGameMode;
    Status                              _currentStatus;
	CSteamAchievements*					_achievements;
};

#endif

