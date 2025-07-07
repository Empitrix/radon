#include <raylib.h>
#include <stdint.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "thread.h"
#include "utils.h"
#include "shaders.h"
#include "config.h"
#include "types.h"
#include <stdlib.h>





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
	if(!spacing){ spacing = 1; }
	strcpy(ctrl->font.font_path, font_path);
	strcpy(ctrl->font.fs_file, "assets/glsl330/sdf.fs");
	ctrl->font = fontLoadSDF(font_path, ctrl->font.fs_file, fontsize, spacing);
	ctrl->index = 0;                   // Current position
	ctrl->tabsize = tabsize;           // Tab Size
	ctrl->hscroll = 0;                 // Horizontal Scroll Position
	ctrl->vscroll = 0;                 // Vertical Scroll Position
	ctrl->blinky = 1;                  // Cursor Blink
	ctrl->csize = fontsize + spacing;  // Character Size
	exe_thread(blink_cursor, ctrl);    // Start Blinky


	ctrl->cursor = cursorDefaultInit();

	// Letter factor (each size (monospace))
	int fac = ctrl->font.fontsize * ctrl->font.spacing;
	ctrl->wFactor = fac / 2;
	ctrl->hFactor = ctrl->font.fontsize;

	return ctrl;
}



/* getMaxLines: maximum lines in controller_t's buffer */
int getMaxLines(controller_t *ctrl){
	int i = 0, l = 0;
	while(ctrl->buffer[i] != '\0'){
		if(ctrl->buffer[i] == '\n'){ l++; }
		i++;
	}
	if(i != 0){ l++; }
	return l;
}


/* getLineLength: returns the length of the specified line num from the ctrl's buffer */
int getLineLength(controller_t *ctrl, int num){
	int i = 0, l = 0;
	int len = 0;
	
	if(num > getMaxLines(ctrl) - 1){ return 0; }

	while(ctrl->buffer[i] != '\0'){

		if(l == num){
			int start = 0;
			while(ctrl->buffer[start + i] != '\n' && ctrl->buffer[start + i] != '\0'){
				len++;
				start++;
			}
			break;
		}
		if(ctrl->buffer[i] == '\n'){ l++; }
		i++;
	}
	return len;
}





void controllerSetWindowSize(controller_t *ctrl, int width, int height){
	ctrl->windowHeight = height;
	ctrl->windowWidth = width;
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

	if(ctrl->cursor.current.x < 0){
		ctrl->cursor.current.x = getLineLength(ctrl, ctrl->cursor.current.y - 1);
		ctrl->cursor.current.y--;
	}

}


void getCurrentLineData(controller_t *ctrl, int *line, int *c){
	int cl = 0, col = 0;  // Current Line, Column
	for(int i = 0; i < ctrl->index; i++){
		if(ctrl->buffer[i] == '\n'){
			cl++;
			col = 0;
		} else {
			col++;
		}
	}
	if(line != NULL){ *line = cl; }
	if(c != NULL){ *c = col; }
}


int getUpCursorDiff(controller_t *ctrl){
	int lineN = 0, charN = 0;
	getCurrentLineData(ctrl, &lineN, &charN);
	if(lineN != 0){
		int pLen = getLineLength(ctrl, lineN - 1);
		int afterPLine = (pLen - charN);
		if(afterPLine < 0){
			return  charN + 1;
		}
		return  afterPLine + charN + 1;
	}
	return 0;
}


int getDownCursorDiff(controller_t *ctrl){
	int lineN = 0, charN = 0;
	getCurrentLineData(ctrl, &lineN, &charN);
	int current = getMaxLines(ctrl);
	if(current != 0){ current--; }
	if(lineN != current && current != 0){
		int cLineEnd = getLineLength(ctrl, lineN) - charN + 1;
		int aLineLen = getLineLength(ctrl, lineN + 1);
		if((aLineLen - charN) < 0){
			return cLineEnd + aLineLen;
		}
		return cLineEnd + charN;
	}
	return 0;
}


void modify_buffer(controller_t *ctrl, char ch, insert_mode_t mode){
	switch(mode){
		case INSERT_MODE:
			insertCharAt(ctrl->buffer, ctrl->index, ch);
			ctrl->index++;
			break;
		case REMOVE_MODE:
			if(ctrl->cursor.mode == CURSOR_SELECT){
				removeCharRange(ctrl->buffer, ctrl->cursor.selection.start, ctrl->cursor.selection.end);
				ctrl->index = ctrl->cursor.selection.start;
				ctrl->cursor.mode = CURSOR_WRITE;
				// Set the cursor at [0, 0] if there is nothing
				if((int)strlen(ctrl->buffer) == 0){ ctrl->hscroll = 0; ctrl->vscroll = 0; }
			} else {
				if(ctrl->index > 0){
					removeCharAt(ctrl->buffer, --ctrl->index);
				}
			}
			break;
		default: break;
	}
	ctrl->cursor.mode = CURSOR_WRITE;
	ctrl->cursor.selection = (range_t){ctrl->index, ctrl->index};
	ctrl->blinky = 1;
}


void setCursorMouseClick(controller_t *ctrl){
	int x = GetMouseX();
	int y = GetMouseY();
	if(x <= ctrl->windowWidth && y <= ctrl->windowHeight){
		int xPx = 0, yPx = 0;
		int i = 0;
		while(ctrl->buffer[i] != '\0'){

			if(ctrl->buffer[i] == '\n'){
				yPx += ctrl->hFactor;
				xPx = 0;
			} else {
				xPx += ctrl->wFactor;
			}

			int aX = xPx + ctrl->wFactor;
			int aY = yPx + ctrl->hFactor;

			if((x <= aX && x >= xPx) && (y <= aY && y >= yPx)){
				ctrl->index = i - 1;
				break;
			}

			i++;
		}
	}
}


int getMaxCurrentIndex(controller_t *ctrl){
	int i  = ctrl->index;
	while(ctrl->buffer[i] != '\n' && i <= (int)strlen(ctrl->buffer)){
		i++;
	}
	return i;
}


int getPreviusWordPos(controller_t *ctrl){
	if (ctrl->index <= 0 || ctrl->index > strlen(ctrl->buffer)){
		return ctrl->index;
	}

	// Step back from pos - 1 while skipping spaces
	int i = ctrl->index - 1;
	while (i >= 0 && isspace((unsigned char)ctrl->buffer[i])) {
		i--;
	}

	// Step back over the word
	while (i >= 0 && !isspace((unsigned char)ctrl->buffer[i])) {
		i--;
	}

	return i + 1;
}


int getNextWordPos(controller_t *ctrl){
	return ctrl->index;
}



void remove_word(controller_t *ctrl){
	int pPos = getPreviusWordPos(ctrl);

	int wordStart = pPos;
	int wordEnd = ctrl->index;

	// Shift the rest of the string to the left
	for (int j = wordEnd; ; j++, wordStart++) {
		ctrl->buffer[wordStart] = ctrl->buffer[j];
		if (ctrl->buffer[j] == '\0') break;
	}

	// Update cursor (index)
	ctrl->index = pPos;
	ctrl->blinky = 1;
}


void updateController(controller_t *ctrl){
	char ch = '\0';
	if((ch = GetCharPressed()) != '\0' && ctrl->index != MAXBUF){
		modify_buffer(ctrl, ch, INSERT_MODE);
	}

	KeyboardKey ckey = GetKeyPressed();

	if(IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)){
		if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) {
			remove_word(ctrl);
		} else {
			modify_buffer(ctrl, '\0', REMOVE_MODE);
		}
	}


	if(ckey == KEY_ENTER){
		modify_buffer(ctrl, '\n', INSERT_MODE);
	}

	if(ckey == KEY_TAB){
		for(int i = 0; i < ctrl->tabsize; i++){
			modify_buffer(ctrl, ' ', INSERT_MODE);
		}
	}



	if(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_CONTROL)){
		ctrl->hscroll -= GetMouseWheelMove() * ctrl->font.font.baseSize;
		if(ctrl->hscroll < 0){
			ctrl->hscroll = 0;
		}
	} else {
		ctrl->vscroll -= GetMouseWheelMove() * ctrl->font.font.baseSize;
		if(ctrl->vscroll < 0){
			ctrl->vscroll = 0;
		}
	}


	if((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_PAGE_UP)){
		if(IsFontValid(ctrl->font.font)){
			if(ctrl->font.fontsize < 255){
				UnloadFont(ctrl->font.font);
				ctrl->font = fontLoadSDF(ctrl->font.font_path, ctrl->font.fs_file, ++ctrl->font.fontsize, ctrl->font.spacing);
				int fac = ctrl->font.fontsize * ctrl->font.spacing;
				ctrl->wFactor = fac / 2;
				ctrl->hFactor = ctrl->font.fontsize;
			}
		}
	}

	if((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_PAGE_DOWN)){
		if(IsFontValid(ctrl->font.font)){
			if(ctrl->font.fontsize > 1){
				UnloadFont(ctrl->font.font);
				ctrl->font = fontLoadSDF(ctrl->font.font_path, ctrl->font.fs_file, --ctrl->font.fontsize, ctrl->font.spacing);
				int fac = ctrl->font.fontsize * ctrl->font.spacing;
				ctrl->wFactor = fac / 2;
				ctrl->hFactor = ctrl->font.fontsize;
			}
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


	// Cursor Selection Right
	if(IsKeyPressed(KEY_RIGHT) && IsKeyDown(KEY_LEFT_SHIFT)){
		if(ctrl->cursor.mode == CURSOR_WRITE){
			ctrl->cursor.mode = CURSOR_SELECT;
		} else {
			ctrl->cursor.selection.end = ctrl->index;
		}
	}

	// Cursor Selection Left
	if(IsKeyPressed(KEY_LEFT) && IsKeyDown(KEY_LEFT_SHIFT)){
		if(ctrl->cursor.mode == CURSOR_WRITE){
			ctrl->cursor.selection.start = ctrl->index - 1;
			ctrl->cursor.selection.end = ctrl->index   - 1;
			ctrl->cursor.mode = CURSOR_SELECT;
		} else {
			ctrl->cursor.selection.start = ctrl->index - 1;
		}
	}

	// Cursor Selection to UP
	if(IsKeyPressed(KEY_UP) && IsKeyDown(KEY_LEFT_SHIFT)){
		int diff = getUpCursorDiff(ctrl);
		ctrl->cursor.selection.start = ctrl->index - diff;
		ctrl->cursor.selection.end = ctrl->index - 1;
		ctrl->cursor.mode = CURSOR_SELECT;
		ctrl->blinky = 1;
	}

	// Cursor Selection to DOWN
	if(IsKeyPressed(KEY_DOWN) && IsKeyDown(KEY_LEFT_SHIFT)){
		int diff = getDownCursorDiff(ctrl);
		ctrl->cursor.selection.start = ctrl->index;
		ctrl->cursor.selection.end = ctrl->index + diff - 1;
		ctrl->cursor.mode = CURSOR_SELECT;
		ctrl->blinky = 1;
	}

	if(IsKeyPressed(KEY_A) && IsKeyDown(KEY_LEFT_CONTROL)){
		ctrl->cursor.selection.start = 0;
		ctrl->cursor.selection.end = strlen(ctrl->buffer) - 1;
		ctrl->cursor.mode = CURSOR_SELECT;
		ctrl->index = (int)strlen(ctrl->buffer);
		ctrl->blinky = 1;
	}



	// Cursor movement
	if(IsKeyPressed(KEY_RIGHT) || IsKeyPressedRepeat(KEY_RIGHT)){
		if(ctrl->index < (int)strlen(ctrl->buffer)){
			ctrl->index++;
		}
		ctrl->blinky = 1;
	}

	if(IsKeyPressed(KEY_LEFT) || IsKeyPressedRepeat(KEY_LEFT)){
		if(ctrl->index){
			ctrl->index--;
		}
		ctrl->blinky = 1;
	}

	if(IsKeyPressed(KEY_UP) || IsKeyPressedRepeat(KEY_UP)){
		ctrl->index -= getUpCursorDiff(ctrl);
		ctrl->blinky = 1;
	}

	if(IsKeyPressed(KEY_DOWN) || IsKeyPressedRepeat(KEY_DOWN)){
		ctrl->index += getDownCursorDiff(ctrl);
		ctrl->blinky = 1;
	}


	// Exit selection
	if((IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_UP)) && !IsKeyDown(KEY_LEFT_SHIFT)){
		ctrl->cursor.selection.start = ctrl->index;
		ctrl->cursor.selection.end = ctrl->index;
		ctrl->cursor.mode = CURSOR_WRITE;
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




	// Set cursor pos by Mouse click
	if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
		setCursorMouseClick(ctrl);
	}

}



