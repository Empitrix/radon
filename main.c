#include <raylib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
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


static double max = 0;



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

	// controllerInit(&ctrl, "assets/CourierPrime-Bold.ttf", fontsize, tabsize, spacing);


	controllerSetWindowSize(&ctrl, borderWidth, borderHeight);


	while(!WindowShouldClose()){



		// struct timespec start, end;



		updateController(&ctrl);



		if(strstr(ctrl.buffer, ">exit")){ break; }


		BeginDrawing();
		// DrawRectangle(0, 0, borderWidth, borderHeight, RED);  // Border

		ClearBackground(RAYWHITE);


		// clock_gettime(CLOCK_MONOTONIC_RAW, &start);

		render_buffer(&ctrl);

		// clock_gettime(CLOCK_MONOTONIC_RAW, &end);
		// double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

		// if(elapsed > max){
		// 	printf("Largest time for render: %.9f s\n", elapsed);
		// 	max = elapsed;
		// }

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
