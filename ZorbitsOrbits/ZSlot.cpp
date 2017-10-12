
// Copyright(c) 2017 Happy Rock Studios

#include "ZSlot.h"
#include "ZorbitsOrbits.h"
#include "Zorbit.h"
#include "ZorbitLevel.h"
#include "ZGlob.h"
#include "JollyLava/Tool.h"

ZSlot::ZSlot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle) : jl::Prop(name,
            level, layerNames, "Slot")
{
    // Body texture
    jl::TextureDesc::Ptr bodyTex;
    bodyTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/slot/slot.png",
            sf::Vector2f(150, 50), 300, 100);

    // Individual slot texture
    _iconTextureDesc = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/slot/icon.png",
            sf::Vector2f(50, 50), 100, 100);

    // Slot sprites
    _slot1Sprite.setTexture(*_iconTextureDesc->texture());
    _slot1Sprite.setTextureRect(sf::IntRect(0,0,100,100));
    _slot1Sprite.setOrigin(50,50);

    _slot2Sprite.setTexture(*_iconTextureDesc->texture());
    _slot2Sprite.setTextureRect(sf::IntRect(0,100,100,100));
    _slot2Sprite.setOrigin(50,50);

    _slot3Sprite.setTexture(*_iconTextureDesc->texture());
    _slot3Sprite.setTextureRect(sf::IntRect(0,200,100,100));
    _slot3Sprite.setOrigin(50,50);

    // Blank texture, used for spawn point
    jl::TextureDesc::Ptr spawnTex;
    spawnTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/blank.png",
            sf::Vector2f(1, 1), 1, 1);

    // Create root body.
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyDef.linearDamping = 0.0f;
    bodyDef.angularDamping = 0.0f;
    bodyNew("ZSlotBody", "foreground", bodyTex, &bodyDef, initPos, initAngle);

    // Right Spawn body
    float xDistToSpawn = 7 * cos(jl::Tool::b2Angle(initAngle));
    float yDistToSpawn = 7 * -sin(jl::Tool::b2Angle(initAngle));
    bodyNew("ZSlotRightSpawn", "foreground", spawnTex, &bodyDef, initPos + b2Vec2(xDistToSpawn, yDistToSpawn), 0);

    // Left Spawn body
    xDistToSpawn *= -1;
    yDistToSpawn *= -1;
    bodyNew("ZSlotLeftSpawn", "foreground", spawnTex, &bodyDef, initPos + b2Vec2(xDistToSpawn, yDistToSpawn), 0);

    // Slot1 body
    xDistToSpawn = -1.6 * cos(jl::Tool::b2Angle(initAngle));
    yDistToSpawn = -1.6 * -sin(jl::Tool::b2Angle(initAngle));
    bodyNew("Slot1Body", "foreground", spawnTex, &bodyDef, initPos + b2Vec2(xDistToSpawn, yDistToSpawn), initAngle);

    // Slot2 body
    bodyNew("Slot2Body", "foreground", spawnTex, &bodyDef, initPos, initAngle);

    // Slot3 body
    xDistToSpawn = 1.6 * cos(jl::Tool::b2Angle(initAngle));
    yDistToSpawn = 1.6 * -sin(jl::Tool::b2Angle(initAngle));
    bodyNew("Slot3Body", "foreground", spawnTex, &bodyDef, initPos + b2Vec2(xDistToSpawn, yDistToSpawn), initAngle);


    _slot1Sprite.setRotation(-jl::Tool::sfAngle(angle()));
    _slot1Sprite.setPosition(jl::Tool::sfVec(body("Slot1Body")->b2body()->GetPosition()));

    _slot2Sprite.setRotation(-jl::Tool::sfAngle(angle()));
    _slot2Sprite.setPosition(jl::Tool::sfVec(body("Slot2Body")->b2body()->GetPosition()));

    _slot3Sprite.setRotation(-jl::Tool::sfAngle(angle()));
    _slot3Sprite.setPosition(jl::Tool::sfVec(body("Slot3Body")->b2body()->GetPosition()));

    float width = 2.5f;
    float height = 0.8f;

    defineFixtures(width, height);
    defineAnimations();
    defineSounds();

    _active = false;

    _chronos["spin"] = _spinClock;
}

void ZSlot::defineFixtures(float width, float height)
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

    // Left Spawn fixture
    b2CircleShape sensorShape;
    sensorShape.m_radius = 0.5f;
    b2FixtureDef sensorFixtureDef;
    sensorFixtureDef.shape = &sensorShape;
    sensorFixtureDef.isSensor = true;

    jl::FixtureUserData::Ptr leftSpawnFixUD = new jl::FixtureUserData(name() + "LeftSpawn");
    body("ZSlotLeftSpawn")->fixtureNew(leftSpawnFixUD, &sensorFixtureDef);

    // Right Spawn Fixture
    jl::FixtureUserData::Ptr rightSpawnFixUD = new jl::FixtureUserData(name() + "RightSpawn");
    body("ZSlotRightSpawn")->fixtureNew(rightSpawnFixUD, &sensorFixtureDef);
}

void ZSlot::defineAnimations()
{
    _knownAnims["Stop"]    = new jl::Animation("Stop", sf::IntRect(0, 0, 300, 100), 1, false);
    _knownAnims["Spinning"] = new jl::Animation("Spinning", sf::IntRect(300, 0, 300, 100), 2, true);

    currentAnimIs(_knownAnims["Stop"]);
}

void ZSlot::defineSounds()
{
    level()->soundMan()->soundNew("slot_activate", "resources/sounds/slot_activate.wav", false);
    level()->soundMan()->soundNew("slot_whir", "resources/sounds/slot_whir.wav", false);
}

void ZSlot::reset()
{
    _active = false;
}

void ZSlot::readySet()
{
    Entity::readySet();
}

void ZSlot::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    Prop::beginContact(contact, thisFixUD, thatCollidable, thatFixUD);
}

void ZSlot::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    Prop::endContact(contact, thisFixUD, thatCollidable, thatFixUD);
}

void ZSlot::updateConcrete()
{
    // Spinning
    if(_active && _chronos["spin"].getElapsedTime().asSeconds() < 2)
    {
        currentAnimIs(_knownAnims["Spinning"]);
        level()->soundMan()->playSound("slot_whir");
    }
    // Finished spinning
    if(_active && _chronos["spin"].getElapsedTime().asSeconds() >= 2)
    {
        currentAnimIs(_knownAnims["Stop"]);
        std::string result = generateResult();
        setSprites(result);
        reward(result);
        _active = false;
    }
}

void ZSlot::draw()
{
    jl::Prop::draw();
    if(_chronos["spin"].getElapsedTime().asSeconds() >= 2
       || !_active)
    {
        level()->game()->renderWindow()->draw(_slot1Sprite);
        level()->game()->renderWindow()->draw(_slot2Sprite);
        level()->game()->renderWindow()->draw(_slot3Sprite);
    }
}

void ZSlot::activate()
{
    _active = true;
    _chronos["spin"].reset(true);
    level()->soundMan()->playSound("slot_activate");
}

void ZSlot::setSprites(std::string result)
{
    std::string firstSlot = result.substr(0,1);
    std::string secondSlot = result.substr(1,1);
    std::string thirdSlot = result.substr(2,1);

    // Slot 1
    if(firstSlot.compare("Z") == 0)
        _slot1Sprite.setTextureRect(sf::IntRect(0,100, 100, 100));
    else if(firstSlot.compare("C") == 0)
        _slot1Sprite.setTextureRect(sf::IntRect(0,0, 100, 100));
    else if(firstSlot.compare("G") == 0)
        _slot1Sprite.setTextureRect(sf::IntRect(0,200, 100, 100));

    // Slot 2
    if(secondSlot.compare("Z") == 0)
        _slot2Sprite.setTextureRect(sf::IntRect(0,100, 100, 100));
    else if(secondSlot.compare("C") == 0)
        _slot2Sprite.setTextureRect(sf::IntRect(0,0, 100, 100));
    else if(secondSlot.compare("G") == 0)
        _slot2Sprite.setTextureRect(sf::IntRect(0,200, 100, 100));

    // Slot 3
    if(thirdSlot.compare("Z") == 0)
        _slot3Sprite.setTextureRect(sf::IntRect(0,100, 100, 100));
    else if(thirdSlot.compare("C") == 0)
        _slot3Sprite.setTextureRect(sf::IntRect(0,0, 100, 100));
    else if(thirdSlot.compare("G") == 0)
        _slot3Sprite.setTextureRect(sf::IntRect(0,200, 100, 100));
}

void ZSlot::reward(std::string result)
{
    if(result.compare("ZZZ") == 0) // Extra life
    {
        static_cast<Zorbit*>(level()->entityMan()->entity("Zorbit").ptr())->increaseCogs(100);
    }
    else if(result.compare("CCC") == 0) // 50 Cogs
    {
        static_cast<Zorbit*>(level()->entityMan()->entity("Zorbit").ptr())->increaseCogs(50);
    }
    else if(result.compare("CCZ") == 0) // 25 Cogs
    {
        static_cast<Zorbit*>(level()->entityMan()->entity("Zorbit").ptr())->increaseCogs(25);
    }
    else if(result.compare("CCG") == 0) // 25 Cogs
    {
        static_cast<Zorbit*>(level()->entityMan()->entity("Zorbit").ptr())->increaseCogs(25);
    }
    else if(result.compare("CGC") == 0) // 25 Cogs
    {
        static_cast<Zorbit*>(level()->entityMan()->entity("Zorbit").ptr())->increaseCogs(25);
    }
    else if(result.compare("CZC") == 0) // 25 Cogs
    {
        static_cast<Zorbit*>(level()->entityMan()->entity("Zorbit").ptr())->increaseCogs(25);
    }
    else if(result.compare("GCC") == 0) // 25 Cogs
    {
        static_cast<Zorbit*>(level()->entityMan()->entity("Zorbit").ptr())->increaseCogs(25);
    }
    else if(result.compare("ZCC") == 0) // 25 Cogs
    {
        static_cast<Zorbit*>(level()->entityMan()->entity("Zorbit").ptr())->increaseCogs(25);
    }
    else if(result.compare("GGG") == 0) // 6 Globbies
    {
        for(int iii = 6; iii > 0; iii--)
        {
            if(iii % 2 == 0)
            {
                std::stringstream globName;
                globName << this->name() << "globbie" << iii;
                level()->entityMan()->entityNew(globName.str(),
                                                "Layer1",
                                                new ZGlob(globName.str(),
                                                          static_cast<ZorbitLevel*>(level()),
                                                          _layerNames,
                                                          body("ZSlotLeftSpawn")->b2body()->GetPosition(),
                                                          jl::Tool::sfAngle(body("ZSlotLeftSpawn")->b2body()->GetAngle())));
                // Send the globby flying!
                level()->entityMan()->entity(globName.str())->rootBody()->applyPolarImpulseToCenter(-100,-100);
            }
            else
            {
                std::stringstream globName;
                globName << this->name() << "globbie" << iii;
                level()->entityMan()->entityNew(globName.str(),
                                                "Layer1",
                                                new ZGlob(globName.str(),
                                                          static_cast<ZorbitLevel*>(level()),
                                                          _layerNames,
                                                          body("ZSlotRightSpawn")->b2body()->GetPosition(),
                                                          jl::Tool::sfAngle(body("ZSlotRightSpawn")->b2body()->GetAngle())));
                // Send the globby flying!
                level()->entityMan()->entity(globName.str())->rootBody()->applyPolarImpulseToCenter(100,100);
            }
        }
    }
    else return;
}

std::string ZSlot::generateResult()
{
    srand (time (NULL) );
    int firstSlot = rand() % 3 + 1;
    int secondSlot = rand() % 3 + 1;
    int thirdSlot = rand() % 3 + 1;

    std::stringstream result;
    result << intToSlotResult(firstSlot) << intToSlotResult(secondSlot) << intToSlotResult(thirdSlot);

    return result.str();
}

std::string ZSlot::intToSlotResult(int i)
{
    switch(i)
    {
    case 1:
        return "Z";
        break;
    case 2:
        return "C";
        break;
    case 3:
        return "G";
        break;
    default:
        break;
    }
}
