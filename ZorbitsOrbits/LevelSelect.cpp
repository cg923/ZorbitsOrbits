// Copyright (c) 2017 Happy Rock Studios

#include <sstream>
#include <string>
#include <fstream>

#include "JollyLava/Tool.h"

#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "LoadingScreen.h"
#include "LevelSelect.h"

LevelSelectScreen::LevelSelectScreen(ZorbitsOrbits * game) : Screen("LevelSelect", game)
{
    initializeConcrete();
}

void LevelSelectScreen::initializeConcrete()
{
    load();
    _currentLevel    = tutorial_s;
    _lastLevelBeaten = determineLastLevelBeaten();
    _moveLeft        = false;
    _moveRight       = false;
    _canSelect       = true;
    _chooseDifficulty  = false;
    _mLCounter       = 0;
    _mRCounter       = 0;
    _nextX           = 0;
    _previousX       = 0;

    float width  = (float)game()->videoMode().width / 1920.0f;
    float height = (float)game()->videoMode().height / 1080.0f;

    _scale = sf::Vector2f(width,height);
    //if(_scale.x <= 1 && _scale.y <=1)
    //    _scale = sf::Vector2f(1.2*_scale.x, 1.2*_scale.y);

    // DESCS
    _arrowDesc      = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/arrow.png", sf::Vector2f(10, 15), 20, 30);
    _buttonsDesc    = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/buttons.png", sf::Vector2f(100, 25), 450, 50);
    _tutScreenDesc  = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/i_tut.png", sf::Vector2f(200, 300), 400, 600);
    _higScreenDesc  = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/i_hig.png", sf::Vector2f(200, 300), 400, 600);
    _wonScreenDesc  = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/i_won.png", sf::Vector2f(200, 300), 400, 600);
    _vegScreenDesc  = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/i_veg.png", sf::Vector2f(200, 300), 400, 600);
    _junkScreenDesc = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/i_junk.png", sf::Vector2f(200, 300), 400, 600);
    _jungScreenDesc = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/i_jung.png", sf::Vector2f(200, 300), 400, 600);
    _robScreenDesc  = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/i_rob.png", sf::Vector2f(200, 300), 400, 600);
    _bosScreenDesc  = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/i_bos.png", sf::Vector2f(200, 300), 400, 600);
    _frogScreenDesc = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/i_frog.png", sf::Vector2f(200, 300), 400, 600);
    _frogDesc       = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/froggie2.png", sf::Vector2f(225, 172), 451, 343);
    _blackDesc      = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/black.png", sf::Vector2f(0, 0), 1, 1);
    _unlockDesc     = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/unlock.png", sf::Vector2f(100, 150), 200, 300);
    _bgDesc         = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/background2.png", sf::Vector2f(960, 540), 1920, 1080);

    _bgSprite.setTexture(*_bgDesc->texture());
    _bgSprite.setOrigin(_bgDesc->origin());
    _bgSprite.setTextureRect(_bgDesc->rect());
    _bgSprite.setPosition(this->game()->renderWindow()->getView().getCenter());

    // SPRITES
    _blackSprite.setTexture(*_blackDesc->texture());
    _blackSprite.setOrigin(0,0);
    _blackSprite.setTextureRect(_blackDesc->rect());
    _blackSprite.scale(this->game()->renderWindow()->getSize().x / _scale.x,
                       this->game()->renderWindow()->getSize().y / _scale.y);
    _blackSprite.setPosition(sf::Vector2f(0 - abs(float(1920-this->game()->renderWindow()->getSize().x)),
                                          0 - abs(float(1080-this->game()->renderWindow()->getSize().y))));

    _leftArrowSprite.setTexture(*_arrowDesc->texture());
    _leftArrowSprite.setOrigin(_arrowDesc->origin());
    _leftArrowSprite.setTextureRect(_arrowDesc->rect());
    _leftArrowSprite.setPosition(this->game()->renderWindow()->getView().getCenter().x - 270,
                                 this->game()->renderWindow()->getView().getCenter().y - 80);
    _leftArrowSprite.setScale(-1, 1);

    _rightArrowSprite.setTexture(*_arrowDesc->texture());
    _rightArrowSprite.setOrigin(_arrowDesc->origin());
    _rightArrowSprite.setTextureRect(_arrowDesc->rect());
    _rightArrowSprite.setPosition(this->game()->renderWindow()->getView().getCenter().x + 250,
                                  this->game()->renderWindow()->getView().getCenter().y - 80);

    _leftArrowSprite2.setTexture(*_arrowDesc->texture());
    _leftArrowSprite2.setOrigin(_arrowDesc->origin());
    _leftArrowSprite2.setTextureRect(_arrowDesc->rect());
    _leftArrowSprite2.setPosition(this->game()->renderWindow()->getView().getCenter().x - 270,
                                 this->game()->renderWindow()->getView().getCenter().y - 80);
    _leftArrowSprite2.setScale(-1, 1);

    _rightArrowSprite2.setTexture(*_arrowDesc->texture());
    _rightArrowSprite2.setOrigin(_arrowDesc->origin());
    _rightArrowSprite2.setTextureRect(_arrowDesc->rect());
    _rightArrowSprite2.setPosition(this->game()->renderWindow()->getView().getCenter().x + 250,
                                  this->game()->renderWindow()->getView().getCenter().y - 80);

    _confirmSprite.setTexture(*_buttonsDesc->texture());
    _confirmSprite.setOrigin(0,0);
    _confirmSprite.setTextureRect(sf::IntRect(0,0,50,50));
    _confirmSprite.setPosition(this->game()->renderWindow()->getView().getCenter().x - 200,
                                  this->game()->renderWindow()->getView().getCenter().y + 400);

    _backSprite.setTexture(*_buttonsDesc->texture());
    _backSprite.setOrigin(0,0);
    _backSprite.setTextureRect(sf::IntRect(50,0,50,50));
    _backSprite.setPosition(this->game()->renderWindow()->getView().getCenter().x + 30,
                                  this->game()->renderWindow()->getView().getCenter().y + 400);

    _storyModeSprite.setTexture(*_buttonsDesc->texture());
    _storyModeSprite.setOrigin(0,0);
    _storyModeSprite.setTextureRect(sf::IntRect(100,0,50,50));
    _storyModeSprite.setPosition(this->game()->renderWindow()->getView().getCenter().x + 200,
                                  this->game()->renderWindow()->getView().getCenter().y + 400);
    _storyModeSprite.setColor(sf::Color(255,255,255,255));

    _tutScreenSprite.setTexture(*_tutScreenDesc->texture());
    _tutScreenSprite.setOrigin(_tutScreenDesc->origin());
    _tutScreenSprite.setTextureRect(_tutScreenDesc->rect());
    _tutScreenSprite.setPosition(this->game()->renderWindow()->getView().getCenter().x,
                                  this->game()->renderWindow()->getView().getCenter().y - 80);

    _higScreenSprite.setTexture(*_higScreenDesc->texture());
    _higScreenSprite.setOrigin(_tutScreenDesc->origin());
    _higScreenSprite.setTextureRect(_higScreenDesc->rect());
    _higScreenSprite.setPosition(this->game()->renderWindow()->getView().getCenter().x + 500,
                                  this->game()->renderWindow()->getView().getCenter().y - 80);

    _wonScreenSprite.setTexture(*_wonScreenDesc->texture());
    _wonScreenSprite.setOrigin(_tutScreenDesc->origin());
    _wonScreenSprite.setTextureRect(_wonScreenDesc->rect());
    _wonScreenSprite.setPosition(this->game()->renderWindow()->getView().getCenter().x + 1000,
                                  this->game()->renderWindow()->getView().getCenter().y - 80);

    _vegScreenSprite.setTexture(*_vegScreenDesc->texture());
    _vegScreenSprite.setOrigin(_tutScreenDesc->origin());
    _vegScreenSprite.setTextureRect(_vegScreenDesc->rect());
    _vegScreenSprite.setPosition(this->game()->renderWindow()->getView().getCenter().x + 1500,
                                  this->game()->renderWindow()->getView().getCenter().y - 80);

    _junkScreenSprite.setTexture(*_junkScreenDesc->texture());
    _junkScreenSprite.setOrigin(_tutScreenDesc->origin());
    _junkScreenSprite.setTextureRect(_junkScreenDesc->rect());
    _junkScreenSprite.setPosition(this->game()->renderWindow()->getView().getCenter().x + 2000,
                                  this->game()->renderWindow()->getView().getCenter().y - 80);

    _jungScreenSprite.setTexture(*_jungScreenDesc->texture());
    _jungScreenSprite.setOrigin(_tutScreenDesc->origin());
    _jungScreenSprite.setTextureRect(_jungScreenDesc->rect());
    _jungScreenSprite.setPosition(this->game()->renderWindow()->getView().getCenter().x + 2500,
                                  this->game()->renderWindow()->getView().getCenter().y - 80);

    _robScreenSprite.setTexture(*_robScreenDesc->texture());
    _robScreenSprite.setOrigin(_tutScreenDesc->origin());
    _robScreenSprite.setTextureRect(_robScreenDesc->rect());
    _robScreenSprite.setPosition(this->game()->renderWindow()->getView().getCenter().x + 3000,
                                  this->game()->renderWindow()->getView().getCenter().y - 80);

    _bosScreenSprite.setTexture(*_bosScreenDesc->texture());
    _bosScreenSprite.setOrigin(_tutScreenDesc->origin());
    _bosScreenSprite.setTextureRect(_bosScreenDesc->rect());
    _bosScreenSprite.setPosition(this->game()->renderWindow()->getView().getCenter().x + 3500,
                                  this->game()->renderWindow()->getView().getCenter().y - 80);

    _frogScreenSprite.setTexture(*_frogScreenDesc->texture());
    _frogScreenSprite.setOrigin(_frogScreenDesc->origin());
    _frogScreenSprite.setTextureRect(_frogScreenDesc->rect());
    _frogScreenSprite.setPosition(this->game()->renderWindow()->getView().getCenter().x + 4000,
                                  this->game()->renderWindow()->getView().getCenter().y - 80);

    _unlockSprite.setTexture(*_unlockDesc->texture());
    _unlockSprite.setOrigin(_unlockDesc->origin());
    _unlockSprite.setTextureRect(_unlockDesc->rect());
    _unlockSprite.setPosition(_higScreenSprite.getPosition());

    _frogSprite.setTexture(*_frogDesc->texture());
    _frogSprite.setOrigin(_frogDesc->origin());
    _frogSprite.setPosition(this->game()->renderWindow()->getView().getCenter().x,
                                  this->game()->renderWindow()->getView().getCenter().y);

    // TEXT
    _frogText.setFont(*game()->font("foo"));
    _frogText.setCharacterSize(30);
    _frogText.setColor(sf::Color::Black);
    _frogText.setPosition(_frogSprite.getPosition() + sf::Vector2f(-80, -150));
    _frogText.setString("Difficulty:");

    _diffChooseText.setFont(*game()->font("foo"));
    _diffChooseText.setCharacterSize(40);
    _diffChooseText.setColor(sf::Color::White);
    _diffChooseText.setPosition(_frogSprite.getPosition() + sf::Vector2f(-110, 30));
    _diffChooseText.setString("Difficulty:");

    _curDifficulty.setFont(*game()->font("foo"));
    _curDifficulty.setCharacterSize(30);
    _curDifficulty.setColor(sf::Color::White);
    _curDifficulty.setPosition(_diffChooseText.getPosition() + sf::Vector2f(20, 50));
    _curDifficulty.setString("Easy");

    _leftArrowSprite2.setPosition(_curDifficulty.getPosition().x - 20,
                                    _curDifficulty.getPosition().y + 20);
    _rightArrowSprite2.setPosition(_curDifficulty.getPosition().x + 190,
                                    _curDifficulty.getPosition().y + 20);

    _levelNameText.setFont(*game()->font("foo"));
    _levelNameText.setCharacterSize(50);
    _levelNameText.setColor(sf::Color::White);
    _levelNameText.setString("Robotropolis");
    _levelNameText.setPosition(this->game()->renderWindow()->getView().getCenter().x - _levelNameText.getGlobalBounds().width / 2,
                                this->game()->renderWindow()->getView().getCenter().y - 500);

    _difficultyText.setFont(*game()->font("foo"));
    _difficultyText.setCharacterSize(30);
    _difficultyText.setColor(sf::Color::White);
    _difficultyText.setPosition(this->game()->renderWindow()->getView().getCenter().x - 200,
                                 this->game()->renderWindow()->getView().getCenter().y + 260);
    _difficultyText.setString(generateDifficultyText("tutorial"));

    _timeText.setFont(*game()->font("foo"));
    _timeText.setCharacterSize(30);
    _timeText.setColor(sf::Color::White);
    _timeText.setPosition(this->game()->renderWindow()->getView().getCenter().x - 200,
                          this->game()->renderWindow()->getView().getCenter().y + 290);
    _timeText.setString(generateTimeText("tutorial"));

    _cogsText.setFont(*game()->font("foo"));
    _cogsText.setCharacterSize(30);
    _cogsText.setColor(sf::Color::White);
    _cogsText.setPosition(this->game()->renderWindow()->getView().getCenter().x - 200,
                          this->game()->renderWindow()->getView().getCenter().y + 320);
    _cogsText.setString(generateCogsText("tutorial"));

    _hrText.setFont(*game()->font("foo"));
    _hrText.setCharacterSize(30);
    _hrText.setColor(sf::Color::White);
    _hrText.setPosition(this->game()->renderWindow()->getView().getCenter().x - 200,
                        this->game()->renderWindow()->getView().getCenter().y + 350);
    _hrText.setString(generateHrText("tutorial"));

    _confirmText.setFont(*game()->font("foo"));
    _confirmText.setCharacterSize(40);
    _confirmText.setColor(sf::Color::White);
    _confirmText.setPosition(_confirmSprite.getPosition() + sf::Vector2f(50, 0));
    _confirmText.setString("Confirm");

    _backText.setFont(*game()->font("foo"));
    _backText.setCharacterSize(40);
    _backText.setColor(sf::Color::White);
    _backText.setPosition(_backSprite.getPosition() + sf::Vector2f(50, 0));
    _backText.setString("Back");

    _storyModeText.setFont(*game()->font("foo"));
    _storyModeText.setCharacterSize(40);
    _storyModeText.setColor(sf::Color::White);
    _storyModeText.setPosition(_storyModeSprite.getPosition() + sf::Vector2f(50, 0));
    _storyModeText.setString("Start in Story Mode");
    _storyModeText.setColor(sf::Color(255,255,255,255));

    _cogNumbers["tutorial"]     = 157;
    _cogNumbers["highway"]      = 120;
    _cogNumbers["wondercog"]    = 226;
    _cogNumbers["vegas"]        = 148;
    _cogNumbers["junkyard"]     = 73;
    _cogNumbers["jungle"]       = 40;
    _cogNumbers["roboruins"]    = 123;
    _cogNumbers["boss"]         = 0;
}

void LevelSelectScreen::reset()
{
    _moveLeft = false;
    _moveRight = false;
    _mLCounter = 0;
    _mRCounter = 0;
    _nextX           = 0;
    _previousX       = 0;
    _lastLevelBeaten = determineLastLevelBeaten();
}

void LevelSelectScreen::load()
{
    _levelStats["tutorial"]     = loadStats("tutorial");
    _levelStats["tutorial"].id  = tutorial_s;
    _levelStats["highway"]      = loadStats("highway");
    _levelStats["highway"].id   = highway_s;
    _levelStats["wondercog"]    = loadStats("wondercog");
    _levelStats["wondercog"].id = wondercog_s;
    _levelStats["vegas"]        = loadStats("vegas");
    _levelStats["vegas"].id     = vegas_s;
    _levelStats["junkyard"]     = loadStats("junkyard");
    _levelStats["junkyard"].id  = junkyard_s;
    _levelStats["jungle"]       = loadStats("jungle");
    _levelStats["jungle"].id    = jungle_s;
    _levelStats["roboruins"]    = loadStats("roboruins");
    _levelStats["roboruins"].id = roboruins_s;
    _levelStats["boss"]         = loadStats("boss");
    _levelStats["boss"].id      = boss_s;
}

void LevelSelectScreen::save()
{
    saveStats("tutorial");
    saveStats("highway");
    saveStats("wondercog");
    saveStats("vegas");
    saveStats("junkyard");
    saveStats("jungle");
    saveStats("roboruins");
    saveStats("boss");
}

void LevelSelectScreen::play()
{
    //clean out state man:
    std::list<std::string> dontClear;
    dontClear.push_back("LoadingScreen");
    dontClear.push_back("LevelSelect");
    dontClear.push_back("mainmenu");
    dontClear.push_back("Credits");
    game()->stateMan()->powerDownExcept(dontClear);

    if(_storyMode)
    {
        if(game()->stateMan()->state("tutorial"))
        {
            game()->stateMan()->deactivateAndFinishState("tutorial");
            //static_cast<ZorbitLevel*>(game()->stateMan()->state("tutorial").ptr())->finish();
            //game()->stateMan()->stateDel("tutorial");
        }
        game()->soundMan()->playMusic("robotropolis");
        static_cast<ZorbitsOrbits*>(game())->clearLevelNames();
        static_cast<ZorbitsOrbits*>(game())->setLevelsForPlay();
        static_cast<ZorbitsOrbits*>(game())->currentGameModeIs(classic);
        // FINAL
        //static_cast<LoadingScreen*>(game()->stateMan()->state("LoadingScreen").ptr())->levelNameIs("tc1");
        static_cast<LoadingScreen*>(game()->stateMan()->state("LoadingScreen").ptr())->levelNameIs("tutorial");
        static_cast<ZorbitsOrbits*>(game())->_currentLevel = cinematic;
        game()->settings()->currentLivesIs(game()->settings()->maxLives());
        game()->stateMan()->activeStateIs("LoadingScreen");
        return;
    }

    switch(_currentLevel)
    {
    case tutorial_s:
        game()->soundMan()->playMusic("robotropolis");
        break;
    case highway_s:
        game()->soundMan()->playMusic("vex");
        break;
    case wondercog_s:
        game()->soundMan()->playMusic("wondercog");
        break;
    case vegas_s:
        game()->soundMan()->playMusic("vegas");
        break;
    case junkyard_s:
        game()->soundMan()->playMusic("junkyard");
        break;
    case jungle_s:
        game()->soundMan()->playMusic("jungle");
        break;
    case roboruins_s:
        game()->soundMan()->playMusic("roboruins");
        break;
    case boss_s:
        game()->soundMan()->playMusic("boss");
        break;
    default:
        break;
    }

    static_cast<ZorbitsOrbits*>(game())->clearLevelNames();
    switch(_currentLevel)
    {
    case tutorial_s:
        if(game()->stateMan()->state("tutorial"))
        {
            //static_cast<ZorbitLevel*>(game()->stateMan()->state("tutorial").ptr())->finish();
            //game()->stateMan()->stateDel("tutorial");
            game()->stateMan()->deactivateAndFinishState("tutorial");
        }
        static_cast<LoadingScreen*>(game()->stateMan()->state("LoadingScreen").ptr())->levelNameIs("tutorial");
        static_cast<ZorbitsOrbits*>(game())->_currentLevel = tutorial;
        break;
    case highway_s:
        if(game()->stateMan()->state("highway"))
        {
            game()->stateMan()->deactivateAndFinishState("highway");
        }
        static_cast<LoadingScreen*>(game()->stateMan()->state("LoadingScreen").ptr())->levelNameIs("highway");
        static_cast<ZorbitsOrbits*>(game())->_currentLevel = highway;
        break;
    case wondercog_s:
        if(game()->stateMan()->state("wondercog"))
        {
            static_cast<ZorbitLevel*>(game()->stateMan()->state("wondercog").ptr())->finish();
            game()->stateMan()->stateDel("wondercog");
        }
        static_cast<LoadingScreen*>(game()->stateMan()->state("LoadingScreen").ptr())->levelNameIs("wondercog");
        static_cast<ZorbitsOrbits*>(game())->_currentLevel = wondercog;
        break;
    case vegas_s:
        if(game()->stateMan()->state("vegas"))
        {
            static_cast<ZorbitLevel*>(game()->stateMan()->state("vegas").ptr())->finish();
            game()->stateMan()->stateDel("vegas");
        }
        static_cast<LoadingScreen*>(game()->stateMan()->state("LoadingScreen").ptr())->levelNameIs("vegas");
        static_cast<ZorbitsOrbits*>(game())->_currentLevel = vegas;
        break;
    case junkyard_s:
        if(game()->stateMan()->state("junkyard"))
        {
            static_cast<ZorbitLevel*>(game()->stateMan()->state("junkyard").ptr())->finish();
            game()->stateMan()->stateDel("junkyard");
        }
        static_cast<LoadingScreen*>(game()->stateMan()->state("LoadingScreen").ptr())->levelNameIs("junkyard");
        static_cast<ZorbitsOrbits*>(game())->_currentLevel = junkyard;
        break;
    case jungle_s:
        if(game()->stateMan()->state("jungle"))
        {
            static_cast<ZorbitLevel*>(game()->stateMan()->state("jungle").ptr())->finish();
            game()->stateMan()->stateDel("jungle");
        }
        static_cast<LoadingScreen*>(game()->stateMan()->state("LoadingScreen").ptr())->levelNameIs("jungle");
        static_cast<ZorbitsOrbits*>(game())->_currentLevel = jungle;
        break;
    case roboruins_s:
        if(game()->stateMan()->state("roboruins"))
        {
            static_cast<ZorbitLevel*>(game()->stateMan()->state("roboruins").ptr())->finish();
            game()->stateMan()->stateDel("roboruins");
        }
        static_cast<LoadingScreen*>(game()->stateMan()->state("LoadingScreen").ptr())->levelNameIs("roboruins");
        static_cast<ZorbitsOrbits*>(game())->_currentLevel = roboruins;
        break;
    case boss_s:
        if(game()->stateMan()->state("boss"))
        {
            static_cast<ZorbitLevel*>(game()->stateMan()->state("boss").ptr())->finish();
            game()->stateMan()->stateDel("boss");
        }
        static_cast<LoadingScreen*>(game()->stateMan()->state("LoadingScreen").ptr())->levelNameIs("boss");
        static_cast<ZorbitsOrbits*>(game())->_currentLevel = boss;
        break;
    default:
        throw fwk::InternalException("Reached unexpected default case!");
        break;
    }

    game()->settings()->currentLivesIs(game()->settings()->maxLives());
    if(!_storyMode) static_cast<ZorbitsOrbits*>(game())->currentGameModeIs(level_select);
    game()->stateMan()->activeStateIs("LoadingScreen");
}

LevelStats LevelSelectScreen::loadStats(std::string levelName)
{
    std::ifstream in;
    std::stringstream path;
    path << "levels/" << levelName << "/stats.txt";
    std::string sPath = path.str();

    in.open(const_cast<const char*>(sPath.c_str()));

    LevelStats stats;
    stats.name = levelName;

    if (!in.is_open()) // Stat file doesn't exist.  Let's create one.
    {
        std::ofstream out;
        out.open(const_cast<const char*>(sPath.c_str()));

        out << "}7c7bAz3Wf" << std::endl;
        out << "    }qz11z\\cW7uf&bJt}pqz11z\\cW7uf" << std::endl;
        out << "    }\\bovf,}p\\bovf" << std::endl;
        out << "    }[Af13Wvt}p[Af" << std::endl;
        out << "    }13v7tv7f,:,,:,,}p13v7tv7f" << std::endl;
        out << "}TJqf";
        out.close();

        // try again
        in.open(const_cast<const char*>(sPath.c_str()));
        if(!in.is_open())
            throw fwk::InternalException("Couldn't create stat file");
    }
    if(in.is_open())
    {
        // Used for parsing Level data.
        std::string line = "";
        std::string stat;
        std::string value_;

        std::getline(in, line);

        while (line.compare("<End>") != 0)
        {
            std::getline(in, line);
            line = game()->settings()->decrypt(line);
            stat = jl::Tool::tag(line);
            value_ = jl::Tool::value(line);

            if(stat.compare("difficulty") == 0)
                stats.hardestDifficultyCompleted = value_;
            else if(stat.compare("cogs") == 0)
                stats.cogsFound = atoi(value_.c_str());
            else if(stat.compare("hr") == 0)
            {
                if (value_.compare("true") == 0)
                    stats.happyRockFound = true;
                else stats.happyRockFound = false;
            }
            else if(stat.compare("fastest") == 0)
            {
                stats.fastestTimeMinutes        = atoi(value_.substr(0, value_.find(":")).c_str());
                stats.fastestTimeSeconds        = atoi(value_.substr(value_.find(":") + 1, 2).c_str());
                stats.fastestTimeMilliseconds   = atoi(value_.substr(value_.rfind(":") + 1, 2).c_str());
            }
            else if(stat.compare("End") == 0) {}
            else throw fwk::InternalException("Invalid stat file! Replace this levels stat file with /levels/stats.txt");
        }
    }

    return stats;
}

void LevelSelectScreen::saveStats(std::string levelName)
{
    std::stringstream path;
    path << "levels/" << levelName << "/stats.txt";
    std::string indent = "    ";

    std::ofstream       _settingsOut;

    // Open the file.
    _settingsOut.open(const_cast<const char*>(path.str().c_str()));

    path.str("");
    path << "<" << levelName << ">";
    _settingsOut << encrypt(path.str()) << std::endl;
    _settingsOut << indent << encrypt("<difficulty>") << encrypt(_levelStats[levelName].hardestDifficultyCompleted) << encrypt("</difficulty>") << std::endl;
    _settingsOut << indent << encrypt("<cogs>") << encrypt(jl::Tool::toString(_levelStats[levelName].cogsFound)) << encrypt("</cogs>") << std::endl;
    if(_levelStats[levelName].happyRockFound)
        _settingsOut << indent << encrypt("<hr>") << encrypt("true") << encrypt("</hr>") << std::endl;
    else _settingsOut << indent << encrypt("<hr>") << encrypt("false") << encrypt("</hr>") << std::endl;
    _settingsOut << indent << encrypt("<fastest>") << encrypt(jl::Tool::toString(_levelStats[levelName].fastestTimeMinutes)) <<
        encrypt(":") << encrypt(jl::Tool::toString(_levelStats[levelName].fastestTimeSeconds)) <<
        encrypt(":") << encrypt(jl::Tool::toString(_levelStats[levelName].fastestTimeMilliseconds)) << encrypt("</fastest>") << std::endl;

    _settingsOut << encrypt("<End>") << std::endl;

    _settingsOut.close();
}

void LevelSelectScreen::setStat(std::string levelName, std::string stat, std::string value)
{
    if(stat.compare("difficulty") == 0)
        setDifficultyIfBetter(levelName, value);
    else if(stat.compare("cogs") == 0)
        setCogsIfBetter(levelName, atoi(value.c_str()));
    else if(stat.compare("hr") == 0)
    {
        if (value.compare("true") == 0)
            _levelStats[levelName].happyRockFound = true;
        else _levelStats[levelName].happyRockFound = false;
    }
    else if(stat.compare("fastest") == 0)
        setTimeIfFaster(levelName, value);
}

void LevelSelectScreen::setDifficultyIfBetter(std::string levelName, std::string newDif)
{
    std::string oldDif = _levelStats[levelName].hardestDifficultyCompleted;
    std::string difToSave = newDif;

    if (oldDif.compare("Easy") == 0)
    {
        if(difToSave.compare("None") == 0)
            difToSave = oldDif;
    }
    else if (oldDif.compare("Medium") == 0)
    {
        if(difToSave.compare("None") == 0
           || difToSave.compare("Easy") == 0)
            difToSave = oldDif;
    }
    else if (oldDif.compare("Hard") == 0)
    {
        if(difToSave.compare("None") == 0
           || difToSave.compare("Easy") == 0
           || difToSave.compare("Medium") == 0)
            difToSave = oldDif;
    }
    else if( oldDif.compare("Very Hard") == 0)
    {
        if(difToSave.compare("None") == 0
           || difToSave.compare("Easy") == 0
           || difToSave.compare("Medium") == 0
           || difToSave.compare("Hard") == 0)
           difToSave = oldDif;
    }
    else if (oldDif.compare("Insane")== 0)
        difToSave = oldDif;

    _levelStats[levelName].hardestDifficultyCompleted = difToSave;

}

void LevelSelectScreen::setCogsIfBetter(std::string levelName, int cogs)
{
    if (cogs > _levelStats[levelName].cogsFound)
        _levelStats[levelName].cogsFound = cogs;
}

void LevelSelectScreen::setTimeIfFaster(std::string levelName, std::string value)
{
        int newMinutes = atoi(value.substr(0, value.find(":")).c_str());
        int newSeconds = atoi(value.substr(value.find(":") + 1, 2).c_str());
        int newMilliseconds = atoi(value.substr(value.rfind(":") + 1, 2).c_str());

        int minsToSave = _levelStats[levelName].fastestTimeMinutes;
        int secsToSave = _levelStats[levelName].fastestTimeSeconds;
        int millisToSave = _levelStats[levelName].fastestTimeMilliseconds;

        if(_levelStats[levelName].hardestDifficultyCompleted.compare("None") == 0)
        {
            minsToSave = newMinutes;
            secsToSave = newSeconds;
            millisToSave = newMilliseconds;
        }
        else if (minsToSave == 0 && secsToSave == 0 && millisToSave == 0)
        {
            minsToSave = newMinutes;
            secsToSave = newSeconds;
            millisToSave = newMilliseconds;
        }
        else if( newMinutes < minsToSave
                || (newMinutes <= minsToSave && newSeconds < secsToSave)
                || (newMinutes <= minsToSave && newSeconds <= secsToSave && newMilliseconds < millisToSave))
        {
            minsToSave = newMinutes;
            secsToSave = newSeconds;
            millisToSave = newMilliseconds;
        }

        _levelStats[levelName].fastestTimeMinutes       = minsToSave;
        _levelStats[levelName].fastestTimeSeconds       = secsToSave;
        _levelStats[levelName].fastestTimeMilliseconds  = millisToSave;
}

std::string LevelSelectScreen::generateDifficultyText(std::string levelName)
{
    std::stringstream ss;
    ss << "Hardest Difficulty:  " << _levelStats[levelName].hardestDifficultyCompleted;
    return ss.str();
}

std::string LevelSelectScreen::generateTimeText(std::string levelName)
{
    std::stringstream ss;
    ss << "Fastest Time:             " << _levelStats[levelName].fastestTimeMinutes;
    if (_levelStats[levelName].fastestTimeSeconds < 10)         ss << ":0" << _levelStats[levelName].fastestTimeSeconds;
    else                                                        ss << ":" << _levelStats[levelName].fastestTimeSeconds;
    if(_levelStats[levelName].fastestTimeMilliseconds < 10)    ss << ":0" << _levelStats[levelName].fastestTimeMilliseconds;
    else                                                        ss << ":" << _levelStats[levelName].fastestTimeMilliseconds;

    return ss.str();
}

std::string LevelSelectScreen::generateTimeText2(std::string levelName)
{
    std::stringstream ss;
    ss << _levelStats[levelName].fastestTimeMinutes;
    if (_levelStats[levelName].fastestTimeSeconds < 10)         ss << ":0" << _levelStats[levelName].fastestTimeSeconds;
    else                                                        ss << ":" << _levelStats[levelName].fastestTimeSeconds;
    if(_levelStats[levelName].fastestTimeMilliseconds < 10)    ss << ":0" << _levelStats[levelName].fastestTimeMilliseconds;
    else                                                        ss << ":" << _levelStats[levelName].fastestTimeMilliseconds;

    return ss.str();
}

std::string LevelSelectScreen::generateCogsText(std::string levelName)
{
    std::stringstream ss;
    ss << "Cogs Found:               " << _levelStats[levelName].cogsFound << "/" << _cogNumbers[levelName];
    return ss.str();
}

int LevelSelectScreen::totalCogs(std::string levelName) {
	return _cogNumbers[levelName];
}

std::string LevelSelectScreen::generateHrText(std::string levelName)
{
    std::stringstream ss;
    ss << "Happy Rock Found:   ";
    if(_levelStats[levelName].happyRockFound) ss << "Yes!";
    else ss << "No!";
    return ss.str();
}

int LevelSelectScreen::determineLastLevelBeaten()
{
    int lastLevelBeaten = none_s;

    if(_levelStats["tutorial"].hardestDifficultyCompleted.compare("None") != 0)     lastLevelBeaten = tutorial_s;
    if(_levelStats["highway"].hardestDifficultyCompleted.compare("None") != 0)      lastLevelBeaten = highway_s;
    if(_levelStats["wondercog"].hardestDifficultyCompleted.compare("None") != 0)    lastLevelBeaten = wondercog_s;
    if(_levelStats["vegas"].hardestDifficultyCompleted.compare("None") != 0)        lastLevelBeaten = vegas_s;
    if(_levelStats["junkyard"].hardestDifficultyCompleted.compare("None") != 0)     lastLevelBeaten = junkyard_s;
    if(_levelStats["jungle"].hardestDifficultyCompleted.compare("None") != 0)       lastLevelBeaten = jungle_s;
    if(_levelStats["roboruins"].hardestDifficultyCompleted.compare("None") != 0)    lastLevelBeaten = roboruins_s;
    if(_levelStats["boss"].hardestDifficultyCompleted.compare("None") != 0)         lastLevelBeaten = boss_s;

    return lastLevelBeaten;
}

void LevelSelectScreen::unlockAllLevels()
{
    std::map<std::string, LevelStats>::iterator itr;
    for(itr = _levelStats.begin(); itr != _levelStats.end(); itr++)
    {
        if(itr->second.hardestDifficultyCompleted.compare("None") == 0)
            itr->second.hardestDifficultyCompleted = "Easy";
        saveStats(itr->first);
    }
}

void LevelSelectScreen::processInput(sf::Event sfevent)
{
    if(_chooseDifficulty)
    {
        // Keyboard
        if (sfevent.type == sf::Event::KeyPressed)
        {
            switch(sfevent.key.code)
            {
                case sf::Keyboard::Escape:
                    game()->soundMan()->playSound("cancel");
                    _chooseDifficulty = false;
                    break;
                case sf::Keyboard::Left:
                    game()->soundMan()->playSound("menubeep");
                    game()->settings()->prevDifficulty();
                    break;
                case sf::Keyboard::Right:
                    game()->soundMan()->playSound("menubeep");
                    game()->settings()->nextDifficulty();
                    break;
                case sf::Keyboard::Return:
                    _chooseDifficulty = false;
                    play();
                    break;
                default:
                    break;
            }
        }
        // Joystick
        if (sfevent.type == sf::Event::JoystickMoved)
        {
            // X Axis
            if (sfevent.joystickMove.axis == sf::Joystick::X)
            {
                if (sfevent.joystickMove.position > 70)
                {
                    if(_nextX == 0)
                    {
                        game()->soundMan()->playSound("menubeep");
                        game()->settings()->nextDifficulty();
                    }
                    _nextX++;
                }
                if (sfevent.joystickMove.position < -70)
                {
                    if(_previousX == 0)
                    {
                        game()->soundMan()->playSound("menubeep");
                        game()->settings()->prevDifficulty();
                    }
                    _previousX++;
                }
                if (sfevent.joystickMove.position < 40)
                {
                    _nextX = 0;

                }
                if (sfevent.joystickMove.position > -40)
                {
                    _previousX = 0;

                }
            }
        }
        if (sfevent.type == sf::Event::JoystickButtonPressed)
        {
            if(sfevent.joystickButton.button == 0)
            {
                play();
                _chooseDifficulty = false;
            }
            if(sfevent.joystickButton.button == 1)
            {
                game()->soundMan()->playSound("cancel");
                _chooseDifficulty = false;
            }
        }
    }
    else
    {
        // Keyboard
        if (sfevent.type == sf::Event::KeyPressed)
        {
            switch(sfevent.key.code)
            {
                case sf::Keyboard::Escape:
                    game()->soundMan()->playSound("cancel");
                    game()->stateMan()->activeStateIs("mainmenu");
                    break;
                case sf::Keyboard::Left:
                    if(!_moveLeft && !_moveRight)
                        moveLeft();
                    break;
                case sf::Keyboard::Right:
                    if(!_moveRight && !_moveLeft)
                        moveRight();
                    break;
                case sf::Keyboard::Return:
                    if(_currentLevel == frog_s) game()->stateMan()->activeStateIs("FrogScreen");
                    else
                    {
                        game()->soundMan()->playSound("confirm2");
                        _chooseDifficulty = true;
                        _storyMode = false;
                    }
                    break;
                case sf::Keyboard::S:
                    {
                        if(_currentLevel != tutorial_s)
                            break;
                        game()->soundMan()->playSound("confirm2");
                        _chooseDifficulty = true;
                        _storyMode = true;
                    }
                default:
                    break;
            }
        }
        // Joystick
        if (sfevent.type == sf::Event::JoystickMoved)
        {
            // X Axis
            if (sfevent.joystickMove.axis == sf::Joystick::X)
            {
                if (sfevent.joystickMove.position > 70)
                {
                    if(_nextX == 0)
                    {
                        if(!_moveRight && !_moveLeft)
                            moveRight();
                    }
                    _nextX++;
                }
                if (sfevent.joystickMove.position < -70)
                {
                    if(_previousX == 0)
                    {
                        if(!_moveLeft && !_moveRight)
                            moveLeft();
                    }
                    _previousX++;
                }
                if (sfevent.joystickMove.position < 40)
                {
                    _nextX = 0;

                }
                if (sfevent.joystickMove.position > -40)
                {
                    _previousX = 0;

                }
            }
        }
        if (sfevent.type == sf::Event::JoystickButtonPressed)
        {
            if(sfevent.joystickButton.button == 0)
            {
                if(_currentLevel == frog_s) game()->stateMan()->activeStateIs("FrogScreen");
                else
                {
                    game()->soundMan()->playSound("confirm2");
                    _chooseDifficulty = true;
                    _storyMode = false;
                }
            }
            if(sfevent.joystickButton.button == 1)
            {
                game()->soundMan()->playSound("cancel");
                game()->stateMan()->activeStateIs("mainmenu");
            }
            if(sfevent.joystickButton.button == 2)
            {
                if(_currentLevel == tutorial_s)
                {
                    game()->soundMan()->playSound("confirm2");
                    _chooseDifficulty = true;
                    _storyMode = true;
                }
            }
        }
    }
}

void LevelSelectScreen::moveLeft()
{
    if(_currentLevel == tutorial_s)
        return;

    game()->soundMan()->playSound("menubeep");
    _canSelect = false;
    _moveLeft = true;
    _mLCounter = 0;
    _currentLevel--;
}

void LevelSelectScreen::moveRight()
{
    if(_currentLevel == frog_s)
        return;

    if(_currentLevel - 1 == _lastLevelBeaten)
        return;

    game()->soundMan()->playSound("menubeep");
    _canSelect = false;
    _moveRight = true;
    _mRCounter = 0;
    _currentLevel++;
}

void LevelSelectScreen::highlightBeatenLevels()
{
    if(_levelStats["tutorial"].hardestDifficultyCompleted.compare("None")  != 0) _higScreenSprite.setColor(sf::Color(255,255,255,255));
    if(_levelStats["highway"].hardestDifficultyCompleted.compare("None")   != 0) _wonScreenSprite.setColor(sf::Color(255,255,255,255));
    if(_levelStats["wondercog"].hardestDifficultyCompleted.compare("None") != 0) _vegScreenSprite.setColor(sf::Color(255,255,255,255));
    if(_levelStats["vegas"].hardestDifficultyCompleted.compare("None")     != 0) _junkScreenSprite.setColor(sf::Color(255,255,255,255));
    if(_levelStats["junkyard"].hardestDifficultyCompleted.compare("None")  != 0) _jungScreenSprite.setColor(sf::Color(255,255,255,255));
    if(_levelStats["jungle"].hardestDifficultyCompleted.compare("None")    != 0) _robScreenSprite.setColor(sf::Color(255,255,255,255));
    if(_levelStats["roboruins"].hardestDifficultyCompleted.compare("None") != 0) _bosScreenSprite.setColor(sf::Color(255,255,255,255));
    if(_levelStats["boss"].hardestDifficultyCompleted.compare("None")      != 0) _frogScreenSprite.setColor(sf::Color(255,255,255,255));
}

void LevelSelectScreen::unhighlightUnbeatenLevels()
{
    if(_levelStats["highway"].hardestDifficultyCompleted.compare("None")   == 0) _higScreenSprite.setColor(sf::Color(255,255,255,80));
    if(_levelStats["wondercog"].hardestDifficultyCompleted.compare("None") == 0) _wonScreenSprite.setColor(sf::Color(255,255,255,80));
    if(_levelStats["vegas"].hardestDifficultyCompleted.compare("None")     == 0) _vegScreenSprite.setColor(sf::Color(255,255,255,80));
    if(_levelStats["junkyard"].hardestDifficultyCompleted.compare("None")  == 0) _junkScreenSprite.setColor(sf::Color(255,255,255,80));
    if(_levelStats["jungle"].hardestDifficultyCompleted.compare("None")    == 0) _jungScreenSprite.setColor(sf::Color(255,255,255,80));
    if(_levelStats["roboruins"].hardestDifficultyCompleted.compare("None") == 0) _robScreenSprite.setColor(sf::Color(255,255,255,80));
    if(_levelStats["boss"].hardestDifficultyCompleted.compare("None")      == 0) _bosScreenSprite.setColor(sf::Color(255,255,255,80));
        if(_levelStats["boss"].hardestDifficultyCompleted.compare("None")  == 0) _frogScreenSprite.setColor(sf::Color(255,255,255,80));


}

int LevelSelectScreen::totalCogsCollected()
{
    int cogs = 0;
    cogs += _levelStats["tutorial"].cogsFound;
    cogs += _levelStats["highway"].cogsFound;
    cogs += _levelStats["wondercog"].cogsFound;
    cogs += _levelStats["vegas"].cogsFound;
    cogs += _levelStats["junkyard"].cogsFound;
    cogs += _levelStats["jungle"].cogsFound;
    cogs += _levelStats["roboruins"].cogsFound;

    return cogs;
}

int LevelSelectScreen::totalHappyRocksCollected()
{
	int rocks = 0;
	if (_levelStats["tutorial"].happyRockFound) rocks += 1;
	if (_levelStats["highway"].happyRockFound) rocks += 1;
	if (_levelStats["wondercog"].happyRockFound) rocks += 1;
	if (_levelStats["vegas"].happyRockFound) rocks += 1;
	if (_levelStats["junkyard"].happyRockFound) rocks += 1;
	if (_levelStats["jungle"].happyRockFound) rocks += 1;
	if (_levelStats["roboruins"].happyRockFound) rocks += 1;

	return rocks;
}

void LevelSelectScreen::update()
{
    if (determineLastLevelBeaten() == none_s) _unlockSprite.setPosition(_higScreenSprite.getPosition());
    if (determineLastLevelBeaten() == tutorial_s) _unlockSprite.setPosition(_wonScreenSprite.getPosition());
    if (determineLastLevelBeaten() == highway_s) _unlockSprite.setPosition(_vegScreenSprite.getPosition());
    if (determineLastLevelBeaten() == wondercog_s) _unlockSprite.setPosition(_junkScreenSprite.getPosition());
    if (determineLastLevelBeaten() == vegas_s) _unlockSprite.setPosition(_jungScreenSprite.getPosition());
    if (determineLastLevelBeaten() == junkyard_s) _unlockSprite.setPosition(_robScreenSprite.getPosition());
    if (determineLastLevelBeaten() == jungle_s) _unlockSprite.setPosition(_bosScreenSprite.getPosition());
    if (determineLastLevelBeaten() == roboruins_s) _unlockSprite.setColor(sf::Color(255,255,255,0));
    if (determineLastLevelBeaten() == boss_s) _unlockSprite.setColor(sf::Color(255,255,255,0));
    int increment = 20;
    int width = _tutScreenSprite.getTextureRect().width + 100;

    _curDifficulty.setString(game()->difficulty());

    if(_chooseDifficulty) _blackSprite.setColor(sf::Color(255,255,255,200));
    else _blackSprite.setColor(sf::Color(255,255,255,0));

    if(game()->difficulty().compare("Easy") == 0)
        _frogText.setString("As many\nlives as ya\nneed, bub.");
    else if(game()->difficulty().compare("Medium") == 0)
        _frogText.setString("Ten lives\n\'n that\'s it.");
    else if(game()->difficulty().compare("Hard") == 0)
        _frogText.setString("Cats gots\nnine lives.\nYous got five.");
    else if(game()->difficulty().compare("Very Hard") == 0)
        _frogText.setString("One life.\nLive it well,\nkid.");
    else if(game()->difficulty().compare("Insane") == 0)
        _frogText.setString("Get hit once\n\'n yer a\ngoner.");

    if(_moveLeft)
    {
        if(width - _mLCounter < increment)
            increment = width - _mLCounter;

        _tutScreenSprite.setPosition(_tutScreenSprite.getPosition().x + increment, _tutScreenSprite.getPosition().y);
        _higScreenSprite.setPosition(_higScreenSprite.getPosition().x + increment, _higScreenSprite.getPosition().y);
        _wonScreenSprite.setPosition(_wonScreenSprite.getPosition().x + increment, _wonScreenSprite.getPosition().y);
        _vegScreenSprite.setPosition(_vegScreenSprite.getPosition().x + increment, _vegScreenSprite.getPosition().y);
        _junkScreenSprite.setPosition(_junkScreenSprite.getPosition().x + increment, _junkScreenSprite.getPosition().y);
        _jungScreenSprite.setPosition(_jungScreenSprite.getPosition().x + increment, _jungScreenSprite.getPosition().y);
        _robScreenSprite.setPosition(_robScreenSprite.getPosition().x + increment, _robScreenSprite.getPosition().y);
        _bosScreenSprite.setPosition(_bosScreenSprite.getPosition().x + increment, _bosScreenSprite.getPosition().y);
        _frogScreenSprite.setPosition(_frogScreenSprite.getPosition().x + increment, _frogScreenSprite.getPosition().y);
        _mLCounter += increment;

        if(_mLCounter >= width)
        {
            _moveLeft = false;
            increment = 12;
            _canSelect = true;
        }
    }

    if(_moveRight)
    {
        if(width - _mRCounter < increment)
            increment = width - _mRCounter;

        _tutScreenSprite.setPosition(_tutScreenSprite.getPosition().x - increment, _tutScreenSprite.getPosition().y);
        _higScreenSprite.setPosition(_higScreenSprite.getPosition().x - increment, _higScreenSprite.getPosition().y);
        _wonScreenSprite.setPosition(_wonScreenSprite.getPosition().x - increment, _wonScreenSprite.getPosition().y);
        _vegScreenSprite.setPosition(_vegScreenSprite.getPosition().x - increment, _vegScreenSprite.getPosition().y);
        _junkScreenSprite.setPosition(_junkScreenSprite.getPosition().x - increment, _junkScreenSprite.getPosition().y);
        _jungScreenSprite.setPosition(_jungScreenSprite.getPosition().x - increment, _jungScreenSprite.getPosition().y);
        _robScreenSprite.setPosition(_robScreenSprite.getPosition().x - increment, _robScreenSprite.getPosition().y);
        _bosScreenSprite.setPosition(_bosScreenSprite.getPosition().x - increment, _bosScreenSprite.getPosition().y);
        _frogScreenSprite.setPosition(_frogScreenSprite.getPosition().x - increment, _frogScreenSprite.getPosition().y);
        _mRCounter += increment;

        if(_mRCounter >= width)
        {
            _moveRight = false;
            increment = 12;
            _canSelect = true;
        }
    }

    if(_currentLevel == tutorial_s) _leftArrowSprite.setColor(sf::Color(255,255,255,80));
    else _leftArrowSprite.setColor(sf::Color(255,255,255,255));

    if(_currentLevel == frog_s) _rightArrowSprite.setColor(sf::Color(255,255,255,80));
    else _rightArrowSprite.setColor(sf::Color(255,255,255,255));

    if(_currentLevel - 1 == _lastLevelBeaten || _lastLevelBeaten == none_s)
        _rightArrowSprite.setColor(sf::Color(255,255,255,80));

    switch(_currentLevel)
    {
    case tutorial_s:
        _levelNameText.setString("Robotropolis");
        //_levelNameText.setPosition(this->game()->renderWindow()->getView().getCenter().x - 156,

        _difficultyText.setString(generateDifficultyText("tutorial"));
        _timeText.setString(generateTimeText("tutorial"));
        _cogsText.setString(generateCogsText("tutorial"));
        _hrText.setString(generateHrText("tutorial"));
        break;
    case highway_s:
        _levelNameText.setString("Cosmic Highway");
        _difficultyText.setString(generateDifficultyText("highway"));
        _timeText.setString(generateTimeText("highway"));
        _cogsText.setString(generateCogsText("highway"));
        _hrText.setString(generateHrText("highway"));
        break;
    case wondercog_s:
        _levelNameText.setString("Wondercog");
        _difficultyText.setString(generateDifficultyText("wondercog"));
        _timeText.setString(generateTimeText("wondercog"));
        _cogsText.setString(generateCogsText("wondercog"));
        _hrText.setString(generateHrText("wondercog"));
        break;
    case vegas_s:
        _levelNameText.setString("Vegas");
        _difficultyText.setString(generateDifficultyText("vegas"));
        _timeText.setString(generateTimeText("vegas"));
        _cogsText.setString(generateCogsText("vegas"));
        _hrText.setString(generateHrText("vegas"));
        break;
    case junkyard_s:
        _levelNameText.setString("Junkyard");
        _difficultyText.setString(generateDifficultyText("junkyard"));
        _timeText.setString(generateTimeText("junkyard"));
        _cogsText.setString(generateCogsText("junkyard"));
        _hrText.setString(generateHrText("junkyard"));
        break;
    case jungle_s:
        _levelNameText.setString("Jungle");
        _difficultyText.setString(generateDifficultyText("jungle"));
        _timeText.setString(generateTimeText("jungle"));
        _cogsText.setString(generateCogsText("jungle"));
        _hrText.setString(generateHrText("jungle"));
        break;
    case roboruins_s:
        _levelNameText.setString("Robo Ruins");
        _difficultyText.setString(generateDifficultyText("roboruins"));
        _timeText.setString(generateTimeText("roboruins"));
        _cogsText.setString(generateCogsText("roboruins"));
        _hrText.setString(generateHrText("roboruins"));
        break;
    case boss_s:
        _levelNameText.setString("King Globulon");
        _difficultyText.setString(generateDifficultyText("boss"));
        _timeText.setString(generateTimeText("boss"));
        _cogsText.setString(generateCogsText("boss"));
        _hrText.setString(generateHrText("boss"));
        break;
    case frog_s:
        _levelNameText.setString("Frog-E Fixit");
        _difficultyText.setString(generateDifficultyText("boss"));
        _timeText.setString(generateTimeText("boss"));
        _cogsText.setString(generateCogsText("boss"));
        _hrText.setString(generateHrText("boss"));
        break;
    default:
        throw fwk::InternalException("Reached unexpected default case!");
        break;
    }

    _levelNameText.setPosition(this->game()->renderWindow()->getView().getCenter().x - _levelNameText.getGlobalBounds().width / 2,
        this->game()->renderWindow()->getView().getCenter().y - 450);

    unhighlightUnbeatenLevels();
    highlightBeatenLevels();

    if(sf::Joystick::isConnected(0))
    {
        _confirmSprite.setTextureRect(sf::IntRect(0,0,50,50));
        _backSprite.setTextureRect(sf::IntRect(50,0,50,50));
        _storyModeSprite.setTextureRect(sf::IntRect(100,0,50,50));
    }
    else
    {
        _confirmSprite.setTextureRect(sf::IntRect(300,0,50,50));
        _backSprite.setTextureRect(sf::IntRect(350,0,50,50));
        _storyModeSprite.setTextureRect(sf::IntRect(400,0,50,50));
    }
}

void LevelSelectScreen::draw()
{
    sf::View view = game()->renderWindow()->getDefaultView();
    view.zoom(1/scale().x);
    //view.setSize(game()->renderWindow()->getSize().x, game()->renderWindow()->getSize().y);
    game()->renderWindow()->setView(view);
    game()->renderWindow()->draw(_bgSprite);
    game()->renderWindow()->draw(_leftArrowSprite);
    game()->renderWindow()->draw(_rightArrowSprite);
    game()->renderWindow()->draw(_tutScreenSprite);
    game()->renderWindow()->draw(_higScreenSprite);
    game()->renderWindow()->draw(_wonScreenSprite);
    game()->renderWindow()->draw(_vegScreenSprite);
    game()->renderWindow()->draw(_junkScreenSprite);
    game()->renderWindow()->draw(_jungScreenSprite);
    game()->renderWindow()->draw(_robScreenSprite);
    game()->renderWindow()->draw(_bosScreenSprite);
    game()->renderWindow()->draw(_frogScreenSprite);
    game()->renderWindow()->draw(_unlockSprite);
    game()->renderWindow()->draw(_levelNameText);
    if(_currentLevel != frog_s) game()->renderWindow()->draw(_difficultyText);
    if(_currentLevel != frog_s) game()->renderWindow()->draw(_timeText);
    if(_currentLevel != boss_s && _currentLevel != frog_s) game()->renderWindow()->draw(_cogsText);
    if(_currentLevel != boss_s && _currentLevel != frog_s) game()->renderWindow()->draw(_hrText);
    if(_currentLevel == tutorial_s) game()->renderWindow()->draw(_storyModeText);
    if(_currentLevel == tutorial_s) game()->renderWindow()->draw(_storyModeSprite);

    if(_chooseDifficulty)
    {
        game()->renderWindow()->draw(_blackSprite);
        game()->renderWindow()->draw(_frogSprite);
        game()->renderWindow()->draw(_frogText);
        game()->renderWindow()->draw(_curDifficulty);
        game()->renderWindow()->draw(_diffChooseText);
        game()->renderWindow()->draw(_leftArrowSprite2);
        game()->renderWindow()->draw(_rightArrowSprite2);
    }

    game()->renderWindow()->draw(_confirmSprite);
    game()->renderWindow()->draw(_backSprite);


    game()->renderWindow()->draw(_confirmText);
    game()->renderWindow()->draw(_backText);
}

void LevelSelectScreen::powerDown()
{
    _arrowDesc = NULL;
    _buttonsDesc = NULL;
    _tutScreenDesc = NULL;
    _higScreenDesc = NULL;
    _wonScreenDesc = NULL;
    _junkScreenDesc = NULL;
    _jungScreenDesc = NULL;
    _vegScreenDesc = NULL;
    _robScreenDesc = NULL;
    _bosScreenDesc = NULL;
}

void LevelSelectScreen::printStats()
{
    std::map<std::string, LevelStats>::iterator itr;
    for(itr = _levelStats.begin(); itr != _levelStats.end(); itr++ )
    {
        std::cout << "Level: " << itr->second.name << std::endl;
        std::cout << "Difficulty: " << itr->second.hardestDifficultyCompleted << std::endl;
        std::cout << "Cogs found: " << itr->second.cogsFound << std::endl;
        if( itr->second.happyRockFound) std::cout << "HappyRock found: true" << std::endl;
        else std::cout << "HappyRock found: false" << std::endl;
        std::cout << "Fastest time: " << itr->second.fastestTimeMinutes << ":" <<
                itr->second.fastestTimeSeconds << ":" <<
                itr->second.fastestTimeMilliseconds << std::endl;
    }
}

void LevelSelectScreen::printStats(std::string levelName)
{
    std::cout << "Level: " << _levelStats[levelName].name << std::endl;
    std::cout << "Difficulty: " << _levelStats[levelName].hardestDifficultyCompleted << std::endl;
    std::cout << "Cogs found: " << _levelStats[levelName].cogsFound << std::endl;
    if( _levelStats[levelName].happyRockFound) std::cout << "HappyRock found: true" << std::endl;
    else std::cout << "HappyRock found: false" << std::endl;
    std::cout << "Fastest time: " << _levelStats[levelName].fastestTimeMinutes << ":" <<
                _levelStats[levelName].fastestTimeSeconds << ":" <<
                _levelStats[levelName].fastestTimeMilliseconds << std::endl;
}

std::string LevelSelectScreen::encrypt(std::string toEncrypt)
{
    return game()->settings()->encrypt(toEncrypt);
}

