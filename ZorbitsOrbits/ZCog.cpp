
// Copyright(c) 2017 Happy Rock Studios

#include "ZCog.h"
#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "JollyLava/Tool.h"

ZCog::ZCog(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle) : jl::Prop(name,
            level, layerNames, "Cog")
{
    // Body texture
    jl::TextureDesc::Ptr bodyTex;
    bodyTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/cog/cog.png",
            sf::Vector2f(30, 30), 60, 60);

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("ZCogBody", "foreground", bodyTex, &bodyDef, initPos, initAngle);

    float width = 0.4f;
    float height = 0.4f;

    defineFixtures(width, height);
    defineAnimations();

    _active = true;
}

void ZCog::defineFixtures(float width, float height)
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

    //jl::Prop::defineFixtures(width, height);
}

void ZCog::defineAnimations()
{
    _knownAnims["Stop"]    = new jl::Animation("Stop", sf::IntRect(0, 0, 60, 60), 15, true);

    currentAnimIs(_knownAnims["Stop"]);
}

void ZCog::readySet()
{
    Prop::readySet();
    orientTowardGravityFieldCenter();
}

void ZCog::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    Prop::beginContact(contact, thisFixUD, thatCollidable, thatFixUD);
}

void ZCog::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    Prop::endContact(contact, thisFixUD, thatCollidable, thatFixUD);
}

void ZCog::activate()
{
    _active = true;
    visibilityIs(true);
}

void ZCog::deactivate()
{
    _active = false;
    visibilityIs(false);
}


