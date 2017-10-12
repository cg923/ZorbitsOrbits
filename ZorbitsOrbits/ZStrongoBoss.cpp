
// Copyright(c) 2017 Happy Rock Studios

#include "ZStrongoBoss.h"
#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "Zorbit.h"
#include "JollyLava/Tool.h"
#include "ZShot.h"
#include "ZPlatform.h"

ZStrongoBoss::ZStrongoBoss(ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle) : ZBoss("StrongoBoss",
            level, layerNames)
{
    _prefix = "sboss";

    // Body texture
    jl::TextureDesc::Ptr bodyTex;
    bodyTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/strongoboss/strongoboss.png",
            sf::Vector2f(143, 195), 286, 391);

    // Blank texture, used for thrower position
    jl::TextureDesc::Ptr throwerTex;
    throwerTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/blank.png",
            sf::Vector2f(1, 1), 1, 1);

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("ZStrongoBossBody", "foreground", bodyTex, &bodyDef, initPos, initAngle);

    // Create thrower body.
    float xDistToGun = 0; //cos(jl::Tool::b2Angle(initAngle));
    float yDistToGun = 5; //100 * sin(jl::Tool::b2Angle(initAngle));
    bodyNew("thrower", "foreground", throwerTex, &bodyDef, initPos + b2Vec2(xDistToGun, yDistToGun), 0);

    // Create foot body.
    yDistToGun = -3;
    bodyNew("ZStrongoFootBody", "foreground", throwerTex, &bodyDef, initPos + b2Vec2(xDistToGun, yDistToGun), 0);

    float width = 2.0f;
    float height = 3.0f;

    // Set values
    _active         = false;
    _maxHealth      = 250;   //200
    _currentHealth  = _maxHealth;
    _damage         = 1;
    _numShots       = 1;
    _maxSpeed       = 13;
    _jumpHeight     = 0;
    _lFootContacts  = 0;
    _rFootContacts  = 0;
    _minShotIntervalInSeconds = 3;
    _attacking      = false;
    _intro          = false;
    _destroyingJoints = true;
    _seesZorbit     = false;
    _phase2         = false;
    _phase3         = false;
    directedRightIs(false);

    defineFixtures(width, height);
    defineAnimations();
    defineSounds();
    defineActions();

    // Add chronometers to chronometer list
    _chronos["shottimer"] = _shotTimer;
    _chronos["actiontimer"] = _actionTimer;
    _chronos["poundtimer"] = _poundTimer;
    _chronos["poundtimer2"] = _poundTimer2;
    _chronos["shottimer"].resume();
    _chronos["actiontimer"].resume();
    _chronos["poundtimer"].resume();
}

void ZStrongoBoss::defineFixtures(float width, float height)
{
    // Torso fixture
    b2FixtureDef fixDef;
    b2PolygonShape fixShape;
    fixShape.SetAsBox(width, height);
    fixDef.shape = &fixShape;
    fixDef.density = 1.0f;
    fixDef.friction = 0.0f;
    fixDef.restitution = 0.0f;

    jl::FixtureUserData::Ptr fixUD = new jl::FixtureUserData("Root");
    rootBody()->fixtureNew(fixUD, &fixDef);

    fixShape.SetAsBox(width / 4, height / 4);
    fixDef.shape = &fixShape;
    fixDef.density = 0.0f;
    fixDef.isSensor = true;
    jl::FixtureUserData::Ptr fixUD2 = new jl::FixtureUserData("Thrower");
    body("thrower")->fixtureNew(fixUD2, &fixDef);

    // Thrower joint.
    b2RevoluteJointDef tJointDef;
    tJointDef.Initialize(body("ZStrongoBossBody")->b2body(), body("thrower")->b2body(), body("thrower")->b2body()->GetPosition());
    _throwerJoint = (b2RevoluteJoint *)this->level()->b2world()->CreateJoint(&tJointDef);

    tJointDef.Initialize(body("ZStrongoBossBody")->b2body(), body("ZStrongoFootBody")->b2body(), body("ZStrongoFootBody")->b2body()->GetPosition());
    _footJoint = (b2RevoluteJoint *)this->level()->b2world()->CreateJoint(&tJointDef);

    jl::Character::defineFixtures(width, height);
}

void ZStrongoBoss::defineAnimations()
{
    _knownAnims["Stop"]             = new jl::Animation("Stop", sf::IntRect(0, 0, 286, 391), 1, true);
    _knownAnims["Appear"]           = new jl::Animation("Appear", sf::IntRect(0, 391, 286, 391), 2, false);
    _knownAnims["MoveRight"]        = new jl::Animation("MoveRight", sf::IntRect(286, 0, 286, 391), 4, true);
    _knownAnims["MoveLeft"]         = _knownAnims["MoveRight"];
    _knownAnims["Attack"]           = new jl::Animation("Attack", sf::IntRect(1430, 0, 250, 391), 1, false);
    _knownAnims["Pound"]            = new jl::Animation("Pound", sf::IntRect(572, 391, 423, 391), 4, false, 250);
    _knownAnims["Die"]              = new jl::Animation("Die", sf::IntRect(0, 783, 483, 391), 8, false, 80);

    currentAnimIs(_knownAnims["Stop"]);
}

void ZStrongoBoss::defineSounds()
{
    level()->soundMan()->soundNew("sboss_hit", "resources/sounds/tboss_hit.wav", false);
    level()->soundMan()->soundNew("sboss_die", "resources/sounds/sboss_die.wav", false);
    level()->soundMan()->soundNew("sboss_intro", "resources/sounds/sboss_intro.wav", false);
    level()->soundMan()->soundNew("sboss_throw", "resources/sounds/sboss_throw.wav", false);
    level()->soundMan()->soundNew("sboss_pound", "resources/sounds/sboss_pound.wav", false);
    level()->soundMan()->soundNew("sboss_trash_land", "resources/sounds/sboss_trash_land.wav", false);
}

void ZStrongoBoss::defineActions()
{
    _knownActions["MoveRight"]          = new jl::cMRightAction(this);
    _knownActions["MoveLeft"]           = new jl::cMLeftAction(this);
    _knownActions["Attack"]             = new sbAttackAction(this);
    _knownActions["Pound"]              = new sbPoundAction(this);
    _knownActions["Stop"]               = new jl::aStopAction(this);
    _knownActions["Appear"]             = new jl::aStopAction(this);
    _knownActions["Die"]                = new bDieAction("tb_die", this);
}

void ZStrongoBoss::reset()
{
    ZBoss::reset();
    _phase2 = false;
    _phase3 = false;
    _minShotIntervalInSeconds = 3;
    _maxSpeed = 13;

    if(!directedRight())
    {
        switchDirection();
        directedRightIs(true);
    }

    _chronos["shottimer"].reset(true);
    _chronos["actiontimer"].reset(true);
    _chronos["poundtimer"].reset(true);
}

void ZStrongoBoss::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    ZBoss::beginContact(contact, thisFixUD, thatCollidable, thatFixUD);

    // Get the type of the other collidable for comparisons.
    string thatType = thatCollidable->type();
    if(thatType.compare("ZShot") == 0
       && thisFixUD->name().compare("Root") == 0)
    {
        static_cast<ZShot *>(thatCollidable.ptr())->toDie();
        decreaseHealth(static_cast<ZShot *>(thatCollidable.ptr())->damage());
        hit();

        if(level()->soundMan()->sound("sboss_hit"))
            level()->soundMan()->playSound("sboss_hit", true);
    }
    if(thatType.compare("Player") == 0
       && thatFixUD->name().compare("Root") == 0
       && thisFixUD->name().compare("Root") == 0)
    {
        static_cast<Zorbit *>(thatCollidable.ptr())->hit(_damage);
    }
}

void ZStrongoBoss::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    ZBoss::endContact(contact, thisFixUD, thatCollidable, thatFixUD);
}

void ZStrongoBoss::updateConcrete()
{
    if(!noActions() && actionMan()->peekAction()->name().compare("Pound") != 0
       && actionMan()->peekAction()->name().compare("Die") != 0)
        rootBody()->sprite()->setOrigin(143, 195);
    else if(!noActions() && actionMan()->peekAction()->name().compare("Die") == 0)
        rootBody()->sprite()->setOrigin(241, 195);

    if(gravityField())
        rootBody()->b2body()->SetTransform(position(), polarAngle() - jl::Tool::PI() / 2);

    if(_currentHealth <= 0
       && _destroyingJoints)
    {        std::list<std::string>::iterator itr;
        for(itr = _trash.begin(); itr != _trash.end(); itr++)
        {
            if(level()->entityMan()->entity(*itr))
            {
                static_cast<SBTrash*>(level()->entityMan()->entity(*itr).ptr())->breakJoints();
                level()->entityMan()->entity(*itr).ptr()->die();
            }
        }
        _destroyingJoints = false;
    }
    else if (_currentHealth > 0)
    {
        if(noActions())
            _actionMan->pushAction(_knownActions["Stop"]);

        if(!_active)
        {
            rootBody()->sprite()->setTextureRect(sf::IntRect(0, 391, 286, 391));
            _chronos["actiontimer"].reset(true);
        }
        else
        {
            if(!_intro)
            {
                rootBody()->sprite()->setTextureRect(sf::IntRect(286, 391, 286, 391));
                level()->soundMan()->playSound("sboss_intro", false);
                if(_chronos["actiontimer"].getElapsedTime().asSeconds() > 2)
                {
                    _intro = true;
                    _chronos["actiontimer"].reset(true);
                }
            }
            else
            {
                if(_currentHealth >= _maxHealth * 0.6)
                    phase1(directedRight());
                else if(_currentHealth >= _maxHealth * 0.3
                        && _currentHealth < _maxHealth * 0.6)
                    phase2(directedRight());
                else if(_currentHealth < _maxHealth * 0.3)
                    phase3(directedRight());
            }
        }
    }
}

void ZStrongoBoss::phase1(bool right)
{
    if(right)
    {
        // STOP
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() < 1
            && _actionMan->peekAction()->name().compare("Stop") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Stop"]);
        }
        // MOVE RIGHT
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 1
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 4
            && _actionMan->peekAction()->name().compare("MoveRight") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["MoveRight"]);
        }
        // STOP
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 4
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 4.5
            && _actionMan->peekAction()->name().compare("Stop") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Stop"]);
        }
        // Attack
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 4.5
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 5.5
            && _actionMan->peekAction()->name().compare("Attack") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Attack"]);
        }
        // STOP
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 5.5
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 6
            && _actionMan->peekAction()->name().compare("Stop") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Stop"]);
        }
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 6)
        {
            randomDirection();
            _chronos["actiontimer"].reset(true);
        }
    }
    else
    {
        // STOP
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() < 1
            && _actionMan->peekAction()->name().compare("Stop") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Stop"]);
        }
        // MOVE RIGHT
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 1
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 4
            && _actionMan->peekAction()->name().compare("MoveLeft") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["MoveLeft"]);
        }
        // STOP
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 4
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 4.5
            && _actionMan->peekAction()->name().compare("Stop") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Stop"]);
        }
        // THROW
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 4.5
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 5.5
            && _actionMan->peekAction()->name().compare("Attack") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Attack"]);
        }
        // STOP
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 5.5
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 6
            && _actionMan->peekAction()->name().compare("Stop") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Stop"]);
        }
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 6)
        {
            randomDirection();
            _chronos["actiontimer"].reset(true);
        }
    }
}

void ZStrongoBoss::phase2(bool right)
{
    if(!_phase2)
    {
        _chronos["actiontimer"].reset(true);
        _maxSpeed = 15;
        _phase2 = true;
    }
    if(right)
    {
        // STOP
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() < 1
            && _actionMan->peekAction()->name().compare("Stop") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Stop"]);
        }
        // MOVE RIGHT
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 1
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 4
            && _actionMan->peekAction()->name().compare("MoveRight") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["MoveRight"]);
        }
        // STOP
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 4
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 4.5
            && _actionMan->peekAction()->name().compare("Stop") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Stop"]);
        }
        // POUND
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 4.5
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 5.5
            && _actionMan->peekAction()->name().compare("Pound") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Pound"]);
        }
         // STOP
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 5.5
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 6
            && _actionMan->peekAction()->name().compare("Stop") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Stop"]);
        }
        // THROW
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 6
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 7
            && _actionMan->peekAction()->name().compare("Attack") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Attack"]);
        }
        // STOP
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 7
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 8
            && _actionMan->peekAction()->name().compare("Stop") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Stop"]);
        }
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 8)
        {
            randomDirection();
            _chronos["actiontimer"].reset(true);
        }
    }
    else
    {
        // STOP
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() < 1
            && _actionMan->peekAction()->name().compare("Stop") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Stop"]);
        }
        // MOVE RIGHT
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 1
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 4
            && _actionMan->peekAction()->name().compare("MoveLeft") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["MoveLeft"]);
        }
        // STOP
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 4
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 4.5
            && _actionMan->peekAction()->name().compare("Stop") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Stop"]);
        }
         // POUND
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 4.5
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 5.5
            && _actionMan->peekAction()->name().compare("Pound") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Pound"]);
        }
         // STOP
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 5.5
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 6
            && _actionMan->peekAction()->name().compare("Stop") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Stop"]);
        }
        // THROW
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 6
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 7
            && _actionMan->peekAction()->name().compare("Attack") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Attack"]);
        }
        // STOP
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 7
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 8
            && _actionMan->peekAction()->name().compare("Stop") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Stop"]);
        }
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 8)
        {
            randomDirection();
            _chronos["actiontimer"].reset(true);
        }
    }
}

void ZStrongoBoss::phase3(bool right)
{
    if(!_phase3)
    {
        _chronos["actiontimer"].reset(true);
        _phase3 = true;
        _maxSpeed = 17;
        _minShotIntervalInSeconds = 1;
    }
    if(right)
    {
        // STOP
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() < 1
            && _actionMan->peekAction()->name().compare("Stop") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Stop"]);
        }
        // ATTACK
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 1
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 3
            && _actionMan->peekAction()->name().compare("Attack") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Attack"]);
        }
        // MOVE RIGHT
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 3
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 5
            && _actionMan->peekAction()->name().compare("MoveRight") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["MoveRight"]);
        }
        // POUND
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 5
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 6
            && _actionMan->peekAction()->name().compare("Pound") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Pound"]);
        }
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 7)
        {
            randomDirection();
            _chronos["actiontimer"].reset(true);
        }
    }
    else
    {
        // STOP
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() < 1
            && _actionMan->peekAction()->name().compare("Stop") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Stop"]);
        }
        // ATTACK
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 1
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 3
            && _actionMan->peekAction()->name().compare("Attack") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Attack"]);
        }
        // MOVE RIGHT
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 3
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 5
            && _actionMan->peekAction()->name().compare("MoveLeft") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["MoveLeft"]);
        }
        // POUND
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 5
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 6
            && _actionMan->peekAction()->name().compare("Pound") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Pound"]);
        }
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 7)
        {
            randomDirection();
            _chronos["actiontimer"].reset(true);
        }
    }
}

void ZStrongoBoss::attack()
{
    if(!level()->entityMan()->entity("Zorbit"))
        return;

    if(_chronos["shottimer"].getElapsedTime().asSeconds() >= _minShotIntervalInSeconds)
    {
        //if(level()->entityMan()->entity("SBTrash15"))
        //    _numShots = 1;

        // generate shot name
        _shotName.str("");
        _shotName.clear();
        _shotName << "SBTrash" << _numShots;

        // pick a random piece of trash
        int whichTrash = rand() % 4;
        std::string direction;

        if(!_phase3)
        {
            if(directedRight()) direction = "Right";
            else direction = "Left";
        }
        else
        {
            int directionInt = rand() % 2;
            if (directionInt == 0)
                direction = "Left";
            else direction = "Right";
        }

        SBTrash::Ptr trash = new SBTrash(_shotName.str(),
                                         static_cast<ZorbitLevel *>(level()),
                                          _layerNames,
                                          whichTrash,
                                          direction,
                                          body("thrower")->b2body()->GetPosition());
        if(_phase3)
        {
            trash->setHoldTime(0.5);
            trash->setThrowPower(700);
        }

        level()->entityMan()->entityNew(trash->name(), "Layer1", trash);
        _trash.push_back(trash->name());

        //level()->soundMan()->playSound("fb_snot_shot", true);

        _numShots++;
        _chronos["shottimer"].reset(true);
    }
}

void ZStrongoBoss::pound()
{
    if(directedRight())
    {
        if(currentAnimation()->currentFrame() % 2 == 0)
        {
            level()->soundMan()->playSound("sboss_pound", true);
            shoot();
            //_chronos["poundtimer"].reset(true);
        }
        else
            move("Right");
    }
    else
    {
        if(currentAnimation()->currentFrame() % 2 == 0)
        {
            level()->soundMan()->playSound("sboss_pound", true);
            shoot();
            //_chronos["poundtimer"].reset(true);
        }
        else
            move("Left");
    }
}

void ZStrongoBoss::shoot()
{
    if(gravityField() && _chronos["poundtimer"].getElapsedTime().asSeconds() > 0.4)
    {
        // Calculate shot
        b2Vec2 initPos = body("ZStrongoFootBody")->b2body()->GetPosition();
        float shotRadius = jl::Tool::distance(initPos, gravityField()->position());

        if(level()->entityMan()->entity("SBShot15"))
            _numShots = 1;

        std::stringstream name;
        name << "SBShot" << _numShots;

        if(directedRight())
        {
            SBShot::Ptr shot = new SBShot(name.str(), static_cast<ZorbitLevel *>(level()), _layerNames, initPos, shotRadius, "Right");
            level()->entityMan()->entityNew(shot->name(), "Layer1", shot);
        }
        else
        {
            SBShot::Ptr shot = new SBShot(name.str(), static_cast<ZorbitLevel *>(level()), _layerNames, initPos, shotRadius, "Left");
            level()->entityMan()->entityNew(shot->name(), "Layer1", shot);
        }

        _numShots++;
        level()->camera()->shake(20);
        _chronos["poundtimer"].reset(true);
    }
}

void ZStrongoBoss::move(std::string direction)
{
    if (_maxSpeed < 0)
        throw fwk::InternalException("Speed must be non-negative.");

    if(direction == "Right" && gravityField())
    {
        if(polarVelocity().x < _maxSpeed)
            rootBody()->applyPolarForceToCenter(0,
                                                _maxSpeed * 100 * gravityField()->specificForce() / gravityField()->defaultSpecificForce());
    }
    else if(direction == "Left" && gravityField())
    {
        if(polarVelocity().x > -_maxSpeed)
            rootBody()->applyPolarForceToCenter(0,
                                                -_maxSpeed * 100 * gravityField()->specificForce() / gravityField()->defaultSpecificForce());
    }
}

void ZStrongoBoss::stop()
{
    // heading right
    if(polarVelocity().x > 0)
            rootBody()->applyPolarForceToCenter(0, -_maxSpeed * 5 * gravityField()->specificForce() / gravityField()->defaultSpecificForce());

    // heading left
    if(polarVelocity().x < 0)
            rootBody()->applyPolarForceToCenter(0, _maxSpeed * 5 * gravityField()->specificForce() / gravityField()->defaultSpecificForce());

}

void ZStrongoBoss::destroyJoints()
{
    level()->b2world()->DestroyJoint( _throwerJoint );
    level()->b2world()->DestroyJoint( _footJoint );
}

void ZStrongoBoss::cleanUp()
{
    _trash.clear();
    ZBoss::cleanUp();
}

// **************************
//  SBTrash methods
// **************************

SBTrash::SBTrash(std::string name,
                 ZorbitLevel * level,
                 std::vector<string> layerNames,
                 int whichTrash,
                 std::string direction,
                 b2Vec2 initPos,
                 float radius,
                 float duration) : ZCircle(name,
                                           level,
                                           layerNames,
                                           "resources/zorbitsorbits/strongoboss/trash.png",
                                           initPos,
                                           0,
                                           radius,
                                           "true",
                                           false)
{
    _duration = duration;
    _fading = false;
    _canHurtZorbit = true;
    _destroyJoint = true;
    _breakJoint = true;
    _direction = direction;
    _holdTime = 1;
    _throwPower = 500;
    changeType("Trash");

    // Change sprite, etc.
    rootBody()->sprite()->setOrigin(100,100);
    switch(whichTrash)
    {
    case tire:
        _knownAnims["Stop"]->changeRect(sf::IntRect(0,0,200,200));
        break;
    case gear:
        _knownAnims["Stop"]->changeRect(sf::IntRect(200,0,200,200));
        break;
    case bear:
        _knownAnims["Stop"]->changeRect(sf::IntRect(400,0,200,200));
        break;
    case trump:
        _knownAnims["Stop"]->changeRect(sf::IntRect(600,0,200,200));
        break;
    default:
        throw fwk::InternalException("Strongo trash default case");
        break;
    }

    // Pin in place for a hot second
    b2WeldJointDef jointDef;
    jointDef.bodyA = rootBody()->b2body();
    jointDef.bodyB = this->level()->entityMan()->entity("StrongoBoss")->body("thrower")->b2body();
    _positionJoint = (b2WeldJoint *)this->level()->b2world()->CreateJoint(&jointDef);

    _chronos["fadetimer"] = _fadeTimer;
    _chronos["timer"] = _timer;
}

void SBTrash::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // Get the type of the other collidable for comparisons.
    string thatType = thatCollidable->type();

    if(thisFixUD->name() == "Root")
    {
        if(thatType == "GravityField")
            gravityFieldIs(static_cast<jl::GravityField *>(thatCollidable.ptr()));
    }
    if(thatType.compare("Planet") == 0)
    {
        level()->soundMan()->playSound("sboss_trash_land", false);
    }
    if(thatType.compare("Player") == 0
       && thisFixUD->name().compare("Root") == 0
       && thatFixUD->name().compare("Root") == 0)
    {
        static_cast<Zorbit *>(thatCollidable.ptr())->hit(1);
        die();
    }
}

void SBTrash::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
}

void SBTrash::updateConcrete()
{
    // Time to get thrown!
    if(_chronos["timer"].getElapsedTime().asSeconds() > _holdTime)
    {
        if(_destroyJoint)
        {
            level()->soundMan()->playSound("sboss_throw", false);
            level()->b2world()->DestroyJoint( _positionJoint );
            float direction = 1;
            if(_direction.compare("Left") == 0) direction = -1;
            rootBody()->applyPolarImpulseToCenter(-50, direction * _throwPower);
            _destroyJoint = false;
        }
    }

    // Time to die!
    if(_chronos["timer"].getElapsedTime().asSeconds() > _duration)
    {
        _fading = true;
        _canHurtZorbit = false;
    }

    if(!_fading)
        _chronos["fadetimer"].reset(true);

    // Fade out
    if(_fading
       && _chronos["fadetimer"].getElapsedTime().asMilliseconds() >= 5)
    {
        sf::Color color = rootBody()->sprite()->getColor();
        if((color.a > 0 && color.a <= 20) || color.a <=0)
           color.a = 20;

        rootBody()->sprite()->setColor(sf::Color(255, 255, 255, color.a - 20));

        // Stop fading.
        if(color.a - 20 <= 0)
        {
            _fading = false;
            die();
        }

        _chronos["fadetimer"].reset(true);
    }
}

void SBTrash::breakJoints()
{
    if(_positionJoint
       && _breakJoint == true
       && _destroyJoint == true)
        level()->b2world()->DestroyJoint( _positionJoint );

    _breakJoint = false;
}

// **************************
//  SBShot methods
// **************************

SBShot::SBShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float distance, std::string direction, float duration) : jl::Projectile(name,
            level, layerNames, direction, duration, "SBShot")
{
    jl::TextureDesc::Ptr tex;
    tex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/elvisboss/smoke.png",
            sf::Vector2f(100, 100), 200, 200);

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("SBShotBody", "foreground", tex, &bodyDef, initPos, 0);
    rootBody()->b2body()->SetLinearDamping(0);

    if(direction.compare("Left") == 0)
        flipSprites();


    _initialDistance = distance + 0.75f;
    _spaceAngle = 0;

    // Set values
    _maxSpeed       = 50;
    _damage         = 0;

    float width     = 0.25;
    float height    = 0.25;

    defineFixtures(width, height);
    defineAnimations();
    defineActions();
    defineSounds();

    _chronos["fadetimer"] = _fadeTimer;

}

SBShot::SBShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float angle, float duration) : jl::Projectile(name,
            level, layerNames, "Space", duration, "EBShot")
{
    jl::TextureDesc::Ptr tex;
    tex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/elvisboss/smoke.png",
            sf::Vector2f(100, 100), 200, 200);

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("SBShotBody", "foreground", tex, &bodyDef, initPos, 0);
    rootBody()->b2body()->SetLinearDamping(0);

    _initialDistance = 0;
    _spaceAngle = angle;

    // Set values
    _maxSpeed       = 30;
    _damage         = 1;

    float width     = 0.25;
    float height    = 0.25;

    defineFixtures(width, height);
    defineAnimations();
    defineActions();
    defineSounds();

}

void SBShot::defineAnimations()
{
    _knownAnims["Move"] = new jl::Animation("Move", sf::IntRect(0, 0, 200, 200), 1, true);

    currentAnimIs(_knownAnims["Move"]);
}

void SBShot::defineActions()
{
    _knownActions["Move"] = new sbShotMoveAction(this);
}

void SBShot::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // Get the type of the other collidable for comparisons.
    string thatType = thatCollidable->type();

    if(thisFixUD->name() == "Root")
    {
        if(thatType == "GravityField")
            gravityFieldIs(static_cast<jl::GravityField *>(thatCollidable.ptr()));
    }
    if(thatType.compare("Player") == 0
       && thisFixUD->name().compare("Root") == 0
       && thatFixUD->name().compare("Root") == 0)
    {
        static_cast<Zorbit *>(thatCollidable.ptr())->hit(1);
        die();
    }
}

void SBShot::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
}

void SBShot::updateConcrete()
{
    if(noActions())
        _actionMan->pushAction(_knownActions["Move"]);

    if(_chronos["fadetimer"].getElapsedTime().asSeconds() >= _duration / 2)
    {
        sf::Color color = rootBody()->sprite()->getColor();

        if((color.a > 0 && color.a <= 20) || color.a <=0)
           color.a = 20;

        rootBody()->sprite()->setColor(sf::Color(255, 255, 255, color.a - 20));
        _chronos["fadetimer"].reset(true);

        if(_maxSpeed >= 5)
            _maxSpeed -= 5;
    }

}
