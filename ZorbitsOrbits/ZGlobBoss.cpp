

// Copyright(c) 2017 Happy Rock Studios

#include "ZGlobBoss.h"
#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "Zorbit.h"
#include "JollyLava/Tool.h"
#include "ZShot.h"

ZGlobBoss::ZGlobBoss(ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle) : ZBoss("GlobBoss",
            level, layerNames)
{
    _prefix = "gboss";

    // Head texture
    jl::TextureDesc::Ptr headTex;
    headTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/globboss/head.png",
            sf::Vector2f(250, 250), 500, 500);
    this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/globboss/death.png",
            sf::Vector2f(512, 762), 1024, 1024);

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("HeadBody", "foreground", headTex, &bodyDef, initPos, initAngle);

    // Tentacle texture
    jl::TextureDesc::Ptr tentTex;
    tentTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/globboss/tentacle.png",
            sf::Vector2f(250, 250), 500, 500);

    // Create Tentacle body.
    bodyNew("TentacleBody1", "foreground", tentTex, &bodyDef, initPos + b2Vec2(5,0), initAngle);
    bodyNew("TentacleBody2", "foreground", tentTex, &bodyDef, initPos - b2Vec2(5,0), initAngle);

    // Mouth texture
    jl::TextureDesc::Ptr mouthTex;
    mouthTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/globboss/mouth.png",
            sf::Vector2f(512, 512), 4096, 4096);

    _mouthSprite.setTexture(*mouthTex->texture());
    _mouthSprite.setOrigin(0,0);
    _mouthSprite.setTextureRect(sf::IntRect(0, 0, 512, 512));
    _mouthSprite.setPosition(sf::Vector2f(3800,4150));
    _mouthSprite.scale(1.8,1.8);

    float width = 3.5f;
    float height = 3.0f;

    // Set values
    _active         = false;
    _maxHealth      = 170;   //200
    _currentHealth  = _maxHealth;
    _damage         = 1;
    _numShots       = 1;
    _maxSpeed       = 20;
    _jumpHeight     = 0;
    _lFootContacts  = 0;
    _rFootContacts  = 0;
    _headPos        = 1;
    _attacking      = false;
    _seesZorbit     = false;
    _headVisible    = true;
    _tentVisible1    = false;
    _tentVisible2    = false;
    _tentOverlap1    = false;
    _tentOverlap2    = false;
    _tentRight1      = true;
    _tentRight2      = true;
    directedRightIs(false);

    _positions["pos1"] = jl::Tool::b2Vec(sf::Vector2f(4225,3679));
    _positions["pos2"] = jl::Tool::b2Vec(sf::Vector2f(5115,4631));
    _positions["pos3"] = jl::Tool::b2Vec(sf::Vector2f(4225,5481));
    _positions["pos4"] = jl::Tool::b2Vec(sf::Vector2f(3227,4631));

    defineFixtures(width, height);
    defineAnimations();
    defineSounds();
    defineActions();

    _mouthTimer.resume();
}

void ZGlobBoss::defineFixtures(float width, float height)
{
    // Head fixture
    b2FixtureDef fixDef;
    b2PolygonShape fixShape;
    fixShape.SetAsBox(width, height);
    fixDef.shape = &fixShape;
    fixDef.density = 10.0f;
    fixDef.friction = 5.0f;

    jl::FixtureUserData::Ptr fixUD = new jl::FixtureUserData("Root");
    rootBody()->fixtureNew(fixUD, &fixDef);

    // Tentacle fixture
    fixShape.SetAsBox(width, height);
    fixDef.shape = &fixShape;
    fixDef.density = 10.0f;
    fixDef.friction = 5.0f;

    jl::FixtureUserData::Ptr fixUD2 = new jl::FixtureUserData("Tentacle1");
    body("TentacleBody1")->fixtureNew(fixUD2, &fixDef);

    // Tentacle sensor right
    fixShape.SetAsBox(width, height*1.5, b2Vec2(width*1.5, -2.5), 0);
    fixDef.shape = &fixShape;
    fixDef.isSensor = true;

    jl::FixtureUserData::Ptr fixUD3 = new jl::FixtureUserData("TentacleSensorRight1");
    body("TentacleBody1")->fixtureNew(fixUD3, &fixDef);

    // Tentacle sensor right
    fixShape.SetAsBox(width, height*1.5, b2Vec2(-width*1.5, -2.5), 0);
    fixDef.shape = &fixShape;
    fixDef.isSensor = true;

    jl::FixtureUserData::Ptr fixUD4 = new jl::FixtureUserData("TentacleSensorLeft1");
    body("TentacleBody1")->fixtureNew(fixUD4, &fixDef);

    // Second tentacle
    fixShape.SetAsBox(width, height);
    fixDef.shape = &fixShape;
    fixDef.density = 10.0f;
    fixDef.friction = 5.0f;
    jl::FixtureUserData::Ptr fixUD5 = new jl::FixtureUserData("Tentacle2");
    body("TentacleBody2")->fixtureNew(fixUD5, &fixDef);

    // Tentacle sensor right
    fixShape.SetAsBox(width, height*1.5, b2Vec2(width*1.5, -2.5), 0);
    fixDef.shape = &fixShape;
    fixDef.isSensor = true;

    jl::FixtureUserData::Ptr fixUD6 = new jl::FixtureUserData("TentacleSensorRight2");
    body("TentacleBody2")->fixtureNew(fixUD6, &fixDef);

    // Tentacle sensor right
    fixShape.SetAsBox(width, height*1.5, b2Vec2(-width*1.5, -2.5), 0);
    fixDef.shape = &fixShape;
    fixDef.isSensor = true;

    jl::FixtureUserData::Ptr fixUD7 = new jl::FixtureUserData("TentacleSensorLeft2");
    body("TentacleBody2")->fixtureNew(fixUD7, &fixDef);
}

void ZGlobBoss::readySet()
{
    Actor::readySet();

    if(gravityField())
    {
        b2DistanceJointDef jointDef;
        jointDef.bodyA = body("TentacleBody1")->b2body();
        jointDef.bodyB = gravityField()->b2body();
        jointDef.collideConnected = true;
        jointDef.length = jl::Tool::distance(body("TentacleBody1")->b2body()->GetPosition(), gravityField()->position()) + 1.4;
        _tentacleJoint1 = (b2DistanceJoint *)this->level()->b2world()->CreateJoint(&jointDef);

        jointDef.bodyA = body("TentacleBody2")->b2body();
        jointDef.bodyB = gravityField()->b2body();
        jointDef.collideConnected = true;
        jointDef.length = jl::Tool::distance(body("TentacleBody2")->b2body()->GetPosition(), gravityField()->position()) + 1.4;
        _tentacleJoint2 = (b2DistanceJoint *)this->level()->b2world()->CreateJoint(&jointDef);

        b2DistanceJointDef jointDef2;
        jointDef2.bodyA = body("HeadBody")->b2body();
        jointDef2.bodyB = gravityField()->b2body();
        jointDef2.collideConnected = true;
        jointDef2.length = jl::Tool::distance(body("HeadBody")->b2body()->GetPosition(), gravityField()->position()) + 2.0;
        _headJoint = (b2DistanceJoint *)this->level()->b2world()->CreateJoint(&jointDef2);
    }
}

void ZGlobBoss::defineAnimations()
{
    _knownAnims["Stop"]             = new jl::Animation("Stop", sf::IntRect(0, 521, 500, 520), 8, false);
    _knownAnims["Move"]             = new jl::Animation("Move", sf::IntRect(0, 521, 500, 520), 1, false);
    _knownAnims["Appear"]           = new jl::Animation("Appear", sf::IntRect(0, 0, 500, 520), 8, false);
    _knownAnims["Disappear"]        = new jl::Animation("Disappear", sf::IntRect(0, 1041, 500, 519), 8, false);
    _knownAnims["Attack"]           = new jl::Animation("Attack", sf::IntRect(0, 1561, 500, 520), 8, false);
    _knownAnims["TentacleAttack"]   = new jl::Animation("TentacleAttack", sf::IntRect(0, 1561, 1023, 520), 4, false);
    _knownAnims["Die"]              = new jl::Animation("Die", sf::IntRect(0, 0, 1024, 1024), 9, false, 60);

    currentAnimIs(_knownAnims["Stop"]);
}

void ZGlobBoss::defineSounds()
{
    level()->soundMan()->soundNew("gboss_hit", "resources/sounds/tboss_hit.wav", false);
    level()->soundMan()->soundNew("gboss_die", "resources/sounds/gboss_die.wav", false);
    level()->soundMan()->soundNew("gboss_roar", "resources/sounds/tboss_roar.wav", false);
    level()->soundMan()->soundNew("gboss_intro", "resources/sounds/tboss_intro.wav", false);
    level()->soundMan()->soundNew("gboss_tentacle", "resources/sounds/gboss_tentacle.wav", false);
}

void ZGlobBoss::defineActions()
{
    _knownActions["DisappearHead"]              = new gbDisappearAction(this, "HeadBody");
    _knownActions["DisappearTentacle1"]          = new gbDisappearAction(this, "TentacleBody1");
    _knownActions["DisappearTentacle2"]          = new gbDisappearAction(this, "TentacleBody2");
    _knownActions["AppearHead"]                 = new gbAppearAction(this, "HeadBody");
    _knownActions["AppearTentacle1"]             = new gbAppearAction(this, "TentacleBody1");
    _knownActions["AppearTentacle2"]             = new gbAppearAction(this, "TentacleBody2");

    _knownActions["MoveTentacle1"]               = new gbMoveAction(this, "TentacleBody1");
    _knownActions["MoveTentacle2"]               = new gbMoveAction(this, "TentacleBody2");
    _knownActions["MoveHead"]                   = new gbMoveAction(this, "HeadBody");

    _knownActions["AttackTentacle1"]             = new gbAttackAction(this, "TentacleAttack", "TentacleBody1", "All");
    _knownActions["AttackTentacle2"]             = new gbAttackAction(this, "TentacleAttack", "TentacleBody2", "All");
    _knownActions["AttackHeadLeft"]             = new gbAttackAction(this, "Attack", "HeadBody", "Left");
    _knownActions["AttackHeadRight"]            = new gbAttackAction(this, "Attack", "HeadBody", "Right");

    _knownActions["StopLong"]                   = new jl::aStopAction(50, this);
    _knownActions["StopShort"]                  = new jl::aStopAction(20, this);
    _knownActions["Stop"]                       = _knownActions["StopShort"];

    _knownActions["Die"]                        = new gbDieAction("tb_die", this);
}

void ZGlobBoss::destroyJoints()
{
    level()->b2world()->DestroyJoint( _headJoint );
    level()->b2world()->DestroyJoint( _tentacleJoint1 );
    level()->b2world()->DestroyJoint( _tentacleJoint2 );
}

void ZGlobBoss::reset()
{
    ZBoss::reset();

    rootBody()->changeTexture(level()->game()->textureMan()->textureDesc("resources/zorbitsorbits/globboss/head.png"));
    body("HeadBody")->b2body()->SetTransform(_positions["pos1"], 0);

    for (b2Fixture* f = body("HeadBody")->b2body()->GetFixtureList(); f; f = f->GetNext())
    {
        //get the existing filter
        b2Filter filter = f->GetFilterData();

        //sensor
        if(static_cast<jl::FixtureUserData*>(f->GetUserData())->name().compare("TentacleSensor") != 0)
            f->SetSensor(false);

        //and set it back
        f->SetFilterData(filter);
    }

    _headVisible = true;

    for (b2Fixture* f = body("TentacleBody1")->b2body()->GetFixtureList(); f; f = f->GetNext())
    {
        //get the existing filter
        b2Filter filter = f->GetFilterData();

        //sensor
        if(static_cast<jl::FixtureUserData*>(f->GetUserData())->name().compare("TentacleSensor") != 0)
            f->SetSensor(true);

        //and set it back
        f->SetFilterData(filter);
    }
    for (b2Fixture* f = body("TentacleBody2")->b2body()->GetFixtureList(); f; f = f->GetNext())
    {
        //get the existing filter
        b2Filter filter = f->GetFilterData();

        //sensor
        if(static_cast<jl::FixtureUserData*>(f->GetUserData())->name().compare("TentacleSensor") != 0)
            f->SetSensor(true);

        //and set it back
        f->SetFilterData(filter);
    }

    _tentVisible1 = false;
    _tentVisible2 = false;
}

void ZGlobBoss::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    ZBoss::beginContact(contact, thisFixUD, thatCollidable, thatFixUD);

    // Get the type of the other collidable for comparisons.
    string thatType = thatCollidable->type();
    if(thatType.compare("ZShot") == 0
       && thisFixUD->name().compare("Root") == 0
       && _headVisible)
    {
        static_cast<ZShot *>(thatCollidable.ptr())->toDie();
        decreaseHealth(static_cast<ZShot *>(thatCollidable.ptr())->damage());
        hit();

        if(level()->soundMan()->sound("gboss_hit"))
            level()->soundMan()->playSound("gboss_hit", true);
    }
    if(thatType.compare("ZShot") == 0
       && thisFixUD->name().compare("Tentacle1") == 0
       && _tentVisible1)
    {
        static_cast<ZShot *>(thatCollidable.ptr())->toDie();
    }
    if(thatType.compare("ZShot") == 0
       && thisFixUD->name().compare("Tentacle2") == 0
       && _tentVisible2)
    {
        static_cast<ZShot *>(thatCollidable.ptr())->toDie();
    }

    if(thatType.compare("Player") == 0
       && thatFixUD->name().compare("Root") == 0
       && (thisFixUD->name().compare("TentacleSensorRight1") == 0
           || thisFixUD->name().compare("TentacleSensorLeft1") == 0))
    {
        _tentOverlap1 = true;
    }
    if(thatType.compare("Player") == 0
       && thatFixUD->name().compare("Root") == 0
       && (thisFixUD->name().compare("TentacleSensorRight2") == 0
           || thisFixUD->name().compare("TentacleSensorLeft2") == 0))
    {
        _tentOverlap2 = true;
    }

    if(thatType.compare("Player") == 0
       && thatFixUD->name().compare("Root") == 0
       && thisFixUD->name().compare("TentacleSensorRight1") == 0)
    {
        if(!_tentRight1)
        {
            flipTentacleSprite(1);
            _tentRight1 = true;
        }
    }
    if(thatType.compare("Player") == 0
       && thatFixUD->name().compare("Root") == 0
       && thisFixUD->name().compare("TentacleSensorRight2") == 0)
    {
        if(!_tentRight2)
        {
            flipTentacleSprite(2);
            _tentRight2 = true;
        }
    }
    if(thatType.compare("Player") == 0
       && thatFixUD->name().compare("Root") == 0
       && thisFixUD->name().compare("TentacleSensorLeft1") == 0)
    {
        if(_tentRight1)
        {
            flipTentacleSprite(1);
            _tentRight1 = false;
        }
    }
    if(thatType.compare("Player") == 0
       && thatFixUD->name().compare("Root") == 0
       && thisFixUD->name().compare("TentacleSensorLeft2") == 0)
    {
        if(_tentRight2)
        {
            flipTentacleSprite(2);
            _tentRight2 = false;
        }
    }
    if(thatType.compare("Player") == 0
       && thatFixUD->name().compare("Root") == 0
       && thisFixUD->name().compare("Root") == 0
       && _headVisible)
    {
        static_cast<Zorbit *>(level()->entityMan()->entity("Zorbit").ptr())->hit(_damage);
    }
}

void ZGlobBoss::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    ZBoss::endContact(contact, thisFixUD, thatCollidable, thatFixUD);

    string thatType = thatCollidable->type();
    if(thatType.compare("Player") == 0
       && thatFixUD->name().compare("Root") == 0
       && (thisFixUD->name().compare("TentacleSensorRight1") == 0
           || thisFixUD->name().compare("TentacleSensorLeft1") == 0))
    {
        _tentOverlap1 = false;
    }
    if(thatType.compare("Player") == 0
       && thatFixUD->name().compare("Root") == 0
       && (thisFixUD->name().compare("TentacleSensorRight2") == 0
           || thisFixUD->name().compare("TentacleSensorLeft2") == 0))
    {
        _tentOverlap2 = false;
    }
}

void ZGlobBoss::updateConcrete()
{
    if(_currentHealth > 1)
    {
        float interval = 0.06;
        if(_mouthTimer.getElapsedTime().asSeconds() < 2)
        {
            _mouthSprite.setTextureRect(sf::IntRect(0,0,512,512));
        }
        if(_mouthTimer.getElapsedTime().asSeconds() >= 2
           && _mouthTimer.getElapsedTime().asSeconds() < 4)
        {
            _mouthSprite.setTextureRect(sf::IntRect(513,0,512,512));
        }
        if(_mouthTimer.getElapsedTime().asSeconds() >= 4
           && _mouthTimer.getElapsedTime().asSeconds() < 4 + interval*1)
        {
            _mouthSprite.setTextureRect(sf::IntRect(1025,0,512,512));
        }
        if(_mouthTimer.getElapsedTime().asSeconds() >= 4 + interval*1
           && _mouthTimer.getElapsedTime().asSeconds() < 4 + interval*2)
        {
            _mouthSprite.setTextureRect(sf::IntRect(1537,0,512,512));
        }
        if(_mouthTimer.getElapsedTime().asSeconds() >= 4 + interval*2
           && _mouthTimer.getElapsedTime().asSeconds() < 4 + interval*3)
        {
            _mouthSprite.setTextureRect(sf::IntRect(0,513,512,512));
        }
        if(_mouthTimer.getElapsedTime().asSeconds() >= 4 + interval*3
           && _mouthTimer.getElapsedTime().asSeconds() < 4 + interval*4)
        {
            _mouthSprite.setTextureRect(sf::IntRect(513,513,512,512));
        }
        if(_mouthTimer.getElapsedTime().asSeconds() >= 4 + interval*4
           && _mouthTimer.getElapsedTime().asSeconds() < 4 + interval*5)
        {
            _mouthSprite.setTextureRect(sf::IntRect(1025,513,512,512));
        }
        if(_mouthTimer.getElapsedTime().asSeconds() >= 4 + interval*5
           && _mouthTimer.getElapsedTime().asSeconds() < 4 + interval*6)
        {
            _mouthSprite.setTextureRect(sf::IntRect(1537,513,512,512));
        }
        if(_mouthTimer.getElapsedTime().asSeconds() >= 4 + interval*6
           && _mouthTimer.getElapsedTime().asSeconds() < 4 + interval*7)
        {
            _mouthSprite.setTextureRect(sf::IntRect(0,1025,512,512));
        }
        if(_mouthTimer.getElapsedTime().asSeconds() >= 4 + interval*7
           && _mouthTimer.getElapsedTime().asSeconds() < 4 + interval*8)
        {
            _mouthSprite.setTextureRect(sf::IntRect(513,1025,512,512));
        }
        if(_mouthTimer.getElapsedTime().asSeconds() >= 4 + interval*8
           && _mouthTimer.getElapsedTime().asSeconds() < 4 + interval*9)
        {
            _mouthSprite.setTextureRect(sf::IntRect(1025,1025,512,512));
        }
        if(_mouthTimer.getElapsedTime().asSeconds() >= 4 + interval*9
           && _mouthTimer.getElapsedTime().asSeconds() < 4 + interval*10)
        {
            _mouthSprite.setTextureRect(sf::IntRect(1537,1025,512,512));
        }
        if(_mouthTimer.getElapsedTime().asSeconds() >= 4 + interval*10
           && _mouthTimer.getElapsedTime().asSeconds() < 4 + interval*11)
        {
            _mouthSprite.setTextureRect(sf::IntRect(0,1537,512,512));
        }
        if(_mouthTimer.getElapsedTime().asSeconds() >= 4 + interval*11
           && _mouthTimer.getElapsedTime().asSeconds() < 4 + interval*12)
        {
            _mouthSprite.setTextureRect(sf::IntRect(513,1537,512,512));
        }
        if(_mouthTimer.getElapsedTime().asSeconds() >= 4 + interval*12
           && _mouthTimer.getElapsedTime().asSeconds() < 4 + interval*13)
        {
            _mouthSprite.setTextureRect(sf::IntRect(1025,1537,512,512));
        }
        if(_mouthTimer.getElapsedTime().asSeconds() >= 4 + interval*13
           && _mouthTimer.getElapsedTime().asSeconds() < 4 + interval*14)
        {
            _mouthSprite.setTextureRect(sf::IntRect(1537,1537,512,512));
        }
        if(_mouthTimer.getElapsedTime().asSeconds() >= 4 + interval*14)
        {
            _mouthTimer.reset(true);
        }
    }
    if(_currentHealth == 1)
    {
        level()->soundMan()->stopMusic();
        _mouthSprite.setTextureRect(sf::IntRect(0,0,512,512));
        level()->entityMan()->killType("GBShot");
    }

    if(static_cast<Zorbit*>(level()->entityMan()->entity("Zorbit").ptr())->currentHealth() <= 0)
    {
        level()->entityMan()->killType("GBShot");
    }

    if(!_tentVisible1) body("TentacleBody1")->sprite()->setTextureRect(sf::IntRect(0,0,500,500));
    if(!_tentVisible2) body("TentacleBody2")->sprite()->setTextureRect(sf::IntRect(0,0,500,500));


    // Keep bodies correctly oriented.
    if(gravityField())
    {
        // Orient
        rootBody()->keepUpright();
        body("TentacleBody1")->keepUpright(-10);
        body("TentacleBody2")->keepUpright(-10);

        // Gravity
        body("TentacleBody1")->applyPolarForceToCenter(gravitationalAttraction(gravityField()), 0);
        body("TentacleBody2")->applyPolarForceToCenter(gravitationalAttraction(gravityField()), 0);
    }

    if(!_active)
    {
        if(noActions())
            _actionMan->pushAction(_knownActions["StopShort"]);
    }
    else
    {
        if(noActions())
        {
            if(_currentHealth > _maxHealth * 0.6)
                phase1();
            else if(_currentHealth <= _maxHealth * 0.6 && _currentHealth > _maxHealth * 0.3)
                phase2();
            else if(_currentHealth < _maxHealth * 0.3)
                phase3();
        }
    }
}

void ZGlobBoss::phase1()
{
    _actionMan->pushAction(_knownActions["DisappearHead"], "Bottom", false);
    _actionMan->pushAction(_knownActions["MoveHead"], "Bottom", false);
    _actionMan->pushAction(_knownActions["MoveTentacle1"], "Bottom", false);
    _actionMan->pushAction(_knownActions["StopShort"], "Bottom", false);
    _actionMan->pushAction(_knownActions["AppearTentacle1"], "Bottom", false);
    _actionMan->pushAction(_knownActions["StopShort"], "Bottom", false);
    _actionMan->pushAction(_knownActions["AttackTentacle1"], "Bottom", false);
    _actionMan->pushAction(_knownActions["DisappearTentacle1"], "Bottom", false);
    _actionMan->pushAction(_knownActions["StopShort"], "Bottom", false);
    _actionMan->pushAction(_knownActions["AppearTentacle2"], "Bottom", false);
    _actionMan->pushAction(_knownActions["StopShort"], "Bottom", false);
    _actionMan->pushAction(_knownActions["AttackTentacle2"], "Bottom", false);
    _actionMan->pushAction(_knownActions["DisappearTentacle2"], "Bottom", false);
    _actionMan->pushAction(_knownActions["StopShort"], "Bottom", false);
    _actionMan->pushAction(_knownActions["AppearHead"], "Bottom", false);
    _actionMan->pushAction(_knownActions["StopLong"], "Bottom", false);
    _actionMan->pushAction(_knownActions["AttackHeadLeft"], "Bottom", false);
    _actionMan->pushAction(_knownActions["AttackHeadRight"], "Bottom", false);
    _actionMan->pushAction(_knownActions["StopShort"], "Bottom", false);

    _actionMan->pushAction(_knownActions["DisappearHead"], "Bottom", false);
    _actionMan->pushAction(_knownActions["MoveHead"], "Bottom", false);
    _actionMan->pushAction(_knownActions["MoveTentacle2"], "Bottom", false);
    _actionMan->pushAction(_knownActions["StopShort"], "Bottom", false);
    _actionMan->pushAction(_knownActions["AppearTentacle1"], "Bottom", false);
    _actionMan->pushAction(_knownActions["StopShort"], "Bottom", false);
    _actionMan->pushAction(_knownActions["AttackTentacle1"], "Bottom", false);
    _actionMan->pushAction(_knownActions["DisappearTentacle1"], "Bottom", false);
    _actionMan->pushAction(_knownActions["StopShort"], "Bottom", false);
    _actionMan->pushAction(_knownActions["AppearTentacle2"], "Bottom", false);
    _actionMan->pushAction(_knownActions["StopShort"], "Bottom", false);
    _actionMan->pushAction(_knownActions["AttackTentacle2"], "Bottom", false);
    _actionMan->pushAction(_knownActions["DisappearTentacle2"], "Bottom", false);
    _actionMan->pushAction(_knownActions["StopShort"], "Bottom", false);
    _actionMan->pushAction(_knownActions["AppearHead"], "Bottom", false);
    _actionMan->pushAction(_knownActions["StopLong"], "Bottom", false);
    _actionMan->pushAction(_knownActions["AttackHeadLeft"], "Bottom", false);
    _actionMan->pushAction(_knownActions["AttackHeadRight"], "Bottom", false);
    _actionMan->pushAction(_knownActions["StopShort"], "Bottom", false);
}

void ZGlobBoss::phase2()
{
    _actionMan->pushAction(_knownActions["DisappearHead"], "Bottom", false);
    _actionMan->pushAction(_knownActions["StopLong"], "Bottom", false);
    _actionMan->pushAction(_knownActions["MoveHead"], "Bottom", false);
    _actionMan->pushAction(_knownActions["AppearHead"], "Bottom", false);
    _actionMan->pushAction(_knownActions["AttackHeadLeft"], "Bottom", false);
    _actionMan->pushAction(_knownActions["DisappearHead"], "Bottom", false);
    _actionMan->pushAction(_knownActions["MoveHead"], "Bottom", false);
    _actionMan->pushAction(_knownActions["AppearHead"], "Bottom", false);
    _actionMan->pushAction(_knownActions["AttackHeadRight"], "Bottom", false);
}

void ZGlobBoss::phase3()
{
    _actionMan->pushAction(_knownActions["AppearHead"], "Bottom", false);
    _actionMan->pushAction(_knownActions["MoveTentacle1"], "Bottom", false);
    _actionMan->pushAction(_knownActions["AppearTentacle1"], "Bottom", false);
    _actionMan->pushAction(_knownActions["StopShort"], "Bottom", false);
    //_actionMan->pushAction(_knownActions["AttackTentacle"], "Bottom", false);
    //_actionMan->pushAction(_knownActions["StopShort"], "Bottom", false);
    _actionMan->pushAction(_knownActions["AttackHeadLeft"], "Bottom", false);
    _actionMan->pushAction(_knownActions["AttackHeadRight"], "Bottom", false);
    _actionMan->pushAction(_knownActions["StopShort"], "Bottom", false);
    _actionMan->pushAction(_knownActions["DisappearHead"], "Bottom", false);
    _actionMan->pushAction(_knownActions["MoveHead"], "Bottom", false);
    _actionMan->pushAction(_knownActions["DisappearTentacle1"], "Bottom", false);
    _actionMan->pushAction(_knownActions["StopShort"], "Bottom", false);
}

void ZGlobBoss::animate()
{
    if(_body.empty() || noActions())
        return;

    if(_actionMan->peekAction()->bodyName().compare("TentacleBody1") == 0)
        body("TentacleBody1")->sprite()->setTextureRect(_currentAnim->update());
    else if(_actionMan->peekAction()->bodyName().compare("TentacleBody2") == 0)
        body("TentacleBody2")->sprite()->setTextureRect(_currentAnim->update());
    else
        body("HeadBody")->sprite()->setTextureRect(_currentAnim->update());
}

void ZGlobBoss::appear(std::string bodyName)
{
    if(bodyName.compare("HeadBody") == 0 && _headVisible)
        return;

    if(bodyName.compare("TentacleBody1") == 0 && _tentVisible1)
        return;

    if(bodyName.compare("TentacleBody2") == 0 && _tentVisible2)
        return;

    for (b2Fixture* f = body(bodyName)->b2body()->GetFixtureList(); f; f = f->GetNext())
    {
        //get the existing filter
        b2Filter filter = f->GetFilterData();

        //sensor
        if(static_cast<jl::FixtureUserData*>(f->GetUserData())->name().compare("TentacleSensorLeft1") != 0
           && static_cast<jl::FixtureUserData*>(f->GetUserData())->name().compare("TentacleSensorRight1") != 0
           && static_cast<jl::FixtureUserData*>(f->GetUserData())->name().compare("TentacleSensorLeft2") != 0
           && static_cast<jl::FixtureUserData*>(f->GetUserData())->name().compare("TentacleSensorRight2") != 0)
            f->SetSensor(false);

        //and set it back
        f->SetFilterData(filter);
    }

    makeVisible(bodyName);
}

void ZGlobBoss::disappear(std::string bodyName)
{
    if(bodyName.compare("HeadBody") == 0 && !_headVisible)
        return;

    if(bodyName.compare("TentacleBody1") == 0 && !_tentVisible1)
        return;

    if(bodyName.compare("TentacleBody2") == 0 && !_tentVisible2)
        return;

    for (b2Fixture* f = body(bodyName)->b2body()->GetFixtureList(); f; f = f->GetNext())
    {
        //get the existing filter
        b2Filter filter = f->GetFilterData();

        //sensor
        if(static_cast<jl::FixtureUserData*>(f->GetUserData())->name().compare("TentacleSensorLeft1") != 0
           && static_cast<jl::FixtureUserData*>(f->GetUserData())->name().compare("TentacleSensorRight1") != 0
           && static_cast<jl::FixtureUserData*>(f->GetUserData())->name().compare("TentacleSensorLeft2") != 0
           && static_cast<jl::FixtureUserData*>(f->GetUserData())->name().compare("TentacleSensorRight2") != 0)
            f->SetSensor(true);

        //and set it back
        f->SetFilterData(filter);
    }
}

void ZGlobBoss::flipTentacleSprite(int which)
{
    if(which == 1)
        body("TentacleBody1")->sprite()->scale(-1.f, 1.f);
    else if(which == 2)
        body("TentacleBody2")->sprite()->scale(-1.f, 1.f);
    else throw fwk::InternalException("Invalid Tentacle Body!");
}

void ZGlobBoss::makeInvisible(std::string bodyName)
{
    if(bodyName.compare("HeadBody") == 0)
        _headVisible = false;

    if(bodyName.compare("TentacleBody1") == 0)
        _tentVisible1 = false;

    if(bodyName.compare("TentacleBody2") == 0)
        _tentVisible2 = false;
}

void ZGlobBoss::makeVisible(std::string bodyName)
{
    if(bodyName.compare("HeadBody") == 0)
        _headVisible = true;

    if(bodyName.compare("TentacleBody1") == 0)
        _tentVisible1 = true;

    if(bodyName.compare("TentacleBody2") == 0)
        _tentVisible2 = true;
}

void ZGlobBoss::attack(std::string direction, std::string bodyName)
{
    if(bodyName.compare("HeadBody") == 0)
        headAttack(direction);
    else if(bodyName.compare("TentacleBody1") == 0)
        tentacleAttack(1, direction);
    else if(bodyName.compare("TentacleBody2") == 0)
        tentacleAttack(2, direction);
    else throw fwk::InternalException("Invalid body name");
}

void ZGlobBoss::headAttack(std::string direction)
{
    if(gravityField())
    {
        // Calculate name
        if(_numShots == 15)
            _numShots = 1;

        std::stringstream name;
        name << "GBShot" << _numShots;

        // Calculate shot
        float angle = body("HeadBody")->b2body()->GetAngle();
        b2Vec2 initPos = body("HeadBody")->b2body()->GetPosition() + 1 * b2Vec2(sin(angle), -cos(angle));
        float shotRadius = jl::Tool::distance(initPos, gravityField()->position()) + 1;

        // Create shot
        GBShot::Ptr shot = new GBShot(name.str(), static_cast<ZorbitLevel *>(level()), _layerNames, initPos, shotRadius, direction);
        level()->entityMan()->entityNew(shot->name(), "Layer1", shot);

        _numShots++;

        // Play sound
        level()->soundMan()->playSound("gboss_roar", false);
    }
    else throw fwk::InternalException("GlobBoss should not be placed outside a gravity field!");
}

void ZGlobBoss::tentacleAttack(int which, std::string direction)
{
    level()->soundMan()->playSound("gboss_tentacle", false);
    if(which == 1)
    {
        if(_tentOverlap1 && _tentVisible1)
        {
            static_cast<Zorbit *>(level()->entityMan()->entity("Zorbit").ptr())->hit(_damage);

            b2Vec2 direction = b2Vec2(sin(body("TentacleBody1")->b2body()->GetAngle()), -cos(body("TentacleBody1")->b2body()->GetAngle()));
            level()->entityMan()->entity("Zorbit")->rootBody()->b2body()->ApplyLinearImpulse(200 * direction, level()->entityMan()->entity("Zorbit")->rootBody()->b2body()->GetWorldCenter());
        }
    }
    else if(which == 2)
    {
        if(_tentOverlap2 && _tentVisible2)
        {
            static_cast<Zorbit *>(level()->entityMan()->entity("Zorbit").ptr())->hit(_damage);

            b2Vec2 direction = b2Vec2(sin(body("TentacleBody2")->b2body()->GetAngle()), -cos(body("TentacleBody2")->b2body()->GetAngle()));
            level()->entityMan()->entity("Zorbit")->rootBody()->b2body()->ApplyLinearImpulse(200 * direction, level()->entityMan()->entity("Zorbit")->rootBody()->b2body()->GetWorldCenter());
        }
    }
    else throw fwk::InternalException("Invalid Tentacle Body!");

}

void ZGlobBoss::moveRandom(std::string bodyName)
{
    if(bodyName.compare("TentacleBody1") == 0)
    {
        body(bodyName)->b2body()->SetTransform(level()->entityMan()->entity("Zorbit")->position(), 0);
    }
    else if(bodyName.compare("TentacleBody2") == 0)
    {
        body(bodyName)->b2body()->SetTransform(level()->entityMan()->entity("Zorbit")->position(), 0);
    }
    else
    {
        int pos = rand() % 4 + 1;
        _headPos = pos;

        switch(pos)
        {
        case 1:
            body(bodyName)->b2body()->SetTransform(_positions["pos1"], 0);
            break;
        case 2:
            body(bodyName)->b2body()->SetTransform(_positions["pos2"], 0);
            break;
        case 3:
            body(bodyName)->b2body()->SetTransform(_positions["pos3"], 0);
            break;
        case 4:
            body(bodyName)->b2body()->SetTransform(_positions["pos4"], 0);
            break;
        default:
            throw fwk::InternalException("Invalid position");
            break;
        }
    }

    body(bodyName)->b2body()->SetAwake(true);
    body(bodyName)->b2body()->SetLinearVelocity(b2Vec2(0,0));
}

void ZGlobBoss::draw()
{
    if(_headVisible)
        rootBody()->draw();
    if(_tentVisible1)
        body("TentacleBody1")->draw();
    if(_tentVisible2)
        body("TentacleBody2")->draw();
    level()->game()->renderWindow()->draw(_mouthSprite);
}

void ZGlobBoss::cleanUp()
{
    _positions.clear();
    actionMan()->clearActions();

    ZBoss::cleanUp();
}

// **************************
//  GBShot methods
// **************************

GBShot::GBShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float distance, std::string direction, float duration) : jl::Projectile(name,
            level, layerNames, direction, duration, "GBShot")
{
    jl::TextureDesc::Ptr tex;
    tex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/globboss/gbshot.png",
            sf::Vector2f(27, 27), 54, 54);

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("GBShotBody", "foreground", tex, &bodyDef, initPos, 0);
    rootBody()->sprite()->scale(2,2);
    rootBody()->b2body()->SetLinearDamping(0);

    _initialDistance = distance + 0.75f;
    _spaceAngle = 0;

    // Set values
    _maxSpeed       = 20;
    _damage         = 1;

    float width     = 0.5;
    float height    = 0.5;

    defineFixtures(width, height);
    defineAnimations();
    defineActions();
    defineSounds();

}

GBShot::GBShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float angle, float duration) : jl::Projectile(name,
            level, layerNames, "Space", duration, "GBShot")
{
    jl::TextureDesc::Ptr tex;
    tex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/globboss/gbshot.png",
            sf::Vector2f(27, 27), 54, 54);

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("GBShotBody", "foreground", tex, &bodyDef, initPos, 0);
    rootBody()->sprite()->scale(2,2);
    rootBody()->b2body()->SetLinearDamping(0);

    _initialDistance = 0;
    _spaceAngle = angle;

    // Set values
    _maxSpeed       = 10;
    _damage         = 1;

    float width     = 0.5;
    float height    = 0.5;

    defineFixtures(width, height);
    defineAnimations();
    defineActions();
    defineSounds();

}

void GBShot::defineAnimations()
{
    _knownAnims["Move"] = new jl::Animation("Move", sf::IntRect(0, 0, 54, 54), 6, true);

    currentAnimIs(_knownAnims["Move"]);
}

void GBShot::defineActions()
{
    _knownActions["Move"] = new gbShotMoveAction(this);
}

void GBShot::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    Projectile::beginContact(contact, thisFixUD, thatCollidable, thatFixUD);

    // Get the type of the other collidable for comparisons.
    string thatType = thatCollidable->type();

    if(thatType.compare("Player") == 0
       && thisFixUD->name().compare("Root") == 0
       && thatFixUD->name().compare("Root") == 0)
    {
        static_cast<Zorbit *>(thatCollidable.ptr())->hit(_damage);
        toDie();
    }
}

void GBShot::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    Projectile::endContact(contact, thisFixUD, thatCollidable, thatFixUD);
}

void GBShot::updateConcrete()
{
    if(noActions())
        _actionMan->pushAction(_knownActions["Move"]);
}

void gbDieAction::update(bool animFinished)
{
    if(!_finished)
    {
        actor()->level()->eventMan()->event("bossfinish")->execute();
        actor()->rootBody()->changeTexture(actor()->level()->game()->textureMan()->textureDesc("resources/zorbitsorbits/globboss/death.png"));

        _finished = true;
    }
    if(actor()->body("TentacleBody1")) actor()->body("TentacleBody1")->sprite()->setColor(sf::Color(255,255,255,actor()->body("TentacleBody1")->sprite()->getColor().a - 5));
    if(actor()->body("TentacleBody2")) actor()->body("TentacleBody2")->sprite()->setColor(sf::Color(255,255,255,actor()->body("TentacleBody2")->sprite()->getColor().a - 5));

    if(static_cast<ZGlobBoss*>(actor())->_mouthSprite.getColor().a >= 5)
       static_cast<ZGlobBoss*>(actor())->_mouthSprite.setColor(sf::Color(255,255,255, static_cast<ZGlobBoss*>(actor())->_mouthSprite.getColor().a - 5));
    else static_cast<ZGlobBoss*>(actor())->_mouthSprite.setColor(sf::Color(255,255,255, 0));

    if(animFinished)
    {
        actor()->die();
    }
    Action::update(animFinished);
}


