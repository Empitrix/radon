#include <raylib.h>

static int _width = 720, _height = 480;


int main(){
	InitWindow(_width, _height, "Radon");
	SetExitKey(KEY_ESCAPE);

	while(!WindowShouldClose()){
		BeginDrawing();
		ClearBackground(RAYWHITE);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
