
#include <sstream>

#include "FrogScreen.h"
#include "ZorbitsOrbits.h"
#include "LevelSelect.h"

FrogScreen::FrogScreen(ZorbitsOrbits* game) : jl::Screen("FrogScreen", game)
{
}

void FrogScreen::initializeConcrete()
{
    float width  = float(game()->renderWindow()->getSize().x) / 1920.0f;
    float height = float(game()->renderWindow()->getSize().y) / 1080.0f;

    _scale = sf::Vector2f(width,height);

    jl::TextureDesc::Ptr bgDesc = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/frog/bg.png",
        sf::Vector2f(960, 540), 1920, 1080);

    jl::TextureDesc::Ptr buttonsDesc = this->game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/buttons.png",
        sf::Vector2f(100, 25), 450, 50);

    // SPRITES
    _bgSprite.setTexture(*bgDesc->texture());
    _bgSprite.setOrigin(bgDesc->origin());
    _bgSprite.setPosition(game()->renderWindow()->getView().getCenter());
    _bgSprite.scale(_scale.x, _scale.y);

    _backSprite.setTexture(*buttonsDesc->texture());
    _backSprite.setOrigin(0,0);
    _backSprite.setPosition(game()->renderWindow()->getView().getCenter().x - 300*_scale.x,
                            game()->renderWindow()->getSize().y - 100*_scale.y);

    // TEXT OBJECTS
    _backText.setFont(*game()->font("foo"));
    _backText.setCharacterSize(int(40*_scale.x));
    _backText.setColor(sf::Color::White);
    _backText.setPosition(_backSprite.getPosition() + sf::Vector2f(50, 0));
    _backText.setString("Back");

    _cogsCollectedText.setFont(*game()->font("foo"));
    _cogsCollectedText.setCharacterSize(int(60*_scale.x));
    _cogsCollectedText.setFillColor(sf::Color::White);
    _cogsCollectedText.setOutlineColor(sf::Color::Black);
    _cogsCollectedText.setOutlineThickness(4);
    _cogsCollectedText.setPosition(620*_scale.x, 300*_scale.y);

    _sillyText.setFont(*game()->font("foo"));
    _sillyText.setCharacterSize(int(60*_scale.x));
    _sillyText.setFillColor(sf::Color(255,255,255,80));
    _sillyText.setOutlineColor(sf::Color::Black);
    _sillyText.setOutlineThickness(4);
    _sillyText.setPosition(340*_scale.x, 420*_scale.y);
    _sillyText.setString("Collect 300 cogs to unlock!");

    _on1Text.setFont(*game()->font("foo"));
    _on1Text.setCharacterSize(int(60*_scale.x));
    _on1Text.setFillColor(sf::Color(255,255,255,80));
    _on1Text.setOutlineColor(sf::Color::Black);
    _on1Text.setOutlineThickness(4);
    _on1Text.setPosition(500*_scale.x, 490*_scale.y);
    _on1Text.setString("On");

    _off1Text.setFont(*game()->font("foo"));
    _off1Text.setCharacterSize(int(60*_scale.x));
    _off1Text.setFillColor(sf::Color(255,255,255,255));
    _off1Text.setOutlineColor(sf::Color::Black);
    _off1Text.setOutlineThickness(4);
    _off1Text.setPosition(630*_scale.x, 490*_scale.y);
    _off1Text.setString("Off");

    _fastText.setFont(*game()->font("foo"));
    _fastText.setCharacterSize(int(60*_scale.x));
    _fastText.setFillColor(sf::Color(255,255,255,80));
    _fastText.setOutlineColor(sf::Color::Black);
    _fastText.setOutlineThickness(4);
    _fastText.setPosition(340*_scale.x, 600*_scale.y);
    _fastText.setString("Collect 500 cogs to unlock!");

    _on2Text.setFont(*game()->font("foo"));
    _on2Text.setCharacterSize(int(60*_scale.x));
    _on2Text.setFillColor(sf::Color(255,255,255,80));
    _on2Text.setOutlineColor(sf::Color::Black);
    _on2Text.setOutlineThickness(4);
    _on2Text.setPosition(500*_scale.x, 670*_scale.y);
    _on2Text.setString("On");

    _off2Text.setFont(*game()->font("foo"));
    _off2Text.setCharacterSize(int(60*_scale.x));
    _off2Text.setFillColor(sf::Color(255,255,255,255));
    _off2Text.setOutlineColor(sf::Color::Black);
    _off2Text.setOutlineThickness(4);
    _off2Text.setPosition(630*_scale.x, 670*_scale.y);
    _off2Text.setString("Off");

    _vexText.setFont(*game()->font("foo"));
    _vexText.setCharacterSize(int(60*_scale.x));
    _vexText.setFillColor(sf::Color(255,255,255,80));
    _vexText.setOutlineColor(sf::Color::Black);
    _vexText.setOutlineThickness(4);
    _vexText.setPosition(340*_scale.x, 780*_scale.y);
    _vexText.setString("Collect 887 cogs to unlock!");

    _on3Text.setFont(*game()->font("foo"));
    _on3Text.setCharacterSize(int(60*_scale.x));
    _on3Text.setFillColor(sf::Color(255,255,255,80));
    _on3Text.setOutlineColor(sf::Color::Black);
    _on3Text.setOutlineThickness(4);
    _on3Text.setPosition(500*_scale.x, 850*_scale.y);
    _on3Text.setString("On");

    _off3Text.setFont(*game()->font("foo"));
    _off3Text.setCharacterSize(int(60*_scale.x));
    _off3Text.setFillColor(sf::Color(255,255,255,255));
    _off3Text.setOutlineColor(sf::Color::Black);
    _off3Text.setOutlineThickness(4);
    _off3Text.setPosition(630*_scale.x, 850*_scale.y);
    _off3Text.setString("Off");

    _frogEText.setFont(*game()->font("foo"));
    _frogEText.setCharacterSize(int(60*_scale.x));
    _frogEText.setFillColor(sf::Color::Black);
    _frogEText.setOutlineColor(sf::Color::Black);
    _frogEText.setOutlineThickness(0);
    _frogEText.setPosition(1380*_scale.x, 450*_scale.y);
    generateFrogText();

    _currentSelection   = Silly;
    _next               = 0;
    _previous           = 0;
    _nextX              = 0;
    _previousX          = 0;
    _cogsCollected      = 0;
}

void FrogScreen::processInput(sf::Event event)
{
    // Keyboard
    if (event.type == sf::Event::KeyPressed)
    {
        switch(event.key.code)
        {
            case sf::Keyboard::Escape:
                game()->soundMan()->playSound("cancel");
                game()->stateMan()->activeStateIs("LevelSelect");
                break;
            case sf::Keyboard::Up:
                prevSelection();
                break;
            case sf::Keyboard::Down:
                nextSelection();
                break;
            case sf::Keyboard::Left:
                switchActivation();
                break;
            case sf::Keyboard::Right:
                switchActivation();
                break;
            case sf::Keyboard::V:
                static_cast<ZorbitsOrbits*>(game())->_vex = true;
                break;
            default:
                break;
        }
    }
    // Controller
    if (event.type == sf::Event::JoystickButtonPressed)
    {
        if(event.joystickButton.button == 1)
        {
            game()->soundMan()->playSound("cancel");
            game()->stateMan()->activeStateIs("LevelSelect");
        }
    }
    if (event.type == sf::Event::JoystickMoved)
    {
        // Y Axis
        if (event.joystickMove.axis == sf::Joystick::Y)
        {
            if (event.joystickMove.position > 70)
            {
                if(_next == 0)
                {
                    nextSelection();
                }
                _next++;
            }
            if (event.joystickMove.position < -70)
            {
                if(_previous == 0)
                {
                    prevSelection();
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
                    switchActivation();
                }
                _nextX++;
            }
            if (event.joystickMove.position < -70)
            {
                if(_previousX == 0)
                {
                    switchActivation();
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
}

void FrogScreen::prevSelection()
{
    switch(_currentSelection)
    {
    case Silly:
        break;
    case Fast:
        _currentSelection = Silly;
        break;
    case Vex:
        _currentSelection = Fast;
        break;
    default:
        break;
    }
}

void FrogScreen::nextSelection()
{
    switch(_currentSelection)
    {
    case Silly:
        if(_cogsCollected >= 500)
            _currentSelection = Fast;
        break;
    case Fast:
        if(_cogsCollected == 887)
            _currentSelection = Vex;
        break;
    case Vex:
        break;
    default:
        break;
    }
}

void FrogScreen::switchActivation()
{
    switch(_currentSelection)
    {
    case Silly:
        if(!static_cast<ZorbitsOrbits*>(game())->_sillyMode)
        {
            _on1Text.setFillColor(sf::Color(255,255,255,255));
            _off1Text.setFillColor(sf::Color(255,255,255,80));
            static_cast<ZorbitsOrbits*>(game())->_sillyMode = true;
        }
        else
        {
            _on1Text.setFillColor(sf::Color(255,255,255,80));
            _off1Text.setFillColor(sf::Color(255,255,255,255));
            static_cast<ZorbitsOrbits*>(game())->_sillyMode = false;
        }
        break;
    case Fast:
        if(!static_cast<ZorbitsOrbits*>(game())->_fastMode)
        {
            _on2Text.setFillColor(sf::Color(255,255,255,255));
            _off2Text.setFillColor(sf::Color(255,255,255,80));
            static_cast<ZorbitsOrbits*>(game())->_fastMode = true;
        }
        else
        {
            _on2Text.setFillColor(sf::Color(255,255,255,80));
            _off2Text.setFillColor(sf::Color(255,255,255,255));
            static_cast<ZorbitsOrbits*>(game())->_fastMode = false;
        }
        break;
    case Vex:
        if(!static_cast<ZorbitsOrbits*>(game())->_vex)
        {
            _on3Text.setFillColor(sf::Color(255,255,255,255));
            _off3Text.setFillColor(sf::Color(255,255,255,80));
            static_cast<ZorbitsOrbits*>(game())->_vex = true;
        }
        else
        {
            _on3Text.setFillColor(sf::Color(255,255,255,80));
            _off3Text.setFillColor(sf::Color(255,255,255,255));
            static_cast<ZorbitsOrbits*>(game())->_vex = false;
        }
        break;
    }
}

void FrogScreen::generateFrogText()
{
    srand(time(0));
    int saying = rand() % 15 + 1;
    std::string sayString;

    switch(saying)
    {
    case 1:
        sayString = "Got a problem?\nFrog-E Fix it.";
        break;
    case 2:
        sayString = "Real classy,\nPal.";
        break;
    case 3:
        sayString = "Hurry up kid,\nI ain\'t got\nall day.";
        break;
    case 4:
        sayString = "You gonna\ngimme some\ncogs or what?";
        break;
    case 5:
        sayString = "You\'re my worst\ncustomer, \'n that\'s\nsaying something.";
        break;
    case 6:
        sayString = "You look terrible.";
        break;
    case 7:
        sayString = "I need a\nnew gig.";
        break;
    case 8:
        sayString = "You touch it,\nyou buy it.";
        break;
    case 9:
        sayString = "Customers\nmust wash\nhands.";
        break;
    case 10:
        sayString = "Not you again.";
        break;
    case 11:
        sayString = "Quiet, I\'m\nwatchin\' the\nYets game.";
        break;
    case 12:
        sayString = "I\'ve seen\nworse, but\nnot lately.";
        break;
    case 13:
        sayString = "MISSINGNO.";
        break;
    case 14:
        sayString = "Sorry, kid.\nI don\'t sell frog\n protection.";
        break;
    case 15:
        sayString = "Lighten up.";
        break;
    default:
        sayString = "Default case?\nHow\'d you\ndo that?";
        break;
    }
    _frogEText.setString(sayString);
}

void FrogScreen::update()
{
    _cogsCollected = static_cast<LevelSelectScreen*>(game()->stateMan()->state("LevelSelect").ptr())->totalCogsCollected();

    std::stringstream ss;
    ss << _cogsCollected << " / 887";
    _cogsCollectedText.setString(ss.str());

    if(_cogsCollected >= 300)
    {
        _sillyText.setString("Silly mode");
        if(_currentSelection == Silly)
            _sillyText.setFillColor(sf::Color(255,255,255,255));
        else _sillyText.setFillColor(sf::Color(255,255,255,80));
    }
    if(_cogsCollected >= 500)
    {
        _fastText.setString("Fast mode");
        if(_currentSelection == Fast)
            _fastText.setFillColor(sf::Color(255,255,255,255));
        else _fastText.setFillColor(sf::Color(255,255,255,80));
    }
    if(_cogsCollected == 887)
    {
        _vexText.setString("Play as Vex");
        if(_currentSelection == Vex)
            _vexText.setFillColor(sf::Color(255,255,255,255));
        else _vexText.setFillColor(sf::Color(255,255,255,80));
    }

    if(sf::Joystick::isConnected(0))
    {
        _backSprite.setTextureRect(sf::IntRect(50,0,50,50));
    }
    else
    {
        _backSprite.setTextureRect(sf::IntRect(350,0,50,50));
    }

}

void FrogScreen::draw()
{
    sf::View view = game()->renderWindow()->getDefaultView();
    game()->renderWindow()->setView(view);

    game()->renderWindow()->draw(_bgSprite);
    game()->renderWindow()->draw(_cogsCollectedText);
    game()->renderWindow()->draw(_sillyText);
    game()->renderWindow()->draw(_fastText);
    game()->renderWindow()->draw(_vexText);
    game()->renderWindow()->draw(_backText);
    game()->renderWindow()->draw(_backSprite);
    game()->renderWindow()->draw(_frogEText);

    if(_cogsCollected >= 300)
    {
        game()->renderWindow()->draw(_on1Text);
        game()->renderWindow()->draw(_off1Text);
    }
    if(_cogsCollected >= 500)
    {
        game()->renderWindow()->draw(_on2Text);
        game()->renderWindow()->draw(_off2Text);
    }
    if(_cogsCollected == 887)
    {
        game()->renderWindow()->draw(_on3Text);
        game()->renderWindow()->draw(_off3Text);
    }
}

