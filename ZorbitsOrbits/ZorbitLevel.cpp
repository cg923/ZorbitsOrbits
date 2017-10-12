// Copyright(c) 2017 Happy Rock Studios

#include "JollyLava/Tool.h"

#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "ZorbitsOrbits.h"
#include "Zorbit.h"
#include "ZShot.h"
#include "ZGlob.h"
#include "ZGlobBat.h"
#include "ZGlobDrop.h"
#include "ZGrapple.h"
#include "ZBoinger.h"
#include "ZBooster.h"
#include "ZCircle.h"
#include "ZMovingPlatform.h"
#include "ZCog.h"
#include "ZPlatform.h"
#include "ZSpaceGenerator.h"
#include "ZHappyRock.h"
#include "ZRectShape.h"
#include "ZSlot.h"
#include "ZKops.h"
#include "HealthHUD.h"
#include "CogHUD.h"
#include "BossHUD.h"
#include "ZorbitMenu.h"
#include "LoadingScreen.h"
#include "LevelSelect.h"
#include "Cinematic.h"
#include "ScoreScreen.h"

// Bosses
#include "ZTutorialBoss.h"
#include "ZFunzoBoss.h"
#include "ZVexBoss.h"
#include "ZElvisBoss.h"
#include "ZStrongoBoss.h"
#include "ZKrawlurBoss.h"
#include "ZGlobBoss.h"

void ZorbitLevel::populate()
{
    _firstTouch = false;
    _wholeLevelBeaten = false;

    //TODO do we really need to specify different layer names for every entity?
    //We could probably just have every Entity have a foreground, mid, and back by default.
    _layerNames.push_back("background");
    _layerNames.push_back("foreground");

    _boingerFactor = 1;

    // Fast mode
    if(static_cast<ZorbitsOrbits*>(game())->_fastMode)
        _boingerFactor = 1.3;

    _toBeCreatedBySubclass.insert("levels/highway/images/fire.png");
    _toBeCreatedBySubclass.insert("levels/wondercog/images/fire.png");
    _toBeCreatedBySubclass.insert("levels/junkyard/images/fire.png");
    _toBeCreatedBySubclass.insert("levels/roboruins/images/fire.png");
    _toBeCreatedBySubclass.insert("levels/roboruins/images/shortfire.png");
    _toBeCreatedBySubclass.insert("levels/roboruins/images/albot.png");

    if(loadLevel(_filePath))
    {
        if(_firstInitialization)
        {

            _entityMan->entityNew("Zorbit", "Layer1", new Zorbit(this, _layerNames, _startingPos, _startingAngle, static_cast<ZorbitsOrbits*>(game())->_vex));

            if(static_cast<ZorbitsOrbits*>(game())->_sillyMode) static_cast<Zorbit*>(_entityMan->entity("Zorbit").ptr())->flipSprites();
            if(static_cast<ZorbitsOrbits*>(game())->currentGameMode() == classic)
                static_cast<Zorbit*>(entityMan()->entity("Zorbit").ptr())->setHealth(_healthToSet);

            generateBoss();
            
            sf::Vector2f hudScale = sf::Vector2f(game()->renderWindow()->getDefaultView().getSize().x/1920,
                                                game()->renderWindow()->getDefaultView().getSize().y/1080);

            if(hudScale.x > 1 || hudScale.y > 1) hudScale = sf::Vector2f(1,1);

            _hudMan->hudNew("ZorbitHealth", new HealthHUD(this, sf::Vector2f()));
            _hudMan->hudNew("ZorbitCog", new CogHUD(this, sf::Vector2f(1505 * hudScale.x, 0)));
            _hudMan->hud("ZorbitCog")->scale(hudScale);
            if(_bossName.compare("FunzoBoss") == 0) _hudMan->hudNew("BossHUD", new BossHUD(_bossName, this, sf::Vector2f(game()->renderWindow()->getDefaultView().getSize().x / 2,
                                                                                                                     game()->renderWindow()->getDefaultView().getSize().y - 80)));
            else _hudMan->hudNew("BossHUD", new BossHUD(_bossName, this, sf::Vector2f(game()->renderWindow()->getDefaultView().getSize().x / 2, 40)));

            _spaceGen = new ZSpaceGenerator(this);

            if(name().compare("boss") != 0)  // No HR on boss level
            {
                _happyRockPos = _entityMan->entity("HappyRock")->position();
                _happyRockAngle = jl::Tool::sfAngle(_entityMan->entity("HappyRock")->angle() - jl::Tool::PI() / 2);
            }

            if(static_cast<ZorbitsOrbits*>(game())->_fastMode)  // Fast mode cheat
            {
                _planetMan->setGravityFactor(2);
            }
        }

        // Hardcoding this just to make sure.
        if(name().compare("tutorial") == 0)     _initZoom = 1.0;
        if(name().compare("highway") == 0)      _initZoom = 2.0;
        if(name().compare("wondercog") == 0)    _initZoom = 1.1;
        if(name().compare("vegas") == 0)        _initZoom = 1.5;
        if(name().compare("junkyard") == 0)     _initZoom = 1.5;
        if(name().compare("jungle") == 0)       _initZoom = 1.2;
        if(name().compare("roboruins") == 0)    _initZoom = 1.5;

        if(name().compare("jungle") == 0)
        {
            _jungleWaterPos = jl::Tool::b2Vec(sf::Vector2f(4428,4773));
            soundMan()->soundNew("waterfall", "resources/sounds/waterfall.wav", false);
            soundMan()->soundNew("boulder", "resources/sounds/boulder1.wav", false);
            soundMan()->soundNew("boulder2", "resources/sounds/boulder2.wav", false);
        }
        else _jungleWaterPos = b2Vec2(0,0);

        jl::TextureDesc::Ptr resetTex = game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/reset.png",
            sf::Vector2f(180, 22), 359, 43);
        _resetSprite.setTexture(*resetTex->texture());
        _resetSprite.setOrigin(resetTex->origin());
        _resetSprite.setTextureRect(resetTex->rect());
        _resetSprite.setPosition(this->game()->renderWindow()->getView().getCenter().x,
                                      this->game()->renderWindow()->getView().getCenter().y);
        _bullocked = false;
        _canBullock = true;

        jl::TextureDesc::Ptr partDesc = game()->textureMan()->textureDescIs("resources/zorbitsorbits/parts.png",
            sf::Vector2f(66, 90), 399, 360);
        _partSprite.setTexture(*partDesc->texture());
        _partSprite.setOrigin(partDesc->origin());
        _partSprite.setColor(sf::Color(255,255,255,0));
        b2BodyDef bodyDef;
        _partBody = b2world()->CreateBody(&bodyDef);
        _partPlaced = false;
        _partMove   = false;

        //initialize the camera
        _camera->initialize(_entityMan->entity("Zorbit"), true, _initZoom);

        //set minimum distance for sounds
        game()->soundMan()->setMinDistance(12);

        //variables - TODO these should be in JollyLava
        _joyLeft = false;
        _joyRight = false;

        _subTimer.resume();

        _scoreScreen = new ScoreScreen(this);
    }
    else throw fwk::InternalException("Level population was unsuccessful.");
}

void ZorbitLevel::reset()
{
    jl::Level::reset();
    _subTimer.reset(true);
    _bullocked = false;
}

void ZorbitLevel::processInputConcrete(sf::Event sfevent)
{
    if(!_entityMan->entity("Zorbit")) return;

    Zorbit::Ptr zorbit = static_cast<Zorbit*>(_entityMan->entity("Zorbit").ptr());

    std::string currentAction;
    if(!zorbit->actionMan()->noActions())
        currentAction = zorbit->actionMan()->peekAction()->name();
    else
        currentAction = "";

    if (sfevent.type == sf::Event::Closed)
    {
        game()->stateMan()->state("mainmenu")->powerDown();
        game()->stateMan()->stateDel("LoadingScreen");
        powerDown();
        _finished = true;
    }

    // Joystick
    if (sfevent.type == sf::Event::JoystickMoved && _playerInput)
    {
        // X Axis
        if (sfevent.joystickMove.axis == sf::Joystick::X)
        {
            if (sfevent.joystickMove.position > static_cast<ZorbitsOrbits*>(game())->xSensitivity())
            {
                zorbit->handle("Right");
                _joyRight = true;
            }
            if (sfevent.joystickMove.position < -1 * static_cast<ZorbitsOrbits*>(game())->xSensitivity())
            {
                zorbit->handle("Left");
                _joyLeft = true;
            }
            if (sfevent.joystickMove.position <= static_cast<ZorbitsOrbits*>(game())->xSensitivity())
            {
                if(_joyRight)
                {
                    _joyRight = false;
                    zorbit->actionMan()->endAction("MoveRight");
                    zorbit->_floatRight = false;
                    zorbit->_swingRight = false;
                    zorbit->_swingLeft = false;
                }
            }
            if (sfevent.joystickMove.position >= -1 * static_cast<ZorbitsOrbits*>(game())->xSensitivity())
            {
                if(_joyLeft)
                {
                    _joyLeft = false;
                    zorbit->actionMan()->endAction("MoveLeft");
                    zorbit->_floatLeft = false;
                    zorbit->_swingLeft = false;
                    zorbit->_swingRight = false;
                }
            }
            if(abs(sfevent.joystickMove.position) <= abs(static_cast<ZorbitsOrbits*>(game())->xSensitivity())) //Dead zone
            {
                _joyLeft = false;
                _joyRight = false;
                zorbit->_floatLeft = false;
                zorbit->_floatRight = false;
                zorbit->_swingLeft = false;
                zorbit->_swingRight = false;
                zorbit->actionMan()->endAction("MoveLeft");
                zorbit->actionMan()->endAction("MoveRight");
            }
        }
    }
    if (sfevent.type == sf::Event::JoystickButtonPressed && _playerInput)
    {
        if(sfevent.joystickButton.button == game()->settings()->button("Jump"))
        {
            if(_scoreScreen->active() && !_scoreScreen->fadeIn())
                _scoreScreen->finish();
            zorbit->handle("Jump");
        }
        if(sfevent.joystickButton.button == game()->settings()->button("Shoot"))
        {
            zorbit->shoot();
        }
        if(sfevent.joystickButton.button == game()->settings()->button("Grapple"))
        {
            if(game()->settings()->grappleSetting().compare("Toggle") == 0)
            {
                // Not moving this to Zorbit because
                // of dependency on local variable _grappleDown.
                if(zorbit->grappling() && !_grappleDown)
                {
                    zorbit->disconnectFromGrapple();
                    if(zorbit->grapPoints().size() > 1)
                        zorbit->connectToGrapple();
                    else zorbit->enableWheel();
                }
                else if(zorbit->closestGrapple() && !_grappleDown)
                {
                    zorbit->connectToGrapple();
                    zorbit->disableWheel();
                }

                _grappleDown = true;
            }
            else
            {
                if(!zorbit->grappling() && zorbit->closestGrapple())
                {
                    zorbit->connectToGrapple();
                    zorbit->disableWheel();
                }
            }
        }
        if(sfevent.joystickButton.button == 7)
        {
            //entityMan()->pauseAllChronometers();
            if(!_scoreScreen->active())
            {
               pause();
                static_cast<ZorbitMenu*>(game()->stateMan()->state("mainmenu").ptr())->selectionIs(Play);
                game()->stateMan()->activeStateIs("mainmenu");
            }
        }
    }
    if(sfevent.type == sf::Event::JoystickButtonReleased && _playerInput)
    {
        if(sfevent.joystickButton.button == game()->settings()->button("Grapple"))
        {
            if(game()->settings()->grappleSetting().compare("Toggle") == 0)
                _grappleDown = false;
            else
            {
                if(zorbit->grappling())
                {
                    zorbit->disconnectFromGrapple();
                    zorbit->enableWheel();
                }
            }
        }
    }

    // Keyboard
    if (sfevent.type == sf::Event::KeyPressed && _playerInput)
    {
        // Hardcoded keys
        switch(sfevent.key.code)
        {
            // DEMO
            
            case sf::Keyboard::Z:
                _camera->zoomOut();
                break;
            case sf::Keyboard::X:
                _camera->zoomIn();
                break;
            
            case sf::Keyboard::Escape:
                //entityMan()->pauseAllChronometers();
                if(_scoreScreen->active()) break;
                pause();
                static_cast<ZorbitMenu*>(game()->stateMan()->state("mainmenu").ptr())->selectionIs(Play);
                game()->stateMan()->activeStateIs("mainmenu");
                break;
                
            case sf::Keyboard::Num1:
                zorbit->setPosition(checkPointPosition(1));
                break;
            case sf::Keyboard::Num2:
                zorbit->setPosition(checkPointPosition(2));
                break;
            case sf::Keyboard::Num3:
                zorbit->setPosition(checkPointPosition(3));
                break;
            case sf::Keyboard::Num4:
                zorbit->setPosition(checkPointPosition(4));
                break;
            case sf::Keyboard::Num5:
                zorbit->setPosition(checkPointPosition(5));
                break;
            case sf::Keyboard::Num6:
                zorbit->setPosition(checkPointPosition(6));
                break;
            case sf::Keyboard::Num7:
                zorbit->setPosition(checkPointPosition(7));
                break;
            case sf::Keyboard::U:
                zorbit->setHealth(zorbit->_maxHealth);
                break;
            case sf::Keyboard::O:
                static_cast<jl::Character*>(entityMan()->entity(_bossName).ptr())->setHealth(1);
                break;
            case sf::Keyboard::Q:
                togglePause();
                break;
            case sf::Keyboard::Comma:
                finish();
                break;
            case sf::Keyboard::Period:
                zorbit->setHealth(0);
                break;
            case sf::Keyboard::Num8:
                soundMan()->mute();
                break;
            
            case sf::Keyboard::Return:
                if(_bullocked)
                {
                    zorbit->setHealth(0);
                    _bullocked = false;
                }
                if(_scoreScreen->active() && !_scoreScreen->fadeIn())
                    _scoreScreen->finish();
                break;
                /*
            case sf::Keyboard::P:
                std::cout << entityMan()->countType("Cog") << std::endl;
                break;*/
            default:
                break;
        }

        // Player-adjusted keys
        if(game()->settings()->key(sfevent.key.code).compare("NOT FOUND") != 0)
        {
            std::string keyboardKey = game()->settings()->key(sfevent.key.code);

            if(keyboardKey.compare("Right") == 0)
                zorbit->handle("Right");
            if(keyboardKey.compare("Left") == 0)
                zorbit->handle("Left");
            if(keyboardKey.compare("Jump") == 0)
                zorbit->handle("Jump");
            if(keyboardKey.compare("Shoot") == 0)
                zorbit->shoot();
            if(keyboardKey.compare("Grapple") == 0)
            {
                // Not moving this to Zorbit because
                // of dependency on local variable _grappleDown.
                if(zorbit->grappling() && !_grappleDown)
                {
                    zorbit->disconnectFromGrapple();
                    if(zorbit->grapPoints().size() > 1)
                        zorbit->connectToGrapple();
                    else zorbit->enableWheel();
                }
                else if(zorbit->closestGrapple() && !_grappleDown)
                {
                    zorbit->connectToGrapple();
                    zorbit->disableWheel();
                }
                _grappleDown = true;
            }
        }
    }
    if (sfevent.type == sf::Event::KeyReleased && _playerInput)
    {
        switch (sfevent.key.code)
        {
            case sf::Keyboard::LControl:
                _ctrl = false;
                break;
            default:
                break;
        }
        if(game()->settings()->key(sfevent.key.code).compare("NOT FOUND") != 0)
        {
            std::string keyboardKey = game()->settings()->key(sfevent.key.code);

            if(keyboardKey.compare("Right") == 0)
            {
                zorbit->actionMan()->endAction("MoveRight");
                zorbit->_floatRight = false;
                zorbit->_swingRight = false;
                zorbit->_swingLeft = false;
            }
            if(keyboardKey.compare("Left") == 0)
            {
                zorbit->actionMan()->endAction("MoveLeft");
                zorbit->_floatLeft = false;
                zorbit->_swingLeft = false;
                zorbit->_swingRight = false;
            }
            if(keyboardKey.compare("Grapple") == 0)
            {
                _grappleDown = false;
            }
        }
    }
}

void ZorbitLevel::togglePause()
{
    _update = !_update;

    if(!_update)
    {
        entityMan()->pauseAllChronometers();
        _paused = true;
    }
    else
    {
        entityMan()->resumeAllChronometers();
        _paused = false;
    }
}

void ZorbitLevel::pauseConcrete()
{
    _update = false;
    entityMan()->pauseAllChronometers();
}

void ZorbitLevel::unpauseConcrete()
{
    _update = true;
    entityMan()->resumeAllChronometers();
}

void ZorbitLevel::generateBoss()
{
    if(name().compare("tutorial") == 0)
    {
        _entityMan->entityNew("TutorialBoss", "Layer1", new ZTutorialBoss(this, _layerNames, _bossPos, _bossAngle));
        _bossName = "TutorialBoss";
    }
    else if(name().compare("wondercog") == 0)
    {
        _entityMan->entityNew("FunzoBoss", "Layer1", new ZFunzoBoss(this, _layerNames, _bossPos, _bossAngle));
        _bossName = "FunzoBoss";
    }
    else if(name().compare("highway") == 0)
    {
        _entityMan->entityNew("VexBoss", "Layer1", new ZVexBoss(this, _layerNames, _bossPos, _bossAngle));
        _bossName = "VexBoss";
    }
    else if(name().compare("vegas") == 0)
    {
        _entityMan->entityNew("ElvisBoss", "Layer1", new ZElvisBoss(this, _layerNames, _bossPos, _bossAngle));
        _bossName = "ElvisBoss";
    }
    else if(name().compare("junkyard") == 0)
    {
        _entityMan->entityNew("StrongoBoss", "Layer1", new ZStrongoBoss(this, _layerNames, _bossPos, _bossAngle));
        _bossName = "StrongoBoss";
    }
    else if(name().compare("jungle") == 0)
    {
        _entityMan->entityNew("KrawlurBoss", "Layer1", new ZKrawlurBoss(this, _layerNames, _bossPos, _bossAngle));
        _bossName = "KrawlurBoss";
    }
    else if(name().compare("boss") == 0)
    {
        _entityMan->entityNew("GlobBoss", "Layer1", new ZGlobBoss(this, _layerNames, _bossPos, _bossAngle));
        _bossName = "GlobBoss";
    }
}

void ZorbitLevel::update()
{
    if(_scoreScreen->totallyFinished())
    {
        finish();
    }

    if(_scoreScreen->active()) _scoreScreen->update();

    if(!_firstTouch)
    {
        _timer.reset();
    }
    if(!_update || _finished)
    {
        _timer.pause();
        _subTimer.pause();
    }
    else
    {
        if(!_scoreScreen->active())
            _timer.resume();
        _subTimer.resume();
        _resetSprite.setPosition(this->game()->renderWindow()->getView().getCenter().x,
                                      this->game()->renderWindow()->getView().getCenter().y);
    }

    // UGH
    if(name().compare("jungle") == 0)
        soundMan()->playSound("waterfall", distanceToZorbit(_jungleWaterPos));

    // More ugh
    _partSprite.setPosition(jl::Tool::sfVec(_partBody->GetPosition()));

    if(_partPlaced && _partSprite.getColor().a < 252)
        _partSprite.setColor(sf::Color(255,255,255,_partSprite.getColor().a + 3));
    if(_partPlaced && _partSprite.getColor().a >= 252)
    {
        _partSprite.setColor(sf::Color(255,255,255,255));
        _partMove = true;
        _partPlaced = false;
    }
    if(_partMove
       && abs(_partBody->GetPosition().x - entityMan()->entity("Zorbit")->position().x) <= 0.25
       && abs(_partBody->GetPosition().y - entityMan()->entity("Zorbit")->position().y) <= 0.25)
    {
        _partSprite.setColor(sf::Color(255,255,255,0));
        soundMan()->playSound("zorbit_cog_pickup");
    }
    else if(_partMove)
    {
        float diffX = _partBody->GetPosition().x - entityMan()->entity("Zorbit")->position().x;
        float diffY = _partBody->GetPosition().y - entityMan()->entity("Zorbit")->position().y;

        _partBody->SetTransform(b2Vec2(_partBody->GetPosition().x - diffX / 35, _partBody->GetPosition().y - diffY / 35), 0);
    }

    Level::update();
}

void ZorbitLevel::draw()
{
    if(!_finished)
    {
        _spaceGen->draw();

        Level::draw();
        game()->renderWindow()->setView(_camera->getView());
        game()->renderWindow()->draw(_partSprite);
        game()->renderWindow()->setView(game()->renderWindow()->getDefaultView());
        if(_bullocked) game()->renderWindow()->draw(_resetSprite);
        if(_scoreScreen->active()) _scoreScreen->draw();
    }

}

void ZorbitLevel::customObjectNew(std::string name, std::string type, std::string layer, int x, int y, float angle, std::string extra, std::string extra2, std::string extra3)
{
    if(!_entityMan->entity(name))
    {
        if(type.compare("Image") == 0)
        {
            if(!_entityMan->background(name))
            {
                if(extra.compare("levels/highway/images/fire.png") == 0
                   || extra.compare("levels/wondercog/images/fire.png") == 0
                   || extra.compare("levels/junkyard/images/fire.png") == 0
                   || extra.compare("levels/roboruins/images/fire.png") == 0
                   || extra.compare("levels/roboruins/images/shortfire.png") == 0)
                {
                    _entityMan->backgroundNew(name, layer,new jl::Background(name, this, extra, sf::Vector2f(x, y), angle, true), true);
                    _entityMan->background(name)->defineAnimation(sf::IntRect(0,0,223,219),4);
                }
                if(extra.compare("levels/roboruins/images/albot.png") == 0)
                {
                    _entityMan->backgroundNew(name, layer, new jl::Background(name, this, extra, sf::Vector2f(x, y), angle, true), false);
                    _entityMan->background(name)->defineAnimation(sf::IntRect(0,0,140,200),5, 0.1);
                }
            }
        }
        if(type.compare("Boss") == 0)
        {
            _bossPos = jl::Tool::b2Vec(sf::Vector2f(x, y));
            _bossAngle = angle;
        }
        if(type.compare("HappyRock") == 0)
            _entityMan->entityNew(name, layer, new ZHappyRock(name, this, _layerNames, jl::Tool::b2Vec(sf::Vector2f(x, y)), angle));
        if(type.compare("Glob") == 0)
            _entityMan->entityNew(name, layer, new ZGlob(name, this, _layerNames, jl::Tool::b2Vec(sf::Vector2f(x, y)), angle));
        if(type.compare("GlobBat") == 0)
            _entityMan->entityNew(name, layer, new ZGlobBat(name, this, _layerNames, jl::Tool::b2Vec(sf::Vector2f(x, y)), angle));
        if(type.compare("GlobDrop") == 0)
        {
            if(extra.compare("true") == 0)
                _entityMan->entityNew(name, layer, new ZGlobDrop(name, this, _layerNames, jl::Tool::b2Vec(sf::Vector2f(x, y)), angle, true));
            else
                _entityMan->entityNew(name, layer, new ZGlobDrop(name, this, _layerNames, jl::Tool::b2Vec(sf::Vector2f(x, y)), angle, false));
        }
        if(type.compare("Grapple") == 0)
        {
            if(extra.compare("true") == 0)
                _entityMan->entityNew(name, "Layer1", new ZGrapple(name, this, _layerNames, jl::Tool::b2Vec(sf::Vector2f(x, y)), angle, true));
            else
                _entityMan->entityNew(name, "Layer1", new ZGrapple(name, this, _layerNames, jl::Tool::b2Vec(sf::Vector2f(x, y)), angle, false));
        }

        if(type.compare("Boinger") == 0)
        {
            if(extra2.compare("true") == 0)
                _entityMan->entityNew(name, layer, new ZBoinger(name, this, _layerNames, jl::Tool::b2Vec(sf::Vector2f(x, y)), angle, atof(extra.c_str()), true));
            else
                _entityMan->entityNew(name, layer, new ZBoinger(name, this, _layerNames, jl::Tool::b2Vec(sf::Vector2f(x, y)), angle, atof(extra.c_str()), false));
        }
        if(type.compare("Booster") == 0)
            _entityMan->entityNew(name, layer, new ZBooster(name, this, _layerNames, jl::Tool::b2Vec(sf::Vector2f(x, y)), angle, atof(extra.c_str())));
        if(type.compare("Cog") == 0)
        {
            _entityMan->entityNew(name, layer, new ZCog(name, this, _layerNames, jl::Tool::b2Vec(sf::Vector2f(x, y)), angle));
        }
        if(type.compare("Platform") == 0)
            _entityMan->entityNew(name, layer, new ZPlatform(name, this, _layerNames, jl::Tool::b2Vec(sf::Vector2f(x, y)), angle, extra));
        if(type.compare("Slot") == 0)
            _entityMan->entityNew(name, layer, new ZSlot(name, this, _layerNames, jl::Tool::b2Vec(sf::Vector2f(x, y)), angle));
        if(type.compare("Kops") == 0)
            _entityMan->entityNew(name, layer, new ZKops(name, this, _layerNames, jl::Tool::b2Vec(sf::Vector2f(x, y)), angle));
        if(type.compare("CircleShape") == 0)
        {
            if(this->name().compare("vegas") == 0 && (name.compare("CircleShape0") == 0 || name.compare("CircleShape8") == 0))
                _entityMan->entityNew(name, layer, new ZCircle(name, this, _layerNames, extra3, jl::Tool::b2Vec(sf::Vector2f(x, y)), angle, jl::Tool::pixels2meters(atof(extra.c_str())), extra2, true));
            else if(this->name().compare("jungle") == 0)
                _entityMan->entityNew(name, layer, new ZCircle(name, this, _layerNames, extra3, jl::Tool::b2Vec(sf::Vector2f(x, y)), angle, jl::Tool::pixels2meters(atof(extra.c_str())), extra2, true));
            else
                _entityMan->entityNew(name, layer, new ZCircle(name, this, _layerNames, extra3, jl::Tool::b2Vec(sf::Vector2f(x, y)), angle, jl::Tool::pixels2meters(atof(extra.c_str())), extra2));
        }
        if(type.compare("RectShape") == 0)
            _entityMan->entityNew(name, layer, new ZRectShape(name, this, _layerNames, jl::Tool::b2Vec(sf::Vector2f(x, y)), angle, jl::Tool::pixels2meters(atof(extra.c_str())) / 2, jl::Tool::pixels2meters(atof(extra2.c_str())) / 2, extra3.c_str()));
        if(type.compare("MovingPlatform") == 0)
        {
            b2Vec2 b2position = jl::Tool::b2Vec(sf::Vector2f(x + atof(extra.c_str()) / 2,
                                                             y + atof(extra2.c_str()) / 2));

            _entityMan->entityNew(name, layer, new ZMovingPlatform(name,
                                                                   this,
                                                                   _layerNames,
                                                                   b2position,
                                                                   angle,
                                                                   sf::FloatRect(b2position.x,
                                                                                 b2position.y,
                                                                                 jl::Tool::pixels2meters(atof(extra.c_str())) / 2,
                                                                                 jl::Tool::pixels2meters(atof(extra2.c_str())) / 2),
                                                                   false,
                                                                   extra3));
        }
    }
}

void ZorbitLevel::handleEvent(std::string actor, std::string action, std::string value)
{
    if(actor.compare("Level") == 0)
    {
        if(action.compare("SetCheckPoint") == 0)
            setCheckPoint(value);
        if(action.compare("Beaten") == 0)
        {
            _canBullock = false;
            _wholeLevelBeaten = true;
            _timer.pause();
            _scoreScreen->activeIs(true);
            _scoreScreen->setStats(name(),
                               game()->difficulty(),
                               jl::Tool::toString(static_cast<Zorbit*>(entityMan()->entity("Zorbit").ptr())->currentCogs()),
                               CogHUD::timeString(levelTime()),
                               _happyRock);
        }
    }
    //TODO - does this ever get used?
    if(actor.compare("Collision") == 0)
    {
        if(action.compare("Zorbit") == 0)
        {
            if(value.compare("Die") == 0)
                static_cast<Zorbit*>(entityMan()->entity(action).ptr())->setHealth(0);
        }
    }
    if(actor.compare("Boss") == 0)
    {
        if(action.compare("Activate") == 0)
            static_cast<ZBoss*>(entityMan()->entity(_bossName).ptr())->activate();
    }
    if(actor.compare("ZElvisBoss") == 0)
    {
        if(action.compare("Limit") == 0)
        {
            if(value.compare("true") == 0)
                static_cast<ZElvisBoss*>(entityMan()->entity("ElvisBoss").ptr())->limitIs(true);
        }
    }
    if(actor.compare("Planet") == 0)
    {
        if(value.compare("SensorOn") == 0)
            static_cast<jl::Planet*>(planetMan()->planet(action).ptr())->sensorOn();
        if(value.compare("SensorOff") == 0)
            static_cast<jl::Planet*>(planetMan()->planet(action).ptr())->sensorOff();
    }
    if(actor.compare("Cinematic") == 0)
    {
        if(static_cast<ZorbitsOrbits*>(game())->currentGameMode() != classic) return;
        if(action.compare("Play") == 0)
        {
            if(value.compare("hb1") == 0)
            {
                game()->stateMan()->stateNew(new Cinematic("hb1", game(), "resources/zorbitsorbits/cinematics/hb1.png", "hb2"));
                game()->stateMan()->stateNew(new Cinematic("hb2", game(), "resources/zorbitsorbits/cinematics/hb2.png", "hb3"));
                game()->stateMan()->stateNew(new Cinematic("hb3", game(), "resources/zorbitsorbits/cinematics/hb3.png", "highway"));
                game()->stateMan()->activeStateIs("hb1");
            }
            if(value.compare("fb1") == 0)
            {
                game()->stateMan()->stateNew(new Cinematic("fb1", game(), "resources/zorbitsorbits/cinematics/fb1.png", "fb2"));
                game()->stateMan()->stateNew(new Cinematic("fb2", game(), "resources/zorbitsorbits/cinematics/fb2.png", "wondercog"));
                game()->stateMan()->activeStateIs("fb1");
            }
            if(value.compare("eb1") == 0)
            {
                game()->stateMan()->stateNew(new Cinematic("eb1", game(), "resources/zorbitsorbits/cinematics/eb1.png", "eb2"));
                game()->stateMan()->stateNew(new Cinematic("eb2", game(), "resources/zorbitsorbits/cinematics/eb2.png", "vegas"));
                game()->stateMan()->activeStateIs("eb1");
            }
            if(value.compare("sb1") == 0)
            {
                game()->stateMan()->stateNew(new Cinematic("sb1", game(), "resources/zorbitsorbits/cinematics/sb1.png", "sb2"));
                game()->stateMan()->stateNew(new Cinematic("sb2", game(), "resources/zorbitsorbits/cinematics/sb2.png", "junkyard"));
                game()->stateMan()->activeStateIs("sb1");
            }
            if(value.compare("kb1") == 0)
            {
                game()->stateMan()->stateNew(new Cinematic("kb1", game(), "resources/zorbitsorbits/cinematics/kb1.png", "kb2"));
                game()->stateMan()->stateNew(new Cinematic("kb2", game(), "resources/zorbitsorbits/cinematics/kb2.png", "kb3"));
                game()->stateMan()->stateNew(new Cinematic("kb3", game(), "resources/zorbitsorbits/cinematics/kb3.png", "kb4"));
                game()->stateMan()->stateNew(new Cinematic("kb4", game(), "resources/zorbitsorbits/cinematics/kb4.png", "jungle"));
                game()->stateMan()->activeStateIs("kb1");
            }
        }
    }
    if(entityMan()->background(actor))
    {
        // TODO - This doesn't work.
        if(action.compare("SetColor") == 0)
        {
            if(value.compare("Red") == 0)
                entityMan()->background(actor)->setColor(sf::Color(255,0,0, 0));
            if(value.compare("Blue") == 0)
                entityMan()->background(actor)->setColor(sf::Color(0,0,255, 0));
            if(value.compare("Green") == 0)
                entityMan()->background(actor)->setColor(sf::Color(0,255,0, 0));
        }
    }
    if(entityMan()->entity(actor))
    {
        if(!entityMan()->entity(actor))
            return;

        if(action.compare("SetPosition") == 0)
        {
            sf::Vector2f pos = sf::Vector2f(atof(value.substr(0, value.find(",")).c_str()),
                                                atof(value.substr(value.find(",") + 1, value.rfind(",") - value.find(",") - 1).c_str()));
            float angle = atof(value.substr(value.rfind(",") + 1).c_str());
            entityMan()->entity(actor)->rootBody()->b2body()->SetTransform(jl::Tool::b2Vec(pos), angle);
        }
        //Zorbit
        if(entityMan()->entity(actor)->type().compare("Player") == 0)
        {
            if(action.compare("FootSensors") == 0)
                static_cast<Zorbit*>(entityMan()->entity(actor).ptr())->footContactsIs(atoi(value.c_str()));
            if(action.compare("SetMaxSpeed") == 0)
                static_cast<Zorbit*>(entityMan()->entity(actor).ptr())->setMaxSpeed(atof(value.c_str()));
            if(action.compare("Die") == 0)
                static_cast<Zorbit*>(entityMan()->entity(actor).ptr())->setHealth(0);
        }
        if(entityMan()->entity(actor)->type().compare("Circle") == 0)
        {
            if(action.compare("SwitchSpinDirection") == 0)
                static_cast<ZCircle *>(entityMan()->entity(actor).ptr())->switchSpinDirection();
            if(action.compare("ChangeSpinSpeed") == 0)
                static_cast<ZCircle *>(entityMan()->entity(actor).ptr())->changeSpinSpeed(atof(value.c_str()));
            if(action.compare("SetMass") == 0)
                static_cast<ZCircle *>(entityMan()->entity(actor).ptr())->setMass(atof(value.c_str()));
            if(action.compare("SetDensity") == 0)
                static_cast<ZCircle *>(entityMan()->entity(actor).ptr())->setDensity(atof(value.c_str()));
            if(action.compare("SetSensor") == 0)
            {
                if(value.compare("True") == 0) static_cast<ZCircle *>(entityMan()->entity(actor).ptr())->setSensor(true);
                else static_cast<ZCircle *>(entityMan()->entity(actor).ptr())->setSensor(false);
            }
            if(action.compare("StartStopSwitch") == 0)
            {
                // Stop a spinning wheel.
                if(static_cast<ZCircle *>(entityMan()->entity(actor).ptr())->spinning())
                    static_cast<ZCircle *>(entityMan()->entity(actor).ptr())->changeSpinSpeed(0);
                // Start a stopped wheel.
                else
                {
                    static_cast<ZCircle *>(entityMan()->entity(actor).ptr())->changeSpinSpeed(-1);
                    static_cast<ZCircle *>(entityMan()->entity(actor).ptr())->switchSpinDirection();
                }
            }
            if(action.compare("DefineLimits") == 0)
            {
                int lower = atof(value.substr(0, value.find(",")).c_str());
                int upper = atof(value.substr(value.find(",") + 1).c_str());
                static_cast<ZCircle *>(entityMan()->entity(actor).ptr())->defineLimits(lower, upper);
            }
            if(action.compare("Reset") == 0)
            {
                static_cast<ZCircle *>(entityMan()->entity(actor).ptr())->reset();
            }
            if(action.compare("Freeze") == 0)
            {
                static_cast<ZCircle *>(entityMan()->entity(actor).ptr())->defineLimits((entityMan()->entity(actor).ptr()->angle() + jl::Tool::PI() / 2) - 0.05,
                                                                                       entityMan()->entity(actor).ptr()->angle() + jl::Tool::PI() / 2 + 0.05);
            }
            if(action.compare("Fade") == 0)
            {
                static_cast<ZCircle *>(entityMan()->entity(actor).ptr())->fade();
            }
        }
        if(entityMan()->entity(actor)->type().compare("MovingPlatform") == 0)
        {
            if(action.compare("SwitchDirection") == 0)
                static_cast<ZMovingPlatform *>(entityMan()->entity(actor).ptr())->switchDirection();
            if(action.compare("SetDirection") == 0)
                static_cast<ZMovingPlatform *>(entityMan()->entity(actor).ptr())->setDirection(value.c_str());
            if(action.compare("ChangeMoveSpeed") == 0)
                static_cast<ZMovingPlatform *>(entityMan()->entity(actor).ptr())->changeMoveSpeed(atof(value.c_str()));
            if(action.compare("StartStopSwitch") == 0)
            {
                if(static_cast<ZMovingPlatform *>(entityMan()->entity(actor).ptr())->moving())
                    static_cast<ZMovingPlatform *>(entityMan()->entity(actor).ptr())->changeMoveSpeed(0);
                else
                {
                    static_cast<ZMovingPlatform *>(entityMan()->entity(actor).ptr())->changeMoveSpeed(atof(value.c_str()));
                    static_cast<ZMovingPlatform *>(entityMan()->entity(actor).ptr())->switchDirection();
                }
            }
        }
        if(entityMan()->entity(actor)->type().compare("Slot") == 0)
        {
            if(action.compare("Activate") == 0)
                static_cast<ZSlot *>(entityMan()->entity(actor).ptr())->activate();
        }
    }
}

void ZorbitLevel::setCheckPoint(std::string input)
{
    if(eventMan()->event(input))
        _checkPoint = input;
    else throw fwk::InternalException("Checkpoint does not exist!!");
}

b2Vec2 ZorbitLevel::checkPointPosition() const
{
    if(_checkPoint.compare("None") != 0)
        return _eventMan->event(_checkPoint)->location();
    else return _startingPos;
}

b2Vec2 ZorbitLevel::checkPointPosition(int checkpoint) const
{
    if(checkpoint == 0)
        return _startingPos;

    std::stringstream ss;
    ss << "checkpoint" << checkpoint;
    if(!_eventMan->event(ss.str()))
       return _startingPos;
    else return _eventMan->event(ss.str())->location();
}

void ZorbitLevel::stopInput()
{
    static_cast<Zorbit*>(_entityMan->entity("Zorbit").ptr())->stopCompletely();

    Level::stopInput();
}

void ZorbitLevel::resumeInput()
{
    static_cast<Zorbit*>(_entityMan->entity("Zorbit").ptr())->unStop();

    Level::resumeInput();
}

float ZorbitLevel::distanceToZorbit(b2Vec2 from)
{
    return abs(jl::Tool::distance(from, _entityMan->entity("Zorbit")->position()));
}

void ZorbitLevel::generateFadeTexture()
{
    jl::TextureDesc::Ptr fadeTex;
    fadeTex = game()->textureMan()->textureDescIs("resources/zorbitsorbits/black.png",
            sf::Vector2f(1, 1), 1, 1);
    _fadeSprite.setTexture(*game()->textureMan()->textureDesc("resources/zorbitsorbits/black.png")->texture());
    _fadeSprite.setScale(game()->videoMode().width, game()->videoMode().height);
}

void ZorbitLevel::finish()
{
    soundMan()->stopMusic();
    if(static_cast<ZorbitsOrbits*>(game())->currentGameMode() != classic)
        soundMan()->playMusic("zorbit");

    std::string nextLevel = game()->nextLevelName();
    if(nextLevel.compare("reset") != 0)
    {
        static_cast<ZorbitsOrbits*>(game())->currentHealthIs(static_cast<Zorbit*>(entityMan()->entity("Zorbit").ptr())->currentHealth());
        static_cast<LoadingScreen*>(game()->stateMan()->state("LoadingScreen").ptr())->levelNameIs(nextLevel);
    }
    else
    {
        //static_cast<ZorbitsOrbits*>(game()->ptr())->clearLevelNames();
        static_cast<LoadingScreen*>(game()->stateMan()->state("LoadingScreen").ptr())->levelNameIs("reset");
        static_cast<ZorbitsOrbits*>(game())->currentLevelIs(uninitialized);
    }

    game()->stateMan()->queueState("LoadingScreen");

/*
    if(static_cast<ZorbitsOrbits*>(game())->gameOver())
        game()->stateMan()->queueState("LoadingScreen");
    else
    {
        if(!game()->stateMan()->state("scorescreen")) game()->stateMan()->stateNew(new ScoreScreen(static_cast<ZorbitsOrbits*>(game())));
        game()->stateMan()->queueState("scorescreen");
    }*/

    Level::finish();
}

void ZorbitLevel::foundHR()
{
    _happyRock = true;
    static_cast<ZorbitsOrbits*>(game())->foundHR(name());
}

void ZorbitLevel::resetHR()
{
    if(!_happyRock)
        return;
    else
    {
        _happyRock = false;
        _entityMan->entityNew("HappyRock", "Layer1", new ZHappyRock("HappyRock", this, _layerNames, _happyRockPos, _happyRockAngle));
        static_cast<ZorbitsOrbits*>(game())->unFoundHR(name());
    }
}

void ZorbitLevel::print()
{
    game()->stateMan()->print();
}

void ZorbitLevel::bullockedIs(bool value)
{
    if(!_canBullock) return;

    _bullocked = value;
}

void ZorbitLevel::placePart(b2Vec2 position, float angle)
{
    _partBody->SetTransform(position, angle);
    _partSprite.setRotation(angle);
    if(name().compare("highway") == 0)
        _partSprite.setTextureRect(sf::IntRect(0,0,133,180));
    if(name().compare("wondercog") == 0)
        _partSprite.setTextureRect(sf::IntRect(134,0,133,180));
    if(name().compare("vegas") == 0)
        _partSprite.setTextureRect(sf::IntRect(267,0,133,180));
    if(name().compare("junkyard") == 0)
        _partSprite.setTextureRect(sf::IntRect(0,180,133,180));
    if(name().compare("jungle") == 0)
        _partSprite.setTextureRect(sf::IntRect(134,180,133,180));
    _partPlaced = true;
}

void ZorbitLevel::onZeroReferences() const
{
    delete _spaceGen;
    delete _scoreScreen;
    b2world()->DestroyBody(_partBody);

    Level::onZeroReferences();
}

