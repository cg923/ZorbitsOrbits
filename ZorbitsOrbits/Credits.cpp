// Copyright (c) 2017 Happy Rock Studios

#include <sstream>
#include <string>

#include "ZorbitsOrbits.h"
#include "Credits.h"
#include "ZSpaceGenerator.h"

Credits::Credits(ZorbitsOrbits * game) : Screen("Credits", game) {}

void Credits::initializeConcrete()
{
    generateCredits();

    _space = new ZSpaceGenerator(this);

    game()->soundMan()->playMusic("credits", false);

    _zorbitsOrbitsDesc   = game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/zorbitsorbits.png", sf::Vector2f(338, 200), 677, 400);
    _zorbitSprite.setTexture(*_zorbitsOrbitsDesc->texture());
    _zorbitSprite.setOrigin(_zorbitsOrbitsDesc->origin());
    _zorbitSprite.setTextureRect(_zorbitsOrbitsDesc->rect());
    _zorbitSprite.setPosition(game()->renderWindow()->getView().getCenter().x,
                       game()->renderWindow()->getView().getCenter().y + 11.5*1080);

    _zorbitDesc   = game()->textureMan()->textureDescIs("resources/zorbitsorbits/menu/zorbit.png", sf::Vector2f(62, 66), 150, 150);
    _zSprite.setTexture(*_zorbitDesc->texture());
    _zSprite.setOrigin(_zorbitDesc->origin());
    _zSprite.setTextureRect(sf::IntRect(0,0,150,149));
    repositionZorbit();

    _fadeDesc   = game()->textureMan()->textureDescIs("resources/zorbitsorbits/fade.png", sf::Vector2f(960, 540), 1920, 1080);
    _fadeSprite.setTexture(*_fadeDesc->texture());
    _fadeSprite.setOrigin(_fadeDesc->origin());
    _fadeSprite.setTextureRect(_fadeDesc->rect());
    _fadeSprite.setPosition(game()->renderWindow()->getView().getCenter().x,
                             game()->renderWindow()->getView().getCenter().y);
    if(game()->renderWindow()->getSize().x > 1920)
        _fadeSprite.scale(float(game()->renderWindow()->getSize().x / 1920), float(game()->renderWindow()->getSize().y / 1080));
    _fadeSprite.setColor(sf::Color(255,255,255,255));


    _finished   = false;
    _ending     = false;

    _timer.pause();
    _fadeTimer.pause();
    _nonResettingTimer.pause();
}

void Credits::reset()
{
    _finished   = false;
    _ending     = false;
    game()->soundMan()->playMusic("credits", false);
}

void Credits::processInput(sf::Event sfevent)
{
    if(_nonResettingTimer.getElapsedTime().asSeconds() < 5) return;
    // Keyboard
    if (sfevent.type == sf::Event::KeyPressed)
    {
        _ending = true;
    }
    // Joystick
    if (sfevent.type == sf::Event::JoystickButtonPressed)
    {
        _ending = true;
    }
}

void Credits::repositionZorbit()
{
    int whichSector = rand() % 8 + 1;
    int xVerseY     = rand() % 100 + 1;
    sf::View view = game()->renderWindow()->getView();
    sf::IntRect rect = sf::IntRect(view.getCenter().x - view.getSize().x / 2,
                                       view.getCenter().y - view.getSize().y / 2,
                                       view.getSize().x,
                                       view.getSize().y);
    switch(whichSector)
    {
    case 1:
        _zSprite.setPosition(rect.left - 200, rect.top - 200);
        _zorbitTrajectory = sf::Vector2f(xVerseY, 100 - xVerseY);
        break;
    case 2:
        _zSprite.setPosition(rect.left + rand() % (rect.width - 150), rect.top - 200);
        if(_zSprite.getPosition().x > rect.left + rect.width / 2)
            _zorbitTrajectory = sf::Vector2f(-xVerseY, 100 - xVerseY);
        else
            _zorbitTrajectory = sf::Vector2f(xVerseY, 100 - xVerseY);
        break;
    case 3:
        _zSprite.setPosition(rect.left + rect.width + 350, rect.top - 200);
        _zorbitTrajectory = sf::Vector2f(-xVerseY, 100 - xVerseY);
        break;
    case 4:
        _zSprite.setPosition(rect.left - 200, rect.top + rand() % (rect.height - 150));
        if(_zSprite.getPosition().y > rect.top + rect.height / 2)
            _zorbitTrajectory = sf::Vector2f(xVerseY, -1 * (100 - xVerseY));
        else
            _zorbitTrajectory = sf::Vector2f(xVerseY, 100 - xVerseY);
        break;
    case 5:
        _zSprite.setPosition(rect.left + rect.width + 350, rect.top + rand() % (rect.height - 150));
        if(_zSprite.getPosition().y > rect.top + rect.height / 2)
            _zorbitTrajectory = sf::Vector2f(-xVerseY, -1 * (100 - xVerseY));
        else
            _zorbitTrajectory = sf::Vector2f(-xVerseY, 100 - xVerseY);
        break;
    case 6:
        _zSprite.setPosition(rect.left - 200, rect.top + rect.height + 200);
        _zorbitTrajectory = sf::Vector2f(xVerseY, -1 *(100 - xVerseY));
        break;
    case 7:
        _zSprite.setPosition(rect.left + rand() % (rect.width - 150), rect.top + rect.height + 200);
        if(_zSprite.getPosition().x > rect.left + rect.width / 2)
            _zorbitTrajectory = sf::Vector2f(-xVerseY, -1 * (100 - xVerseY));
        else
            _zorbitTrajectory = sf::Vector2f(xVerseY, -1 * (100 - xVerseY));
        break;
    case 8:
        _zSprite.setPosition(rect.left + rect.width + 350, rect.top + rect.height + 350);
        _zorbitTrajectory = sf::Vector2f(-xVerseY, -1 * (100 - xVerseY));
        break;
    }

    _zorbitTrajectory = sf::Vector2f(_zorbitTrajectory.x * 0.05, _zorbitTrajectory.y * 0.05);
    _zorbitTimer.reset(true);
}

void Credits::update()
{
    _timer.resume();
    _fadeTimer.resume();
    _nonResettingTimer.resume();

    _fadeSprite.setPosition(game()->renderWindow()->getView().getCenter());

    if(_nonResettingTimer.getElapsedTime().asSeconds() < 2
       && _fadeSprite.getColor().a >= 5)
        _fadeSprite.setColor(sf::Color(255,255,255,_fadeSprite.getColor().a - 5));

    if(_nonResettingTimer.getElapsedTime().asSeconds() > 5
       && _zorbitSprite.getPosition().y > game()->renderWindow()->getView().getCenter().y)
    {
        sf::View view = game()->renderWindow()->getView();
        view.move(0,3*game()->renderWindow()->getSize().y / 1080);
        game()->renderWindow()->setView(view);
    }

    _zSprite.rotate(1);

    sf::FloatRect zorbit = _zSprite.getGlobalBounds();
    sf::View view = game()->renderWindow()->getView();
    if((zorbit.left + zorbit.width >= view.getCenter().x - view.getSize().x / 2
       && zorbit.left <= view.getCenter().x + view.getSize().x / 2
       && zorbit.top + zorbit.height >= view.getCenter().y - view.getSize().y / 2
       && zorbit.top <= view.getCenter().y + view.getSize().y / 2)
       || _zorbitTimer.getElapsedTime().asSeconds() <= 1)
    {
        _zSprite.setPosition(sf::Vector2f(_zSprite.getPosition().x + _zorbitTrajectory.x, _zSprite.getPosition().y + _zorbitTrajectory.y));
    }
    else if(_zorbitTimer.getElapsedTime().asSeconds() > 1) repositionZorbit();

    game()->soundMan()->update();

    if(_ending)
    {
        game()->soundMan()->fadeMusic(3);

        if (_fadeSprite.getColor().a + 3 <= 255)
        {
            _fadeSprite.setColor(sf::Color(255,255,255,_fadeSprite.getColor().a + 3));
        }
        else _finished = true;
    }

    if(_finished)
    {
        game()->stateMan()->activeStateIs("mainmenu");
        game()->soundMan()->playMusic("zorbit");
    }
}

void Credits::draw()
{
    _space->draw();
    std::list<sf::Text>::iterator itr;
    for(itr = _credits.begin(); itr != _credits.end(); itr++)
    {
        game()->renderWindow()->draw(*itr);
    }
    game()->renderWindow()->draw(_zorbitSprite);
    game()->renderWindow()->draw(_zSprite);
    game()->renderWindow()->draw(_fadeSprite);
}

sf::Text Credits::formattedText(std::string str, float yCoord)
{
    sf::Text text;
    text.setFont(*game()->font("foo"));
    text.setCharacterSize(50);
    text.setColor(sf::Color(255,255,255,255));
    text.setString(str);
    text.setPosition(game()->renderWindow()->getView().getCenter().x - text.getLocalBounds().width / 2,
                     yCoord);
    return text;
}

void Credits::generateCredits()
{
    _credits.push_back(formattedText("Zorbit\'s Orbits Created By", game()->renderWindow()->getView().getCenter().y - 100));
    _credits.push_back(formattedText("Corey Selover & Jeremie Duval", game()->renderWindow()->getView().getCenter().y - 50));

    _credits.push_back(formattedText("Lead Programmer", game()->renderWindow()->getView().getCenter().y + 1080));
    _credits.push_back(formattedText("Corey Selover", game()->renderWindow()->getView().getCenter().y + 1080 + 50));

    _credits.push_back(formattedText("Additional Programming", game()->renderWindow()->getView().getCenter().y + 1.25*1080));
    _credits.push_back(formattedText("Chris Dembia", game()->renderWindow()->getView().getCenter().y + 1.25*1080 + 50));

    _credits.push_back(formattedText("Lead Artist", game()->renderWindow()->getView().getCenter().y + 2*1080));
    _credits.push_back(formattedText("Jeremie Duval", game()->renderWindow()->getView().getCenter().y + 2*1080 + 50));

    _credits.push_back(formattedText("Additional Art", game()->renderWindow()->getView().getCenter().y + 2.25*1080));
    _credits.push_back(formattedText("David Thor Fjalarsson", game()->renderWindow()->getView().getCenter().y + 2.25*1080 + 50));

    _credits.push_back(formattedText("Music and Sound", game()->renderWindow()->getView().getCenter().y + 3*1080));
    _credits.push_back(formattedText("Corey Selover", game()->renderWindow()->getView().getCenter().y + 3*1080 + 50));

    _credits.push_back(formattedText("Audio Engineering", game()->renderWindow()->getView().getCenter().y + 3.25*1080));
    _credits.push_back(formattedText("Andrea Allmond", game()->renderWindow()->getView().getCenter().y + 3.25*1080 + 50));

    _credits.push_back(formattedText("Testers", game()->renderWindow()->getView().getCenter().y + 3.75*1080));
    _credits.push_back(formattedText("Erik Anson", game()->renderWindow()->getView().getCenter().y + 3.75*1080 + 50));
    _credits.push_back(formattedText("Nathan Burgdorff", game()->renderWindow()->getView().getCenter().y + 3.75*1080 + 100));
    _credits.push_back(formattedText("Adam Franco", game()->renderWindow()->getView().getCenter().y + 3.75*1080 + 150));
    _credits.push_back(formattedText("Eli Madden", game()->renderWindow()->getView().getCenter().y + 3.75*1080 + 200));
    _credits.push_back(formattedText("Matt Paragano", game()->renderWindow()->getView().getCenter().y + 3.75*1080 + 250));
    _credits.push_back(formattedText("Roger Poon", game()->renderWindow()->getView().getCenter().y + 3.75*1080 + 300));
    _credits.push_back(formattedText("Ben Rosenthal", game()->renderWindow()->getView().getCenter().y + 3.75*1080 + 350));
    _credits.push_back(formattedText("Chad Sano", game()->renderWindow()->getView().getCenter().y + 3.75*1080 + 400));
    _credits.push_back(formattedText("Nathaniel Snider", game()->renderWindow()->getView().getCenter().y + 3.75*1080 + 450));
    _credits.push_back(formattedText("Reade Vaisman", game()->renderWindow()->getView().getCenter().y + 3.75*1080 + 500));

    _credits.push_back(formattedText("Technical Assistance", game()->renderWindow()->getView().getCenter().y + 4.5*1080));
    _credits.push_back(formattedText("(Whether they knew it or not!)", game()->renderWindow()->getView().getCenter().y + 4.5*1080 + 50));
    _credits.push_back(formattedText("C++ Forums Members:", game()->renderWindow()->getView().getCenter().y + 4.5*1080 + 125));
    _credits.push_back(formattedText("clanmjc", game()->renderWindow()->getView().getCenter().y + 4.5*1080 + 175));
    _credits.push_back(formattedText("Cubbi", game()->renderWindow()->getView().getCenter().y + 4.5*1080 + 225));
    _credits.push_back(formattedText("hamsterman", game()->renderWindow()->getView().getCenter().y + 4.5*1080 + 275));
    _credits.push_back(formattedText("iHutch105", game()->renderWindow()->getView().getCenter().y + 4.5*1080 + 325));
    _credits.push_back(formattedText("ne555", game()->renderWindow()->getView().getCenter().y + 4.5*1080 + 375));
    _credits.push_back(formattedText("Peter87", game()->renderWindow()->getView().getCenter().y + 4.5*1080 + 425));
    _credits.push_back(formattedText("SFML Forums Members:", game()->renderWindow()->getView().getCenter().y + 4.5*1080 + 500));
    _credits.push_back(formattedText("aBallofWin", game()->renderWindow()->getView().getCenter().y + 4.5*1080 + 550));
    _credits.push_back(formattedText("eXpl0it3r", game()->renderWindow()->getView().getCenter().y + 4.5*1080 + 600));
    _credits.push_back(formattedText("Haikarainen", game()->renderWindow()->getView().getCenter().y + 4.5*1080 + 650));
    _credits.push_back(formattedText("Hapax", game()->renderWindow()->getView().getCenter().y + 4.5*1080 + 700));
    _credits.push_back(formattedText("Laurent", game()->renderWindow()->getView().getCenter().y + 4.5*1080 + 750));
    _credits.push_back(formattedText("Perde", game()->renderWindow()->getView().getCenter().y + 4.5*1080 + 800));

    _credits.push_back(formattedText("GoFundMe Supporters", game()->renderWindow()->getView().getCenter().y + 5.75*1080));
    _credits.push_back(formattedText("Daniel Aibar", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 50));
    _credits.push_back(formattedText("Jen Angeli", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 100));
    _credits.push_back(formattedText("Chris Barrett", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 150));
    _credits.push_back(formattedText("Erik Brooks", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 200));
    _credits.push_back(formattedText("Nathan Burgdorff", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 250));
    _credits.push_back(formattedText("Coconut The Cat", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 300));
    _credits.push_back(formattedText("Spencer Church", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 350));
    _credits.push_back(formattedText("Chris Dembia", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 400));
    _credits.push_back(formattedText("Keri Fair", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 450));
    _credits.push_back(formattedText("Alan Fishbone", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 500));
    _credits.push_back(formattedText("Adam Franco", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 550));
    _credits.push_back(formattedText("Elliott Fox", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 600));
    _credits.push_back(formattedText("David Hamberlin", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 650));
    _credits.push_back(formattedText("Richard Herold", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 700));
    _credits.push_back(formattedText("Alexandra King", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 750));
    _credits.push_back(formattedText("Jean Lin", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 800));
    _credits.push_back(formattedText("Mia Monnier", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 850));
    _credits.push_back(formattedText("Dan Murphy", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 900));
    _credits.push_back(formattedText("Hannah Rae Murphy", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 950));
    _credits.push_back(formattedText("Matt Paragano", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 1000));
    _credits.push_back(formattedText("Dean Pierce", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 1050));
    _credits.push_back(formattedText("Laura Pollard", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 1100));
    _credits.push_back(formattedText("Roger Poon", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 1150));
    _credits.push_back(formattedText("Bard Ricciardi", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 1200));
    _credits.push_back(formattedText("Ben Rosenthal", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 1250));
    _credits.push_back(formattedText("Joe Schine", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 1300));
    _credits.push_back(formattedText("Nick Schon", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 1350));
    _credits.push_back(formattedText("Rob Selover", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 1400));
    _credits.push_back(formattedText("Tom Silver", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 1450));
    _credits.push_back(formattedText("Avery Starkey", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 1500));
    _credits.push_back(formattedText("Jeffrey Stauch", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 1550));
    _credits.push_back(formattedText("Will Tristram", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 1600));
    _credits.push_back(formattedText("Becky Trombley", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 1650));
    _credits.push_back(formattedText("Willy The Turtle", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 1700));
    _credits.push_back(formattedText("Annie Ulrich", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 1750));
    _credits.push_back(formattedText("Jessica Vankempen", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 1800));
    _credits.push_back(formattedText("Holly Weir", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 1850));
    _credits.push_back(formattedText("Linda White", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 1900));
    _credits.push_back(formattedText("Meaghan Wilbur", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 1950));
    _credits.push_back(formattedText("Charles Wulster", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 2000));
    _credits.push_back(formattedText("Yuki Yoshida", game()->renderWindow()->getView().getCenter().y + 5.75*1080 + 2050));

    _credits.push_back(formattedText("Corey thanks", game()->renderWindow()->getView().getCenter().y + 8*1080));
    _credits.push_back(formattedText("Ashlyn Aiello", game()->renderWindow()->getView().getCenter().y + 8*1080 + 50));
    _credits.push_back(formattedText("Babatunde Bello", game()->renderWindow()->getView().getCenter().y + 8*1080 + 100));
    _credits.push_back(formattedText("John Castronuovo", game()->renderWindow()->getView().getCenter().y + 8*1080 + 150));
    _credits.push_back(formattedText("Brendan Collins", game()->renderWindow()->getView().getCenter().y + 8*1080 + 200));
    _credits.push_back(formattedText("Max Cooper", game()->renderWindow()->getView().getCenter().y + 8*1080 + 250));
    _credits.push_back(formattedText("Nana Essuman", game()->renderWindow()->getView().getCenter().y + 8*1080 + 300));
    _credits.push_back(formattedText("Bill Heminway", game()->renderWindow()->getView().getCenter().y + 8*1080 + 350));
    _credits.push_back(formattedText("Yanlin Ho", game()->renderWindow()->getView().getCenter().y + 8*1080 + 400));
    _credits.push_back(formattedText("Jaeduk Kim", game()->renderWindow()->getView().getCenter().y + 8*1080 + 450));
    _credits.push_back(formattedText("William Kim", game()->renderWindow()->getView().getCenter().y + 8*1080 + 500));
    _credits.push_back(formattedText("Angela Kim", game()->renderWindow()->getView().getCenter().y + 8*1080 + 550));
    _credits.push_back(formattedText("Chase Lawrence", game()->renderWindow()->getView().getCenter().y + 8*1080 + 600));
    _credits.push_back(formattedText("Maggie Lawrence", game()->renderWindow()->getView().getCenter().y + 8*1080 + 650));
    _credits.push_back(formattedText("Pam Lawrence", game()->renderWindow()->getView().getCenter().y + 8*1080 + 700));
    _credits.push_back(formattedText("Stew Lawrence", game()->renderWindow()->getView().getCenter().y + 8*1080 + 750));
    _credits.push_back(formattedText("Molly Magnell", game()->renderWindow()->getView().getCenter().y + 8*1080 + 800));
    _credits.push_back(formattedText("Miles Manns", game()->renderWindow()->getView().getCenter().y + 8*1080 + 850));
    _credits.push_back(formattedText("Amo Manuel", game()->renderWindow()->getView().getCenter().y + 8*1080 + 900));
    _credits.push_back(formattedText("Madeleine McClintic", game()->renderWindow()->getView().getCenter().y + 8*1080 + 950));
    _credits.push_back(formattedText("Alex Morrow", game()->renderWindow()->getView().getCenter().y + 8*1080 + 1000));
    _credits.push_back(formattedText("Danielle Nieves", game()->renderWindow()->getView().getCenter().y + 8*1080 + 1050));
    _credits.push_back(formattedText("Chia Okorie", game()->renderWindow()->getView().getCenter().y + 8*1080 + 1100));
    _credits.push_back(formattedText("Krissy Pelley", game()->renderWindow()->getView().getCenter().y + 8*1080 + 1150));
    _credits.push_back(formattedText("Robert Rush", game()->renderWindow()->getView().getCenter().y + 8*1080 + 1200));
    _credits.push_back(formattedText("Indira Sanchez", game()->renderWindow()->getView().getCenter().y + 8*1080 + 1250));
    _credits.push_back(formattedText("Joe Schine", game()->renderWindow()->getView().getCenter().y + 8*1080 + 1300));
    _credits.push_back(formattedText("Casey Selover", game()->renderWindow()->getView().getCenter().y + 8*1080 + 1350));
    _credits.push_back(formattedText("Ed Selover", game()->renderWindow()->getView().getCenter().y + 8*1080 + 1400));
    _credits.push_back(formattedText("Rob Selover", game()->renderWindow()->getView().getCenter().y + 8*1080 + 1450));
    _credits.push_back(formattedText("Whitney Selover", game()->renderWindow()->getView().getCenter().y + 8*1080 + 1500));
    _credits.push_back(formattedText("Simon Sharp", game()->renderWindow()->getView().getCenter().y + 8*1080 + 1550));
    _credits.push_back(formattedText("Michael St. Louis", game()->renderWindow()->getView().getCenter().y + 8*1080 + 1600));
    _credits.push_back(formattedText("Jeffrey Stauch", game()->renderWindow()->getView().getCenter().y + 8*1080 + 1650));
    _credits.push_back(formattedText("Jeremy Teicher", game()->renderWindow()->getView().getCenter().y + 8*1080 + 1700));
    _credits.push_back(formattedText("Jim Teicher", game()->renderWindow()->getView().getCenter().y + 8*1080 + 1750));
    _credits.push_back(formattedText("Mala Teicher", game()->renderWindow()->getView().getCenter().y + 8*1080 + 1800));
    _credits.push_back(formattedText("Katherine Tobeason", game()->renderWindow()->getView().getCenter().y + 8*1080 + 1850));
    _credits.push_back(formattedText("Becky Trombley", game()->renderWindow()->getView().getCenter().y + 8*1080 + 1900));
    _credits.push_back(formattedText("Mallory Tyler", game()->renderWindow()->getView().getCenter().y + 8*1080 + 1950));
    _credits.push_back(formattedText("Annie Ulrich", game()->renderWindow()->getView().getCenter().y + 8*1080 + 2000));
    _credits.push_back(formattedText("Jacquie Werner-Gavrin", game()->renderWindow()->getView().getCenter().y + 8*1080 + 2050));
    _credits.push_back(formattedText("Sydni White", game()->renderWindow()->getView().getCenter().y + 8*1080 + 2100));
    _credits.push_back(formattedText("Kelly Xia", game()->renderWindow()->getView().getCenter().y + 8*1080 + 2150));
    _credits.push_back(formattedText("All of the students at the Red Cedar School", game()->renderWindow()->getView().getCenter().y + 8*1080 + 2200));

    _credits.push_back(formattedText("Jeremie thanks", game()->renderWindow()->getView().getCenter().y + 10.25*1080));
    _credits.push_back(formattedText("Lisa Love", game()->renderWindow()->getView().getCenter().y + 10.25*1080 + 50));
    _credits.push_back(formattedText("Selina", game()->renderWindow()->getView().getCenter().y + 10.25*1080 + 100));
    _credits.push_back(formattedText("Meaghen Wilbur", game()->renderWindow()->getView().getCenter().y + 10.25*1080 + 150));
    _credits.push_back(formattedText("Avery and Tom", game()->renderWindow()->getView().getCenter().y + 10.25*1080 + 200));
    _credits.push_back(formattedText("Jose and Blake", game()->renderWindow()->getView().getCenter().y + 10.25*1080 + 250));
    _credits.push_back(formattedText("Mammie Aiiache", game()->renderWindow()->getView().getCenter().y + 10.25*1080 + 300));
    _credits.push_back(formattedText("Annie Hanson", game()->renderWindow()->getView().getCenter().y + 10.25*1080 + 350));
    _credits.push_back(formattedText("Weekly Planet", game()->renderWindow()->getView().getCenter().y + 10.25*1080 + 400));
    _credits.push_back(formattedText("Conan who gave inspiration", game()->renderWindow()->getView().getCenter().y + 10.25*1080 + 450));
    _credits.push_back(formattedText("Our friends, fans, and family", game()->renderWindow()->getView().getCenter().y + 10.25*1080 + 500));
    _credits.push_back(formattedText("Our parents and siblings", game()->renderWindow()->getView().getCenter().y + 10.25*1080 + 550));
    _credits.push_back(formattedText("And all those who believed in us :)", game()->renderWindow()->getView().getCenter().y + 10.25*1080 + 600));
}

void Credits::powerDown()
{
    _zorbitsOrbitsDesc = NULL;
    delete _space;
}
