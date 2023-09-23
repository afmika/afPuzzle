#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>

#include "GameFiles.h"
#include "PicManager.h"
#include "Utils.h"

const int width = 600;
const int height = 600;
const float volume = 6.0;
const int frames = 30;
const int msTime = 1000 / frames;

int division = 3;

int dimX = width / division;
int dimY = height / division;
int levelIndexNumber = 1;
int posCurrentPic = 0;
int performance = 0;
int totalMoves = 0;

bool showNumbers = false;
bool turnOnChrono = true;
bool pickNextRandomly = false;
bool modeRandomHole = false;
bool modeHidden = false;
bool modeFlashlight = false;
bool pixelateShaderLoaded = false;
bool flashlightShaderLoaded = false;
bool hideMenu = false;
bool hideSoundIsPlayed = false;

std::vector<std::vector<int>> *uncoveredPiece = nullptr;
const int hiddenTickTime = 2 * msTime;
int hiddenTickCount = 0;

bool first = true;
bool takeScreenshot = false;

double timeElapsed = .0f;
double imgShowingDurationHidden = 20.f; // pic replaced after 30 frames
double imgShowingCounter = .0f;

float pixelateIntensityInit = .1f;
float pixelateIntensity = pixelateIntensityInit;
const float pixelateVelocity = pixelateIntensityInit / 5.0f; // back to normal after 5 frames

// performance animation frame count
const int perfAnimationFrames = 30;
int perfAnimationCount = 0;
bool countDone = true;

// flashlight animation frame count
const int flashlightAnimationFrames = 30;
int flashlightAnimationCount = 0;
bool flashlightAnimDone = true;

sf::Sound *hideSound = NULL;
sf::Texture mainLevelPic;
sf::Texture flashlightTexture;
sf::Sprite mainPic;
sf::Sprite hiddenBack;
sf::Sprite chronoBack;
sf::Sprite flashlightBack;
sf::Image mainIcon;
sf::Shader pixShader;
sf::Shader flashlightShader;
sf::Font font;

std::vector<std::string> files;
std::vector<std::string> *modesUsed = new std::vector<std::string>();

struct Piece
{
    int x;
    int y;
    int index_number;
    bool active;
};

std::vector<std::vector<Piece>> puzzle;

int printLoadingError(std::string filename)
{
    std::cout << "Error loading " << filename << '\n';
    return EXIT_FAILURE;
}

void initTable()
{
    dimX = width / division;
    dimY = height / division;
    puzzle = std::vector<std::vector<Piece>>(division);
    for (int i = 0; i < division; i++)
    {
        puzzle[i] = std::vector<Piece>(division);
    }
}

double getMultiplicator()
{
    double multPerf = 1;
    multPerf -= showNumbers ? 0.6 : 0;     // penalty
    multPerf += modeHidden ? 0.3 : 0;      // bonus
    multPerf += modeFlashlight ? 0.2 : 0;  // bonus
    multPerf += modeRandomHole ? 0.05 : 0; // bonus
    return multPerf;
}

int refreshPerformance(int moves)
{
    const double cte = 1.6f;
    const double p = cte * ((double)division) - 4;
    const double k = p * ((double)division) / ((double)moves);
    const double multPerf = getMultiplicator();

    double perf = k * 5000;
    perf *= multPerf;
    perf += 1000 * (division / timeElapsed);

    return (int)perf;
}

int randomGeneratorInf(int limit = 1000)
{
    return rand() % limit;
}

std::vector<int> getSingleLuckyPiece()
{
    int i = randomGeneratorInf(division);
    int j = randomGeneratorInf(division);
    if (puzzle[i][j].active)
    {
        return std::vector<int>{i, j};
    }
    return getSingleLuckyPiece();
}

void pickLuckyPiece(int count = ((division - 1) * division))
{
    if (uncoveredPiece != nullptr)
    {
        delete uncoveredPiece;
    }

    uncoveredPiece = new std::vector<std::vector<int>>(count);
    for (int i = 0; i < count; i++)
    {
        (*uncoveredPiece)[i] = getSingleLuckyPiece();
    }
}

bool isLuckyPiece(int x, int y)
{
    if (uncoveredPiece != nullptr)
    {
        for (const auto &piece : *uncoveredPiece)
        {
            if (piece.at(0) == x && piece.at(1) == y)
                return true;
        }
    }
    return false;
}

sf::RectangleShape getBlackSquare(int i, int j, int dim_x, int dimY)
{
    double num = static_cast<double>(hiddenTickCount);
    double den = static_cast<double>(hiddenTickTime);
    const int black_color = 255. * (num / den);
    sf::RectangleShape rect;
    rect.setPosition(j * dim_x, i * dimY);
    rect.setFillColor(sf::Color(0, 0, 20, 255 - black_color));
    rect.setSize(sf::Vector2f(dim_x, dimY));
    return rect;
}

void initPiece()
{
    initTable();
    int xrand = randomGeneratorInf(division);
    int yrand = randomGeneratorInf(division);
    for (int i = 0; i < division; i++)
    {
        for (int j = 0; j < division; j++)
        {
            puzzle[i][j].x = j;
            puzzle[i][j].y = i;
            puzzle[i][j].index_number = division * i + j + 1;
            if (modeRandomHole)
            {
                puzzle[i][j].active = !(j == xrand && i == yrand);
            }
            else
            {
                puzzle[i][j].active = !((i + j) == 2 * (division - 1));
            }
        }
    }
}

void initSprite(bool changePic, bool spaceTabPressed)
{
    const int tempPos = posCurrentPic;
    if (changePic)
    {
        if (pickNextRandomly && spaceTabPressed)
        {
            posCurrentPic = randomGeneratorInf(files.size());
        }
        else if (!pickNextRandomly && spaceTabPressed)
        {
            posCurrentPic++;
        }
        // occurs when the randomized number is the same as the current index
        if (spaceTabPressed && posCurrentPic == tempPos)
        {
            posCurrentPic++;
        }
        // checks
        posCurrentPic = posCurrentPic < 0 ? files.size() - 1 : posCurrentPic;
        posCurrentPic = posCurrentPic >= (int)files.size() ? 0 : posCurrentPic;
    }

    const std::string picFileName = files[posCurrentPic];

    if (!mainLevelPic.loadFromFile(picFileName))
    {
        printLoadingError(picFileName);
    }
    else
    {
        const sf::Vector2f sizeImg = (sf::Vector2f)mainLevelPic.getSize();
        const float scaleFactorX = width / sizeImg.x, scaleFactorY = height / sizeImg.y;
        mainPic.setScale(scaleFactorX, scaleFactorY);
        mainPic.setTexture(mainLevelPic);
    }
}

sf::Sprite ajustSprite(sf::Sprite pic, int i, int j, int x, int y)
{
    pic.setTextureRect(sf::IntRect(i * dimX, j * dimY, dimX, dimY));
    pic.setPosition(x * dimX, y * dimY);
    return pic;
}

void readPuzzleTab(sf::RenderWindow &window)
{
    sf::Text tmp;
    sf::CircleShape circle;
    const bool notHiddenYet = !modeHidden || (imgShowingCounter < imgShowingDurationHidden);
    if (showNumbers && notHiddenYet)
    {
        tmp.setFont(font);
        tmp.setCharacterSize(15);
        tmp.setFillColor(sf::Color::Cyan);

        sf::Color color(0, 0, 18);
        circle.setRadius(15);
        circle.setFillColor(color);
        circle.setOutlineThickness(1);
        circle.setOutlineColor(sf::Color::Black);
    }

    if (modeHidden)
    {
        imgShowingCounter++;
    }

    for (int i = 0; i < division; i++)
    {
        for (int j = 0; j < division; j++)
        {
            if (puzzle[i][j].active)
            {
                const int x = puzzle[i][j].x;
                const int y = puzzle[i][j].y;
                if (modeHidden == false || imgShowingCounter < (division * imgShowingDurationHidden))
                {
                    window.draw(ajustSprite(mainPic, x, y, j, i));
                }
                else
                {
                    if (!hideSoundIsPlayed)
                    {
                        hideSound->play();
                        hideSoundIsPlayed = true;
                    }
                    if (isLuckyPiece(x, y))
                    {
                        window.draw(ajustSprite(mainPic, x, y, j, i));
                        window.draw(getBlackSquare(i, j, dimX, dimY));
                        continue;
                    }
                    window.draw(ajustSprite(hiddenBack, x, y, j, i));
                }

                if (showNumbers)
                {
                    tmp.setString(doubleToStr(puzzle[i][j].index_number));
                    tmp.setPosition(j * dimX + (dimX - 52), i * dimY + 20);
                    circle.setPosition(j * dimX + (dimX - 60), i * dimY + 16);
                    window.draw(circle);
                    window.draw(tmp);
                }
            }
        }
    }
}

void swapPiece(int x, int y, int x_, int y_)
{
    Piece tmp = puzzle[y][x];
    puzzle[y][x] = puzzle[y_][x_];
    puzzle[y_][x_] = tmp;
}

void movePiece(int xx, int yy)
{
    if (puzzle[yy][xx].active)
    {
        // single cell
        for (int i = yy - 1; i <= yy + 1; i++)
        {
            for (int j = xx - 1; j <= xx + 1; j++)
            {
                const bool correctPos = (i >= 0 && j >= 0 && i < division && j < division);
                const bool corner = ((i == yy - 1) && (j == xx - 1)) || ((i == yy + 1) && (j == xx + 1)) || ((i == yy - 1) && (j == xx + 1)) || ((i == yy + 1) && (j == xx - 1));
                if (correctPos && !corner)
                {
                    if (!puzzle[i][j].active)
                    {
                        swapPiece(xx, yy, j, i);
                        totalMoves++;
                        return;
                    }
                }
            }
        }

        // multiple cells
        for (int i = -division; i <= division; i++)
        {
            if (yy + i >= 0 && yy + i < division && !puzzle[yy + i][xx].active)
            {
                // 1 : down , -1 : up
                int tmpY = yy + i, dy = (yy + i < yy) ? 1 : -1;
                while (tmpY != yy)
                {
                    swapPiece(xx, tmpY, xx, tmpY + dy);
                    tmpY = tmpY + dy;
                }
                totalMoves++;
                break;
            }
            if (xx + i >= 0 && xx + i < division && !puzzle[yy][xx + i].active)
            {
                // 1 : right , -1 : left
                int tmpX = xx + i, dx = (xx + i < xx) ? 1 : -1;
                while (tmpX != xx)
                {
                    swapPiece(tmpX, yy, tmpX + dx, yy);
                    tmpX = tmpX + dx;
                }
                totalMoves++;
                break;
            }
        }
    }
}

void drawGrid(sf::RenderWindow &window)
{
    sf::RectangleShape r;
    r.setFillColor(sf::Color::Transparent);
    r.setOutlineThickness(.5);
    r.setOutlineColor(modeHidden ? sf::Color::White : sf::Color::Black);
    r.setSize(sf::Vector2f(dimX, dimY));

    for (int i = 0; i < division; i++)
    {
        for (int j = 0; j < division; j++)
        {
            r.setPosition(j * dimX, i * dimY);
            window.draw(r);
        }
    }
}

bool gameClear()
{
    for (int y = 0; y < division; y++)
    {
        for (int x = 0; x < division; x++)
        {
            Piece p = puzzle[y][x];
            if (!(p.x == x && p.y == y))
                return false;
        }
    }
    return true;
}

void initRandomPos()
{
    initPiece();
    for (int i = 0; i < 5000 * division; i++)
    {
        int xr = randomGeneratorInf(division);
        int yr = randomGeneratorInf(division);
        movePiece(xr, yr);
    }
    while (gameClear())
        initRandomPos();
}

void newGame(bool dontChangePic, bool spaceOrTabPressed)
{
    timeElapsed = .0;
    srand(time(NULL));
    initSprite(dontChangePic, spaceOrTabPressed);
    initRandomPos();
    totalMoves = 0;
    performance = 0;

    modesUsed = new std::vector<std::string>;
    hideSoundIsPlayed = false;
    imgShowingCounter = .0f;
    perfAnimationCount = .0f;
}

void increaseLevel(bool positive)
{
    int inc = positive ? 1 : -1;
    division += inc;
    if (division < 3)
        division = 3;
    if (division > 10)
        division = 10;
    if (600 % division == 1)
        division += 1;
    levelIndexNumber++;
}

std::string formatTime()
{
    int seconds = (int)floor(timeElapsed);
    int hours = (seconds / 3600);
    int minutes = (seconds / 60) % 60;

    int sec_left = seconds - ((60 * minutes) + (3600 * hours));
    std::string val;
    val.append(hours < 10 ? "0" + doubleToStr(hours) : doubleToStr(hours));
    val.append(" : ");
    val.append(minutes < 10 ? "0" + doubleToStr(minutes) : doubleToStr(minutes));
    val.append(" : ");

    val.append(sec_left < 10 ? "0" + doubleToStr(sec_left) : doubleToStr(sec_left));
    return val;
}

void drawChrono(sf::Text &timeText, sf::RenderWindow &window)
{
    sf::Vector2f anchor(0, 0);
    chronoBack.setPosition(anchor);

    timeText.setPosition(anchor + sf::Vector2f(5, 2));
    timeText.setString(formatTime().c_str());

    window.draw(chronoBack);
    window.draw(timeText);
}

std::string getAppropriateText(std::string button, bool switchState, std::string text)
{
    std::string tmp("[ " + button + " ] : ");
    tmp.append(text + " ");
    tmp.append(switchState ? "[ON]\n" : "[OFF]\n");
    return tmp;
}
std::string getAppropriateText(std::string button, std::string text)
{
    std::string tmp("[ " + button + " ] : ");
    tmp.append(text + "\n");
    return tmp;
}

void initAppropriateControlText(sf::Text &controlText)
{
    std::string all;
    all.append(getAppropriateText("UP / DOWN", "Change level"));
    all.append(getAppropriateText("Left / Right", "Navigate"));
    all.append(getAppropriateText("R", pickNextRandomly, "Random picture"));
    all.append("\n");
    all.append(getAppropriateText("N", showNumbers, "Show numbers"));
    all.append(getAppropriateText("T", modeRandomHole, "Mode random hole"));
    all.append(getAppropriateText("H", modeHidden, "Mode Hidden"));
    all.append(getAppropriateText("F", modeFlashlight, "Mode FlashLight"));
    all.append("\n");
    std::string tmp((pickNextRandomly ? " (Random)" : ""));
    all.append(getAppropriateText("Space / Tab", "Next puzzle" + tmp));
    all.append(getAppropriateText("S", "Screenshot"));
    all.append(getAppropriateText("M", "Hide/Show Menu"));

    // first call
    if (modesUsed->size() == 0)
    {
        const double mult = getMultiplicator();
        all.append("\nSCORE x " + doubleToStr(mult));
    }

    controlText.setString(all);
}

std::string getModesUsedText()
{
    std::string text = "";
    for (const auto &used : *modesUsed)
        text.append(used);
    return text;
}

void drawTextMenu(sf::Sound scoreSound, sf::Sprite backSprite, sf::RenderWindow &window)
{
    sf::Vector2f anchor(0, 160);
    sf::Text controlText;
    sf::Text authorText;
    sf::Text filesText;
    sf::Text gameDimText;

    // background
    backSprite.setPosition(anchor);
    window.draw(backSprite);
    sf::Vector2f size = (sf::Vector2f)backSprite.getTexture()->getSize();

    // modes
    initAppropriateControlText(controlText);
    controlText.setCharacterSize(15);
    controlText.setFont(font);
    controlText.setFillColor(sf::Color::Cyan);
    controlText.setPosition(anchor + sf::Vector2f(20, 6));
    window.draw(controlText);

    // dim
    std::string dim = doubleToStr(division) + " x " + doubleToStr(division);
    gameDimText.setString(dim);
    gameDimText.setCharacterSize(40);
    gameDimText.setFont(font);
    gameDimText.setFillColor(sf::Color::Transparent);
    gameDimText.setOutlineColor(sf::Color::Magenta);
    gameDimText.setOutlineThickness(0.5f);
    gameDimText.setPosition(anchor + sf::Vector2f(430, 100));
    window.draw(gameDimText);

    // perfs + moves
    std::string modesStr = getModesUsedText();
    if (modesStr.compare("") != 0)
    {
        std::string moves = doubleToStr((double)totalMoves);
        std::string perf = doubleToStr((double)floor(perfAnimationCount));
        modesStr = modesStr.compare("\n") == 0 ? "" : modesStr;
        sf::Text perfText;
        perfText.setString("> " + moves + " Moves " + modesStr + "\n> " + perf + " Points ");
        perfText.setCharacterSize(28);
        perfText.setFont(font);
        perfText.setFillColor(sf::Color::Transparent);
        perfText.setOutlineColor(sf::Color::White);
        perfText.setOutlineThickness(0.7f);
        perfText.setPosition(anchor + sf::Vector2f(20, 200));

        if (perfAnimationCount <= performance)
            perfAnimationCount += ((double)performance / perfAnimationFrames);
        else
            countDone = true;

        window.draw(perfText);
    }

    // loaded files
    std::string fileStatus(doubleToStr(files.size()));
    fileStatus.append(" pics.");
    filesText.setFont(font);
    filesText.setString(fileStatus);
    filesText.setFillColor(sf::Color::Transparent);
    filesText.setOutlineColor(sf::Color::Green);
    filesText.setOutlineThickness(0.6f);
    filesText.setCharacterSize(60);
    filesText.setPosition(anchor + sf::Vector2f(height - filesText.getCharacterSize() * 4, 20));
    window.draw(filesText);

    authorText.setFont(font);
    authorText.setString("afmichael73@gmail.com\n");
    authorText.setCharacterSize(14);
    authorText.setPosition(anchor + size - sf::Vector2f(authorText.getCharacterSize() * 20, 38));
    window.draw(authorText);
}

void load_flashlight_shader(sf::RenderStates &states)
{
    // note : shaders's order matters
    flashlightShaderLoaded = flashlightShader.loadFromFile(FLASHLIGHT_SHADER, sf::Shader::Fragment);

    if (!flashlightShaderLoaded)
        printLoadingError("FLASHLIGHTS SHADERS");

    states.shader = &flashlightShader;
}

void refreshFlashLightShader(float x, float y)
{
    if (flashlightShaderLoaded)
    {
        y = height - y;
        float radius = flashlightAnimationCount * (width / flashlightAnimationFrames);
        sf::Vector3f vec_color(0, 0, 18);
        float alpha = 1;

        flashlightShader.setUniform("texture", sf::Shader::CurrentTexture);
        flashlightShader.setUniform("pos_mouse", sf::Vector2f(x, y));
        flashlightShader.setUniform("radius", radius);
        flashlightShader.setUniform("back_color", vec_color);
        flashlightShader.setUniform("alpha", alpha);
    }
}

void refreshPixelateShader(float intensity)
{
    if (pixelateShaderLoaded)
    {
        intensity = intensity < 0 ? 0 : intensity;
        pixShader.setUniform("texture", sf::Shader::CurrentTexture);
        pixShader.setUniform("pixel_threshold", intensity);
    }
}

void stopAndInitGame(sf::Sound &nextSound, bool spaceOrTabPressed)
{
    first = true;
    pixelateIntensity = pixelateIntensityInit;

    newGame(true, spaceOrTabPressed);
    nextSound.play();

    totalMoves = 0;
    performance = 0;
}

int main()
{
    files = loadPictures();
    sf::Clock clock; // timer

    sf::RenderWindow window(sf::VideoMode(width, height), "afPuzzle 3.1 :: afmika", sf::Style::Titlebar | sf::Style::Close);
    sf::Texture backTexture, hiddenTexture, chronoTexture;
    sf::Sprite backSprite;

    if (!font.loadFromFile(FONT_FILE))
    {
        printLoadingError(FONT_FILE);
    }

    if (mainIcon.loadFromFile(IMG_ICON))
    {
        const sf::Vector2u iconDim = mainIcon.getSize();
        window.setIcon(iconDim.x, iconDim.y, mainIcon.getPixelsPtr());
    }
    else
    {
        printLoadingError(IMG_ICON);
    }

    if (backTexture.loadFromFile(TEXT_BACK))
    {
        backTexture.setSmooth(true);
        backSprite.setTexture(backTexture);
    }
    else
    {
        printLoadingError(TEXT_BACK);
    }

    if (hiddenTexture.loadFromFile(HIDDEN_BACK))
    {
        hiddenTexture.setSmooth(true);
        hiddenBack.setTexture(hiddenTexture);
    }
    else
    {
        printLoadingError(HIDDEN_BACK);
    }
    if (chronoTexture.loadFromFile(CHRONO_BACK))
    {
        chronoTexture.setSmooth(true);
        chronoBack.setTexture(chronoTexture);
    }
    else
    {
        printLoadingError(CHRONO_BACK);
    }
    if (flashlightTexture.loadFromFile(FLASHLIGHT_BACK))
    {
        flashlightTexture.setSmooth(true);
        flashlightBack.setTexture(flashlightTexture);
    }
    else
    {
        printLoadingError(FLASHLIGHT_BACK);
    }

    newGame(false, false);

    sf::Text timeText;
    timeText.setFillColor(sf::Color::Cyan);
    timeText.setFont(font);
    timeText.setCharacterSize(20);

    // SOUNDS
    sf::SoundBuffer hitBuff, congratsBuff, nextBuff, hideEffectBuff, scoreEffectBuff;
    if (!hitBuff.loadFromFile(HIT_SOUND))
    {
        printLoadingError(HIT_SOUND);
    }
    if (!congratsBuff.loadFromFile(CONGRATS_SOUND))
    {
        printLoadingError(CONGRATS_SOUND);
    }
    if (!nextBuff.loadFromFile(NEXT_SOUND))
    {
        printLoadingError(NEXT_SOUND);
    }
    if (!scoreEffectBuff.loadFromFile(SCORE_EFFECT_SOUND))
    {
        printLoadingError(SCORE_EFFECT_SOUND);
    }
    sf::Sound hitSound(hitBuff),
        congratsSound(congratsBuff),
        nextSound(nextBuff),
        scoreSound(scoreEffectBuff);

    hitSound.setVolume(volume);
    nextSound.setVolume(volume);
    congratsSound.setVolume(volume);
    scoreSound.setVolume(volume);

    if (!hideEffectBuff.loadFromFile(HIDE_EFFECT_SOUND))
    {
        printLoadingError(HIDE_EFFECT_SOUND);
    }
    else
    {
        hideSound = new sf::Sound(hideEffectBuff);
        hideSound->setVolume(volume);
    }

    // SHADER
    sf::RenderStates pixelateState;
    if (!pixShader.loadFromFile(PIXEL_SHADER, sf::Shader::Fragment))
    {
        printLoadingError(PIXEL_SHADER);
    }
    else
    {
        pixelateState.shader = &pixShader;
        pixelateShaderLoaded = true;
    }

    sf::RenderStates flashlightState;
    load_flashlight_shader(flashlightState);

    while (window.isOpen())
    {
        sf::Event e;
        float x = static_cast<float>(sf::Mouse::getPosition(window).x);
        float y = static_cast<float>(sf::Mouse::getPosition(window).y);
        while (window.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
            {
                window.close();
                return 0;
            }
            // mouse events
            if (e.type == sf::Event::MouseButtonPressed && countDone)
            {
                if (first)
                {
                    first = false;
                    newGame(false, true);
                }
                else
                {
                    int x = e.mouseButton.x / dimX;
                    int y = e.mouseButton.y / dimY;
                    movePiece(x, y);
                    std::cout << totalMoves << '\n';
                    performance = refreshPerformance(totalMoves);
                    hitSound.play();
                }
            }
            // key events
            if (e.type != sf::Event::KeyReleased)
                continue;
            if (first)
            {
                switch (e.key.code)
                {
                    // modes
                case sf::Keyboard::T:
                    modeRandomHole = !modeRandomHole;
                    break;
                case sf::Keyboard::H:
                    modeHidden = !modeHidden;
                    break;
                case sf::Keyboard::F:
                    modeFlashlight = !modeFlashlight;
                    break;
                case sf::Keyboard::N:
                    showNumbers = !showNumbers;
                    break;
                    // level
                case sf::Keyboard::Up:
                    increaseLevel(true);
                    newGame(false, true);
                    break;
                case sf::Keyboard::Down:
                    increaseLevel(false);
                    newGame(false, true);
                    break;
                default:
                    break;
                }
            }

            // navigation
            if (countDone)
            {
                if (e.key.code == sf::Keyboard::Tab || e.key.code == sf::Keyboard::Space)
                {
                    stopAndInitGame(nextSound, true);
                }
                else if (e.key.code == sf::Keyboard::Left)
                {
                    posCurrentPic--;
                    stopAndInitGame(nextSound, false);
                }
                else if (e.key.code == sf::Keyboard::Right)
                {
                    posCurrentPic++;
                    stopAndInitGame(nextSound, false);
                }
            }

            // UI elements
            switch (e.key.code)
            {
            case sf::Keyboard::C:
                turnOnChrono = !turnOnChrono;
                break;
            case sf::Keyboard::M:
                hideMenu = !hideMenu;
                break;
            case sf::Keyboard::R:
                if (first && countDone)
                    pickNextRandomly = !pickNextRandomly;
                break;
            case sf::Keyboard::S:
                takeScreenshot = true;
                break;
            default:
                break;
            }
        }

        if (first)
        {
            // first load
            clock.restart();
            if (pixelateIntensity > 0)
            {
                pixelateIntensity -= pixelateVelocity;
            }
            else
            {
                pixelateIntensity = 0;
            }

            if (modeFlashlight && !flashlightAnimDone)
            {
                refreshFlashLightShader(x, y);
                window.draw(mainPic, flashlightState);
                flashlightAnimationCount++;
                if (flashlightAnimationCount > flashlightAnimationFrames)
                {
                    flashlightAnimationCount = 0;
                    flashlightAnimDone = true;
                }
            }
            else
            {
                refreshPixelateShader(pixelateIntensity);
                window.draw(mainPic, pixelateState);

                drawGrid(window);
                if (!hideMenu)
                    drawTextMenu(scoreSound, backSprite, window);
            }
        }
        else
        {
            readPuzzleTab(window);
            drawGrid(window);

            timeElapsed = clock.getElapsedTime().asSeconds();
            // for HD mode
            if ((++hiddenTickCount) % hiddenTickTime == 0)
            {
                pickLuckyPiece();
                hiddenTickCount = 0;
            }

            if (modeFlashlight)
            {
                x = x < 0 ? 0 : x;
                x = x > width ? height : x;
                y = y < 0 ? 0 : y;
                y = y > height ? height : y;
                sf::Vector2u size = flashlightTexture.getSize();
                flashlightBack.setPosition(x - size.x / 2, y - size.y / 2);
                window.draw(flashlightBack);
                // window.draw(points, flashlightState);
            }

            if (gameClear())
            {
                if (modeFlashlight)
                    flashlightAnimDone = false;
                countDone = false;
                first = true;
                // pixelateIntensity = pixelateIntensityInit;
                congratsSound.play();
                // newGame();
                // increaseLevel(true);
                nextSound.play();
                // dealing with modes result
                if (modeHidden)
                    modesUsed->push_back("[HD]");
                if (modeFlashlight)
                    modesUsed->push_back("[FL]");
                if (modeRandomHole)
                    modesUsed->push_back("[RL]");
                if (showNumbers)
                    modesUsed->push_back("[NB]");
                if (modesUsed->size() == 0)
                    modesUsed->push_back("\n"); // a little trick
            }
        }

        if (turnOnChrono)
            drawChrono(timeText, window);

        if (takeScreenshot)
        {
            takeScreenshot = false;
            sf::Vector2u windowSize = window.getSize();
            sf::Texture texture;
            texture.create(windowSize.x, windowSize.y);
            texture.update(window);
            sf::Image screenshot = texture.copyToImage();

            screenshot.saveToFile("screenshots/screenshot." + doubleToStr(randomGeneratorInf(10000)) + ".png");
        }

        window.display();
        window.clear();
        sf::sleep(sf::milliseconds(msTime));
    }
    return 0;
}
