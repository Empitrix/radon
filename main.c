#include <raylib.h>
#include <stdint.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "src/thread.h"
#include "src/utils.h"


#define V2(x, y) ((Vector2){x, y})
#define MAXBUF 1024 * 1024

static int _width = 720, _height = 480;
static Font font = (Font){ 0 };
// static int fontsize = 18;
static int fontsize = 42;
static int spacing = 1;
static int tabsize = 4;
static char buffer[MAXBUF] = { 0 };
static uint32_t bufsiz = 0;

static Color bg_color = RAYWHITE;
static Color fg_color = BLACK;

static int swapper = 0;

static int vert_scroll_offset = 0;
static int horz_scroll_offset = 0;


typedef enum {
	INSERT_MODE,
	REMOVE_MODE,
} insert_mode_t;


void modify_buffer(char ch, insert_mode_t mode){
	switch(mode){
		case INSERT_MODE:
			buffer[bufsiz++] = ch;
			break;
		case REMOVE_MODE:
			buffer[--bufsiz] = '\0';
			break;
		default: break;
	}
	swapper = 1;
}

void remove_word(void) {
	while (bufsiz > 0) {
		// Remove trailing spaces or punctuation first
		char c = buffer[bufsiz - 1];
		if (!isalnum(c)) {
			buffer[--bufsiz] = '\0';
		} else {
			break;
		}
	}
	while (bufsiz > 0) {
		char c = buffer[bufsiz - 1];
		if (isalnum(c)) {
			buffer[--bufsiz] = '\0';
		} else {
			break;
		}
	}
	swapper = 1;
}


void *blink_cursor(void *_){
	while(1){
		swapper = swapper == 0;
		usleep(500000L);
	}
	return NULL;
}


void render_buffer() {
	int line_y = 0;
	const char *start = buffer;
	const char *end;

	while (*start) {
		end = strchr(start, '\n');
		int len = end ? (end - start) : strlen(start);

		char line[1024];
		strncpy(line, start, len);
		line[len] = '\0';

		DrawTextEx(font, line, V2(-horz_scroll_offset, line_y - vert_scroll_offset), fontsize, spacing, BLACK);

		line_y += fontsize + spacing;
		start = end ? end + 1 : NULL;
		if (!start) break;
	}
}



void render_cursor(){
	int buffer_size = (int)strlen(buffer);
	int last_line = last_line_length(buffer);
	int cntr = 0;
	int nlines = 0;  // Number of lines
	while(cntr < buffer_size){
		if(buffer[cntr] == '\n'){ nlines++; }
		cntr++;
	}


	int x = MeasureTextEx(font, get_last_line(buffer), fontsize, spacing).x;
	int y = MeasureTextEx(font, buffer, fontsize, spacing).y;
	if(y == 0){ y += fontsize; }
	//DrawRectangle(x + 2 - horz_scroll_offset, y - fontsize - vert_scroll_offset, 3, fontsize, swapper ? BLUE : bg_color);
	DrawRectangle(x + 2 - horz_scroll_offset, (nlines * (fontsize + spacing)) - vert_scroll_offset, 3, fontsize, swapper ? BLUE : bg_color);
}




int main(){
	SetTraceLogLevel(LOG_NONE);
	SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_RESIZABLE);
	InitWindow(_width, _height, "Radon");
	SetExitKey(KEY_ESCAPE);

	font = LoadFontEx("assets/HackNerdFont-Regular.ttf", fontsize, NULL, 450);
	exe_thread(blink_cursor, NULL);


	while(!WindowShouldClose()){
		char ch = '\0';
		if((ch = GetCharPressed()) != '\0' && bufsiz != MAXBUF){
			modify_buffer(ch, INSERT_MODE);
		}

		KeyboardKey ckey = GetKeyPressed();

		if (ckey == KEY_BACKSPACE && bufsiz != 0) {
			if (IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) {
				remove_word();
			} else {
				modify_buffer('\0', REMOVE_MODE);
			}
		}

		if(IsKeyPressedRepeat(KEY_BACKSPACE) && bufsiz != 0){
			modify_buffer('\0', REMOVE_MODE);
		}

		if(ckey == KEY_ENTER){
			modify_buffer('\n', INSERT_MODE);
		}

		if(ckey == KEY_TAB){
			for(int i = 0; i < tabsize; i++){
				modify_buffer(' ', INSERT_MODE);
			}
		}

		if(IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)){
			horz_scroll_offset -= GetMouseWheelMove() * font.baseSize;
			if(horz_scroll_offset < 0){
				horz_scroll_offset = 0;
			}
		} else {
			vert_scroll_offset -= GetMouseWheelMove() * font.baseSize;
			if(vert_scroll_offset < 0){
				vert_scroll_offset = 0;
			}
		}



		if ((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_V)) {
			const char *text = GetClipboardText();
			for(int i = 0; i < (int)strlen(text); i++){
				switch(text[i]){
					case '\t':
						for(int i = 0; i < tabsize; i++){
							modify_buffer(' ', INSERT_MODE);
						}
						break;
					default:
						modify_buffer(text[i], INSERT_MODE);
						break;
				}
			}
		}


		/*
		if(strstr(buffer, "FPS")){
			modify_buffer('\0', REMOVE_MODE);
			modify_buffer('\0', REMOVE_MODE);
			modify_buffer('\0', REMOVE_MODE);

			char bfr[128] = { 0 };
			sprintf(bfr, "%d", GetFPS());
			for(int i = 0; i < (int)strlen(bfr); i++){
				modify_buffer(bfr[i], INSERT_MODE);
			}
		}
		*/


		BeginDrawing();
		ClearBackground(RAYWHITE);

		render_buffer();
		render_cursor();

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
