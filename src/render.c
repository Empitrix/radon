#include <raylib.h>
#include "controller.h"
#include "config.h"



void render_cursor(controller_t *ctrl, int padding);



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




void render_cursor(controller_t *ctrl, int padding){


	updateCursorCurrentPos(ctrl);

	const char *buffer = ctrl->buffer;
	int width = 0, height = 0, spacing = 2, cursor_width = 3;


	int factor = ctrl->fontsize + ctrl->spacing;

	width = ctrl->cursor.current.x;
	height = ctrl->cursor.current.y;

	// while(*buffer){
	// 	if(*buffer == '\n'){ width = 0, height++; } else { width++; }
	// 	buffer++;
	// 	if(0){ break; }
	// }

	if(ctrl->blinky){
		DrawRectangle(((width * factor) / 2) + spacing, height * factor, cursor_width, ctrl->fontsize, BLUE);
	}

}

