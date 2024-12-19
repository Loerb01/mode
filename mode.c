#include <ncurses.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mode_file.h"

struct cursLoc {
	unsigned char x;
	unsigned char y;
};

void scrollScrn(mode_Line* topLine);
void printStatus(const char* modeName, struct cursLoc cursLoc);

int main(int argc, char** argv) {
	//Verify argument~s~
	if(argc == 1) {
		fprintf(stderr, "Input file required!\n");
		exit(-1);
	}else if(argc >2) {
		fprintf(stderr, "Too many arguments!\n");
		exit(-1);
	}

	//open given file
	mode_File* openFile = mode_openFile(argv[1]);

	//initialize ncurses
	setlocale(LC_ALL, "");
	initscr();
	start_color();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	init_pair(1, 0, 2);

	//Get term cells size
	int max_x = 0;
	int max_y = 0;
	getmaxyx(stdscr, max_y, max_x);

	//Variables for prog to use
	bool run = true;
	char gottenCh = 0;
	int topLineNum = 0;
	mode_Line* topLine;
	int workingLineNum = 0;
	mode_Line* workingLine;

	//Desired cursos location
	struct cursLoc cursLoc;
	cursLoc.x = 0;
	cursLoc.y = 0;

	//Get initial first line
	topLine = openFile->firstLine;
	workingLine = topLine;

	//Print initial file contents.
	scrollScrn(topLine);
	move(0, 0);
	printStatus("Mode", cursLoc);
	move(0, 0);

	while(run) {
		gottenCh = getch();
		switch(gottenCh) {
			case 'q':
				run = false;
				break;
			case 'n':
				if(max_y - cursLoc.y < 6 && topLine->nextLine != NULL) {
					topLine = topLine->nextLine;
					topLineNum++;
					scrollScrn(topLine);
				}else {
					cursLoc.y++;
				}
				if(workingLine->nextLine != NULL) {
					workingLine = workingLine->nextLine;
					workingLineNum++;
				}
				break;

			case 'e':
				if(cursLoc.y < 6 && topLine->prevLine != NULL) {
					topLine = topLine->prevLine;
					topLineNum--;
					scrollScrn(topLine);
				}else if(cursLoc.y > 0){
					cursLoc.y--;
				}
				if(workingLine->prevLine != NULL) {
					workingLine = workingLine->prevLine;
					workingLineNum--;
				}
				break;

			case 't':
				if(workingLine->line[cursLoc.x] == '\n' || cursLoc.x == max_x || cursLoc.x >= strlen(workingLine->line)-1) {
					cursLoc.x = strlen(workingLine->line)-1;
				}else {
					cursLoc.x++;
				}
				break;

			case 's':
				if(cursLoc.x <= 0) {
					cursLoc.x = 0;
				}else {
					cursLoc.x--;
				}
				break;

			default:
				break;
		}
	
		printStatus("Mode", cursLoc);

		//Don't touch below this point, ensures accuracy of cursor location
		move(cursLoc.y, cursLoc.x);
		refresh();
	}

	mode_closeFile(openFile);
	endwin();

	return 0;
}

void scrollScrn(mode_Line* topLine) { //Redefine for arb window, and macroize?
	clear();

	//Get term cells size
	int max_x = 0;
	int max_y = 0;
	getmaxyx(stdscr, max_y, max_x);

	mode_Line* workingLine = topLine;
	for(int i = 0; (i < max_y) && (workingLine != NULL); i++) {
		addnstr(workingLine->line, max_x);
		workingLine = workingLine->nextLine;
	}
	move(0, 0);
}

void printStatus(const char* modeName, struct cursLoc cursLoc) {
	int max_x = 0;
	int max_y = 0;
	getmaxyx(stdscr, max_y, max_x);

	char* toPrint = malloc(max_x);
	if(!toPrint) {
		exit(99);
	}

	int used = snprintf(toPrint, max_x, "%s (%d,%d):", modeName, cursLoc.x, cursLoc.y);
	attr_set(A_NORMAL, 1, NULL);
	mvaddnstr(max_y-1, 0, toPrint, max_x);
	for(int i = used; i < max_x; i++) {
		addch(' ');
	}
	attr_set(A_NORMAL, 0, NULL);
}

