// Copyright(c) 2017 Happy Rock Studios

#include "ZFunzoBoss.h"
#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "Zorbit.h"
#include "ZShot.h"
#include "ZCircle.h"
#include "JollyLava/Tool.h"

FunzoHand::FunzoHand(bool leftHand, jl::TextureDesc::Ptr tex, b2Vec2 gravPosition)
{
    _sprite.setTexture(*tex->texture());
    _sprite.setOrigin(tex->origin());
    _sprite.setTextureRect(tex->rect());

    if(leftHand)    _spritePosition = sf::Vector2f(17164,10276);
    else            _spritePosition = sf::Vector2f(18732,10276);
    _sprite.setPosition(_spritePosition);
    _position = jl::Tool::b2Vec(_spritePosition);
    _angle = 0;

    _isLeftHand                     = leftHand;
    _actionsLocked                  = false;
    _gravPosition                   = gravPosition;
    if(_isLeftHand) _directedLeft   = true;
    else            _directedLeft   = false;
    _distance                       = jl::Tool::distance(_position, _gravPosition);

    if(_isLeftHand) _sprite.setTextureRect(sf::IntRect(0,0,300,309));
    else _sprite.setTextureRect(sf::IntRect(370,0,300,309));

    _state = Stop;
    _stopCounter = 0;
    _moveCounter = 0;
    _smashCounter = 0;
    _clapCounter = 0;
    _jumping = false;
}

void FunzoHand::reset()
{
    if(_isLeftHand)    _spritePosition = sf::Vector2f(17164,10276);
    else            _spritePosition = sf::Vector2f(18732,10276);
    _sprite.setPosition(_spritePosition);
    _position = jl::Tool::b2Vec(_spritePosition);
    _angle = 0;

    _actionsLocked                  = false;
    if(_isLeftHand) _directedLeft   = true;
    else            _directedLeft   = false;
    _distance                       = jl::Tool::distance(_position, _gravPosition);

    if(_isLeftHand) _sprite.setTextureRect(sf::IntRect(0,0,300,309));
    else _sprite.setTextureRect(sf::IntRect(370,0,300,309));

    _state = Stop;
    _stopCounter = 0;
    _moveCounter = 0;
    _smashCounter = 0;
    _clapCounter = 0;
    _actionQueue.clear();
}

void FunzoHand::addAction(HandState action, int loops)
{
    if(_actionsLocked) return;

    // This ensures that we don't add the same action twice.
    // Instead if we want an action to continue, we should
    // change the number of loops.
    if(_actionQueue.back().first == action) return;

    std::pair<HandState, int> p;
    p.first = action;
    p.second = loops;
    _actionQueue.push_back(p);
}

void FunzoHand::drop(int amount)
{
    moveHand(-amount,0);
}

void FunzoHand::update()
{
    _spritePosition = jl::Tool::sfVec(_position);
    _sprite.setPosition(_spritePosition);
    _sprite.setRotation(jl::Tool::sfAngle(atan2(_position.x - _gravPosition.x, _position.y - _gravPosition.y)));
    _angle = -jl::Tool::b2Angle(_sprite.getRotation());

    if(_actionQueue.empty())
    {
        if(_actionsLocked)
            _actionsLocked = false;
        addAction(Stop, 1);
    }
    if(_actionQueue.empty()) return;

    _state = _actionQueue.front().first;

    switch(_state)
    {
        case Stop:
            {
                _stopCounter++;
                if(_stopCounter < 20)
                    moveHand(0.01, 0);
                else if(_stopCounter >=20 && _stopCounter < 39)
                    moveHand(-0.01, 0);
                else
                {
                    _stopCounter = 0;
                    _actionQueue.front().second--;
                }
            }
            break;
        case Move:
            {
                _moveCounter++;
                if(_directedLeft) moveHand(0, -0.1);
                else moveHand(0, 0.1);

                if(_moveCounter >= 30)
                {
                    _moveCounter = 0;
                    _actionQueue.front().second--;
                }
            }
            break;
        case Clap:
            {
                _clapCounter++;
                if(_clapCounter < 60)
                {
                    if(_isLeftHand) _sprite.setTextureRect(sf::IntRect(305,0,355,600));
                    else _sprite.setTextureRect(sf::IntRect(0,0,355,600));
                    if(_isLeftHand) _directedLeft = true;
                    else _directedLeft = false;
                    if(_directedLeft) moveHand(0, -0.85);
                    else moveHand(0, 0.85);
                }
                else if (_clapCounter >= 60 && _clapCounter < 68) {}
                else if (_clapCounter >= 68 && _clapCounter < 129)
                {
                    if(_directedLeft) moveHand(0, 0.85);
                    else moveHand(0, -0.85);
                }
                else if(_clapCounter >= 99)
                {
                    if(_isLeftHand) _sprite.setTextureRect(sf::IntRect(0,0,300,309));
                    else _sprite.setTextureRect(sf::IntRect(370,0,300,309));
                    _clapCounter = 0;
                    _actionQueue.front().second--;
                }
            }
            break;
        case Smash:
            {
                _smashCounter++;

                if(_smashCounter < 19)
                    moveHand(0.5, 0);
                else if(_smashCounter >= 19 && _smashCounter < 40) {}
                else if(_smashCounter >= 40 && _smashCounter < 51)
                {
                    moveHand(-1,0);
                    _jumping = true;
                }
                else if(_smashCounter >=51 && _smashCounter < 80) { _jumping = false; }
                else if(_smashCounter>=80 && abs(jl::Tool::distance(_position, _gravPosition)) < abs(_distance))
                    moveHand(0.5, 0);
                else
                {
                    _smashCounter = 0;
                    _actionQueue.front().second--;
                }
            }
            break;
        default:
            break;
    }

    if(_actionQueue.front().second <= 0)
        _actionQueue.pop_front();
}

void FunzoHand::moveHand(float radial, float tangential)
{
    float theta = jl::Tool::polarAngle(_position, _gravPosition); //atan((_position.x - _gravPosition.x) / (_position.y - _gravPosition.y));
    float forceX = radial * cos(theta) + tangential * sin(theta);
    float forceY = radial * sin(theta) - tangential * cos(theta);
    _position = _position + b2Vec2(forceX, forceY);
}

// FUNZO

ZFunzoBoss::ZFunzoBoss(ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle) : ZBoss("FunzoBoss",
            level, layerNames)
{
    _prefix = "funzo";
    _initPos = initPos;
    _initAngle = initAngle;

    // Body texture
    jl::TextureDesc::Ptr bodyTex;
    bodyTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/funzoboss/funzobody.png",
            sf::Vector2f(272, 150), 545, 685);

    // Create root body
    b2BodyDef bodyDef;
    bodyDef.type = b2_staticBody;
    bodyNew("FunzoBody", "foreground", bodyTex, &bodyDef, initPos, initAngle);

    // Head texture
    jl::TextureDesc::Ptr headTex;
    headTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/funzoboss/funzohead.png",
            sf::Vector2f(400, 400), 800, 800);

    // Create head body
    float xDistToHead = -sin(jl::Tool::b2Angle(initAngle));
    float yDistToHead = 5.0 * cos(jl::Tool::b2Angle(initAngle));
    bodyDef.type = b2_dynamicBody;
    bodyNew("FunzoHead", "foreground", headTex, &bodyDef, initPos + b2Vec2(xDistToHead, yDistToHead), initAngle);

    // Hand textures
    jl::TextureDesc::Ptr lHandDesc;
    jl::TextureDesc::Ptr rHandDesc;
    lHandDesc = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/funzoboss/funzolhand.png", sf::Vector2f(150, 150), 300, 309);
    rHandDesc = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/funzoboss/funzorhand.png", sf::Vector2f(150, 150), 300, 309);

    _leftHand = new FunzoHand(true, lHandDesc, this->level()->planetMan()->gravity("Gravity7")->position());
    _rightHand = new FunzoHand(false, rHandDesc, this->level()->planetMan()->gravity("Gravity7")->position());

    bodyNew("LeftHand", "foreground", lHandDesc, &bodyDef, initPos, initAngle);
    bodyNew("RightHand", "foreground", rHandDesc, &bodyDef, initPos, initAngle);
    body("LeftHand")->visibleIs(false);
    body("RightHand")->visibleIs(false);

    float width = 5.0f;
    float height = 7.0f;

    // Set values
    _active         = false;
    _maxHealth      = 100;
    _currentHealth  = _maxHealth;
    _damage         = 1;
    _maxSpeed       = 0;
    _jumpHeight     = 0;
    _lFootContacts  = 0;
    _rFootContacts  = 0;
    _numShots       = 0;
    _loops          = 0;
    _wheelSpeed     = 2;
    _headJointOn    = true;
    _fading         = false;
    _dieing         = false;
    _wheel          = "left";
    directedRightIs(false);

    _chronos["actiontimer"] = _actionTimer;
    _chronos["shottimer"] = _shotTimer;
    _chronos["dietimer"] = _dieTimer;
    _chronos["fadetimer"] = _fadeTimer;

    _chronos["actiontimer"].reset(true);
    _chronos["shottimer"].reset(true);
    _chronos["dietimer"].reset(true);
    _chronos["fadetimer"].reset(true);

    defineFixtures(width, height);
    defineAnimations();
    defineSounds();
    defineActions();
}

void ZFunzoBoss::defineFixtures(float width, float height)
{
    // Torso fixture
    b2FixtureDef fixDef;
    b2PolygonShape fixShape;
    fixShape.SetAsBox(width, height);
    fixDef.shape = &fixShape;
    fixDef.density = 1.0f;
    fixDef.friction = 10.0f;

    jl::FixtureUserData::Ptr fixUD = new jl::FixtureUserData("Root");
    rootBody()->fixtureNew(fixUD, &fixDef);

    // Head fixture
    b2CircleShape headShape;
    headShape.m_radius = width * 1.25;
    fixDef.shape = &headShape;
    fixDef.isSensor = true;
    fixDef.density = 0.001f;

    jl::FixtureUserData::Ptr fixUD2 = new jl::FixtureUserData("Head");
    body("FunzoHead")->fixtureNew(fixUD2, &fixDef);

    // Left hand
    fixShape.SetAsBox(2.25,3.75);
    fixDef.shape = &fixShape;
    fixDef.density = 5.0;
    fixDef.friction = 1.0;
    fixDef.isSensor = false;

    jl::FixtureUserData::Ptr fixUD3 = new jl::FixtureUserData("LeftHand");
    body("LeftHand")->fixtureNew(fixUD3, &fixDef);

    // Right hand
    jl::FixtureUserData::Ptr fixUD4 = new jl::FixtureUserData("RightHand");
    body("RightHand")->fixtureNew(fixUD4, &fixDef);
}

void ZFunzoBoss::defineAnimations()
{
    _knownAnims["Stop"]            = new jl::Animation("Stop", sf::IntRect(0, 0, 545, 685), 1, true);
    _knownAnims["SpinShoot"]       = new jl::Animation("SpinShoot", sf::IntRect(0,0, 545, 685), 1, false);
    _knownAnims["TongueWhip"]      = new jl::Animation("TongueWhip", sf::IntRect(0,0, 545, 685), 1, false);
    _knownAnims["Die"]             = new jl::Animation("Die", sf::IntRect(0, 0, 545, 685), 1, false);

    currentAnimIs(_knownAnims["Stop"]);
}

void ZFunzoBoss::defineSounds()
{
    level()->soundMan()->soundNew("fb_hit", "resources/sounds/fb_hit.wav", false);
    level()->soundMan()->soundNew("fb_fake_hit", "resources/sounds/fb_fake_hit.wav", false);
    level()->soundMan()->soundNew("fb_hand_land", "resources/sounds/fb_hand_land.wav", false);
    level()->soundMan()->soundNew("fb_laugh", "resources/sounds/fb_laugh.wav", false);
    level()->soundMan()->soundNew("fb_laugh_die", "resources/sounds/fb_laugh_die.wav", false);
    level()->soundMan()->soundNew("fb_snot_shot", "resources/sounds/fb_snot_shot.wav", false);
}

void ZFunzoBoss::defineActions()
{
    _knownActions["SpinShoot"]          = new fbSpinShootAction(1, this);
    _knownActions["SpinShootLong"]      = new fbSpinShootAction(2, this);
    _knownActions["TongueWhip"]         = new fbTongueAction(1, this);
    _knownActions["Stop"]               = new jl::aStopAction(1, this);
    _knownActions["Die"]                = new bDieAction("fb_die", this);
}

void ZFunzoBoss::readySet()
{
    Actor::readySet();

    if(gravityField())
    {
        // Head joint
        b2RevoluteJointDef jointDef2;
        jointDef2.bodyA = body("FunzoHead")->b2body();
        jointDef2.bodyB = gravityField()->b2body();
        float xDistToHead = -sin(jl::Tool::b2Angle(_initAngle));
        float yDistToHead = 27.0 * cos(jl::Tool::b2Angle(_initAngle));
        jointDef2.localAnchorB = b2Vec2(xDistToHead, yDistToHead);
        jointDef2.motorSpeed = 2.0f;
        jointDef2.maxMotorTorque = 100.0f;
        jointDef2.collideConnected = false;
        jointDef2.enableMotor = false;
        _headJoint = (b2RevoluteJoint *)this->level()->b2world()->CreateJoint(&jointDef2);
    }
}

void ZFunzoBoss::reset()
{
    _active = false;
    level()->hudMan()->hud("BossHUD")->reset();
    actionMan()->clearActions();
    _currentHealth = _maxHealth;

    _loops          = 0;
    _wheelSpeed     = 2;
    _attacking      = false;
    _seesZorbit     = false;

    resetHands();

    _chronos["actiontimer"].reset(true);
    _chronos["shottimer"].reset(true);

    stopHeadSpin();
}

void ZFunzoBoss::resetHands()
{
    _leftHand->reset();
    _rightHand->reset();
}

void ZFunzoBoss::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    ZBoss::beginContact(contact, thisFixUD, thatCollidable, thatFixUD);

    // Get the type of the other collidable for comparisons.
    string thatType = thatCollidable->type();
    if(thatType.compare("Player") == 0)
    {
        if((thisFixUD->name().compare("LeftHand") == 0 || thisFixUD->name().compare("RightHand") == 0)
            && thatFixUD->name().compare("Root") == 0)
                if(!_dieing) static_cast<Zorbit *>(thatCollidable.ptr())->hit(1);
    }
    if(thisFixUD->name().compare("LeftHand") == 0
       && thatType.compare("Planet") == 0)
    {
        //if(_leftHand->jumping())
        if(static_cast<ZorbitLevel*>(level())->levelTime().asSeconds() > 1)
        {
            level()->soundMan()->playSound("fb_hand_land", false);
            level()->camera()->shake(20);
        }
    }
    if(thisFixUD->name().compare("RightHand") == 0
       && thatType.compare("Planet") == 0)
    {
        //if(_rightHand->jumping())
        if(static_cast<ZorbitLevel*>(level())->levelTime().asSeconds() > 1)
            level()->soundMan()->playSound("fb_hand_land", false);
    }
    if(thatType.compare("ZShot") == 0
       && thisFixUD->name().compare("Head") == 0)
    {
        if(_dieing) return;
        static_cast<ZShot *>(thatCollidable.ptr())->toDie();
        decreaseHealth(static_cast<ZShot *>(thatCollidable.ptr())->damage());
        hit();

        if(level()->soundMan()->sound("fb_hit"))
            level()->soundMan()->playSound("fb_hit", true);
    }
    else if(thatType.compare("ZShot") == 0)
    {
        static_cast<ZShot *>(thatCollidable.ptr())->toDie();
        if(level()->soundMan()->sound("fb_fake_hit"))
            level()->soundMan()->playSound("fb_fake_hit", true);
    }
}

void ZFunzoBoss::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    ZBoss::endContact(contact, thisFixUD, thatCollidable, thatFixUD);

/*
    if((thisFixUD->name().compare("LeftHand") == 0
       && thatFixUD->name().compare("RightHand") == 0)
       ||(thisFixUD->name().compare("RightHand") == 0
       && thatFixUD->name().compare("LeftHand") == 0))
    {
        for (b2Fixture* f = body("FunzoLeftHand")->b2body()->GetFixtureList(); f; f = f->GetNext()){
            f->SetSensor(true);
        }
        for (b2Fixture* f = body("FunzoRightHand")->b2body()->GetFixtureList(); f; f = f->GetNext()){
            f->SetSensor(true);
        }
    }
    */
}

void ZFunzoBoss::updateConcrete()
{
    if(_currentHealth < 5)
    {
        _dieing = true;
        level()->soundMan()->stopMusic();
        level()->hudMan()->hud("BossHUD")->visibleIs(false);

        if(_chronos["dietimer"].getElapsedTime().asSeconds() > 0.1
           && _chronos["dietimer"].getElapsedTime().asSeconds() < 0.2)
        {
            level()->camera()->setTarget(sf::Vector2f(17934,9820) ,0);
        }
            if(_chronos["dietimer"].getElapsedTime().asSeconds() > 0.5
                && _chronos["dietimer"].getElapsedTime().asSeconds() < 0.6)
                {
                    body("FunzoHead")->sprite()->setTextureRect(sf::IntRect(801,801,800,800));
                    level()->soundMan()->playSound("fb_laugh_die", false);
                    stopHeadSpin();
                    actionMan()->clearActions();
                    actionMan()->pushAction(_knownActions["Stop"]);
                }
            if(_chronos["dietimer"].getElapsedTime().asSeconds() > 0.6
                && _chronos["dietimer"].getElapsedTime().asSeconds() < 0.7)
                body("FunzoHead")->sprite()->setTextureRect(sf::IntRect(0,801,800,800));
            if(_chronos["dietimer"].getElapsedTime().asSeconds() > 0.7
                && _chronos["dietimer"].getElapsedTime().asSeconds() < 0.8)
                body("FunzoHead")->sprite()->setTextureRect(sf::IntRect(801,801,800,800));
            if(_chronos["dietimer"].getElapsedTime().asSeconds() > 0.8
                && _chronos["dietimer"].getElapsedTime().asSeconds() < 1.1)
                body("FunzoHead")->sprite()->setTextureRect(sf::IntRect(0,801,800,800));
            if(_chronos["dietimer"].getElapsedTime().asSeconds() > 1.1
                && _chronos["dietimer"].getElapsedTime().asSeconds() < 1.7)
                body("FunzoHead")->sprite()->setTextureRect(sf::IntRect(801,801,800,800));
            if(_chronos["dietimer"].getElapsedTime().asSeconds() > 1.7
                && _chronos["dietimer"].getElapsedTime().asSeconds() < 2.3)
                {
                    _fading = true;
                    static_cast<Zorbit*>(level()->entityMan()->entity("Zorbit").ptr())->footContactsIs(0);
                    body("FunzoHead")->sprite()->setTextureRect(sf::IntRect(0,801,800,800));
                }
            if(_chronos["dietimer"].getElapsedTime().asSeconds() > 2.3
                && _chronos["dietimer"].getElapsedTime().asSeconds() < 2.9)
                body("FunzoHead")->sprite()->setTextureRect(sf::IntRect(801,801,800,800));
            if(_chronos["dietimer"].getElapsedTime().asSeconds() > 2.9
                && _chronos["dietimer"].getElapsedTime().asSeconds() < 3)
                {
                    body("FunzoHead")->sprite()->setTextureRect(sf::IntRect(0,801,800,800));
                }

        if(_chronos["dietimer"].getElapsedTime().asSeconds() > 3){
            static_cast<ZorbitLevel*>(level())->wholeLevelBeatenIs(true);
            _actionMan->pushAction(_knownActions["Die"]);
        }
    }
    else _chronos["dietimer"].reset(true);

    if(_fading
       && _chronos["fadetimer"].getElapsedTime().asMilliseconds() >= 20)
    {
        sf::Color color = rootBody()->sprite()->getColor();
        if((color.a > 0 && color.a <= 10) || color.a <=0)
           color.a = 10;

        rootBody()->sprite()->setColor(sf::Color(255, 255, 255, color.a - 10));
        body("FunzoHead")->sprite()->setColor(sf::Color(255, 255, 255, color.a - 10));
        _leftHand->_sprite.setColor(sf::Color(255, 255, 255, color.a - 10));
        _rightHand->_sprite.setColor(sf::Color(255, 255, 255, color.a - 10));

        // Stop fading.
        if(color.a - 10 <= 0)
        {
            static_cast<ZorbitLevel*>(level())->placePart(position(), cameraAngle());
            _fading = false;
        }

        _chronos["fadetimer"].reset(true);
    }

    if(_dieing) return;

    updateHands();

    if(!_active)
    {
        if(noActions())
            _actionMan->pushAction(_knownActions["Stop"]);

        _chronos["actiontimer"].reset(true);
    }
    else
    {
        if (gravityField() && _currentHealth > 0)
        {
            if(_currentHealth <= _maxHealth / 2)
                _wheelSpeed = 4;

            if(noActions())
                _actionMan->pushAction(_knownActions["Stop"]);

            // Boss logic.
            // LAUGH - This is a dumb way to do this and I don't care.
            if(_chronos["actiontimer"].getElapsedTime().asSeconds() > 0.5
                && _chronos["actiontimer"].getElapsedTime().asSeconds() < 0.6)
                {
                    body("FunzoHead")->sprite()->setTextureRect(sf::IntRect(801,801,800,800));
                    level()->soundMan()->playSound("fb_laugh", false);
                }
            if(_chronos["actiontimer"].getElapsedTime().asSeconds() > 0.6
                && _chronos["actiontimer"].getElapsedTime().asSeconds() < 0.7)
                body("FunzoHead")->sprite()->setTextureRect(sf::IntRect(0,801,800,800));
            if(_chronos["actiontimer"].getElapsedTime().asSeconds() > 0.7
                && _chronos["actiontimer"].getElapsedTime().asSeconds() < 0.8)
                body("FunzoHead")->sprite()->setTextureRect(sf::IntRect(801,801,800,800));
            if(_chronos["actiontimer"].getElapsedTime().asSeconds() > 0.8
                && _chronos["actiontimer"].getElapsedTime().asSeconds() < 0.9)
                body("FunzoHead")->sprite()->setTextureRect(sf::IntRect(0,801,800,800));
            if(_chronos["actiontimer"].getElapsedTime().asSeconds() > 0.9
                && _chronos["actiontimer"].getElapsedTime().asSeconds() < 1.0)
                body("FunzoHead")->sprite()->setTextureRect(sf::IntRect(801,801,800,800));
            if(_chronos["actiontimer"].getElapsedTime().asSeconds() > 1.0
                && _chronos["actiontimer"].getElapsedTime().asSeconds() < 1.1)
                body("FunzoHead")->sprite()->setTextureRect(sf::IntRect(0,801,800,800));
            if(_chronos["actiontimer"].getElapsedTime().asSeconds() > 1.1
                && _chronos["actiontimer"].getElapsedTime().asSeconds() < 1.2)
                body("FunzoHead")->sprite()->setTextureRect(sf::IntRect(801,801,800,800));
            if(_chronos["actiontimer"].getElapsedTime().asSeconds() > 1.2
                && _chronos["actiontimer"].getElapsedTime().asSeconds() < 1.3)
                body("FunzoHead")->sprite()->setTextureRect(sf::IntRect(0,801,800,800));
            if(_chronos["actiontimer"].getElapsedTime().asSeconds() > 1.3)
                body("FunzoHead")->sprite()->setTextureRect(sf::IntRect(0,0,800,800));

            // PAUSE
            if(_chronos["actiontimer"].getElapsedTime().asSeconds() < 3
                    && _actionMan->peekAction()->name().compare("Stop") != 0)
            {
                _actionMan->clearActions();
                _actionMan->pushAction(_knownActions["Stop"]);

            }
            // SPIN N SHOOT
            if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 3
               &&_chronos["actiontimer"].getElapsedTime().asSeconds() < 5.1
               && _actionMan->peekAction()->name().compare("SpinShoot") != 0)
            {
                    _actionMan->clearActions();
                    _actionMan->pushAction(_knownActions["SpinShoot"]);
            }
            // PAUSE
            if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 5.2
                    && _chronos["actiontimer"].getElapsedTime().asSeconds() < 6
                    && _actionMan->peekAction()->name().compare("Stop") != 0)
            {
                _actionMan->clearActions();
                stopHeadSpin();
                _actionMan->pushAction(_knownActions["Stop"]);
            }
            // SINGLE HAND JUMPS
            if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 6
                    && _chronos["actiontimer"].getElapsedTime().asSeconds() < 7)
                    //&& !_lHandJump
                    //&& !_rHandJump)
            {
                _leftHand->addAction(Smash, 1);
                _rightHand->addAction(Smash, 1);
            }
            // MULTI HAND JUMPS
            if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 8
                    && _chronos["actiontimer"].getElapsedTime().asSeconds() < 12) // 20
            {
                _actionMan->clearActions();
                _actionMan->pushAction(_knownActions["Stop"]);
                _leftHand->addAction(Move, 2);
                _rightHand->addAction(Move, 2);
                _leftHand->addAction(Smash, 1);
                _rightHand->addAction(Smash, 1);
                _leftHand->addAction(Move, 2);
                _rightHand->addAction(Move, 2);
                _leftHand->addAction(Smash, 1);
                _rightHand->addAction(Smash, 1);
                _leftHand->addAction(Move, 2);
                _rightHand->addAction(Move, 2);
                _leftHand->addAction(Smash, 1);
                _rightHand->addAction(Smash, 1);
                _leftHand->lockActions();
                _rightHand->lockActions();
            }
            // HANDS GO BACK
            if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 14
                    && _chronos["actiontimer"].getElapsedTime().asSeconds() < 14.5) // 20, 25
            {
                _leftHand->unlockActions();
                _rightHand->unlockActions();
            }
            // PAUSE
            if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 14.5
                    && _chronos["actiontimer"].getElapsedTime().asSeconds() < 15.5)
            {
                _leftHand->directedLeftIs(false);
                _rightHand->directedLeftIs(true);
                _leftHand->addAction(Move, 6);
                _rightHand->addAction(Move, 6);
                _leftHand->addAction(Stop, 4);
                _rightHand->addAction(Stop, 4);
                _leftHand->addAction(Clap,1);
                _rightHand->addAction(Clap,1);
                _leftHand->addAction(Stop, 2);
                _rightHand->addAction(Stop, 2);
                _leftHand->lockActions();
                _rightHand->lockActions();
            }
            if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 25)
            {
                _leftHand->drop(1);
                _rightHand->drop(1);
                _leftHand->directedLeftIs(true);
                _rightHand->directedLeftIs(false);
                _leftHand->unlockActions();
                _rightHand->unlockActions();
                switchWheel();

                _chronos["actiontimer"].reset(true);
           }
        }
    }
}

void ZFunzoBoss::updateHands()
{
    _leftHand->update();
    _rightHand->update();
    body("LeftHand")->b2body()->SetTransform(_leftHand->b2Position(),_leftHand->b2Angle());
    body("RightHand")->b2body()->SetTransform(_rightHand->b2Position(),_rightHand->b2Angle());
}

void ZFunzoBoss::draw()
{
    ZBoss::draw();

    level()->game()->renderWindow()->draw(_leftHand->sprite());
    level()->game()->renderWindow()->draw(_rightHand->sprite());
}
void ZFunzoBoss::switchWheel()
{
    if(_wheel.compare("left") == 0)
    {
        changeWheelSpeed(0, "left");
        changeWheelSpeed(_wheelSpeed, "right");
        _wheel = "right";
    }
    else
    {
        changeWheelSpeed(_wheelSpeed, "left");
        changeWheelSpeed(0, "right");
        _wheel = "left";
    }
}

void ZFunzoBoss::changeWheelSpeed(float value, std::string which)
{
    if(static_cast<Zorbit *>(level()->entityMan()->entity("Zorbit").ptr())->grappling())
        static_cast<Zorbit *>(level()->entityMan()->entity("Zorbit").ptr())->disconnectFromGrapple();
    if(which.compare("left") == 0)
        static_cast<ZCircle *>(level()->entityMan()->entity("CircleShape6").ptr())->changeSpinSpeed(value);
    else if(which.compare("right") == 0)
        static_cast<ZCircle *>(level()->entityMan()->entity("CircleShape8").ptr())->changeSpinSpeed(value);
    else if(which.compare("both") == 0)
    {
        static_cast<ZCircle *>(level()->entityMan()->entity("CircleShape6").ptr())->changeSpinSpeed(value);
        static_cast<ZCircle *>(level()->entityMan()->entity("CircleShape8").ptr())->changeSpinSpeed(value);
    }
}

void ZFunzoBoss::destroyJoints()
{
    level()->b2world()->DestroyJoint(_headJoint);

    // TODO - this should go somewhere else.
    delete _leftHand;
    delete _rightHand;
}

void ZFunzoBoss::spinHead(std::string direction, float speed)
{
    _headJoint->EnableMotor(true);

    if(direction.compare("clockwise") == 0)
        _headJoint->SetMotorSpeed(-1 * speed);
    else if(direction.compare("counterclockwise") == 0)
        _headJoint->SetMotorSpeed(speed);
}

void ZFunzoBoss::stopHeadSpin()
{
    body("FunzoHead")->sprite()->setTextureRect(sf::IntRect(0,0,800,800));
    _headJoint->EnableMotor(false);
}

void ZFunzoBoss::shoot()
{
    if(static_cast<Zorbit*>(level()->entityMan()->entity("Zorbit").ptr())->currentHealth() <= 0
       || static_cast<ZorbitLevel*>(level())->subTimer().asSeconds() <= 5)
        return;
    if(_chronos["shottimer"].getElapsedTime().asMilliseconds() > 70)
    {
        // generate shot name
        _shotName.str("");
        _shotName.clear();
        _shotName << "zFBShot" << _numShots;

        float angle = -body("FunzoHead")->b2body()->GetAngle() + jl::Tool::PI();
        FBShot::Ptr shot = new FBShot(_shotName.str(), static_cast<ZorbitLevel *>(level()), _layerNames, body("FunzoHead")->b2body()->GetPosition(), angle);
        level()->entityMan()->entityNew(shot->name(), "Layer1", shot);

        level()->soundMan()->playSound("fb_snot_shot", true);

        _numShots++;
        _chronos["shottimer"].reset(true);
    }
}

void ZFunzoBoss::print()
{
}

// FBSHOT METHODS

FBShot::FBShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float distance, std::string direction, float duration) : jl::Projectile(name,
            level, layerNames, direction, duration, "FBShot")
{
    throw fwk::InternalException("Should not be initializing FBShot this way");
}

FBShot::FBShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float angle, float duration) : jl::Projectile(name,
            level, layerNames, "Space", duration, "FBShot")
{
    jl::TextureDesc::Ptr tex;
    tex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/funzoboss/lazer.png",
            sf::Vector2f(100, 10), 200, 20);

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("FBShotBody", "foreground", tex, &bodyDef, initPos, jl::Tool::sfAngle(angle - (jl::Tool::PI() / 2)));
    rootBody()->b2body()->SetLinearDamping(0);

    _initialDistance = 0;
    _spaceAngle = angle;

    // Set values
    _maxSpeed       = 50;
    _damage         = 1;

    float width     = 0.25;
    float height    = 0.25;

    defineFixtures(width, height);
    defineAnimations();
    defineActions();
    defineSounds();

}

void FBShot::defineAnimations()
{
    _knownAnims["Move"] = new jl::Animation("Move", sf::IntRect(0, 0, 200, 20), 1, true);

    currentAnimIs(_knownAnims["Move"]);
}

void FBShot::defineActions()
{
    _knownActions["Move"] = new fbShotMoveAction(this);
}

void FBShot::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    //Projectile::beginContact(contact, thisFixUD, thatCollidable, thatFixUD);

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

void FBShot::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    Projectile::endContact(contact, thisFixUD, thatCollidable, thatFixUD);
}

void FBShot::updateConcrete()
{
    if(static_cast<Zorbit*>(level()->entityMan()->entity("Zorbit").ptr())->currentHealth() <= 0)
        die();
    if(noActions())
        _actionMan->pushAction(_knownActions["Move"]);
}
