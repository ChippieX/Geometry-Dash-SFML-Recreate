/*
This program is designed to replicate the mobile game Geometry Dash by
RobTopGames using SFML. This was my final project for my introductory C++
class

Geometry Dash is NOT made, designed, or published by me. The same thing can be
said about the soundracks used, those being Stereo Madness by ForeverBound, and
March of the Spoons by Kevin McCloud. This recreation was made as a personal
project to test myself with SFML and C++, and not for any sort of personal 
gain or profit.
*/

/*
    menu guide:

    menu 0 is the main menu
    menu 1 is the level menu
    menu 2 loads the first level
    menu 3 and 4 are for unfinished levels
    (I never actually got to work on those, sorry :/)
    menu 5 loads the custom level
*/
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>
#include <math.h>
#include <string.h>

const int SIZE_X = 1000, SIZE_Y = 10;

void createText(sf::Text&, sf::Font&);
void textStringUpdate(sf::Text&, std::string, int);
void createRect(sf::RectangleShape&, int, int);
void setLevel(char level[][SIZE_Y], std::string, int&);
void setEnd(char level[][SIZE_Y], int& endPos);
void levelDraw(sf::Sprite, sf::Sprite, sf::Sprite, sf::Sprite,
    sf::RectangleShape, sf::RenderWindow&, char level[][SIZE_Y],
    float, int, int);
bool buttonPressed(sf::RectangleShape, sf::RenderWindow&);
bool drawCheck(sf::RectangleShape, sf::RenderWindow&, int, int, int, int);
bool collision(sf::RectangleShape, sf::RectangleShape);
bool blockCollision(sf::RectangleShape, sf::RectangleShape);
bool groundCollision(sf::RectangleShape, sf::RectangleShape);
bool levelCollision(sf::RectangleShape, sf::RectangleShape, sf::RectangleShape,
    sf::RectangleShape, sf::RectangleShape, sf::RenderWindow&,
    char level[][SIZE_Y], float, int&, int, bool&);
bool onGround(sf::RectangleShape, sf::RectangleShape);
bool onBlock(sf::RectangleShape, sf::RectangleShape);
int newRotation(sf::Sprite icon);

int main()
{
    //@var
    const int GROUND_COUNT = 8;
    float LEVEL_SPEED_DEFAULT = 9.6f;
    sf::RenderWindow window(sf::VideoMode(1280, 720),
        "Geometry Dash Education Edition");
    sf::Font pusab;
    sf::Text mainTitle, mainSubTitle, mainButtonText, secButtonText,
        thrButtonText, fourButtonText, exitText, pauseButtonsText[3],
        pausedText;
    sf::RectangleShape mainButton, secButton, thrButton, fourButton,
        pauseButtons[3], groundLine, iconHitbox, iconBlockHitbox,
        spikeHitbox, blockHitbox, shortSpikeHitbox, pauseMenu;
    sf::Texture backgroundImage, iconImage, groundImage, spikeImage,
        blockImage, shortSpikeImage, endwallImage;
    sf::Sprite background[2], icon, ground[GROUND_COUNT], block, spike,
        shortSpike, endwall;
    sf::Clock clock;
    sf::SoundBuffer menuLoopBuffer, selectBuffer, deathBuffer;
    sf::Sound menuLoop, select, death;
    sf::Music stereoMadness, customSong;
    bool mouseHeld = false, isGrounded = true, jumpCheck = false,
        iconAlive = true, fall = false, paused = false, pausedReleased,
        end = false, winText = false, escText = false,
        blockCollisionTest = false;
    int menu = 0, menuChange = 0, iconDefaultY, iconGroundDefaultY,
        endPos = 0, aniEndPos = window.getSize().y / 2,
        aniScale = 1.0 / 4, aniStartPos, aniVariable;
    float bgPosition[2]{ 0, -1330 }, levelSpeed = LEVEL_SPEED_DEFAULT,
        levelPos = 0, jumpCount = 0, jumpHeight = 0;
    char level[SIZE_X][SIZE_Y]{};

    //setting framerate
    window.setFramerateLimit(60);

    //loading font "pusab"
    if (!pusab.loadFromFile("pusab.otf")) {}

    //loading the background
    if (!backgroundImage.loadFromFile("background.png")) {}
    backgroundImage.setSmooth(true);
    for (int i = 0; i < 2; i++)
    {
        background[i].setTexture(backgroundImage);
        background[i].setScale(sf::Vector2f(0.65, 0.6));
        background[i].setOrigin(background[i].getLocalBounds().width / 2,
            background[i].getLocalBounds().height / 2);
        background[i].setPosition(window.getSize().x / 2,
            window.getSize().y / 2);
        background[i].setColor(sf::Color(0, 150, 255));
    }

    //main text setup
    createText(mainTitle, pusab);
    textStringUpdate(mainTitle, "Geometry Dash", 65);
    mainTitle.setPosition(window.getSize().x / 2, 100);

    //second text setup
    createText(mainSubTitle, pusab);
    textStringUpdate(mainSubTitle, "Education Edition", 35);
    mainSubTitle.setPosition(window.getSize().x / 2, 150);

    //main button text setup
    createText(mainButtonText, pusab);
    textStringUpdate(mainButtonText, "Play", 35);
    mainButtonText.setPosition(window.getSize().x / 2, window.getSize().y / 2);

    //pause menu text setup
    for (int i = 0; i < 3; i++)
        createText(pauseButtonsText[i], pusab);
    textStringUpdate(pauseButtonsText[0], "Resume", 25);
    textStringUpdate(pauseButtonsText[1], "Exit", 25);
    textStringUpdate(pauseButtonsText[2], "Restart", 25);

    //other button text setups, not shown on first menu
    createText(secButtonText, pusab);
    textStringUpdate(secButtonText, "ABC", 35);
    createText(thrButtonText, pusab);
    textStringUpdate(thrButtonText, "ABC", 35);
    createText(fourButtonText, pusab);
    textStringUpdate(fourButtonText, "ABC", 35);
    createText(exitText, pusab);
    textStringUpdate(exitText, "ABC", 35);

    //pause menu large text setup
    createText(pausedText, pusab);
    textStringUpdate(pausedText, "Paused", 100);
    pausedText.setPosition(window.getSize().x / 2, window.getSize().y / 6);

    //main button setup
    createRect(mainButton, 400, 100);
    mainButton.setPosition(window.getSize().x / 2, window.getSize().y / 2);

    //pause button setup
    for (int i = 0; i < 3; i++)
        createRect(pauseButtons[i], 200, 200);
    pauseButtons[0].setPosition(window.getSize().x / 2,
        window.getSize().y / 2);
    pauseButtonsText[0].setPosition(window.getSize().x / 2,
        window.getSize().y / 2);
    pauseButtons[1].setPosition(window.getSize().x / 2 - 400,
        window.getSize().y / 2);
    pauseButtonsText[1].setPosition(window.getSize().x / 2
        - 400, window.getSize().y / 2);
    pauseButtons[2].setPosition(window.getSize().x / 2 + 400,
        window.getSize().y / 2);
    pauseButtonsText[2].setPosition(window.getSize().x / 2
        + 400, window.getSize().y / 2);

    //other button setups
    createRect(secButton, 400, 100);
    createRect(thrButton, 400, 100);
    createRect(fourButton, 400, 100);

    //setup for the pause menu background
    createRect(pauseMenu, 1152, 648);
    pauseMenu.setFillColor(sf::Color(50, 50, 50, 200));
    pauseMenu.setPosition(window.getSize().x / 2, window.getSize().y / 2);

    //@icon
    //load icon texture
    if (!iconImage.loadFromFile("icon.png")) {}
    iconImage.setSmooth(true);

    //setup for the main icon
    icon.setTexture(iconImage);
    icon.setScale(sf::Vector2f(0.18f, 0.18f));
    icon.setOrigin(icon.getLocalBounds().width / 2,
        icon.getLocalBounds().height / 2);
    icon.setPosition(200, window.getSize().y - 200);

    //setup for the icon's hitbox
    createRect(iconHitbox, icon.getGlobalBounds().width,
        icon.getGlobalBounds().height);
    iconHitbox.setFillColor(sf::Color::Red);
    iconHitbox.setPosition(icon.getPosition().x, icon.getPosition().y);

    //setup for the icon's block hitbox
    createRect(iconBlockHitbox, icon.getGlobalBounds().width / 5,
        icon.getGlobalBounds().height / 5);
    iconBlockHitbox.setFillColor(sf::Color::Blue);
    iconBlockHitbox.setPosition(icon.getPosition().x, icon.getPosition().y);

    //icon defaults needed for position change later
    iconDefaultY = icon.getPosition().y;
    iconGroundDefaultY = iconDefaultY;

    //@ground
    //ground setup
    if (!groundImage.loadFromFile("ground square.png")) {}
    groundImage.setSmooth(true);
    for (int i = 0; i < GROUND_COUNT; i++)
    {
        ground[i].setTexture(groundImage);
        ground[i].setScale(sf::Vector2f(0.8, 0.8));
        //center is not the origin, its the top left corner
        ground[i].setPosition(i * ground[0].getGlobalBounds().width,
            icon.getGlobalBounds().top + icon.getGlobalBounds().height);
        ground[i].setColor(sf::Color(0, 150, 255));
    }
    groundLine.setSize(sf::Vector2f(window.getSize().x, 3));
    groundLine.setPosition(0, icon.getGlobalBounds().top +
        icon.getGlobalBounds().height);

    //spike setup
    if (!spikeImage.loadFromFile("default spike.png")) {}
    spikeImage.setSmooth(true);
    spike.setTexture(spikeImage);
    spike.setScale(0.5, 0.5);
    spike.setOrigin(spike.getLocalBounds().width / 2,
        spike.getLocalBounds().height / 2);

    //spike hitbox setup
    createRect(spikeHitbox, 15, 30);
    spikeHitbox.setFillColor(sf::Color::Red);

    //@block
    //block setup
    if (!blockImage.loadFromFile("default block.png")) {}
    blockImage.setSmooth(true);
    block.setTexture(blockImage);
    block.setOrigin(40, 61);
    block.setScale(0.48, 0.48);

    //block hitbox setup
    createRect(blockHitbox, block.getGlobalBounds().width,
        block.getGlobalBounds().height);
    blockHitbox.setFillColor(sf::Color::Blue);

    //short spike setup
    if (!shortSpikeImage.loadFromFile("short spike.png")) {}
    shortSpikeImage.setSmooth(true);
    shortSpike.setTexture(shortSpikeImage);
    shortSpike.setScale(0.5, 0.5);
    shortSpike.setOrigin(shortSpike.getLocalBounds().width / 2,
        shortSpike.getLocalBounds().height / 2);

    //short spike hitbox setup
    createRect(shortSpikeHitbox, 15, 15);
    shortSpikeHitbox.setFillColor(sf::Color::Red);

    //@sounds
    //sound setup
    if (menuLoopBuffer.loadFromFile("menu loop.ogg")) {}
    menuLoop.setBuffer(menuLoopBuffer);
    menuLoop.setLoop(true);
    menuLoop.play();

    if (selectBuffer.loadFromFile("select.ogg")) {}
    select.setBuffer(selectBuffer);

    if (deathBuffer.loadFromFile("death.ogg")) {}
    death.setBuffer(deathBuffer);

    if (stereoMadness.openFromFile("stereo madness.ogg")) {}
    if (customSong.openFromFile("custom song.ogg")) {}

    //@wall
    //endwall setup
    if (endwallImage.loadFromFile("endwall.png")) {}
    endwallImage.setSmooth(true);
    endwall.setTexture(endwallImage);
    endwall.setOrigin(endwall.getLocalBounds().width,
        endwall.getLocalBounds().height);
    endwall.setPosition(500, 50);

    //level array setup
    for (int i = 0; i < SIZE_X; i++)
        for (int j = 0; j < SIZE_Y; j++)
            level[i][j] = '-';

    //setup done, this is where live processes start
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        //@button
        //click detection for activating buttons
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !mouseHeld)
        {
            switch (menu)
            {
            case 0:
                if (buttonPressed(mainButton, window))
                    menu = 1;
                break;
            case 1:
                if (buttonPressed(mainButton, window))
                    menu = 2;
                if (buttonPressed(fourButton, window))
                    menu = 5;
                break;
            default:
                break;
            }

            mouseHeld = true;
        }
        else
        {
            mouseHeld = false;
        }

        //@bg
        //handling background movement
        if (!paused && iconAlive && !end)
        {
            for (int i = 0; i < 2; i++)
            {
                background[i].setPosition(window.getSize().x / 2 -
                    bgPosition[i], window.getSize().y / 2);
                bgPosition[i] += 0.625;

                if (bgPosition[i] >= 1330)
                    bgPosition[i] = -1330;
            }
        }

        //@menu
        //handling menu changes
        if (menuChange != menu)
        {
            menuChange = menu;

            switch (menu)
            {
            case 0:
                mainTitle.setCharacterSize(65);
                mainTitle.setPosition(window.getSize().x / 2, 100);
                mainTitle.setString("Geometry Dash");
                mainSubTitle.setCharacterSize(35);
                mainSubTitle.setPosition(window.getSize().x / 2, 150);
                mainSubTitle.setString("Education Edition");
                mainButton.setPosition(window.getSize().x / 2,
                    window.getSize().y / 2);
                textStringUpdate(mainButtonText, "Play", 35);
                mainButtonText.setPosition(window.getSize().x / 2,
                    window.getSize().y / 2);
                break;
            case 1:
                mainButton.setPosition(window.getSize().x / 2,
                    window.getSize().y / 2 - 225);
                textStringUpdate(mainButtonText, "Stereo Madness", 35);
                mainButtonText.setPosition(window.getSize().x / 2,
                    window.getSize().y / 2 - 225);
                secButton.setPosition(window.getSize().x / 2,
                    window.getSize().y / 2 - 75);
                textStringUpdate(secButtonText, "Coming soon :)", 35);
                secButtonText.setPosition(window.getSize().x / 2,
                    window.getSize().y / 2 - 75);
                thrButton.setPosition(window.getSize().x / 2,
                    window.getSize().y / 2 + 75);
                textStringUpdate(thrButtonText, "Coming soon :)", 35);
                thrButtonText.setPosition(window.getSize().x / 2,
                    window.getSize().y / 2 + 75);
                fourButton.setPosition(window.getSize().x / 2,
                    window.getSize().y / 2 + 225);
                textStringUpdate(fourButtonText, "Custom level", 35);
                fourButtonText.setPosition(window.getSize().x / 2,
                    window.getSize().y / 2 + 225);
                break;
            case 2:
                menuLoop.stop();
                select.play();
                clock.restart();
                setLevel(level, "stereo madness.txt", endPos);
                while (clock.getElapsedTime().asSeconds() <= 2) {}
                stereoMadness.play();
                break;
            case 3:
            case 4:
                break;
            case 5:
                menuLoop.stop();
                select.play();
                clock.restart();
                setLevel(level, "custom level.txt", endPos);
                while (clock.getElapsedTime().asSeconds() <= 2) {}
                customSong.play();
                break;
            default:
                break;
            }

            if (menu >= 2)
            {
                icon.setPosition(200, window.getSize().y - 200);
                iconHitbox.setPosition(200, window.getSize().y - 200);
                iconBlockHitbox.setPosition(200, window.getSize().y - 200);
                icon.setRotation(0);

                textStringUpdate(mainButtonText, "You Win!", 75);
                mainButtonText.setPosition(window.getSize().x / 2,
                    window.getSize().y / 2 - 200);

                textStringUpdate(secButtonText, "Press escape to exit.", 25);
                secButtonText.setPosition(window.getSize().x / 2,
                    window.getSize().y / 2 - 125);
            }
        }

        //goes back to main menu from level menu
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) && menu == 1)
        {
            menu = 0;
        }

        //@pause
        //leaving a level and pause detection
        if (menu < 2)
        {
            stereoMadness.stop();
            levelPos = 0;
            if (menuLoop.getStatus() == sf::SoundSource::Stopped)
            {
                menuLoop.play();
            }
        }
        else
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) && iconAlive
                && !end)
            {
                paused = true;
                pausedReleased = false;
                switch (menu)
                {
                case 2:
                    stereoMadness.pause();
                    break;
                case 5:
                    customSong.pause();
                }

            }
        }

        //handling the pause menu
        if (paused && sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            if (buttonPressed(pauseButtons[0], window))
            {
                //resume level
                switch (menu)
                {
                case 2:
                    stereoMadness.play();
                    break;
                case 5:
                    customSong.play();
                }
                paused = false;
            }
            if (buttonPressed(pauseButtons[1], window))
            {
                //exit level
                switch (menu)
                {
                case 2:
                    stereoMadness.stop();
                    break;
                case 5:
                    customSong.stop();
                }
                menu = 1;
                paused = false;
            }
            if (buttonPressed(pauseButtons[2], window))
            {
                //restart level
                icon.setRotation(0);
                levelPos = 0;
                iconDefaultY = window.getSize().y - 200;
                jumpCount = jumpHeight = 0;
                jumpCheck = fall = false;
                icon.setPosition(icon.getPosition().x, iconDefaultY);
                switch (menu)
                {
                case 2:
                    stereoMadness.stop();
                    stereoMadness.play();
                    break;
                case 5:
                    customSong.stop();
                    customSong.play();
                }
                paused = false;
            }
        }
        else if (!paused && !sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            //preventing an instant, unwanted jump from simply clicking resume
            pausedReleased = true;
        }

        //@jump
        //click detection for icon jumping
        if (menu >= 2 && (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)
            || sf::Keyboard::isKeyPressed(sf::Keyboard::Space)
            || sf::Mouse::isButtonPressed(sf::Mouse::Left))
            && isGrounded && !paused && (pausedReleased || !isGrounded))
        {
            jumpCheck = true;
        }

        //making the icon jump,
        //jumpHeight utilizes a parabola to simulate the icon's gravity
        if (jumpCheck && !paused && !end)
        {
            if (jumpCount++ == 0)
                isGrounded = false;
            jumpHeight = 0.75 * (0 - jumpCount) * (jumpCount - 26);
            //y = -0.8x(x-25.5), forms a parabola, x coordinate is irrelevant
            //to us since x position is determined by the level speed

            //update icon position with the new jump height
            icon.setPosition(icon.getPosition().x, iconDefaultY - jumpHeight);
            iconHitbox.setPosition(icon.getPosition().x, icon.getPosition().y);
            iconBlockHitbox.setPosition(icon.getPosition().x,
                icon.getPosition().y);
        }

        /* detecting if the player has just fallen off of a block
        if the player is not on a block, not on the ground, and not jumping
        then the jump command will begin */
        if (!onBlock(iconHitbox, blockHitbox)
            && !onGround(iconHitbox, groundLine)
            && !jumpCheck)
            fall = true;
        else
            fall = false;

        //runs the fall command, even if requirements to 
        //begin falling are not met
        if (fall && !paused && !end)
        {
            if (jumpCount == 0)
            {
                isGrounded = false;
            }
            jumpCount++;
            jumpHeight = 0.8 * (0 - jumpCount) * jumpCount;
            //y = -0.8x^2

            //update icon position with the new jump height
            icon.setPosition(icon.getPosition().x, iconDefaultY - jumpHeight);
            iconHitbox.setPosition(icon.getPosition().x, icon.getPosition().y);
            iconBlockHitbox.setPosition(icon.getPosition().x,
                icon.getPosition().y);

        }

        //handling death here so that i can use blockCollisionTest properly.
        //see the if (!iconAlive) function for death details
        if (menu >= 2 && levelCollision(spikeHitbox, blockHitbox,
            shortSpikeHitbox, iconHitbox, iconBlockHitbox, window, level,
            levelPos, iconDefaultY, iconGroundDefaultY, blockCollisionTest))
        {
            iconAlive = false;
        }

        //runs functions to detect if icon is grounded again
        if (!isGrounded && !paused)
        {
            icon.rotate(180.0 / 26);

            if (blockCollisionTest)
            {
                //icon repositioning
                icon.setPosition(icon.getPosition().x, iconDefaultY);
                iconHitbox.setPosition(icon.getPosition().x,
                    icon.getPosition().y);
                iconBlockHitbox.setPosition(icon.getPosition().x,
                    icon.getPosition().y);
                isGrounded = true;
                blockCollisionTest = false;
            }

            if (groundCollision(iconHitbox, groundLine))
            {
                //new default Y value for the icon, 
                //groundline has its origin at its top left corner
                iconDefaultY = groundLine.getPosition().y
                    - iconHitbox.getOrigin().y;
                //icon repositioning
                icon.setPosition(icon.getPosition().x, iconDefaultY);
                iconHitbox.setPosition(icon.getPosition().x,
                    icon.getPosition().y);
                iconBlockHitbox.setPosition(icon.getPosition().x,
                    icon.getPosition().y);
                //grounded
                isGrounded = true;
            }
        }

        if (isGrounded && !end)
        {
            jumpCheck = false;
            jumpCount = 0;
            icon.setRotation(newRotation(icon));
        }

        //@move
        //handling level movement
        if (menu >= 2 && !paused && iconAlive)
        {
            //continuously loops ground segments
            for (int i = 0; i < GROUND_COUNT; i++)
            {
                //move the entire ground to the left by levelSpeed
                ground[i].setPosition(ground[i].getPosition().x - levelSpeed,
                    ground[i].getPosition().y);

                //if ground[i] is off of the screen
                if (ground[i].getPosition().x <= 0 -
                    ground[i].getGlobalBounds().width)
                {
                    if (i == 0)
                    {
                        //accounting for global boundary inaccuracies, this 
                        //offsets ground[0] by a few extra pixels.
                        //see next comment to see what this normally does.
                        ground[i].setPosition(ground[((i - 1) + GROUND_COUNT) %
                            GROUND_COUNT].getPosition().x +
                            ground[1].getGlobalBounds().width - 10,
                            ground[i].getPosition().y);
                    }
                    else
                    {
                        //set ground[i]'s position to the right edge of 
                        //the previous tile in the sequence
                        ground[i].setPosition(ground[((i - 1) + GROUND_COUNT) %
                            GROUND_COUNT].getPosition().x +
                            ground[1].getGlobalBounds().width,
                            ground[i].getPosition().y);
                    }
                }
            }

            //increases level position for spike and block placements
            levelPos += levelSpeed;
        }

        //@death
        //collision, death effect, and level restart
        if (!iconAlive)
        {
            switch (menu)
            {
            case 2:
                stereoMadness.stop();
                break;
            case 5:
                customSong.stop();
            }
            iconDefaultY = window.getSize().y - 200;

            if (clock.getElapsedTime().asMilliseconds() > 1020)
                clock.restart();

            if (clock.getElapsedTime().asMilliseconds() == 0)
                death.play();

            if (clock.getElapsedTime().asSeconds() >= 1)
            {
                icon.setRotation(0);
                levelPos = 0;
                jumpCount = jumpHeight = 0;
                jumpCheck = fall = false;
                icon.setPosition(icon.getPosition().x, iconDefaultY);
                switch (menu)
                {
                case 2:
                    stereoMadness.play();
                    break;
                case 5:
                    customSong.play();
                }
                iconAlive = true;
            }
        }

        //@end
        //handling level completion
        if (menu >= 2 && (endPos + 6) * iconHitbox.getSize().x - levelPos + 10
            <= iconHitbox.getPosition().x)
        {
            end = true;
            levelSpeed = 0;
        }

        if (end)
        {
            if (icon.getPosition().x <= window.getSize().x + 50)
            {
                //all of this is for the end animation
                aniVariable = icon.getPosition().x - 200;
                if (aniVariable == 0)
                {
                    aniStartPos = (icon.getPosition().y - aniEndPos) / aniEndPos;
                    jumpHeight = 0; //reduce, reuse, recycle
                }

                icon.setPosition(icon.getPosition().x, (1 / aniEndPos) *
                    ((aniScale * aniVariable) - (aniEndPos * aniStartPos)) *
                    ((aniScale * aniVariable) - aniEndPos) + aniEndPos);
                //its complicated but trust me its a parabola
                jumpHeight += 0.1;
                icon.setPosition(icon.getPosition().x +
                    (LEVEL_SPEED_DEFAULT * jumpHeight), icon.getPosition().y);

                icon.rotate(180.0 / 26);
            }
            else
            {
                switch (menu)
                {
                case 2:
                    stereoMadness.stop();
                    break;
                case 5:
                    customSong.stop();
                }

                //all of this is for the end text and exiting the level
                if (!winText)
                {
                    jumpHeight = 0;
                    clock.restart();
                }

                winText = true;
                jumpHeight += atan(1) / 9;

                textStringUpdate(mainButtonText, "You Win!", 75 +
                    (10 * sin(jumpHeight)));

                if (clock.getElapsedTime().asSeconds() >= 1)
                {
                    escText = true;
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                {
                    menu = 0;
                    end = winText = escText = false;
                    levelSpeed = LEVEL_SPEED_DEFAULT;
                }
            }
        }

        //@draw
        //drawing objects to window based on the current menu
        window.clear();
        for (int i = 0; i < 2; i++)
            window.draw(background[i]);

        switch (menu)
        {
        case 0:
            window.draw(mainTitle);
            window.draw(mainSubTitle);
            window.draw(mainButton);
            window.draw(mainButtonText);
            break;
        case 1:
            window.draw(mainButton);
            window.draw(mainButtonText);
            window.draw(secButton);
            window.draw(secButtonText);
            window.draw(thrButton);
            window.draw(thrButtonText);
            window.draw(fourButton);
            window.draw(fourButtonText);
            break;
        case 2:
        case 3:
        case 4:
        case 5:
            for (int i = 0; i < GROUND_COUNT; i++)
                window.draw(ground[i]);
            window.draw(groundLine);
            if (iconAlive)
            {
                window.draw(icon);
            }
            levelDraw(spike, block, shortSpike, endwall, iconHitbox, window,
                level, levelPos, iconGroundDefaultY, endPos);
            if (winText)
            {
                window.draw(mainButtonText);
                if (escText)
                {
                    window.draw(secButtonText);
                }
            }

            break;
        default:
            break;
        }
        if (menu >= 2 && paused)
        {
            window.draw(pauseMenu);
            window.draw(pausedText);
            for (int i = 0; i < 3; i++)
            {
                window.draw(pauseButtons[i]);
                window.draw(pauseButtonsText[i]);
            }
        }
        window.display();
    }
    return 0;
}

//basic setup for text
void createText(sf::Text& text, sf::Font& font)
{
    text.setFont(font);
    text.setFillColor(sf::Color::White);
    text.setOutlineColor(sf::Color::Black);
    text.setOutlineThickness(1);
}

//automatic readjustment for string changes in text
void textStringUpdate(sf::Text& text, std::string string, int size)
{
    text.setCharacterSize(size);
    text.setString(string);
    text.setOrigin(text.getLocalBounds().width / 2,
        text.getLocalBounds().height / 2);
}

//basic setup for rectangles (mainly menu buttons)
void createRect(sf::RectangleShape& rect, int sizeX, int sizeY)
{
    rect.setSize(sf::Vector2f(sizeX, sizeY));
    rect.setOrigin(rect.getSize().x / 2,
        rect.getSize().y / 2);
    rect.setFillColor(sf::Color(255, 255, 255, 75));
}

//@level
//loads characters from file into the level array
void setLevel(char level[][SIZE_Y], std::string levelFileName, int& endPos)
{
    std::fstream seed;
    char input;

    seed.open(levelFileName, std::ios::in);
    for (int i = 0; i < SIZE_X; i++)
    {
        for (int j = 0; j < SIZE_Y; j++)
        {
            seed >> input;
            level[i][j] = input;
        }
    }
    seed.close();
    setEnd(level, endPos);
}

//gets the X position of the last object in the level
void setEnd(char level[][SIZE_Y], int& endPos)
{
    for (int i = 0; i < SIZE_X; i++)
    {
        for (int j = 0; j < SIZE_Y; j++)
        {
            switch (level[i][j])
            {
            case 's':
            case 'b':
            case 'h':
                endPos = i;
            }
        }
    }
}

//draws level images into position based off of their position inside of the
//level array/text file
void levelDraw(sf::Sprite spike, sf::Sprite block, sf::Sprite shortSpike,
    sf::Sprite endwall, sf::RectangleShape icon, sf::RenderWindow& window,
    char level[][SIZE_Y], float levelPos, int iconGroundDefaultY, int endPos)
{
    for (int i = 0; i < SIZE_X; i++)
    {
        for (int j = 0; j < SIZE_Y; j++)
        {
            switch (level[i][j])
            {
            case 's':
                spike.setPosition((i + 1) * icon.getSize().x - levelPos +
                    icon.getPosition().x + 10, iconGroundDefaultY - j *
                    icon.getSize().y - 1);
                if (drawCheck(icon, window, iconGroundDefaultY, levelPos, i, j))
                {
                    window.draw(spike);
                }
                break;
            case 'b':
                block.setPosition((i + 1) * icon.getSize().x - levelPos +
                    icon.getPosition().x, iconGroundDefaultY - j *
                    icon.getSize().y);
                if (drawCheck(icon, window, iconGroundDefaultY, levelPos, i, j))
                {
                    window.draw(block);
                }
                break;
            case 'h':
                shortSpike.setPosition((i + 1) * icon.getSize().x - levelPos +
                    icon.getPosition().x + 10, iconGroundDefaultY - j *
                    icon.getSize().y + (shortSpike.getOrigin().y / 2) + 1);
                if (drawCheck(icon, window, iconGroundDefaultY, levelPos, i, j))
                {
                    window.draw(shortSpike);
                }
            }

            if (i == endPos && j == 0)
            {
                endwall.setPosition((i + 1) * icon.getSize().x - levelPos +
                    icon.getPosition().x + icon.getSize().x / 2 + 1145,
                    iconGroundDefaultY + icon.getSize().y / 2);
                if (drawCheck(icon, window, iconGroundDefaultY, levelPos, i, j))
                {
                    window.draw(endwall);
                }
            }
        }
    }
}

//simple function to detect if a button is clicked based on the bounderies
//of the button and the cursors position on the window
bool buttonPressed(sf::RectangleShape button, sf::RenderWindow& window)
{
    if (sf::Mouse::getPosition(window).x >= button.getPosition().x
        - button.getOrigin().x
        && sf::Mouse::getPosition(window).x <= button.getPosition().x
        + button.getOrigin().x
        && sf::Mouse::getPosition(window).y >= button.getPosition().y
        - button.getOrigin().y
        && sf::Mouse::getPosition(window).y <= button.getPosition().y
        + button.getOrigin().y)
    {
        return true;
    }
    return false;
}

//used inside of levelDraw to check if the object is on the screen
bool drawCheck(sf::RectangleShape icon, sf::RenderWindow& window,
    int iconGroundDefaultY, int levelPos, int i, int j)
{
    if ((i + 1) * icon.getSize().x - levelPos + icon.getPosition().x + 10 <=
        window.getSize().x + 50
        && (i + 1) * icon.getSize().x - levelPos +
        icon.getPosition().x + 10 >= 0 - 50
        )
    {
        return true;
    }
    return false;
}

//detects if two objects intersect
bool collision(sf::RectangleShape obj1, sf::RectangleShape obj2)
{
    //bottom right corner
    if (obj1.getPosition().x + obj1.getOrigin().x > obj2.getPosition().x
        - obj2.getOrigin().x //right of left side
        && obj1.getPosition().x + obj1.getOrigin().x < obj2.getPosition().x
        + obj2.getOrigin().x //left of right side
        && obj1.getPosition().y + obj1.getOrigin().y < obj2.getPosition().y
        + obj2.getOrigin().y //above the bottom side
        && obj1.getPosition().y + obj1.getOrigin().y > obj2.getPosition().y
        - obj2.getOrigin().y) //below the top side
        return true;

    //top right corner
    if (obj1.getPosition().x + obj1.getOrigin().x > obj2.getPosition().x
        - obj2.getOrigin().x
        && obj1.getPosition().x + obj1.getOrigin().x < obj2.getPosition().x
        + obj2.getOrigin().x
        && obj1.getPosition().y - obj1.getOrigin().y < obj2.getPosition().y
        + obj2.getOrigin().y
        && obj1.getPosition().y - obj1.getOrigin().y > obj2.getPosition().y
        - obj2.getOrigin().y)
        return true;

    //top left corner
    if (obj1.getPosition().x - obj1.getOrigin().x > obj2.getPosition().x
        - obj2.getOrigin().x
        && obj1.getPosition().x - obj1.getOrigin().x < obj2.getPosition().x
        + obj2.getOrigin().x
        && obj1.getPosition().y - obj1.getOrigin().y < obj2.getPosition().y
        + obj2.getOrigin().y
        && obj1.getPosition().y - obj1.getOrigin().y > obj2.getPosition().y
        - obj2.getOrigin().y)
        return true;

    //bottom left corner
    if (obj1.getPosition().x - obj1.getOrigin().x > obj2.getPosition().x
        - obj2.getOrigin().x
        && obj1.getPosition().x - obj1.getOrigin().x < obj2.getPosition().x
        + obj2.getOrigin().x
        && obj1.getPosition().y + obj1.getOrigin().y < obj2.getPosition().y
        + obj2.getOrigin().y
        && obj1.getPosition().y + obj1.getOrigin().y > obj2.getPosition().y
        - obj2.getOrigin().y)
        return true;

    return false;
}

//used when the player jumps on top of a block, 
//not for detecting general collision.
bool blockCollision(sf::RectangleShape icon, sf::RectangleShape block)
{
    //utilizes basic collision, BUT takes the bottom edge and 
    //replaces it with "35 from the top edge"

    //bottom right corner
    if (icon.getPosition().x + icon.getOrigin().x > block.getPosition().x
        - block.getOrigin().x //right of left side
        && icon.getPosition().x + icon.getOrigin().x < block.getPosition().x
        + block.getOrigin().x //left of right side
        && icon.getPosition().y + icon.getOrigin().y < block.getPosition().y
        - block.getOrigin().y + 35 //above 35 from the top edge
        && icon.getPosition().y + icon.getOrigin().y > block.getPosition().y
        - block.getOrigin().y) //below the top side
        return true;

    //bottom left corner
    if (icon.getPosition().x - icon.getOrigin().x > block.getPosition().x
        - block.getOrigin().x
        && icon.getPosition().x - icon.getOrigin().x < block.getPosition().x
        + block.getOrigin().x
        && icon.getPosition().y + icon.getOrigin().y < block.getPosition().y
        - block.getOrigin().y + 35
        && icon.getPosition().y + icon.getOrigin().y > block.getPosition().y
        - block.getOrigin().y)
        return true;

    return false;
}

//used when the player jumps on top of the ground, 
//not for detecting general collision.
bool groundCollision(sf::RectangleShape icon, sf::RectangleShape ground)
{
    if (icon.getPosition().y + icon.getOrigin().y >= ground.getPosition().y)
        return true;

    return false;
}

//capsule for  collision tests that utilizes the level array
//to position the hitboxes and returns if a collision has occured
//also utilizes a boolean variable to handle block tests as well
bool levelCollision(sf::RectangleShape spike, sf::RectangleShape block,
    sf::RectangleShape shortSpike, sf::RectangleShape icon,
    sf::RectangleShape iconBlock, sf::RenderWindow& window,
    char level[][SIZE_Y], float levelPos, int& iconDefaultY,
    int iconGroundDefaultY, bool& blockTest)
{
    for (int i = 0; i < SIZE_X; i++)
    {
        for (int j = 0; j < SIZE_Y; j++)
        {
            switch (level[i][j])
            {
            case 's':
                spike.setPosition((i + 1) * icon.getSize().x - levelPos +
                    icon.getPosition().x + 10, iconGroundDefaultY - j *
                    icon.getSize().y - 1);
                if (drawCheck(icon, window, iconGroundDefaultY, levelPos, i, j)
                    && collision(spike, icon))
                {
                    return true;
                }
                break;
            case 'b':
                block.setPosition((i + 1) * icon.getSize().x - levelPos +
                    icon.getPosition().x + 10, iconGroundDefaultY - j *
                    icon.getSize().y);
                if (drawCheck(icon, window, iconGroundDefaultY, levelPos, i, j)
                    && blockCollision(icon, block))
                {
                    iconDefaultY = block.getPosition().y
                        - block.getOrigin().y
                        - icon.getOrigin().y;

                    blockTest = true;
                }
                if (drawCheck(icon, window, iconGroundDefaultY, levelPos, i, j)
                    && collision(iconBlock, block))
                {
                    return true;
                }
                break;
            case 'h':
                shortSpike.setPosition((i + 1) * icon.getSize().x - levelPos +
                    icon.getPosition().x + 10, iconGroundDefaultY - j *
                    icon.getSize().y +
                    (shortSpike.getOrigin().y / 2) + 10);
                if (drawCheck(icon, window, iconGroundDefaultY, levelPos, i, j)
                    && collision(shortSpike, icon))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

//used to detect if the player is on top of the ground
//NOT A COLLISION TEST. made for fall condition.
bool onGround(sf::RectangleShape icon, sf::RectangleShape ground)
{
    //if the icon is one above the ground
    if (icon.getPosition().y + icon.getOrigin().y + 1
        >= ground.getPosition().y)
        return true;

    return false;
}

//used to detect of the player is on top of a block
//NOT A COLLISION TEST. made for fall condition.
bool onBlock(sf::RectangleShape icon, sf::RectangleShape block)
{
    //if the left edge or the right edge are within the left and right
    //boundaries of the block and if the icon is one above the block
    if ((icon.getPosition().x - icon.getOrigin().x >= block.getPosition().x
        - block.getOrigin().x
        && icon.getPosition().x - icon.getOrigin().x <= block.getPosition().x
        + block.getOrigin().x)
        || (icon.getPosition().x + icon.getOrigin().x >= block.getPosition().x
            - block.getOrigin().x
            && icon.getPosition().x + icon.getOrigin().x <= block.getPosition().x
            + block.getOrigin().x)
        && (icon.getPosition().y + icon.getOrigin().y + 1
            >= block.getPosition().y - block.getOrigin().y)
        && (icon.getPosition().y + icon.getOrigin().y + 1
            <= block.getPosition().y + block.getOrigin().y))
        return true;

    return false;
}

//snaps the icon's rotation to the nearest multiple of 90
int newRotation(sf::Sprite icon)
{
    int cycle = icon.getRotation();
    int newCycle = cycle / 90;

    if (cycle % 90 > 45)
        newCycle++;
    return newCycle * 90;
}