// Copyright (c) 2017 Happy Rock Studios

#include "Jollylava/Tool.h"
#include "ZBoss.h"
#include "BossHUD.h"
#include "ZorbitLevel.h"
#include "Zorbit.h"

ZBoss::ZBoss(std::string name, ZorbitLevel* level, std::vector<string> layerNames) : jl::Character(name,
            level, layerNames, "Boss"),
            _active(false),
            _seesZorbit(false),
            _attacking(false),
            _lFootContacts(0),
            _rFootContacts(0),
            _cameraMoved(false)
{
    _chronos["deathtimer"] = _deathTimer;
}

void ZBoss::activate()
{
    actionMan()->clearActions();
    _active = true;
       level()->hudMan()->hud("BossHUD")->visibleIs(true);
}

void ZBoss::reset()
{
    _active = false;
    level()->hudMan()->hud("BossHUD")->reset();
    actionMan()->clearActions();
    jl::Character::reset();
}

void ZBoss::hit()
{
    jl::Character::hit();
    static_cast<BossHUD*>(level()->hudMan()->hud("BossHUD").ptr())->updateHealth();
}

void ZBoss::handleDie()
{
    if(_currentHealth <= 0)
    {
        static_cast<Zorbit*>(level()->entityMan()->entity("Zorbit").ptr())->invincible();

        if(!_cameraMoved)
        {
            if(name().compare("GlobBoss") != 0
               && name().compare("TutorialBoss") != 0) static_cast<ZorbitLevel*>(level())->placePart(position(), cameraAngle());
            level()->soundMan()->stopMusic();
            level()->camera()->setTarget(jl::Tool::sfVec(position()),cameraAngle());
            if(level()->name().compare("vegas") != 0)
                static_cast<Zorbit*>(level()->entityMan()->entity("Zorbit").ptr())->footContactsMinusMinus();
            if(level()->name().compare("boss") == 0)
                static_cast<Zorbit*>(level()->entityMan()->entity("Zorbit").ptr())->footContactsIs(0);
            _cameraMoved = true;
        }
        _chronos["deathtimer"].resume();
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["Stop"]);

        prepDieConcrete();

        if(_chronos["deathtimer"].getElapsedTime().asSeconds() >= 1.0)
        {
            changeToDeathTexture();
            actionMan()->pushAction(_knownActions["Die"], "Top", true);

            std::stringstream soundName;
            soundName << _prefix << "_die";
            if(level()->soundMan()->sound(soundName.str()))
                level()->soundMan()->playSound(soundName.str());

            static_cast<ZorbitLevel*>(level())->wholeLevelBeatenIs(true);
        }
    }
    else _chronos["deathtimer"].reset(false);
}

void bDieAction::update(bool animFinished)
{
    if(!_finished)
    {
        actor()->level()->eventMan()->event("bossfinish")->execute();
        _finished = true;
    }
    if(animFinished)
    {
        actor()->die();
    }

    Action::update(animFinished);
}
