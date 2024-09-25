#pragma once
#include <stdio.h>

#define LINE_GROWTH 128
#define INITIAL_LINE_SIZE 1024

typedef struct mode_Line {
	int length;
	int capacity;
	struct mode_Line* prevLine;
	struct mode_Line* nextLine;
	char* line;
} mode_Line;

typedef struct mode_File {
	char* path;
	int lines;
	mode_Line* firstLine;
} mode_File;

mode_File* mode_openFile(char* path);
void mode_saveFile(mode_File);
mode_Line* addLine(mode_Line* toAddAfter);
void mode_closeFile(mode_File* toClose);
void mode_removeLine(mode_Line* toRemove);
