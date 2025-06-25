#include <raylib.h>
#include <stdint.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "thread.h"
#include "utils.h"
#include <stdlib.h>

#define MAXBUF 1024 * 1024  // 1MB buffer
#define MAXLINES 1024



typedef struct {
	char lines[MAXLINES][MAXBUF];      // 1GB holding buffer (TEMP SPACE)
	char hold[MAXBUF];                 // 1MB holding line   (TEMP SPACE)
	unsigned long int holdIdx;         // Hold index
	int cln;                           // Current Line Number
} controllerBuffer;


typedef enum {
	CURSOR_WRITE,
	CURSOR_SELECT,
} cursor_mode;


typedef struct {
	int x;
	int y;
} pos_t;


typedef struct {
	int start;
	int end;
} range_t;



typedef struct {
	cursor_mode mode;
	range_t selection;         // Range of the selected text [start, end]
	pos_t current;
	pos_t extend;
	int max;
} cursor_t;




typedef struct {
	char buffer[MAXBUF];       // Buffer
	char font_path[128];       // Font path buffer
	Font font;                 // Config Font
	uint32_t index;            // Index (Buf pos)
	int tabsize;               // Tab size
	int spacing;               // Letter spacing
	int hscroll;               // Horizontal Scroll Offset
	int vscroll;               // Vertical Scroll Offset
	uint8_t blinky;            // Cursor
	uint8_t fontsize;          // Font Size
	int csize;                 // Char size
	// int curosr;                // Cursor pos
	int windowWidth;           // Window Width
	int windowHeight;          // Window Height
	controllerBuffer lines;    // Splited lines
	cursor_t cursor;
} controller_t;



void *blink_cursor(void *arg){
	controller_t *ctrl = (controller_t *)arg;
	while(1){
		ctrl->blinky = ctrl->blinky == 0;
		usleep(500000L);
	}
	return NULL;
}


cursor_t cursorDefaultInit(void){
	return (cursor_t){
		.mode = CURSOR_WRITE,
		.current = (pos_t){0, 0},
		.extend = (pos_t){0, 0},
		.max = 0,
		// .selection = (range_t){2, 12}
		.selection = (range_t){0, 0}
	};
}


controller_t *controllerInit(controller_t *ctrl, const char* font_path, int fontsize, int tabsize, int spacing){
	strcpy(ctrl->font_path, font_path);
	ctrl->font = LoadFontEx(font_path, fontsize, NULL, 540);
	ctrl->index = 0;                   // Current position
	ctrl->tabsize = tabsize;           // Tab Size
	ctrl->spacing = spacing;           // Letter Spacing
	ctrl->hscroll = 0;                 // Horizontal Scroll Position
	ctrl->vscroll = 0;                 // Vertical Scroll Position
	ctrl->blinky = 1;                  // Cursor Blink
	ctrl->fontsize = fontsize;         // Font Size
	ctrl->csize = fontsize + spacing;  // Character Size
	exe_thread(blink_cursor, ctrl);    // Start Blinky

	// Init controller Buffer
	memset(ctrl->lines.lines, 0, sizeof(ctrl->lines.lines));
	memset(ctrl->lines.hold, 0, sizeof(ctrl->lines.hold));
	ctrl->lines.cln = 0;
	ctrl->lines.holdIdx = 0;

	ctrl->cursor = cursorDefaultInit();

	return ctrl;
}


void controllerSetWindowSize(controller_t *ctrl, int width, int height){
	ctrl->windowHeight = height;
	ctrl->windowWidth = width * 2;
}


void updateCursorCurrentPos(controller_t *ctrl){
	int height = 0, width = 0;
	int i = 0;
	while(i <= ctrl->index){
		if(ctrl->buffer[i] == '\n'){
			height++;
			width = 0;
		} else {
			width++;
		}
		i++;
	}
	ctrl->cursor.current.x = width - 1;
	ctrl->cursor.current.y = height;

	// ctrl->cursor.selection = (range_t){ ctrl->cursor.current.x,  ctrl->cursor.current.x };
}



void controllerUpdateBuffer(controller_t *ctrl){
	// lines[cln][...........]
	//           hold[holdIdx]
	const char *start = ctrl->buffer;
	memset(ctrl->lines.hold, '\0', sizeof(ctrl->lines.hold));
	ctrl->lines.cln = 0;
	ctrl->lines.holdIdx = 0;
	while(*start){
		if(*start == '\n'){
			strcpy(ctrl->lines.lines[ctrl->lines.cln++], ctrl->lines.hold);
			memset(ctrl->lines.hold, '\0', sizeof(ctrl->lines.hold));
			ctrl->lines.holdIdx = 0;
		} else {
			ctrl->lines.hold[ctrl->lines.holdIdx++] = *start;
		}
		start++;
	}
	strcpy(ctrl->lines.lines[ctrl->lines.cln++], ctrl->lines.hold);



	// Update curosr.max
	ctrl->cursor.max = (int)strlen(ctrl->lines.lines[ctrl->lines.cln - 1]);

}



typedef enum {
	INSERT_MODE,
	REMOVE_MODE,
} insert_mode_t;


void modify_buffer(controller_t *ctrl, char ch, insert_mode_t mode){
	switch(mode){
		case INSERT_MODE:
			insertCharAt(ctrl->buffer, (ctrl->index++ - 1), ch);
			break;
		case REMOVE_MODE:
			removeCharAt(ctrl->buffer, --ctrl->index);
			break;
		default: break;
	}
	ctrl->cursor.mode = CURSOR_WRITE;
	ctrl->cursor.selection = (range_t){ctrl->index, ctrl->index};
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
		if (IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_CONTROL)) {
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
		// printf("%d\n", ctrl->vscroll);
	}

	if(ckey == KEY_TAB){
		for(int i = 0; i < ctrl->tabsize; i++){
			modify_buffer(ctrl, ' ', INSERT_MODE);
		}
	}

	if(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_CONTROL)){
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


	if((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_PAGE_UP)){
		if(IsFontValid(ctrl->font)){
			if(ctrl->fontsize < 255){
				UnloadFont(ctrl->font);
				ctrl->font = LoadFontEx(ctrl->font_path, ++ctrl->fontsize, NULL, 540);
			}
		}
		// SetWindowTitle(TextFormat("Rodon (%d)", ctrl->fontsize));
	}

	if((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_PAGE_DOWN)){
		if(IsFontValid(ctrl->font)){
			if(ctrl->fontsize > 1){
				UnloadFont(ctrl->font);
				ctrl->font = LoadFontEx(ctrl->font_path, --ctrl->fontsize, NULL, 540);
			}
		}
		// SetWindowTitle(TextFormat("Rodon (%d)", ctrl->fontsize));
	}

	if ((IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_V)) {
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


	if(IsKeyPressed(KEY_RIGHT)){
		if(ctrl->index < (int)strlen(ctrl->buffer)){
			ctrl->index++;
		}
		ctrl->blinky = 1;
	}


	if(IsKeyPressed(KEY_LEFT)){
		if(ctrl->index){
			ctrl->index--;
		}
		ctrl->blinky = 1;
	}

	if(IsKeyPressed(KEY_RIGHT) && IsKeyDown(KEY_LEFT_SHIFT)){
		if(ctrl->cursor.mode == CURSOR_WRITE){
			ctrl->cursor.selection.start = ctrl->index - 1;
			ctrl->cursor.selection.end = ctrl->index;
			ctrl->cursor.mode = CURSOR_SELECT;
		} else {
			ctrl->cursor.selection.end = ctrl->index;
		}
	}

	if(IsKeyPressed(KEY_LEFT) && IsKeyDown(KEY_LEFT_SHIFT)){
		if(ctrl->cursor.mode == CURSOR_WRITE){
			ctrl->cursor.selection.end = ctrl->index + 1;
			ctrl->cursor.selection.start = ctrl->index;
			ctrl->cursor.mode = CURSOR_SELECT;
		} else {
			ctrl->cursor.selection.start = ctrl->index;
		}
	}



	if((IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT)) && !IsKeyDown(KEY_LEFT_SHIFT)){
		ctrl->cursor.selection.start = ctrl->index;
		ctrl->cursor.selection.end = ctrl->index;
	}


	// Set the cursor at the end of the line
	if(IsKeyPressed(KEY_END)){
		if(ctrl->index != strlen(ctrl->buffer) - 1){ ctrl->index++; }
		while(ctrl->buffer[ctrl->index] != '\n' && ctrl->buffer[ctrl->index] != '\0' && ctrl->index < (int)strlen(ctrl->buffer)){
			ctrl->index++;
		}
	}

	// Set the cursor at the begining of the line
	if(IsKeyPressed(KEY_HOME)){
		if(strlen(ctrl->buffer) != 0){ ctrl->index--; }
		while(ctrl->buffer[ctrl->index] != '\n' && ctrl->buffer[ctrl->index] != '\0' && ctrl->index > 0){
			ctrl->index--;
		}
		if(ctrl->index > 0){ ctrl->index++; }
	}

}



