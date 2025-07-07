#include <raylib.h>
#include <stdio.h>
#include <stdint.h>
#include "controller.h"
#include "config.h"



void render_cursor(controller_t *ctrl, int padding);


static int fontLoaded = 0;



void render_buffer(controller_t *ctrl){

	DrawRectangle(0, 0, ctrl->windowWidth, ctrl->windowHeight, (Color){26, 27, 38, 255});


	// DrawRectangle(0, 0, ctrl->windowWidth, ctrl->windowHeight, RED);  // Border

	const char *start = ctrl->buffer;
	int margin = 0;

	// controllerUpdateBuffer(ctrl);

	int l = getIntLen(getMaxLines(ctrl));

	int numLineSize = l * ctrl->wFactor;

	int cln = 0;

	getCurrentLineData(ctrl, &cln, NULL);


	BeginShaderMode(ctrl->font.shader);

	numLineSize += 12;


	// Show Selected Background
	if(ctrl->cursor.mode == CURSOR_SELECT){
		int ch = 0;
		int ln = 0;
		for(int i = 0; i < (int)strlen(ctrl->buffer); i++){

			int x = (ch * ctrl->wFactor)-ctrl->hscroll + numLineSize;
			int y = (ln * ctrl->hFactor)-ctrl->vscroll;

			if(ctrl->buffer[i] == '\n'){
				ch = 0;
				ln++;
			} else {
				ch++;
			}

			if(i >= ctrl->cursor.selection.start && i <= ctrl->cursor.selection.end){
				DrawRectangle(x, y, ctrl->wFactor, ctrl->font.fontsize, YELLOW);
			}

		}
	}

	// Render cursor
	render_cursor(ctrl, numLineSize);


	// Render Text
	int chr = 0, ln = 0;
	for(int i = 0; i < (int)strlen(ctrl->buffer); i++){
		int selected = i >= ctrl->cursor.selection.start && i <= ctrl->cursor.selection.end && ctrl->cursor.mode == CURSOR_SELECT;
		int x = (ctrl->wFactor * chr) -ctrl->hscroll + numLineSize;
		int y = (ctrl->hFactor * ln) -ctrl->vscroll;

		if(ctrl->buffer[i] == '\n'){
			chr = 0;
			ln++;
			continue;
		} else {
			chr++;
		}


		Color klr = selected ? BLACK : WHITE;


		const char *word = "local";

		// uintptr_t start = (uintptr_t)strstr(&ctrl->buffer[i], word);
		// // uintptr_t start = (uintptr_t)strstr(&ctrl->buffer[i], word);
		// if(start >= (uintptr_t)&ctrl->buffer[i + (int)strlen(word)] && start - 5 <= (uintptr_t)&ctrl->buffer[i]){
		// 	klr = RED;
		// }


		uintptr_t start = (uintptr_t)strstr(ctrl->buffer, word);
		// uintptr_t start = (uintptr_t)strstr(&ctrl->buffer[i], word);
		if(start > (uintptr_t)&ctrl->buffer[i - (int)strlen(word)] && start <= (uintptr_t)&ctrl->buffer[i]){
			klr = RED;
		}


		DrawTextCodepoint(ctrl->font.font, ctrl->buffer[i], V2(x, y), ctrl->font.fontsize, klr);
	}


	// Render Numbers
	for(int i = 0; i < ctrl->windowHeight; i += ctrl->hFactor){
		int lNum = i / ctrl->hFactor;  // line number
		int selected = cln == (lNum - 1);
		DrawRectangle(0, i - ctrl->hFactor, (numLineSize - 12) + 7, ctrl->hFactor, (Color){26, 27, 38, 255});
		int max = getMaxLines(ctrl);
		if(max == 0){ max++; }
		if((lNum - 1) < max){
			DrawTextEx(ctrl->font.font, TextFormat("%*d", l, lNum), V2(2, (i - ctrl->hFactor) -ctrl->vscroll), ctrl->font.fontsize, ctrl->font.spacing, selected ? YELLOW : GRAY);
		}
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

