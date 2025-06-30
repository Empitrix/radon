#include <raylib.h>
#include <stdio.h>
#include "controller.h"
#include "config.h"



void render_cursor(controller_t *ctrl, int padding);


static int fontLoaded = 0;



void render_buffer(controller_t *ctrl){

	DrawRectangle(0, 0, ctrl->windowWidth, ctrl->windowHeight, (Color){26, 27, 38, 255});


	// DrawRectangle(0, 0, ctrl->windowWidth, ctrl->windowHeight, RED);  // Border

	const char *start = ctrl->buffer;
	int margin = 0;

	controllerUpdateBuffer(ctrl);


	int l = getIntLen(ctrl->lines.cln);

	int numLineSize = l * ctrl->wFactor;

	int cln = 0;

	getCurrentLineData(ctrl, &cln, NULL);


	BeginShaderMode(ctrl->font.shader);

	// Render Numbers
	for(int i = 0; i < ctrl->windowHeight; i += ctrl->hFactor){
			
		int lNum = i / ctrl->hFactor;  // line number
		int selected = cln == (lNum - 1);

		// DrawRectangle(0, i - ctrl->hFactor, numLineSize, ctrl->hFactor, selected ? BLACK : GRAY);
		if(lNum < ctrl->lines.cln + 1){
			// DrawTextEx(ctrl->font.font, TextFormat("%*d", l, lNum), V2(0, i - ctrl->hFactor), ctrl->font.fontsize - 2, ctrl->font.spacing, selected ? YELLOW : GRAY);
			DrawTextEx(ctrl->font.font, TextFormat("%*d", l, lNum), V2(2, (i - ctrl->hFactor + 2) -ctrl->vscroll), ctrl->font.fontsize - 2, ctrl->font.spacing, selected ? YELLOW : GRAY);
		}
	}

	numLineSize += 12;

	if(ctrl->cursor.mode == CURSOR_SELECT){
		int nl = 0;  // Number of processed letters
		for(int i = 0; i < ctrl->lines.cln; i++){

			int x = -ctrl->hscroll + numLineSize;
			int y = (i * ctrl->hFactor)-ctrl->vscroll;

			int len = (int)strlen(ctrl->lines.lines[i]);


			for(int j = 0; j < len; j++){
				int lp = j * ctrl->wFactor; // left-padding
				if((nl + j) >= ctrl->cursor.selection.start && (nl + j) <= ctrl->cursor.selection.end){
					// (Color){ 16, 128, 188, 255 };  // Selection color in chromium
					DrawRectangle(lp + x, y, ctrl->wFactor, ctrl->font.fontsize, YELLOW);
				}
			}

			nl += (len + 1);  // +1 for the line (\n)
		}
	}


	render_cursor(ctrl, numLineSize);


	// Render lines
	int nl = 0;  // Number of processed letters
	for(int i = 0; i < ctrl->lines.cln; i++){
		int x = -ctrl->hscroll + numLineSize;
		int y = (i * ctrl->hFactor)-ctrl->vscroll;
		// DrawTextEx(ctrl->font, ctrl->lines.lines[i], V2(x, y), ctrl->font.baseSize, ctrl->spacing, WHITE);

		int len = strlen(ctrl->lines.lines[i]);
		for(int j = 0; j < len; j++){
			int selected = ((nl + j) >= ctrl->cursor.selection.start && (nl + j) <= ctrl->cursor.selection.end) && ctrl->cursor.mode == CURSOR_SELECT;

			DrawTextCodepoint(ctrl->font.font, ctrl->lines.lines[i][j], V2(x + (j * ctrl->wFactor), y), ctrl->font.fontsize, selected ? BLACK : WHITE);
		}
		nl += (len + 1);  // +1 for the line (\n)
	}


	EndShaderMode();
}




void render_cursor(controller_t *ctrl, int padding){
	updateCursorCurrentPos(ctrl);

	const char *buffer = ctrl->buffer;
	int width = 0, height = 0, spacing = 1, cursor_width = 2;

	width = ctrl->cursor.current.x;
	height = ctrl->cursor.current.y;

	if(ctrl->blinky){
		int x = ((width * ctrl->wFactor) + spacing) - ctrl->hscroll + padding;
		int y = (height * ctrl->hFactor) - ctrl->vscroll;
		DrawRectangle(x, y, cursor_width, ctrl->font.fontsize, BLUE);
	}

}

