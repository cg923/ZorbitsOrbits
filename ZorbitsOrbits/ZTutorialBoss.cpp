
// Copyright(c) 2017 Happy Rock Studios

#include "ZTutorialBoss.h"
#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "Zorbit.h"
#include "JollyLava/Tool.h"
#include "ZShot.h"

ZTutorialBoss::ZTutorialBoss(ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle) : ZBoss("TutorialBoss",
            level, layerNames)
{
    _prefix = "tboss";

    // Body texture
    jl::TextureDesc::Ptr bodyTex;
    bodyTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/tutorialboss/tutorialboss.png",
            sf::Vector2f(225, 200), 450, 400);

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("ZTutorialBossBody", "foreground", bodyTex, &bodyDef, initPos, initAngle);

    float width = 2.0f;
    float height = 1.5f;

    // Set values
    _active         = false;
    _maxHealth      = 200;   //200
    _currentHealth  = _maxHealth;
    _damage         = 1;
    _numShots       = 1;
    _maxSpeed       = 3;
    _jumpHeight     = 0;
    _lFootContacts  = 0;
    _rFootContacts  = 0;
    _attacking      = false;
    _seesZorbit     = false;
    directedRightIs(false);

    defineFixtures(width, height);
    defineAnimations();
    defineSounds();
    defineActions();
}

void ZTutorialBoss::defineFixtures(float width, float height)
{
    // Torso fixture
    b2FixtureDef fixDef;
    b2PolygonShape fixShape;
    fixShape.SetAsBox(width, height);
    fixDef.shape = &fixShape;
    fixDef.density = 1.0f;
    fixDef.friction = 0.0f;

    jl::FixtureUserData::Ptr fixUD = new jl::FixtureUserData("Root");
    rootBody()->fixtureNew(fixUD, &fixDef);

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


    // Zorbit detection fixture
    b2CircleShape circShape;
    circShape.m_radius = 6;
    fixDef.shape = &circShape;
    fixDef.density = 0.0f;
    //fixDef.isSensor = true;

    jl::FixtureUserData::Ptr fixUD2 = new jl::FixtureUserData("Sensor");
    rootBody()->fixtureNew(fixUD2, &fixDef);


    jl::Character::defineFixtures(width * .75, height);
}

void ZTutorialBoss::defineAnimations()
{
    _knownAnims["Stop"]             = new jl::Animation("Stop", sf::IntRect(0, 0, 450, 400), 36, true);
    _knownAnims["MoveRight"]        = _knownAnims["Stop"];
    _knownAnims["MoveLeft"]         = _knownAnims["Stop"];
    _knownAnims["AttackLeft"]       = new jl::Animation("AttackLeft", sf::IntRect(0, 1601, 450, 399), 20, false);
    _knownAnims["AttackRight"]      = new jl::Animation("AttackRight", sf::IntRect(0, 1601, 450, 399), 20, false);
    _knownAnims["Die"]              = new jl::Animation("Die", sf::IntRect(0, 2800, 450, 400), 26, false);

    currentAnimIs(_knownAnims["Stop"]);
}

void ZTutorialBoss::defineSounds()
{
    level()->soundMan()->soundNew("tboss_hit", "resources/sounds/tboss_hit.wav", false);
    level()->soundMan()->soundNew("tboss_die", "resources/sounds/tboss_die.wav", false);
    level()->soundMan()->soundNew("tboss_roar", "resources/sounds/tboss_roar.wav", false);
    level()->soundMan()->soundNew("tboss_intro", "resources/sounds/tboss_intro.wav", false);
}

void ZTutorialBoss::defineActions()
{
    _knownActions["MoveRightShort"]     = new jl::cMRightAction(1, this);
    _knownActions["MoveRightLong"]      = new jl::cMRightAction(3, this);
    _knownActions["MoveLeftShort"]      = new jl::cMLeftAction(1, this);
    _knownActions["MoveLeftLong"]       = new jl::cMLeftAction(3, this);
    _knownActions["AttackLeft"]         = new tbLAttackAction(this);
    _knownActions["AttackRight"]        = new tbRAttackAction(this);
    _knownActions["Stop"]               = new jl::aStopAction(1, this);
    _knownActions["Die"]                = new bDieAction("tb_die", this);
}

void ZTutorialBoss::reset()
{
    ZBoss::reset();

    _maxSpeed = 3;
}

void ZTutorialBoss::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    ZBoss::beginContact(contact, thisFixUD, thatCollidable, thatFixUD);

    // Get the type of the other collidable for comparisons.
    string thatType = thatCollidable->type();
    if(thatType.compare("ZShot") == 0
       && thisFixUD->name().compare("Root") == 0)
    {
        static_cast<ZShot *>(thatCollidable.ptr())->toDie();
        hit();
        decreaseHealth(static_cast<ZShot *>(thatCollidable.ptr())->damage());

        if(level()->soundMan()->sound("tboss_hit"))
            level()->soundMan()->playSound("tboss_hit", true);
    }
    if(thatType.compare("Player") == 0
       && thatFixUD->name().compare("Root") == 0
       && thisFixUD->name().compare("Root") == 0)
    {
        static_cast<Zorbit *>(thatCollidable.ptr())->hit(_damage);
    }
}

void ZTutorialBoss::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    ZBoss::endContact(contact, thisFixUD, thatCollidable, thatFixUD);
}

void ZTutorialBoss::updateConcrete()
{
    if(!_active)
    {
        if(noActions())
            _actionMan->pushAction(_knownActions["Stop"]);
    }
    else
    {
        if(noActions())
        {
            if(_currentHealth >= _maxHealth * 3/5)
            {
                // Phase 1
                moveRandom();
                _actionMan->pushAction(_knownActions["Stop"], "Bottom", false);
                _actionMan->pushAction(_knownActions["AttackLeft"], "Bottom", false);
                moveRandom();
                _actionMan->pushAction(_knownActions["Stop"], "Bottom", false);
                _actionMan->pushAction(_knownActions["AttackRight"], "Bottom", false);
            }
            else if(_currentHealth < _maxHealth * 3/5 && _currentHealth > 0)
            {
                // Phase 2
                _actionMan->pushAction(_knownActions["Stop"], "Bottom", false);
                _actionMan->pushAction(_knownActions["AttackLeft"], "Bottom", false);
                _actionMan->pushAction(_knownActions["AttackRight"], "Bottom", false);
                _actionMan->pushAction(_knownActions["AttackLeft"], "Bottom", false);
                _actionMan->pushAction(_knownActions["AttackRight"], "Bottom", false);
                moveRandom();
            }

            _maxSpeed += 3;
        }
    }
}

void ZTutorialBoss::moveRandom()
{
    int duration = rand() % 2 + 1;
    int direction = rand() % 2 + 1;

    if(direction == 1)   // Right
    {
        if(duration == 1){_actionMan->pushAction(_knownActions["MoveRightShort"], "Bottom", false);}
        else{_actionMan->pushAction(_knownActions["MoveRightLong"], "Bottom", false);}
    }
    else                // Left
    {
        if(duration == 1){_actionMan->pushAction(_knownActions["MoveLeftShort"], "Bottom", false);}
        else{_actionMan->pushAction(_knownActions["MoveLeftLong"], "Bottom", false);}
    }
}

void ZTutorialBoss::attack(std::string direction)
{
    if(gravityField())
    {
        // Calculate name
        if(_numShots == 4)
            _numShots = 1;

        std::stringstream name;
        name << "TBShot" << _numShots;

        // Calculate shot
        b2Vec2 initPos = body("ZTutorialBossBody")->b2body()->GetPosition();
        float shotRadius = jl::Tool::distance(initPos, gravityField()->position()) + 0.25;

        // Create shot
        TBShot::Ptr shot = new TBShot(name.str(), static_cast<ZorbitLevel *>(level()), _layerNames, initPos, shotRadius, direction);
        level()->entityMan()->entityNew(shot->name(), "Layer1", shot);

        _numShots++;

        // Play sound
        level()->soundMan()->playSound("tboss_roar", true);
    }
    else throw fwk::InternalException("TutorialBoss should not be placed outside a gravity field!");
}

// **************************
//  TBShot methods
// **************************

TBShot::TBShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float distance, std::string direction, float duration) : jl::Projectile(name,
            level, layerNames, direction, duration, "TBShot")
{
    jl::TextureDesc::Ptr tex;
    tex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/tutorialboss/tbshot.png",
            sf::Vector2f(27, 27), 54, 54);

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("TBShotBody", "foreground", tex, &bodyDef, initPos, 0);
    rootBody()->b2body()->SetLinearDamping(0);

    _initialDistance = distance + 0.75f;
    _spaceAngle = 0;

    // Set values
    _maxSpeed       = 10;
    _damage         = 1;

    float width     = 0.25;
    float height    = 0.25;

    defineFixtures(width, height);
    defineAnimations();
    defineActions();
    defineSounds();

}

TBShot::TBShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float angle, float duration) : jl::Projectile(name,
            level, layerNames, "Space", duration, "TBShot")
{
    jl::TextureDesc::Ptr tex;
    tex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/tutorialboss/tbshot.png",
            sf::Vector2f(27, 27), 54, 54);

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("TBShotBody", "foreground", tex, &bodyDef, initPos, 0);
    rootBody()->b2body()->SetLinearDamping(0);

    _initialDistance = 0;
    _spaceAngle = angle;

    // Set values
    _maxSpeed       = 10;
    _damage         = 1;

    float width     = 0.25;
    float height    = 0.25;

    defineFixtures(width, height);
    defineAnimations();
    defineActions();
    defineSounds();

}

void TBShot::defineAnimations()
{
    _knownAnims["Move"] = new jl::Animation("Move", sf::IntRect(0, 0, 54, 54), 6, true);

    currentAnimIs(_knownAnims["Move"]);
}

void TBShot::defineActions()
{
    _knownActions["Move"] = new tbShotMoveAction(this);
}

void TBShot::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    Projectile::beginContact(contact, thisFixUD, thatCollidable, thatFixUD);

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

void TBShot::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    Projectile::endContact(contact, thisFixUD, thatCollidable, thatFixUD);
}

void TBShot::updateConcrete()
{
    if(noActions())
        _actionMan->pushAction(_knownActions["Move"]);
}


