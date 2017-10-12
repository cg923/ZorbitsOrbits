// Copyright (c) 2017 Happy Rock Studios

#ifndef ZCHECKPOINT_H
#define ZCHECKPOINT_H

#include "JollyLava/Event.h"

class ZorbitLevel;

class ZCheckPoint : public jl::PhysicalEvent
{
public:
    typedef fwk::Ptr<ZCheckPoint> Ptr;

    ZCheckPoint(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float initAngle, float width, float height)
                : PhysicalEvent(name, level, initPos, initAngle, width, height) {}

    void execute()
    {
        static_cast<ZorbitLevel *>(level())->setCheckPoint(name());
        finish();
    }
};
#endif
