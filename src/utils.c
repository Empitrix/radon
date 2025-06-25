#include "config.h"
#include <stdio.h>
#include <string.h>


int last_line_length(const char *buffer) {
	const char *last_newline = strrchr(buffer, '\n');
	if (last_newline) {
		return (int)strlen(last_newline + 1); // Length after the last '\n'
	} else {
		return (int)strlen(buffer); // No newline found; whole buffer is one line
	}
}


const char* get_last_line(const char *buffer) {
	const char *last_newline = strrchr(buffer, '\n');
	if (last_newline) {
		return last_newline + 1; // Pointer to start of last line
	} else {
		return buffer; // No newline found; whole buffer is one line
	}
}




int getIntSize(int n){
	static char buffer[124] = { 0 };
	sprintf(buffer, "%d", n);
	return (int)strlen(buffer);
}



/* insertBuffAt: Inserts the 'txt' into the 'buff' at the given position */
/* CAUTION: this function needs to be implemented more efficiently and this is an aweful example */
void insertBuffAt(char *buff, int pos, const char *txt){
	static char beforeBuff[MAXBUF] = { 0 };
	static char afterBuff[MAXBUF] = { 0 };
	int i = 0;
	while(i <= pos){
		beforeBuff[i] = buff[i];
		i++;
	}
	beforeBuff[i] = '\0';
	i = pos + 1;
	int j = 0;
	while(i < (int)strlen(buff)){
		afterBuff[j++] = buff[i];
		i++;
	}
	afterBuff[j] = '\0';
	sprintf(buff, "%s%s%s", beforeBuff, txt, afterBuff);
}


void insertCharAt(char *buff, int pos, const char txt){
	static char beforeBuff[MAXBUF] = { 0 };
	static char afterBuff[MAXBUF] = { 0 };
	int i = 0;
	while(i <= pos){
		beforeBuff[i] = buff[i];
		i++;
	}
	beforeBuff[i] = '\0';
	i = pos + 1;
	int j = 0;
	while(i < (int)strlen(buff)){
		afterBuff[j++] = buff[i];
		i++;
	}
	afterBuff[j] = '\0';
	sprintf(buff, "%s%c%s", beforeBuff, txt, afterBuff);
}


void removeCharAt(char *buff, int pos){
	for(int i = pos; i < (int)strlen(buff); i++){
		buff[i] = buff[i + 1];
	}
}




void getNextPos(const char *buff, const char *txt){
}

void getLastPos(const char *buff, const char *txt){
}


