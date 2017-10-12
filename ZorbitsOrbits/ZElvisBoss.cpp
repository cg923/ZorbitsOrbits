
// Copyright(c) 2017 Happy Rock Studios

#include "ZElvisBoss.h"
#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "Zorbit.h"
#include "JollyLava/Tool.h"
#include "ZShot.h"
#include "ZPlatform.h"

ZElvisBoss::ZElvisBoss(ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle) : ZBoss("ElvisBoss",
            level, layerNames)
{
    _prefix = "eboss";

    // Body texture
    jl::TextureDesc::Ptr bodyTex;
    bodyTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/elvisboss/elvox.png",
            sf::Vector2f(683, 386), 1366, 772);

    // Death texture
    this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/elvisboss/death.png",
            sf::Vector2f(683, 386), 1366, 772);

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("ZElvisBossBody", "foreground", bodyTex, &bodyDef, initPos, initAngle);

    float width = 6.0f;
    float height = 6.0f;

    // Set values
    _active         = false;
    _maxHealth      = 200;   //200
    _currentHealth  = _maxHealth;
    _damage         = 1;
    _numShots       = 1;
    _timesOnGround  = 1;
    _maxSpeed       = 3;
    _jumpHeight     = 5510;
    _lFootContacts  = 0;
    _rFootContacts  = 0;
    SHOOT_RATE      = 500;
    _attacking      = false;
    _seesZorbit     = false;
    _stunning       = false;
    _tailHit        = false;
    _shooting       = false;
    _phase2         = false;
    _phase3         = false;
    _limit          = false;
    directedRightIs(true);

    defineFixtures(width, height);
    defineAnimations();
    defineSounds();
    defineActions();

    _chronos["actiontimer"] = _actionTimer;
    _chronos["shottimer"] = _shotTimer;
    _chronos["stuntimer"] = _stunClock;

    _chronos["actiontimer"].resume();
    _chronos["shottimer"].resume();
    _chronos["stuntimer"].resume();
}

void ZElvisBoss::defineFixtures(float width, float height)
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

    // Tail fixture
    fixShape.SetAsBox(width / 2, height / 4, b2Vec2(width, -height + 1.2), 0);
    fixDef.shape = &fixShape;
    fixDef.density = 0.0f;
    fixDef.isSensor = true;

    jl::FixtureUserData::Ptr fixUD3 = new jl::FixtureUserData("Tail");
    rootBody()->fixtureNew(fixUD3, &fixDef);

    // Foot fixture
    fixShape.SetAsBox(width / 2, height / 8, b2Vec2(0, -height),0);// - 0.9), 0);
    fixDef.shape = &fixShape;
    fixDef.density = 0.0f;
    fixDef.isSensor = true;

    jl::FixtureUserData::Ptr fixUD2 = new jl::FixtureUserData("Foot");
    rootBody()->fixtureNew(fixUD2, &fixDef);
}

void ZElvisBoss::defineAnimations()
{
    _knownAnims["Stop"]             = new jl::Animation("Stop", sf::IntRect(0, 0, 1366, 772), 2, true, 300);
    _knownAnims["MoveRight"]        = new jl::Animation("MoveRight", sf::IntRect(2733, 0, 1366, 772), 1, false);
    _knownAnims["MoveLeft"]         = _knownAnims["MoveRight"];
    _knownAnims["Jump"]             = new jl::Animation("Jump", sf::IntRect(0, 773, 1366, 909), 2, false, 500);
    _knownAnims["TailWhip"]         = new jl::Animation("TailWhip", sf::IntRect(0, 1680, 1360, 780), 3, false, 80);
    _knownAnims["Die"]              = new jl::Animation("Die", sf::IntRect(0, 0, 1366, 772), 7, false, 130);

    currentAnimIs(_knownAnims["Stop"]);
}

void ZElvisBoss::defineSounds()
{
    level()->soundMan()->soundNew("eboss_hit", "resources/sounds/tboss_hit.wav", false);
    level()->soundMan()->soundNew("eboss_die", "resources/sounds/eboss_die.wav", false);
    level()->soundMan()->soundNew("eboss_roar", "resources/sounds/eboss_roar.wav", false);
    level()->soundMan()->soundNew("eboss_land", "resources/sounds/fb_hand_land.wav", false);
    level()->soundMan()->soundNew("eboss_sing", "resources/sounds/eboss_sing.wav", false);
    level()->soundMan()->soundNew("eboss_whip", "resources/sounds/eboss_whip.wav", false);
    level()->soundMan()->soundNew("eboss_jump", "resources/sounds/eboss_jump.wav", false);

}

void ZElvisBoss::defineActions()
{
    _knownActions["MoveRight"]          = new jl::cMRightAction(this);
    _knownActions["MoveLeft"]           = new jl::cMLeftAction(this);
    _knownActions["TailWhip"]           = new ebAttackAction(this);
    _knownActions["Stop"]               = new jl::aStopAction(this);
    _knownActions["Jump"]               = new jl::cJumpAction(this);
    _knownActions["Die"]                = new bDieAction("tb_die", this);
}

void ZElvisBoss::reset()
{
    ZBoss::reset();

    _shooting   = false;
    _tailHit    = false;
    _stunning   = false;

    _maxSpeed = 3;
    SHOOT_RATE = 500;

    _phase2 = false;
    _phase3 = false;

    if(!directedRight())
    {
        switchDirection();
        directedRightIs(true);
    }

    _chronos["shottimer"].reset(true);
    _chronos["actiontimer"].reset(true);
    _chronos["stuntimer"].reset(true);
}

void ZElvisBoss::orientTowardGravityFieldCenter()
{
}

void ZElvisBoss::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
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

        if(level()->soundMan()->sound("eboss_hit"))
            level()->soundMan()->playSound("eboss_hit", true);
    }
    if(thatType.compare("Player") == 0
       && thatFixUD->name().compare("Root") == 0
       && thisFixUD->name().compare("Root") == 0)
    {
        static_cast<Zorbit *>(thatCollidable.ptr())->hit(_damage);
    }
    if(thatType.compare("Player") == 0
       && thatFixUD->name().compare("Root") == 0
       && thisFixUD->name().compare("Tail") == 0)
    {
        _tailHit = true;
    }
    if(thatType.compare("Planet") == 0
       && thisFixUD->name().compare("Foot") == 0)
    {
        if(_footContacts == 0
           && _timesOnGround > 1)
        {
            level()->soundMan()->playSound("eboss_land");
            _stunning = true;
            _chronos["stuntimer"].reset(true);
        }
        _footContacts++;
        _timesOnGround++;
    }
}

void ZElvisBoss::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    ZBoss::endContact(contact, thisFixUD, thatCollidable, thatFixUD);

    string thatType = thatCollidable->type();

    if(thatType.compare("Planet") == 0
    && thisFixUD->name().compare("Foot") == 0)
    {
        _footContacts--;
    }
    if(thatType.compare("Player") == 0
       && thatFixUD->name().compare("Root") == 0
       && thisFixUD->name().compare("Tail") == 0)
    {
        _tailHit = false;
    }
}

void ZElvisBoss::changeToDeathTexture()
{
    rootBody()->changeTexture(level()->game()->textureMan()->textureDesc("resources/zorbitsorbits/elvisboss/death.png"));
}

void ZElvisBoss::updateConcrete()
{
    _linearDamp = 0.0f;
    rootBody()->b2body()->SetLinearDamping(_linearDamp);
    rootBody()->b2body()->SetTransform(rootBody()->b2body()->GetPosition(), polarAngle() - jl::Tool::PI() / 2);

    if(_currentHealth <= 1)
    {
        static_cast<ZPlatform*>(level()->entityMan()->entity("Platform7").ptr())->activationIs(false);
        static_cast<ZPlatform*>(level()->entityMan()->entity("Platform8").ptr())->activationIs(false);
    }

    if(!_active)
    {
        if(noActions())
            _actionMan->pushAction(_knownActions["Stop"]);

        _chronos["actiontimer"].reset(true);
    }
    else
    {
        if(_stunning)
            groundSmoke();

        if(_shooting)
        {
            shoot();
            rootBody()->sprite()->setTextureRect(sf::IntRect(2732,773,1366,909));
            rootBody()->sprite()->setOrigin(683, 504);
        }
        else rootBody()->sprite()->setOrigin(683, 386);

        if(_chronos["stuntimer"].getElapsedTime().asSeconds() > 0.2)
            _stunning = false;

        if(noActions())
            _actionMan->pushAction(_knownActions["Stop"]);

        if(level()->entityMan()->entity("Zorbit")
           && level()->entityMan()->entity("Zorbit")->polarAngle() <= polarAngle()
           && !directedRight())
        {
            directedRightIs(true);
            flipSprites();
        }
        else if(level()->entityMan()->entity("Zorbit")
           && level()->entityMan()->entity("Zorbit")->polarAngle() > polarAngle()
           && directedRight())
        {
            directedRightIs(false);
            flipSprites();
        }

        if(_currentHealth >= _maxHealth * 0.6)
        {
            phase1(directedRight());
        }
        else if(_currentHealth >= _maxHealth * 0.4 && _currentHealth < _maxHealth * 0.6)
        {
            phase2(directedRight());
        }
        else if(_currentHealth < _maxHealth * 0.4 && _currentHealth > _maxHealth * 0.05)
        {
            phase3(directedRight());
        }
        else
        {
            _shooting = false;
            //roar();
        }
    }
}

void ZElvisBoss::roar()
{
    //level()->soundMan()->playSound("eboss_roar");

    static_cast<ZPlatform*>(level()->entityMan()->entity("Platform7").ptr())->activationIs(false);
    static_cast<ZPlatform*>(level()->entityMan()->entity("Platform8").ptr())->activationIs(false);
}

void ZElvisBoss::phase1(bool right)
{
    if(right)
    {
        // STOP
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() < 2
            && _actionMan->peekAction()->name().compare("Stop") != 0)
        {

        }
        // JUMP
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 2
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 3.2
            && _actionMan->peekAction()->name().compare("Jump") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Jump"]);
        }
        // STOP AND SHOOT
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 3.2
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 7
            && _actionMan->peekAction()->name().compare("Stop") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Stop"]);
        }

        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 4.7) _shooting = true;
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 5.7) _shooting = false;

        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 7
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 10
            && _actionMan->peekAction()->name().compare("MoveRight") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["MoveRight"]);
        }
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 10
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 10.5
            && _actionMan->peekAction()->name().compare("TailWhip") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["TailWhip"]);
        }
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 11)
        {
            //level()->soundMan()->playSound("eboss_roar");
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Stop"]);
            _chronos["actiontimer"].reset(true);
        }
    }
    else
    {
        // STOP
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() < 2
            && _actionMan->peekAction()->name().compare("Stop") != 0)
        {

        }
        // JUMP
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 2
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 3.2
            && _actionMan->peekAction()->name().compare("Jump") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Jump"]);
        }
        // STOP AND SHOOT
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 3.2
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 7
            && _actionMan->peekAction()->name().compare("Stop") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Stop"]);
        }

        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 4.7) _shooting = true;
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 5.7) _shooting = false;

        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 7
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 10
            && _actionMan->peekAction()->name().compare("MoveLeft") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["MoveLeft"]);
        }
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 10
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 10.5
            && _actionMan->peekAction()->name().compare("TailWhip") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["TailWhip"]);
        }
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 11)
        {
            //level()->soundMan()->playSound("eboss_roar");
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Stop"]);
            _chronos["actiontimer"].reset(true);
        }
    }
}

void ZElvisBoss::phase2(bool right)
{
    if(!_phase2)
    {
        _chronos["actiontimer"].reset(true);
        _phase2 = true;
        _maxSpeed = 10;
        _shooting = false;
        static_cast<ZPlatform*>(level()->entityMan()->entity("Platform7").ptr())->switchActivation();
        static_cast<ZPlatform*>(level()->entityMan()->entity("Platform8").ptr())->switchActivation();
    }
    if(right)
    {
        // STOP
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() < 1
            && _actionMan->peekAction()->name().compare("Stop") != 0)
        {
        }
        // JUMP
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 1
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 4
            && _actionMan->peekAction()->name().compare("MoveRight") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["MoveRight"]);
        }
        // STOP AND SHOOT
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 4
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 5.2
            && _actionMan->peekAction()->name().compare("Jump") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Jump"]);
        }

        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 5.2
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 6
            && _actionMan->peekAction()->name().compare("Stop") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Stop"]);
        }
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 6
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 6.5
            && _actionMan->peekAction()->name().compare("TailWhip") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["TailWhip"]);
        }
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 7)
        {
            //level()->soundMan()->playSound("eboss_roar");
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Stop"]);
            _chronos["actiontimer"].reset(true);
        }
    }
    else
    {
        // STOP
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() < 1
            && _actionMan->peekAction()->name().compare("Stop") != 0)
        {
            //level()->soundMan()->playSound("eboss_roar");
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Stop"]);
        }
        // JUMP
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 1
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 4
            && _actionMan->peekAction()->name().compare("MoveLeft") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["MoveLeft"]);
        }
        // STOP AND SHOOT
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 4
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 5.2
            && _actionMan->peekAction()->name().compare("Jump") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Jump"]);
        }

        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 5.2
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 6
            && _actionMan->peekAction()->name().compare("Stop") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Stop"]);
        }
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 6
            && _chronos["actiontimer"].getElapsedTime().asSeconds() < 6.5
            && _actionMan->peekAction()->name().compare("TailWhip") != 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["TailWhip"]);
        }
        if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 7)
        {
            //level()->soundMan()->playSound("eboss_roar");
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["Stop"]);
            _chronos["actiontimer"].reset(true);
        }
    }
}

void ZElvisBoss::phase3(bool right)
{
    if(!_phase3)
    {
        _chronos["actiontimer"].reset(true);
        _phase3 = true;
        //SHOOT_RATE = 300;
    }
    // STOP AND SHOOT
    if(_chronos["actiontimer"].getElapsedTime().asSeconds() < 6
        && _actionMan->peekAction()->name().compare("Stop") != 0)
    {
        //level()->soundMan()->playSound("eboss_roar");
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["Stop"]);
    }

    if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 2) _shooting = true;
    if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 4) _shooting = false;

    if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 6
        && _chronos["actiontimer"].getElapsedTime().asSeconds() < 7.2
        && _actionMan->peekAction()->name().compare("Jump") != 0)
    {
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["Jump"]);
    }

    if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 7.2)
    {
        static_cast<ZPlatform*>(level()->entityMan()->entity("Platform7").ptr())->switchActivation();
        static_cast<ZPlatform*>(level()->entityMan()->entity("Platform8").ptr())->switchActivation();
        _chronos["actiontimer"].reset(true);
    }
}

void ZElvisBoss::tailWhip()
{
    level()->soundMan()->playSound("eboss_whip", false);

    if(_tailHit && level()->entityMan()->entity("Zorbit"))
    {
        static_cast<Zorbit *>(level()->entityMan()->entity("Zorbit").ptr())->hit(_damage);
        if(directedRight())
            level()->entityMan()->entity("Zorbit")->rootBody()->b2body()->ApplyLinearImpulse(b2Vec2(20, 50),
                                                gravityField()->center());
        else
            level()->entityMan()->entity("Zorbit")->rootBody()->b2body()->ApplyLinearImpulse(b2Vec2(-20, 50),
                                                gravityField()->center());
    }
}

void ZElvisBoss::move(std::string direction)
{
    if (_maxSpeed < 0)
        throw fwk::InternalException("Speed must be non-negative.");

    if(direction == "Right" && gravityField())
    {
        if(polarVelocity().x < _maxSpeed)
            rootBody()->applyPolarForceToCenter(0,
                                                _maxSpeed * 200 * gravityField()->specificForce() / gravityField()->defaultSpecificForce());
    }
    else if(direction == "Left" && gravityField())
    {
        if(polarVelocity().x > -_maxSpeed)
            rootBody()->applyPolarForceToCenter(0,
                                                -_maxSpeed * 200 * gravityField()->specificForce() / gravityField()->defaultSpecificForce());
    }
}

void ZElvisBoss::stop()
{
    // heading right
    if(polarVelocity().x > 0)
            rootBody()->applyPolarForceToCenter(0, -_maxSpeed * 250 * gravityField()->specificForce() / gravityField()->defaultSpecificForce());

    // heading left
    if(polarVelocity().x < 0)
            rootBody()->applyPolarForceToCenter(0, _maxSpeed * 250 * gravityField()->specificForce() / gravityField()->defaultSpecificForce());

    //rootBody()->b2body()->SetLinearVelocity(b2Vec2(0,0));
}

void ZElvisBoss::shoot()
{
    if(!level()->entityMan()->entity("Zorbit"))
        return;

    if(_chronos["shottimer"].getElapsedTime().asMilliseconds() > SHOOT_RATE)
    {
        _shotName.str("");
        _shotName.clear();
        _shotName << "zEBShot" << _numShots;

        float angle = 0;
        b2Vec2 shootPos = rootBody()->b2body()->GetPosition() + b2Vec2(0,3);
        if(directedRight())
        {
            shootPos = shootPos + b2Vec2(4,0);
            angle = -jl::Tool::thetaCoordinate(shootPos, level()->entityMan()->entity("Zorbit")->position()) - jl::Tool::PI() / 2;
        }
        else
        {
            shootPos = shootPos + b2Vec2(-4,0);
            angle = -jl::Tool::thetaCoordinate(shootPos, level()->entityMan()->entity("Zorbit")->position()) - jl::Tool::PI() / 2;
        }

        EBShot2::Ptr shot = new EBShot2(_shotName.str(), static_cast<ZorbitLevel *>(level()), _layerNames, shootPos, angle);
        level()->entityMan()->entityNew(shot->name(), "Layer1", shot);

        level()->soundMan()->playSound("eboss_sing", false);

        _numShots++;
        _chronos["shottimer"].reset(true);
    }
}

void ZElvisBoss::groundSmoke()
{
    if(level()->entityMan()->entity("leftSmoke")
       || level()->entityMan()->entity("rightSmoke"))
           return;

    if(gravityField())
    {
        // Calculate shot
        b2Vec2 initPos = body("ZElvisBossBody")->b2body()->GetPosition() - b2Vec2(0, 5.5);
        float shotRadius = jl::Tool::distance(initPos, gravityField()->position());

        // Create  left smoke
        EBShot::Ptr shot = new EBShot("leftSmoke", static_cast<ZorbitLevel *>(level()), _layerNames, initPos, shotRadius, "Left");
        level()->entityMan()->entityNew(shot->name(), "Layer1", shot);

        // Create  right smoke
        EBShot::Ptr shot2 = new EBShot("rightSmoke", static_cast<ZorbitLevel *>(level()), _layerNames, initPos, shotRadius, "Right");
        level()->entityMan()->entityNew(shot2->name(), "Layer1", shot2);
    }
    else throw fwk::InternalException("ElvisBoss should not be placed outside a gravity field!");

    level()->camera()->shake(20);
}

// **************************
//  EBShot methods
// **************************

EBShot::EBShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float distance, std::string direction, float duration) : jl::Projectile(name,
            level, layerNames, direction, duration, "EBShot")
{
    jl::TextureDesc::Ptr tex;
    tex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/elvisboss/smoke.png",
            sf::Vector2f(100, 100), 200, 200);

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("EBShotBody", "foreground", tex, &bodyDef, initPos, 0);
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

EBShot::EBShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float angle, float duration) : jl::Projectile(name,
            level, layerNames, "Space", duration, "EBShot")
{
    jl::TextureDesc::Ptr tex;
    tex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/elvisboss/smoke.png",
            sf::Vector2f(100, 100), 200, 200);

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("EBShotBody", "foreground", tex, &bodyDef, initPos, 0);
    rootBody()->b2body()->SetLinearDamping(0);

    _initialDistance = 0;
    _spaceAngle = angle;

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

void EBShot::defineAnimations()
{
    _knownAnims["Move"] = new jl::Animation("Move", sf::IntRect(0, 0, 200, 200), 1, true);

    currentAnimIs(_knownAnims["Move"]);
}

void EBShot::defineActions()
{
    _knownActions["Move"] = new ebShotMoveAction(this);
}

void EBShot::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    //Projectile::beginContact(contact, thisFixUD, thatCollidable, thatFixUD);

    // Get the type of the other collidable for comparisons.
    string thatType = thatCollidable->type();

    if(thisFixUD->name() == "Root")
    {
        if(thatType == "GravityField")
            gravityFieldIs(static_cast<jl::GravityField *>(thatCollidable.ptr()));
    }
}

void EBShot::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    //Projectile::endContact(contact, thisFixUD, thatCollidable, thatFixUD);
}

void EBShot::updateConcrete()
{
    if(_chronos["fadetimer"].getElapsedTime().asMilliseconds() >= 5)
    {
        sf::Color color = rootBody()->sprite()->getColor();

        if((color.a > 0 && color.a <= 20) || color.a <=0)
           color.a = 20;

        rootBody()->sprite()->setColor(sf::Color(255, 255, 255, color.a - 20));
        _chronos["fadetimer"].reset(true);

        if(_maxSpeed >= 5)
            _maxSpeed -= 5;
    }
    if(noActions())
        _actionMan->pushAction(_knownActions["Move"]);
}

// **************************
//  EBShot2 methods
// **************************

EBShot2::EBShot2(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float distance, std::string direction, float duration) : jl::Projectile(name,
            level, layerNames, direction, duration, "EBShot2")
{
    throw fwk::InternalException("Should not be initializing FBShot this way");
}

EBShot2::EBShot2(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float angle, float duration) : jl::Projectile(name,
            level, layerNames, "Space", duration, "EBShot2")
{
    jl::TextureDesc::Ptr tex;
    tex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/elvisboss/notes.png",
            sf::Vector2f(32, 32), 64, 64);

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("EBShot2Body", "foreground", tex, &bodyDef, initPos, jl::Tool::sfAngle(angle - (jl::Tool::PI() / 2)));
    rootBody()->b2body()->SetLinearDamping(0);

    _frame = rand() % 3;

    _initialDistance = 0;
    _spaceAngle = angle;

    // Set values
    _maxSpeed       = 50;
    _damage         = 1;

    float width     = 0.25;
    float height    = 0.25;

    defineFixtures(width, height);
    defineAnimations();
    defineActions();
    defineSounds();

}

void EBShot2::defineAnimations()
{
    _knownAnims["Move"] = new jl::Animation("Move", sf::IntRect(0, 0, 64, 64), 1, true);

    currentAnimIs(_knownAnims["Move"]);
}

void EBShot2::defineActions()
{
    _knownActions["Move"] = new ebShot2MoveAction(this);
}

void EBShot2::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    //Projectile::beginContact(contact, thisFixUD, thatCollidable, thatFixUD);

    // Get the type of the other collidable for comparisons.
    string thatType = thatCollidable->type();

    if(thatType.compare("Player") == 0
       && thisFixUD->name().compare("Root") == 0
       && thatFixUD->name().compare("Root") == 0)
    {
        static_cast<Zorbit *>(thatCollidable.ptr())->hit(_damage);
        toDie();
    }
}

void EBShot2::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    //Projectile::endContact(contact, thisFixUD, thatCollidable, thatFixUD);
}

void EBShot2::updateConcrete()
{
    if(!level()->entityMan()->entity("Zorbit")
       || static_cast<Zorbit*>(level()->entityMan()->entity("Zorbit").ptr())->currentHealth() <= 0)
        toDie();

    if(noActions())
        _actionMan->pushAction(_knownActions["Move"]);

    rootBody()->sprite()->setTextureRect(sf::IntRect(64 * _frame, 0, 64, 64));
}

