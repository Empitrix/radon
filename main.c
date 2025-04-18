#include <raylib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "src/render.h"
#include "src/controller.h"


static int _width = 720, _height = 480;
static Font font = (Font){ 0 };
static int fontsize = 24;
static int spacing = 1;
static int tabsize = 4;

static Color bg_color = RAYWHITE;
static Color fg_color = BLACK;
static controller_t ctrl;




int main(){
	SetTraceLogLevel(LOG_NONE);
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_RESIZABLE);
	InitWindow(_width, _height, "Radon");
	SetExitKey(KEY_ESCAPE);

	font = LoadFontEx("", fontsize, NULL, 450);

	initController(&ctrl, "assets/HackNerdFont-Regular.ttf", fontsize, tabsize, spacing);

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
		render_cursor(&ctrl, bg_color);

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
