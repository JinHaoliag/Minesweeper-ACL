/*
Creator: JinHaoliag
Date: 2025/02/08
Description: 扫雷游戏
  一个简单的扫雷游戏，使用ACL库实现，支持左键揭示格子，右键标记地雷，N键开始新游戏，R键重新开始游戏。
  有10个地雷，9x9的格子，每个格子有可能是地雷，也有可能是安全格子，揭示安全格子时会显示周围的地雷数。
  揭示到地雷时游戏结束，揭示所有非地雷格子时游戏胜利。
  通过随机生成地雷位置，预计算每个格子周围的地雷数，实现了简单的扫雷逻辑。
  通过ACL库的绘图功能，实现了简单的界面显示。
*/

#include <vector>
#include <random>
#include <string>
#include "acllib.h"

using namespace std;

//  游戏配置常量
const int GRID_SIZE = 9;
const int CELL_SIZE = 32;
const int MINE_COUNT = 10;
const int WINDOW_SIZE = GRID_SIZE * CELL_SIZE;

// 图像资源
ACL_Image blank, boom, def, flag;

// 游戏状态封装
struct GameState
{
    vector<vector<bool>> revealed;  // 已揭示格子
    vector<vector<bool>> mines;     // 地雷位置
    vector<vector<bool>> flags;     // 标记位置
    vector<vector<int>> neighbors;  // 预计算的相邻地雷数
	bool gameOver;                  // 游戏结束标志
	int safeCells;                  // 安全格子数

    GameState() :
        revealed(GRID_SIZE, vector<bool>(GRID_SIZE, false)),
        mines(GRID_SIZE, vector<bool>(GRID_SIZE, false)),
        flags(GRID_SIZE, vector<bool>(GRID_SIZE, false)),
        neighbors(GRID_SIZE, vector<int>(GRID_SIZE, 0)),
        gameOver(false) {
    }
};

GameState game;

// 图像加载
void loadImg()
{
    loadImage("res/blank.jpg", &blank);
    loadImage("res/boom.jpg", &boom);
    loadImage("res/def.jpg", &def);
    loadImage("res/flag.jpg", &flag);
}

//  绘制单元格
void drawCell(int x, int y)
{
    int pixelX = x * CELL_SIZE;
    int pixelY = y * CELL_SIZE;

    beginPaint();
    if (game.revealed[x][y])
    {
        if (game.mines[x][y])  putImage(&boom, pixelX + 1, pixelY + 1);
        else
        {
            putImage(&blank, pixelX + 1, pixelY + 1);
            if (game.neighbors[x][y] > 0)
            {
                setTextSize(16);
                paintText(pixelX + 12, pixelY + 8, to_string(game.neighbors[x][y]).c_str());
            }
        }
    }
    else if (game.flags[x][y])  putImage(&flag, pixelX + 1, pixelY + 1);
    else  putImage(&def, pixelX + 1, pixelY + 1);
    endPaint();
}

// 打印当前棋盘状态
void printBoard()
{
    for (int i = 0; i < GRID_SIZE; ++i)
    {
        for (int j = 0; j < GRID_SIZE; ++j)
        {
            drawCell(i, j);
        }
    }
}

//  绘制网格
void drawGrid()
{
    beginPaint();
    for (int i = 0; i <= GRID_SIZE; ++i)
    {
        line(i * CELL_SIZE, 0, i * CELL_SIZE, WINDOW_SIZE);
    }
    for (int i = 0; i <= GRID_SIZE; ++i)
    {
        line(0, i * CELL_SIZE, WINDOW_SIZE, i * CELL_SIZE);
    }
    endPaint();
	printBoard();
}

// 检查坐标是否有效
bool isValid(int x, int y)
{
    return x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE;
}

// 计算相邻地雷数（初始化时预计算）
void calculateNeighbors() {
    for (int x = 0; x < GRID_SIZE; ++x) {
        for (int y = 0; y < GRID_SIZE; ++y) {
            if (game.mines[x][y])  continue;

            int count = 0;
            for (int dx = -1; dx <= 1; ++dx) {
                for (int dy = -1; dy <= 1; ++dy) {
                    int nx = x + dx;
                    int ny = y + dy;
                    if (isValid(nx, ny) && game.mines[nx][ny])  ++count;
                }
            }
            game.neighbors[x][y] = count;
        }
    }
}

// 初始化游戏
void defGame()
{
	game = GameState();
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, GRID_SIZE - 1);

    int minesPlaced = 0;
    while (minesPlaced < MINE_COUNT)
    {
        int x = dis(gen);
        int y = dis(gen);
        if (!game.mines[x][y])
        {
            game.mines[x][y] = true;
            minesPlaced++;
        }
    }
	calculateNeighbors();
}

// 重新开始游戏
void reGame()
{
    game.gameOver = false;
    game.safeCells = GRID_SIZE * GRID_SIZE - MINE_COUNT;
    for (int i = 0; i < GRID_SIZE; ++i)
    {
        for (int j = 0; j < GRID_SIZE; ++j)
        {
            game.revealed[i][j] = false;
            game.flags[i][j] = false;
        }
    }
    drawGrid();
}

// 开始新游戏
void newGame()
{
    defGame();
    drawGrid();
}

// 递归揭示格子
void reveal(int x, int y)
{
	if (!isValid(x, y) || game.revealed[x][y] || game.flags[x][y])  return; // 不合法的格子或已揭示的格子或已标记的格子

    game.revealed[x][y] = true;
	--game.safeCells;

    if (game.neighbors[x][y] == 0)
    {
        for (int dx = -1; dx <= 1; ++dx)
        {
            for (int dy = -1; dy <= 1; ++dy)
            {
                if (dx != 0 || dy != 0)  reveal(x + dx, y + dy);    // 递归揭示相邻格子
            }
        }
    }
}

// 检查是否胜利（所有非地雷格子都被揭示）
bool checkWin()
{
    return game.safeCells == 0;
}

// 鼠标事件处理
void mouseListener(int x, int y, int button, int event)
{
    if (game.gameOver)  return; // 如果游戏结束，不再处理点击事件

    int gridX = x / CELL_SIZE;
    int gridY = y / CELL_SIZE;

    if (!isValid(gridX, gridY))  return;

    if (button == LEFT_BUTTON && event == BUTTON_DOWN)
    {
        if (game.flags[gridX][gridY])  return; // 如果格子被标记，不处理左键点击

        if (game.mines[gridX][gridY])
        {
            // 如果踩到地雷，游戏结束
            game.gameOver = true;
            // 揭示所有地雷
            for (int i = 0; i < GRID_SIZE; ++i)
            {
                for (int j = 0; j < GRID_SIZE; ++j)
                {
                    if (game.mines[i][j])  game.revealed[i][j] = true;
                }
            }
            printBoard(); // 打印最终棋盘

            if (MessageBox(NULL,("你输了!\n(重试可重新游玩)"), ("游戏结束"), MB_RETRYCANCEL) == IDRETRY)  reGame();
            return;
        }

        reveal(gridX, gridY); // 揭示当前格子
        printBoard(); // 更新棋盘显示

        if (checkWin())
        {
			// 如果胜利，游戏结束
            game.gameOver = true;
            
			if (MessageBox(NULL, ("你赢了!\n(重试可开始新游戏)"), ("游戏结束"), MB_RETRYCANCEL) == IDRETRY)  newGame();
            return;
        }
    }
    else if (button == RIGHT_BUTTON && event == BUTTON_DOWN)
    {
        // 右键点击标记地雷
        if (!game.revealed[gridX][gridY])
        {
            game.flags[gridX][gridY] = !game.flags[gridX][gridY]; // 切换标记状态
            printBoard(); // 更新棋盘显示
        }
    }
}

// 键盘事件处理
void keyListener(int key, int event)
{
    if (event == KEY_DOWN)
    {
        if (key == 'N' || key == 'R')
        {
            if (key == 'N') defGame();
            reGame();
        }
    }
}

int Setup()
{
    initWindow("扫雷", 200, 200, WINDOW_SIZE, WINDOW_SIZE);
    loadImg();
    drawGrid();
    defGame();
    registerMouseEvent(mouseListener);
    registerKeyboardEvent(keyListener);
    return 0;
}

int main()
{
    Setup();
    return 0;
}