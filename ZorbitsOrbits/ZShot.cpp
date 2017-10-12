// Copyright(c) 2017 Happy Rock Studios

#include <sstream>

#include "ZShot.h"
#include "Zorbit.h"
#include "ZorbitLevel.h"
#include "ZorbitsOrbits.h"
#include "JollyLava/Tool.h"

ZShot::ZShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float distance, std::string direction, float duration) : jl::Projectile(name,
            level, layerNames, direction, duration, "ZShot")
{
    jl::TextureDesc::Ptr tex;
    tex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/zshot/zshot.png",
            sf::Vector2f(10, 10), 20, 20);

    // Create root body.
    b2BodyDef bodyDef;
    bodyDef.bullet = true;
    bodyNew("ZShotBody", "foreground", tex, &bodyDef, initPos, 0);
    rootBody()->b2body()->SetLinearDamping(0);

    _initialDistance = distance;
    _spaceAngle = 0;

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

ZShot::ZShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float angle, float duration) : jl::Projectile(name,
            level, layerNames, "Space", duration, "ZShot")
{
    jl::TextureDesc::Ptr tex;
    tex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/zshot/zshot.png",
            sf::Vector2f(10, 10), 20, 20);

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("ZShotBody", "foreground", tex, &bodyDef, initPos, 0);
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

void ZShot::defineAnimations()
{
    _knownAnims["Move"] = new jl::Animation("Move", sf::IntRect(0, 0, 20, 20), 1, true);

    currentAnimIs(_knownAnims["Move"]);
}

void ZShot::defineActions()
{
    _knownActions["Move"] = new zShotMoveAction(this);
}

void ZShot::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    Projectile::beginContact(contact, thisFixUD, thatCollidable, thatFixUD);

    // Get the type of the other collidable for comparisons.
    string thatType = thatCollidable->type();

    if(thatType.compare("Enemy") == 0
       && thisFixUD->name().compare("Root") == 0
       && thatFixUD->name().compare("Root") == 0)
    {
        static_cast<jl::Character *>(thatCollidable.ptr())->decreaseHealth(_damage);
        toDie();

        std::stringstream sfx;
        sfx << static_cast<jl::Character *>(thatCollidable.ptr())->prefix() << "_hit";

        if(level()->soundMan()->sound(sfx.str()))
            level()->soundMan()->playSound(sfx.str(), true);
    }
    if(thatType.compare("Boss") == 0
       && thisFixUD->name().compare("Root") == 0
       && thatFixUD->name().compare("Root") == 0)
    {
        //toDie();
    }

    if(thatType.compare("RectShape") == 0
       || thatType.compare("Trash") == 0) toDie();
}

void ZShot::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    Projectile::endContact(contact, thisFixUD, thatCollidable, thatFixUD);
}

void ZShot::updateConcrete()
{
    if(noActions())
        _actionMan->pushAction(_knownActions["Move"]);
}

