// Copyright (c) 2017 Happy Rock Studios

#include "LoadingScreen.h"
#include "LevelSelect.h"
#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "Credits.h"
#include "GameOver.h"
#include "Cinematic.h"

LoadingScreen::LoadingScreen(ZorbitsOrbits * game) : jl::Screen("LoadingScreen", game)
{
    _loadDesc = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/loading.png",
        sf::Vector2f(400, 300), 800, 600);
    _tutDesc = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/t_tut.png",
        sf::Vector2f(533, 400), 1067, 800);
    _higDesc = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/t_hig.png",
        sf::Vector2f(533, 400), 1067, 800);
    _wonDesc = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/t_won.png",
        sf::Vector2f(533, 400), 1067, 800);
    _vegDesc = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/t_veg.png",
        sf::Vector2f(533, 400), 1067, 800);
    _junkDesc = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/t_junk.png",
        sf::Vector2f(533, 400), 1067, 800);
    _jungDesc = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/t_jung.png",
        sf::Vector2f(533, 400), 1067, 800);
    _robDesc = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/t_rob.png",
        sf::Vector2f(533, 400), 1067, 800);
    _borderDesc = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/border.png",
        sf::Vector2f(960, 540), 1920, 1080);

    _spaceDesc = this->game()->textureMan()->textureDesc("resources/zorbitsorbits/menu/background2.png");

    _sprite.setTexture(*_loadDesc->texture());
    _sprite.setOrigin(_loadDesc->origin());
    _sprite.setTextureRect(_loadDesc->rect());
    _sprite.setPosition(this->game()->renderWindow()->getView().getCenter());

    _borderSprite.setTexture(*_borderDesc->texture());
    _borderSprite.setOrigin(_borderDesc->origin());
    _borderSprite.setPosition(this->game()->renderWindow()->getView().getCenter());
    _borderSprite.scale(float(this->game()->renderWindow()->getSize().x) / 1920.f,
                       float(this->game()->renderWindow()->getSize().y) / 1080.f);

    _spaceSprite.setTexture(*_spaceDesc->texture());
    _spaceSprite.setOrigin(_spaceDesc->origin());
    _spaceSprite.setPosition(this->game()->renderWindow()->getView().getCenter());
    _spaceSprite.scale(float(this->game()->renderWindow()->getSize().x) / 1920.f,
                       float(this->game()->renderWindow()->getSize().y) / 1080.f);

    _loadingText.setFont(*this->game()->font("foo"));
    _loadingText.setCharacterSize(60);
    _loadingText.setColor(sf::Color::White);
    _loadingText.setString("Loading...");
    _loadingText.setPosition(this->game()->renderWindow()->getView().getCenter().x - _loadingText.getLocalBounds().width / 2,
                           this->game()->renderWindow()->getView().getSize().y - 80);

    if(this->game()->renderWindow()->getSize().x < 1920)
        _sprite.scale(float(this->game()->renderWindow()->getSize().x) / 1920.0f, float(this->game()->renderWindow()->getSize().y) / 1080.0f);

    _loaded = notBegun;
    _levelName = "uninitialized";
    _returnName = "None";
}

void LoadingScreen::levelNameIs(std::string levelName)
{
    //if(_levelName.compare(levelName) != 0)
    //{
        _levelName = levelName;
        _loaded = loading;
    //}
}

void LoadingScreen::loadLevel(std::string levelName)
{
    if(levelName.compare("reset") == 0)
    {
        _sprite.setTexture(*_loadDesc->texture());
        _sprite.setOrigin(_loadDesc->origin());
        _sprite.setTextureRect(_loadDesc->rect());
        _loadingText.setString("");

        if(static_cast<ZorbitsOrbits*>(game())->currentGameMode() == classic
           && !static_cast<ZorbitsOrbits*>(game())->gameOver())
        {
            game()->stateMan()->stateNew(new Credits(static_cast<ZorbitsOrbits*>(game())));
            static_cast<ZorbitsOrbits*>(game())->currentStatusIs(not_playing);
            static_cast<Credits*>(game()->stateMan()->state("Credits").ptr())->reset();
            game()->stateMan()->activeStateIs("Credits");
        }
        else if(static_cast<ZorbitsOrbits*>(game())->gameOver())
        {
            game()->stateMan()->stateNew(new GameOver(static_cast<ZorbitsOrbits*>(game())));
            static_cast<ZorbitsOrbits*>(game())->currentStatusIs(not_playing);
            game()->stateMan()->activeStateIs("GameOver");
        }
        else
        {
            static_cast<LevelSelectScreen*>(game()->stateMan()->state("LevelSelect").ptr())->reset();
            static_cast<ZorbitsOrbits*>(game())->currentStatusIs(not_playing);
            game()->stateMan()->activeStateIs("LevelSelect");
        }

        levelNameIs("uninitialized");
    }
    if(levelName.compare("tutorial") == 0)
    {
        _loadingText.setString("Loading...");
        _loadingText.setPosition(this->game()->renderWindow()->getView().getCenter().x - _loadingText.getLocalBounds().width / 2,
                           this->game()->renderWindow()->getView().getSize().y - 90);
        _sprite.setTexture(*_tutDesc->texture());
        _sprite.setOrigin(_tutDesc->origin());
        _sprite.setTextureRect(_tutDesc->rect());
        game()->soundMan()->playMusic("robotropolis");
        game()->stateMan()->stateNew(new ZorbitLevel("tutorial", "tutorial", game())); //level name, filepath, game
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("highway") == 0)
    {
        _loadingText.setString("Loading...");
        _loadingText.setPosition(this->game()->renderWindow()->getView().getCenter().x - _loadingText.getLocalBounds().width / 2,
                           this->game()->renderWindow()->getView().getSize().y - 90);
        _sprite.setTexture(*_higDesc->texture());
        _sprite.setOrigin(_higDesc->origin());
        _sprite.setTextureRect(_higDesc->rect());
        game()->soundMan()->playMusic("vex");
        game()->stateMan()->stateNew(new ZorbitLevel("highway", "highway", game(), static_cast<ZorbitsOrbits*>(game())->currentHealth())); //level name, filepath, game, health
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("wondercog") == 0)
    {
        _loadingText.setString("Loading...");
        _loadingText.setPosition(this->game()->renderWindow()->getView().getCenter().x - _loadingText.getLocalBounds().width / 2,
                           this->game()->renderWindow()->getView().getSize().y - 90);
        _sprite.setTexture(*_wonDesc->texture());
        _sprite.setOrigin(_wonDesc->origin());
        _sprite.setTextureRect(_wonDesc->rect());
        game()->soundMan()->playMusic("wondercog");
        game()->stateMan()->stateNew(new ZorbitLevel("wondercog", "wondercog", game(), static_cast<ZorbitsOrbits*>(game())->currentHealth()));  //level name, filepath, game
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("vegas") == 0)
    {
        _loadingText.setString("Loading...");
        _loadingText.setPosition(this->game()->renderWindow()->getView().getCenter().x - _loadingText.getLocalBounds().width / 2,
                           this->game()->renderWindow()->getView().getSize().y - 90);
        _sprite.setTexture(*_vegDesc->texture());
        _sprite.setOrigin(_vegDesc->origin());
        _sprite.setTextureRect(_vegDesc->rect());
        game()->soundMan()->playMusic("vegas");
        game()->stateMan()->stateNew(new ZorbitLevel("vegas", "vegas", game(), static_cast<ZorbitsOrbits*>(game())->currentHealth()));  //level name, filepath, game
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("junkyard") == 0)
    {
        _loadingText.setString("Loading...");
        _loadingText.setPosition(this->game()->renderWindow()->getView().getCenter().x - _loadingText.getLocalBounds().width / 2,
                           this->game()->renderWindow()->getView().getSize().y - 90);
        _sprite.setTexture(*_junkDesc->texture());
        _sprite.setOrigin(_junkDesc->origin());
        _sprite.setTextureRect(_junkDesc->rect());
        game()->soundMan()->playMusic("junkyard");
        game()->stateMan()->stateNew(new ZorbitLevel("junkyard", "junkyard", game(), static_cast<ZorbitsOrbits*>(game())->currentHealth()));  //level name, filepath, game
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("jungle") == 0)
    {
        _loadingText.setString("Loading...");
        _loadingText.setPosition(this->game()->renderWindow()->getView().getCenter().x - _loadingText.getLocalBounds().width / 2,
                           this->game()->renderWindow()->getView().getSize().y - 90);
        _sprite.setTexture(*_jungDesc->texture());
        _sprite.setOrigin(_jungDesc->origin());
        _sprite.setTextureRect(_jungDesc->rect());
        game()->soundMan()->playMusic("jungle");
        game()->stateMan()->stateNew(new ZorbitLevel("jungle", "jungle", game(), static_cast<ZorbitsOrbits*>(game())->currentHealth()));  //level name, filepath, game
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("roboruins") == 0)
    {
        _loadingText.setString("Loading...");
        _loadingText.setPosition(this->game()->renderWindow()->getView().getCenter().x - _loadingText.getLocalBounds().width / 2,
                           this->game()->renderWindow()->getView().getSize().y - 90);
        _sprite.setTexture(*_robDesc->texture());
        _sprite.setOrigin(_robDesc->origin());
        _sprite.setTextureRect(_robDesc->rect());
        game()->soundMan()->playMusic("roboruins");
        game()->stateMan()->stateNew(new ZorbitLevel("roboruins", "roboruins", game(), static_cast<ZorbitsOrbits*>(game())->currentHealth())); //level name, filepath, game
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("boss") == 0)
    {
        _loadingText.setString("");
        _sprite.setTexture(*_loadDesc->texture());
        _sprite.setOrigin(_loadDesc->origin());
        _sprite.setTextureRect(_loadDesc->rect());
        game()->soundMan()->playMusic("boss");
        game()->stateMan()->stateNew(new ZorbitLevel("boss", "boss", game(), static_cast<ZorbitsOrbits*>(game())->currentHealth()));  //level name, filepath, game
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }

    // Cinematics

    if(levelName.compare("tc1") == 0)
    {
        _loadingText.setString("");
        _sprite.setTexture(*_loadDesc->texture());
        _sprite.setOrigin(_loadDesc->origin());
        _sprite.setTextureRect(_loadDesc->rect());
        game()->soundMan()->playMusic("robotropolis");
        game()->stateMan()->stateNew(new Cinematic("tc1", game(), "resources/zorbitsorbits/cinematics/tc1.png"));
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("tc2") == 0)
    {
        game()->soundMan()->playMusic("robotropolis");
        game()->stateMan()->stateNew(new Cinematic("tc2", game(), "resources/zorbitsorbits/cinematics/tc2.png"));
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("tc3") == 0)
    {
        game()->soundMan()->playMusic("robotropolis");
        game()->stateMan()->stateNew(new Cinematic("tc3", game(), "resources/zorbitsorbits/cinematics/tc3.png"));
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("tc4") == 0)
    {
        game()->soundMan()->playMusic("robotropolis");
        game()->stateMan()->stateNew(new Cinematic("tc4", game(), "resources/zorbitsorbits/cinematics/tc4.png"));
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("tc5") == 0)
    {
        game()->soundMan()->playMusic("robotropolis");
        game()->stateMan()->stateNew(new Cinematic("tc5", game(), "resources/zorbitsorbits/cinematics/tc5.png"));
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("tc6") == 0)
    {
        game()->soundMan()->playMusic("robotropolis");
        game()->stateMan()->stateNew(new Cinematic("tc6", game(), "resources/zorbitsorbits/cinematics/tc6.png"));
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("hc1") == 0)
    {
        _loadingText.setString("");
        _sprite.setTexture(*_loadDesc->texture());
        _sprite.setOrigin(_loadDesc->origin());
        _sprite.setTextureRect(_loadDesc->rect());
        game()->soundMan()->playMusic("vex");
        game()->stateMan()->stateNew(new Cinematic("hc1", game(), "resources/zorbitsorbits/cinematics/hc1.png"));
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("hc2") == 0)
    {
        game()->soundMan()->playMusic("vex");
        game()->stateMan()->stateNew(new Cinematic("hc2", game(), "resources/zorbitsorbits/cinematics/hc2.png"));
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("hc3") == 0)
    {
        game()->soundMan()->playMusic("vex");
        game()->stateMan()->stateNew(new Cinematic("hc3", game(), "resources/zorbitsorbits/cinematics/hc3.png"));
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("hc4") == 0)
    {
        game()->soundMan()->playMusic("vex");
        game()->stateMan()->stateNew(new Cinematic("hc4", game(), "resources/zorbitsorbits/cinematics/hc4.png"));
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("jc1") == 0)
    {
        _loadingText.setString("");
        _sprite.setTexture(*_loadDesc->texture());
        _sprite.setOrigin(_loadDesc->origin());
        _sprite.setTextureRect(_loadDesc->rect());
        game()->soundMan()->playMusic("roboruins");
        game()->stateMan()->stateNew(new Cinematic("jc1", game(), "resources/zorbitsorbits/cinematics/jc1.png"));
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("jc2") == 0)
    {
        game()->soundMan()->playMusic("roboruins");
        game()->stateMan()->stateNew(new Cinematic("jc2", game(), "resources/zorbitsorbits/cinematics/jc2.png"));
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("jc3") == 0)
    {
        game()->soundMan()->playMusic("roboruins");
        game()->stateMan()->stateNew(new Cinematic("jc3", game(), "resources/zorbitsorbits/cinematics/jc3.png"));
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("jc4") == 0)
    {
        game()->soundMan()->playMusic("roboruins");
        game()->stateMan()->stateNew(new Cinematic("jc4", game(), "resources/zorbitsorbits/cinematics/jc4.png"));
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("rc1") == 0)
    {
        _loadingText.setString("");
        _sprite.setTexture(*_loadDesc->texture());
        _sprite.setOrigin(_loadDesc->origin());
        _sprite.setTextureRect(_loadDesc->rect());
        game()->soundMan()->playMusic("ruinsend");
        game()->stateMan()->stateNew(new Cinematic("rc1", game(), "resources/zorbitsorbits/cinematics/rc1.png"));
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("rc2") == 0)
    {
        game()->soundMan()->playMusic("ruinsend");
        game()->stateMan()->stateNew(new Cinematic("rc2", game(), "resources/zorbitsorbits/cinematics/rc2.png"));
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("rc3") == 0)
    {
        game()->soundMan()->playMusic("ruinsend");
        game()->stateMan()->stateNew(new Cinematic("rc3", game(), "resources/zorbitsorbits/cinematics/rc3.png"));
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("rc4") == 0)
    {
        game()->soundMan()->playMusic("ruinsend");
        game()->stateMan()->stateNew(new Cinematic("rc4", game(), "resources/zorbitsorbits/cinematics/rc4.png"));
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("rc5") == 0)
    {
        game()->soundMan()->playMusic("ruinsend");
        game()->stateMan()->stateNew(new Cinematic("rc5", game(), "resources/zorbitsorbits/cinematics/rc5.png"));
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("rc6") == 0)
    {
        game()->soundMan()->playMusic("ruinsend");
        game()->stateMan()->stateNew(new Cinematic("rc6", game(), "resources/zorbitsorbits/cinematics/rc6.png"));
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("rc7") == 0)
    {
        game()->soundMan()->playMusic("ruinsend");
        game()->stateMan()->stateNew(new Cinematic("rc7", game(), "resources/zorbitsorbits/cinematics/rc7.png"));
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("bc1") == 0)
    {
        _loadingText.setString("");
        _sprite.setTexture(*_loadDesc->texture());
        _sprite.setOrigin(_loadDesc->origin());
        _sprite.setTextureRect(_loadDesc->rect());
        game()->soundMan()->playMusic("credits");
        game()->stateMan()->stateNew(new Cinematic("bc1", game(), "resources/zorbitsorbits/cinematics/bc1.png"));
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }
    if(levelName.compare("bc2") == 0)
    {
        game()->soundMan()->playMusic("credits");
        game()->stateMan()->stateNew(new Cinematic("bc2", game(), "resources/zorbitsorbits/cinematics/bc2.png"));
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(playing);
    }

    _loaded = finishedLoading;
}

void LoadingScreen::update()
{
    game()->soundMan()->resumeMusic();

    if(_loaded == loading)
        loadLevel(_levelName);
    else if(_loaded == finishedLoading)
    {
        game()->stateMan()->activeStateIs(_levelName);
    }
}

void LoadingScreen::update2()
{
    //std::cout << "adsfasdf" << std::endl;
    //_spaceSprite.rotate(1);
}

void LoadingScreen::draw()
{
    if(_levelName.compare("tutorial") == 0
       || _levelName.compare("highway") == 0
       || _levelName.compare("wondercog") == 0
       || _levelName.compare("vegas") == 0
       || _levelName.compare("junkyard") == 0
       || _levelName.compare("jungle") == 0
       || _levelName.compare("roboruins") == 0)
    {
        game()->renderWindow()->draw(_spaceSprite);
        game()->renderWindow()->draw(_sprite);
        game()->renderWindow()->draw(_borderSprite);
    }
    game()->renderWindow()->draw(_loadingText);
}

void LoadingScreen::cleanUp()
{

}
