// Copyright (c) 2017 Happy Rock Studios

#include "ZorbitsOrbits.h"

#include "JollyLava/Splash.h"

#include "ZorbitMenu.h"
#include "LoadingScreen.h"
#include "LevelSelect.h"
#include "ZorbitLevel.h"
#include "Credits.h"
#include "ScoreScreen.h"
#include "FrogScreen.h"

void ZorbitsOrbits::initializeStates()
{
        //_steamWrapper = new SteamWrapper();

        // Splash screen.
        jl::TextureDesc::Ptr tex;
        tex = textureMan()->textureDescIs("resources/zorbitsorbits/splash/splash.png",
            sf::Vector2f(400, 300), 800, 600);
        sf::Time t1 = sf::seconds(3.0f);
        stateMan()->stateNew(new jl::Splash("splash", this, tex, t1));

        // Second splash.
        jl::TextureDesc::Ptr tex2;
        tex2 = textureMan()->textureDescIs("resources/zorbitsorbits/splash/splash2.png",
            sf::Vector2f(400, 300), 800, 600);
        stateMan()->stateNew(new jl::Splash("splash2", this, tex2, t1));

        // Menu screen.
        jl::TextureDesc::Ptr tex3;
        tex3 = textureMan()->textureDescIs("resources/zorbitsorbits/menu/menu.png",
            sf::Vector2f(400, 300), 800, 600);
        jl::TextureDesc::Ptr tex4;
        tex4 = textureMan()->textureDescIs("resources/zorbitsorbits/menu/pointer.png",
            sf::Vector2f(20, 20), 40, 40);
        stateMan()->stateNew(new ZorbitMenu(this, tex3, tex4));

        _fonts["foo"] = new sf::Font();;
        _fonts["foo"]->loadFromFile("resources/fonts/foo.ttf");

        _fonts["arial"] = new sf::Font();;
        _fonts["arial"]->loadFromFile("resources/fonts/arial.ttf");

        _currentLevel = uninitialized;
        _gameOver = false;
        _currentHealth = 0;
        _currentStatus = not_playing;

        //Cheats
        _godMode    = false;
        _sillyMode  = false;
        _fastMode   = false;
        _vex        = false;

        soundMan()->soundNew("slot_activate", "resources/sounds/slot_activate.wav", false);
        soundMan()->soundNew("slot_whir", "resources/sounds/slot_whir.wav", false);

        // Setup states.
        stateMan()->stateNew(new LoadingScreen(this));
        stateMan()->stateNew(new LevelSelectScreen(this));
        stateMan()->stateNew(new FrogScreen(this));
        //stateMan()->stateNew(new ScoreScreen(this));
        stateMan()->queueState("mainmenu");
        stateMan()->queueState("splash2");
        stateMan()->activeStateIs("splash");

        soundMan()->musicNew("credits", "resources/music/credits.ogg", false);
        soundMan()->musicNew("gameover", "resources/music/gameover.ogg", false);
        soundMan()->musicNew("robotropolis", "resources/music/robotropolis.ogg", false, 20);
        soundMan()->musicNew("tboss", "resources/music/tboss.ogg", false, 30);
        soundMan()->musicNew("vex", "resources/music/vex.ogg", false);
        soundMan()->musicNew("vboss", "resources/music/vboss.ogg", false, 10);
        soundMan()->musicNew("wondercog", "resources/music/wondercog.ogg", false);
        soundMan()->musicNew("fboss", "resources/music/fboss.ogg", false, 20);
        soundMan()->musicNew("vegas", "resources/music/vegas.ogg", false);
        soundMan()->musicNew("eboss", "resources/music/eboss.ogg", false);
        soundMan()->musicNew("junkyard", "resources/music/junkyard.ogg", false, 10);
        soundMan()->musicNew("sboss", "resources/music/sboss.ogg", false);
        soundMan()->musicNew("jungle", "resources/music/jungle.ogg", false);
        soundMan()->musicNew("kboss", "resources/music/kboss.ogg", false, 20);
        soundMan()->musicNew("roboruins", "resources/music/roboruins.ogg", false);
        soundMan()->musicNew("ruinsend", "resources/music/ruinsend.ogg", false);
        soundMan()->musicNew("boss", "resources/music/boss.ogg", false);
        soundMan()->musicNew("zorbit", "resources/music/zorbit.ogg", false);

        //stateMan()->stateNew(new Credits(this));
        //stateMan()->activeStateIs("mainmenu");

        //settings()->encryptFile("levels/stats_decrypted_blank.txt");
        //settings()->encryptFile("levels/tutorial/tutorial.txt");
        //settings()->encryptFile("levels/tutorial/events/planet4.txt");
        //settings()->encryptFile("levels/highway/events/bossactivate.txt");
        //settings()->encryptFile("levels/wondercog/events/bossfinish.txt");
        //settings()->encryptFile("levels/vegas/events/levelfinish.txt");
        //settings()->encryptFile("levels/junkyard/events/levelfinish.txt");
        //settings()->encryptFile("levels/jungle/events/bossactivate.txt");
        //settings()->encryptFile("levels/roboruins/events/cinematic.txt");
        //settings()->encryptFile("levels/boss/events/init.txt");
        //settings()->encryptFile("levels/wondercog/wondercog.txt");
        //settings()->encryptFile("levels/wondercog/events/camera14.txt");
        //settings()->encryptFile("levels/vegas/vegas.txt");
        //settings()->encryptFile("levels/vegas/events/ballreset6.txt");
        //settings()->encryptFile("levels/vegas/events/camera10.txt");
        //settings()->encryptFile("levels/jungle/jungle.txt");
        //settings()->encryptFile("levels/jungle/events/boulder.txt");
        //settings()->encryptFile("levels/jungle/events/boulder2.txt");
        //settings()->encryptFile("levels/boss/boss.txt");
        //settings()->encryptFile("levels/roboruins/events/cinematic.txt");
        //settings()->encryptFile("levels/roboruins/roboruins.txt");
        //settings()->encryptFile("levels/wondercog/wondercog.txt");
        //settings()->encryptFile("levels/wondercog/events/camera10.txt");
        //settings()->encryptFile("levels/wondercog/events/camera13.txt");
        //settings()->encryptFile("levels/highway/highway.txt");
        //settings()->encryptFile("levels/highway/events/camera1.txt");
        //settings()->encryptFile("levels/junkyard/junkyard.txt");
        //settings()->encryptFile("levels/junkyard/events/init.txt");
        //settings()->encryptFile("levels/junkyard/events/button5.txt");
        //settings()->encryptFile("levels/junkyard/events/platformstop14.txt");
}

void ZorbitsOrbits::tearDownConcrete()
{
    //delete _steamWrapper;
}
void ZorbitsOrbits::gameIsOver(bool value)
{
    if(!value)
    {
        _gameOver = false;
        return;
    }

    _gameOver = true;
    _levelNames.clear();
    static_cast<ZorbitLevel*>(stateMan()->activeState().ptr())->finish();
    _currentLevel = uninitialized;
}

LevelSelectScreen* ZorbitsOrbits::levelSelectScreen()
{
    return static_cast<LevelSelectScreen*>(stateMan()->state("LevelSelect").ptr());}
