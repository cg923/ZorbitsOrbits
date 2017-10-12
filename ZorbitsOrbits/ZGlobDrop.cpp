// Copyright(c) 2017 Happy Rock Studios

#include "ZGlobDrop.h"
#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "Zorbit.h"
#include "JollyLava/Tool.h"

ZGlobDrop::ZGlobDrop(std::string name,
                     ZorbitLevel * level,
                     std::vector<string> layerNames,
                     b2Vec2 initPos,
                     int initAngle,
                     bool sideways) : jl::Character(name,
                                                    level,
                                                    layerNames,
                                                    "Enemy")
{
    _prefix = "globdrop";

    // Body texture
    jl::TextureDesc::Ptr bodyTex;
    bodyTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/globdrop/globdrop.png",
        sf::Vector2f(50, 40), 100, 128);

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("ZGlobDropBody", "foreground", bodyTex, &bodyDef, initPos, initAngle, b2_staticBody);

    float width = 0.2f;
    float height = 0.2f;

    // Set values
    _maxHealth      = 3;
    _currentHealth  = 3;
    _damage         = 1;
    _maxSpeed       = 0;
    _jumpHeight     = 0;
    _dropCount      = 0;
    _dropped        = false;
    directedRightIs(false);

    defineFixtures(width, height);
    defineAnimations(sideways);
    defineSounds();
    defineActions();

    int spriteSize = rand() % 5;
    if(spriteSize == 0)
        rootBody()->sprite()->scale(0.8,0.8);
    if(spriteSize == 1)
        rootBody()->sprite()->scale(-1,1);
}

void ZGlobDrop::defineFixtures(float width, float height)
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

    jl::Character::defineFixtures(width, height);
}

void ZGlobDrop::defineAnimations(bool sideways)
{
    //_knownAnims["Stop"]         = new jl::Animation("Stop", sf::IntRect(0, 0, 100, 128), 1, false);
    if(sideways)
        _knownAnims["Attack"]       = new jl::Animation("Attack", sf::IntRect(0, 129, 100, 128), 7, true, 100);
    else
        _knownAnims["Attack"]       = new jl::Animation("Attack", sf::IntRect(0, 0, 100, 128), 7, true, 100);

    //_knownAnims["Die"]          = new jl::Animation("Die", sf::IntRect(0, 80, 80, 80), 1, false);

    currentAnimIs(_knownAnims["Attack"]);
}

void ZGlobDrop::defineSounds()
{
    level()->soundMan()->soundNew("globdrop_drip", "resources/sounds/drip.wav", false);
    //level()->soundMan()->soundNew("glob_die", new jl::Sound("glob_die", "resources/sounds/glob_die.wav", false));
}

void ZGlobDrop::defineActions()
{
    _knownActions["Attack"]             = new gdAttackAction(this);
    _knownActions["Stop"]               = new jl::aStopAction(1, this);
    _knownActions["Die"]                = new jl::cDieAction("glob_die", this);
}

void ZGlobDrop::reset()
{

}

void ZGlobDrop::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    Character::beginContact(contact, thisFixUD, thatCollidable, thatFixUD);

    // Get the type of the other collidable for comparisons.
    string thatType = thatCollidable->type();

    // Main fixture
    if(thisFixUD->name().compare("Root") == 0)
    {
        if(thatType.compare("GravityField") == 0)
            nearbyGravityFieldsInsert(static_cast<jl::GravityField *>(thatCollidable.ptr()));
    }
}

void ZGlobDrop::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    Character::endContact(contact, thisFixUD, thatCollidable, thatFixUD);

    // Get the type of the other collidable for comparisons.
    string thatType = thatCollidable->type();

    // Main fixture
    if(thisFixUD->name().compare("Root") == 0)
    {
        if(thatType.compare("GravityField") == 0)
            nearbyGravityFieldsRemove(static_cast<jl::GravityField *>(thatCollidable.ptr()));
    }
}

void ZGlobDrop::updateConcrete()
{
    if(!gravityField())
        gravityFieldIs(closestNearbyGravityField());

    if(noActions() && static_cast<Zorbit*>(level()->entityMan()->entity("Zorbit").ptr())->currentHealth() > 0)
    {
        _actionMan->pushAction(_knownActions["Attack"]);
    }
}

void ZGlobDrop::attack()
{
    if(_currentAnim->currentFrame() == 1) _dropped = false;
    if(_currentAnim->currentFrame() == 6 && !_dropped)
    {
        if(_dropCount > 100)
            _dropCount = 1;

        std::stringstream name;
        name << this->name() << "Drop" << _dropCount;

        float forceX = 0.85 * cos(angle() - jl::Tool::PI() / 2);
        float forceY = 0.85 * sin(angle() - jl::Tool::PI() / 2);
        b2Vec2 initPos = body("ZGlobDropBody")->b2body()->GetPosition() + b2Vec2(forceX, forceY);

        // Create shot
        Drop::Ptr drop = new Drop(name.str(), static_cast<ZorbitLevel *>(level()), _layerNames, initPos, angle());
        level()->entityMan()->entityNew(drop->name(), "Layer1", drop);

        level()->soundMan()->playSound("globdrop_drip", static_cast<ZorbitLevel*>(level())->distanceToZorbit(position()));

        _dropCount++;
        _dropped = true;
    }
}

Drop::Drop(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float angle) : jl::Projectile(name,
            level, layerNames, "Left", 100, "Drop")
{
    jl::TextureDesc::Ptr tex;
    tex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/globdrop/drop.png",
            sf::Vector2f(15, 25), 30, 50);

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("DropBody", "foreground", tex, &bodyDef, initPos, angle);
    rootBody()->b2body()->SetLinearDamping(0);

    _initialDistance = 0;
    _spaceAngle = 0;

    // Set values
    _maxSpeed       = 30;
    _damage         = 1;

    float width     = 0.3;
    float height    = 0.3;

    defineFixtures(width, height);
    defineAnimations();
    defineActions();
    defineSounds();

}

void Drop::defineAnimations()
{
    _knownAnims["Drop"] = new jl::Animation("Drop", sf::IntRect(0, 0, 30, 50), 1, true);

    currentAnimIs(_knownAnims["Drop"]);
}

void Drop::defineActions()
{
    _knownActions["Drop"] = new dDropAction(this);
}

void Drop::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
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

void Drop::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    Projectile::endContact(contact, thisFixUD, thatCollidable, thatFixUD);
}

void Drop::updateConcrete()
{
    if(gravityField())
        rootBody()->b2body()->SetTransform(position(), polarAngle() - jl::Tool::PI() / 2);

    if(noActions())
        _actionMan->pushAction(_knownActions["Drop"]);
}

void Drop::drop()
{
}





