
#include <sstream>

#include "JollyLava/Tool.h"

#include "ScoreScreen.h"
#include "ZorbitLevel.h"
#include "LevelSelect.h"
#include "ZorbitsOrbits.h"

ScoreScreen::ScoreScreen(ZorbitLevel* level) : _level(level)
{
    jl::TextureDesc::Ptr buttonsDesc = _level->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/buttons.png", sf::Vector2f(100, 25), 450, 50);
    jl::TextureDesc::Ptr fadeDesc = _level->game()->textureMan()->textureDescIs("resources/zorbitsorbits/black.png", sf::Vector2f(1, 1), 1, 1);
    jl::TextureDesc::Ptr recordDesc = _level->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/record.png", sf::Vector2f(0, 0), 100, 70);

    _fadeSprite.setTexture(*fadeDesc->texture());
    _fadeSprite.setScale(_level->game()->videoMode().width, _level->game()->videoMode().height);
    _fadeSprite.setColor(sf::Color(255,255,255,0));

    _newRecordSprite1.setTexture(*recordDesc->texture());
    _newRecordSprite1.setOrigin(recordDesc->origin());
    _newRecordSprite1.setColor(sf::Color(255,255,255,0));

    _newRecordSprite2.setTexture(*recordDesc->texture());
    _newRecordSprite2.setOrigin(recordDesc->origin());
    _newRecordSprite2.setColor(sf::Color(255,255,255,0));

    _newRecordSprite3.setTexture(*recordDesc->texture());
    _newRecordSprite3.setOrigin(recordDesc->origin());
    _newRecordSprite3.setColor(sf::Color(255,255,255,0));

    _newRecordSprite4.setTexture(*recordDesc->texture());
    _newRecordSprite4.setOrigin(recordDesc->origin());
    _newRecordSprite4.setColor(sf::Color(255,255,255,0));

    _levelNameText.setFont(*_level->game()->font("foo"));
    _levelNameText.setCharacterSize(80);
    _levelNameText.setColor(sf::Color(255,255,255,0));
    _levelNameText.setString("None");

    _difficultyText.setFont(*_level->game()->font("foo"));
    _difficultyText.setCharacterSize(50);
    _difficultyText.setFillColor(sf::Color(255,255,255,0));
    _difficultyText.setOutlineColor(sf::Color(255,255,255,0));
    _difficultyText.setOutlineThickness(4);
    _difficultyText.setString("Difficulty:");
    _difficultyText.setPosition(200, 350);
    _newRecordSprite1.setPosition(_difficultyText.getPosition() + sf::Vector2f(600, 0));

    _cogsText.setFont(*_level->game()->font("foo"));
    _cogsText.setCharacterSize(50);
    _cogsText.setFillColor(sf::Color(255,255,255,0));
    _cogsText.setOutlineColor(sf::Color(255,255,255,0));
    _cogsText.setOutlineThickness(4);
    _cogsText.setString("Cogs found:");
    _cogsText.setPosition(200, 490);
    _newRecordSprite2.setPosition(_cogsText.getPosition() + sf::Vector2f(600, 0));

    _timeText.setFont(*_level->game()->font("foo"));
    _timeText.setCharacterSize(50);
    _timeText.setFillColor(sf::Color(255,255,255,0));
    _timeText.setOutlineColor(sf::Color(255,255,255,0));
    _timeText.setOutlineThickness(4);
    _timeText.setString("Time completed:");
    _timeText.setPosition(200, 420);
    _newRecordSprite3.setPosition(_timeText.getPosition() + sf::Vector2f(600, 0));

    _happyRockText.setFont(*_level->game()->font("foo"));
    _happyRockText.setCharacterSize(50);
    _happyRockText.setFillColor(sf::Color(255,255,255,0));
    _happyRockText.setOutlineColor(sf::Color(255,255,255,0));
    _happyRockText.setOutlineThickness(4);
    _happyRockText.setString("Happyrock found:");
    _happyRockText.setPosition(200, 560);
    _newRecordSprite4.setPosition(_happyRockText.getPosition() + sf::Vector2f(600, 0));

    _confirmText.setFont(*_level->game()->font("foo"));
    _confirmText.setCharacterSize(50);
    _confirmText.setFillColor(sf::Color::White);
    _confirmText.setOutlineColor(sf::Color::Black);
    _confirmText.setOutlineThickness(4);
    _confirmText.setString("Continue: ");
    _confirmText.setPosition(_level->game()->renderWindow()->getSize().x / 2 - 150, _level->game()->renderWindow()->getSize().y - 100);

    _confirmSprite.setTexture(*buttonsDesc->texture());
    _confirmSprite.setOrigin(0,0);
    _confirmSprite.setTextureRect(sf::IntRect(0,0,50,50));
    _confirmSprite.setPosition(_confirmText.getPosition().x + 250, _confirmText.getPosition().y + 10);

    _levelName  = "none";
    _difficulty = "none";
    _cogs       = "0";
    _time       = "99:99:99";
    _happyRock  = false;
    _finished   = false;
    _fadeIn     = true;
    _active     = false;
    _totallyFinished = false;

    _difficultyRecord = false;
    _timeRecord = false;
    _cogsRecord = false;
    _happyRecord = false;

    _fancyTimer.pause();

    _level->soundMan()->soundNew("score", "resources/sounds/score.wav", false);
}

void ScoreScreen::setStats(std::string levelName,
                           std::string difficulty,
                           std::string cogs,
                           std::string time,
                           bool happyRock)
{
    _levelName = levelName;

    if(_levelName.compare("tutorial") == 0)
        _levelNameText.setString("Robotropolis");
    if(_levelName.compare("highway") == 0)
        _levelNameText.setString("Cosmic Highway");
    if(_levelName.compare("wondercog") == 0)
        _levelNameText.setString("Wondercog");
    if(_levelName.compare("vegas") == 0)
        _levelNameText.setString("Vegas");
    if(_levelName.compare("junkyard") == 0)
        _levelNameText.setString("Junkyard");
    if(_levelName.compare("jungle") == 0)
        _levelNameText.setString("Jungle");
    if(_levelName.compare("roboruins") == 0)
        _levelNameText.setString("Roboruins");
    if(_levelName.compare("boss") == 0)
        _levelNameText.setString("King Globulon");

    _difficulty = difficulty;
    _cogs = cogs;
    _time = time;
    _happyRock = happyRock;

    // DIFFICULTY
    std::stringstream ss;
    if(static_cast<ZorbitsOrbits*>(_level->game())->currentGameMode() == classic)
    {
        ss << "Difficulty: " << _difficulty; // << " / " << static_cast<LevelSelectScreen*>(_level->game()->stateMan()->state("LevelSelect").ptr())->_levelStats[levelName].hardestDifficultyCompleted;
        _difficultyText.setString(ss.str());
        ss.str("");
        _difficultyRecord = isDifficultyARecord(levelName, _difficulty);
    }
    else _difficultyText.setString("Difficulty: Can only be set in story mode!");

    // COGS
    ss << "Cogs found: " << _cogs << " / " << static_cast<LevelSelectScreen*>(_level->game()->stateMan()->state("LevelSelect").ptr())->_cogNumbers[levelName];
    _cogsText.setString(ss.str());
    ss.str("");
    if(atoi(_cogs.c_str()) > static_cast<LevelSelectScreen*>(_level->game()->stateMan()->state("LevelSelect").ptr())->_levelStats[levelName].cogsFound) _cogsRecord = true;

    // TIME
    ss << "Time completed: " << _time; // << " / " << static_cast<LevelSelectScreen*>(_level->game()->stateMan()->state("LevelSelect").ptr())->generateTimeText2(levelName);
    _timeText.setString(ss.str());
    ss.str("");
    _timeRecord = isTimeARecord(levelName, _time);

    // HAPPY
    if(_happyRock) ss << "Happyrock Found: Yes!"; // / ";
    else ss << "Happyrock Found: No!"; // / ";

    //if(static_cast<LevelSelectScreen*>(_level->game()->stateMan()->state("LevelSelect").ptr())->_levelStats[levelName].happyRockFound) ss << "Yes!";
    //else ss << "No!";
    _happyRockText.setString(ss.str());
    ss.str("");
    if(_happyRock && !static_cast<LevelSelectScreen*>(_level->game()->stateMan()->state("LevelSelect").ptr())->_levelStats[levelName].happyRockFound)
    _happyRecord = true;

    if(static_cast<ZorbitsOrbits*>(_level->game())->_fastMode || static_cast<ZorbitsOrbits*>(_level->game())->_godMode)
    {
        _difficultyText.setString("Cheater!");
        _cogsText.setString("Cheater!");
        _timeText.setString("Pumpkin!");
        _happyRockText.setString("EATER!");
    }

}

bool ScoreScreen::isTimeARecord(std::string levelName, std::string time)
{
    int newMinutes = atoi(time.substr(0, time.find(":")).c_str());
    int newSeconds = atoi(time.substr(time.find(":") + 1, 2).c_str());
    int newMilliseconds = atoi(time.substr(time.rfind(":") + 1, 2).c_str());

    int minsToSave = static_cast<LevelSelectScreen*>(_level->game()->stateMan()->state("LevelSelect").ptr())->_levelStats[levelName].fastestTimeMinutes;
    int secsToSave = static_cast<LevelSelectScreen*>(_level->game()->stateMan()->state("LevelSelect").ptr())->_levelStats[levelName].fastestTimeSeconds;
    int millisToSave = static_cast<LevelSelectScreen*>(_level->game()->stateMan()->state("LevelSelect").ptr())->_levelStats[levelName].fastestTimeMilliseconds;

    if(static_cast<LevelSelectScreen*>(_level->game()->stateMan()->state("LevelSelect").ptr())->_levelStats[levelName].hardestDifficultyCompleted.compare("None") == 0)
    {
        return true;
    }
    else if (minsToSave == 0 && secsToSave == 0 && millisToSave == 0)
    {
        return true;
    }
    else if( newMinutes < minsToSave
            || (newMinutes <= minsToSave && newSeconds < secsToSave)
            || (newMinutes <= minsToSave && newSeconds <= secsToSave && newMilliseconds < millisToSave))
    {
            return true;
    }

    return false;
}

bool ScoreScreen::isDifficultyARecord(std::string levelName, std::string difficulty)
{
    std::string oldDif = static_cast<LevelSelectScreen*>(_level->game()->stateMan()->state("LevelSelect").ptr())->_levelStats[levelName].hardestDifficultyCompleted;
    std::string difToSave = difficulty;

    if(oldDif.compare("None") == 0)
        return true;

    if (oldDif.compare("Easy") == 0)
    {
        if(difToSave.compare("Easy") != 0)
            return true;
    }
    else if (oldDif.compare("Medium") == 0)
    {
        if(difToSave.compare("Hard") == 0
            || difToSave.compare("Very Hard") == 0
            || difToSave.compare("Insane") == 0)
            return true;
    }
    else if (oldDif.compare("Hard") == 0)
    {
        if(difToSave.compare("Very Hard") == 0
            || difToSave.compare("Insane") == 0)
            return true;
    }
    else if( oldDif.compare("Very Hard") == 0)
    {
        if(difToSave.compare("Insane") == 0)
            return true;
    }
    return false;
}

void ScoreScreen::update()
{
    _fancyTimer.resume();

    _levelNameText.setPosition(_level->game()->renderWindow()->getSize().x / 2 - _levelNameText.getLocalBounds().width / 2, 40);

    if(_fadeIn)
    {
        if(_fancyTimer.getElapsedTime().asSeconds() > 0.5
           && _fancyTimer.getElapsedTime().asSeconds() <= 0.6){
            _levelNameText.setFillColor(sf::Color(255,255,255,255));
            _levelNameText.setOutlineColor(sf::Color::Black);
            _level->soundMan()->playSound("score");
        }
        if(_fancyTimer.getElapsedTime().asSeconds() > 1
           && _fancyTimer.getElapsedTime().asSeconds() <= 1.1){
            _difficultyText.setFillColor(sf::Color(255,255,255,255));
            _difficultyText.setOutlineColor(sf::Color::Black);
            if(_difficultyRecord) _newRecordSprite1.setColor(sf::Color(255,255,255,255));
            _level->soundMan()->playSound("score");
        }
        if(_fancyTimer.getElapsedTime().asSeconds() > 1.5
           && _fancyTimer.getElapsedTime().asSeconds() <= 1.6){
            _timeText.setFillColor(sf::Color(255,255,255,255));
            _timeText.setOutlineColor(sf::Color::Black);
            if(_timeRecord) _newRecordSprite3.setColor(sf::Color(255,255,255,255));
            _level->soundMan()->playSound("score");
        }
        if(_fancyTimer.getElapsedTime().asSeconds() > 2
           && _fancyTimer.getElapsedTime().asSeconds() <= 2.1){
            _cogsText.setFillColor(sf::Color(255,255,255,255));
            _cogsText.setOutlineColor(sf::Color::Black);
            if(_cogsRecord) _newRecordSprite2.setColor(sf::Color(255,255,255,255));
            _level->soundMan()->playSound("score");
        }
        if(_fancyTimer.getElapsedTime().asSeconds() > 2.5
           && _fancyTimer.getElapsedTime().asSeconds() <= 2.6){
            _happyRockText.setFillColor(sf::Color(255,255,255,255));
            _happyRockText.setOutlineColor(sf::Color::Black);
            if(_happyRecord) _newRecordSprite4.setColor(sf::Color(255,255,255,255));
            _level->soundMan()->playSound("score");
        }
        if(_fancyTimer.getElapsedTime().asSeconds() > 3.5)
        {
            _fadeIn = false;
            _level->soundMan()->playSound("score");
        }
    }

    if(sf::Joystick::isConnected(0))
    {
        _confirmSprite.setTextureRect(sf::IntRect(0,0,50,50));
    }
    else
    {
        _confirmSprite.setTextureRect(sf::IntRect(300,0,50,50));
    }

    if(_finished)
    {
        if(_fadeTimer.getElapsedTime().asSeconds() > 0.1)
        {
            int a = _fadeSprite.getColor().a;
            if(a + 6 <= 255)
                _fadeSprite.setColor(sf::Color(255,255,255,a + 4));
            else _fadeSprite.setColor(sf::Color(255,255,255,255));
        }
        if(_fadeSprite.getColor().a == 255)
        {
            if(!static_cast<ZorbitsOrbits*>(_level->game())->_fastMode && !static_cast<ZorbitsOrbits*>(_level->game())->_godMode)
            {
                if(static_cast<ZorbitsOrbits*>(_level->game())->currentGameMode() == classic)
                    static_cast<ZorbitsOrbits*>(_level->game())->levelSelectScreen()->setStat(_levelName, "difficulty", _level->game()->difficulty());
                static_cast<ZorbitsOrbits*>(_level->game())->levelSelectScreen()->setStat(_levelName, "cogs", _cogs);
                static_cast<ZorbitsOrbits*>(_level->game())->levelSelectScreen()->setStat(_levelName, "fastest", _time);
                if(_happyRock) static_cast<ZorbitsOrbits*>(_level->game())->levelSelectScreen()->setStat(_levelName, "hr", "true");
                static_cast<ZorbitsOrbits*>(_level->game())->levelSelectScreen()->saveStats(_levelName);
            }
            _totallyFinished = true;

            //game()->stateMan()->queueState("LoadingScreen");
            //game()->stateMan()->deactivateAndFinishState(name());
        }
    }
    else _fadeTimer.reset(true);
}

void ScoreScreen::draw()
{
    _level->game()->renderWindow()->draw(_levelNameText);
    _level->game()->renderWindow()->draw(_difficultyText);
    if(_levelName.compare("boss") != 0) _level->game()->renderWindow()->draw(_cogsText);
    _level->game()->renderWindow()->draw(_timeText);
    if(_levelName.compare("boss") != 0) _level->game()->renderWindow()->draw(_happyRockText);
    if(!_fadeIn) _level->game()->renderWindow()->draw(_confirmText);
    if(!_fadeIn) _level->game()->renderWindow()->draw(_confirmSprite);
    _level->game()->renderWindow()->draw(_newRecordSprite1);
    _level->game()->renderWindow()->draw(_newRecordSprite2);
    _level->game()->renderWindow()->draw(_newRecordSprite3);
    _level->game()->renderWindow()->draw(_newRecordSprite4);
    _level->game()->renderWindow()->draw(_fadeSprite);
}
