
// Copyright(c) 2017 Happy Rock Studios

#include "ZPlatform.h"
#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "Zorbit.h"
#include "JollyLava/Tool.h"

ZPlatform::ZPlatform(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle, std::string activated) : jl::Prop(name,
            level, layerNames, "Platform")
{
    // Body texture
    jl::TextureDesc::Ptr bodyTex;
    bodyTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/platform/platform.png",
            sf::Vector2f(75, 16), 150, 32);

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("ZPlatformBody", "foreground", bodyTex, &bodyDef, initPos, initAngle);

    float width = 2.25f;
    float height = 0.25f;

    defineFixtures(width, height);
    defineAnimations();
    this->level()->soundMan()->soundNew("platform_switch", "resources/sounds/platform_switch.wav", false);

    _initialActivation = activated;
    _touchingZorbit = false;

    if(_initialActivation.compare("True") == 0)
    {
        _activated = true;
        currentAnimIs(_knownAnims["Activated"]);
        rootBody()->collisionIs(true);
    }
    else
    {
        _activated = false;
        currentAnimIs(_knownAnims["Deactivated"]);
        rootBody()->collisionIs(false);
    }
}

void ZPlatform::defineFixtures(float width, float height)
{
    // Main fixture
    b2PolygonShape fixShape;
    fixShape.SetAsBox(width, height);
    _fixtureDef.shape = &fixShape;
    _fixtureDef.density = 1.0f;
    _fixtureDef.friction = 3.0f;

    jl::FixtureUserData::Ptr fixUD = new jl::FixtureUserData("Root");
    rootBody()->fixtureNew(fixUD, &_fixtureDef);
}

void ZPlatform::defineAnimations()
{
    _knownAnims["Activated"]      = new jl::Animation("Activated", sf::IntRect(0, 0, 150, 32), 1, false);
    _knownAnims["Deactivated"]    = new jl::Animation("Deactivated", sf::IntRect(0, 32, 150, 32), 1, false);

    if(_activated)
        currentAnimIs(_knownAnims["Activated"]);
    else
        currentAnimIs(_knownAnims["Deactivated"]);
}

void ZPlatform::handleInput(std::string input)
{
    if(input.compare("SwitchActivation") == 0)
        switchActivation();
}

void ZPlatform::switchActivation()
{
    _activated = !_activated;

    level()->soundMan()->playSound("platform_switch");

    if(_activated)
    {
        currentAnimIs(_knownAnims["Activated"]);
        rootBody()->collisionIs(true);
    }
    else
    {
        if(_touchingZorbit) static_cast<Zorbit*>(level()->entityMan()->entity("Zorbit").ptr())->footContactsMinusMinus();
        currentAnimIs(_knownAnims["Deactivated"]);
        rootBody()->collisionIs(false);
    }
}

void ZPlatform::activationIs(bool input)
{
    if(_activated == input)
        return;

    _activated = input;

    level()->soundMan()->playSound("platform_switch");

    if(_activated)
    {
        currentAnimIs(_knownAnims["Activated"]);
        rootBody()->collisionIs(true);
    }
    else
    {
        if(_touchingZorbit) static_cast<Zorbit*>(level()->entityMan()->entity("Zorbit").ptr())->footContactsMinusMinus();
        currentAnimIs(_knownAnims["Deactivated"]);
        rootBody()->collisionIs(false);
    }
}

void ZPlatform::readySet()
{
    Prop::readySet();
}

void ZPlatform::reset()
{
    if(_initialActivation.compare("True") == 0)
    {
        _activated = true;
        currentAnimIs(_knownAnims["Activated"]);
        rootBody()->collisionIs(true);
    }
    else
    {
        _activated = false;
        currentAnimIs(_knownAnims["Deactivated"]);
        rootBody()->collisionIs(false);
    }
}

void ZPlatform::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    Prop::beginContact(contact, thisFixUD, thatCollidable, thatFixUD);

    if(thatCollidable->name().compare("Zorbit") == 0
       && thatFixUD->name().compare("Bottom") == 0)
        _touchingZorbit = true;
}

void ZPlatform::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    Prop::endContact(contact, thisFixUD, thatCollidable, thatFixUD);

    if(thatCollidable->name().compare("Zorbit") == 0
        && thatFixUD->name().compare("Bottom") == 0)
        _touchingZorbit = false;
}

