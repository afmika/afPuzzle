/*
 * @author afmika
 * @mail afmichael73@gmail.com
 * github : https://github.com/afmika
 * */

#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>
#include <vector>


#include "classes/Game_Files.h"
#include "classes/PicManager.h"
#include "classes/afTools.h"

using namespace std;

const int width = 600;
const int height = 600;
const float volume = 6.0;
const int frames = 30;

int division = 3;

int dim_x = width / division;
int dim_y = height / division;
int level_index_number = 1;
int pos_current_pic = 0;
int ms_time = 1000 / frames;
int performance = 0;
int total_moves = 0;

bool show_numbers = false;
bool turn_on_chrono = true;
bool pick_next_randomly = false;
bool mode_random_hole = false;
bool mode_hidden = false;
bool mode_flashlight = false;
bool pixelate_shader_loaded = false;
bool flashlight_shader_loaded = false;
bool hide_menu = false;
bool hide_sound_is_played = false;

vector<vector<int>> *uncovered_piece = nullptr;
int hidden_tick_time = 2 * ms_time;
int hidden_tick_count = 0;


bool first = true;
bool take_screenshot = false;

double time_elapsed = .0f;
double img_showing_duration_hidden = 20.f; // pic replaced after 30 frames 
double img_showing_counter = .0f;

float pixelate_intensity_init = .1f;
float pixelate_intensity = pixelate_intensity_init;
const float pixelate_velocity = pixelate_intensity_init / 5.0f; // back to normal after 5 frames

// performance anim
const int perf_animation_frames = 30; // 30 frames
int perf_animation_count = 0;
bool count_done = true;

// flashlight anim
const int flashlight_animation_frames = 30; // 30 frames
int flashlight_animation_count = 0;
bool flashlight_anim_done = true;

sf::Sound *hide_sound = NULL;

sf::Texture main_level_pic;
sf::Texture flashlight_texture;
sf::Sprite main_pic;
sf::Sprite hidden_back;
sf::Sprite chrono_back;
sf::Sprite flashlight_back;

sf::Image main_icon;
sf::Shader pix_shader;
sf::Shader flashlight_shader;

sf::Font font;


vector<string> files;
vector<string> *modes_used = new vector<string>();

sf::VertexArray points;

typedef struct piece {
    int x;
    int y;
    int index_number;
    bool active;
} Piece;

vector<vector<Piece>> puzzle;

int printLoadingError(string filename) {
    cout << "Error loading " << filename << endl;
    return EXIT_FAILURE;
}

void initTable() {
    dim_x = width / division;
    dim_y = height / division;
    puzzle = vector<vector<Piece>>(division);
    for (int i = 0; i < division; i++) {
        puzzle[i] = vector<Piece>(division);
    }
}

double getMultiplicator() {
    double mult_perf = 1;
    mult_perf -= show_numbers ? 0.6 : 0; // minus
    mult_perf += mode_hidden ? 0.3 : 0; // bonus
    mult_perf += mode_flashlight ? 0.2 : 0; // bonus
    mult_perf += mode_random_hole ? 0.05 : 0; // bonus
    return mult_perf;
}

int refreshPerformance(int moves) {
    const double cte = 1.6f;
    const double p = cte * ((double) division) - 4;
    const double k = p * ((double) division) / ((double) moves);
    double perf = k * 5000;

    double mult_perf = getMultiplicator();

    perf *= mult_perf;
    
    perf += 1000 * ( division / time_elapsed);

    return (int) perf;
}

int randomGeneratorInf(int limit = 1000) {
    return rand() % limit;
}

vector<int> getSingleLuckyPiece() {
    int i = randomGeneratorInf(division);
    int j = randomGeneratorInf(division);

    if ( !puzzle[i][j].active )
        return getSingleLuckyPiece();;
    
    vector<int> result(2);
    result[0] = i;
    result[1] = j;
    return result;
}

void pickLuckyPiece(int count = ((division - 1) * division)) {
    if ( uncovered_piece != nullptr )
        delete uncovered_piece;
    uncovered_piece = new vector<vector<int>>(count);
    for (int i = 0; i < count; i++)
        (*uncovered_piece) [i] = getSingleLuckyPiece();
}

bool isLuckyPiece(int x, int y) {
    if ( uncovered_piece != nullptr ) {
        for (auto piece : *uncovered_piece)
            if (piece.at(0) == x && piece.at(1) == y)
                return true;
    }
    return false;
}

sf::RectangleShape getBlackSquare(int i, int j, int dim_x, int dim_y) {
    double num = static_cast<double>(hidden_tick_count);
    double den = static_cast<double>(hidden_tick_time);
    const int black_color = 255. * ( num / den );
    sf::RectangleShape rect;
    rect.setPosition(j * dim_x, i * dim_y);
    rect.setFillColor(sf::Color(0, 0, 20, 255 - black_color));
    rect.setSize(sf::Vector2f(dim_x, dim_y));
    return rect;
}

void initPiece() {
    initTable();
    int xrand = randomGeneratorInf(division);
    int yrand = randomGeneratorInf(division);
    for (int i = 0; i < division; i++) {
        for (int j = 0; j < division; j++) {
            puzzle[i][j].x = j;
            puzzle[i][j].y = i;
            puzzle[i][j].index_number = division * i + j + 1;
            if (mode_random_hole) {
                puzzle[i][j].active = ! (j == xrand && i == yrand);
            } else {
                puzzle[i][j].active = ! (( i + j ) == 2 * (division - 1));
            }
        }
    }
}

void initSprite(bool change_pic, bool space_tab_pressed) {
    int temp_pos = pos_current_pic;

    if (change_pic) {
        if (pick_next_randomly && space_tab_pressed) {
            pos_current_pic = randomGeneratorInf(files.size());
        } else if (!pick_next_randomly && space_tab_pressed) {
            pos_current_pic++;
        }
        // occurs when the randomized number is the same as the current index
        if (space_tab_pressed && pos_current_pic == temp_pos) {
            pos_current_pic++;
        } 
        // checks
        pos_current_pic = pos_current_pic < 0 ? files.size()-1 : pos_current_pic;
        pos_current_pic = pos_current_pic >= (int) files.size() ? 0 : pos_current_pic;
    }

    string pic_file_name = files[pos_current_pic];
    // cout << " Index " << pos_current_pic << endl;
    // cout << pic_file_name << endl;

    if (!main_level_pic.loadFromFile(pic_file_name)) {
        printLoadingError(pic_file_name);
    } else {
        const sf::Vector2f sizeImg = (sf::Vector2f) main_level_pic.getSize();
        float scaleFactorX = width / sizeImg.x , scaleFactorY = height / sizeImg.y;
        main_pic.setScale( scaleFactorX, scaleFactorY );
        main_pic.setTexture(main_level_pic);
    }
}

sf::Sprite ajustSprite(sf::Sprite pic, int i, int j, int x, int y) {
    pic.setTextureRect(sf::IntRect(i * dim_x, j * dim_y, dim_x, dim_y));
    pic.setPosition(x * dim_x, y * dim_y);
    return pic;
}

void readPuzzleTab(sf::RenderWindow &window) {
    sf::Text tmp;
    sf::CircleShape circle;
    bool not_hidden_yet = ! mode_hidden || (img_showing_counter < img_showing_duration_hidden);
    if (show_numbers && not_hidden_yet ) {
        tmp.setFont(font);
        tmp.setCharacterSize(15);
        tmp.setFillColor(sf::Color::Cyan);

        sf::Color color(0, 0, 18);
        circle.setRadius(15);
        circle.setFillColor(color);
        circle.setOutlineThickness(1);
        circle.setOutlineColor(sf::Color::Black);
    }

	if (mode_hidden) {
		img_showing_counter++;
	}

    for (int i = 0; i < division; i++) {
        for (int j = 0; j < division; j++) {
            if ( puzzle[i][j].active ) {
                int x = puzzle[i][j].x;
                int y = puzzle[i][j].y;
                if (mode_hidden == false || img_showing_counter < (division * img_showing_duration_hidden) ) {
                    window.draw(ajustSprite( main_pic, x, y, j, i ));
                } else {
					if (! hide_sound_is_played ) {
						hide_sound->play();	
						hide_sound_is_played = true;
					}
                    if ( isLuckyPiece(x, y) ) {
                        window.draw(ajustSprite( main_pic, x, y, j, i ));
                        window.draw(getBlackSquare(i, j, dim_x, dim_y));
						continue;
                    }
					window.draw(ajustSprite( hidden_back, x, y, j, i ));
                }
                if ( show_numbers ) {
                    tmp.setString(doubleToStr( puzzle[i][j].index_number ));
                    tmp.setPosition(j * dim_x + (dim_x - 52), i * dim_y + 20);
                    circle.setPosition(j * dim_x + (dim_x - 60), i * dim_y + 16);

                    window.draw(circle);
                    window.draw(tmp);
                }
            }
        }
    }
}

void swapPiece(int x, int y, int x_, int y_) {
    Piece tmp = puzzle[y][x];
    puzzle[y][x] = puzzle[y_][x_];
    puzzle[y_][x_] = tmp;
}

void movePiece(int xx, int yy) {
    if (puzzle[yy][xx].active) {
        // single cell
        for (int i = yy - 1; i <= yy + 1; i++) {
            for (int j = xx - 1; j <= xx + 1; j++) {
                bool correct_pos = (i >=0 && j >= 0 && i < division && j < division);
                bool corner =  ((i == yy - 1) && (j == xx - 1))
                            || ((i == yy + 1) && (j == xx + 1))
                            || ((i == yy - 1) && (j == xx + 1))
                            || ((i == yy + 1) && (j == xx - 1));
                if (correct_pos && ! corner ) {
                    if (!puzzle[i][j].active) {
                        swapPiece(xx, yy, j, i);
                        total_moves++;
                        return;
                    }
                }
            }
        }
        // multiple cells
        for (int i = -division; i <= division; i++) {
            if (yy+i >= 0 && yy+i < division && ! puzzle[yy+i][xx].active) {
                //1 : down , -1 : up
                int tmp_y = yy + i, dy = (yy + i < yy) ? 1 : -1;
                while (tmp_y != yy) {
                    swapPiece(xx, tmp_y, xx, tmp_y + dy);
                    tmp_y = tmp_y + dy;
                }
                total_moves++;
                break;
            }
            if (xx+i >= 0 && xx+i < division && ! puzzle[yy][xx+i].active) {
                //1 : right , -1 : left
                int tmp_x = xx + i, dx = (xx + i < xx) ? 1 : -1;
                while (tmp_x != xx) {
                    swapPiece(tmp_x, yy, tmp_x + dx, yy);
                    tmp_x = tmp_x + dx;
                }
                total_moves++;
                break;
            }
        }
    }
}

void drawGrid(sf::RenderWindow &window) {
    sf::RectangleShape r;
    r.setFillColor(sf::Color::Transparent);
    r.setOutlineThickness(.5);
    r.setOutlineColor(mode_hidden ? sf::Color::White : sf::Color::Black);
    r.setSize(sf::Vector2f(dim_x, dim_y));

    for (int i = 0; i < division; i++) {
        for (int j = 0; j < division; j++) {
            r.setPosition(j * dim_x, i * dim_y);
            window.draw(r);
        }
    }
}

bool gameClear() {
    for (int y=0; y < division; y++) {
        for (int x=0; x < division; x++) {
            Piece p = puzzle[y][x];
            if (!(p.x == x && p.y == y)) {
                return false;
            }
        }
    }
    return true;
}

void initRandomPos() {
    initPiece();
    for (int i = 0; i < 5000 * division; i++) {
        int xr = randomGeneratorInf(division);
        int yr = randomGeneratorInf(division);
        movePiece(xr, yr);
    }
    while (gameClear()) initRandomPos();
}

void newGame(bool dont_change_pic, bool space_or_tab_pressed) {
    time_elapsed = .0;
    srand(time(NULL));
    initSprite(dont_change_pic, space_or_tab_pressed);
    initRandomPos();
    total_moves = 0;
    performance = 0;

    modes_used = new vector<string>;
	hide_sound_is_played = false;
	img_showing_counter = .0f;
    perf_animation_count = .0f;
}

void increaseLevel(bool positive) {
    int inc = positive ? 1 : -1;
    division += inc;
    if (division < 3) division = 3;
    if (division > 10) division = 10;
    if (600 % division == 1) division += 1;
    level_index_number++;
}

string formatTime() {
    int seconds = (int) floor(time_elapsed);
    int hours = (seconds / 3600);
    int minutes = (seconds / 60) % 60;

    int sec_left = seconds - ((60 * minutes)+(3600 * hours));
    string val;
    val.append(hours < 10 ? "0"+doubleToStr(hours) : doubleToStr(hours));
    val.append(" : ");
    val.append(minutes < 10 ? "0"+doubleToStr(minutes) : doubleToStr(minutes));
    val.append(" : ");

    val.append(sec_left < 10 ? "0"+doubleToStr(sec_left): doubleToStr(sec_left));
    return val;
}

void drawChrono(sf::Text &time_text, sf::RenderWindow &window) {
    sf::Vector2f anchor(0, 0);
    
    chrono_back.setPosition(anchor);

    time_text.setPosition(anchor + sf::Vector2f(5, 2));
    time_text.setString(formatTime().c_str());
    
    window.draw(chrono_back);
    window.draw(time_text);
}

string getAppropriedText(string button, bool switch_state, string text) {
    string tmp("[ "+button+" ] : ");
    tmp.append(text + " ");
    tmp.append(switch_state ? "[ON]\n" : "[OFF]\n");
    return tmp;
}
string getAppropriedText(string button, string text) {
    string tmp("[ "+button+" ] : ");
    tmp.append(text + "\n");
    return tmp;
}

void initAppropriateControlText(sf::Text &control_text) {
    string all;
    all.append(getAppropriedText("UP / DOWN", "Change level"));
    all.append(getAppropriedText("Left / Right", "Navigate"));
    all.append(getAppropriedText("R", pick_next_randomly, "Random picture"));
    all.append("\n");
    all.append(getAppropriedText("N", show_numbers, "Show numbers"));
    all.append(getAppropriedText("T", mode_random_hole, "Mode random hole"));
    all.append(getAppropriedText("H", mode_hidden, "Mode Hidden"));
    all.append(getAppropriedText("F", mode_flashlight, "Mode FlashLight"));
    all.append("\n");
    string tmp((pick_next_randomly ? " (Random)" : ""));
    all.append(getAppropriedText("Space / Tab", "Next puzzle" + tmp ));
    all.append(getAppropriedText("S", "Screenshot"));
    all.append(getAppropriedText("M", "Hide/Show Menu"));
    
    // first call
    if (modes_used->size() == 0) {
        const double mult = getMultiplicator();
		all.append("\nSCORE x "+doubleToStr( mult ));    
    }

    control_text.setString(all);
}

string getModesUsedText() {
    string text = "";
    for (size_t i = 0; i < modes_used->size(); i++) {
        text.append( modes_used->at(i) );
    }
    return text;
}

void drawTextMenu(sf::Sound score_sound, sf::Sprite back_sprite, sf::RenderWindow &window) {
    sf::Vector2f anchor(0, 160);
    sf::Text control_text;
    sf::Text author_text;
    sf::Text files_text;
    sf::Text game_dim_text;

    // background
    back_sprite.setPosition(anchor);
    window.draw(back_sprite);
    sf::Vector2f size = (sf::Vector2f) back_sprite.getTexture()->getSize();

    // modes
    initAppropriateControlText(control_text);
    control_text.setCharacterSize(15);
    control_text.setFont(font);
    control_text.setFillColor(sf::Color::Cyan);
    control_text.setPosition(anchor + sf::Vector2f(20, 6));
    window.draw(control_text);

    // dim
    string dim = doubleToStr(division) + " x " + doubleToStr(division);
    game_dim_text.setString(dim);
    game_dim_text.setCharacterSize(40);
    game_dim_text.setFont(font);
    game_dim_text.setFillColor(sf::Color::Transparent);
    game_dim_text.setOutlineColor(sf::Color::Magenta);
    game_dim_text.setOutlineThickness(0.5f);
    game_dim_text.setPosition(anchor + sf::Vector2f(430, 100));
    window.draw(game_dim_text);
    
    // perfs + moves
    string modes_str = getModesUsedText();
    if (modes_str.compare("") != 0) {
        string moves = doubleToStr((double) total_moves);
        string perf = doubleToStr((double) floor(perf_animation_count) );
        modes_str = modes_str.compare("\n") == 0 ? "" : modes_str;
        sf::Text perf_text;
        perf_text.setString("> "+moves + " Moves "+modes_str +"\n> "+ perf + " Points ");
        perf_text.setCharacterSize(28);
        perf_text.setFont(font);
        perf_text.setFillColor(sf::Color::Transparent);
        perf_text.setOutlineColor(sf::Color::White);
        perf_text.setOutlineThickness(0.7f);
        perf_text.setPosition(anchor + sf::Vector2f(20, 200));

        if (perf_animation_count <= performance) {
            perf_animation_count += ((double) performance / perf_animation_frames);
            // score_sound.play();
        } else {
            count_done = true;
        }

        window.draw(perf_text);
    }

    // loaded files
    string file_status(doubleToStr( files.size() ));
    file_status.append(" pics.");
    files_text.setFont(font);
    files_text.setString(file_status);
    files_text.setFillColor(sf::Color::Transparent);
    files_text.setOutlineColor(sf::Color::Green);
    files_text.setOutlineThickness(0.6f);
    files_text.setCharacterSize(60);
    files_text.setPosition(anchor + sf::Vector2f(height - files_text.getCharacterSize() * 4, 20));
    window.draw(files_text);

    // author text
    author_text.setFont(font);
    author_text.setString("afmichael73@gmail.com\nhttps://github.com/afmika");
    author_text.setCharacterSize(14);
    author_text.setPosition(anchor + size - sf::Vector2f(author_text.getCharacterSize() * 20, 38) );
    window.draw(author_text);
}


void load_flashlight_shader(sf::RenderStates &states) {
    // note : shaders's order matters
    flashlight_shader_loaded = flashlight_shader.loadFromFile("shaders/flashlight.frag.af", sf::Shader::Fragment);

    if (!flashlight_shader_loaded)
        printLoadingError("FLASHLIGHTS SHADERS");

    states.shader = &flashlight_shader;
}

void refreshFlashLightShader(float x, float y) {
    if (flashlight_shader_loaded) {
        y = height - y;
        float radius = flashlight_animation_count * (width / flashlight_animation_frames);
        sf::Vector3f vec_color(0, 0, 18);
        float alpha = 1;

        flashlight_shader.setUniform("texture", sf::Shader::CurrentTexture);
        flashlight_shader.setUniform("pos_mouse", sf::Vector2f(x, y));
        flashlight_shader.setUniform("radius", radius);
        flashlight_shader.setUniform("back_color",  vec_color);
        flashlight_shader.setUniform("alpha",  alpha);        
    }
}

void refreshPixelateShader(float intensity) {
    if (pixelate_shader_loaded) {
        intensity = intensity < 0 ? 0 : intensity; 
        pix_shader.setUniform("texture", sf::Shader::CurrentTexture);
        pix_shader.setUniform("pixel_threshold", intensity );
    }
}

void stopAndInitGame(sf::Sound &next_sound, bool space_or_tab_pressed) {
    first = true;
    pixelate_intensity = pixelate_intensity_init;

    newGame(true, space_or_tab_pressed);
    next_sound.play();

    total_moves = 0;
    performance = 0;
}

int main() {
    files = loadPictures();
    sf::Clock clock; // timer

    sf::RenderWindow window(sf::VideoMode(width, height), "afPuzzle 3 :: afmika", sf::Style::Titlebar | sf::Style::Close);
    sf::Texture back_texture, hidden_texture, chrono_texture;
    sf::Sprite back_sprite;
    
    if (!font.loadFromFile(FONT_FILE)) {
        printLoadingError(FONT_FILE);
    }

    if (main_icon.loadFromFile(IMG_ICON)) {
        sf::Vector2u icon_dim = main_icon.getSize();
        window.setIcon(icon_dim.x, icon_dim.y, main_icon.getPixelsPtr());
    } else {
        printLoadingError(IMG_ICON);
    }

    if (back_texture.loadFromFile(TEXT_BACK)) {
        back_texture.setSmooth( true ); 
        back_sprite.setTexture(back_texture);  
    } else {
        printLoadingError(TEXT_BACK);
    }
    
    if (hidden_texture.loadFromFile(HIDDEN_BACK)) {
        hidden_texture.setSmooth( true ); 
        hidden_back.setTexture(hidden_texture);  
    } else {
        printLoadingError(HIDDEN_BACK);
    }
    if (chrono_texture.loadFromFile(CHRONO_BACK)) {
        chrono_texture.setSmooth( true ); 
        chrono_back.setTexture(chrono_texture);  
    } else {
        printLoadingError(CHRONO_BACK);
    }
    if (flashlight_texture.loadFromFile(FLASHLIGHT_BACK)) {
        flashlight_texture.setSmooth( true ); 
        flashlight_back.setTexture(flashlight_texture);  
    } else {
        printLoadingError(FLASHLIGHT_BACK);
    }

    newGame(false, false);

    sf::Text time_text;
    time_text.setFillColor(sf::Color::Cyan);
    time_text.setFont(font);
    time_text.setCharacterSize(20);

    // SOUNDS
    sf::SoundBuffer hit_buff, congrats_buff, next_buff, hide_effect_buff, score_effect_buff;
    if (!hit_buff.loadFromFile(HIT_SONG)) {
       printLoadingError(HIT_SONG);
    }
    if (!congrats_buff.loadFromFile(CONGRATS_SONG)) {
       printLoadingError(CONGRATS_SONG);
    }
    if (!next_buff.loadFromFile(NEXT_SONG)) {
       printLoadingError(NEXT_SONG);
    }
    if (!score_effect_buff.loadFromFile(SCORE_EFFECT_SONG)) {
       printLoadingError(SCORE_EFFECT_SONG);
    }
    sf::Sound   hit_sound(hit_buff), 
                congrats_sound(congrats_buff), 
                next_sound(next_buff),
                score_sound(score_effect_buff);

    hit_sound.setVolume(volume);
    next_sound.setVolume(volume);
    congrats_sound.setVolume(volume);
    score_sound.setVolume(volume);

	if (!hide_effect_buff.loadFromFile(HIDE_EFFECT_SONG)) {
       printLoadingError(HIDE_EFFECT_SONG);
    } else {
		hide_sound = new sf::Sound(hide_effect_buff);
		hide_sound->setVolume(volume);
	}
	

    // SHADER
    sf::RenderStates pixelate_state;
    if (! pix_shader.loadFromFile(PIXEL_SHADER, sf::Shader::Fragment) ) {
        printLoadingError(PIXEL_SHADER);
    } else {
        pixelate_state.shader = &pix_shader;
        pixelate_shader_loaded = true;
    }

    sf::RenderStates flashlight_state;
    load_flashlight_shader(flashlight_state);
    
    while (window.isOpen()) {
        sf::Event e;
        float x = static_cast<float>(sf::Mouse::getPosition(window).x);
        float y = static_cast<float>(sf::Mouse::getPosition(window).y);
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                window.close();
                return 0;
            }
            if (e.type == sf::Event::MouseButtonPressed && count_done) {
                if (first) {
                    first = false;
                    newGame(false, true);
                } else {
                    int x = e.mouseButton.x / dim_x;
                    int y = e.mouseButton.y / dim_y;
                    movePiece(x, y);

                    cout << total_moves << endl;
                    performance = refreshPerformance(total_moves);

                    hit_sound.play();
                }
            }
            if (e.type == sf::Event::KeyReleased) {               
                // modes
                if (e.key.code == sf::Keyboard::T && first  && count_done) {
                    mode_random_hole = ! mode_random_hole;
                }
                if (e.key.code == sf::Keyboard::H && first  && count_done) {
                    mode_hidden = ! mode_hidden;
                }
                if (e.key.code == sf::Keyboard::F && first && count_done) {
                    mode_flashlight = ! mode_flashlight;
                }
                if ( e.key.code == sf::Keyboard::N && first && count_done) {
                    show_numbers = ! show_numbers;
                }

                // navigation
                if ((e.key.code == sf::Keyboard::Tab || e.key.code == sf::Keyboard::Space)  && count_done ) {
                    stopAndInitGame(next_sound, true);
                }
                if (e.key.code == sf::Keyboard::Left  && count_done) {
                    pos_current_pic--;
                    stopAndInitGame(next_sound, false);
                }
                if (e.key.code == sf::Keyboard::Right && count_done) {
                    pos_current_pic++;
                    stopAndInitGame(next_sound, false);
                }

                // Level
                if (e.key.code == sf::Keyboard::Up && first  && count_done) {
                    increaseLevel(true);
                    newGame(false, true); //false:: on ne change pas l img
                }

                if (e.key.code == sf::Keyboard::Down && first  && count_done) {
                    increaseLevel(false);
                    newGame(false, true);//false:: on ne change pas l img
                }

                if (e.key.code == sf::Keyboard::C) {
                    turn_on_chrono = ! turn_on_chrono;
                }

                if (e.key.code == sf::Keyboard::M) {
                    hide_menu = ! hide_menu;
                }             
                if (e.key.code == sf::Keyboard::R && first && count_done) {
                    pick_next_randomly = ! pick_next_randomly;
                }

                if (e.key.code == sf::Keyboard::S) {
                    take_screenshot = true;
                }

            }
        }

        if (first) {
            // first load
            clock.restart();
            if (pixelate_intensity > 0) {
                pixelate_intensity -= pixelate_velocity;
            } else {
                pixelate_intensity = 0;
            }

            if (mode_flashlight && !flashlight_anim_done) {
                refreshFlashLightShader(x , y );
                window.draw(main_pic, flashlight_state);
                flashlight_animation_count++;
                if (flashlight_animation_count > flashlight_animation_frames) {
                    flashlight_animation_count = 0;
                    flashlight_anim_done = true;
                }
            } else {
                refreshPixelateShader( pixelate_intensity );
                window.draw(main_pic, pixelate_state);
                
                drawGrid(window);
                if (! hide_menu )
                    drawTextMenu(score_sound, back_sprite, window);
            }
        } else {
            readPuzzleTab(window);
            drawGrid(window);

            time_elapsed = clock.getElapsedTime().asSeconds();
            // for HD mode
            if ( (++hidden_tick_count) % hidden_tick_time == 0) {
                pickLuckyPiece();
                hidden_tick_count = 0;
            }
            
            if (mode_flashlight) {
                x = x < 0 ? 0 : x; x = x > width ? height : x;
                y = y < 0 ? 0 : y; y = y > height ? height : y;
                sf::Vector2u size = flashlight_texture.getSize();
                flashlight_back.setPosition(x - size.x/2, y - size.y/2);
                window.draw(flashlight_back);
                // window.draw(points, flashlight_state);
            }

            if (gameClear()) {
                if (mode_flashlight)
                    flashlight_anim_done = false;
                count_done = false;
                first = true;
                // pixelate_intensity = pixelate_intensity_init;
                congrats_sound.play();
                // newGame();
                // increaseLevel(true);
                next_sound.play();
                // dealing with modes result
                if (mode_hidden)
                    modes_used->push_back("[HD]");
                if (mode_flashlight)
                    modes_used->push_back("[FL]");
                if (mode_random_hole)
                    modes_used->push_back("[RL]");
                if (show_numbers) {
                    modes_used->push_back("[NB]");
                }
                
                if (modes_used->size() == 0) {
                    modes_used->push_back("\n"); // a little trick
                }
            }
        }

        if (turn_on_chrono)
            drawChrono(time_text, window);

        if (take_screenshot) {
            take_screenshot = false;
            sf::Vector2u windowSize = window.getSize();
            sf::Texture texture;
            texture.create(windowSize.x, windowSize.y);
            texture.update(window);
            sf::Image screenshot = texture.copyToImage();

            screenshot.saveToFile("screenshots/screenshot." +doubleToStr( randomGeneratorInf(10000) )+ ".png");
        }

        window.display();
        window.clear();
        sf::sleep(sf::milliseconds(ms_time));
    }
    return 0;
}
