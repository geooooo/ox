/*
********************
*                  *
*                  *
*  Game XO         *
*                  *
*	x|x|o      *
*	o|x|o      *
*       o|o|x      *
*                  *
*  For MS-DOS      *
*		   *
*  Version 1.00    *
*                  *
*                  *
*                  *
********************
*/



#include <stdlib.h>
#include <stddef.h>
#include <conio.h>
#include <time.h>
#include <math.h>


#define	ESC	 0x1b
#define F1	 0x3b
#define	F2	 0x3c
#define	PAGEUP	 0x49
#define PAGEDOWN 0x51
#define LEFT	 0x4b
#define	RIGHT	 0x4d
#define	TOP	 0x48
#define	BOTTOM	 0x50
#define ENTER	 0xd
#define SPACE	 0x20
#define	MARKER	 0xdb
#define	PLAYER1M 'O'
#define PLAYER2M 'X'

#define	X	     3
#define PLAYERS      2
#define TMP_STR_SIZE 4
#define	ALWAYS	     8
#define	MAX_ERRORS   5
#define	DELAY	     500
#define AI_DELAY     500
#define AI	     PLAYER2
#define HUMAN	     PLAYER1

#define	BGCOLOR		BLACK
#define	TEXTCOLOR	LIGHTBLUE
#define SELMODECOLOR	WHITE
#define	KEYTEXTCOLOR   	LIGHTMAGENTA
#define PLAYER1_COLOR	BLUE
#define	PLAYER2_COLOR	RED
#define	MARKER_COLOR	WHITE


typedef unsigned char uchar;
typedef enum {FALSE = 0, TRUE = 1} Bool;
typedef enum {PLAYER1 = 1, PLAYER2 = 2} Player;
/*
	EASY < 76 -> error computer ai
	NORM < 51 -> error computer ai
	HARD < 16 -> error computer ai
*/
typedef enum {EASY = 75, NORMAL = 50, HARD = 15} Computer_level;
typedef enum {UNDEFINED, PVSC, PVSP} Game_mode;


/******* UI FUNCTIONS ********/
/* Paint user interface */
void paint_ui(void);

/* Paint top bar */
void paint_top_bar(void);

/* Paint bottom bar */
void paint_bottom_bar(void);

/* Paint keys inforamation */
void paint_start_info(void);

/* Paint key text selected color */
void paint_keytext(char* message);

/* Paint statistic game */
void paint_statistic(void);

/* Paint switch player 1 or 2 */
void paint_switch_player(void);

/* Paint player point on field */
void paint_player_point(void);

/* Paint game field */
void paint_field(void);

/* Paint left and right bar */
void paint_float_bar(void);
/******* UI FUNCTIONS *********/


/******* GAME OX FUNCTIONS *******/
/* Initialization game */
void game_ini(void);

/* Initialization game field */
void field_ini(void);

/* Toggle player 1->2 or 2->1 */
void toggle_player(void);

/* Set point in field */
void field_set(void);

/* Process errors */
void process_error(void);

/* Win player the game ? */
Bool player_is_win(void);

/* Game field is full ? */
Bool field_is_full(void);

/* Run computer ai */
void run_ai(void);
/******* GAME OX CORE FUNCTIONS *******/

Game_mode game_mode = UNDEFINED;
Computer_level comp_lvl = NORMAL;
char field[X][X];
char xpos;
char ypos;
Player player;
uchar player_win[PLAYERS];
uchar game_count;



int main()
{
	extern Game_mode game_mode;
	extern Computer_level comp_lvl;
	uchar c;

	paint_ui();
	while (ALWAYS) {
		c = getch();
		if (c == ESC)              	/* ESC - exit */
			exit(EXIT_SUCCESS);
		if (c == ENTER)          	/* ENTER - set point */
			field_set();
		else if (c == 0) {		/* Function keys */
			c = getch();
			switch (c) {
			case F1:		/* F1 - Game mode player/computer */
				game_mode = PVSC;
				game_ini();
				break;
			case F2:		/* F2 - Game mode player/player */
				game_mode = PVSP;
				game_ini();
				break;
			case PAGEUP:		/* PAGEUP - up level computer */
				if (game_mode == PVSC) {
					game_ini();
					if (comp_lvl == EASY)
						comp_lvl = NORMAL;
					else if (comp_lvl == NORMAL)
						comp_lvl = HARD;
				}
				break;
			case PAGEDOWN:		/* PAGEDOWN - down level computer */
				if (game_mode == PVSC) {
					game_ini();
					if (comp_lvl == HARD)
						comp_lvl = NORMAL;
					else if (comp_lvl == NORMAL)
						comp_lvl = EASY;
				}
				break;
			case TOP:     		/* TOP - arrow top */
				if ((game_mode != UNDEFINED) &&
				    (ypos != 0))
					--ypos;
				break;
			case BOTTOM:            /* BOTTOM - arrow bottom */
				if ((game_mode != UNDEFINED) &&
				    (ypos != X-1))
					++ypos;
				break;
			case LEFT:              /* LEFT - arrow left */
				if ((game_mode != UNDEFINED) && (xpos != 0))
					--xpos;
				break;
			case RIGHT:             /* RIGHT - arrow right */
				if ((game_mode != UNDEFINED) && (xpos != X-1))
					++xpos;
				break;
			default:
				process_error();
			}
			paint_ui();
		}
		else
			process_error();
	}
	return EXIT_SUCCESS;
}





/**************************************************************************/
/******* UI FUNCTIONS *****************************************************/
/**************************************************************************/
void paint_keytext(char* msg)
{
	textcolor(KEYTEXTCOLOR);
	cputs(msg);
	textcolor(TEXTCOLOR);
}





void paint_ui()
{
	extern Game_mode game_mode;

	textmode(C40);
	_setcursortype(_NOCURSOR);
	textbackground(BGCOLOR);
	textcolor(TEXTCOLOR);
	clrscr();
	paint_top_bar();
	paint_bottom_bar();
	paint_float_bar();
	if (game_mode != UNDEFINED) {
		paint_field();
		paint_player_point();
	}
}





void paint_float_bar(void)
{
	extern Game_mode game_mode;

	if (game_mode == UNDEFINED)
		return;
	paint_statistic();
	paint_switch_player();
}





void paint_start_info(void)
{
	struct text_info mode_info;

	gettextinfo(&mode_info);
	paint_keytext("ESC");
	cputs(" - exit");
	gotoxy(mode_info.screenwidth/2-4,1);
	paint_keytext("F1");
	cputs(" - PvsC");
	gotoxy(mode_info.screenwidth-8,1);
	paint_keytext("F2");
	cputs(" - PvsP");
	gotoxy(mode_info.screenwidth/2-14,mode_info.screenheight/2);
	cputs("Switch game mode ");
	paint_keytext("F1");
	cputs(" or ");
	paint_keytext("F2");
	cputs("...");
	gotoxy(1,mode_info.screenheight-2);
	paint_keytext("Enter");
	cputs(" - set point");
	gotoxy(1,mode_info.screenheight-1);
	paint_keytext("PageUp");
	cputs(" - up level");
	gotoxy(1,mode_info.screenheight);
	paint_keytext("PageDown");
	cputs(" - down level");
	gotoxy(mode_info.screenwidth-10,mode_info.screenheight-2);
	cputs("Move cursor");
	gotoxy(mode_info.screenwidth-8,mode_info.screenheight-1);
	paint_keytext("   ^");
	gotoxy(mode_info.screenwidth-8,mode_info.screenheight);
	paint_keytext("<- I ->");
}





void paint_top_bar(void)
{
	extern Game_mode game_mode;
	struct text_info mode_info;

	gettextinfo(&mode_info);
	switch (game_mode) {
	case PVSC:
		paint_keytext("ESC");
		cputs(" - exit");
		textcolor(SELMODECOLOR);
		gotoxy(mode_info.screenwidth/2-4,1);
		paint_keytext("F1");
		cputs(" - PvsC");
		textcolor(TEXTCOLOR);
		gotoxy(mode_info.screenwidth-8,1);
		paint_keytext("F2");
		cputs(" - PvsP");
		break;
	case PVSP:
		paint_keytext("ESC");
		cputs(" - exit");
		gotoxy(mode_info.screenwidth/2-4,1);
		paint_keytext("F1");
		cputs(" - PvsC");
		textcolor(SELMODECOLOR);
		gotoxy(mode_info.screenwidth-8,1);
		paint_keytext("F2");
		cputs(" - PvsP");
		textcolor(TEXTCOLOR);
		break;
	case UNDEFINED:
		paint_start_info();
		break;
	}
}





void paint_bottom_bar(void)
{
	extern Game_mode game_mode;
	extern Computer_level comp_lvl;
	struct text_info mode_info;

	gettextinfo(&mode_info);
	if (game_mode != PVSC)
		return;
	gotoxy(mode_info.screenwidth/2-10,mode_info.screenheight);
	cputs("Computer level - ");
	textcolor(SELMODECOLOR);
	switch (comp_lvl) {
	case EASY:
		cputs("EASY");
		break;
	case NORMAL:
		cputs("NORMAL");
		break;
	case HARD:
		cputs("HARD");
		break;
	}
	textcolor(TEXTCOLOR);
}





void paint_statistic(void)
{
	extern uchar player_win[];
	struct text_info mode_info;
	char tmp_str[TMP_STR_SIZE];

	gettextinfo(&mode_info);
	gotoxy(1,mode_info.screenheight/2-1);
	textcolor(PLAYER1_COLOR);
	cputs("Player 1: ");
	itoa(player_win[0],tmp_str,10);
	textcolor(SELMODECOLOR);
	cputs(tmp_str);

	gotoxy(1,mode_info.screenheight/2);
	textcolor(PLAYER2_COLOR);
	cputs("Player 2: ");
	itoa(player_win[1],tmp_str,10);
	textcolor(SELMODECOLOR);
	cputs(tmp_str);

	gotoxy(1,mode_info.screenheight/2+1);
	textcolor(TEXTCOLOR);
	cputs("Games count: ");
	itoa(game_count,tmp_str,10);
	textcolor(SELMODECOLOR);
	cputs(tmp_str);
	textcolor(TEXTCOLOR);
}





void paint_switch_player(void)
{
     extern Player player;
     struct text_info mode_info;

     gettextinfo(&mode_info);
     gotoxy(mode_info.screenwidth-11,mode_info.screenheight/2);
     if (player == PLAYER1) {
	textcolor(PLAYER1_COLOR);
	putch(PLAYER1M);
	cputs(" -> Player1");
     }
     else {
	textcolor(PLAYER2_COLOR);
	putch(PLAYER2M);
	cputs(" -> Player2");
     }
}





void paint_field(void)
{
	extern char field[][X];
	struct text_info mode_info;

	gettextinfo(&mode_info);
	textcolor(TEXTCOLOR);
	gotoxy(mode_info.screenwidth/2-2,mode_info.screenheight/2-2);
	cprintf("%c\xba%c\xba%c",(field[0][0] == PLAYER1)? PLAYER1M : (field[0][0] == PLAYER2)? PLAYER2M : SPACE,
				 (field[0][1] == PLAYER1)? PLAYER1M : (field[0][1] == PLAYER2)? PLAYER2M : SPACE,
				 (field[0][2] == PLAYER1)? PLAYER1M : (field[0][2] == PLAYER2)? PLAYER2M : SPACE);
	gotoxy(mode_info.screenwidth/2-2,mode_info.screenheight/2-1);
	cputs("\xcd\xce\xcd\xce\xcd");
	gotoxy(mode_info.screenwidth/2-2,mode_info.screenheight/2);
	cprintf("%c\xba%c\xba%c",(field[1][0] == PLAYER1)? PLAYER1M : (field[1][0] == PLAYER2)? PLAYER2M : SPACE,
				 (field[1][1] == PLAYER1)? PLAYER1M : (field[1][1] == PLAYER2)? PLAYER2M : SPACE,
				 (field[1][2] == PLAYER1)? PLAYER1M : (field[1][2] == PLAYER2)? PLAYER2M : SPACE);
	gotoxy(mode_info.screenwidth/2-2,mode_info.screenheight/2+1);
	cputs("\xcd\xce\xcd\xce\xcd");
	gotoxy(mode_info.screenwidth/2-2,mode_info.screenheight/2+2);
	cprintf("%c\xba%c\xba%c",(field[2][0] == PLAYER1)? PLAYER1M : (field[2][0] == PLAYER2)? PLAYER2M : SPACE,
				 (field[2][1] == PLAYER1)? PLAYER1M : (field[2][1] == PLAYER2)? PLAYER2M : SPACE,
				 (field[2][2] == PLAYER1)? PLAYER1M : (field[2][2] == PLAYER2)? PLAYER2M : SPACE);
}





void paint_player_point(void)
{
	extern char field[][X];
	extern char xpos;
	extern char ypos;
	struct text_info mode_info;

	gettextinfo(&mode_info);
	gotoxy(mode_info.screenwidth/2-2+xpos*2,mode_info.screenheight/2-2+ypos*2);
	textcolor(MARKER_COLOR);
	putch(MARKER);
	textcolor(TEXTCOLOR);
}





/**************************************************************************/
/******************* GAME OX FUNCTIONS ************************************/
/**************************************************************************/
void game_ini(void)
{
	extern Player player;
	extern uchar player_win[];
	extern uchar game_count;

	srand((int)time(NULL));
	player = (rand()%3+1 == 1)? PLAYER1 : PLAYER2;	/* Randomize one player */
	game_count = 0;
	player_win[0] = player_win[1] = 0;
	field_ini();
	if ((game_mode == PVSC) && (player == PLAYER2))
		run_ai();
}





void field_ini(void)
{
	extern char field[X][X];
	extern char xpos;
	extern char ypos;
	char y;
	char x;

	/* Initialize game field */
	xpos = ypos = 0;
	for (y = 0 ; y < X; ++y)
		for (x = 0; x < X; ++x)
			field[y][x] = SPACE;
}





void toggle_player(void)
{
	extern Player player;

	player = (player == PLAYER1)? PLAYER2 : PLAYER1;
}





void field_set(void)
{
	extern Player player;
	extern uchar player_win[];
	extern uchar game_count;
	extern Game_mode game_mode;
	extern char field[][X];
	extern char xpos;
	extern char ypos;

	if (field[ypos][xpos] != SPACE)
		return;
	field[ypos][xpos] = player;
	if (player_is_win()) {
		++player_win[player-1];
		++game_count;
		delay(DELAY);
		if ((game_mode == PVSC) && (player == PLAYER2)) {
			paint_field();
			delay(AI_DELAY);
		}
		field_ini();
		paint_ui();
	}
	else if (field_is_full()) {
		++game_count;
		delay(DELAY);
		if ((game_mode == PVSC) && (player == PLAYER2)) {
			paint_field();
			delay(AI_DELAY);
		}
		field_ini();
		paint_ui();
	}
	else {
		toggle_player();
		if ((game_mode == PVSC) && (player == PLAYER2))
			run_ai();
	}
}




void process_error(void)
{
	static error_count = 0;

	++error_count;
	if (error_count == MAX_ERRORS) { 	/* Paint help */
		game_mode = UNDEFINED;
		error_count = 0;
		paint_ui();
	}
}





Bool player_is_win(void)
{
	extern char field[][X];
	extern Player player;
	char count;
	char x;
	char y;

	for (count = x = 0; x < X; ++x)		/* - horizontal */
		if (field[ypos][x] == player)
			++count;
	if (count == X)
		return TRUE;
	for (count = y = 0; y < X; ++y)		/* | vertical */
		if (field[y][xpos] == player)
			++count;
	if (count == X)
		return TRUE;
	if (xpos == ypos) {			/* \ diagonal */
		for (count = x = y = 0; x < X; ++x, ++y)
			if (field[y][x] == player)
				++count;
		if (count == X)
			return TRUE;
	}
	if ((field[2][0] == player) && (field[1][1] == player) &&
	    (field[0][2] == player))		/* / diagonal */
	   return TRUE;
	return FALSE;
}





void run_ai(void)
{
	extern Game_mode game_mode;
	extern char field[][X];
	extern char xpos;
	extern char ypos;
	extern Computer_level comp_lvl;
	uchar ce;

	srand((int)time(NULL));
	ce = (rand()%100 < comp_lvl)? TRUE : FALSE;
	if (TRUE) {	/* Computer is error, xpos and ypos -> random */
		do {
			xpos = rand() % 3;
			ypos = rand() % 3;
		} while(field[ypos][xpos] != SPACE);
	}
	else {		/* Computer is smart... */
		char x;
		char y;
		char _xpos;
		char _ypos;
		char _c;
		char c;        /* position count */

		/* Find position in horizont */
		for (c = y = 0; (c != X) && (y < X); ++y) {
			for (_c = x = 0; x < X; ++x) {
				if (field[y][x] == HUMAN)
					--_c;
				if (field[y][x] == SPACE) {
					++_c;
				       _xpos = x;
				       _ypos = y;
				}
			}
			if (c < abs(_c)) {
				c = _c;
				xpos = _xpos;
				ypos = _ypos;
			}
		}
		/* Find position in vertical*/
		for (x = 0;  (c != X) && (x < X); ++x) {
			for (_c = y = 0; y < X; ++y) {
				if (field[y][x] == HUMAN)
					--_c;
				if (field[y][x] == SPACE) {
					++_c;
				       _xpos = x;
				       _ypos = y;
				}
			}
			if (c < abs(_c)) {
				c = _c;
				xpos = _xpos;
				ypos = _ypos;
			}
		}
	}
	field_set();
	paint_ui();
}





Bool field_is_full(void)
{
	extern char field[][X];
	char x;
	char y;

	for (y = 0; y < X; ++y)
		for (x = 0; x < X; ++x)
			if (field[y][x] == SPACE)
				return FALSE;
	return TRUE;
}