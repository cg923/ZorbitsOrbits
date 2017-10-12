// Copyright(c) 2017 Happy Rock Studios

#include "ZGlobBat.h"
#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "JollyLava/Tool.h"

ZGlobBat::ZGlobBat(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle) : jl::Character(name,
            level, layerNames, "Enemy")
{
    _prefix = "globbat";

    // Body texture
    jl::TextureDesc::Ptr bodyTex;
    bodyTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/globbat/globbat.png",
            sf::Vector2f(100, 94), 200, 188);

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("ZGlobBatBody", "foreground", bodyTex, &bodyDef, initPos, initAngle);

    float width = 1.0f;
    float height = 0.7f;

    // Set values
    _maxHealth          = 3;
    _currentHealth      = 3;
    _damage             = 1;
    _maxSpeed           = 7;
    _jumpHeight         = 0;
    _jointDestroyed     = false;
    directedRightIs(false);

    defineFixtures(width, height);
    defineAnimations();
    defineSounds();
    defineActions();
}

void ZGlobBat::defineFixtures(float width, float height)
{
    // Torso fixture
    b2FixtureDef fixDef;
    b2PolygonShape fixShape;
    fixShape.SetAsBox(width, height);
    fixDef.shape = &fixShape;
    fixDef.friction = 0.0f;

    jl::FixtureUserData::Ptr fixUD = new jl::FixtureUserData("Root");
    rootBody()->fixtureNew(fixUD, &fixDef);

    jl::Character::defineFixtures(width * .5, height);
}

void ZGlobBat::defineAnimations()
{
    _knownAnims["MoveRight"]    = new jl::Animation("MoveRight", sf::IntRect(0, 0, 200, 188), 10, true);
    _knownAnims["MoveLeft"]     = new jl::Animation("MoveLeft", sf::IntRect(0, 0, 200, 188), 10, true);
    _knownAnims["Die"]          = new jl::Animation("Die", sf::IntRect(0, 188, 200, 188), 10, false);

    currentAnimIs(_knownAnims["MoveLeft"]);
}

void ZGlobBat::defineSounds()
{
    level()->soundMan()->soundNew("globbat_hit", "resources/sounds/globbat_hit.wav", false);
    level()->soundMan()->soundNew("globbat_die", "resources/sounds/globbat_die.wav", false);
    level()->soundMan()->soundNew("globbat_move", "resources/sounds/globbat_move.wav", true);
}

void ZGlobBat::defineActions()
{
    _knownActions["MoveRight"]     = new gbMRightAction(this);
    _knownActions["MoveLeft"]      = new gbMLeftAction(this);
    _knownActions["Die"]           = new jl::cDieAction("glob_die", this);
}

void ZGlobBat::readySet()
{
    Actor::readySet();

    if(gravityField())
    {
        b2DistanceJointDef jointDef;
        jointDef.bodyA = rootBody()->b2body();
        jointDef.bodyB = gravityField()->b2body();
        jointDef.collideConnected = true;
        jointDef.length = jl::Tool::distance(this->position(), gravityField()->position());
        _distanceJoint = (b2DistanceJoint *)this->level()->b2world()->CreateJoint(&jointDef);
    }
}

void ZGlobBat::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    if(currentHealth() <= 0)
        return;

    // First run through all collisions handled by superclasses.
    Character::beginContact(contact, thisFixUD, thatCollidable, thatFixUD);

    // Get the type of the other collidable for comparisons.
    string thatType = thatCollidable->type();

    if(thatCollidable->type().compare("Player") != 0
       && thatCollidable->type().compare("Cog") != 0
       && thatCollidable->type().compare("ZShot") != 0
       && thatCollidable->type().compare("Event") != 0
       && thatCollidable->type().compare("GravityField") != 0
       && thisFixUD->name().compare("Root") == 0)
       {
            switchDirection();
       }
}

void ZGlobBat::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    Character::endContact(contact, thisFixUD, thatCollidable, thatFixUD);
}

void ZGlobBat::updateConcrete()
{
    if(noActions())
    {
        if(directedRight())
            actionMan()->pushAction(knownAction("MoveRight"));
        else
            actionMan()->pushAction(knownAction("MoveLeft"));
    }

    if(gravityField())
        rootBody()->b2body()->SetTransform(position(), polarAngle() - jl::Tool::PI() / 2);

    if(_currentHealth <= 0 && !_jointDestroyed)
    {
        level()->b2world()->DestroyJoint(_distanceJoint);
        _jointDestroyed = true;
    }
}

void ZGlobBat::move(std::string direction)
{
    if (_maxSpeed < 0)
        throw fwk::InternalException("Speed must be non-negative.");

    if(direction == "Right")
    {
        float theta = polarAngle();
        float forceX = _maxSpeed * sin(theta);
        float forceY = -_maxSpeed * cos(theta);
        rootBody()->b2body()->SetLinearVelocity(b2Vec2(forceX, forceY));
    }
    else if(direction == "Left")
    {
        float theta = polarAngle();
        float forceX = -_maxSpeed * sin(theta);
        float forceY = _maxSpeed * cos(theta);
        rootBody()->b2body()->SetLinearVelocity(b2Vec2(forceX, forceY));
    }
}

void ZGlobBat::switchDirection()
{
    Actor::switchDirection();
    actionMan()->clearActions();

}

std::string ZGlobBat::direction()
{
    if(directedRight())
        return "Right";
    else return "Left";
}

void ZGlobBat::destroyJoints()
{
    if(!_jointDestroyed)
    {
        level()->b2world()->DestroyJoint( _distanceJoint );
        _jointDestroyed = true;
    }
}

void ZGlobBat::die()
{
    Entity::die();
}

void gbMRightAction::update(bool animFinished)
{
    static_cast<ZGlobBat *>(actor())->move("Right");

    if(!actor()->level()->soundMan()->sound("globbat_move")->playing())
        actor()->level()->soundMan()->playSound("globbat_move",
                                                static_cast<ZorbitLevel*>(actor()->level())->distanceToZorbit(actor()->position()));

    Action::update(animFinished);
}

void gbMLeftAction::update(bool animFinished)
{
    static_cast<ZGlobBat *>(actor())->move("Left");

    if(!actor()->level()->soundMan()->sound("globbat_move")->playing())
        actor()->level()->soundMan()->playSound("globbat_move",
                                                static_cast<ZorbitLevel*>(actor()->level())->distanceToZorbit(actor()->position()));

    Action::update(animFinished);
}



