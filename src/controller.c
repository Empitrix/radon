#include <raylib.h>
#include <stdint.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "thread.h"
#include <stdlib.h>

#define MAXBUF 1024 * 1024  // 1MB buffer


typedef struct {
	char buffer[MAXBUF];  // Buffer
	Font font;            // Config Font
	uint32_t index;       // Index (Buf pos)
	int tabsize;          // Tab size
	int spacing;          // Letter spacing
	int hscroll;          // Horizontal Scroll Offset
	int vscroll;          // Vertical Scroll Offset
	uint8_t blinky;       // Cursor
} controller_t;



void *blink_cursor(void *arg){
	controller_t *ctrl = (controller_t *)arg;
	while(1){
		ctrl->blinky = ctrl->blinky == 0;
		usleep(500000L);
	}
	return NULL;
}

controller_t *initController(controller_t *ctrl, Font font, int tabsize, int spacing){
	ctrl->font = font;
	ctrl->index = 0;
	ctrl->tabsize = tabsize;
	ctrl->spacing = spacing;
	ctrl->hscroll = 0;
	ctrl->vscroll = 0;
	ctrl->blinky = 1;
	exe_thread(blink_cursor, ctrl);  // Start Blinky
	return ctrl;
}



typedef enum {
	INSERT_MODE,
	REMOVE_MODE,
} insert_mode_t;


void modify_buffer(controller_t *ctrl, char ch, insert_mode_t mode){
	switch(mode){
		case INSERT_MODE:
			ctrl->buffer[ctrl->index++] = ch;
			break;
		case REMOVE_MODE:
			ctrl->buffer[--ctrl->index] = '\0';
			break;
		default: break;
	}
	ctrl->blinky = 1;
}

void remove_word(controller_t *ctrl){
	while (ctrl->index > 0) {
		// Remove trailing spaces or punctuation first
		char c = ctrl->buffer[ctrl->index - 1];
		if (!isalnum(c)) {
			ctrl->buffer[--ctrl->index] = '\0';
		} else {
			break;
		}
	}

	while(ctrl->index > 0) {
		char c = ctrl->buffer[ctrl->index - 1];
		if (isalnum(c)) {
			ctrl->buffer[--ctrl->index] = '\0';
		} else {
			break;
		}
	}
	ctrl->blinky = 1;
}


void updateController(controller_t *ctrl){
		char ch = '\0';
		if((ch = GetCharPressed()) != '\0' && ctrl->index != MAXBUF){
			modify_buffer(ctrl, ch, INSERT_MODE);
		}

		KeyboardKey ckey = GetKeyPressed();

		if (ckey == KEY_BACKSPACE && ctrl->index != 0) {
			if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) {
				remove_word(ctrl);
			} else {
				modify_buffer(ctrl, '\0', REMOVE_MODE);
			}
		}

		if(IsKeyPressedRepeat(KEY_BACKSPACE) && ctrl->index != 0){
			modify_buffer(ctrl, '\0', REMOVE_MODE);
		}

		if(ckey == KEY_ENTER){
			modify_buffer(ctrl, '\n', INSERT_MODE);
		}

		if(ckey == KEY_TAB){
			for(int i = 0; i < ctrl->index; i++){
				modify_buffer(ctrl, ' ', INSERT_MODE);
			}
		}

		if(IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)){
			ctrl->hscroll -= GetMouseWheelMove() * ctrl->font.baseSize;
			if(ctrl->hscroll < 0){
				ctrl->hscroll = 0;
			}
		} else {
			ctrl->vscroll -= GetMouseWheelMove() * ctrl->font.baseSize;
			if(ctrl->vscroll < 0){
				ctrl->vscroll = 0;
			}
		}



		if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_V)) {
			const char *text = GetClipboardText();
			for(int i = 0; i < (int)strlen(text); i++){
				switch(text[i]){
					case '\t':
						for(int i = 0; i < ctrl->tabsize; i++){
							modify_buffer(ctrl, ' ', INSERT_MODE);
						}
						break;
					default:
						modify_buffer(ctrl, text[i], INSERT_MODE);
						break;
				}
			}
		}

}
