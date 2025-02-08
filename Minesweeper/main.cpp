/*
Creator: JinHaoliag
Date: 2025/02/08
Description: ɨ����Ϸ
  һ���򵥵�ɨ����Ϸ��ʹ��ACL��ʵ�֣�֧�������ʾ���ӣ��Ҽ���ǵ��ף�N����ʼ����Ϸ��R�����¿�ʼ��Ϸ��
  ��10�����ף�9x9�ĸ��ӣ�ÿ�������п����ǵ��ף�Ҳ�п����ǰ�ȫ���ӣ���ʾ��ȫ����ʱ����ʾ��Χ�ĵ�������
  ��ʾ������ʱ��Ϸ��������ʾ���зǵ��׸���ʱ��Ϸʤ����
  ͨ��������ɵ���λ�ã�Ԥ����ÿ��������Χ�ĵ�������ʵ���˼򵥵�ɨ���߼���
  ͨ��ACL��Ļ�ͼ���ܣ�ʵ���˼򵥵Ľ�����ʾ��
*/

#include <vector>
#include <random>
#include <string>
#include "acllib.h"

using namespace std;

//  ��Ϸ���ó���
const int GRID_SIZE = 9;
const int CELL_SIZE = 32;
const int MINE_COUNT = 10;
const int WINDOW_SIZE = GRID_SIZE * CELL_SIZE;

// ͼ����Դ
ACL_Image blank, boom, def, flag;

// ��Ϸ״̬��װ
struct GameState
{
    vector<vector<bool>> revealed;  // �ѽ�ʾ����
    vector<vector<bool>> mines;     // ����λ��
    vector<vector<bool>> flags;     // ���λ��
    vector<vector<int>> neighbors;  // Ԥ��������ڵ�����
	bool gameOver;                  // ��Ϸ������־
	int safeCells;                  // ��ȫ������

    GameState() :
        revealed(GRID_SIZE, vector<bool>(GRID_SIZE, false)),
        mines(GRID_SIZE, vector<bool>(GRID_SIZE, false)),
        flags(GRID_SIZE, vector<bool>(GRID_SIZE, false)),
        neighbors(GRID_SIZE, vector<int>(GRID_SIZE, 0)),
        gameOver(false) {
    }
};

GameState game;

// ͼ�����
void loadImg()
{
    loadImage("res/blank.jpg", &blank);
    loadImage("res/boom.jpg", &boom);
    loadImage("res/def.jpg", &def);
    loadImage("res/flag.jpg", &flag);
}

//  ���Ƶ�Ԫ��
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

// ��ӡ��ǰ����״̬
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

//  ��������
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

// ��������Ƿ���Ч
bool isValid(int x, int y)
{
    return x >= 0 && x < GRID_SIZE && y >= 0 && y < GRID_SIZE;
}

// �������ڵ���������ʼ��ʱԤ���㣩
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

// ��ʼ����Ϸ
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

// ���¿�ʼ��Ϸ
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

// ��ʼ����Ϸ
void newGame()
{
    defGame();
    drawGrid();
}

// �ݹ��ʾ����
void reveal(int x, int y)
{
	if (!isValid(x, y) || game.revealed[x][y] || game.flags[x][y])  return; // ���Ϸ��ĸ��ӻ��ѽ�ʾ�ĸ��ӻ��ѱ�ǵĸ���

    game.revealed[x][y] = true;
	--game.safeCells;

    if (game.neighbors[x][y] == 0)
    {
        for (int dx = -1; dx <= 1; ++dx)
        {
            for (int dy = -1; dy <= 1; ++dy)
            {
                if (dx != 0 || dy != 0)  reveal(x + dx, y + dy);    // �ݹ��ʾ���ڸ���
            }
        }
    }
}

// ����Ƿ�ʤ�������зǵ��׸��Ӷ�����ʾ��
bool checkWin()
{
    return game.safeCells == 0;
}

// ����¼�����
void mouseListener(int x, int y, int button, int event)
{
    if (game.gameOver)  return; // �����Ϸ���������ٴ������¼�

    int gridX = x / CELL_SIZE;
    int gridY = y / CELL_SIZE;

    if (!isValid(gridX, gridY))  return;

    if (button == LEFT_BUTTON && event == BUTTON_DOWN)
    {
        if (game.flags[gridX][gridY])  return; // ������ӱ���ǣ�������������

        if (game.mines[gridX][gridY])
        {
            // ����ȵ����ף���Ϸ����
            game.gameOver = true;
            // ��ʾ���е���
            for (int i = 0; i < GRID_SIZE; ++i)
            {
                for (int j = 0; j < GRID_SIZE; ++j)
                {
                    if (game.mines[i][j])  game.revealed[i][j] = true;
                }
            }
            printBoard(); // ��ӡ��������

            if (MessageBox(NULL,("������!\n(���Կ���������)"), ("��Ϸ����"), MB_RETRYCANCEL) == IDRETRY)  reGame();
            return;
        }

        reveal(gridX, gridY); // ��ʾ��ǰ����
        printBoard(); // ����������ʾ

        if (checkWin())
        {
			// ���ʤ������Ϸ����
            game.gameOver = true;
            
			if (MessageBox(NULL, ("��Ӯ��!\n(���Կɿ�ʼ����Ϸ)"), ("��Ϸ����"), MB_RETRYCANCEL) == IDRETRY)  newGame();
            return;
        }
    }
    else if (button == RIGHT_BUTTON && event == BUTTON_DOWN)
    {
        // �Ҽ������ǵ���
        if (!game.revealed[gridX][gridY])
        {
            game.flags[gridX][gridY] = !game.flags[gridX][gridY]; // �л����״̬
            printBoard(); // ����������ʾ
        }
    }
}

// �����¼�����
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
    initWindow("ɨ��", 200, 200, WINDOW_SIZE, WINDOW_SIZE);
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