

// Copyright(c) 2017 Happy Rock Studios

#include "ZRectShape.h"
#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "JollyLava/Tool.h"
#include "JollyLava/Prop.h"

ZRectShape::ZRectShape(std::string name,
                                 ZorbitLevel * level,
                                 std::vector<string> layerNames,
                                  b2Vec2 initPos,
                                  int initAngle,
                                  float width,
                                  float height,
                                  std::string filepath) : jl::Actor(name,
                                                            level,
                                                            layerNames,
                                                            "RectShape")
{
    _initRect = sf::FloatRect(initPos.x, initPos.y, width, height);

    // Body texture
    jl::TextureDesc::Ptr bodyTex;
    bodyTex = this->level()->game()->textureMan()->textureDescIs(filepath);

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("ZRectShapeBody", "foreground", bodyTex, &bodyDef, initPos, initAngle, b2_dynamicBody);

    float widthRatio;
    float heightRatio;

    if(width > height) // Rectangle is not the orientation that's useful to us
    {
        rootBody()->setSpriteAdjustment(-90);
        widthRatio =  jl::Tool::meters2pixels(width) * 2 / bodyTex->rect().height;
        heightRatio = jl::Tool::meters2pixels(height) * 2 / bodyTex->rect().width;
    }
    else
    {
        // adjust sprite
        widthRatio =  jl::Tool::meters2pixels(width) * 2 / bodyTex->rect().width;
        heightRatio = jl::Tool::meters2pixels(height) * 2 / bodyTex->rect().height;
    }

    body("ZRectShapeBody")->sprite()->scale(widthRatio, heightRatio);

    defineFixtures(_initRect.width, _initRect.height);
    defineAnimations();
    defineActions();
}

void ZRectShape::reset()
{
    Actor::reset();
}

void ZRectShape::defineFixtures(float width, float height)
{
    // Main fixture
    b2PolygonShape fixShape;
    fixShape.SetAsBox(width, height, b2Vec2(0,0), 0);
    b2FixtureDef def;
    def.shape = &fixShape;
    def.density = 0.8f;
    def.friction = 3.0f;

    jl::FixtureUserData::Ptr fixUD = new jl::FixtureUserData("Shape");
    rootBody()->fixtureNew(fixUD, &def);
}

void ZRectShape::defineAnimations()
{
    _knownAnims["Stop"]         = new jl::Animation("Stop", sf::IntRect(0, 0, rootBody()->sprite()->getTextureRect().width,
                                                                              rootBody()->sprite()->getTextureRect().height), 1, false);
    currentAnimIs(_knownAnims["Stop"]);
}

void ZRectShape::defineActions()
{
    _knownActions["Stop"]           = new jl::aStopAction(1, this);
}

void ZRectShape::readySet()
{
    Actor::readySet();
}

void ZRectShape::update()
{
    if (gravityField())
    {
        // Find the polar angle coord. to this gravity field's center.
        updatePolarAngle();

        // Apply gravitational attraction (Actor's bodies could be dynamic).
        rootBody()->applyPolarForceToCenter(gravitationalAttraction(gravityField()), 0);

        // Update the polar velocity of the actor, using root body.
        b2Vec2 vel = rootBody()->b2body()->GetLinearVelocity();
        setPolarVelocity(b2Vec2(
                vel.x * sin(polarAngle()) - vel.y * cos(polarAngle()),
                vel.x * cos(polarAngle()) + vel.y * sin(polarAngle())));
    }

    // Take care of entity-related updates.
    Entity::update();

    // Handle whatever actions this Actor should be executing.
    handleActions();

    // Update the current action name
    if(_actionMan->peekAction())
        _currentAction = actionMan()->peekAction()->name();
    else _currentAction = "NoAction";
}

void ZRectShape::updateConcrete()
{
    if(noActions())
        _actionMan->pushAction(_knownActions["Stop"]);
}

void ZRectShape::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    Actor::beginContact(contact, thisFixUD, thatCollidable, thatFixUD);
}

void ZRectShape::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
}

float ZRectShape::cameraAngle()
{
    float uprightAngle = polarAngle() - 0.5 * jl::Tool::PI();
    return - jl::Tool::sfAngle(uprightAngle);
}


