#include <ncurses.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include "mode_file.h"

void scrollScrn(mode_Line* topLine);

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
	cbreak();
	noecho();
	keypad(stdscr, TRUE);

	//Get term cells size
	int max_x = 0;
	int max_y = 0;
	getmaxyx(stdscr, max_y, max_x);

	//Variables for prog to use
	char gottenCh = 0;
	int topLineNum = 0;
	mode_Line* topLine;
	int workingLineNum = 0;
	mode_Line* workingLine;

	//Desired cursos location
	struct cursLoc {
		unsigned char x;
		unsigned char y;
	}cursLoc;
	cursLoc.x = 0;
	cursLoc.y = 0;

	//Get initial first line
	topLine = openFile->firstLine;
	workingLine = topLine;

	//Print initial file contents.
	scrollScrn(topLine);
	move(0, 0);

	while(gottenCh != 'q') {
		switch(gottenCh) {
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
				cursLoc.x = 0;
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
				cursLoc.x = 0;
				break;

			case 't':
				if(workingLine->line[cursLoc.x] == '\n' || cursLoc.x == max_x) {

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
		}

		move(cursLoc.y, cursLoc.x);
		refresh();
		gottenCh = getch();
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
	return;
}

