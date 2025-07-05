#include <raylib.h>
#include <stdint.h>
#include "config.h"



typedef enum {
	CURSOR_WRITE,
	CURSOR_SELECT,
} cursor_mode;


typedef struct {
	int x;
	int y;
} pos_t;


typedef struct {
	int start;
	int end;
} range_t;



typedef struct {
	cursor_mode mode;
	range_t selection;         // Range of the selected text [start, end]
	pos_t current;
	pos_t extend;
	int max;
} cursor_t;


typedef struct {
	Shader shader;
	char font_path[128];       // Font path buffer
	char fs_file[128];         // Font path buffer
	Font font;                 // Config Font
	int spacing;               // Letter spacing
	uint8_t fontsize;          // Font Size
} font_t; 



typedef struct {
	char buffer[MAXBUF];       // Buffer
	font_t font;
	uint32_t index;            // Index (Buf pos)
	int tabsize;               // Tab size
	int hscroll;               // Horizontal Scroll Offset
	int vscroll;               // Vertical Scroll Offset
	uint8_t blinky;            // Cursor
	int csize;                 // Char size
	// int curosr;                // Cursor pos
	int windowWidth;           // Window Width
	int windowHeight;          // Window Height

	cursor_t cursor;

	int wFactor;
	int hFactor;
} controller_t;
