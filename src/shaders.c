#include <raylib.h>


Font fontLoadSDF(const char *font_file, const char *shader_file, int size, Shader *shader){
	int fileSize = 0;
	unsigned char *fileData = LoadFileData(font_file, &fileSize);
	// int glyph = 95;
	int glyph = 95;


	Font fontSDF = { 0 };
	fontSDF.baseSize = size;
	fontSDF.glyphCount = glyph;
	fontSDF.glyphs = LoadFontData(fileData, fileSize, size, 0, 0, FONT_SDF);
	Image atlas = GenImageFontAtlas(fontSDF.glyphs, &fontSDF.recs, glyph, size, 0, 1);
	fontSDF.texture = LoadTextureFromImage(atlas);
	UnloadImage(atlas);

	UnloadFileData(fileData);
	*shader = LoadShader(0, shader_file);
	SetTextureFilter(fontSDF.texture, TEXTURE_FILTER_BILINEAR);
	return fontSDF;
}

