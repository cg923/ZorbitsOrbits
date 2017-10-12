// Copyright 2017 Happyrock Studios

#ifndef LOADING_SCREEN_H
#define LOADING_SCREEN_H

#include "Jollylava/Screen.h"

class ZorbitsOrbits;

enum LoadState
{
    notBegun,
    loading,
    finishedLoading
};

class LoadingScreen : public jl::Screen
{
public:
    LoadingScreen(ZorbitsOrbits * game);

    void initializeConcrete() {}

    void pauseConcrete() {}

    void unpauseConcrete() {}

    void processInput(sf::Event event) {}

    void levelNameIs(std::string levelName);

    void loadLevel(std::string levelName);

    void update();

    void update2();

    void draw();

    void cleanUp();

private:
    LoadState              _loaded;

    std::string             _levelName;
    std::string             _returnName;

    jl::TextureDesc::Ptr    _loadDesc;
    jl::TextureDesc::Ptr    _tutDesc;
    jl::TextureDesc::Ptr    _higDesc;
    jl::TextureDesc::Ptr    _wonDesc;
    jl::TextureDesc::Ptr    _vegDesc;
    jl::TextureDesc::Ptr    _junkDesc;
    jl::TextureDesc::Ptr    _jungDesc;
    jl::TextureDesc::Ptr    _robDesc;
    jl::TextureDesc::Ptr    _borderDesc;
    jl::TextureDesc::Ptr    _spaceDesc;

    sf::Sprite              _sprite;
    sf::Sprite              _borderSprite;
    sf::Sprite              _spaceSprite;
    sf::Text                _loadingText;
};

#endif // LOADING_SCREEN_H
