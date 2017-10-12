// Copyright(c) 2017 Happy Rock Studios

#include "ZKops.h"
#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "Zorbit.h"
#include "ZBooster.h"
#include "JollyLava/Tool.h"

ZKops::ZKops(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle) : jl::Character(name,
            level, layerNames, "Enemy")
{
    _prefix = "kops";

    // Body texture
    jl::TextureDesc::Ptr bodyTex;
    bodyTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/kops/kops.png",
            sf::Vector2f(125, 102), 250, 205);

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("ZKopsBody", "foreground", bodyTex, &bodyDef, initPos, initAngle);

    float width = 1.75f;
    float height = 1.5f;

    // Set values
    _maxHealth      = 10;
    _currentHealth  = 10;
    _damage         = 0;
    _maxSpeed       = 12;
    _jumpHeight     = 30;
    _lFootContacts  = 0;
    _rFootContacts  = 0;
    _attack         = 0;
    _leftSensor     = false;
    _rightSensor    = false;
    _attack         = false;
    directedRightIs(true);

    defineFixtures(width, height);
    defineAnimations();
    defineSounds();
    defineActions();
}

void ZKops::defineFixtures(float width, float height)
{
    // Torso fixture
    b2FixtureDef fixDef;
    b2PolygonShape fixShape;
    fixShape.SetAsBox(width * .75, height * 0.75);
    fixDef.shape = &fixShape;
    fixDef.density = 1.0f;
    fixDef.friction = 0.0f;

    jl::FixtureUserData::Ptr fixUD = new jl::FixtureUserData("Root");
    rootBody()->fixtureNew(fixUD, &fixDef);

    // Head (so Zorbit can't stand on a Kops)
    b2CircleShape circleShape;
    circleShape.m_radius = width * .8;
    circleShape.m_p.Set(0, height * .4);
    fixDef.shape = &circleShape;

    jl::FixtureUserData::Ptr fixUD5 = new jl::FixtureUserData("Head");
    rootBody()->fixtureNew(fixUD5, &fixDef);

    // Left foot
    fixShape.SetAsBox(width / 8, height / 4, b2Vec2(-width, -height), 0);
    fixDef.shape = &fixShape;
    fixDef.isSensor = true;

    jl::FixtureUserData::Ptr fixUD3 = new jl::FixtureUserData("LeftFoot");
    rootBody()->fixtureNew(fixUD3, &fixDef);

    // Right foot
    fixShape.SetAsBox(width / 8, height / 4, b2Vec2(width, -height), 0);
    fixDef.shape = &fixShape;
    fixDef.isSensor = true;

    jl::FixtureUserData::Ptr fixUD4 = new jl::FixtureUserData("RightFoot");
    rootBody()->fixtureNew(fixUD4, &fixDef);

    // Zorbit Right detection fixture
    fixShape.SetAsBox(width * 5.5, height * 4.5, b2Vec2(width * 5.5, height * 1.5), 0);
    fixDef.shape = &fixShape;
    fixDef.density = 0.0f;
    //fixDef.isSensor = true;

    jl::FixtureUserData::Ptr fixUD2 = new jl::FixtureUserData("RightSensor");
    rootBody()->fixtureNew(fixUD2, &fixDef);

    // Zorbit Left detection fixture
    fixShape.SetAsBox(width * 5.5, height * 4.5, b2Vec2(-5.5 * width, height * 1.5), 0);
    fixDef.shape = &fixShape;
    fixDef.density = 0.0f;
    //fixDef.isSensor = true;

    jl::FixtureUserData::Ptr fixUD6 = new jl::FixtureUserData("LeftSensor");
    rootBody()->fixtureNew(fixUD6, &fixDef);

    jl::Character::defineFixtures(width * .6, height);
}

void ZKops::defineAnimations()
{
    _knownAnims["Stop"]         = new jl::Animation("Stop", sf::IntRect(0, 0, 250, 205), 10, true);
    _knownAnims["FollowRight"]  = new jl::Animation("FollowRight", sf::IntRect(0, 206, 250, 205), 10, true);
    _knownAnims["FollowLeft"]   = new jl::Animation("FollowLeft", sf::IntRect(0, 206, 250, 205), 10, true);
    _knownAnims["Attack"]       = new jl::Animation("Attack", sf::IntRect(0, 412, 250, 205), 10, true);
    _knownAnims["Die"]          = new jl::Animation("Die", sf::IntRect(0, 618, 250, 205), 10, false);

    currentAnimIs(_knownAnims["Stop"]);
}

void ZKops::defineSounds()
{
    //level()->soundMan()->soundNew("glob_hit", "resources/sounds/glob_hit.wav", false);
    //level()->soundMan()->soundNew("glob_bite", "resources/sounds/glob_bite.wav", false);
    //level()->soundMan()->soundNew("glob_jump", "resources/sounds/glob_jump.wav", false);
    //level()->soundMan()->soundNew("glob_die", "resources/sounds/glob_die.wav", false);
}

void ZKops::defineActions()
{
    _knownActions["FollowRight"]        = new jl::cMRightAction(1, this, "FollowRight");
    _knownActions["FollowLeft"]         = new jl::cMLeftAction(1, this, "FollowLeft");
    _knownActions["Attack"]             = new kAttackAction(this);
    _knownActions["Stop"]               = new jl::aStopAction(1, this);
    _knownActions["Die"]                = new jl::cDieAction("glob_die", this);
}

void ZKops::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
	/*
    // First run through all collisions handled by superclasses.
    Character::beginContact(contact, thisFixUD, thatCollidable, thatFixUD);

    // Get the type of the other collidable for comparisons.
    string thatType = thatCollidable->type();

    // Ground
    if(thatType.compare("Planet") == 0
       || thatType.compare("Platform") == 0
       || thatType.compare("RectShape") == 0)
    {
        if(thisFixUD->name().compare("LeftFoot") == 0)
            _lFootContacts++;
        if(thisFixUD->name().compare("RightFoot") == 0)
            _rFootContacts++;
    }

    // Zorbit
    if(thatType.compare("Player") == 0)
    {
        // Close enough to see.
        if( thisFixUD->name().compare("LeftSensor") == 0
            && thatFixUD->name().compare("Root") == 0)
                _leftSensor = true;
        if( thisFixUD->name().compare("RightSensor") == 0
            && thatFixUD->name().compare("Root") == 0)
                _rightSensor = true;

        // Close enough to attack.
        if( (thisFixUD->name().compare("Right") == 0
           || thisFixUD->name().compare("Left") == 0)
            && thatFixUD->name().compare("Root") == 0)
        {
            _attack = true;
        }
    }

    // Booster
    if(thatType.compare("Booster") == 0 && thisFixUD->name().compare("Root") == 0)
    {
        float smallerX = std::min(fabs(50 * rootBody()->b2body()->GetLinearVelocity().x),
                                  5 * static_cast<ZBooster *>(thatCollidable.ptr())->boostStrength());
        float smallerY = std::min(fabs(50 * rootBody()->b2body()->GetLinearVelocity().y),
                                  5 * static_cast<ZBooster *>(thatCollidable.ptr())->boostStrength());

        float desiredX = std::max(smallerX, 0.f);
        float desiredY = std::max(smallerY, 0.f);

        if(rootBody()->b2body()->GetLinearVelocity().x < 0)
            desiredX *= -1;
        if(rootBody()->b2body()->GetLinearVelocity().y < 0)
            desiredY *= -1;

        rootBody()->b2body()->ApplyLinearImpulse(b2Vec2(desiredX, desiredY),
                                                 rootBody()->b2body()->GetWorldCenter());
    }
	*/
}

void ZKops::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    Character::endContact(contact, thisFixUD, thatCollidable, thatFixUD);

    // Get the type of the other collidable for comparisons.
    string thatType = thatCollidable->type();

    // Ground
    if(thatType.compare("Planet") == 0)
    {
        if(thisFixUD->name().compare("LeftFoot") == 0)
            _lFootContacts--;
        if(thisFixUD->name().compare("RightFoot") == 0)
            _rFootContacts--;
    }

    if(thatType.compare("Player") == 0
       && thisFixUD->name().compare("LeftSensor") == 0
       && thatFixUD->name().compare("Root") == 0)
    {
        _leftSensor = false;
    }

    if(thatType.compare("Player") == 0
       && thisFixUD->name().compare("RightSensor") == 0
       && thatFixUD->name().compare("Root") == 0)
    {
        _rightSensor = false;
    }
    // Close enough to attack.
        if( (thisFixUD->name().compare("Right") == 0
           || thisFixUD->name().compare("Left") == 0)
            && thatFixUD->name().compare("Root") == 0)
        {
            _attack = false;
        }
}

void ZKops::updateConcrete()
{
    // Stopped
    if(noActions())
        _actionMan->pushAction(_knownActions["Stop"]);

    // Sees Zorbit
    if(_leftSensor
       && !_attack
       && _currentHealth > 0
       && _actionMan->peekAction()->name().compare("FollowLeft") != 0)
    {
        _actionMan->pushAction(_knownActions["FollowLeft"], "Top", true);
    }

    if(_rightSensor
       && !_attack
       && _currentHealth > 0
       && _actionMan->peekAction()->name().compare("FollowRight") != 0)
    {
        _actionMan->pushAction(_knownActions["FollowRight"], "Top", true);
    }

    if(_attack
       && _currentHealth > 0
       && _actionMan->peekAction()->name().compare("Attack") != 0)
    {
        _actionMan->pushAction(_knownActions["Attack"], "Top", true);
    }

    /*
    // Sound
    std::string currentAction;
    currentAction = _actionMan->peekAction()->name();

    if(currentAction.compare("MoveRight") == 0
        || currentAction.compare("MoveLeft") == 0)
    {
        if(currentAnimation()->currentFrame() == 2)
            level()->soundMan()->playSound("glob_jump", static_cast<ZorbitLevel*>(level())->distanceToZorbit(position()));
    }

    if(currentAction.compare("FollowLeft") == 0
        || currentAction.compare("FollowRight") == 0)
    {
        if(currentAnimation()->currentFrame() == 6)
            level()->soundMan()->playSound("glob_bite", static_cast<ZorbitLevel*>(level())->distanceToZorbit(position()));
    }
    */
}

void ZKops::attack()
{
    if(_attackTimer.getElapsedTime().asMilliseconds() > 100)
    {
        static_cast<Zorbit*>(level()->entityMan()->entity("Zorbit").ptr())->increaseCogs(-1);
        _attackTimer.reset(true);
    }
}

std::string ZKops::direction()
{
    if(directedRight())
        return "Right";
    else return "Left";
}



