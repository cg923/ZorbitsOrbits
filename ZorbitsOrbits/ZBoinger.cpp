
// Copyright(c) 2017 Happy Rock Studios

#include "ZBoinger.h"
#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "JollyLava/Tool.h"

ZBoinger::ZBoinger(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle, float strength, bool dynamic) : jl::Prop(name,
            level, layerNames, "Boinger")
{
    // Body texture
    jl::TextureDesc::Ptr bodyTex;
    bodyTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/boinger/boinger.png",
            sf::Vector2f(71, 38), 143, 77);

    // Create root body.
    b2BodyDef bodyDef;
    bodyDef.linearDamping = 0.0f;
    bodyDef.angularDamping = 0.0f;

    if(dynamic)
    {
        bodyNew("ZBoingerBody", "foreground", bodyTex, &bodyDef, initPos, initAngle, b2_dynamicBody);
        _anchored = false;
    }
    else bodyNew("ZBoingerBody", "foreground", bodyTex, &bodyDef, initPos, initAngle);

    float width = 1.0f;
    float height = 0.5f;

    defineFixtures(width, height);
    defineAnimations();

    _strength = strength * static_cast<ZorbitLevel*>(this->level())->_boingerFactor;
    _dynamic = dynamic;
}

void ZBoinger::defineFixtures(float width, float height)
{
    // Main fixture
    b2FixtureDef fixDef;
    b2PolygonShape fixShape;
    fixShape.SetAsBox(width, height);
    fixDef.shape = &fixShape;
    fixDef.density = 0.1f;
    fixDef.friction = 0.0f;

    jl::FixtureUserData::Ptr fixUD = new jl::FixtureUserData("Root");
    rootBody()->fixtureNew(fixUD, &fixDef);

    fixShape.SetAsBox(width * 0.75, height / 4, b2Vec2(0, height), 0);
    fixDef.shape = &fixShape;

    jl::FixtureUserData::Ptr fixUD2 = new jl::FixtureUserData("Top");
    rootBody()->fixtureNew(fixUD2, &fixDef);
}

void ZBoinger::defineAnimations()
{
    _knownAnims["Stop"]    = new jl::Animation("Stop", sf::IntRect(0, 0, 143, 77), 1, false);

    currentAnimIs(_knownAnims["Stop"]);
}

void ZBoinger::readySet()
{
    Entity::readySet();
}

void ZBoinger::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    Prop::beginContact(contact, thisFixUD, thatCollidable, thatFixUD);
}

void ZBoinger::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    Prop::endContact(contact, thisFixUD, thatCollidable, thatFixUD);
}

void ZBoinger::handleInput(std::string input)
{
    if(input.compare("Invisible") == 0)
        visibilityIs(false);
    else if(input.compare("Visible") == 0)
        visibilityIs(true);
}
