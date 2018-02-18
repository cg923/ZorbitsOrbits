// Copyright (c) 2017 Happy Rock Studios

#include "ZorbitMenu.h"
#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "LoadingScreen.h"
#include "ZSpaceGenerator.h"
#include "LevelSelect.h"
#include "Credits.h"

ZorbitMenu::ZorbitMenu(ZorbitsOrbits * game, jl::TextureDesc::Ptr tex1, jl::TextureDesc::Ptr tex2) :
    jl::MainMenu("mainmenu", game, tex1, tex2)
{
    _currentSelection   = Play;
    _curConSelection    = c_keyCon;
    _curKeySelection    = k_keyCon;
    _numSelections      = 5;
    _highlight          = 0;
    _sfx                = false;
    _canConfirm         = false;
    _drawSettings       = false;
    _drawControls       = false;
    _listening          = false;
    _soundUp            = false;
    _musicUp            = false;
    _soundDown          = false;
    _musicDown          = false;
    _startingStoryMode  = false;
    _fading             = false;
    _chooseDifficulty   = false;
    _draw               = true;
	_vsync				= true;
    _bounce             = 0;
    _nextX              = 0;
    _previousX          = 0;

    // Menu options
    _selectionsDesc = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/selections.png",
        sf::Vector2f(200, 150), 400, 300);

    // Backgrounds
    _bgDesc = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/background.png",
        sf::Vector2f(960, 540), 1920, 1080);
    _bg2Desc = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/background2.png",
        sf::Vector2f(960, 540), 1920, 1080);

    // Screen fade
    _blackDesc = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/black.png",
        sf::Vector2f(0, 0), 1, 1);

    // Sounds and music
    this->game()->soundMan()->soundNew("menubeep", "resources/sounds/menubeep.wav", false);
    this->game()->soundMan()->soundNew("menucrash", "resources/sounds/menucrash.wav", false);
    this->game()->soundMan()->soundNew("confirm", "resources/sounds/confirm.wav", false);
    this->game()->soundMan()->soundNew("confirm2", "resources/sounds/confirm2.wav", false);
    this->game()->soundMan()->soundNew("cancel", "resources/sounds/cancel.wav", false);
}

void ZorbitMenu::initializeConcrete()
{
    game()->soundMan()->playMusic("zorbit");

    _cheatListenerOn = false;

    float width  = (float)game()->videoMode().width / 1920;
    float height = (float)game()->videoMode().height / 1080;

    _scale = sf::Vector2f(width, height);

    // Screen fade
    _fadeSprite.setTexture(*_blackDesc->texture());
    _fadeSprite.setOrigin(_blackDesc->origin());
    _fadeSprite.setTextureRect(_blackDesc->rect());
    _fadeSprite.setPosition(0,0);
    _fadeSprite.scale(game()->videoMode().width, game()->videoMode().height);
    _fadeSprite.setColor(sf::Color(255,255,255,0));

    // Background
    _backgroundSprite.setTexture(*_backgroundDesc->texture());
    _backgroundSprite.setOrigin(_backgroundDesc->origin());
    _backgroundSprite.setTextureRect(_backgroundDesc->rect());
    _backgroundSprite.setPosition(this->game()->renderWindow()->getView().getCenter().x,
                                  this->game()->renderWindow()->getView().getCenter().y
                                    - this->game()->renderWindow()->getView().getSize().y /2);

    // Pointer
    _pointerSprite.setTexture(*_pointerDesc->texture());
    _pointerSprite.setOrigin(_pointerDesc->origin());
    _pointerSprite.setTextureRect(_pointerDesc->rect());
    _pointerSprite.setPosition(game()->renderWindow()->getView().getCenter());
    _pointerSprite.setColor(sf::Color(255,255,255,0));

    // Play
    _playSprite.setTexture(*_selectionsDesc->texture());
    _playSprite.setOrigin(_selectionsDesc->origin());
    _playSprite.setTextureRect(sf::IntRect(0, 0, 400, 59));
    _playSprite.setPosition(this->game()->renderWindow()->getView().getCenter().x,
                                  this->game()->renderWindow()->getView().getCenter().y
                                    + this->game()->renderWindow()->getView().getSize().y * 0.55);

    // Level Select
    _levelsSprite.setTexture(*_selectionsDesc->texture());
    _levelsSprite.setOrigin(_selectionsDesc->origin());
    _levelsSprite.setTextureRect(sf::IntRect(0, 60, 400, 59));
    _levelsSprite.setPosition(_playSprite.getPosition() + sf::Vector2f(0, _scale.y*60));

    // Settings
    _settingsSprite.setTexture(*_selectionsDesc->texture());
    _settingsSprite.setOrigin(_selectionsDesc->origin());
    _settingsSprite.setTextureRect(sf::IntRect(0, 120, 400, 59));
    _settingsSprite.setPosition(_playSprite.getPosition() + sf::Vector2f(0, _scale.y*120));

    // Credits
    _creditsSprite.setTexture(*_selectionsDesc->texture());
    _creditsSprite.setOrigin(_selectionsDesc->origin());
    _creditsSprite.setTextureRect(sf::IntRect(0, 180, 400, 59));
    _creditsSprite.setPosition(_playSprite.getPosition() + sf::Vector2f(0, _scale.y*180));

    // Exit
    _exitSprite.setTexture(*_selectionsDesc->texture());
    _exitSprite.setOrigin(_selectionsDesc->origin());
    _exitSprite.setTextureRect(sf::IntRect(0, 240, 400, 59));
    _exitSprite.setPosition(_playSprite.getPosition() + sf::Vector2f(0, _scale.y*240));

    _levelsSprite.setColor(sf::Color(255,255,255,128));
    _settingsSprite.setColor(sf::Color(255,255,255,128));
    _creditsSprite.setColor(sf::Color(255,255,255,128));
    _exitSprite.setColor(sf::Color(255,255,255,128));

    // background
    _bgSprite.setTexture(*_bgDesc->texture());
    _bgSprite.setOrigin(_bgDesc->origin());
    _bgSprite.setTextureRect(_bgDesc->rect());
    _bgSprite.setPosition(this->game()->renderWindow()->getView().getCenter());

    // background 2
    _bg2Sprite.setTexture(*_bg2Desc->texture());
    _bg2Sprite.setOrigin(_bg2Desc->origin());
    _bg2Sprite.setTextureRect(_bg2Desc->rect());
    _bg2Sprite.setPosition(this->game()->renderWindow()->getView().getCenter());

    // Version
    _version.setFont(*game()->font("arial"));
    _version.setCharacterSize(20);
    _version.setColor(sf::Color::White);
    _version.setPosition(20, 20);
    _version.setString("Version: 1.0.2");

    definePointerPositions();

    // SETTINGS setup
    _settingsBGDesc = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/settings.png",
        sf::Vector2f(400, 225), 800, 450);
    _settingsBGSprite.setTexture(*_settingsBGDesc->texture());
    _settingsBGSprite.setOrigin(_settingsBGDesc->origin());
    _settingsBGSprite.setPosition(this->game()->renderWindow()->getView().getCenter().x,
                                  this->game()->renderWindow()->getView().getCenter().y);
    _settingsPos = _settingsBGSprite.getPosition() - sf::Vector2f(_scale.x*180, _scale.y*150);

    // SETTINGS text
    _frogDesc = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/froggie2.png",
        sf::Vector2f(225, 172), 451, 343);
    _frogSprite.setTexture(*_frogDesc->texture());
    _frogSprite.setOrigin(_frogDesc->origin());
    _frogSprite.setPosition(this->game()->renderWindow()->getView().getCenter().x,
                                  this->game()->renderWindow()->getView().getCenter().y);

    _frogText.setFont(*game()->font("foo"));
    _frogText.setCharacterSize(30);
    _frogText.setColor(sf::Color::Black);
    _frogText.setPosition(_frogSprite.getPosition() + sf::Vector2f(_scale.x*-80, _scale.y*-150));
    _frogText.setString("Difficulty:");

    DIFFICULTY_TEXT.setFont(*game()->font("foo"));
    DIFFICULTY_TEXT.setCharacterSize(40);
    DIFFICULTY_TEXT.setColor(sf::Color::White);
    DIFFICULTY_TEXT.setPosition(_frogSprite.getPosition() + sf::Vector2f(_scale.x*-110, _scale.y*30));
    DIFFICULTY_TEXT.setString("Difficulty:");

    _curDifficulty.setFont(*game()->font("foo"));
    _curDifficulty.setCharacterSize(30);
    _curDifficulty.setColor(sf::Color::White);
    _curDifficulty.setPosition(DIFFICULTY_TEXT.getPosition() + sf::Vector2f(_scale.x*20, _scale.y*50));
    _curDifficulty.setString("Easy");

    MUSIC_VOLUME_TEXT.setFont(*game()->font("foo"));
    MUSIC_VOLUME_TEXT.setCharacterSize(40);
    MUSIC_VOLUME_TEXT.setColor(sf::Color(255,255,255,255));
    MUSIC_VOLUME_TEXT.setPosition(_settingsPos - sf::Vector2f(0, _scale.y*50));
    MUSIC_VOLUME_TEXT.setString("Music Volume:");

    _currentMusicVolume.setFont(*game()->font("foo"));
    _currentMusicVolume.setCharacterSize(30);
    _currentMusicVolume.setColor(sf::Color(255,255,255,80));
    _currentMusicVolume.setPosition(MUSIC_VOLUME_TEXT.getPosition() + sf::Vector2f(_scale.x*360, _scale.y*50));
    std::stringstream ss;
    ss << game()->musicVolume();
    _currentMusicVolume.setString(ss.str());

    SOUND_VOLUME_TEXT.setFont(*game()->font("foo"));
    SOUND_VOLUME_TEXT.setCharacterSize(40);
    SOUND_VOLUME_TEXT.setPosition(_settingsPos + sf::Vector2f(0, _scale.y*30));
    SOUND_VOLUME_TEXT.setString("Sound Volume:");

    _currentSoundVolume.setFont(*game()->font("foo"));
    _currentSoundVolume.setCharacterSize(30);
    _currentSoundVolume.setPosition(SOUND_VOLUME_TEXT.getPosition() + sf::Vector2f(_scale.x*360, _scale.y*50));
    std::stringstream ss2;
    ss2 << game()->soundVolume();
    _currentSoundVolume.setString(ss2.str());

	VSYNC_TEXT.setFont(*game()->font("foo"));
	VSYNC_TEXT.setCharacterSize(40);
	VSYNC_TEXT.setPosition(_settingsPos + sf::Vector2f(0, _scale.y * 120));
	VSYNC_TEXT.setString("VSync:");

	_vsyncOnOff.setFont(*game()->font("foo"));
	_vsyncOnOff.setCharacterSize(40);
	_vsyncOnOff.setPosition(VSYNC_TEXT.getPosition() + sf::Vector2f(_scale.x * 160, _scale.y * 40));
	if (game()->verticalSync())
		_vsyncOnOff.setString("On");
	else
		_vsyncOnOff.setString("Off");

    CUSTOMIZE_CONTROLS_TEXT.setFont(*game()->font("foo"));
    CUSTOMIZE_CONTROLS_TEXT.setCharacterSize(40);
    CUSTOMIZE_CONTROLS_TEXT.setPosition(_settingsPos + sf::Vector2f(0, _scale.y*220));
    CUSTOMIZE_CONTROLS_TEXT.setString("Customize Controls");

    ACCEPT_TEXT.setFont(*game()->font("foo"));
    ACCEPT_TEXT.setCharacterSize(40);
    ACCEPT_TEXT.setPosition(_settingsPos + sf::Vector2f(_scale.x*-100, _scale.y*280));
    ACCEPT_TEXT.setString("ACCEPT");

    DISCARD_TEXT.setFont(*game()->font("foo"));
    DISCARD_TEXT.setCharacterSize(40);
    DISCARD_TEXT.setPosition(ACCEPT_TEXT.getPosition() + sf::Vector2f(_scale.x*200, 0));
    DISCARD_TEXT.setString("DISCARD");

    DEFAULT_TEXT.setFont(*game()->font("foo"));
    DEFAULT_TEXT.setCharacterSize(40);
    DEFAULT_TEXT.setPosition(ACCEPT_TEXT.getPosition() + sf::Vector2f(_scale.x*410, 0));
    DEFAULT_TEXT.setString("RESET");

    _confirmText.setFont(*game()->font("foo"));
    _confirmText.setCharacterSize(40);
    _confirmText.setPosition(game()->renderWindow()->getView().getCenter() + sf::Vector2f(_scale.x*-160, _scale.y*210));
    _confirmText.setString("Confirm");

    _backText.setFont(*game()->font("foo"));
    _backText.setCharacterSize(40);
    _backText.setPosition(game()->renderWindow()->getView().getCenter() + sf::Vector2f(_scale.x*90, _scale.y*210));
    _backText.setString("Back");

    // SETTINGS sprites
    _sliderDesc = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/slider.png",
        sf::Vector2f(0, 0), 300, 20);
    _cursorDesc = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/cursor.png",
        sf::Vector2f(10, 15), 20, 30);

    _musicSliderSprite.setTexture(*_sliderDesc->texture());
    _soundSliderSprite.setTexture(*_sliderDesc->texture());
    _musicSliderSprite.setOrigin(_sliderDesc->origin());
    _soundSliderSprite.setOrigin(_sliderDesc->origin());
    _musicSliderSprite.setPosition(MUSIC_VOLUME_TEXT.getPosition().x + _scale.x*20,
                                    MUSIC_VOLUME_TEXT.getPosition().y + _scale.y*60);
    _soundSliderSprite.setPosition(SOUND_VOLUME_TEXT.getPosition().x + _scale.x*20,
                                    SOUND_VOLUME_TEXT.getPosition().y + _scale.y*60);

    _musicCursorSprite.setTexture(*_cursorDesc->texture());
    _soundCursorSprite.setTexture(*_cursorDesc->texture());
    _musicCursorSprite.setOrigin(_cursorDesc->origin());
    _soundCursorSprite.setOrigin(_cursorDesc->origin());
    _musicCursorSprite.setPosition(_musicSliderSprite.getPosition().x + _scale.x*20,
                                    _musicSliderSprite.getPosition().y + _scale.y*10);
    _soundCursorSprite.setPosition(_soundSliderSprite.getPosition().x + _scale.x*20,
                                    _soundSliderSprite.getPosition().y + _scale.y*10);

    _musicSliderSprite.setColor(sf::Color(255,255,255,255));
    _soundSliderSprite.setColor(sf::Color(255,255,255,80));
    _musicCursorSprite.setColor(sf::Color(255,255,255,255));
    _soundCursorSprite.setColor(sf::Color(255,255,255,80));
    _currentSoundVolume.setColor(sf::Color(255,255,255,80));
	VSYNC_TEXT.setColor(sf::Color(255, 255, 255, 80));
	_vsyncOnOff.setColor(sf::Color(255, 255, 255, 80));
    CUSTOMIZE_CONTROLS_TEXT.setColor(sf::Color(255,255,255,80));
    ACCEPT_TEXT.setColor(sf::Color(255,255,255,80));
    DISCARD_TEXT.setColor(sf::Color(255,255,255,80));
    DEFAULT_TEXT.setColor(sf::Color(255,255,255,80));
    SOUND_VOLUME_TEXT.setColor(sf::Color(255,255,255,80));

    _curSettingsSelection = MusicVolume;

    // Control settings
    KEYBOARD_CONTROLLER_TEXT.setFont(*game()->font("foo"));
    KEYBOARD_CONTROLLER_TEXT.setCharacterSize(40);
    KEYBOARD_CONTROLLER_TEXT.setColor(sf::Color::White);
    KEYBOARD_CONTROLLER_TEXT.setPosition(_settingsBGSprite.getPosition() - sf::Vector2f(_scale.x*80, _scale.y*200));
    if(sf::Joystick::isConnected(0))
        KEYBOARD_CONTROLLER_TEXT.setString("Controller");
    else
        KEYBOARD_CONTROLLER_TEXT.setString(" Keyboard");

    MOVE_SENSITIVITY_TEXT.setFont(*game()->font("foo"));
    MOVE_SENSITIVITY_TEXT.setCharacterSize(30);
    MOVE_SENSITIVITY_TEXT.setColor(sf::Color::White);
    MOVE_SENSITIVITY_TEXT.setPosition(KEYBOARD_CONTROLLER_TEXT.getPosition() + sf::Vector2f(_scale.x*-200, _scale.y*50));
    MOVE_SENSITIVITY_TEXT.setString("Joystick sensitivity: ");
    MOVE_SENSITIVITY_TEXT.setColor(sf::Color(255,255,255,80));

    MOVE_LEFT_TEXT.setFont(*game()->font("foo"));
    MOVE_LEFT_TEXT.setCharacterSize(30);
    MOVE_LEFT_TEXT.setColor(sf::Color::White);
    MOVE_LEFT_TEXT.setPosition(KEYBOARD_CONTROLLER_TEXT.getPosition() + sf::Vector2f(_scale.x*-200, _scale.y*50));
    MOVE_LEFT_TEXT.setString("Move left: ");
    MOVE_LEFT_TEXT.setColor(sf::Color(255,255,255,80));

    MOVE_RIGHT_TEXT.setFont(*game()->font("foo"));
    MOVE_RIGHT_TEXT.setCharacterSize(30);
    MOVE_RIGHT_TEXT.setColor(sf::Color::White);
    MOVE_RIGHT_TEXT.setPosition(KEYBOARD_CONTROLLER_TEXT.getPosition() + sf::Vector2f(_scale.x*-200, _scale.y*90));
    MOVE_RIGHT_TEXT.setString("Move right: ");
    MOVE_RIGHT_TEXT.setColor(sf::Color(255,255,255,80));

    SHOOT_TEXT.setFont(*game()->font("foo"));
    SHOOT_TEXT.setCharacterSize(30);
    SHOOT_TEXT.setColor(sf::Color::White);
    SHOOT_TEXT.setPosition(KEYBOARD_CONTROLLER_TEXT.getPosition() + sf::Vector2f(_scale.x*-200, _scale.y*130));
    SHOOT_TEXT.setString("Shoot: ");
    SHOOT_TEXT.setColor(sf::Color(255,255,255,80));

    JUMP_TEXT.setFont(*game()->font("foo"));
    JUMP_TEXT.setCharacterSize(30);
    JUMP_TEXT.setColor(sf::Color::White);
    JUMP_TEXT.setPosition(KEYBOARD_CONTROLLER_TEXT.getPosition() + sf::Vector2f(_scale.x*-200, _scale.y*170));
    JUMP_TEXT.setString("Jump / Ungrapple: ");
    JUMP_TEXT.setColor(sf::Color(255,255,255,80));

    GRAPPLE_TEXT.setFont(*game()->font("foo"));
    GRAPPLE_TEXT.setCharacterSize(30);
    GRAPPLE_TEXT.setColor(sf::Color::White);
    GRAPPLE_TEXT.setPosition(KEYBOARD_CONTROLLER_TEXT.getPosition() + sf::Vector2f(_scale.x*-200, _scale.y*210));
    GRAPPLE_TEXT.setString("Grapple: ");
    GRAPPLE_TEXT.setColor(sf::Color(255,255,255,80));

    GRAPPLE_SETTING_TEXT.setFont(*game()->font("foo"));
    GRAPPLE_SETTING_TEXT.setCharacterSize(30);
    GRAPPLE_SETTING_TEXT.setColor(sf::Color::White);
    GRAPPLE_SETTING_TEXT.setPosition(KEYBOARD_CONTROLLER_TEXT.getPosition() + sf::Vector2f(_scale.x*-40, _scale.y*250));
    GRAPPLE_SETTING_TEXT.setString("Grapple setting: ");
    GRAPPLE_SETTING_TEXT.setColor(sf::Color(255,255,255,80));

    _curGrappleSetting.setFont(*game()->font("foo"));
    _curGrappleSetting.setCharacterSize(30);
    _curGrappleSetting.setColor(sf::Color::White);
    _curGrappleSetting.setPosition(KEYBOARD_CONTROLLER_TEXT.getPosition() + sf::Vector2f(0, _scale.y*290));
    _curGrappleSetting.setString(game()->settings()->grappleSetting());
    _curGrappleSetting.setColor(sf::Color(255,255,255,80));

    _moveSense.setFont(*game()->font("foo"));
    _moveSense.setCharacterSize(30);
    _moveSense.setColor(sf::Color::White);
    _moveSense.setPosition(MOVE_SENSITIVITY_TEXT.getPosition() + sf::Vector2f(_scale.x*300, 0));
    _moveSense.setString("Medium");
    _moveSense.setColor(sf::Color(255,255,255,80));

    _moveLeft.setFont(*game()->font("foo"));
    _moveLeft.setCharacterSize(30);
    _moveLeft.setColor(sf::Color::White);
    _moveLeft.setPosition(MOVE_LEFT_TEXT.getPosition() + sf::Vector2f(_scale.x*300, 0));
    _moveLeft.setString(game()->settings()->keyAsString("Left"));
    _moveLeft.setColor(sf::Color(255,255,255,80));

    _moveRight.setFont(*game()->font("foo"));
    _moveRight.setCharacterSize(30);
    _moveRight.setColor(sf::Color::White);
    _moveRight.setPosition(MOVE_RIGHT_TEXT.getPosition() + sf::Vector2f(_scale.x*300, 0));
    _moveRight.setString(game()->settings()->keyAsString("Right"));
    _moveRight.setColor(sf::Color(255,255,255,80));

    _shoot.setFont(*game()->font("foo"));
    _shoot.setCharacterSize(30);
    _shoot.setColor(sf::Color::White);
    _shoot.setPosition(SHOOT_TEXT.getPosition() + sf::Vector2f(_scale.x*300, 0));
    _shoot.setString(game()->settings()->keyAsString("Shoot"));
    _shoot.setColor(sf::Color(255,255,255,80));

    _jump.setFont(*game()->font("foo"));
    _jump.setCharacterSize(30);
    _jump.setColor(sf::Color::White);
    _jump.setPosition(JUMP_TEXT.getPosition() + sf::Vector2f(_scale.x*300, 0));
    _jump.setString(game()->settings()->keyAsString("Jump"));
    _jump.setColor(sf::Color(255,255,255,80));

    _grapple.setFont(*game()->font("foo"));
    _grapple.setCharacterSize(30);
    _grapple.setColor(sf::Color::White);
    _grapple.setPosition(GRAPPLE_TEXT.getPosition() + sf::Vector2f(_scale.x*300, 0));
    _grapple.setString(game()->settings()->keyAsString("Grapple"));
    _grapple.setColor(sf::Color(255,255,255,80));

    _arrowDesc = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/arrow.png",
        sf::Vector2f(10, 15), 20, 30);

    _leftArrowSprite.setTexture(*_arrowDesc->texture());
    _leftArrowSprite2.setTexture(*_arrowDesc->texture());
    _leftArrowSprite3.setTexture(*_arrowDesc->texture());
    _leftArrowSprite4.setTexture(*_arrowDesc->texture());
	_leftArrowSprite5.setTexture(*_arrowDesc->texture());
    _rightArrowSprite.setTexture(*_arrowDesc->texture());
    _rightArrowSprite2.setTexture(*_arrowDesc->texture());
    _rightArrowSprite3.setTexture(*_arrowDesc->texture());
    _rightArrowSprite4.setTexture(*_arrowDesc->texture());
	_rightArrowSprite5.setTexture(*_arrowDesc->texture());
    _leftArrowSprite.setOrigin(_arrowDesc->origin());
    _leftArrowSprite2.setOrigin(_arrowDesc->origin());
    _leftArrowSprite3.setOrigin(_arrowDesc->origin());
    _leftArrowSprite4.setOrigin(_arrowDesc->origin());
	_leftArrowSprite5.setOrigin(_arrowDesc->origin());
    _rightArrowSprite.setOrigin(_arrowDesc->origin());
    _rightArrowSprite2.setOrigin(_arrowDesc->origin());
    _rightArrowSprite3.setOrigin(_arrowDesc->origin());
    _rightArrowSprite4.setOrigin(_arrowDesc->origin());
	_rightArrowSprite5.setOrigin(_arrowDesc->origin());
    _leftArrowSprite.setPosition(KEYBOARD_CONTROLLER_TEXT.getPosition().x - _scale.x*20,
                                    KEYBOARD_CONTROLLER_TEXT.getPosition().y + _scale.y*30);
    _rightArrowSprite.setPosition(KEYBOARD_CONTROLLER_TEXT.getPosition().x + _scale.x*220,
                                    KEYBOARD_CONTROLLER_TEXT.getPosition().y + _scale.y*30);
    _leftArrowSprite2.setPosition(_curGrappleSetting.getPosition().x - _scale.x*20,
                                    _curGrappleSetting.getPosition().y + _scale.y*20);
    _rightArrowSprite2.setPosition(_curGrappleSetting.getPosition().x + _scale.x*190,
                                    _curGrappleSetting.getPosition().y + _scale.y*20);
    _leftArrowSprite3.setPosition(_moveSense.getPosition().x - _scale.x*20,
                                    _moveSense.getPosition().y + _scale.y*20);
    _rightArrowSprite3.setPosition(_moveSense.getPosition().x + _scale.x*190,
                                    _moveSense.getPosition().y + _scale.y*20);
    _leftArrowSprite4.setPosition(_curDifficulty.getPosition().x - _scale.x*20,
                                    _curDifficulty.getPosition().y + _scale.y*20);
    _rightArrowSprite4.setPosition(_curDifficulty.getPosition().x + _scale.x*190,
                                    _curDifficulty.getPosition().y + _scale.y*20);
	_leftArrowSprite5.setPosition(_vsyncOnOff.getPosition().x - _scale.x * 20,
		_vsyncOnOff.getPosition().y + _scale.y * 30);
	_rightArrowSprite5.setPosition(_vsyncOnOff.getPosition().x + _scale.x * 80,
		_vsyncOnOff.getPosition().y + _scale.y * 30);
    _leftArrowSprite3.setColor(sf::Color(255,255,255,80));
    _rightArrowSprite3.setColor(sf::Color(255,255,255,80));
	_leftArrowSprite5.setColor(sf::Color(255, 255, 255, 80));
	_rightArrowSprite5.setColor(sf::Color(255, 255, 255, 80));
    _leftArrowSprite.setScale(-1,1);
    _leftArrowSprite2.setScale(-1,1);
    _leftArrowSprite3.setScale(-1,1);
    _leftArrowSprite4.setScale(-1,1);
	_leftArrowSprite5.setScale(-1, 1);

    _buttonsDesc = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/buttons.png",
        sf::Vector2f(100, 25), 300, 50);
    _shootSprite.setTexture(*_buttonsDesc->texture());
    _shootSprite.setOrigin(0, 0);
    _shootSprite.setColor(sf::Color(255,255,255,80));
    _jumpSprite.setTexture(*_buttonsDesc->texture());
    _jumpSprite.setOrigin(0, 0);
    _jumpSprite.setColor(sf::Color(255,255,255,80));
    _grappleSprite.setTexture(*_buttonsDesc->texture());
    _grappleSprite.setOrigin(0, 0);
    _grappleSprite.setColor(sf::Color(255,255,255,80));

    _confirmSprite.setTexture(*_buttonsDesc->texture());
    _confirmSprite.setOrigin(0, 0);
    _confirmSprite.setTextureRect(sf::IntRect(0,0,50,50));
    _confirmSprite.setPosition(_confirmText.getPosition() - sf::Vector2f(_scale.x*50, 0));

    _backSprite.setTexture(*_buttonsDesc->texture());
    _backSprite.setOrigin(0, 0);
    _backSprite.setTextureRect(sf::IntRect(50,0,50,50));
    _backSprite.setPosition(_backText.getPosition() - sf::Vector2f(_scale.x*50, 0));

    resizeSprites();
}

void ZorbitMenu::definePointerPositions()
{
    _positions[Play]            = _playSprite.getPosition();
    _positions[LevelSelect]     = _levelsSprite.getPosition();
    _positions[Settings]        = _settingsSprite.getPosition();
    _positions[Creditss]         = _creditsSprite.getPosition();
    _positions[Exit]            = _exitSprite.getPosition();
}

void ZorbitMenu::applyCheat()
{
    _cheatListenerOn = false;
    std::string code = "";

    std::list<char>::iterator itr;
    for(itr = _cheatInput.begin(); itr != _cheatInput.end(); itr++)
    {
        code += (*itr);
    }
    if (code.compare("uuddlrlrba") == 0)
    {
        game()->soundMan()->playSound("slot_activate");
        static_cast<ZorbitsOrbits*>(game())->_godMode = true;
    }
    else if(code.compare("ldlrdrllrr") == 0)
    {
        game()->soundMan()->playSound("slot_activate");
        static_cast<ZorbitsOrbits*>(game())->_sillyMode = true;
    }
    else if(code.compare("gofast") == 0)
    {
        game()->soundMan()->playSound("slot_activate");
        static_cast<ZorbitsOrbits*>(game())->_fastMode = true;
    }
    else if(code.compare("homegoal") == 0)
    {
        game()->soundMan()->playSound("slot_activate");
        static_cast<LevelSelectScreen*>(game()->stateMan()->state("LevelSelect").ptr())->unlockAllLevels();
    }
    else {
            std::cout << code << std::endl;
    game()->soundMan()->playSound("cancel");}

    _cheatInput.clear();
}

void ZorbitMenu::processInput(sf::Event event)
{
    if(_cheatListenerOn)
    {
        // Keyboard
        if (event.type == sf::Event::KeyPressed)
        {
            switch(event.key.code)
            {
                case sf::Keyboard::Return:
                    applyCheat();
                    break;
                case sf::Keyboard::Left:
                    _cheatInput.push_back('l');
                    break;
                case sf::Keyboard::Right:
                    _cheatInput.push_back('r');
                    break;
                case sf::Keyboard::Up:
                    _cheatInput.push_back('u');
                    break;
                case sf::Keyboard::Down:
                    _cheatInput.push_back('d');
                    break;
                case sf::Keyboard::B:
                    _cheatInput.push_back('b');
                    break;
                case sf::Keyboard::A:
                    _cheatInput.push_back('a');
                    break;
                case sf::Keyboard::F:
                    _cheatInput.push_back('f');
                    break;
                case sf::Keyboard::S:
                    _cheatInput.push_back('s');
                    break;
                case sf::Keyboard::T:
                    _cheatInput.push_back('t');
                    break;
                case sf::Keyboard::G:
                    _cheatInput.push_back('g');
                    break;
                case sf::Keyboard::O:
                    _cheatInput.push_back('o');
                    break;
                case sf::Keyboard::H:
                    _cheatInput.push_back('h');
                    break;
                case sf::Keyboard::M:
                    _cheatInput.push_back('m');
                    break;
                case sf::Keyboard::E:
                    _cheatInput.push_back('e');
                    break;
                case sf::Keyboard::L:
                    _cheatInput.push_back('l');
                    break;
                default:
                    break;
            }
        }
        return;
    }

    if(_chooseDifficulty)
    {
        // Keyboard
        if (event.type == sf::Event::KeyPressed)
        {
            switch(event.key.code)
            {
                case sf::Keyboard::Right:
                    game()->soundMan()->playSound("menubeep");
                    game()->settings()->nextDifficulty();
                    break;
                case sf::Keyboard::Left:
                    game()->soundMan()->playSound("menubeep");
                    game()->settings()->prevDifficulty();
                    break;
                case sf::Keyboard::Escape:
                    _chooseDifficulty = false;
                    game()->soundMan()->playSound("cancel");
                    break;
                case sf::Keyboard::Return:
                    if(_startingStoryMode) break;
                    game()->soundMan()->stopMusic("zorbit");
                    game()->soundMan()->playSound("confirm");
                    _effectTimer.reset(true);
                    _fadeSprite.setColor(sf::Color(255,255,255,0));
                    _startingStoryMode = true;
                    _fading = true;
                    break;
                default:
                    break;
            }
        }
        if (event.type == sf::Event::JoystickMoved)
        {
            // Y Axis
            if (event.joystickMove.axis == sf::Joystick::X)
            {
                if (event.joystickMove.position > 70)
                {
                    if(_nextX == 0)
                    {
                        game()->soundMan()->playSound("menubeep");
                        game()->settings()->nextDifficulty();
                    }
                    _nextX++;
                }
                if (event.joystickMove.position < -70)
                {
                    if(_previousX == 0)
                    {
                        game()->soundMan()->playSound("menubeep");
                        game()->settings()->prevDifficulty();
                    }
                    _previousX++;
                }
                if (event.joystickMove.position < 40)
                {
                    _nextX = 0;
                }
                if (event.joystickMove.position > -40)
                {
                    _previousX = 0;
                }
            }
        }
        if (event.type == sf::Event::JoystickButtonPressed)
        {
            if(event.joystickButton.button == 0)
            {
                if(!_startingStoryMode)
                {
                    game()->soundMan()->stopMusic("zorbit");
                    game()->soundMan()->playSound("confirm");
                    _effectTimer.reset(true);
                    _fadeSprite.setColor(sf::Color(255,255,255,0));
                    _startingStoryMode = true;
                    _fading = true;
                }
            }
            if(event.joystickButton.button == 1)
            {
                game()->soundMan()->playSound("cancel");
                _chooseDifficulty = false;
            }
        }
    }
    else if(!_drawSettings)
        MainMenu::processInput(event);
    else if (!_drawControls)
    {
        // Keyboard
        if (event.type == sf::Event::KeyPressed)
        {
            switch(event.key.code)
            {
                case sf::Keyboard::Up:
                    previousSettingsSelection();
                    break;
                case sf::Keyboard::Down:
                    nextSettingsSelection();
                    break;
                case sf::Keyboard::Right:
                {
                    if (_curSettingsSelection == Accept || _curSettingsSelection == Discard || _curSettingsSelection == Defaults)
                        nextSettingsSelection();
                    if (_curSettingsSelection == MusicVolume && game()->musicVolume() < 100)
                        game()->setMusicVolume(game()->musicVolume() + 1);
                    if (_curSettingsSelection == SoundVolume && game()->soundVolume() < 100)
                    {
                        game()->setSoundVolume(game()->soundVolume() + 1);
                        game()->soundMan()->playSound("menubeep");
                    }
					if (_curSettingsSelection == Vsync)
					{
						game()->verticalSyncIs(!game()->verticalSync());
						if (game()->verticalSync()) _vsyncOnOff.setString("On");
						else _vsyncOnOff.setString("Off");
					}
                    break;
                }
                case sf::Keyboard::Left:
                {
                    if (_curSettingsSelection == Accept || _curSettingsSelection == Discard || _curSettingsSelection == Defaults)
                        previousSettingsSelection();
                    if (_curSettingsSelection == MusicVolume && game()->musicVolume() > 0)
                        game()->setMusicVolume(game()->musicVolume() - 1);
                    if (_curSettingsSelection == SoundVolume && game()->soundVolume() > 0)
                    {
                        game()->setSoundVolume(game()->soundVolume() - 1);
                        game()->soundMan()->playSound("menubeep");
                    }
					if (_curSettingsSelection == Vsync)
					{
						game()->verticalSyncIs(!game()->verticalSync());
						if (game()->verticalSync()) _vsyncOnOff.setString("On");
						else _vsyncOnOff.setString("Off");
					}
                    break;
                }
                case sf::Keyboard::Return:
                    confirmSettingsSelection();
                    break;
                case sf::Keyboard::Escape:
                    _drawSettings = false;
                    game()->soundMan()->playSound("cancel");
                    game()->settings()->loadSettings();
                    game()->setMusicVolume(game()->settings()->musicVolume());
                    game()->setSoundVolume(game()->settings()->soundVolume());
                    break;
                case sf::Keyboard::R:
                    game()->soundMan()->playSound("slot_whir");
                    _cheatListenerOn = true;
                    break;
                default:
                    break;
            }
        }
        // Joystick
        if (event.type == sf::Event::JoystickMoved)
        {
            // Y Axis
            if (event.joystickMove.axis == sf::Joystick::Y)
            {
                if (event.joystickMove.position > 70)
                {
                    if(_next == 0)
                    {
                        nextSettingsSelection();
                    }
                    _next++;
                }
                if (event.joystickMove.position < -70)
                {
                    if(_previous == 0)
                    {
                        previousSettingsSelection();
                    }
                    _previous++;
                }
                if (event.joystickMove.position < 40)
                {
                    _next = 0;
                }
                if (event.joystickMove.position > -40)
                {
                    _previous = 0;
                }
            }
            // X Axis
            if (event.joystickMove.axis == sf::Joystick::X)
            {
                if (event.joystickMove.position > 70)
                {
                    if(_nextX == 0)
                    {
                        //if (_curSettingsSelection == Difficulty
                        //    && static_cast<ZorbitsOrbits*>(game())->_currentLevel == uninitialized)
                        //    game()->settings()->nextDifficulty();
                        if (_curSettingsSelection == Accept || _curSettingsSelection == Discard || _curSettingsSelection == Defaults)
                            nextSettingsSelection();
                        if (_curSettingsSelection == MusicVolume && game()->musicVolume() < 100)
                            _musicUp = true;
                        if (_curSettingsSelection == SoundVolume && game()->soundVolume() < 100)
                            _soundUp = true;
                    }
                    _nextX++;
                }
                if (event.joystickMove.position < -70)
                {
                    if(_previousX == 0)
                    {
                        //if (_curSettingsSelection == Difficulty
                        //    && static_cast<ZorbitsOrbits*>(game())->_currentLevel == uninitialized)
                        //    game()->settings()->prevDifficulty();
                        if (_curSettingsSelection == Accept || _curSettingsSelection == Discard || _curSettingsSelection == Defaults)
                            previousSettingsSelection();
                        if (_curSettingsSelection == MusicVolume && game()->musicVolume() > 0)
                            _musicDown = true;
                        if (_curSettingsSelection == SoundVolume && game()->soundVolume() > 0)
                            _soundDown = true;
                    }
                    _previousX++;
                }
                if (event.joystickMove.position < 40)
                {
                    _nextX = 0;
                    _soundUp = false;
                    _musicUp = false;
                }
                if (event.joystickMove.position > -40)
                {
                    _previousX = 0;
                    _soundDown = false;
                    _musicDown = false;
                }
            }
        }
        if (event.type == sf::Event::JoystickButtonPressed)
        {
            if(event.joystickButton.button == 0)
            {
                confirmSettingsSelection();
            }
            if(event.joystickButton.button == 1)
            {
                _drawSettings = false;
                game()->soundMan()->playSound("cancel");
                game()->settings()->loadSettings();
                game()->setMusicVolume(game()->settings()->musicVolume());
                game()->setSoundVolume(game()->settings()->soundVolume());
            }
        }
    }
    else if(_drawControls && !_listening)
    {
        if (event.type == sf::Event::KeyPressed)
        {
            switch (event.key.code)
            {
                case sf::Keyboard::Right:
                {
                    if (_curConSelection == c_keyCon
                        && KEYBOARD_CONTROLLER_TEXT.getString().toAnsiString().compare("Controller") == 0)
                        {
                            KEYBOARD_CONTROLLER_TEXT.setString(" Keyboard");
                        }

                    else if (_curKeySelection == k_keyCon
                            && KEYBOARD_CONTROLLER_TEXT.getString().toAnsiString().compare(" Keyboard") == 0)
                        {
                            KEYBOARD_CONTROLLER_TEXT.setString("Controller");
                        }
                    else if (_curKeySelection == k_Accept
                             || _curKeySelection == k_Discard
                             || _curKeySelection == k_Defaults)
                        nextControlsSelection();
                    else if (_curConSelection == c_Accept
                             || _curConSelection == c_Discard
                             || _curConSelection == c_Defaults)
                        nextControlsSelection();
                    else if (_curConSelection == c_MoveSensitivity)
                        game()->settings()->nextXSensitivity();
                    else if (_curConSelection == c_GrappleSetting
                             || _curKeySelection == k_GrappleSetting)
                        game()->settings()->switchGrappleSetting();
                    break;
                }
                case sf::Keyboard::Left:
                {
                    if (_curConSelection == c_keyCon
                        && KEYBOARD_CONTROLLER_TEXT.getString().toAnsiString().compare("Controller") == 0)
                    {
                        KEYBOARD_CONTROLLER_TEXT.setString(" Keyboard");
                    }
                    else if (_curKeySelection == k_keyCon
                            && KEYBOARD_CONTROLLER_TEXT.getString().toAnsiString().compare(" Keyboard") == 0)
                    {
                        KEYBOARD_CONTROLLER_TEXT.setString("Controller");
                    }
                    else if (_curKeySelection == k_Accept
                             || _curKeySelection == k_Discard
                             || _curKeySelection == k_Defaults)
                        previousControlsSelection();
                    else if (_curConSelection == c_Accept
                             || _curConSelection == c_Discard
                             || _curConSelection == c_Defaults)
                        previousControlsSelection();
                    else if (_curConSelection == c_MoveSensitivity)
                        game()->settings()->previousXSensitivity();
                    else if (_curConSelection == c_GrappleSetting
                             || _curKeySelection == k_GrappleSetting)
                        game()->settings()->switchGrappleSetting();
                    break;
                }
                case sf::Keyboard::Down:
                    nextControlsSelection();
                    break;
                case sf::Keyboard::Up:
                    previousControlsSelection();
                    break;
                case sf::Keyboard::Return:
                    confirmControlsSelection();
                    break;
                case sf::Keyboard::Escape:
                    _drawControls = false;
                    game()->soundMan()->playSound("cancel");
                    game()->settings()->loadControlsOnly();
                    break;
                default:
                    break;
            }
        }
        // Joystick
        if (event.type == sf::Event::JoystickMoved)
        {
            // Y Axis
            if (event.joystickMove.axis == sf::Joystick::Y)
            {
                if (event.joystickMove.position > 70)
                {
                    if(_next == 0)
                    {
                        nextControlsSelection();
                    }
                    _next++;
                }
                if (event.joystickMove.position < -70)
                {
                    if(_previous == 0)
                    {
                        previousControlsSelection();
                    }
                    _previous++;
                }
                if (event.joystickMove.position < 40)
                {
                    _next = 0;
                }
                if (event.joystickMove.position > -40)
                {
                    _previous = 0;
                }
            }
            if (event.joystickMove.axis == sf::Joystick::X)
            {
                if (event.joystickMove.position > 70)
                {
                    if(_nextX == 0)
                    {
                        if (_curConSelection == c_Accept
                             || _curConSelection == c_Discard
                             || _curConSelection == c_Defaults)
                            nextControlsSelection();
                        else if (_curKeySelection == k_Accept
                             || _curKeySelection == k_Discard
                             || _curKeySelection == k_Defaults)
                            nextControlsSelection();
                        else if (_curConSelection == c_keyCon
                            && KEYBOARD_CONTROLLER_TEXT.getString().toAnsiString().compare("Controller") == 0)
                        {
                            KEYBOARD_CONTROLLER_TEXT.setString(" Keyboard");
                        }
                        else if (_curKeySelection == k_keyCon
                            && KEYBOARD_CONTROLLER_TEXT.getString().toAnsiString().compare(" Keyboard") == 0)
                        {
                            KEYBOARD_CONTROLLER_TEXT.setString("Controller");
                        }

                        else if (_curConSelection == c_MoveSensitivity)
                            game()->settings()->nextXSensitivity();
                        else if (_curConSelection == c_GrappleSetting
                             || _curKeySelection == k_GrappleSetting)
                        game()->settings()->switchGrappleSetting();
                    }
                    _nextX++;
                }
                if (event.joystickMove.position < -70)
                {
                    if(_previousX == 0)
                    {
                        if (_curConSelection == c_Accept
                             || _curConSelection == c_Discard
                             || _curConSelection == c_Defaults)
                            previousControlsSelection();
                        else if (_curKeySelection == k_Accept
                             || _curKeySelection == k_Discard
                             || _curKeySelection == k_Defaults)
                            previousControlsSelection();
                        else if (_curConSelection == c_keyCon
                            && KEYBOARD_CONTROLLER_TEXT.getString().toAnsiString().compare("Controller") == 0)
                        {
                            KEYBOARD_CONTROLLER_TEXT.setString(" Keyboard");
                        }
                        else if (_curConSelection == c_keyCon
                            && KEYBOARD_CONTROLLER_TEXT.getString().toAnsiString().compare(" Keyboard") == 0)
                        {
                            KEYBOARD_CONTROLLER_TEXT.setString("Controller");
                        }

                        else if (_curConSelection == c_MoveSensitivity)
                            game()->settings()->previousXSensitivity();
                        else if (_curConSelection == c_GrappleSetting
                             || _curKeySelection == k_GrappleSetting)
                        game()->settings()->switchGrappleSetting();
                    }
                    _previousX++;
                }
                if (event.joystickMove.position < 40)
                {
                    _nextX = 0;
                }
                if (event.joystickMove.position > -40)
                {
                    _previousX = 0;
                }
            }
        }
        if (event.type == sf::Event::JoystickButtonReleased)
        {
            if(event.joystickButton.button == 0)
            {
                confirmControlsSelection();
            }
            if(event.joystickButton.button == 1)
            {
                _drawControls = false;
                game()->soundMan()->playSound("cancel");
                game()->settings()->loadControlsOnly();
            }
        }
    }
    else if (_drawControls && _listening)
    {
        if (event.type == sf::Event::KeyPressed)
        {
            if(event.key.code == sf::Keyboard::Return)
            {
                _keyToStore = "";
                _buttonToStore = "";
                _listening = false;
            }
            else if(KEYBOARD_CONTROLLER_TEXT.getString().toAnsiString().compare("Controller") != 0)
            {
                game()->settings()->keyIs(_keyToStore, event.key.code);
                _keyToStore = "";
                _listening = false;
            }
        }
        if (event.type == sf::Event::JoystickButtonReleased)
        {
            if(KEYBOARD_CONTROLLER_TEXT.getString().toAnsiString().compare("Controller") == 0)
            {
                _listening = false;
                game()->settings()->buttonIs(_buttonToStore, event.joystickButton.button);
                _buttonToStore = "";
            }
        }
    }
}

void ZorbitMenu::nextSelection()
{
    MainMenu::nextSelection();

    game()->soundMan()->playSound("menubeep");
    //highlight(_currentSelection);
    _bounce = 0;

    switch(_currentSelection)
    {
    case Play:
        _playSprite.setColor(sf::Color(255,255,255,255));
        break;
    case Settings:
        _settingsSprite.setColor(sf::Color(255,255,255,255));
        break;
    case LevelSelect:
        _levelsSprite.setColor(sf::Color(255,255,255,255));
        break;
    case Creditss:
        _creditsSprite.setColor(sf::Color(255,255,255,255));
        break;
    case Exit:
        _exitSprite.setColor(sf::Color(255,255,255,255));
        break;
    default:
        break;
    }
}

void ZorbitMenu::nextSettingsSelection()
{
    game()->soundMan()->playSound("menubeep");

    switch(_curSettingsSelection)
    {
        /*
        case Difficulty:
            _curSettingsSelection = MusicVolume;
            DIFFICULTY_TEXT.setColor(sf::Color(255,255,255,80));
            _curDifficulty.setColor(sf::Color(255,255,255,80));
            _leftArrowSprite4.setColor(sf::Color(255,255,255,80));
            _rightArrowSprite4.setColor(sf::Color(255,255,255,80));
            _musicSliderSprite.setColor(sf::Color(255,255,255,255));
            _musicCursorSprite.setColor(sf::Color(255,255,255,255));
            _currentMusicVolume.setColor(sf::Color(255,255,255,255));
            MUSIC_VOLUME_TEXT.setColor(sf::Color(255,255,255,255));
            break;
            */
        case MusicVolume:
            _curSettingsSelection = SoundVolume;
            _soundSliderSprite.setColor(sf::Color(255,255,255,255));
            _soundCursorSprite.setColor(sf::Color(255,255,255,255));
            _currentSoundVolume.setColor(sf::Color(255,255,255,255));
            SOUND_VOLUME_TEXT.setColor(sf::Color(255,255,255,255));
            _musicSliderSprite.setColor(sf::Color(255,255,255,80));
            _musicCursorSprite.setColor(sf::Color(255,255,255,80));
            _currentMusicVolume.setColor(sf::Color(255,255,255,80));
            MUSIC_VOLUME_TEXT.setColor(sf::Color(255,255,255,80));
            break;
        case SoundVolume:
            _curSettingsSelection = Vsync;
            _soundSliderSprite.setColor(sf::Color(255,255,255,80));
            _soundCursorSprite.setColor(sf::Color(255,255,255,80));
            SOUND_VOLUME_TEXT.setColor(sf::Color(255,255,255,80));
            _currentSoundVolume.setColor(sf::Color(255,255,255,80));
            VSYNC_TEXT.setColor(sf::Color(255,255,255,255));
			_vsyncOnOff.setColor(sf::Color(255, 255, 255, 255));
			_leftArrowSprite5.setColor(sf::Color(255, 255, 255, 255));
			_rightArrowSprite5.setColor(sf::Color(255, 255, 255, 255));
            break;
		case Vsync:
			_curSettingsSelection = Controls;
			VSYNC_TEXT.setColor(sf::Color(255, 255, 255, 80));
			_vsyncOnOff.setColor(sf::Color(255, 255, 255, 80));
			_leftArrowSprite5.setColor(sf::Color(255, 255, 255, 80));
			_rightArrowSprite5.setColor(sf::Color(255, 255, 255, 80));
			CUSTOMIZE_CONTROLS_TEXT.setColor(sf::Color(255, 255, 255, 255));
			break;
        case Controls:
            _curSettingsSelection = Accept;
            CUSTOMIZE_CONTROLS_TEXT.setColor(sf::Color(255,255,255,80));
            ACCEPT_TEXT.setColor(sf::Color(255,255,255,255));
            break;
        case Accept:
            _curSettingsSelection = Discard;
            ACCEPT_TEXT.setColor(sf::Color(255,255,255,80));
            DISCARD_TEXT.setColor(sf::Color(255,255,255,255));
            break;
        case Discard:
            _curSettingsSelection = Defaults;
            DISCARD_TEXT.setColor(sf::Color(255,255,255,80));
            DEFAULT_TEXT.setColor(sf::Color(255,255,255,255));
            break;
        case Defaults:
            _curSettingsSelection = MusicVolume;
            _musicSliderSprite.setColor(sf::Color(255,255,255,255));
            _musicCursorSprite.setColor(sf::Color(255,255,255,255));
            _currentMusicVolume.setColor(sf::Color(255,255,255,255));
            MUSIC_VOLUME_TEXT.setColor(sf::Color(255,255,255,255));
            DEFAULT_TEXT.setColor(sf::Color(255,255,255,80));
            break;
        default:
            break;
    }
}

void ZorbitMenu::nextControlsSelection()
{
    game()->soundMan()->playSound("menubeep");
    _keyToStore = "";
    _buttonToStore = "";

    if(KEYBOARD_CONTROLLER_TEXT.getString().toAnsiString().compare("Controller") == 0)
    {
        switch(_curConSelection)
        {
        case c_keyCon:
            _curConSelection = c_MoveSensitivity;
            _leftArrowSprite.setColor(sf::Color(255,255,255,80));
            _rightArrowSprite.setColor(sf::Color(255,255,255,80));
            KEYBOARD_CONTROLLER_TEXT.setColor(sf::Color(255,255,255,80));
            MOVE_SENSITIVITY_TEXT.setColor(sf::Color(255,255,255,255));
            _moveSense.setColor(sf::Color(255,255,255,255));
            _leftArrowSprite3.setColor(sf::Color(255,255,255,255));
            _rightArrowSprite3.setColor(sf::Color(255,255,255,255));
            break;
        case c_MoveSensitivity:
            _curConSelection = c_Shoot;
            MOVE_SENSITIVITY_TEXT.setColor(sf::Color(255,255,255,80));
            _moveSense.setColor(sf::Color(255,255,255,80));
            _leftArrowSprite3.setColor(sf::Color(255,255,255,80));
            _rightArrowSprite3.setColor(sf::Color(255,255,255,80));
            SHOOT_TEXT.setColor(sf::Color(255,255,255,255));
            _shootSprite.setColor(sf::Color(255,255,255,255));
            break;
        case c_Shoot:
            _curConSelection = c_Jump;
            SHOOT_TEXT.setColor(sf::Color(255,255,255,80));
            _shootSprite.setColor(sf::Color(255,255,255,80));
            JUMP_TEXT.setColor(sf::Color(255,255,255,255));
            _jumpSprite.setColor(sf::Color(255,255,255,255));
            break;
        case c_Jump:
            _curConSelection = c_Grapple;
            JUMP_TEXT.setColor(sf::Color(255,255,255,80));
            _jumpSprite.setColor(sf::Color(255,255,255,80));
            GRAPPLE_TEXT.setColor(sf::Color(255,255,255,255));
            _grappleSprite.setColor(sf::Color(255,255,255,255));
            break;
        case c_Grapple:
            _curConSelection = c_GrappleSetting;
            GRAPPLE_TEXT.setColor(sf::Color(255,255,255,80));
            GRAPPLE_SETTING_TEXT.setColor(sf::Color(255,255,255,255));
            _grappleSprite.setColor(sf::Color(255,255,255,80));
            _leftArrowSprite2.setColor(sf::Color(255,255,255,255));
            _rightArrowSprite2.setColor(sf::Color(255,255,255,255));
            _curGrappleSetting.setColor(sf::Color(255,255,255,255));
            break;
        case c_GrappleSetting:
            _curConSelection = c_Accept;
            ACCEPT_TEXT.setColor(sf::Color(255,255,255,255));
            GRAPPLE_SETTING_TEXT.setColor(sf::Color(255,255,255,80));
            _leftArrowSprite2.setColor(sf::Color(255,255,255,80));
            _rightArrowSprite2.setColor(sf::Color(255,255,255,80));
            _curGrappleSetting.setColor(sf::Color(255,255,255,80));
            break;
        case c_Accept:
            _curConSelection = c_Discard;
            ACCEPT_TEXT.setColor(sf::Color(255,255,255,80));
            DISCARD_TEXT.setColor(sf::Color(255,255,255,255));
            break;
        case c_Discard:
            _curConSelection = c_Defaults;
            DISCARD_TEXT.setColor(sf::Color(255,255,255,80));
            DEFAULT_TEXT.setColor(sf::Color(255,255,255,255));
            break;
        case c_Defaults:
            _curConSelection = c_keyCon;
            DEFAULT_TEXT.setColor(sf::Color(255,255,255,80));
            KEYBOARD_CONTROLLER_TEXT.setColor(sf::Color(255,255,255,255));
            _leftArrowSprite.setColor(sf::Color(255,255,255,255));
            _rightArrowSprite.setColor(sf::Color(255,255,255,255));
            break;
        default:
            break;
        }
    }
    else
    {
        switch(_curKeySelection)
        {
        case k_keyCon:
            _curKeySelection = k_MoveLeft;
            _leftArrowSprite.setColor(sf::Color(255,255,255,80));
            _rightArrowSprite.setColor(sf::Color(255,255,255,80));
            KEYBOARD_CONTROLLER_TEXT.setColor(sf::Color(255,255,255,80));
            MOVE_LEFT_TEXT.setColor(sf::Color(255,255,255,255));
            _moveLeft.setColor(sf::Color(255,255,255,255));
            break;
        case k_MoveLeft:
            _curKeySelection = k_MoveRight;
            MOVE_LEFT_TEXT.setColor(sf::Color(255,255,255,80));
            _moveLeft.setColor(sf::Color(255,255,255,80));
            MOVE_RIGHT_TEXT.setColor(sf::Color(255,255,255,255));
            _moveRight.setColor(sf::Color(255,255,255,255));
            break;
        case k_MoveRight:
            _curKeySelection = k_Shoot;
            MOVE_RIGHT_TEXT.setColor(sf::Color(255,255,255,80));
            _moveRight.setColor(sf::Color(255,255,255,80));
            SHOOT_TEXT.setColor(sf::Color(255,255,255,255));
            _shoot.setColor(sf::Color(255,255,255,255));
            break;
        case k_Shoot:
            _curKeySelection = k_Jump;
            SHOOT_TEXT.setColor(sf::Color(255,255,255,80));
            _shoot.setColor(sf::Color(255,255,255,80));
            JUMP_TEXT.setColor(sf::Color(255,255,255,255));
            _jump.setColor(sf::Color(255,255,255,255));
            break;
        case k_Jump:
            _curKeySelection = k_Grapple;
            JUMP_TEXT.setColor(sf::Color(255,255,255,80));
            _jump.setColor(sf::Color(255,255,255,80));
            GRAPPLE_TEXT.setColor(sf::Color(255,255,255,255));
            _grapple.setColor(sf::Color(255,255,255,255));
            break;
        case k_Grapple:
            _curKeySelection = k_GrappleSetting;
            GRAPPLE_TEXT.setColor(sf::Color(255,255,255,80));
            _grapple.setColor(sf::Color(255,255,255,80));
            GRAPPLE_SETTING_TEXT.setColor(sf::Color(255,255,255,255));
            _leftArrowSprite2.setColor(sf::Color(255,255,255,255));
            _rightArrowSprite2.setColor(sf::Color(255,255,255,255));
            _curGrappleSetting.setColor(sf::Color(255,255,255,255));
            break;
        case k_GrappleSetting:
            _curKeySelection = k_Accept;
            ACCEPT_TEXT.setColor(sf::Color(255,255,255,255));
            GRAPPLE_SETTING_TEXT.setColor(sf::Color(255,255,255,80));
            _leftArrowSprite2.setColor(sf::Color(255,255,255,80));
            _rightArrowSprite2.setColor(sf::Color(255,255,255,80));
            _curGrappleSetting.setColor(sf::Color(255,255,255,80));
            break;
        case k_Accept:
            _curKeySelection = k_Discard;
            ACCEPT_TEXT.setColor(sf::Color(255,255,255,80));
            DISCARD_TEXT.setColor(sf::Color(255,255,255,255));
            break;
        case k_Discard:
            _curKeySelection = k_Defaults;
            DISCARD_TEXT.setColor(sf::Color(255,255,255,80));
            DEFAULT_TEXT.setColor(sf::Color(255,255,255,255));
            break;
        case k_Defaults:
            _curKeySelection = k_keyCon;
            DEFAULT_TEXT.setColor(sf::Color(255,255,255,80));
            KEYBOARD_CONTROLLER_TEXT.setColor(sf::Color(255,255,255,255));
            _leftArrowSprite.setColor(sf::Color(255,255,255,255));
            _rightArrowSprite.setColor(sf::Color(255,255,255,255));
            break;
        default:
            break;
        }
    }
}

void ZorbitMenu::previousSelection()
{
    MainMenu::previousSelection();

    game()->soundMan()->playSound("menubeep");
    _bounce = 0;

    switch(_currentSelection)
    {
    case Play:
        _playSprite.setColor(sf::Color(255,255,255,255));
        break;
    case Settings:
        _settingsSprite.setColor(sf::Color(255,255,255,255));
        break;
    case LevelSelect:
        _levelsSprite.setColor(sf::Color(255,255,255,255));
        break;
    case Creditss:
        _creditsSprite.setColor(sf::Color(255,255,255,255));
        break;
    case Exit:
        _exitSprite.setColor(sf::Color(255,255,255,255));
        break;
    default:
        break;
    }
}

void ZorbitMenu::selectionIs(Selection selection)
{
    _bounce = 0;
    switch(selection)
    {
    case Play:
        _playSprite.setColor(sf::Color(255,255,255,255));
        break;
    case Settings:
        _settingsSprite.setColor(sf::Color(255,255,255,255));
        break;
    case LevelSelect:
        _levelsSprite.setColor(sf::Color(255,255,255,255));
        break;
    case Creditss:
        _creditsSprite.setColor(sf::Color(255,255,255,255));
        break;
    case Exit:
        _exitSprite.setColor(sf::Color(255,255,255,255));
        break;
    default:
        break;
    }
    _currentSelection = selection;
}

void ZorbitMenu::previousSettingsSelection()
{
    game()->soundMan()->playSound("menubeep");

    switch(_curSettingsSelection)
    {
	case Vsync:
		_curSettingsSelection = SoundVolume;
		_soundSliderSprite.setColor(sf::Color(255, 255, 255, 255));
		_soundCursorSprite.setColor(sf::Color(255, 255, 255, 255));
		_currentSoundVolume.setColor(sf::Color(255, 255, 255, 255));
		SOUND_VOLUME_TEXT.setColor(sf::Color(255, 255, 255, 255));
		VSYNC_TEXT.setColor(sf::Color(255, 255, 255, 80));
		_vsyncOnOff.setColor(sf::Color(255, 255, 255, 80));
		_leftArrowSprite5.setColor(sf::Color(255, 255, 255, 80));
		_rightArrowSprite5.setColor(sf::Color(255, 255, 255, 80));
		break;
    case Controls:
        _curSettingsSelection = Vsync;
        VSYNC_TEXT.setColor(sf::Color(255,255,255,255));
        _vsyncOnOff.setColor(sf::Color(255,255,255,255));
        _leftArrowSprite5.setColor(sf::Color(255,255,255,255));
        _rightArrowSprite5.setColor(sf::Color(255,255,255,255));
        CUSTOMIZE_CONTROLS_TEXT.setColor(sf::Color(255,255,255,80));
        break;
    case Accept:
        _curSettingsSelection = Controls;
        ACCEPT_TEXT.setColor(sf::Color(255,255,255,80));
        CUSTOMIZE_CONTROLS_TEXT.setColor(sf::Color(255,255,255,255));
        break;
    case Discard:
        _curSettingsSelection = Accept;
        DISCARD_TEXT.setColor(sf::Color(255,255,255,80));
        ACCEPT_TEXT.setColor(sf::Color(255,255,255,255));
        break;
    case Defaults:
        _curSettingsSelection = Discard;
        DEFAULT_TEXT.setColor(sf::Color(255,255,255,80));
        DISCARD_TEXT.setColor(sf::Color(255,255,255,255));
        break;
    case MusicVolume:
        _curSettingsSelection = Defaults;
        _musicSliderSprite.setColor(sf::Color(255,255,255,80));
        _musicCursorSprite.setColor(sf::Color(255,255,255,80));
        _currentMusicVolume.setColor(sf::Color(255,255,255,80));
        MUSIC_VOLUME_TEXT.setColor(sf::Color(255,255,255,80));
        DEFAULT_TEXT.setColor(sf::Color(255,255,255,255));
        break;
    case SoundVolume:
        _curSettingsSelection = MusicVolume;
        _musicSliderSprite.setColor(sf::Color(255,255,255,255));
        _musicCursorSprite.setColor(sf::Color(255,255,255,255));
        _currentMusicVolume.setColor(sf::Color(255,255,255,255));
        MUSIC_VOLUME_TEXT.setColor(sf::Color(255,255,255,255));
        _soundSliderSprite.setColor(sf::Color(255,255,255,80));
        _soundCursorSprite.setColor(sf::Color(255,255,255,80));
        _currentSoundVolume.setColor(sf::Color(255,255,255,80));
        SOUND_VOLUME_TEXT.setColor(sf::Color(255,255,255,80));
        break;
        /*
    case Difficulty:
        _curSettingsSelection = Defaults;
        DIFFICULTY_TEXT.setColor(sf::Color(255,255,255,80));
        _curDifficulty.setColor(sf::Color(255,255,255,80));
        _leftArrowSprite4.setColor(sf::Color(255,255,255,80));
        _rightArrowSprite4.setColor(sf::Color(255,255,255,80));
        DEFAULT_TEXT.setColor(sf::Color(255,255,255,255));
        */
    default:
        break;
    }
}

void ZorbitMenu::previousControlsSelection()
{
    game()->soundMan()->playSound("menubeep");

    _keyToStore = "";
    _buttonToStore = "";

    if(KEYBOARD_CONTROLLER_TEXT.getString().toAnsiString().compare("Controller") == 0)
    {
        switch(_curConSelection)
        {
        case c_keyCon:
            _curConSelection = c_Defaults;
            _leftArrowSprite.setColor(sf::Color(255,255,255,80));
            _rightArrowSprite.setColor(sf::Color(255,255,255,80));
            KEYBOARD_CONTROLLER_TEXT.setColor(sf::Color(255,255,255,80));
            DEFAULT_TEXT.setColor(sf::Color(255,255,255,255));
            break;
        case c_Defaults:
            _curConSelection = c_Discard;
            DEFAULT_TEXT.setColor(sf::Color(255,255,255,80));
            DISCARD_TEXT.setColor(sf::Color(255,255,255,255));
            break;
        case c_Discard:
            _curConSelection = c_Accept;
            DISCARD_TEXT.setColor(sf::Color(255,255,255,80));
            ACCEPT_TEXT.setColor(sf::Color(255,255,255,255));
            break;
        case c_Accept:
            _curConSelection = c_GrappleSetting;
            ACCEPT_TEXT.setColor(sf::Color(255,255,255,80));
            GRAPPLE_SETTING_TEXT.setColor(sf::Color(255,255,255,255));
            _leftArrowSprite2.setColor(sf::Color(255,255,255,255));
            _rightArrowSprite2.setColor(sf::Color(255,255,255,255));
            _curGrappleSetting.setColor(sf::Color(255,255,255,255));
            break;
        case c_MoveSensitivity:
            _curConSelection = c_keyCon;
            MOVE_SENSITIVITY_TEXT.setColor(sf::Color(255,255,255,80));
            _moveSense.setColor(sf::Color(255,255,255,80));
            _leftArrowSprite3.setColor(sf::Color(255,255,255,80));
            _rightArrowSprite3.setColor(sf::Color(255,255,255,80));
            KEYBOARD_CONTROLLER_TEXT.setColor(sf::Color(255,255,255,255));
            _leftArrowSprite.setColor(sf::Color(255,255,255,255));
            _rightArrowSprite.setColor(sf::Color(255,255,255,255));
            break;
        case c_Shoot:
            _curConSelection = c_MoveSensitivity;
            SHOOT_TEXT.setColor(sf::Color(255,255,255,80));
            _shootSprite.setColor(sf::Color(255,255,255,80));
            MOVE_SENSITIVITY_TEXT.setColor(sf::Color(255,255,255,255));
            _moveSense.setColor(sf::Color(255,255,255,255));
            _leftArrowSprite3.setColor(sf::Color(255,255,255,255));
            _rightArrowSprite3.setColor(sf::Color(255,255,255,255));
            break;
        case c_Jump:
            _curConSelection = c_Shoot;
            JUMP_TEXT.setColor(sf::Color(255,255,255,80));
            _jumpSprite.setColor(sf::Color(255,255,255,80));
            SHOOT_TEXT.setColor(sf::Color(255,255,255,255));
            _shootSprite.setColor(sf::Color(255,255,255,255));
            break;
        case c_Grapple:
            _curConSelection = c_Jump;
            GRAPPLE_TEXT.setColor(sf::Color(255,255,255,80));
            _grappleSprite.setColor(sf::Color(255,255,255,80));
            JUMP_TEXT.setColor(sf::Color(255,255,255,255));
            _jumpSprite.setColor(sf::Color(255,255,255,255));
            break;
        case c_GrappleSetting:
            _curConSelection = c_Grapple;
            GRAPPLE_SETTING_TEXT.setColor(sf::Color(255,255,255,80));
            _leftArrowSprite2.setColor(sf::Color(255,255,255,80));
            _rightArrowSprite2.setColor(sf::Color(255,255,255,80));
            _curGrappleSetting.setColor(sf::Color(255,255,255,80));
            GRAPPLE_TEXT.setColor(sf::Color(255,255,255,255));
            _grappleSprite.setColor(sf::Color(255,255,255,255));
            break;
        default:
            break;
        }
    }
    else
    {
        switch(_curKeySelection)
        {
        case k_keyCon:
            _curKeySelection = k_Defaults;
            _leftArrowSprite.setColor(sf::Color(255,255,255,80));
            _rightArrowSprite.setColor(sf::Color(255,255,255,80));
            KEYBOARD_CONTROLLER_TEXT.setColor(sf::Color(255,255,255,80));
            DEFAULT_TEXT.setColor(sf::Color(255,255,255,255));
            break;
        case k_Defaults:
            _curKeySelection = k_Discard;
            DEFAULT_TEXT.setColor(sf::Color(255,255,255,80));
            DISCARD_TEXT.setColor(sf::Color(255,255,255,255));
            break;
        case k_Discard:
            _curKeySelection = k_Accept;
            DISCARD_TEXT.setColor(sf::Color(255,255,255,80));
            ACCEPT_TEXT.setColor(sf::Color(255,255,255,255));
            break;
        case k_Accept:
            _curKeySelection = k_GrappleSetting;
            ACCEPT_TEXT.setColor(sf::Color(255,255,255,80));
            GRAPPLE_SETTING_TEXT.setColor(sf::Color(255,255,255,255));
            _leftArrowSprite2.setColor(sf::Color(255,255,255,255));
            _rightArrowSprite2.setColor(sf::Color(255,255,255,255));
            _curGrappleSetting.setColor(sf::Color(255,255,255,255));
            break;
        case k_MoveLeft:
            _curKeySelection = k_keyCon;
            MOVE_LEFT_TEXT.setColor(sf::Color(255,255,255,80));
            _moveLeft.setColor(sf::Color(255,255,255,80));
            KEYBOARD_CONTROLLER_TEXT.setColor(sf::Color(255,255,255,255));
            _leftArrowSprite.setColor(sf::Color(255,255,255,255));
            _rightArrowSprite.setColor(sf::Color(255,255,255,255));
            break;
        case k_MoveRight:
            _curKeySelection = k_MoveLeft;
            MOVE_RIGHT_TEXT.setColor(sf::Color(255,255,255,80));
            _moveRight.setColor(sf::Color(255,255,255,80));
            MOVE_LEFT_TEXT.setColor(sf::Color(255,255,255,255));
            _moveLeft.setColor(sf::Color(255,255,255,255));
            break;
        case k_Shoot:
            _curKeySelection = k_MoveRight;
            SHOOT_TEXT.setColor(sf::Color(255,255,255,80));
            _shoot.setColor(sf::Color(255,255,255,80));
            MOVE_RIGHT_TEXT.setColor(sf::Color(255,255,255,255));
            _moveRight.setColor(sf::Color(255,255,255,255));
            break;
        case k_Jump:
            _curKeySelection = k_Shoot;
            JUMP_TEXT.setColor(sf::Color(255,255,255,80));
            _jump.setColor(sf::Color(255,255,255,80));
            SHOOT_TEXT.setColor(sf::Color(255,255,255,255));
            _shoot.setColor(sf::Color(255,255,255,255));
            break;
        case k_Grapple:
            _curKeySelection = k_Jump;
            GRAPPLE_TEXT.setColor(sf::Color(255,255,255,80));
            _grapple.setColor(sf::Color(255,255,255,80));
            JUMP_TEXT.setColor(sf::Color(255,255,255,255));
            _jump.setColor(sf::Color(255,255,255,255));
            break;
        case k_GrappleSetting:
            _curKeySelection = k_Grapple;
            GRAPPLE_SETTING_TEXT.setColor(sf::Color(255,255,255,80));
            _leftArrowSprite2.setColor(sf::Color(255,255,255,80));
            _rightArrowSprite2.setColor(sf::Color(255,255,255,80));
            _curGrappleSetting.setColor(sf::Color(255,255,255,80));
            GRAPPLE_TEXT.setColor(sf::Color(255,255,255,255));
            _grapple.setColor(sf::Color(255,255,255,255));
            break;
        default:
            break;
        }
    }
}

void ZorbitMenu::confirmSelection()
{
    if(!_canConfirm) return;

    game()->soundMan()->playSound("confirm2");

    switch(_currentSelection)
    {
    case Play:
        play();
        break;
    case LevelSelect:
        game()->stateMan()->activeStateIs("LevelSelect");
        static_cast<LevelSelectScreen*>(game()->stateMan()->state("LevelSelect").ptr())->reset();
        break;
    case Settings:
        _drawSettings = !_drawSettings;
        _next = 0;
        _previous = 0;
        break;
    case Creditss:
        game()->stateMan()->stateNew(new Credits(static_cast<ZorbitsOrbits*>(game())));
        static_cast<ZorbitsOrbits*>(game())->currentStatusIs(not_playing);
        static_cast<Credits*>(game()->stateMan()->state("Credits").ptr())->reset();
        game()->stateMan()->activeStateIs("Credits");
        break;
    case Exit:
        game()->runningIs(false);
        break;
    default:
        break;
    }
}

void ZorbitMenu::confirmSettingsSelection()
{
    if(!_canConfirm) return;

    game()->soundMan()->playSound("confirm2");

    switch(_curSettingsSelection)
    {
        case Controls:
            _drawControls = true;
            _curConSelection = c_keyCon;
            _curKeySelection = k_keyCon;
            KEYBOARD_CONTROLLER_TEXT.setColor(sf::Color(255,255,255,255));
            _leftArrowSprite.setColor(sf::Color(255,255,255,255));
            _rightArrowSprite.setColor(sf::Color(255,255,255,255));
            break;
        case Accept:
            _curSettingsSelection = MusicVolume;
            MUSIC_VOLUME_TEXT.setColor(sf::Color(255,255,255,255));
            _musicCursorSprite.setColor(sf::Color(255,255,255,255));
            _musicSliderSprite.setColor(sf::Color(255,255,255,255));
            ACCEPT_TEXT.setColor(sf::Color(255,255,255,80));
            _curConSelection = c_keyCon;
            _curKeySelection = k_keyCon;
            _drawSettings = false;
            game()->settings()->saveSettings();
            break;
        case Discard:
            _drawSettings = false;
            game()->settings()->loadSettings();
            game()->setMusicVolume(game()->settings()->musicVolume());
            game()->setSoundVolume(game()->settings()->soundVolume());
            break;
        case Defaults:
            game()->settings()->revertToDefaults();
            game()->soundMan()->setMusicVolume(50);
            game()->soundMan()->setSoundVolume(50);
            game()->setMusicVolume(game()->settings()->musicVolume());
            game()->setSoundVolume(game()->settings()->soundVolume());
            break;
        default:
            break;
    }
}

void ZorbitMenu::confirmControlsSelection()
{
    if(!_canConfirm) return;

    game()->soundMan()->playSound("confirm2");

    if(KEYBOARD_CONTROLLER_TEXT.getString().toAnsiString().compare("Controller") == 0)
    {
        switch(_curConSelection)
        {
        case c_Shoot:
            _listening = true;
            _buttonToStore = "Shoot";
            break;
        case c_Jump:
            _listening = true;
            _buttonToStore = "Jump";
            break;
        case c_Grapple:
            _listening = true;
            _buttonToStore = "Grapple";
            break;
        case c_Accept:
            _drawControls = false;
            game()->settings()->saveSettings();
            ACCEPT_TEXT.setColor(sf::Color(255,255,255,80));
            break;
        case c_Discard:
            game()->settings()->loadSettings();
            break;
        case c_Defaults:
            game()->settings()->revertToDefaultControls();
            game()->settings()->saveSettings();
            break;
        default:
            break;
        }
    }
    else
    {
        switch(_curKeySelection)
        {
        case k_MoveLeft:
            _listening = true;
            _keyToStore = "Left";
            break;
        case k_MoveRight:
            _listening = true;
            _keyToStore = "Right";
            break;
        case k_Shoot:
            _listening = true;
            _keyToStore = "Shoot";
            break;
        case k_Jump:
            _listening = true;
            _keyToStore = "Jump";
            break;
        case k_Grapple:
            _listening = true;
            _keyToStore = "Grapple";
            break;
        case k_Accept:
            _drawControls = false;
            game()->settings()->saveSettings();
            ACCEPT_TEXT.setColor(sf::Color(255,255,255,80));
            break;
        case k_Discard:
            game()->settings()->loadSettings();
            break;
        case k_Defaults:
            game()->settings()->revertToDefaultControls();
            game()->settings()->saveSettings();
            break;
        default:
            break;
        }
    }

}

void ZorbitMenu::resizeSprites()
{
    _bg2Sprite.scale(_scale);
    _bgSprite.scale(_scale);

    _playSprite.scale(_scale);
    _levelsSprite.scale(_scale);
    _settingsSprite.scale(_scale);
    _creditsSprite.scale(_scale);
    _exitSprite.scale(_scale);

    _settingsBGSprite.scale(_scale);
    MUSIC_VOLUME_TEXT.scale(_scale);
    _currentMusicVolume.scale(_scale);
    _musicSliderSprite.scale(_scale);
    _musicCursorSprite.scale(_scale);
    SOUND_VOLUME_TEXT.scale(_scale);
    _currentSoundVolume.scale(_scale);
    _soundSliderSprite.scale(_scale);
    _soundCursorSprite.scale(_scale);
    _leftArrowSprite.scale(_scale);
    _rightArrowSprite.scale(_scale);
    CUSTOMIZE_CONTROLS_TEXT.scale(_scale);
    ACCEPT_TEXT.scale(_scale);
    DISCARD_TEXT.scale(_scale);
    DEFAULT_TEXT.scale(_scale);
    DIFFICULTY_TEXT.scale(_scale);
    _curDifficulty.scale(_scale);
    _frogSprite.scale(_scale);
    _frogText.scale(_scale);

    KEYBOARD_CONTROLLER_TEXT.scale(_scale);
    MOVE_SENSITIVITY_TEXT.scale(_scale);
    _moveSense.scale(_scale);
    MOVE_LEFT_TEXT.scale(_scale);
    _moveLeft.scale(_scale);
    MOVE_RIGHT_TEXT.scale(_scale);
    _moveRight.scale(_scale);
    SHOOT_TEXT.scale(_scale);
    _shoot.scale(_scale);
    JUMP_TEXT.scale(_scale);
    _jump.scale(_scale);
    GRAPPLE_TEXT.scale(_scale);
    _grapple.scale(_scale);
    GRAPPLE_SETTING_TEXT.scale(_scale);
    _curGrappleSetting.scale(_scale);
    _leftArrowSprite.scale(_scale);
    _rightArrowSprite.scale(_scale);
    _leftArrowSprite2.scale(_scale);
    _rightArrowSprite2.scale(_scale);
    _leftArrowSprite3.scale(_scale);
    _rightArrowSprite3.scale(_scale);
    _leftArrowSprite4.scale(_scale);
    _rightArrowSprite4.scale(_scale);
    _shootSprite.scale(_scale);
    _jumpSprite.scale(_scale);
    _grappleSprite.scale(_scale);
    _confirmSprite.scale(_scale);
    _confirmText.scale(_scale);
    _backSprite.scale(_scale);
    _backText.scale(_scale);
}

void ZorbitMenu::play()
{
    if(static_cast<ZorbitsOrbits*>(game())->_currentLevel == uninitialized)
    {
        _chooseDifficulty = true;
    }
    else game()->stateMan()->activeStateIs("LoadingScreen");
}

void ZorbitMenu::update()
{
    MainMenu::update();

    int fadeTime = 2;

    if(_chooseDifficulty)
    {
        _curDifficulty.setString(game()->difficulty());

        if(game()->difficulty().compare("Easy") == 0)
            _frogText.setString("As many\nlives as ya\nneed, bub.");
        else if(game()->difficulty().compare("Medium") == 0)
            _frogText.setString("Ten lives\n\'n that\'s it.");
        else if(game()->difficulty().compare("Hard") == 0)
            _frogText.setString("Cats gots\nnine lives.\nYous got five.");
        else if(game()->difficulty().compare("Very Hard") == 0)
            _frogText.setString("One life.\nLive it well,\nkid.");
        else if(game()->difficulty().compare("Insane") == 0)
            _frogText.setString("Get hit once\n\'n yer a\ngoner.");
    }

    if(_fading)
    {
		int interval = 3;// * 60 / fps();
        if(_fadeSprite.getColor().a + interval <= 255)
            _fadeSprite.setColor(sf::Color(255,255,255,_fadeSprite.getColor().a + interval));
        else _fadeSprite.setColor(sf::Color(255,255,255,255));
    }

    // For dramatic effect.
    if(_startingStoryMode && _effectTimer.getElapsedTime().asSeconds() > fadeTime)
    {
        game()->soundMan()->playMusic("robotropolis");
        static_cast<ZorbitsOrbits*>(game())->clearLevelNames();
        static_cast<ZorbitsOrbits*>(game())->setLevelsForPlay();
        static_cast<ZorbitsOrbits*>(game())->currentGameModeIs(classic);
        static_cast<LoadingScreen*>(game()->stateMan()->state("LoadingScreen").ptr())->levelNameIs("tutorial");
        // FINAL
        //static_cast<LoadingScreen*>(game()->stateMan()->state("LoadingScreen").ptr())->levelNameIs("tc1");
        static_cast<ZorbitsOrbits*>(game())->_currentLevel = cinematic;
        game()->settings()->currentLivesIs(game()->settings()->maxLives());
        _startingStoryMode = false;
        _fading = false;
        _chooseDifficulty = false;
        _draw = false;
        game()->stateMan()->activeStateIs("LoadingScreen");
    }

    if(static_cast<ZorbitsOrbits*>(game())->currentStatus() == playing)
        _playSprite.setTextureRect(sf::IntRect(0,300,400,59));
    else _playSprite.setTextureRect(sf::IntRect(0,0,400,59));

    if(_backgroundSprite.getPosition().y < game()->renderWindow()->getView().getCenter().y && !_sfx)
    {
        _backgroundSprite.setPosition(_backgroundSprite.getPosition().x,
                                      _backgroundSprite.getPosition().y + 12);
        _playSprite.setPosition(_playSprite.getPosition().x,
                                      _playSprite.getPosition().y - 12);
        _levelsSprite.setPosition(_levelsSprite.getPosition().x,
                                      _levelsSprite.getPosition().y - 12);
        _settingsSprite.setPosition(_settingsSprite.getPosition().x,
                                      _settingsSprite.getPosition().y - 12);
        _creditsSprite.setPosition(_creditsSprite.getPosition().x,
                                      _creditsSprite.getPosition().y - 12);
        _exitSprite.setPosition(_exitSprite.getPosition().x,
                                      _exitSprite.getPosition().y - 12);
    }
    else if(!_sfx)
    {
        game()->soundMan()->playSound("menucrash");
        _sfx = true;
        _canConfirm = true;
    }
    else
    {
        animate();
    }

    if(sf::Joystick::isConnected(0))
    {
        _confirmSprite.setTextureRect(sf::IntRect(0,0,50,50));
        _backSprite.setTextureRect(sf::IntRect(50,0,50,50));
    }
    else
    {
        _confirmSprite.setTextureRect(sf::IntRect(300,0,50,50));
        _backSprite.setTextureRect(sf::IntRect(350,0,50,50));
    }

    // SETTINGS
    if(_drawSettings)
    {
        _musicCursorSprite.setPosition(_musicSliderSprite.getPosition().x + _scale.x*3 * game()->musicVolume(), _musicSliderSprite.getPosition().y + _scale.y*10);
        std::stringstream ss;
        ss << game()->musicVolume();
        _currentMusicVolume.setString(ss.str());

        _soundCursorSprite.setPosition(_soundSliderSprite.getPosition().x + _scale.x*3 * game()->soundVolume(), _soundSliderSprite.getPosition().y + _scale.y*10);
        std::stringstream ss2;
        ss2 << game()->soundVolume();
        _currentSoundVolume.setString(ss2.str());
    }

    if(_soundUp)
    {
        game()->setSoundVolume(game()->soundVolume() + 1);
        game()->soundMan()->playSound("menubeep");
    }
    if(_musicUp) game()->setMusicVolume(game()->musicVolume() + 1);
    if(_soundDown)
    {
        game()->setSoundVolume(game()->soundVolume() - 1);
        game()->soundMan()->playSound("menubeep");
    }
    if(_musicDown) game()->setMusicVolume(game()->musicVolume() - 1);

    if(game()->musicVolume() < 0 ) game()->setMusicVolume(0);
    if(game()->musicVolume() > 100 ) game()->setMusicVolume(100);
    if(game()->soundVolume() < 0 ) game()->setSoundVolume(0);
    if(game()->soundVolume() > 100 ) game()->setSoundVolume(100);

    // Controls
    if(KEYBOARD_CONTROLLER_TEXT.getString().toAnsiString().compare("Controller") == 0)
    {
        _moveSense.setString(game()->settings()->xSensitivityAsString());
        SHOOT_TEXT.setPosition(KEYBOARD_CONTROLLER_TEXT.getPosition() + sf::Vector2f(_scale.x*-200, _scale.y*90));
        JUMP_TEXT.setPosition(KEYBOARD_CONTROLLER_TEXT.getPosition() + sf::Vector2f(_scale.x*-200, _scale.y*130));
        GRAPPLE_TEXT.setPosition(KEYBOARD_CONTROLLER_TEXT.getPosition() + sf::Vector2f(_scale.x*-200, _scale.y*170));
        _shoot.setPosition(SHOOT_TEXT.getPosition() + sf::Vector2f(_scale.x*300, 0));
        _jump.setPosition(JUMP_TEXT.getPosition() + sf::Vector2f(_scale.x*300, 0));
        _grapple.setPosition(GRAPPLE_TEXT.getPosition() + sf::Vector2f(_scale.x*300, 0));

        _grappleSprite.setTextureRect(sf::IntRect(50 * game()->settings()->button("Grapple"),0,50,50));
        _jumpSprite.setTextureRect(sf::IntRect(50 * game()->settings()->button("Jump"),0,50,50));
        _shootSprite.setTextureRect(sf::IntRect(50 * game()->settings()->button("Shoot"),0,50,50));

        _grappleSprite.setPosition(GRAPPLE_TEXT.getPosition() + sf::Vector2f(_scale.x*300, 0));
        _jumpSprite.setPosition(JUMP_TEXT.getPosition() + sf::Vector2f(_scale.x*300, 0));
        _shootSprite.setPosition(SHOOT_TEXT.getPosition() + sf::Vector2f(_scale.x*300, 0));

    }
    else
    {
        SHOOT_TEXT.setPosition(KEYBOARD_CONTROLLER_TEXT.getPosition() + sf::Vector2f(_scale.x*-200, _scale.y*130));
        JUMP_TEXT.setPosition(KEYBOARD_CONTROLLER_TEXT.getPosition() + sf::Vector2f(_scale.x*-200, _scale.y*170));
        GRAPPLE_TEXT.setPosition(KEYBOARD_CONTROLLER_TEXT.getPosition() + sf::Vector2f(_scale.x*-200, _scale.y*210));
        _shoot.setPosition(SHOOT_TEXT.getPosition() + sf::Vector2f(_scale.x*300, 0));
        _jump.setPosition(JUMP_TEXT.getPosition() + sf::Vector2f(_scale.x*300, 0));
        _grapple.setPosition(GRAPPLE_TEXT.getPosition() + sf::Vector2f(_scale.x*300, 0));

        _shoot.setString(game()->settings()->keyAsString("Shoot"));
        _jump.setString(game()->settings()->keyAsString("Jump"));
        _grapple.setString(game()->settings()->keyAsString("Grapple"));
        _moveLeft.setString(game()->settings()->keyAsString("Left"));
        _moveRight.setString(game()->settings()->keyAsString("Right"));

    }
    _curGrappleSetting.setString(game()->settings()->grappleSetting());

    if (_drawControls && _listening && KEYBOARD_CONTROLLER_TEXT.getString().toAnsiString().compare("Controller") == 0)
    {
        if(_curConSelection == c_Shoot)
            _shootSprite.setColor(sf::Color(255,255,255,255));
        if(_curConSelection == c_Jump)
            _jumpSprite.setColor(sf::Color(255,255,255,255));
        if(_curConSelection == c_Grapple)
            _grappleSprite.setColor(sf::Color(255,255,255,255));
    }
    if(_drawControls && !_listening && KEYBOARD_CONTROLLER_TEXT.getString().toAnsiString().compare("Controller") == 0)
    {
        _shootSprite.setColor(sf::Color(255,255,255,80));
        _jumpSprite.setColor(sf::Color(255,255,255,80));
        _grappleSprite.setColor(sf::Color(255,255,255,80));
    }
    if (_drawControls && _listening && KEYBOARD_CONTROLLER_TEXT.getString().toAnsiString().compare("Controller") != 0)
    {
        if(_curKeySelection == k_MoveLeft)
            _moveLeft.setColor(sf::Color(255,255,255,255));
        if(_curKeySelection == k_MoveRight)
            _moveRight.setColor(sf::Color(255,255,255,255));
        if(_curKeySelection == k_Shoot)
            _shoot.setColor(sf::Color(255,255,255,255));
        if(_curKeySelection == k_Jump)
            _jump.setColor(sf::Color(255,255,255,255));
        if(_curKeySelection == k_Grapple)
            _grapple.setColor(sf::Color(255,255,255,255));
    }
    if(_drawControls && !_listening && KEYBOARD_CONTROLLER_TEXT.getString().toAnsiString().compare("Controller") != 0)
    {
        _shoot.setColor(sf::Color(255,255,255,80));
        _jump.setColor(sf::Color(255,255,255,80));
        _grapple.setColor(sf::Color(255,255,255,80));
        _moveLeft.setColor(sf::Color(255,255,255,80));
        _moveRight.setColor(sf::Color(255,255,255,80));
    }
}

void ZorbitMenu::animate()
{
    std::string whichSprite;
    sf::Color color;

    switch(_currentSelection)
    {
    case Play:
        color = _playSprite.getColor();
        whichSprite = "Play";
        _levelsSprite.setColor(sf::Color(255,255,255,128));
        _settingsSprite.setColor(sf::Color(255,255,255,128));
        _creditsSprite.setColor(sf::Color(255,255,255,128));
        _exitSprite.setColor(sf::Color(255,255,255,128));
        break;
    case LevelSelect:
        color = _levelsSprite.getColor();
        whichSprite = "Levels";
        _playSprite.setColor(sf::Color(255,255,255,128));
        _settingsSprite.setColor(sf::Color(255,255,255,128));
        _creditsSprite.setColor(sf::Color(255,255,255,128));
        _exitSprite.setColor(sf::Color(255,255,255,128));
        break;
    case Settings:
        color = _settingsSprite.getColor();
        whichSprite = "Settings";
        _levelsSprite.setColor(sf::Color(255,255,255,128));
        _playSprite.setColor(sf::Color(255,255,255,128));
        _creditsSprite.setColor(sf::Color(255,255,255,128));
        _exitSprite.setColor(sf::Color(255,255,255,128));
        break;
    case Creditss:
        color = _creditsSprite.getColor();
        whichSprite = "Credits";
        _levelsSprite.setColor(sf::Color(255,255,255,128));
        _playSprite.setColor(sf::Color(255,255,255,128));
        _settingsSprite.setColor(sf::Color(255,255,255,128));
        _exitSprite.setColor(sf::Color(255,255,255,128));
        break;
    case Exit:
        color = _exitSprite.getColor();
        whichSprite = "Exit";
        _levelsSprite.setColor(sf::Color(255,255,255,128));
        _settingsSprite.setColor(sf::Color(255,255,255,128));
        _creditsSprite.setColor(sf::Color(255,255,255,128));
        _playSprite.setColor(sf::Color(255,255,255,128));
        break;
    default:
        break;
    }

    if(_bounce == 20)
        _bounce = 0;

    if(_bounce >= 0 && _bounce < 10)
        colorSprite(whichSprite, sf::Color(255, 255, 255, color.a - 8));
    if(_bounce >= 10 && _bounce < 20)
        colorSprite(whichSprite, sf::Color(255, 255, 255, color.a + 8));

    _bounce++;
}

void ZorbitMenu::colorSprite(std::string sprite, sf::Color color)
{
    if(sprite.compare("Play") == 0)
        _playSprite.setColor(color);
    if(sprite.compare("Levels") == 0)
        _levelsSprite.setColor(color);
    if(sprite.compare("Settings") == 0)
        _settingsSprite.setColor(color);
    if(sprite.compare("Credits") == 0)
        _creditsSprite.setColor(color);
    if(sprite.compare("Exit") == 0)
        _exitSprite.setColor(color);
}

void ZorbitMenu::highlight(int selection)
{
    if(selection == Play)
    {
        _playSprite.setScale(1.1, 1.1);
        _levelsSprite.setScale(0.9, 0.9);
        _settingsSprite.setScale(0.9, 0.9);
        _creditsSprite.setScale(0.9, 0.9);
        _exitSprite.setScale(0.9, 0.9);
    }
    if(selection == LevelSelect)
    {
        _playSprite.setScale(0.9, 0.9);
        _levelsSprite.setScale(1.1, 1.1);
        _settingsSprite.setScale(0.9, 0.9);
        _creditsSprite.setScale(0.9, 0.9);
        _exitSprite.setScale(0.9, 0.9);
    }
    if(selection == Settings)
    {
        _playSprite.setScale(0.9, 0.9);
        _levelsSprite.setScale(0.9, 0.9);
        _settingsSprite.setScale(1.1, 1.1);
        _creditsSprite.setScale(0.9, 0.9);
        _exitSprite.setScale(0.9, 0.9);
    }
    if(selection == Creditss)
    {
        _playSprite.setScale(0.9, 0.9);
        _levelsSprite.setScale(0.9, 0.9);
        _settingsSprite.setScale(0.9, 0.9);
        _creditsSprite.setScale(1.1, 1.1);
        _exitSprite.setScale(0.9, 0.9);
    }
    if(selection == Exit)
    {
        _playSprite.setScale(0.9, 0.9);
        _levelsSprite.setScale(0.9, 0.9);
        _settingsSprite.setScale(0.9, 0.9);
        _creditsSprite.setScale(0.9, 0.9);
        _exitSprite.setScale(1.1, 1.1);
    }
}

void ZorbitMenu::draw()
{
    if(!_draw) return;

    game()->renderWindow()->draw(_bg2Sprite);
    game()->renderWindow()->draw(_bgSprite);
    if(_sfx)game()->renderWindow()->draw(_confirmSprite);
    if(_sfx)game()->renderWindow()->draw(_confirmText);
    if(_sfx)game()->renderWindow()->draw(_backText);
    if(_sfx)game()->renderWindow()->draw(_backSprite);

    if(!_drawSettings && !_chooseDifficulty)
    {
        game()->renderWindow()->draw(_playSprite);
        game()->renderWindow()->draw(_levelsSprite);
        game()->renderWindow()->draw(_settingsSprite);
        game()->renderWindow()->draw(_creditsSprite);
        game()->renderWindow()->draw(_exitSprite);

        jl::MainMenu::draw();
    }
    else if(_chooseDifficulty)
    {
        game()->renderWindow()->draw(_frogSprite);
        game()->renderWindow()->draw(_frogText);
        game()->renderWindow()->draw(DIFFICULTY_TEXT);
        game()->renderWindow()->draw(_curDifficulty);
        game()->renderWindow()->draw(_leftArrowSprite4);
        game()->renderWindow()->draw(_rightArrowSprite4);
    }
    else if(_drawSettings)// Settings
    {
        game()->renderWindow()->draw(_settingsBGSprite);

        game()->renderWindow()->draw(ACCEPT_TEXT);
        game()->renderWindow()->draw(DISCARD_TEXT);
        game()->renderWindow()->draw(DEFAULT_TEXT);

        if(!_drawControls)
        {
            game()->renderWindow()->draw(MUSIC_VOLUME_TEXT);
            game()->renderWindow()->draw(_currentMusicVolume);
            game()->renderWindow()->draw(_musicSliderSprite);
            game()->renderWindow()->draw(_musicCursorSprite);
            game()->renderWindow()->draw(SOUND_VOLUME_TEXT);
            game()->renderWindow()->draw(_currentSoundVolume);
            game()->renderWindow()->draw(_soundSliderSprite);
            game()->renderWindow()->draw(_soundCursorSprite);
			game()->renderWindow()->draw(VSYNC_TEXT);
			game()->renderWindow()->draw(_vsyncOnOff);
			game()->renderWindow()->draw(_leftArrowSprite5);
			game()->renderWindow()->draw(_rightArrowSprite5);
            game()->renderWindow()->draw(CUSTOMIZE_CONTROLS_TEXT);
        }
        else
        {
            game()->renderWindow()->draw(_leftArrowSprite);
            game()->renderWindow()->draw(_leftArrowSprite2);
            game()->renderWindow()->draw(_rightArrowSprite);
            game()->renderWindow()->draw(_rightArrowSprite2);
            game()->renderWindow()->draw(KEYBOARD_CONTROLLER_TEXT);

            if(KEYBOARD_CONTROLLER_TEXT.getString().toAnsiString().compare("Controller") == 0)
            {
                game()->renderWindow()->draw(MOVE_SENSITIVITY_TEXT);
                game()->renderWindow()->draw(_moveSense);
                game()->renderWindow()->draw(_leftArrowSprite3);
                game()->renderWindow()->draw(_rightArrowSprite3);
                game()->renderWindow()->draw(_jumpSprite);
                game()->renderWindow()->draw(_grappleSprite);
                game()->renderWindow()->draw(_shootSprite);
            }
            else
            {
                game()->renderWindow()->draw(MOVE_LEFT_TEXT);
                game()->renderWindow()->draw(MOVE_RIGHT_TEXT);
                game()->renderWindow()->draw(_moveLeft);
                game()->renderWindow()->draw(_moveRight);
                game()->renderWindow()->draw(_shoot);
                game()->renderWindow()->draw(_jump);
                game()->renderWindow()->draw(_grapple);
            }

            game()->renderWindow()->draw(SHOOT_TEXT);
            game()->renderWindow()->draw(JUMP_TEXT);
            game()->renderWindow()->draw(GRAPPLE_TEXT);
            game()->renderWindow()->draw(GRAPPLE_SETTING_TEXT);
            game()->renderWindow()->draw(_curGrappleSetting);
        }
    }
    game()->renderWindow()->draw(_version);

    if(_fading) game()->renderWindow()->draw(_fadeSprite);
}

void ZorbitMenu::powerDown()
{
    jl::MainMenu::powerDown();

    _selectionsDesc = NULL;
    _bgDesc = NULL;
    _bg2Desc = NULL;
    _frogDesc = NULL;
    _settingsBGDesc = NULL;
    _sliderDesc = NULL;
    _cursorDesc = NULL;
    _arrowDesc = NULL;
    _buttonsDesc = NULL;
    _cheatInput.clear();
}

void ZorbitMenu::reactivate()
{
    _fading = false;
    _draw = true;
}
