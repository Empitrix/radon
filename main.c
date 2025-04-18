#include <raylib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "src/utils.h"
#include "src/controller.h"


#define V2(x, y) ((Vector2){x, y})
#define MAXBUF 1024 * 1024


static int _width = 720, _height = 480;
static Font font = (Font){ 0 };
// static int fontsize = 18;
static int fontsize = 38;
static int spacing = 1;
static int tabsize = 4;
// static char buffer[MAXBUF] = { 0 };
// static uint32_t bufsiz = 0;

static Color bg_color = RAYWHITE;
static Color fg_color = BLACK;

static controller_t ctrl;




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



void render_cursor(controller_t *ctrl){
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
	DrawRectangle(x + 2 - ctrl->hscroll, (nlines * (ctrl->font.baseSize + spacing)) - ctrl->vscroll, 3, ctrl->font.baseSize, ctrl->blinky ? BLUE : bg_color);
}




int main(){
	SetTraceLogLevel(LOG_NONE);
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_RESIZABLE);
	InitWindow(_width, _height, "Radon");
	SetExitKey(KEY_ESCAPE);

	font = LoadFontEx("assets/HackNerdFont-Regular.ttf", fontsize, NULL, 450);

	initController(&ctrl, font, tabsize, spacing);

	while(!WindowShouldClose()){


		updateController(&ctrl);

		if(strstr(ctrl.buffer, "FPS")){
			modify_buffer(&ctrl, '\0', REMOVE_MODE);
			modify_buffer(&ctrl, '\0', REMOVE_MODE);
			modify_buffer(&ctrl, '\0', REMOVE_MODE);

			char bfr[128] = { 0 };
			sprintf(bfr, "%d", GetFPS());
			for(int i = 0; i < (int)strlen(bfr); i++){
				modify_buffer(&ctrl, bfr[i], INSERT_MODE);
			}
		}


		if(strstr(ctrl.buffer, ">exit")){ break; }


		BeginDrawing();
		ClearBackground(RAYWHITE);

		render_buffer(&ctrl);
		render_cursor(&ctrl);

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
