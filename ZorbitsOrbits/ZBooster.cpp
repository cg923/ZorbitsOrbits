
// Copyright(c) 2017 Happy Rock Studios

#include "ZBooster.h"
#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "JollyLava/Tool.h"

ZBooster::ZBooster(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle, double strength) : jl::Prop(name,
            level, layerNames, "Booster")
{
    // Body texture
    jl::TextureDesc::Ptr bodyTex;
    bodyTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/booster/booster.png",
            sf::Vector2f(25, 66), 30, 144);

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("ZBoosterBody", "foreground", bodyTex, &bodyDef, initPos, initAngle);

    float width = 0.3f;
    float height = 1.2f;

    defineFixtures(width, height);
    defineAnimations();

    _boostStrength = strength * static_cast<ZorbitLevel*>(this->level())->_boingerFactor;
}

void ZBooster::defineFixtures(float width, float height)
{
    // Main fixture
    b2FixtureDef fixDef;
    b2PolygonShape fixShape;
    fixShape.SetAsBox(width, height);
    fixDef.shape = &fixShape;
    fixDef.density = 1.0f;
    fixDef.friction = 3.0f;
    fixDef.isSensor = true;

    jl::FixtureUserData::Ptr fixUD = new jl::FixtureUserData("Root");
    rootBody()->fixtureNew(fixUD, &fixDef);
}

void ZBooster::defineAnimations()
{
    _knownAnims["Stop"]    = new jl::Animation("Stop", sf::IntRect(0, 0, 30, 144), 1, false);

    currentAnimIs(_knownAnims["Stop"]);
}

void ZBooster::readySet()
{
    Entity::readySet();
}

void ZBooster::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    Prop::beginContact(contact, thisFixUD, thatCollidable, thatFixUD);
}

void ZBooster::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    Prop::endContact(contact, thisFixUD, thatCollidable, thatFixUD);
}

