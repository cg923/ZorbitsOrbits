// Copyright (c) 2017 Happy Rock Studios

#ifndef SPACEGENERATOR_H
#define SPACEGENERATOR_H

#include <map>
#include <string>

// JollyLava
#include "fwk/Ptr.h"

class ZSpaceGenerator
{
public:
    typedef fwk::Ptr<ZSpaceGenerator> Ptr;

    ZSpaceGenerator(jl::State * level);

    void draw();

private:
    jl::State * _level;

    std::list<sf::Sprite> _sprites;

    jl::TextureDesc::Ptr _star1Tex;
    jl::TextureDesc::Ptr _star2Tex;
    jl::TextureDesc::Ptr _star3Tex;
    jl::TextureDesc::Ptr _star4Tex;
    jl::TextureDesc::Ptr _planet1Tex;
    jl::TextureDesc::Ptr _planet2Tex;
    jl::TextureDesc::Ptr _planet3Tex;
    jl::TextureDesc::Ptr _planet4Tex;
    jl::TextureDesc::Ptr _planet5Tex;
    jl::TextureDesc::Ptr _planet6Tex;
    jl::TextureDesc::Ptr _planet7Tex;
    jl::TextureDesc::Ptr _planet8Tex;
    jl::TextureDesc::Ptr _planet9Tex;
    jl::TextureDesc::Ptr _planet10Tex;
    jl::TextureDesc::Ptr _planet11Tex;
};

#endif // SPACEGENERATOR_H
