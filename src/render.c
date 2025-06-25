#include <raylib.h>
#include "controller.h"
#include "config.h"



void render_cursor(controller_t *ctrl, int padding);



/*
void render_buffer(controller_t *ctrl){
	const char *start = ctrl->buffer;
	int letter_factor = ctrl->fontsize + ctrl->spacing;  // Compile-Time Letter Size (SIZE FACTOR)
	int margin = 0;

	controllerUpdateBuffer(ctrl);

	int numLineSize = 0;

	for(int i = 0; i < ctrl->lines.cln; i++){
		DrawTextEx(ctrl->font, ctrl->lines.lines[i], V2(-ctrl->hscroll + numLineSize, (i * letter_factor)-ctrl->vscroll), ctrl->font.baseSize, ctrl->spacing, BLACK);
	}

	render_cursor(ctrl, numLineSize);
}
*/



void render_buffer(controller_t *ctrl){
	const char *start = ctrl->buffer;
	int letter_factor = ctrl->fontsize + ctrl->spacing;  // Compile-Time Letter Size (SIZE FACTOR)
	int margin = 0;

	controllerUpdateBuffer(ctrl);

	int numLineSize = 0;



	// // Render selected background
	// if(ctrl->cursor.selection.start != ctrl->cursor.selection.end && ctrl->cursor.mode == CURSOR_SELECT){
	// 	int nl = 0;  // Number of processed letters
	// 	for(int i = 0; i < ctrl->lines.cln; i++){

	// 		int x = -ctrl->hscroll + numLineSize;
	// 		int y = (i * letter_factor)-ctrl->vscroll;
	// 		int len = (int)strlen(ctrl->lines.lines[i]);

	// 		for(int j = 0; j < len; j++){
	// 			int width = letter_factor / 2;
	// 			int lp = (j * letter_factor) / 2; // left-padding
	// 			if((nl + j) >= ctrl->cursor.selection.start && (nl + j) <= ctrl->cursor.selection.end){
	// 				DrawRectangle(lp + x, y, width, ctrl->fontsize, YELLOW);
	// 			}
	// 		}

	// 		nl += (len + 1);  // +1 for the line (\n)
	// 	}
	// }


	if(ctrl->cursor.selection.start != ctrl->cursor.selection.end && ctrl->cursor.mode == CURSOR_SELECT){
		int nl = 0;  // Number of processed letters
		for(int i = 0; i < ctrl->lines.cln; i++){

			int x = -ctrl->hscroll + numLineSize;
			int y = (i * letter_factor)-ctrl->vscroll;
			int len = (int)strlen(ctrl->lines.lines[i]);

			for(int j = 0; j < len; j++){
				int width = letter_factor / 2;
				int lp = (j * letter_factor) / 2; // left-padding
				if((nl + j) >= ctrl->cursor.selection.start && (nl + j) <= ctrl->cursor.selection.end - 1){
					DrawRectangle(lp + x, y, width, ctrl->fontsize, YELLOW);
				}
			}

			nl += (len + 1);  // +1 for the line (\n)
		}
	}

	// Render lines
	for(int i = 0; i < ctrl->lines.cln; i++){
		int x = -ctrl->hscroll + numLineSize;
		int y = (i * letter_factor)-ctrl->vscroll;
		DrawTextEx(ctrl->font, ctrl->lines.lines[i], V2(x, y), ctrl->font.baseSize, ctrl->spacing, BLACK);
	}


	render_cursor(ctrl, numLineSize);
}




void render_cursor(controller_t *ctrl, int padding){
	updateCursorCurrentPos(ctrl);

	const char *buffer = ctrl->buffer;
	int width = 0, height = 0, spacing = 2, cursor_width = 3;

	int factor = ctrl->fontsize + ctrl->spacing;

	width = ctrl->cursor.current.x;
	height = ctrl->cursor.current.y;
	if(ctrl->blinky){
		DrawRectangle(((width * factor) / 2) + spacing, height * factor, cursor_width, ctrl->fontsize, BLUE);
	}

}

