
// Copyright(c) 2017 Happy Rock Studios

#include "ZHappyRock.h"
#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "JollyLava/Tool.h"

ZHappyRock::ZHappyRock(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle, float strength, bool dynamic) : jl::Prop(name,
            level, layerNames, "HappyRock")
{
    // Body texture
    jl::TextureDesc::Ptr bodyTex;
    bodyTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/happyrock/happyrock.png",
            sf::Vector2f(60, 50), 120, 100);

    _toDie = false;

    // Create root body.
    b2BodyDef bodyDef;
    bodyDef.linearDamping = 0.0f;
    bodyDef.angularDamping = 0.0f;
    bodyNew("ZHappyRockBody", "foreground", bodyTex, &bodyDef, initPos, initAngle);

    float width = 0.5f;
    float height = 0.25f;

    defineFixtures(width, height);
    defineAnimations();
    defineSounds();
}

void ZHappyRock::defineFixtures(float width, float height)
{
    // Main fixture
    b2FixtureDef fixDef;
    b2PolygonShape fixShape;
    fixShape.SetAsBox(width, height);
    fixDef.shape = &fixShape;
    fixDef.isSensor = true;
    fixDef.density = 0.1f;
    fixDef.friction = 0.0f;

    jl::FixtureUserData::Ptr fixUD = new jl::FixtureUserData("Root");
    rootBody()->fixtureNew(fixUD, &fixDef);
}

void ZHappyRock::defineAnimations()
{
    _knownAnims["Stop"]    = new jl::Animation("Stop", sf::IntRect(0, 0,120, 100), 1, false);

    currentAnimIs(_knownAnims["Stop"]);
}

void ZHappyRock::defineSounds()
{
    level()->soundMan()->soundNew("happyrock", "resources/sounds/happyrock.wav", false);
}

void ZHappyRock::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // Get the type of the other collidable for comparisons.
    string thatType = thatCollidable->type();

    if(thatType.compare("Player") == 0 && thatFixUD->name().compare("Root") == 0)
    {
        static_cast<ZorbitLevel *>(this->level())->foundHR();

        if(level()->soundMan()->sound("happyrock"))
            level()->soundMan()->playSound("happyrock");

        toDie();
    }
    if(thatType.compare("GravityField") == 0)
    {
        rootBody()->b2body()->SetTransform(rootBody()->b2body()->GetPosition(), jl::Tool::polarAngle(rootBody()->b2body()->GetPosition(), thatCollidable->position()));
    }
}

void ZHappyRock::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{

}

void ZHappyRock::updateConcrete()
{
    if(_toDie)
        die();
}

