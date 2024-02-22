#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>       // Include Fl_Box for displaying images
#include <FL/fl_ask.H>       // Include fl_ask.H for message box
#include <FL/Fl_PNG_Image.H> // Include Fl_PNG_Image for PNG images
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <array>
#include <random>
#include <utility>
#include <iostream>
using namespace std;
const int BOARD_SIZE = 10;
const int SQUARE_SIZE = 57;
const int BORDER_X = 31;
const int BORDER_Y = 31;
const int WINDOW_WIDTH = BOARD_SIZE * SQUARE_SIZE + 200 + 2 * BORDER_X;
const int WINDOW_HEIGHT = BOARD_SIZE * SQUARE_SIZE + 2 * BORDER_Y;
// TODO: Need a way to figure out if the current player position has a snake or ladder without explicitly checking
// possibly can use a hashmap of positions to boardelements
// Should individual classes inherit from a drawable class that has a virtual draw method
// Or the drawing stuff of each class be done in a separate class
// like player class having a draw method that delegates drawing task to playerGUI's draw method (bridge? observer?)
// separates drawing logic from player logic

class DiceWidget : public Fl_Widget
{
    static DiceWidget *_instance;
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<> dist;
    std::array<Fl_PNG_Image *, 6> diceFaces;
    Fl_Box *diceBox;
    Fl_PNG_Image *currentFace;
    void setDiceImage(int pos)
    {
        currentFace = diceFaces[pos - 1];
    }
    DiceWidget(int x, int y, int w, int h, const char *label = nullptr)
        : Fl_Widget(x, y, w, h, label), gen(rd()), dist(1, 6), currentFace(nullptr)
    {

        for (int i = 0; i < diceFaces.max_size(); ++i)
        {
            std::string filename = "dice_" + std::to_string(i + 1) + ".png";
            diceFaces[i] = new Fl_PNG_Image(filename.c_str());
        }
        diceBox = new Fl_Box(750, 316, 0, 0, "");
        diceBox->box(FL_FLAT_BOX); // Set the box type to FL_FLAT_BOX for a flat appearance
        diceBox->color(FL_WHITE);  // Set the background color of the box
    }

public:
    static DiceWidget *getInstance(int x, int y, int w, int h, const char *label = nullptr)
    {

        if (_instance == nullptr)
        {
            _instance = new DiceWidget(x, y, w, h, label);
        }
        return _instance;
    }
    ~DiceWidget()
    {
        delete diceBox;
        for (int i = 0; i < diceFaces.max_size(); ++i, delete diceFaces[i])
            ;
    }
    void draw() override
    {
        //  Draw dice image

        if (currentFace)
        {
            diceBox->image(currentFace);
            diceBox->size(currentFace->w() + 10, currentFace->h() + 10);
            diceBox->redraw();
        }
    }
    unsigned int roll()
    {
        unsigned int r = dist(gen);
        setDiceImage(r);
        return r;
    }
};
DiceWidget *DiceWidget::_instance = nullptr;
class PlayerWidget : public Fl_Widget
{
    int position;
    Fl_Color color;
    Fl_Box *playerBox;
    int x, y, w, h;

public:
    PlayerWidget(int x, int y, int w, int h, uint col, const char *label = nullptr)
        : Fl_Widget(x, y, w, h, label), color(col), x(0), y(0), w(SQUARE_SIZE - 10), h(SQUARE_SIZE - 10), position(0)
    {
        playerBox = new Fl_Box(0, 0, w, h, "");
        playerBox->color(col);
        playerBox->box(FL_FLAT_BOX); // Set the box type to FL_FLAT_BOX for a flat appearance
    }
    ~PlayerWidget()
    {
        delete playerBox;
    }
    void updatePosition(int pos) { position = pos; }
    int getPosition() const { return position; }
    void updateCoordinates(int x, int y, int w, int h)
    {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }
    void draw() override
    {
        cout << "player drawn\n";
        cout << x << " " << y << " " << w << " " << h << endl;
        playerBox->resize(x, y, w, h);
    }
};

class AbstractBoard : public Fl_Widget
{
protected:
    AbstractBoard(int x, int y, int w, int h, const char *label = nullptr)
        : Fl_Widget(x, y, w, h, label)
    {
    }
    virtual void embedLogic() = 0;

public:
    virtual int getDestination(int pos) = 0;
    void draw(){};
    virtual std::pair<std::pair<int, int>, std::pair<int, int>> getPixelCoordinates(int pos) = 0;
};

class SnakeLadderBoardWidget : public AbstractBoard
{
    static SnakeLadderBoardWidget *_instance;
    Fl_Box *m_boardBox;
    Fl_PNG_Image *m_boardPNG;
    std::array<int, 100> positionMap;
    struct Square
    {
        int x, y;
        int number;
        int destination;
        Square(int x, int y, int number, int destination = -1) : x(x), y(y), number(number), destination(destination) {}
    };
    std::vector<Square> squares;
    void embedLogic() override
    {
        for (size_t i = 0; i < positionMap.max_size(); i++)
        {
            positionMap[i] = i;
        }
        positionMap[3] = 55;
        positionMap[11] = 49;
        positionMap[13] = 54;
        positionMap[21] = 57;
        positionMap[27] = 9;
        positionMap[36] = 2;
        positionMap[40] = 78;
        positionMap[47] = 15;
        positionMap[53] = 87;
        positionMap[74] = 31;
        positionMap[93] = 70;
        positionMap[95] = 41;
        bool reverse = false;
        int square_number = 1;
        for (int i = BOARD_SIZE - 1; i >= 0; --i)
        {
            if (reverse)
            {
                for (int j = BOARD_SIZE - 1; j >= 0; --j)
                {
                    squares.push_back(Square(j * SQUARE_SIZE + BORDER_X, i * SQUARE_SIZE + BORDER_Y, square_number++));
                }
            }
            else
            {
                for (int j = 0; j < BOARD_SIZE; ++j)
                {
                    squares.push_back(Square(j * SQUARE_SIZE + BORDER_X, i * SQUARE_SIZE + BORDER_Y, square_number++));
                }
            }
            reverse = !reverse;
        }
    }

    SnakeLadderBoardWidget(int x, int y, int w, int h, const char *label = nullptr)
        : AbstractBoard(x, y, w, h, label)
    {
        embedLogic();
        m_boardBox = new Fl_Box(0, 15, 0, 0, "");
        m_boardPNG = new Fl_PNG_Image("board.png");
    }

public:
    static SnakeLadderBoardWidget *getInstance(int x, int y, int w, int h, const char *label = nullptr)
    {
        if (_instance == nullptr)
        {
            _instance = new SnakeLadderBoardWidget(x, y, w, h, label);
        }
        return _instance;
    }
    ~SnakeLadderBoardWidget()
    {
        delete m_boardBox;
        delete m_boardPNG;
    }
    int getDestination(int pos) override // query the array to return the actual destination of player
    {
        return positionMap[pos - 1] + 1;
    }
    std::pair<std::pair<int, int>, std::pair<int, int>> getPixelCoordinates(int pos)
    {
        auto p = positionMap[pos];
        auto p1 = std::make_pair(squares[p].x + 5, squares[p].y + 5);
        auto p2 = std::make_pair(SQUARE_SIZE - 20, SQUARE_SIZE - 20);
        return std::make_pair(p1, p2);
    }
    void draw() override
    {

        m_boardBox->image(m_boardPNG);

        m_boardBox->size(m_boardPNG->w(), m_boardPNG->h());
    }
};
SnakeLadderBoardWidget *SnakeLadderBoardWidget::_instance = nullptr;
class ScoreBoardWidget : public Fl_Widget
{
    static ScoreBoardWidget *_instance;
    std::string msg;
    Fl_Box *msgBox;
    ScoreBoardWidget(int x, int y, int w, int h, const char *label = nullptr)
        : Fl_Widget(x, y, w, h, label)
    {
        msgBox = new Fl_Box(x + w + 10, 10, 100, 20, "");
        msgBox->labelfont(FL_ENGRAVED_BOX);
        msgBox->color(FL_WHITE); // Set the background colomsgBox
        msgBox->labelsize(16);
        msg = "";
    }

public:
    static ScoreBoardWidget *getInstance(int x, int y, int w, int h, const char *label = nullptr)
    {
        if (_instance == nullptr)
        {
            _instance = new ScoreBoardWidget(x, y, w, h, label);
        }
        return _instance;
    }
    ~ScoreBoardWidget()
    {
        delete msgBox;
    }
    void setMessage(std::string message)
    {
        msg = message;
    }
    void draw() override
    {
        msgBox->copy_label(msg.c_str());
    }
};
ScoreBoardWidget *ScoreBoardWidget::_instance = nullptr;

class BoardGameWidget : public Fl_Widget
{
protected:
    AbstractBoard *gameBoard;
    DiceWidget *dice;
    ScoreBoardWidget *scoreboard;
    PlayerWidget *player;

    virtual AbstractBoard *CreateBoard() = 0;
    virtual DiceWidget *CreateDice() = 0;
    virtual ScoreBoardWidget *CreateScoreBoard() = 0;
    virtual PlayerWidget *CreatePlayer() = 0;
    Fl_Window *window;

public:
    BoardGameWidget(int x, int y, int w, int h, Fl_Window *_window, const char *label = nullptr)
        : window(_window), Fl_Widget(x, y, w, h, label)
    {
    }
    void CreateGame()
    {
        gameBoard = CreateBoard();
        dice = CreateDice();
        scoreboard = CreateScoreBoard();
        player = CreatePlayer();
        auto coords = gameBoard->getPixelCoordinates(0);
        player->updateCoordinates(coords.first.first, coords.first.second, coords.second.first, coords.second.second);

        window->end();
    }
    int play()
    {

        window->show();
        return 0;
    }
    void draw()
    {
        gameBoard->draw();
        player->draw();
        scoreboard->draw();
        dice->draw();
    }
    virtual int handle(int event) = 0;
};
class SnakeLadderBoardGameWidget : public BoardGameWidget
{

    AbstractBoard *CreateBoard() override
    {
        SnakeLadderBoardWidget *gameBoard = SnakeLadderBoardWidget::getInstance(0, 0, WINDOW_WIDTH - 200, WINDOW_HEIGHT);
        return gameBoard;
    }
    DiceWidget *CreateDice() override
    {
        DiceWidget *dice = DiceWidget::getInstance(0, 0, WINDOW_WIDTH - 200, WINDOW_HEIGHT);
        return dice;
    }
    ScoreBoardWidget *CreateScoreBoard() override
    {
        ScoreBoardWidget *scoreboard = ScoreBoardWidget::getInstance(0, 0, WINDOW_WIDTH - 200, WINDOW_HEIGHT);
        return scoreboard;
    }
    PlayerWidget *CreatePlayer() override
    {
        PlayerWidget *pl = new PlayerWidget(0, 0, WINDOW_WIDTH - 200, WINDOW_HEIGHT, FL_RED);
        return pl;
    }

public:
    SnakeLadderBoardGameWidget(int x, int y, int w, int h, Fl_Window *_window, const char *label = nullptr)
        : BoardGameWidget(x, y, w, h, _window, label)

    {
    }
    int handle(int event) override
    {
        if (event == FL_PUSH)
        {
            redraw();
            cout << "inside handle\n";
            // get dice roll
            auto _throw = dice->roll();
            cout << _throw << endl;
            // get current player
            auto pos = player->getPosition();
            cout << pos << endl;
            // update player position
            pos += _throw;
            // update scoreboard
            std::string rollStr = "Dice Roll: " + std::to_string(_throw);
            scoreboard->setMessage(rollStr);
            // check destination from board
            auto newpos = gameBoard->getDestination(pos);

            auto coords = gameBoard->getPixelCoordinates(newpos);
            player->updatePosition(newpos);
            player->updateCoordinates(coords.first.first, coords.first.second, coords.second.first, coords.second.second);
            cout << coords.first.first << " " << coords.first.second << " " << coords.second.first << " " << coords.second.second << endl;

            // // handle game end
        }

        return Fl_Widget::handle(event);
    }
};

int main()
{
    Fl_Window *window = new Fl_Window(WINDOW_WIDTH, WINDOW_HEIGHT, "Snake and Ladder");
    BoardGameWidget *game = new SnakeLadderBoardGameWidget(0, 0, WINDOW_WIDTH - 200, WINDOW_HEIGHT, window);
    game->CreateGame();
    game->play();
    return Fl::run();
}
