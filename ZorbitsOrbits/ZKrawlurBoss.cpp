
// Copyright(c) 2017 Happy Rock Studios

#include "ZKrawlurBoss.h"
#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "Zorbit.h"
#include "JollyLava/Tool.h"
#include "ZShot.h"
#include "ZPlatform.h"

ZKrawlurBoss::ZKrawlurBoss(ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle) : ZBoss("KrawlurBoss",
            level, layerNames)
{
    _prefix = "kboss";

    // Body texture
    jl::TextureDesc::Ptr bodyTex;
    bodyTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/krawlurboss/krawlur.png",
            sf::Vector2f(129, 107), 258, 215);

    // Blank texture, used for gun position
    jl::TextureDesc::Ptr gunTex;
    gunTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/blank.png",
            sf::Vector2f(1, 1), 1, 1);

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("ZKrawlurBossBody", "foreground", bodyTex, &bodyDef, initPos, initAngle);

    // Create gun body.
    float xDistToGun = 0; //cos(jl::Tool::b2Angle(initAngle));
    float yDistToGun = 2; //100 * sin(jl::Tool::b2Angle(initAngle));
    bodyNew("ZKrawlurGunBody", "foreground", gunTex, &bodyDef, initPos + b2Vec2(xDistToGun, yDistToGun), 0);

    // Create foot body.
    yDistToGun = -3;
    bodyNew("ZKrawlurFootBody", "foreground", gunTex, &bodyDef, initPos + b2Vec2(xDistToGun, yDistToGun), 0);

    float width = 1.0f;
    float height = 1.0f;

    // Set values
    _active         = false;
    _maxHealth      = 70;   //200
    _currentHealth  = _maxHealth;
    _damage         = 1;
    _numShots       = 1;
    _maxSpeed       = 13;
    _jumpHeight     = 350;
    _lFootContacts  = 0;
    _rFootContacts  = 0;
    _maxShotsTracker = 1;
    _minShotIntervalInSeconds = 0.5;
    _attacking      = false;
    _seesZorbit     = false;
    _phase2         = false;
    _phase3         = false;
    _leftTree       = false;
    _rightTree      = false;
    _ground         = false;
    _jumped         = false;
    _shooting       = false;
    directedRightIs(true);

    // Constant values
    MAXSHOTS = 3;

    defineFixtures(width, height);
    defineAnimations();
    defineSounds();
    defineActions();

    // Add chronometers to chronometer list
    _chronos["shottimer"] = _shotTimer;
    _chronos["actiontimer"] = _actionTimer;
    _chronos["shottimer"].resume();
    _chronos["actiontimer"].resume();
}

void ZKrawlurBoss::activate()
{
    ZBoss::activate();
}

void ZKrawlurBoss::defineFixtures(float width, float height)
{
    // Torso fixture
    b2FixtureDef fixDef;
    b2PolygonShape fixShape;
    fixShape.SetAsBox(width, height * 1.3);
    fixDef.shape = &fixShape;
    fixDef.density = 1.0f;
    fixDef.friction = 0.0f;
    fixDef.restitution = 0.0f;
    fixDef.isSensor = false;

    jl::FixtureUserData::Ptr fixUD = new jl::FixtureUserData("Root");
    rootBody()->fixtureNew(fixUD, &fixDef);

    // Gun fixture
    fixShape.SetAsBox(width / 4, height / 4);
    fixDef.shape = &fixShape;
    fixDef.density = 0.0f;
    fixDef.isSensor = true;
    jl::FixtureUserData::Ptr fixUD2 = new jl::FixtureUserData("Gun");
    body("ZKrawlurGunBody")->fixtureNew(fixUD2, &fixDef);

    // Foot fixture
    fixShape.SetAsBox(width, height / 4);
    fixDef.shape = &fixShape;
    fixDef.density = 0.0f;
    fixDef.isSensor = true;
    jl::FixtureUserData::Ptr fixUD3 = new jl::FixtureUserData("Foot");
    body("ZKrawlurFootBody")->fixtureNew(fixUD3, &fixDef);

    // Gun joint.
    b2RevoluteJointDef tJointDef;
    tJointDef.Initialize(body("ZKrawlurBossBody")->b2body(), body("ZKrawlurGunBody")->b2body(), body("ZKrawlurGunBody")->b2body()->GetPosition());
    _gunJoint = (b2RevoluteJoint *)this->level()->b2world()->CreateJoint(&tJointDef);

    // foot joint
    tJointDef.Initialize(body("ZKrawlurBossBody")->b2body(), body("ZKrawlurFootBody")->b2body(), body("ZKrawlurFootBody")->b2body()->GetPosition());
    _footJoint = (b2RevoluteJoint *)this->level()->b2world()->CreateJoint(&tJointDef);

    jl::Character::defineFixtures(width, height);
}

void ZKrawlurBoss::defineAnimations()
{
    _knownAnims["Stop"]             = new jl::Animation("Stop", sf::IntRect(0, 0, 199, 215), 1, true);
    _knownAnims["MoveRight"]        = new jl::Animation("MoveRight", sf::IntRect(0, 0, 199, 215), 3, true);
    _knownAnims["MoveLeft"]         = new jl::Animation("MoveLeft", sf::IntRect(0, 0, 199, 215), 3, true);
    _knownAnims["Attack"]           = _knownAnims["Stop"];
    _knownAnims["Jump"]             = new jl::Animation("Jump", sf::IntRect(600, 0, 199, 215), 2, false, 600);
    _knownAnims["Die"]              = new jl::Animation("Die", sf::IntRect(0, 215, 258, 222), 6, false, 120);

    currentAnimIs(_knownAnims["Stop"]);
}

void ZKrawlurBoss::defineSounds()
{
    level()->soundMan()->soundNew("kboss_hit", "resources/sounds/tboss_hit.wav", false);
    level()->soundMan()->soundNew("kboss_die", "resources/sounds/kboss_die.wav", false);
    level()->soundMan()->soundNew("kboss_laugh", "resources/sounds/kboss_laugh.wav", false);
    level()->soundMan()->soundNew("kboss_jump", "resources/sounds/kboss_jump.wav", false);
    level()->soundMan()->soundNew("kboss_shoot", "resources/sounds/kboss_shoot.wav", false);
}

void ZKrawlurBoss::defineActions()
{
    _knownActions["MoveRight"]          = new jl::cMRightAction(this);
    _knownActions["MoveLeft"]           = new jl::cMLeftAction(this);
    _knownActions["Attack"]             = new kbAttackAction(this);
    _knownActions["Jump"]               = new kbJumpAction(this);
    _knownActions["Stop"]               = new jl::aStopAction(this);
    _knownActions["Die"]                = new bDieAction("tb_die", this);
}

void ZKrawlurBoss::reset()
{
    ZBoss::reset();
    _phase2 = false;
    _phase3 = false;
    _minShotIntervalInSeconds = 0.5;
    _maxShotsTracker = 1;
    _maxSpeed = 13;
    _jumpHeight     = 350;
    if(!directedRight()) switchDirection();
    _chronos["shottimer"] .reset(true);
    _chronos["actiontimer"] .reset();
}

void ZKrawlurBoss::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{

    // First run through all collisions handled by superclasses.
    ZBoss::beginContact(contact, thisFixUD, thatCollidable, thatFixUD);

    // Get the type of the other collidable for comparisons.
    string thatType = thatCollidable->type();
    if(thatType.compare("ZShot") == 0
       && thisFixUD->name().compare("Root") == 0)
    {
        static_cast<ZShot *>(thatCollidable.ptr())->toDie();
        decreaseHealth(static_cast<ZShot *>(thatCollidable.ptr())->damage());
        hit();

        if(level()->soundMan()->sound("kboss_hit"))
            level()->soundMan()->playSound("kboss_hit", true);
    }
    if(thatType.compare("Player") == 0
       && thatFixUD->name().compare("Root") == 0
       && thisFixUD->name().compare("Root") == 0)
    {
        static_cast<Zorbit *>(thatCollidable.ptr())->hit(_damage);
    }
    if(thatCollidable->name().compare("Planet8") == 0
       && thisFixUD->name().compare("Bottom") == 0)
    {
        _ground = true;
        _shooting = false;
    }
    if(thatCollidable->name().compare("Planet150") == 0
       && thisFixUD->name().compare("Bottom") == 0)
        _leftTree = true;
    if(thatCollidable->name().compare("Planet151") == 0
       && thisFixUD->name().compare("Bottom") == 0)
        _rightTree = true;
}

void ZKrawlurBoss::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    ZBoss::endContact(contact, thisFixUD, thatCollidable, thatFixUD);

    string thatType = thatCollidable->type();
    if(thatType.compare("Planet") == 0
       && thisFixUD->name().compare("Foot") == 0)
    {
        //_footContacts--;
    }

    if((thatCollidable->name().compare("Gravity3") == 0
       || thatCollidable->name().compare("Gravity4") == 0)
       && thisFixUD->name().compare("Root") == 0)
        randomDirection();

    if(thatCollidable->name().compare("Planet8") == 0
       && thisFixUD->name().compare("Bottom") == 0)
        _ground = false;
    if(thatCollidable->name().compare("Planet150") == 0
       && thisFixUD->name().compare("Bottom") == 0)
        _leftTree = false;
    if(thatCollidable->name().compare("Planet151") == 0
       && thisFixUD->name().compare("Bottom") == 0)
        _rightTree = false;
}

void ZKrawlurBoss::updateConcrete()
{
    if (_currentHealth > 0)
    {
        if(noActions())
            _actionMan->pushAction(_knownActions["Stop"]);

        if(!_active)
            _chronos["actiontimer"] .reset(true);
        else
        {
            if(!grounded())
            {
                if(polarVelocity().y > 0)
                    rootBody()->sprite()->setTextureRect(sf::IntRect(600, 0, 200, 215));
                else
                    rootBody()->sprite()->setTextureRect(sf::IntRect(800, 0, 200, 215));
            }
            if(_shooting)
            {
                attack();
                _shooting = false;
            }
            if(_currentHealth >= _maxHealth * 0.6)
                phase1(directedRight());
            else if(_currentHealth >= _maxHealth * 0.25
                    && _currentHealth < _maxHealth * 0.6)
                phase2(directedRight());
            else if(_currentHealth < _maxHealth * 0.25)
                phase3(directedRight());
        }
    }
}

void ZKrawlurBoss::phase1(bool right)
{
    _chronos["actiontimer"] .resume();

    if(_chronos["actiontimer"] .getElapsedTime().asSeconds() < 1.5)
    {
        if(_chronos["actiontimer"] .getElapsedTime().asSeconds() < 0.3)
            rootBody()->sprite()->setTextureRect(sf::IntRect(1000, 0, 200, 215));
        if(_chronos["actiontimer"] .getElapsedTime().asSeconds() >= 0.3
           && _chronos["actiontimer"] .getElapsedTime().asSeconds() < 0.6)
            rootBody()->sprite()->setTextureRect(sf::IntRect(1200, 0, 200, 215));
        if(_chronos["actiontimer"] .getElapsedTime().asSeconds() >= 0.6
           && _chronos["actiontimer"] .getElapsedTime().asSeconds() < 0.9)
            rootBody()->sprite()->setTextureRect(sf::IntRect(1000, 0, 200, 215));
        if(_chronos["actiontimer"] .getElapsedTime().asSeconds() >= 0.9
           && _chronos["actiontimer"] .getElapsedTime().asSeconds() < 1.2)
            rootBody()->sprite()->setTextureRect(sf::IntRect(1200, 0, 200, 215));
        if(_chronos["actiontimer"] .getElapsedTime().asSeconds() >= 1.2
           && _chronos["actiontimer"] .getElapsedTime().asSeconds() < 1.5)
            rootBody()->sprite()->setTextureRect(sf::IntRect(1000, 0, 200, 215));

        level()->soundMan()->playSound("kboss_laugh", false);
        return;
    }

    // On the ground
    if(_ground)
    {
        _maxSpeed = 13;

        if(right) // Head right
        {
            float planetAngle = jl::Tool::polarAngle(level()->planetMan()->gravity("Gravity4")->position(),
                                                  level()->planetMan()->gravity("Gravity2")->position()) + 0.2;
            float polarAngle = this->polarAngle();

            if(polarAngle > planetAngle
               && actionMan()->peekAction()->name().compare("MoveRight") != 0)
            {
                _actionMan->clearActions();
                _actionMan->pushAction(_knownActions["MoveRight"]);
            }
            else if(polarAngle <= planetAngle
                    && actionMan()->peekAction()->name().compare("Stop") != 0)
            {
                _actionMan->clearActions();
                _actionMan->pushAction(_knownActions["Stop"]);
            }
            else if (polarAngle <= planetAngle
                     && actionMan()->peekAction()->name().compare("Jump") != 0)
            {
                _actionMan->clearActions();
                _actionMan->pushAction(_knownActions["Jump"]);
            }
        }
        else
        {
            float planetAngle = jl::Tool::polarAngle(level()->planetMan()->gravity("Gravity3")->position(),
                                                  level()->planetMan()->gravity("Gravity2")->position()) + jl::Tool::PI() - 0.2;
            float polarAngle = this->polarAngle() + jl::Tool::PI();

            if(polarAngle < planetAngle
               && actionMan()->peekAction()->name().compare("MoveLeft") != 0)
            {
                _actionMan->clearActions();
                _actionMan->pushAction(_knownActions["MoveLeft"]);
            }
            else if(polarAngle >= planetAngle
                    && actionMan()->peekAction()->name().compare("Stop") != 0)
            {
                _actionMan->clearActions();
                _actionMan->pushAction(_knownActions["Stop"]);
            }
            else if (polarAngle >= planetAngle
                     && actionMan()->peekAction()->name().compare("Jump") != 0)
            {
                _actionMan->clearActions();
                _actionMan->pushAction(_knownActions["Jump"]);
            }
        }
    }
    // On left tree
    else if(_leftTree)
    {
        float desiredAngle = 1.6;

        if(abs(polarAngle() - desiredAngle) >= 0.3)
        {
            if(actionMan()->peekAction()->name().compare("MoveLeft") != 0)
                actionMan()->pushAction(_knownActions["MoveLeft"]);
        }
        else
        {
            if(actionMan()->peekAction()->name().compare("MoveLeft") == 0)
            {
                rootBody()->b2body()->SetLinearVelocity(b2Vec2(0,0));
                actionMan()->clearActions();
                actionMan()->pushAction(_knownActions["Attack"]);
                switchDirection();
            }
            else if(_maxShotsTracker < MAXSHOTS
                    && actionMan()->peekAction()->name().compare("Jump") != 0)
            {
                actionMan()->clearActions();
                actionMan()->pushAction(_knownActions["Attack"]);
            }
            else if(_maxShotsTracker >= MAXSHOTS)
            {
                actionMan()->clearActions();
                actionMan()->pushAction(_knownActions["Jump"]);
                _maxShotsTracker = 1;
            }
        }
    }
    // On right tree
    else if(_rightTree)
    {
        float desiredAngle = 3.6;

        if(abs(polarAngle() - desiredAngle) >= 0.3)
        {
            if(actionMan()->peekAction()->name().compare("MoveLeft") != 0)
                actionMan()->pushAction(_knownActions["MoveLeft"]);
        }
        else
        {
            if(actionMan()->peekAction()->name().compare("MoveLeft") == 0)
            {
                rootBody()->b2body()->SetLinearVelocity(b2Vec2(0,0));
                actionMan()->clearActions();
                actionMan()->pushAction(_knownActions["Attack"]);
            }
            else if(_maxShotsTracker < MAXSHOTS
                    && actionMan()->peekAction()->name().compare("Jump") != 0)
            {
                actionMan()->clearActions();
                actionMan()->pushAction(_knownActions["Attack"]);
            }
            else if(_maxShotsTracker >= MAXSHOTS)
            {
                actionMan()->clearActions();
                actionMan()->pushAction(_knownActions["Jump"]);
                _maxShotsTracker = 1;
            }
        }
    }
}

void ZKrawlurBoss::phase2(bool right)
{
    if(!_phase2)
    {
        _chronos["actiontimer"] .reset(true);
        _maxSpeed = 15;
        _phase2 = true;
        _jumpHeight = 250;
    }
    // On the ground
    if(_ground == true)
    {
        _jumpHeight = 250;
        if(right) // Head right
        {
            if(polarAngle() >= 2.1
               && polarAngle() <= 2.2
               && _jumped == false)
            {
                static_cast<kbJumpAction*>(_knownActions["Jump"].ptr())->jumpReset();
                actionMan()->pushAction(_knownActions["Jump"], "Top", true);
                _jumped = true;
                _shooting = true;
            }
            else if(polarAngle() <= 1.35)
            {
                _jumped = false;
                switchDirection();
            }
            else if(actionMan()->peekAction()->name().compare("MoveRight") != 0)
            {
                actionMan()->clearActions();
                actionMan()->pushAction(_knownActions["MoveRight"]);
            }
        }
        else
        {
            if(polarAngle() >= 1.7
               && polarAngle() <= 2.1
               && _jumped == false)
            {
                static_cast<kbJumpAction*>(_knownActions["Jump"].ptr())->jumpReset();
                actionMan()->pushAction(_knownActions["Jump"], "Top", true);
                _jumped = true;
                _shooting = true;
            }
            else if(polarAngle() >= 2.75)
            {
                _jumped = false;
                switchDirection();
            }
            else if(actionMan()->peekAction()->name().compare("MoveLeft") != 0)
            {
                actionMan()->clearActions();
                actionMan()->pushAction(_knownActions["MoveLeft"]);
            }
        }
    }
    // On left tree
    else if(_leftTree == true)
    {
        float desiredAngle = 1.6;

        if(abs(polarAngle() - desiredAngle) >= 0.3)
        {
            if(actionMan()->peekAction()->name().compare("MoveLeft") != 0)
                actionMan()->pushAction(_knownActions["MoveLeft"]);
        }
        else
        {
            rootBody()->b2body()->SetLinearVelocity(b2Vec2(0,0));
            actionMan()->clearActions();
            _jumpHeight = 500;
            actionMan()->pushAction(_knownActions["Jump"]);
        }
    }
    // On right tree
    else if(_rightTree == true)
    {
        float desiredAngle = 3.6;

        if(abs(polarAngle() - desiredAngle) >= 0.3)
        {
            if(actionMan()->peekAction()->name().compare("MoveLeft") != 0)
                actionMan()->pushAction(_knownActions["MoveLeft"]);
        }
        else
        {
            rootBody()->b2body()->SetLinearVelocity(b2Vec2(0,0));
            actionMan()->clearActions();
            _jumpHeight = 500;
            actionMan()->pushAction(_knownActions["Jump"]);
        }
    }
}

void ZKrawlurBoss::phase3(bool right)
{
    if(!_phase3)
    {
        _chronos["actiontimer"] .reset(true);
        _minShotIntervalInSeconds = 1;
        _phase3 = true;
        _shooting = false;
    }

    // On the ground
    if(_ground == true)
    {
        if(right) // Head right
        {
            float planetAngle = jl::Tool::polarAngle(level()->planetMan()->gravity("Gravity4")->position(),
                                                  level()->planetMan()->gravity("Gravity2")->position()) + 0.2;

            if(polarAngle() >= 2.1
               && polarAngle() <= 2.2
               && _jumped == false)
            {
                _jumpHeight = 250;
                static_cast<kbJumpAction*>(_knownActions["Jump"].ptr())->jumpReset();
                actionMan()->pushAction(_knownActions["Jump"], "Top", true);
                _jumped = true;
            }
            else if(polarAngle() > planetAngle
               && actionMan()->peekAction()->name().compare("MoveRight") != 0)
            {
                _actionMan->pushAction(_knownActions["MoveRight"]);
            }
            else if (polarAngle() <= planetAngle)
                     //&& actionMan()->peekAction()->name().compare("Jump") != 0)
            {
                _jumpHeight = 350;
                _jumped = false;
                _actionMan->clearActions();
                _actionMan->pushAction(_knownActions["Jump"]);
            }
        }
        else
        {
            float planetAngle = jl::Tool::polarAngle(level()->planetMan()->gravity("Gravity3")->position(),
                                                  level()->planetMan()->gravity("Gravity2")->position()) + jl::Tool::PI() - 0.2;
            float polarAngle = this->polarAngle() + jl::Tool::PI();

            if(this->polarAngle() >= 1.7
               && this->polarAngle() <= 2.1
               && _jumped == false)
            {
                _jumpHeight = 250;
                static_cast<kbJumpAction*>(_knownActions["Jump"].ptr())->jumpReset();
                actionMan()->pushAction(_knownActions["Jump"], "Top", true);
                _jumped = true;
            }
            else if(polarAngle < planetAngle
               && actionMan()->peekAction()->name().compare("MoveLeft") != 0)
            {
                _actionMan->clearActions();
                _actionMan->pushAction(_knownActions["MoveLeft"]);
            }
            else if (polarAngle >= planetAngle)
                     //&& actionMan()->peekAction()->name().compare("Jump") != 0)
            {
                _jumpHeight = 350;
                _jumped = false;
                _actionMan->clearActions();
                _actionMan->pushAction(_knownActions["Jump"]);
            }
        }
    }
    // On left tree
    else if(_leftTree == true)
    {
        float desiredAngle = 1.6;

        if(abs(polarAngle() - desiredAngle) >= 0.3)
        {
            if(actionMan()->peekAction()->name().compare("MoveLeft") != 0)
                actionMan()->pushAction(_knownActions["MoveLeft"]);
        }
        else
        {
            if(actionMan()->peekAction()->name().compare("MoveLeft") == 0)
            {
                rootBody()->b2body()->SetLinearVelocity(b2Vec2(0,0));
                actionMan()->clearActions();
                actionMan()->pushAction(_knownActions["Attack"]);
            }
            else if(_maxShotsTracker < MAXSHOTS
                    && actionMan()->peekAction()->name().compare("Jump") != 0)
            {
                actionMan()->clearActions();
                actionMan()->pushAction(_knownActions["Attack"]);
            }
            else if(_maxShotsTracker >= MAXSHOTS)
            {
                actionMan()->clearActions();
                actionMan()->pushAction(_knownActions["Jump"]);
                _maxShotsTracker = 1;
            }
        }
    }
    // On right tree
    else if(_rightTree == true)
    {
        float desiredAngle = 3.6;

        if(abs(polarAngle() - desiredAngle) >= 0.3)
        {
            if(actionMan()->peekAction()->name().compare("MoveLeft") != 0)
                actionMan()->pushAction(_knownActions["MoveLeft"]);
        }
        else
        {
            if(actionMan()->peekAction()->name().compare("MoveLeft") == 0)
            {
                rootBody()->b2body()->SetLinearVelocity(b2Vec2(0,0));
                actionMan()->clearActions();
                actionMan()->pushAction(_knownActions["Attack"]);
            }
            else if(_maxShotsTracker < MAXSHOTS
                    && actionMan()->peekAction()->name().compare("Jump") != 0)
            {
                actionMan()->clearActions();
                actionMan()->pushAction(_knownActions["Attack"]);
            }
            else if(_maxShotsTracker >= MAXSHOTS)
            {
                actionMan()->clearActions();
                actionMan()->pushAction(_knownActions["Jump"]);
                _maxShotsTracker = 1;
            }
        }
    }
}

void ZKrawlurBoss::attack()
{
    if(!level()->entityMan()->entity("Zorbit"))
        return;

    if(_chronos["shottimer"] .getElapsedTime().asSeconds() > _minShotIntervalInSeconds)
    {
        //if(!level()->entityMan()->entity("KBShot1"))
        //    _numShots = 1;

        // generate shot name
        _shotName.str("");
        _shotName.clear();
        _shotName << "KBShot" << _numShots;

        float angle = 0;
        if(directedRight())
            angle = -jl::Tool::thetaCoordinate(body("ZKrawlurGunBody")->b2body()->GetPosition(), level()->entityMan()->entity("Zorbit")->position()) - jl::Tool::PI() / 2;
        else
            angle = -jl::Tool::thetaCoordinate(body("ZKrawlurGunBody")->b2body()->GetPosition(), level()->entityMan()->entity("Zorbit")->position()) - jl::Tool::PI() / 2;

        KBShot::Ptr shot = new KBShot(_shotName.str(), static_cast<ZorbitLevel *>(level()), _layerNames, body("ZKrawlurGunBody")->b2body()->GetPosition(), angle);
        level()->entityMan()->entityNew(shot->name(), "Layer1", shot);

        level()->soundMan()->playSound("kboss_shoot", true);

        _numShots++;
        _maxShotsTracker++;
        _chronos["shottimer"] .reset(true);
    }
}

void ZKrawlurBoss::move(std::string direction)
{
    if (_maxSpeed < 0)
        throw fwk::InternalException("Speed must be non-negative.");

    if(direction == "Right" && gravityField())
    {
        if(polarVelocity().x < _maxSpeed)
            rootBody()->applyPolarForceToCenter(0,
                                                _maxSpeed * 100 * gravityField()->specificForce() / gravityField()->defaultSpecificForce());
    }
    else if(direction == "Left" && gravityField())
    {
        if(polarVelocity().x > -_maxSpeed)
            rootBody()->applyPolarForceToCenter(0,
                                                -_maxSpeed * 100 * gravityField()->specificForce() / gravityField()->defaultSpecificForce());
    }
}

void ZKrawlurBoss::jump(float height)
{
    if(_ground || _leftTree || _rightTree)
    {
        std::stringstream jumpSound;
        jumpSound << _prefix << "_jump";
        if(level()->soundMan()->sound(jumpSound.str()))
            level()->soundMan()->playSound(jumpSound.str());

        float offset = 0;
        if(polarVelocity().y < 0)
            offset = abs(polarVelocity().y * 5);
        rootBody()->b2body()->ApplyLinearImpulse(
                (height + offset) * b2Vec2(cos(polarAngle()), sin(polarAngle())),
                rootBody()->b2body()->GetWorldCenter());
    }
}

void ZKrawlurBoss::stop()
{
    /*
    // heading right
    if(polarVelocity().y > 0.125)
            rootBody()->applyPolarForceToCenter(0, -_maxSpeed * 5 * gravityField()->specificForce() / gravityField()->defaultSpecificForce());

    // heading left
    if(polarVelocity().y < -0.125)
            rootBody()->applyPolarForceToCenter(0, _maxSpeed * 5 * gravityField()->specificForce() / gravityField()->defaultSpecificForce());
    */
}

void ZKrawlurBoss::randomDirection()
{
    int direction = rand() % 2 + 1;
    if(direction == 1 && !directedRight())
        switchDirection();
    else if(direction == 2 && directedRight())
        switchDirection();

    if(direction != 1
       && direction != 2)
        throw fwk::InternalException("ZKrawlurBoss::randomDirection bad calculation");
}

void ZKrawlurBoss::destroyJoints()
{
    level()->b2world()->DestroyJoint( _gunJoint );
    level()->b2world()->DestroyJoint( _footJoint );
}

void ZKrawlurBoss::cleanUp()
{
    ZBoss::cleanUp();
}

// **************************
//  KBShot methods
// **************************

KBShot::KBShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float distance, std::string direction, float duration) : jl::Projectile(name,
            level, layerNames, direction, duration, "KBShot")
{
    throw fwk::InternalException("Should not be initializing KBShot this way");
}

KBShot::KBShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float angle, float duration) : jl::Projectile(name,
            level, layerNames, "Space", duration, "KBShot")
{
    jl::TextureDesc::Ptr tex;
    tex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/krawlurboss/kshot.png",
            sf::Vector2f(25, 25), 50, 50);

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("KBShotBody", "foreground", tex, &bodyDef, initPos, jl::Tool::sfAngle(angle - (jl::Tool::PI() / 2)));
    rootBody()->b2body()->SetLinearDamping(0);

    _initialDistance = 0;
    _spaceAngle = angle;

    // Set values
    _maxSpeed       = 70;
    _damage         = 1;

    float width     = 0.25;
    float height    = 0.25;

    defineFixtures(width, height);
    defineAnimations();
    defineActions();
    defineSounds();

}

void KBShot::defineAnimations()
{
    _knownAnims["Move"] = new jl::Animation("Move", sf::IntRect(0, 0, 50, 50), 1, true);

    currentAnimIs(_knownAnims["Move"]);
}

void KBShot::defineActions()
{
    _knownActions["Move"] = new kbShotMoveAction(this);
}

void KBShot::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{

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

void KBShot::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
}

void KBShot::updateConcrete()
{
    if(!level()->entityMan()->entity("Zorbit")
       || static_cast<Zorbit*>(level()->entityMan()->entity("Zorbit").ptr())->currentHealth() <= 0)
        toDie();

    if(noActions())
        _actionMan->pushAction(_knownActions["Move"]);
}

