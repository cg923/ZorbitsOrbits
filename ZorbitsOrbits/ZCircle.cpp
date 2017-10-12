// Copyright(c) 2017 Happy Rock Studios

#include "ZCircle.h"
#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "JollyLava/Tool.h"
#include "JollyLava/Prop.h" // TODO Why do I need to include this?
#include "JollyLava/Planet.h"
#include "ZGrapple.h"
#include "ZBooster.h"

ZCircle::ZCircle(std::string name,
                 ZorbitLevel * level,
                 std::vector<string> layerNames,
                 std::string texturePath,
                 b2Vec2 initPos,
                 int initAngle,
                 float radius,
                 std::string fixed,
                 bool resizeSprite) : jl::Actor(name,
                                                level,
                                                layerNames,
                                                "Circle")
{
    // Body texture
    jl::TextureDesc::Ptr bodyTex;
    bodyTex = this->level()->game()->textureMan()->textureDescIs(texturePath);

    // Create root body.
    b2BodyDef bodyDef;
    //bodyDef.fixedRotation = true;
    bodyNew("ZCircleBody", "foreground", bodyTex, &bodyDef, initPos, initAngle, b2_dynamicBody);

    // Create axle body.
    jl::TextureDesc::Ptr blankTex;
    blankTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/blank.png");
    bodyNew("AxleBody", "foreground", blankTex, &bodyDef, initPos, initAngle, b2_staticBody);

    _radius = radius;

    if(fixed.compare("true") == 0)
        _fixed = true;
    else _fixed = false;

    defineFixtures(0, 0); // In this case defineFixtures will deal with radius.
    defineAnimations();

    // Scale sprite
    if(resizeSprite)
    {
        float sizeRatio = jl::Tool::meters2pixels(_radius) * 2 / std::max(bodyTex->rect().width, bodyTex->rect().height);
        body("ZCircleBody")->sprite()->scale(sizeRatio, sizeRatio);
    }

    _lastAngle      = initAngle;
    _spinning       = false;
    _sensor         = false;
    _fade           = false;
    _initSpinSpeed  = 0;
}

void ZCircle::defineFixtures(float width, float height)
{
    // Main fixture
    b2FixtureDef fixDef;
    b2CircleShape circleShape;
    circleShape.m_radius = _radius;
    fixDef.shape = &circleShape;
    fixDef.density = 1.0f;
    fixDef.friction = 0.25f;
    if(!_fixed) fixDef.isSensor = true;
    else fixDef.isSensor = false;
    jl::FixtureUserData::Ptr fixUD = new jl::FixtureUserData("Root");
    rootBody()->fixtureNew(fixUD, &fixDef);
}

void ZCircle::setMass(float mass)
{
    b2MassData * massData = new b2MassData();
    massData->mass = mass;
    rootBody()->b2body()->SetMassData(massData);
}

void ZCircle::setDensity(float density)
{
    for (b2Fixture* f = rootBody()->b2body()->GetFixtureList(); f; f = f->GetNext())
    {
        f->SetDensity(density);
    }
    rootBody()->b2body()->ResetMassData();
}

void ZCircle::setSensor(bool sensor)
{
    for (b2Fixture* f = rootBody()->b2body()->GetFixtureList(); f; f = f->GetNext())
    {
        f->SetSensor(sensor);
    }
    _sensor = sensor;
    //rootBody()->b2body()->ResetMassData();
}

void ZCircle::defineAnimations()
{
    float spriteWidth = rootBody()->sprite()->getTextureRect().width;
    float spriteHeight = rootBody()->sprite()->getTextureRect().height;

    _knownAnims["Stop"]  = new jl::Animation("Stop", sf::IntRect(0, 0, spriteWidth, spriteHeight), 1, false);

    currentAnimIs(_knownAnims["Stop"]);
}

void ZCircle::defineActions()
{
    _knownActions["Stop"]        = new jl::aStopAction(1, this);
}

void ZCircle::readySet()
{
    Entity::readySet();

    // Create joints to pin it in space and rotate it.
    if(!_fixed && gravityField())
    {
        b2DistanceJointDef jointDef;
        jointDef.bodyA = rootBody()->b2body();
        jointDef.bodyB = gravityField()->b2body();
        jointDef.collideConnected = false;
        jointDef.length = jl::Tool::distance(this->position(), gravityField()->position());
        _distanceJoint = (b2DistanceJoint *)this->level()->b2world()->CreateJoint(&jointDef);

        b2RevoluteJointDef jd;
        jd.bodyA = body("AxleBody")->b2body();
        jd.bodyB = rootBody()->b2body();
        jd.localAnchorA.Set(0.0f, 0.0f);
        jd.localAnchorB.Set(0.0f, 0.0f);
        jd.lowerAngle = 0;
        jd.upperAngle = 0;
        jd.referenceAngle = 0;
        //jd.enableLimit = true;
        jd.motorSpeed = _initSpinSpeed;
        jd.maxMotorTorque = rootBody()->b2body()->GetMass() * 50000;
        jd.enableMotor = true;
        _revoluteJoint = (b2RevoluteJoint *)this->level()->b2world()->CreateJoint(&jd);

        rootBody()->b2body()->SetAngularVelocity(0);
        body("AxleBody")->b2body()->SetAngularVelocity(0);
    }

    if(_fixed && gravityField())
        orientTowardGravityFieldCenter();
}

void ZCircle::reset()
{
    Entity::reset();

    rootBody()->b2body()->SetTransform(initPos(), initAngle());

    rootBody()->b2body()->SetLinearVelocity(b2Vec2(0,0));
    rootBody()->b2body()->SetAngularVelocity(0);

    if(level()->name().compare("jungle") == 0
       && (name().compare("ZCircleShape1") == 0
           || name().compare("ZCircleShape2") == 0
           || name().compare("ZCircleShape3") == 0))
        _revoluteJoint->EnableLimit(true);

    // Angle toward it.
    updatePolarAngle();
}

void ZCircle::makeConnections()
{
    std::map<std::string, Collidable::Ptr>::iterator itr;

    for(itr = _pendingConnections.begin(); itr != _pendingConnections.end(); itr++)
    {
        b2RevoluteJointDef jd;
        if( itr->second.ptr()->type().compare("Planet") == 0)
        {
            jd.Initialize(rootBody()->b2body(),
                      static_cast<jl::Planet*>(itr->second.ptr())->b2body(),
                      itr->second->position());
        }
        else
        {
            jd.Initialize(rootBody()->b2body(),
                      static_cast<Entity*>(itr->second.ptr())->rootBody()->b2body(),
                      itr->second->position());
            if( itr->second.ptr()->type().compare("Grapple") == 0)
                static_cast<ZGrapple*>(itr->second.ptr())->giveZCircle(this);
        }

        jd.collideConnected = false;
        jd.lowerAngle = 0;
        jd.upperAngle = 0;
        if( itr->second.ptr()->type().compare("Planet") == 0)
            jd.enableLimit = false;
        else jd.enableLimit = true;
        _joints[itr->second->name()] = (b2RevoluteJoint *)this->level()->b2world()->CreateJoint(&jd);
        _connections[itr->second->name()] = itr->second;

        static_cast<jl::Prop*>(itr->second.ptr())->anchoredIs(true);
    }

    _pendingConnections.clear();
}

void ZCircle::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    Entity::beginContact(contact, thisFixUD, thatCollidable, thatFixUD);

    // Get the type of the other collidable for comparisons.
    string thatType = thatCollidable->type();

    if(thatType.compare("Grapple") == 0 || thatType.compare("Boinger") == 0)
    {
        if(static_cast<jl::Prop*>(thatCollidable.ptr())->dynamic())
        {
            if(_pendingConnections.count(thatCollidable->name()) == 0)
                _pendingConnections[thatCollidable->name()] = thatCollidable;
        }
    }

    if(thatType.compare("Planet") == 0)
    {
        if(static_cast<jl::Planet*>(thatCollidable.ptr())->dynamic())
        {
            if(_pendingConnections.count(thatCollidable->name()) == 0)
                _pendingConnections[thatCollidable->name()] = thatCollidable;
        }
    }
    if(thatType.compare("Booster") == 0)
    {
        float smallerX = static_cast<ZBooster *>(thatCollidable.ptr())->boostStrength();
        float smallerY = static_cast<ZBooster *>(thatCollidable.ptr())->boostStrength();

        float desiredX = std::max(smallerX, 0.f);
        float desiredY = std::max(smallerY, 0.f);

        if(rootBody()->b2body()->GetLinearVelocity().x < 0)
            desiredX *= -1;
        if(rootBody()->b2body()->GetLinearVelocity().y < 0)
            desiredY *= -1;

        rootBody()->b2body()->ApplyLinearImpulse(b2Vec2(desiredX, desiredY),
                                                 rootBody()->b2body()->GetWorldCenter());
    }
}

void ZCircle::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
}

void ZCircle::switchSpinDirection()
{
    _revoluteJoint->SetMotorSpeed(-1 * _revoluteJoint->GetMotorSpeed());
}

void ZCircle::changeSpinSpeed(float factor)
{
    if(factor == 0)
    {
        _initSpinSpeed = _revoluteJoint->GetMotorSpeed();
        _revoluteJoint->EnableMotor(false);
        _revoluteJoint->EnableLimit(true);
        _spinning = false;
    }
    else if (factor == -1)
    {
        _revoluteJoint->EnableMotor(true);
        _revoluteJoint->EnableLimit(false);
        _spinning = true;
        _revoluteJoint->SetMotorSpeed(_initSpinSpeed);
    }
    else
    {
        _revoluteJoint->EnableMotor(true);
        _revoluteJoint->EnableLimit(false);
        _spinning = true;
        _revoluteJoint->SetMotorSpeed(factor * std::min(rootBody()->b2body()->GetMass(), 0.5f));
    }
}

void ZCircle::fade()
{
    _fade = true;
}

void ZCircle::disableLimits()
{
    _revoluteJoint->EnableLimit(false);
}

void ZCircle::defineLimits(float lower, float upper)
{
    _revoluteJoint->SetLimits(lower, upper);
    _revoluteJoint->EnableLimit(true);
}

void ZCircle::update()
{
    Entity::update();

    if(_fixed)
    {
         if (gravityField())
        {
            // Find the polar angle coord. to this gravity field's center.
            updatePolarAngle();

            // Apply gravitational attraction (Actor's bodies could be dynamic).
            rootBody()->applyPolarForceToCenter(gravitationalAttraction(gravityField()), 0);

            // Update the polar velocity of the actor, using root body.
            b2Vec2 vel = rootBody()->b2body()->GetLinearVelocity();
            _polarVelocity = b2Vec2(
                vel.x * sin(polarAngle()) - vel.y * cos(polarAngle()),
                vel.x * cos(polarAngle()) + vel.y * sin(polarAngle()));
        }
    }
    if(_fade)
    {
        if(_fadeTimer.getElapsedTime().asSeconds() > 0.08)
        {
            if(rootBody()->sprite()->getColor().a - 2 < 0)
            {
                rootBody()->sprite()->setColor(sf::Color(255,255,255,0));
                _fade = false;
            }
            else
                rootBody()->sprite()->setColor(sf::Color(255,255,255,rootBody()->sprite()->getColor().a - 2));
        }
    }
}

void ZCircle::updateConcrete()
{
    if(_pendingConnections.size() > 0)
        makeConnections();

    _lastAngle = rootBody()->b2body()->GetAngle();
}

float ZCircle::cameraAngle()
{
     return -jl::Tool::sfAngle(jl::Tool::polarAngle(position(), gravityField()->position()) - jl::Tool::PI() / 2);
}

