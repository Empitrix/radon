#include <raylib.h>
#include "controller.h"
#include "config.h"
#include "utils.h"


void render_buffer(controller_t *ctrl){
	int line_y = 0;
	const char *start = ctrl->buffer;
	const char *end;

	while (*start) {
		end = strchr(start, '\n');
		int len = end ? (end - start) : strlen(start);

		char line[1024];
		strncpy(line, start, len);
		line[len] = '\0';

		DrawTextEx(ctrl->font, line, V2(-ctrl->hscroll, line_y -ctrl->vscroll), ctrl->font.baseSize, ctrl->spacing, BLACK);

		line_y += ctrl->font.baseSize + ctrl->spacing;
		start = end ? end + 1 : NULL;
		if (!start) break;
	}
}



void render_cursor(controller_t *ctrl, Color bg_color){
	int buffer_size = (int)strlen(ctrl->buffer);
	int last_line = last_line_length(ctrl->buffer);
	int cntr = 0;
	int nlines = 0;  // Number of lines
	while(cntr < buffer_size){
		if(ctrl->buffer[cntr] == '\n'){ nlines++; }
		cntr++;
	}


	int x = MeasureTextEx(ctrl->font, get_last_line(ctrl->buffer), ctrl->font.baseSize, ctrl->spacing).x;
	int y = MeasureTextEx(ctrl->font, ctrl->buffer, ctrl->font.baseSize, ctrl->spacing).y;
	if(y == 0){ y += ctrl->font.baseSize; }
	//DrawRectangle(x + 2 - horz_scroll_offset, y - fontsize - vert_scroll_offset, 3, fontsize, swapper ? BLUE : bg_color);
	DrawRectangle(x + 2 - ctrl->hscroll, (nlines * (ctrl->font.baseSize + ctrl->spacing)) - ctrl->vscroll, 3, ctrl->font.baseSize, ctrl->blinky ? BLUE : bg_color);
}
