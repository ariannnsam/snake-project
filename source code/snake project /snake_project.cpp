#include <iostream>
#include <conio.h>
#include <fcntl.h>
#include <io.h>
#include <windows.h>

using namespace std;

enum SnakeMove { mov_LEFT = 1, mov_RIGHT, mov_UP, mov_DOWN };
enum MyColor { NONE = -1, Black = 0, Blue, Green, Aqua, Red, Purple, Yellow, White, Gray, LightBlue, LightGreen, LightAqua, LightRed, LightPurple, LightYellow, BrightWhite };

int cols = 104;
int rows = 47;
char console_Snake1_size_str[1024];
wstring playerName1, playerName2;
int dfc;
int player1Wins = 0, player2Wins = 0;
MyColor Snake1_Color = Purple;
MyColor Snake2_Color = LightGreen;
int Snake1_scr, Snake2_scr;

void TextColor(MyColor foreColor, MyColor backColor = NONE) {
	if (foreColor < Black || foreColor>BrightWhite) return;
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	if (backColor >= Black && backColor <= BrightWhite)
		SetConsoleTextAttribute(h, foreColor | backColor * 16);
	else
		SetConsoleTextAttribute(h, foreColor);
}

void gotoxy(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void DrawGameBoard() {
	sprintf(console_Snake1_size_str, "mode con:cols=%d lines=%d", cols, rows);
	HANDLE hout;
	CONSOLE_CURSOR_INFO cursor;
	hout = GetStdHandle(STD_OUTPUT_HANDLE);
	cursor.dwSize = 1;
	cursor.bVisible = false;
	SetConsoleCursorInfo(hout, &cursor);
	//system("mode con:cols=85 lines=25");
	system(console_Snake1_size_str);
	system("My Snake Game");
	system("cls");
	TextColor(LightBlue);
	wprintf(L"\n  ╔"); //┌
	int x;
	for (x = 0; x < cols - 5; x++)
		wprintf(L"═");
	wprintf(L"╗  ");
	for (x = 0; x < rows - 8; x++) {
		gotoxy(2, x + 2);
		wprintf(L"║");
		gotoxy(cols - 2, x + 2);
		wprintf(L"║ ");
	}
	wprintf(L"  ╚");
	for (x = 0; x < cols - 5; x++)
		wprintf(L"═");
	wprintf(L"╝  ");

	//display game information
	TextColor(LightYellow);
	wprintf(L" ┌");
	for (x = 0; x < 41; x++)
		wprintf(L"─");
	wprintf(L"┐\n");
	wprintf(L"  │Press 'SPACE' to Pause/resume            │\n");
	wprintf(L"  │");
	TextColor(Snake1_Color);
	wprintf(L"Press 'UP,DOWN,RIGHT,LEFT' to move snake1");
	TextColor(LightYellow);
	wprintf(L"│\n");
	wprintf(L"  │");
	TextColor(Snake2_Color);
	wprintf(L"Press 'w,s,d,a' to move snake2           ");
	TextColor(LightYellow);
	wprintf(L"│\n");
	wprintf(L"  └");
	for (x = 0; x < 41; x++)
		wprintf(L"─");
	wprintf(L"┘");

	TextColor(White);
}

void PrintStatus(wstring status, MyColor c) {
	gotoxy((cols / 2) - (status.size() / 2 + 2), rows - 3);
	TextColor(c);
	wcout << "    " << status << "    ";
	TextColor(White);
}

void PrintScore(int sc1, int sc2) {
	TextColor(LightYellow);
	gotoxy(cols - 21, rows - 5);
	wprintf(L"┌");
	for (int x = 0; x < 18; x++)
		wprintf(L"─");
	wprintf(L"┐");
	gotoxy(cols - 21, rows - 4);
	wprintf(L"│");
	TextColor(Snake1_Color);
	wprintf(L" SCORE 1: %3d     ", sc1);
	TextColor(LightYellow);
	wprintf(L"│");
	gotoxy(cols - 21, rows - 3);
	wprintf(L"│");
	TextColor(Snake2_Color);
	wprintf(L" SCORE 2: %3d     ", sc2);
	TextColor(LightYellow);
	wprintf(L"│");
	gotoxy(cols - 21, rows - 2);
	wprintf(L"└");
	for (int x = 0; x < 18; x++)
		wprintf(L"─");
	wprintf(L"┘");


}

int _Playing() {
	DrawGameBoard();
	int Snake1_flow, Snake1_size, Food_x, Food_y, Snake1_xpos[100], Snake1_ypos[100];
	int Snake2_flow, Snake2_size, Snake2_xb, Snake2_yb, Snake2_xpos[100], Snake2_ypos[100];
	int speed, FirstTime = 1, tmp, bomb_x, bomb_y;
	srand(time(NULL));
	while (true) {
		if (FirstTime) {
			PrintStatus(L"Playing...", LightAqua);
			for (int k = 1; k < cols - 5; k += 2)
				for (int j = 0; j < rows - 8; j++) {
					gotoxy(k + 3, j + 2);
					wprintf(L" ");
				}
			Snake1_size = 4;
			Snake1_scr = 0;

			Snake2_size = 4;
			Snake2_scr = 0;
			PrintScore(Snake1_scr, Snake2_scr);

			speed = dfc;

			Snake1_flow = mov_RIGHT;
			Snake1_xpos[0] = 20;

			Snake2_flow = mov_DOWN;
			Snake2_xpos[0] = cols - 5 - 20;

			for (int i = 0; i < Snake1_size; i++) {
				Snake1_xpos[i] = Snake1_xpos[0] - i ;
				Snake1_ypos[i] = 10;
			}
			for (int i = 0; i < Snake1_size; i++) {
				gotoxy(Snake1_xpos[i], Snake1_ypos[i]);
				TextColor(Snake1_Color);
				wprintf(L"+");

			}

			for (int i = 0; i < Snake2_size; i++) {
				Snake2_xpos[i] = Snake2_xpos[0] - i;
				Snake2_ypos[i] = 10;
			}
			for (int i = 0; i < Snake2_size; i++) {
				gotoxy(Snake2_xpos[i], Snake2_ypos[i]);
				TextColor(Snake2_Color);
				wprintf(L"+");
			}
			
				for (tmp = 1; true;) { //put food
					do {
						Food_x = 3 + (rand() % (cols - 6));
					} while (Food_x % 2 != 0);
					Food_y = 2 + (rand() % (rows - 8));
					for (int i = 0; i < Snake1_size; i++) {
						if (Food_x == Snake1_xpos[i] && Food_y == Snake1_ypos[i] && Food_x == Snake2_xpos[i] && Food_y == Snake2_ypos[i]) {
							tmp = 0; break;
						}
					}
					if (tmp)
						break;
				}
				gotoxy(Food_x, Food_y);
				TextColor(Aqua);
				wprintf(L"O");
				TextColor(White);
			

			//put bomb
			for (tmp = 1; true;) {
				do {
					bomb_x = 3 + (rand() % (cols - 6));
				} while (bomb_x % 2 != 0);
				bomb_y = 2 + (rand() % (rows - 8));
				for (int i = 0; i < Snake1_size; i++) {
					if (bomb_x == Snake1_xpos[i] && bomb_y == Snake1_ypos[i] && bomb_x == Snake2_xpos[i] && bomb_y == Snake2_ypos[i]) {
						tmp = 0; break;
					}
				}
				if (tmp)
					break;
			}
			gotoxy(bomb_x, bomb_y);
			TextColor(LightGreen, LightRed);
			wprintf(L"B");
			TextColor(White, NONE);


			FirstTime = 0;
		}
		while (!_kbhit() && !FirstTime) {
			if (Snake1_xpos[0] == Food_x && Snake1_ypos[0] == Food_y) {
				for (tmp = 1; true;) {
					do {
						Food_x = 4 + (rand() % (cols - 6));
					} while (Food_x % 2 != 0);
					Food_y = 2 + (rand() % (rows - 8));
					for (int i = 0; i < Snake1_size; i++)
						if (Food_x == Snake1_xpos[i] && Food_y == Snake1_ypos[i] && Food_x == Snake2_xpos[i] && Food_y == Snake2_ypos[i]) {
							tmp = 0; break;
						}
					if (tmp)
						break;
				}
				gotoxy(Food_x, Food_y);
				TextColor(Aqua);
				wprintf(L"O");
				TextColor(White);
				Snake1_size++;
				Snake1_scr++;
				PrintScore(Snake1_scr, Snake2_scr);
			}

			if (Snake2_xpos[0] == Food_x && Snake2_ypos[0] == Food_y) {
				for (tmp = 1; true;) {
					do {
						Food_x = 4 + (rand() % (cols - 6));
					} while (Food_x % 2 != 0);
					Food_y = 2 + (rand() % (rows - 8));
					for (int i = 0; i < Snake2_size; i++)
						if (Food_x == Snake1_xpos[i] && Food_y == Snake1_ypos[i] && Food_x == Snake2_xpos[i] && Food_y == Snake2_ypos[i]) {
							tmp = 0; break;
						}
					if (tmp)
						break;
				}
				gotoxy(Food_x, Food_y);
				TextColor(Aqua);
				wprintf(L"O");
				TextColor(White);
				Snake2_size++;
				Snake2_scr++;
				PrintScore(Snake1_scr, Snake2_scr);
			}

			//Snake 1 moving
			gotoxy(Snake1_xpos[Snake1_size - 1], Snake1_ypos[Snake1_size - 1]);
			int flw1_sz = 0;
			for (flw1_sz = Snake1_size - 1; flw1_sz > 0; flw1_sz--) {
				Snake1_xpos[flw1_sz] = Snake1_xpos[flw1_sz - 1];
				Snake1_ypos[flw1_sz] = Snake1_ypos[flw1_sz - 1];
			}
			switch (Snake1_flow) {
			case mov_RIGHT:
				Snake1_xpos[flw1_sz] += 1; 
				break;
			case mov_LEFT:	
				Snake1_xpos[flw1_sz] -= 1; 
				break;
			case mov_UP:	
				Snake1_ypos[flw1_sz] -= 1; 
				break;
			case mov_DOWN:	
				Snake1_ypos[flw1_sz] += 1;
				break;
			}
			tmp = 1;
			for (int i = 1; i < Snake1_size; i++) { // 
				if (Snake1_xpos[i] == Snake1_xpos[0] && Snake1_ypos[i] == Snake1_ypos[0]) {
					tmp = 0;
					break;
				}
			}

			for (int i = 1; i < Snake2_size; i++) { //برخورد مار1 با مار2
				if (Snake2_xpos[i] == Snake1_xpos[0] && Snake2_ypos[i] == Snake1_ypos[0]) {
					tmp = 0;
					break;
				}
			}

			if (Snake1_xpos[0] > cols - 4 || Snake1_xpos[0] < 4 || Snake1_ypos[0] < 2 || Snake1_ypos[0]>rows - 7)	//برخورد مار 1 با دیوار
				tmp = 0;

			if (Snake1_xpos[0] == bomb_x && Snake1_ypos[0] == bomb_y) //برخورد مار1 با بمب
				tmp = 0;

			if (tmp) {
				TextColor(Snake1_Color);
				wprintf(L" ");
				gotoxy(Snake1_xpos[0], Snake1_ypos[0]);
				wprintf(L"@");
				gotoxy(Snake1_xpos[1], Snake1_ypos[1]);
				wprintf(L"+");
			}
			else {
				TextColor(LightRed);
				wprintf(L"+");
				gotoxy(Snake1_xpos[1], Snake1_ypos[1]);
				wprintf(L"@");
				for (int i = 2; i < Snake1_size; i++) {
					gotoxy(Snake1_xpos[i], Snake1_ypos[i]);
					wprintf(L"+");
				}
				TextColor(White);
				PrintStatus(L"GameOver", Red);
				FirstTime = 1;
				_getch();
				return 0;
			}

			gotoxy(Snake2_xpos[Snake2_size - 1], Snake2_ypos[Snake2_size - 1]);
			int flw2_sz = 0;
			for (flw2_sz = Snake2_size - 1; flw2_sz > 0; flw2_sz--) {
				Snake2_xpos[flw2_sz] = Snake2_xpos[flw2_sz - 1];
				Snake2_ypos[flw2_sz] = Snake2_ypos[flw2_sz - 1];
			}
			switch (Snake2_flow) {
			case mov_RIGHT:
				Snake2_xpos[flw2_sz] += 1; 
				break;
			case mov_LEFT:	
				Snake2_xpos[flw2_sz] -= 1; 
				break;
			case mov_UP:	
				Snake2_ypos[flw2_sz] -= 1; 
				break;
			case mov_DOWN:	
				Snake2_ypos[flw2_sz] += 1;
				break;
			}
			tmp = 1;
			for (int i = 1; i < Snake2_size; i++) {  //Snake 2 collides with itself
				if (Snake2_xpos[i] == Snake2_xpos[0] && Snake2_ypos[i] == Snake2_ypos[0]) {
					tmp = 0;
					break;
				}
			}

			for (int i = 1; i < Snake1_size; i++) { //Snake 2 collides with Snake 1
				if (Snake1_xpos[i] == Snake2_xpos[0] && Snake1_ypos[i] == Snake2_ypos[0]) {
					tmp = 0;
					break;
				}
			}

			if (Snake2_xpos[0] > cols - 4 || Snake2_xpos[0] < 4 || Snake2_ypos[0] < 2 || Snake2_ypos[0]>rows - 7) //Snake 2 collides with wall
				tmp = 0;

			if (Snake2_xpos[0] == bomb_x && Snake2_ypos[0] == bomb_y) //Snake 2 collides with bomb
				tmp = 0;

			if (tmp) {
				TextColor(Snake2_Color);
				wprintf(L" ");
				gotoxy(Snake2_xpos[0], Snake2_ypos[0]);
				wprintf(L"@");
				gotoxy(Snake2_xpos[1], Snake2_ypos[1]);
				wprintf(L"+");
			}
			else {
				TextColor(LightRed);
				wprintf(L"+");
				gotoxy(Snake2_xpos[1], Snake2_ypos[1]);
				wprintf(L"@");
				for (int i = 2; i < Snake2_size; i++) {
					gotoxy(Snake2_xpos[i], Snake2_ypos[i]);
					wprintf(L"*");
				}
				TextColor(White);
				PrintStatus(L"GameOver", Red);
				FirstTime = 1;
				_getch();
				return 0;
			}
			Sleep(speed);
		}

		char ch = _getch();
		switch (tolower(ch)) {
		/*case 'x': 	system("cls");
			return 0;*/
		case ' ': PrintStatus(L"Paused", LightYellow);
			while (true) {
				char z = _getch();
				/*if (z == 'x')
					return 0;*/
				if (z == ' ')
					break;
			}
			PrintStatus(L"Playing...", LightAqua);
			break;

		case 'w': {
			if (Snake2_flow != mov_DOWN)
				Snake2_flow = mov_UP;
			break;
		}

		case 's': {
			if (Snake2_flow != mov_UP)
				Snake2_flow = mov_DOWN;
			break;
		}
		case 'a': {
			if (Snake2_flow != mov_RIGHT)
				Snake2_flow = mov_LEFT;
			break;
		}
		case 'd': {
			if (Snake2_flow != mov_LEFT)
				Snake2_flow = mov_RIGHT;
			break;
		}

		case -32: {
			char chh = _getch();
			if (chh == 72 && Snake1_flow != mov_DOWN)
				Snake1_flow = mov_UP;
			else if (chh == 80 && Snake1_flow != mov_UP)
				Snake1_flow = mov_DOWN;
			else if (chh == 75 && Snake1_flow != mov_RIGHT)
				Snake1_flow = mov_LEFT;
			else if (chh == 77 && Snake1_flow != mov_LEFT)
				Snake1_flow = mov_RIGHT;
			break;
		}
		}
	}
}

int _SetDifficulty()
{
	int res, choice;
	wcout << "\nSET DIFFICULTY\n1: Easy\n2: Medium\n3: hard \nChoose difficulty level: ";
	wcin >> choice;
	switch (choice) {
	case 1:
		res = 200;
		break;
	case 2:
		res = 100;
		break;
	case 3:
		res = 50;
		break;
	default:
		res = 100;
		break;
	}
	return res;
}

void PlayGame() {
	if (playerName1 == L"") {
		TextColor(Snake1_Color);
		wcout << L"enter player 1 name: "; wcin >> playerName1;
	}
	if (playerName2 == L"") {
		TextColor(Snake2_Color);
		wcout << L"enter player 2 name: "; wcin >> playerName2;
	}
	system("cls");
	TextColor(White);
	dfc = _SetDifficulty();
	_Playing();

	system("cls");
	if (Snake1_scr > Snake2_scr) {
		// winner player 1
		TextColor(Snake1_Color);
		wcout << L"player 1 \"" << playerName1 << "\" WIN!";
		wcout << L"\n\nplayer 1 score: " << Snake1_scr << "\n";
		TextColor(Snake2_Color);
		wcout << L"player 2 score: " << Snake2_scr << "\n";
		player1Wins += Snake1_scr;
		player2Wins += Snake2_scr;
	}
	else if (Snake1_scr < Snake2_scr) {
		// winner player 2
		TextColor(Snake2_Color);
		wcout << L"player 2 \"" << playerName2 << "\" WIN!";
		wcout << L"\n\nplayer 2 score: " << Snake2_scr << "\n";
		TextColor(Snake1_Color);
		wcout << L"player 1 score: " << Snake1_scr << "\n";
		player1Wins += Snake1_scr;
		player2Wins += Snake2_scr;
	}
	else {
		// equality
		TextColor(Green);
		wcout << L"Tie!";
		TextColor(Snake1_Color);
		wcout << L"\n\nplayer 1 score: " << Snake1_scr << "\n";
		TextColor(Snake2_Color);
		wcout << L"player 2 score: " << Snake2_scr << "\n";
		player1Wins += Snake1_scr;
		player2Wins += Snake2_scr;
	}
	TextColor(White);
	system("pause");
}

int main()
{
	_setmode(_fileno(stdout), _O_U8TEXT);
	TextColor(White, NONE);

	while (true) {
		int choice;
		system("cls");
		wcout << L"======= Main Menu ========\n";
		wcout << L"1-Play Game\n";
		wcout << L"2-Scores table\n";
		wcout << L"3-Change Playes\n";
		wcout << L"0-Exit\n";

		wcout << "Choose menu item: ";
		wcin >> choice;
		switch (choice) {
		case 1: { // start game
			system("cls");
			PlayGame();
			break;
		}
		case 2: { // score table
			system("cls");
			TextColor(Green);
			wcout << L"\n--------------------------------------------";
			wcout << L"\n|";
			TextColor(Snake1_Color);
			wcout << L" player (" << playerName1 << ") Win Number = " << player1Wins;
			TextColor(Green);
			wcout << L"\n--------------------------------------------";
			wcout << L"\n|";
			TextColor(Snake2_Color);
			wcout << L" player (" << playerName2 << ") Win Number = " << player2Wins;
			TextColor(Green);
			wcout << L"\n--------------------------------------------\n\n";
			TextColor(White);
			system("pause");
			break;
		}
		case 3: //Changing player
		{
			system("cls");
			TextColor(Snake1_Color);
			wcout << L"enter player 1 name: "; wcin >> playerName1;
			TextColor(Snake2_Color);
			wcout << L"enter player 2 name: "; wcin >> playerName2;
			system("cls");
			TextColor(White);
			break;
		}
		case 0: // output
		{
			system("cls");
			exit(0);
			break;
		}
		}
	}

	return 0;
}
