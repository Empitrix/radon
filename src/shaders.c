#include <string.h>
#include <raylib.h>
#include "types.h"


font_t fontLoadSDF(const char *font_file, const char *shader_file, int size, int spacing){
	int fileSize = 0;
	unsigned char *fileData = LoadFileData(font_file, &fileSize);
	int glyph = 95;


	Font fontSDF = { 0 };
	fontSDF.baseSize = size;
	fontSDF.glyphCount = glyph;
	fontSDF.glyphs = LoadFontData(fileData, fileSize, size, 0, 0, FONT_SDF);
	Image atlas = GenImageFontAtlas(fontSDF.glyphs, &fontSDF.recs, glyph, size, 0, 1);
	fontSDF.texture = LoadTextureFromImage(atlas);
	UnloadImage(atlas);

	UnloadFileData(fileData);
	Shader shader = LoadShader(0, shader_file);
	SetTextureFilter(fontSDF.texture, TEXTURE_FILTER_BILINEAR);

	font_t f = (font_t){
		.font = fontSDF,
		.shader = shader,
		.fontsize = size,
		.spacing = spacing
	};

	strcpy(f.font_path, font_file);
	strcpy(f.fs_file, shader_file);

	return f;
}

