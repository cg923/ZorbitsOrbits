// Copyright(c) 2017 Happy Rock Studios

#include "ZGlob.h"
#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "JollyLava/Tool.h"

ZGlob::ZGlob(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle) : jl::Character(name,
            level, layerNames, "Enemy")
{
    _prefix = "glob";

    // Body texture
    jl::TextureDesc::Ptr bodyTex;
    bodyTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/glob/glob.png",
            sf::Vector2f(102, 130), 204, 200);

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("ZGlobBody", "foreground", bodyTex, &bodyDef, initPos, initAngle);

    float width = 1.0f;
    float height = 1.1f;

    // Set values
    _maxHealth      = 3;
    _currentHealth  = 3;
    _damage         = 1;
    _maxSpeed       = 3;
    _jumpHeight     = 30;
    _lFootContacts  = 0;
    _rFootContacts  = 0;
    _leftSensor     = false;
    _rightSensor    = false;
    directedRightIs(false);

    defineFixtures(width, height);
    defineAnimations();
    defineSounds();
    defineActions();
}

void ZGlob::defineFixtures(float width, float height)
{
    // Torso fixture
    b2FixtureDef fixDef;
    b2PolygonShape fixShape;
    fixShape.SetAsBox(width * .75, height * 0.85);
    fixDef.shape = &fixShape;
    fixDef.density = 0.5f;
    fixDef.friction = 0.0f;

    jl::FixtureUserData::Ptr fixUD = new jl::FixtureUserData("Root");
    rootBody()->fixtureNew(fixUD, &fixDef);

    // Head (so Zorbit can't stand on a Glob)
    b2CircleShape circleShape;
    circleShape.m_radius = width * .8;
    circleShape.m_p.Set(0, height * .8);
    fixDef.shape = &circleShape;

    jl::FixtureUserData::Ptr fixUD5 = new jl::FixtureUserData("Head");
    rootBody()->fixtureNew(fixUD5, &fixDef);

    // Left foot
    fixShape.SetAsBox(width / 8, height / 2, b2Vec2(-width, -height), 0);
    fixDef.shape = &fixShape;
    fixDef.isSensor = true;

    jl::FixtureUserData::Ptr fixUD3 = new jl::FixtureUserData("LeftFoot");
    rootBody()->fixtureNew(fixUD3, &fixDef);

    // Right foot
    fixShape.SetAsBox(width / 8, height / 2, b2Vec2(width, -height), 0);
    fixDef.shape = &fixShape;
    fixDef.isSensor = true;

    jl::FixtureUserData::Ptr fixUD4 = new jl::FixtureUserData("RightFoot");
    rootBody()->fixtureNew(fixUD4, &fixDef);

    // Zorbit Right detection fixture
    fixShape.SetAsBox(width * 3, height * 1.5, b2Vec2(width * 3, height /2), 0);
    fixDef.shape = &fixShape;
    fixDef.density = 0.0f;

    jl::FixtureUserData::Ptr fixUD2 = new jl::FixtureUserData("RightSensor");
    rootBody()->fixtureNew(fixUD2, &fixDef);

    // Zorbit Left detection fixture
    fixShape.SetAsBox(width * 3, height * 1.5, b2Vec2(-3 * width, height /2), 0);
    fixDef.shape = &fixShape;
    fixDef.density = 0.0f;

    jl::FixtureUserData::Ptr fixUD6 = new jl::FixtureUserData("LeftSensor");
    rootBody()->fixtureNew(fixUD6, &fixDef);

    jl::Character::defineFixtures(width * .5, height);
}

void ZGlob::defineAnimations()
{
    _knownAnims["Stop"]         = new jl::Animation("Stop", sf::IntRect(0, 0, 204, 200), 17, false);
    _knownAnims["MoveRight"]    = new jl::Animation("MoveRight", sf::IntRect(0, 201, 204, 200), 17, true);
    _knownAnims["MoveLeft"]     = new jl::Animation("MoveLeft", sf::IntRect(0, 201, 204, 200), 17, true);
    _knownAnims["FollowRight"]  = new jl::Animation("FollowRight", sf::IntRect(0, 401, 204, 200), 17, true);
    _knownAnims["FollowLeft"]   = new jl::Animation("FollowLeft", sf::IntRect(0, 401, 204, 200), 17, true);
    _knownAnims["Attack"]       = new jl::Animation("Attack", sf::IntRect(0, 401, 204, 200), 17, true);
    _knownAnims["Die"]          = new jl::Animation("Die", sf::IntRect(0, 800, 204, 200), 10, false);

    currentAnimIs(_knownAnims["Stop"]);
}

void ZGlob::defineSounds()
{
    level()->soundMan()->soundNew("glob_hit", "resources/sounds/glob_hit.wav", false);
    level()->soundMan()->soundNew("glob_bite", "resources/sounds/glob_bite.wav", false);
    level()->soundMan()->soundNew("glob_jump", "resources/sounds/glob_jump.wav", false);
    level()->soundMan()->soundNew("glob_die", "resources/sounds/glob_die.wav", false);
}

void ZGlob::defineActions()
{
    _knownActions["MoveRightShort"]     = new jl::cMRightAction(1, this);
    _knownActions["MoveRightLong"]      = new jl::cMRightAction(3, this);
    _knownActions["MoveLeftShort"]      = new jl::cMLeftAction(1, this);
    _knownActions["MoveLeftLong"]       = new jl::cMLeftAction(3, this);
    _knownActions["FollowRight"]        = new jl::cMRightAction(1, this, "FollowRight");
    _knownActions["FollowLeft"]         = new jl::cMLeftAction(1, this, "FollowLeft");
    _knownActions["Stop"]               = new jl::aStopAction(1, this);
    _knownActions["Die"]                = new jl::cDieAction("glob_die", this);
}

void ZGlob::reset()
{
    Character::reset();
    _leftSensor = false;
    _rightSensor = false;
}

void ZGlob::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
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
        // Close enough to see.
        if( thisFixUD->name().compare("RightSensor") == 0
            && thatFixUD->name().compare("Root") == 0)
                _rightSensor = true;
    }
}

void ZGlob::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
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
        actionMan()->clearActions();
        actionMan()->pushAction(knownAction("Stop"));
    }

    if(thatType.compare("Player") == 0
       && thisFixUD->name().compare("RightSensor") == 0
       && thatFixUD->name().compare("Root") == 0)
    {
        _rightSensor = false;
        actionMan()->clearActions();
        actionMan()->pushAction(knownAction("Stop"));
    }
}

void ZGlob::updateConcrete()
{
    if(noActions() && !_leftSensor && !_rightSensor)
    {
        // Stop
        _actionMan->pushAction(_knownActions["Stop"]);

        // Move
        int duration = rand() % 2 + 1;
        int direction = rand() % 2 + 1;

        if(direction == 1)   // Right
        {
            if(duration == 1){_actionMan->pushAction(_knownActions["MoveRightShort"]);}
            else{_actionMan->pushAction(_knownActions["MoveRightLong"]);}
        }
        else                // Left
        {
            if(duration == 1){_actionMan->pushAction(_knownActions["MoveLeftShort"]);}
            else{_actionMan->pushAction(_knownActions["MoveLeftLong"]);}
        }
    }

    // Sees Zorbit
    if(_leftSensor && _currentHealth > 0)
    {
        _actionMan->pushAction(_knownActions["FollowLeft"], "Top", true);
    }
    if(_rightSensor && _currentHealth > 0)
    {
        _actionMan->pushAction(_knownActions["FollowRight"], "Top", true);
    }

    // Walking off a ledge
    if(_lFootContacts <= 0 && _footContacts > 0)
    {
       if(_actionMan->peekAction()->name().compare("MoveLeft") == 0
          || _actionMan->peekAction()->name().compare("Stop") == 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["MoveRightShort"]);
        }
    }
    if(_rFootContacts <= 0 && _footContacts > 0)
    {
        if(_actionMan->peekAction()->name().compare("MoveRight") == 0
           || _actionMan->peekAction()->name().compare("Stop") == 0)
        {
            _actionMan->clearActions();
            _actionMan->pushAction(_knownActions["MoveLeftShort"]);
        }
    }

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
}

void ZGlob::attack()
{
}

void ZGlob::move(std::string direction)
{
    if(_lFootContacts <= 0 && direction.compare("Left") == 0)
        return;
    if(_rFootContacts <= 0 && direction.compare("Right") == 0)
        return;

    jl::Character::move(direction);
}

std::string ZGlob::direction()
{
    if(directedRight())
        return "Right";
    else return "Left";
}


