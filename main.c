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
static int spacing = 0;
static int tabsize = 4;

static Color bg_color = RAYWHITE;
static Color fg_color = BLACK;
static controller_t ctrl;




int main(){
	SetTraceLogLevel(LOG_NONE);
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_RESIZABLE);
	InitWindow(_width, _height, "Radon");
	SetExitKey(KEY_ESCAPE);
	SetTargetFPS(60);

	font = LoadFontEx("", fontsize, NULL, 450);

	int borderWidth = _width / 1.3, borderHeight = _height;  // Test specifiers

	// controllerInit(&ctrl, "assets/HackNerdFont-Regular.ttf", fontsize, tabsize, spacing);
	controllerInit(&ctrl, "assets/CourierPrime-Regular.ttf", fontsize, tabsize, spacing);

	controllerSetWindowSize(&ctrl, borderWidth, borderHeight);


	while(!WindowShouldClose()){


		// DrawRectangle(0, 0, borderWidth, borderHeight, RED);  // Border

		updateController(&ctrl);

		// Set FPS in the window title!
		// SetWindowTitle(TextFormat("Radon (%d)", GetFPS()));

		if(strstr(ctrl.buffer, "FPS")){
			for(int i = 0; i < strlen("FPS"); i++){
				modify_buffer(&ctrl, '\0', REMOVE_MODE);
			}

			char bfr[128] = { 0 };
			sprintf(bfr, "%d", GetFPS());
			for(int i = 0; i < (int)strlen(bfr); i++){
				modify_buffer(&ctrl, bfr[i], INSERT_MODE);
			}
		}


		if(strstr(ctrl.buffer, ">exit")){ break; }


		BeginDrawing();
		ClearBackground(RAYWHITE);

		// // Selection
		// if(IsKeyDown(KEY_LEFT_CONTROL)){
		// 	printf("Should Work!\n");
		// 	// if(ctrl->index){
		// 	// 	ctrl->index--;
		// 	// }
		// 	// ctrl->blinky = 1;
		// }


		// if(IsKeyPressed(KEY_LEFT) && IsKeyDown(KEY_LEFT_CONTROL)){
		// 	printf("+++++++++++!\n");
		// }

		// controllerUpdateBuffer(&ctrl);
		render_buffer(&ctrl);
		// render_cursor(&ctrl, bg_color, 0);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
