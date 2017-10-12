
#include "ZGrappleChain.h"
#include "Zorbit.h"
#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "JollyLava/Tool.h"

GrappleChain::GrappleChain(jl::Entity * zorbit, jl::Level * level, std::vector<string> layerNames, b2Vec2 initPos): jl::Entity("GrappleChain",
            level, layerNames, "GrappleChain"), _zorbit(zorbit)
{
    _active = false;
    _linkCount = 5;

    // Body texture
    jl::TextureDesc::Ptr bodyTex;
    bodyTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/grapchain/grapplechain.png",
            sf::Vector2f(10, 10), 20, 20);

    //Create bodies
    b2BodyDef bodyDef;

    for(int iii = 1; iii <= _linkCount; iii++)
    {
        //update name of body
        std::stringstream ballName;
        ballName << "ball" << iii;

        //Add the body
        bodyNew(ballName.str(), "foreground", bodyTex, &bodyDef, initPos, 0, b2_staticBody);
    }

    defineAnimations();

    _chronos["visibilitytimer"] = _visibilityClock;
}

void GrappleChain::defineAnimations()
{
    _knownAnims["Stop"] = new jl::Animation("Stop", sf::IntRect(0, 0, 20, 20), 1, false);

    currentAnimIs(_knownAnims["Stop"]);
}

void GrappleChain::attach(jl::Entity * target)
{
    _grapplePoint = target;
    activeIs(true);
}

void GrappleChain::detach()
{
    _grapplePoint = NULL;
    activeIs(false);
}

void GrappleChain::updateConcrete()
{
    if(active())
    {
        b2Vec2 grapplePos = _grapplePoint->position();
        b2Vec2 zorbitPos;
        float distanceFactor = 0.25;
        float x;
        float y;

        zorbitPos = _zorbit->body("ZorbitGrapple")->b2body()->GetPosition();

        //update bodies
        for(int iii = 1; iii <= _linkCount; iii++)
        {
            //update name of body
            std::stringstream ballName;
            ballName << "ball" << iii;

            b2Vec2 pos;

            //get the initial position from grapplepoint if it's the first link
            if(iii == 1)
                pos = grapplePos;
            else{
                //get the position of the preceeding ball
                int prev = iii - 1;
                std::stringstream prevBallName;
                prevBallName << "ball" << prev;
                x = body(prevBallName.str())->b2body()->GetPosition().x;
                y = body(prevBallName.str())->b2body()->GetPosition().y;

                pos = b2Vec2(x + (distanceFactor * jl::Tool::distance(zorbitPos, grapplePos)) * cos(jl::Tool::thetaCoordinate(zorbitPos,grapplePos)) ,
                    y + (distanceFactor * jl::Tool::distance(zorbitPos, grapplePos)) * sin(jl::Tool::thetaCoordinate(zorbitPos, grapplePos)));
            }

            body(ballName.str())->b2body()->SetTransform(b2Vec2(pos.x,pos.y), _zorbit->angle());
        }
        //if(_grapplePoint) visibilityIs(true);
        if(_chronos["visibilitytimer"].getElapsedTime().asMilliseconds() > 50)
            visibilityIs(true);
    }
    else
    {
        _chronos["visibilitytimer"].reset(true);
        visibilityIs(false);
    }
}

