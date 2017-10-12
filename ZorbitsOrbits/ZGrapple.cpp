// Copyright(c) 2017 Happy Rock Studios

#include "ZGrapple.h"
#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "JollyLava/Tool.h"

ZGrapple::ZGrapple(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle, bool dynamic) : jl::Prop(name,
            level, layerNames, "Grapple")
{
    // Body texture
    jl::TextureDesc::Ptr bodyTex;

    bodyTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/grapple/grapple.png",
            sf::Vector2f(24, 72), 48, 95);

    // Create root body.
    b2BodyDef bodyDef;
    if(dynamic)
        bodyNew("ZGrappleBody", "foreground", bodyTex, &bodyDef, initPos, jl::Tool::b2Angle(initAngle), b2_dynamicBody);
    else bodyNew("ZGrappleBody", "foreground", bodyTex, &bodyDef, initPos, jl::Tool::b2Angle(initAngle));

    float width = 0.3f;
    float height = 0.3f;

    defineFixtures(width, height);
    defineAnimations();

    _activated = false;
    _dynamic = dynamic;
    if(_dynamic) _anchored = false;
    _hasZCircle = false;
}

void ZGrapple::defineFixtures(float width, float height)
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

void ZGrapple::defineAnimations()
{
    _knownAnims["Deactivated"]  = new jl::Animation("Deactivated", sf::IntRect(0, 0, 48, 95), 1, false);
    _knownAnims["Activated"]    = new jl::Animation("Activated", sf::IntRect(0, 96, 48, 95), 4, true);

    currentAnimIs(_knownAnims["Deactivated"]);
}

void ZGrapple::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    Prop::beginContact(contact, thisFixUD, thatCollidable, thatFixUD);
}

void ZGrapple::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    Prop::endContact(contact, thisFixUD, thatCollidable, thatFixUD);
}

void ZGrapple::updateConcrete()
{
    orientTowardGravityFieldCenter();

    if(_activated)
        currentAnimIs(_knownAnims["Activated"]);
    else
        currentAnimIs(_knownAnims["Deactivated"]);
}
