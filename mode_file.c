#include "mode_file.h"
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>

mode_File* mode_openFile(char* path) {
	mode_File* ret_F;
	if(!(ret_F = malloc(sizeof(mode_File)))) {
		exit(-99);
	}

	if(!(ret_F->path = calloc(PATH_MAX, sizeof(char)))) {
		exit(-99);
	}
	strncpy(ret_F->path, path, sizeof(char)*PATH_MAX);

	//Don't keep file handles open
	FILE* toOpen = fopen(ret_F->path, "r");

	//The line we're currently working with
	mode_Line* toWrite;

	if(!(toWrite = malloc(sizeof(mode_Line)))) {
		exit(-99);
	}

	toWrite->capacity = INITIAL_LINE_SIZE;
	toWrite->length = 0;
	toWrite->prevLine = NULL;
	toWrite->nextLine = NULL;

	ret_F->firstLine = toWrite;
	if(!(toWrite->line = malloc(sizeof(char)*INITIAL_LINE_SIZE))) {
		exit(-99);
	}

	char charGotten = 0; //Getting next char
	int lineLoc = 0; //Where we are in the line
	ret_F->lines = 1;

	//Copy from disk to memory
	while((charGotten = fgetc(toOpen)) != EOF) {

		//Dynamic Expansion
		if(toWrite->length == toWrite->capacity) {
			//TODO: Don't crash on OOM
			if(!(toWrite->line = realloc(toWrite->line, sizeof(char)*(toWrite->capacity+LINE_GROWTH)))) {
				exit(-99);
			}
			toWrite->capacity += LINE_GROWTH;
		}

		//Handle new lines
		if(charGotten == '\n') {
			toWrite->line[lineLoc] = '\n';
			toWrite = addLine(toWrite);
			ret_F->lines++;
			lineLoc = 0;
		}else {
			toWrite->line[lineLoc++] = charGotten;
		}
	}

	fclose(toOpen);

	return ret_F;
}

void mode_saveFile(mode_File) {

}

mode_Line* addLine(mode_Line* toAddAfter) {
	if(!(toAddAfter->nextLine = malloc(sizeof(mode_Line)))) {
		exit(-99);
	}

	toAddAfter->nextLine->length = 0;
	toAddAfter->nextLine->capacity = INITIAL_LINE_SIZE;
	toAddAfter->nextLine->prevLine = toAddAfter;
	toAddAfter->nextLine->nextLine = NULL;

	if(!(toAddAfter->nextLine->line = malloc(sizeof(char)*INITIAL_LINE_SIZE))) {
		exit(-99);
	}

	return toAddAfter->nextLine;
}

void mode_closeFile(mode_File* toClose) {

	mode_Line* lineToClose = toClose->firstLine;

	//Get last line struct
	while(lineToClose->nextLine != NULL) {
		lineToClose = lineToClose->nextLine;
	}

	//Free char* then line struct
	while(lineToClose->prevLine != NULL) {
		free(lineToClose->line);
		lineToClose = lineToClose->prevLine;
		free(lineToClose->nextLine);
	}

	//Free beginning line struct, then file obj
	free(toClose->firstLine->line);
	free(toClose->firstLine);
	free(toClose->path);
	free(toClose);
	toClose = NULL;
	return;
}

void mode_removeLine(mode_Line* toRemove) {
	toRemove->prevLine->nextLine = toRemove->nextLine;
	free(toRemove->line);
	free(toRemove);
	return;
}
