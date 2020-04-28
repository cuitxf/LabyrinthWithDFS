#pragma warning(disable:4996)
#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <comdef.h>
using namespace std;

#define MAP_SIZE 40
#define BLOCK_SCALE 0.25
#define BLOCK_SIZE ((MAP_SIZE)*(MAP_SIZE)*(BLOCK_SCALE))
#define ARROW_UP 72
#define ARROW_DOWN 80
#define ARROW_LEFT 75
#define ARROW_RIGHT 77

void StartGame();
void ShowTheWay();
void AutoPlay();
void ShowMenu();
void GenerateMap();
void _DFS(int, int, int);
void PrintMap();
void InitDBuffer();
//上0 1，下2 3，左4 5，右6 7，实心方块8 9，玩家10 11,叉叉12 13
const wchar_t* strUnicode = L"\u2191\u2193\u2190\u2192\u25a0\u25c6\u00d7";
_bstr_t strbs = strUnicode;
char *strch = strbs;

HANDLE hOutput, hOutBuf;
COORD coord;
DWORD bytes;
int g_choice = -1, g_playeri = 0, g_playerj = 1, g_escape = 0;
int g_escapei = MAP_SIZE - 1, g_escapej = MAP_SIZE - 2;
char g_map[MAP_SIZE][MAP_SIZE * 2];
char g_map_copy[MAP_SIZE][MAP_SIZE * 2];
void InitDBuffer()
{
	hOutBuf = CreateConsoleScreenBuffer(
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
	);
	hOutput = CreateConsoleScreenBuffer(
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
	);
	CONSOLE_CURSOR_INFO cursor;
	cursor.bVisible = false;
	cursor.dwSize = sizeof(cursor);
	SetConsoleCursorInfo(hOutBuf, &cursor);
	SetConsoleCursorInfo(hOutput, &cursor);
}
void PrintMap()
{
	for (int i = 0; i < MAP_SIZE; i++)
	{
		coord.Y = i;
		WriteConsoleOutputCharacterA(hOutBuf, g_map[i], MAP_SIZE * 2, coord, &bytes);
	}
	SetConsoleActiveScreenBuffer(hOutBuf);
	for (int i = 0; i < MAP_SIZE; i++)
	{
		coord.Y = i;
		WriteConsoleOutputCharacterA(hOutput, g_map[i], MAP_SIZE * 2, coord, &bytes);
	}
	SetConsoleActiveScreenBuffer(hOutput);
}
void GenerateMap()
{
	while (1)
	{
		g_escape = 0;
		g_playeri = 0, g_playerj = 1;
		memset(g_map, 0, sizeof(g_map));
		for (int i = 0; i < MAP_SIZE; i++)
		{
			g_map[0][i * 2] = strch[8]; g_map[0][i * 2 + 1] = strch[9];
			g_map[MAP_SIZE - 1][i * 2] = strch[8]; g_map[MAP_SIZE - 1][i * 2 + 1] = strch[9];
			g_map[i][0] = strch[8]; g_map[i][1] = strch[9];
			g_map[i][MAP_SIZE * 2 - 2] = strch[8]; g_map[i][MAP_SIZE * 2 - 1] = strch[9];
		}
		g_map[g_playeri][g_playerj * 2] = strch[10]; g_map[g_playeri][g_playerj * 2 + 1] = strch[11];
		g_map[g_escapei][g_escapej * 2] = g_map[g_escapei][g_escapej * 2 + 1] = 0;
		int cnt = BLOCK_SIZE, i, j;
		while (cnt--)
		{
			srand((unsigned)time(0));
			while (!((i = rand() % (MAP_SIZE - 1)) > 0
				&& (j = rand() % (MAP_SIZE - 1)) > 0
				&& g_map[i][j * 2] == 0));
			g_map[i][j * 2] = strch[8]; g_map[i][j * 2 + 1] = strch[9];
		}
		memcpy(g_map_copy, g_map, sizeof(g_map));
		_DFS(g_playeri, g_playerj, 0);
		//PrintMap();
		if (g_escape)
		{
			memcpy(g_map, g_map_copy, sizeof(g_map_copy));
			g_escape = 0;
			break;
		}
	}
}
void StartGame()
{
	while (1)
	{
		PrintMap();
		if (kbhit())
		{
			g_map[g_playeri][g_playerj * 2] = g_map[g_playeri][g_playerj * 2 + 1] = 0;
			char ch = getch();
			if ((ch == 'W' || ch == 'w' || ch == ARROW_UP)
				&& g_playeri - 1 >= 0 && g_map[g_playeri - 1][g_playerj * 2] == 0)
			{
				g_playeri--;
			}
			else if ((ch == 'A' || ch == 'a' || ch == ARROW_LEFT)
				&& g_playerj - 1 >= 0 && g_map[g_playeri][(g_playerj - 1) * 2] == 0)
			{
				g_playerj--;
			}
			else if ((ch == 'S' || ch == 's' || ch == ARROW_DOWN)
				&& g_playeri + 1 < MAP_SIZE && g_map[g_playeri + 1][g_playerj * 2] == 0)
			{
				g_playeri++;
			}
			else if ((ch == 'D' || ch == 'd' || ch == ARROW_RIGHT)
				&& g_playerj + 1 < MAP_SIZE && g_map[g_playeri][(g_playerj + 1) * 2] == 0)
			{
				g_playerj++;
			}
			g_map[g_playeri][g_playerj * 2] = strch[10];
			g_map[g_playeri][g_playerj * 2 + 1] = strch[11];
			if (g_playeri == g_escapei && g_playerj == g_escapej)
			{
				printf("\n您已逃脱\n");
				break;
			}
		}
	}
}
void ShowTheWay()
{
	_DFS(g_playeri, g_playerj, 0);
	PrintMap();
}
void _DFS(int x, int y, int delay)
{
	if (x == g_escapei && y == g_escapej)
	{
		g_map[x][y * 2] = strch[2]; g_map[x][y * 2 + 1] = strch[3];
		g_escape = 1;
		if (delay > 0)
		{
			PrintMap();
		}
		return;
	}
	if (x<0 || y<0 || x>MAP_SIZE - 1 || y>MAP_SIZE - 1 || delay < 0)
	{
		return;
	}
	Sleep(delay);
	if (delay > 0)
	{
		PrintMap();
	}
	//down
	if (g_map[x + 1][y * 2] == 0)
	{
		g_map[x][y * 2] = strch[2]; g_map[x][y * 2 + 1] = strch[3];
		x++;
		_DFS(x, y, delay);
		if (g_escape)return;
		g_map[x][y * 2] = strch[12]; g_map[x][y * 2 + 1] = strch[13];
		x--;
	}
	//right
	if (g_map[x][(y + 1) * 2] == 0)
	{
		g_map[x][y * 2] = strch[6]; g_map[x][y * 2 + 1] = strch[7];
		y++;
		_DFS(x, y, delay);
		if (g_escape)return;
		g_map[x][y * 2] = strch[12]; g_map[x][y * 2 + 1] = strch[13];
		y--;
	}
	//up
	if (g_map[x - 1][y * 2] == 0)
	{
		g_map[x][y * 2] = strch[0]; g_map[x][y * 2 + 1] = strch[1];
		x--;
		_DFS(x, y, delay);
		if (g_escape)return;
		g_map[x][y * 2] = strch[12]; g_map[x][y * 2 + 1] = strch[13];
		x++;
	}
	//left
	if (g_map[x][(y - 1) * 2] == 0)
	{
		g_map[x][y * 2] = strch[4]; g_map[x][y * 2 + 1] = strch[5];
		y--;
		_DFS(x, y, delay);
		if (g_escape)return;
		g_map[x][y * 2] = strch[12]; g_map[x][y * 2 + 1] = strch[13];
		y++;
	}
}
void AutoPlay()
{
	_DFS(g_playeri, g_playerj, 50);
}
void ShowMenu()
{
	while (1)
	{
		cout << endl << endl << endl;
		cout << "		==============================" << endl
			<< "		========= 1.开始游戏 =========" << endl
			<< "		========= 2.显示路径 =========" << endl
			<< "		========= 3.自动进行 =========" << endl
			<< "		========= 4.退出游戏 =========" << endl
			<< "		==============================" << endl;
		cout << "做出你的选择(1~4): ";
		scanf("%d", &g_choice);
		InitDBuffer();
		while (!(g_choice >= 1 && g_choice <= 4))
		{
			printf("\n请输入正确的数字: ");
			scanf("%d", &g_choice);
		}
		if (g_choice == 1)
		{
			GenerateMap();
			StartGame();
		}
		else if (g_choice == 2)
		{
			GenerateMap();
			ShowTheWay();
		}
		else if (g_choice == 3)
		{
			GenerateMap();
			AutoPlay();
		}
		else if (g_choice == 4)
		{
			cout << "退出游戏" << endl;
			break;
		}
	}
}
int main()
{
	//上\u2193下\u2192左\u2190右\u2191实心方块\u25a0
	//cout << "\u00d7\u2718\u2610\u2612" << endl;
	ShowMenu();
	return 0;
}