// Copyright 2017 Happy Rock Studios.

#ifndef ZORBIT_MENU_H
#define ZORBIT_MENU_H

// JollyLava
#include "JollyLava/MainMenu.h"
#include "JollyLava/Chronometer.h"

#include <list>

class ZorbitsOrbits;

enum Selection
{
    Play,
    LevelSelect,
    Settings,
    Creditss,
    Exit
};

enum SettingsSelection
{
    Difficulty,
    MusicVolume,
    SoundVolume,
    Controls,
    Accept,
    Discard,
    Defaults
};

enum KeyboardControls
{
    k_keyCon,
    k_MoveLeft,
    k_MoveRight,
    k_Shoot,
    k_Jump,
    k_Grapple,
    k_GrappleSetting,
    k_Accept,
    k_Discard,
    k_Defaults
};

enum ControllerControls
{
    c_keyCon,
    c_MoveSensitivity,
    c_Shoot,
    c_Jump,
    c_Grapple,
    c_GrappleSetting,
    c_Accept,
    c_Discard,
    c_Defaults
};

class ZorbitMenu : public jl::MainMenu
{
public:
    typedef fwk::Ptr<ZorbitMenu> Ptr;

    ZorbitMenu(ZorbitsOrbits * game, jl::TextureDesc::Ptr tex1, jl::TextureDesc::Ptr tex2);

    void processInput(sf::Event event);

    void initializeConcrete();

    void definePointerPositions();

    void nextSelection();

    void nextSettingsSelection();

    void nextControlsSelection();

    void previousSelection();

    void previousSettingsSelection();

    void previousControlsSelection();

    void confirmSelection();

    void selectionIs(Selection selection);

    void confirmSettingsSelection();

    void confirmControlsSelection();

    void update();

    void highlight(int selection);

    void colorSprite(std::string sprite, sf::Color color);

    void animate();

    void draw();

    void play();

    void resizeSprites();

    void applyCheat();

    void powerDown();

    void reactivate();

private:
    sftools::Chronometer    _effectTimer;
    bool                    _startingStoryMode;
    sf::Vector2f            _scale;

    jl::TextureDesc::Ptr    _blackDesc;
    sf::Sprite              _fadeSprite;
    bool                    _fading;
    bool                    _draw;

    jl::TextureDesc::Ptr    _selectionsDesc;
    sf::Sprite              _playSprite;
    sf::Sprite              _levelsSprite;
    sf::Sprite              _settingsSprite;
    sf::Sprite              _creditsSprite;
    sf::Sprite              _exitSprite;
    float                   _highlight;

    sf::Sprite              _bgSprite;
    jl::TextureDesc::Ptr    _bgDesc;

    sf::Sprite              _bg2Sprite;
    jl::TextureDesc::Ptr    _bg2Desc;

    bool                    _sfx;
    bool                    _canConfirm;
    bool                    _chooseDifficulty;

    sf::Text                _version;

    int                     _bounce;

    // FROG-E
    jl::TextureDesc::Ptr    _frogDesc;
    sf::Sprite              _frogSprite;
    sf::Text                _frogText;

    // SETTINGS
    jl::TextureDesc::Ptr    _settingsBGDesc;
    sf::Sprite              _settingsBGSprite;
    bool                    _drawSettings;
    bool                    _drawControls;
    bool                    _soundUp;
    bool                    _musicUp;
    bool                    _soundDown;
    bool                    _musicDown;

    sf::Vector2f            _settingsPos;
    float                   _settingsScale;

    jl::TextureDesc::Ptr    _sliderDesc;
    jl::TextureDesc::Ptr    _cursorDesc;
    sf::Sprite              _musicSliderSprite;
    sf::Sprite              _soundSliderSprite;
    sf::Sprite              _musicCursorSprite;
    sf::Sprite              _soundCursorSprite;

    SettingsSelection       _curSettingsSelection;

    sf::Text                MUSIC_VOLUME_TEXT;
    sf::Text                _currentMusicVolume;
    sf::Text                SOUND_VOLUME_TEXT;
    sf::Text                _currentSoundVolume;
    sf::Text                CUSTOMIZE_CONTROLS_TEXT;
    sf::Text                ACCEPT_TEXT;
    sf::Text                DISCARD_TEXT;
    sf::Text                DEFAULT_TEXT;
    sf::Text                _confirmText;
    sf::Sprite              _confirmSprite;
    sf::Text                _backText;
    sf::Sprite              _backSprite;

    // Controls
    int                     _nextX;
    int                     _previousX;
    bool                    _listening;
    std::string             _buttonToStore;
    std::string             _keyToStore;
    KeyboardControls        _curKeySelection;
    ControllerControls      _curConSelection;
    jl::TextureDesc::Ptr    _arrowDesc;
    jl::TextureDesc::Ptr    _buttonsDesc;
    sf::Text                KEYBOARD_CONTROLLER_TEXT;
    sf::Text                MOVE_SENSITIVITY_TEXT;
    sf::Text                _moveSense;
    sf::Text                MOVE_LEFT_TEXT;
    sf::Text                _moveLeft;
    sf::Text                MOVE_RIGHT_TEXT;
    sf::Text                _moveRight;
    sf::Text                SHOOT_TEXT;
    sf::Text                _shoot;
    sf::Text                JUMP_TEXT;
    sf::Text                _jump;
    sf::Text                GRAPPLE_TEXT;
    sf::Text                _grapple;
    sf::Text                GRAPPLE_SETTING_TEXT;
    sf::Text                _curGrappleSetting;
    sf::Sprite              _leftArrowSprite;
    sf::Sprite              _rightArrowSprite;
    sf::Sprite              _leftArrowSprite2;
    sf::Sprite              _rightArrowSprite2;
    sf::Sprite              _leftArrowSprite3;
    sf::Sprite              _rightArrowSprite3;
    sf::Sprite              _leftArrowSprite4;
    sf::Sprite              _rightArrowSprite4;
    sf::Sprite              _shootSprite;
    sf::Sprite              _jumpSprite;
    sf::Sprite              _grappleSprite;

    // Difficulty
    sf::Text                DIFFICULTY_TEXT;
    sf::Text                _curDifficulty;

    // Cheats
    bool                    _cheatListenerOn;
    std::list<char>         _cheatInput;

};

#endif // ZORBIT_MENU_H
