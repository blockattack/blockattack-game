/*
NFont v4.0.0: A font class for SDL
by Jonathan Dearborn
Dedicated to the memory of Florian Hufsky

License:
    The short:
    Use it however you'd like, but keep the copyright and license notice
    whenever these files or parts of them are distributed in uncompiled form.

    The long:
Copyright (c) 2014 Jonathan Dearborn

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "NFont.h"
#include "SDL_FontCache.h"

#include <cmath>
#include <cstdio>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <string>
#include <cstring>
#include <list>
using std::string;
using std::list;

#ifdef NFONT_USE_SDL_GPU
#define NFont_Target GPU_Target
#define NFont_Image GPU_Image
#define NFont_Log GPU_LogError
#else
#define NFont_Target SDL_Renderer
#define NFont_Image SDL_Texture
#define NFont_Log SDL_Log
#endif

#define MIN(a,b) ((a) < (b)? (a) : (b))
#define MAX(a,b) ((a) > (b)? (a) : (b))

static inline SDL_Surface* createSurface24(Uint32 width, Uint32 height) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	return SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 24, 0xFF0000, 0x00FF00, 0x0000FF, 0);
#else
	return SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 24, 0x0000FF, 0x00FF00, 0xFF0000, 0);
#endif
}

static inline SDL_Surface* createSurface32(Uint32 width, Uint32 height) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	return SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
#else
	return SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
#endif
}

static inline char* copyString(const char* c) {
	if (c == NULL) {
		return NULL;
	}

	char* result = new char[strlen(c)+1];
	strcpy(result, c);

	return result;
}

static inline Uint32 getPixel(SDL_Surface* Surface, int x, int y) {
	Uint8* bits;
	Uint32 bpp;

	if (x < 0 || x >= Surface->w) {
		return 0;    // Best I could do for errors
	}

	bpp = Surface->format->BytesPerPixel;
	bits = ((Uint8*)Surface->pixels) + y*Surface->pitch + x*bpp;

	switch (bpp) {
	case 1:
		return *((Uint8*)Surface->pixels + y * Surface->pitch + x);
		break;
	case 2:
		return *((Uint16*)Surface->pixels + y * Surface->pitch/2 + x);
		break;
	case 3:
		// Endian-correct, but slower
		Uint8 r, g, b;
		r = *((bits)+Surface->format->Rshift/8);
		g = *((bits)+Surface->format->Gshift/8);
		b = *((bits)+Surface->format->Bshift/8);
		return SDL_MapRGB(Surface->format, r, g, b);
		break;
	case 4:
		return *((Uint32*)Surface->pixels + y * Surface->pitch/4 + x);
		break;
	}

	return 0;  // FIXME: Handle errors better
}

static inline void setPixel(SDL_Surface* surface, int x, int y, Uint32 color) {
	int bpp = surface->format->BytesPerPixel;
	Uint8* bits = ((Uint8*)surface->pixels) + y*surface->pitch + x*bpp;

	/* Set the pixel */
	switch (bpp) {
	case 1:
		*((Uint8*)(bits)) = (Uint8)color;
		break;
	case 2:
		*((Uint16*)(bits)) = (Uint16)color;
		break;
	case 3: { /* Format/endian independent */
		Uint8 r,g,b;
		r = (color >> surface->format->Rshift) & 0xFF;
		g = (color >> surface->format->Gshift) & 0xFF;
		b = (color >> surface->format->Bshift) & 0xFF;
		*((bits)+surface->format->Rshift/8) = r;
		*((bits)+surface->format->Gshift/8) = g;
		*((bits)+surface->format->Bshift/8) = b;
	}
	break;
	case 4:
		*((Uint32*)(bits)) = (Uint32)color;
		break;
	}
}

static inline void drawPixel(SDL_Surface* surface, Sint16 x, Sint16 y, Uint32 color, Uint8 alpha) {
	if (x > surface->clip_rect.x + surface->clip_rect.w || x < surface->clip_rect.x || y > surface->clip_rect.y + surface->clip_rect.h || y < surface->clip_rect.y) {
		return;
	}

	switch (surface->format->BytesPerPixel) {
	case 1: { /* Assuming 8-bpp */

		Uint8* pixel = (Uint8*)surface->pixels + y*surface->pitch + x;

		Uint8 dR = surface->format->palette->colors[*pixel].r;
		Uint8 dG = surface->format->palette->colors[*pixel].g;
		Uint8 dB = surface->format->palette->colors[*pixel].b;
		Uint8 sR = surface->format->palette->colors[color].r;
		Uint8 sG = surface->format->palette->colors[color].g;
		Uint8 sB = surface->format->palette->colors[color].b;

		dR = dR + ((sR-dR)*alpha >> 8);
		dG = dG + ((sG-dG)*alpha >> 8);
		dB = dB + ((sB-dB)*alpha >> 8);

		*pixel = SDL_MapRGB(surface->format, dR, dG, dB);

	}
	break;

	case 2: { /* Probably 15-bpp or 16-bpp */

		Uint32 Rmask = surface->format->Rmask, Gmask = surface->format->Gmask, Bmask = surface->format->Bmask, Amask = surface->format->Amask;
		Uint16* pixel = (Uint16*)surface->pixels + y*surface->pitch/2 + x;
		Uint32 dc = *pixel;
		Uint32 R,G,B,A=0;

		R = ((dc & Rmask) + (( (color & Rmask) - (dc & Rmask) ) * alpha >> 8)) & Rmask;
		G = ((dc & Gmask) + (( (color & Gmask) - (dc & Gmask) ) * alpha >> 8)) & Gmask;
		B = ((dc & Bmask) + (( (color & Bmask) - (dc & Bmask) ) * alpha >> 8)) & Bmask;
		if ( Amask ) {
			A = ((dc & Amask) + (( (color & Amask) - (dc & Amask) ) * alpha >> 8)) & Amask;
		}

		*pixel= R | G | B | A;

	}
	break;

	case 3: { /* Slow 24-bpp mode, usually not used */
		Uint8* pix = (Uint8*)surface->pixels + y * surface->pitch + x*3;
		Uint8 rshift8=surface->format->Rshift/8;
		Uint8 gshift8=surface->format->Gshift/8;
		Uint8 bshift8=surface->format->Bshift/8;
		Uint8 ashift8=surface->format->Ashift/8;



		Uint8 dR, dG, dB, dA;
		Uint8 sR, sG, sB, sA;

		pix = (Uint8*)surface->pixels + y * surface->pitch + x*3;

		dR = *((pix)+rshift8);
		dG = *((pix)+gshift8);
		dB = *((pix)+bshift8);
		dA = *((pix)+ashift8);

		sR = (color>>surface->format->Rshift)&0xff;
		sG = (color>>surface->format->Gshift)&0xff;
		sB = (color>>surface->format->Bshift)&0xff;
		sA = (color>>surface->format->Ashift)&0xff;

		dR = dR + ((sR-dR)*alpha >> 8);
		dG = dG + ((sG-dG)*alpha >> 8);
		dB = dB + ((sB-dB)*alpha >> 8);
		dA = dA + ((sA-dA)*alpha >> 8);

		*((pix)+rshift8) = dR;
		*((pix)+gshift8) = dG;
		*((pix)+bshift8) = dB;
		*((pix)+ashift8) = dA;

	}
	break;

	case 4: { /* Probably 32-bpp */
		Uint32 Rmask = surface->format->Rmask, Gmask = surface->format->Gmask, Bmask = surface->format->Bmask, Amask = surface->format->Amask;
		Uint32* pixel = (Uint32*)surface->pixels + y*surface->pitch/4 + x;
		Uint32 source = *pixel;
		Uint32 R,G,B,A;
		R = color & Rmask;
		G = color & Gmask;
		B = color & Bmask;
		A = 0;  // keep this as 0 to avoid corruption of non-alpha surfaces

		// Blend and keep dest alpha
		if ( alpha != SDL_ALPHA_OPAQUE ) {
			R = ((source & Rmask) + (( R - (source & Rmask) ) * alpha >> 8)) & Rmask;
			G = ((source & Gmask) + (( G - (source & Gmask) ) * alpha >> 8)) & Gmask;
			B = ((source & Bmask) + (( B - (source & Bmask) ) * alpha >> 8)) & Bmask;
		}
		if (Amask) {
			A = (source & Amask);
		}

		*pixel = R | G | B | A;
	}
	break;
	}
}

static inline NFont::Rectf rectUnion(const NFont::Rectf& A, const NFont::Rectf& B) {
	float x,x2,y,y2;
	x = MIN(A.x, B.x);
	y = MIN(A.y, B.y);
	x2 = MAX(A.x+A.w, B.x+B.w);
	y2 = MAX(A.y+A.h, B.y+B.h);
	NFont::Rectf result(x, y, MAX(0, x2 - x), MAX(0, y2 - y));
	return result;
}

// Adapted from SDL_IntersectRect
static inline NFont::Rectf rectIntersect(const NFont::Rectf& A, const NFont::Rectf& B) {
	NFont::Rectf result;
	float Amin, Amax, Bmin, Bmax;

	// Horizontal intersection
	Amin = A.x;
	Amax = Amin + A.w;
	Bmin = B.x;
	Bmax = Bmin + B.w;
	if (Bmin > Amin) {
		Amin = Bmin;
	}
	result.x = Amin;
	if (Bmax < Amax) {
		Amax = Bmax;
	}
	result.w = Amax - Amin > 0 ? Amax - Amin : 0;

	// Vertical intersection
	Amin = A.y;
	Amax = Amin + A.h;
	Bmin = B.y;
	Bmax = Bmin + B.h;
	if (Bmin > Amin) {
		Amin = Bmin;
	}
	result.y = Amin;
	if (Bmax < Amax) {
		Amax = Bmax;
	}
	result.h = Amax - Amin > 0 ? Amax - Amin : 0;

	return result;
}

static inline SDL_Surface* copySurface(SDL_Surface* Surface) {
	return SDL_ConvertSurface(Surface, Surface->format, Surface->flags);
}









NFont::Color::Color()
	: r(0), g(0), b(0), a(255) {
}
NFont::Color::Color(Uint8 r, Uint8 g, Uint8 b)
	: r(r), g(g), b(b), a(255) {
}
NFont::Color::Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
	: r(r), g(g), b(b), a(a) {
}
NFont::Color::Color(const SDL_Color& color)
	: r(color.r), g(color.g), b(color.b), a(color.a) {
}

NFont::Color& NFont::Color::rgb(Uint8 R, Uint8 G, Uint8 B) {
	r = R;
	g = G;
	b = B;

	return *this;
}

NFont::Color& NFont::Color::rgba(Uint8 R, Uint8 G, Uint8 B, Uint8 A) {
	r = R;
	g = G;
	b = B;
	a = A;

	return *this;
}

NFont::Color& NFont::Color::color(const SDL_Color& color) {
	r = color.r;
	g = color.g;
	b = color.b;
	a = color.a;

	return *this;
}

SDL_Color NFont::Color::to_SDL_Color() const {
	SDL_Color c = {r, g, b, a};
	return c;
}




NFont::Rectf::Rectf()
	: x(0), y(0), w(0), h(0) {
}

NFont::Rectf::Rectf(float x, float y)
	: x(x), y(y), w(0), h(0) {
}

NFont::Rectf::Rectf(float x, float y, float w, float h)
	: x(x), y(y), w(w), h(h) {
}

NFont::Rectf::Rectf(const SDL_Rect& rect)
	: x(rect.x), y(rect.y), w(rect.w), h(rect.h) {
}

#ifdef NFONT_USE_SDL_GPU
NFont::Rectf::Rectf(const GPU_Rect& rect)
	: x(rect.x), y(rect.y), w(rect.w), h(rect.h) {
}
#endif

SDL_Rect NFont::Rectf::to_SDL_Rect() const {
	SDL_Rect r = {int(x), int(y), int(w), int(h)};
	return r;
}

#ifdef NFONT_USE_SDL_GPU
GPU_Rect NFont::Rectf::to_GPU_Rect() const {
	return GPU_MakeRect(x, y, w, h);
}
#endif







char* NFont::buffer = NULL;  // Shared buffer for efficient drawing


// Constructors
NFont::NFont() {
	init();
}

NFont::NFont(const NFont& font) {
	init();
	// FIXME: Duplicate font data
}

#ifdef NFONT_USE_SDL_GPU
NFont::NFont(TTF_Font* ttf) {
	init();
	load(ttf, FC_GetDefaultColor(font));
}
NFont::NFont(TTF_Font* ttf, const NFont::Color& color) {
	init();
	load(ttf, color);
}
NFont::NFont(const char* filename_ttf, Uint32 pointSize) {
	init();
	load(filename_ttf, pointSize);
}
NFont::NFont(const char* filename_ttf, Uint32 pointSize, const NFont::Color& color, int style) {
	init();
	load(filename_ttf, pointSize, color, style);
}
NFont::NFont(SDL_RWops* file_rwops_ttf, Uint8 own_rwops, Uint32 pointSize, const NFont::Color& color, int style) {
	init();
	load(file_rwops_ttf, own_rwops, pointSize, color, style);
}

#else

NFont::NFont(NFont_Target* renderer, TTF_Font* ttf) {
	init();
	load(renderer, ttf, FC_GetDefaultColor(font));
}
NFont::NFont(NFont_Target* renderer, TTF_Font* ttf, const NFont::Color& color) {
	init();
	load(renderer, ttf, color);
}
NFont::NFont(NFont_Target* renderer, const char* filename_ttf, Uint32 pointSize) {
	init();
	load(renderer, filename_ttf, pointSize);
}
NFont::NFont(NFont_Target* renderer, const char* filename_ttf, Uint32 pointSize, const NFont::Color& color, int style) {
	init();
	load(renderer, filename_ttf, pointSize, color, style);
}
NFont::NFont(NFont_Target* renderer, SDL_RWops* file_rwops_ttf, Uint8 own_rwops, Uint32 pointSize, const NFont::Color& color, int style) {
	init();
	load(renderer, file_rwops_ttf, own_rwops, pointSize, color, style);
}
#endif


NFont::~NFont() {
	FC_FreeFont(font);
}


NFont& NFont::operator=(const NFont& font) {
	// FIXME: Duplicate font data
	return *this;
}

void NFont::init() {
	font = FC_CreateFont();

	if (buffer == NULL) {
		buffer = new char[1024];
	}
}






void NFont::setLoadingString(const char* str) {
	FC_SetLoadingString(font, str);
}

#ifdef NFONT_USE_SDL_GPU
bool NFont::load(TTF_Font* ttf)
#else
bool NFont::load(NFont_Target* renderer, TTF_Font* ttf)
#endif
{
#ifdef NFONT_USE_SDL_GPU
	return load(ttf, FC_GetDefaultColor(font));
#else
	return load(renderer, ttf, Color(0,0,0,255));
#endif
}

#ifdef NFONT_USE_SDL_GPU
bool NFont::load(TTF_Font* ttf, const NFont::Color& color)
#else
bool NFont::load(NFont_Target* renderer, TTF_Font* ttf, const NFont::Color& color)
#endif
{
	if (ttf == NULL) {
		return false;
	}

#ifndef NFONT_USE_SDL_GPU
	if (renderer == NULL) {
		return false;
	}
#endif

	FC_ClearFont(font);
#ifdef NFONT_USE_SDL_GPU
	return FC_LoadFontFromTTF(font, ttf, color.to_SDL_Color());
#else
	return FC_LoadFontFromTTF(font, renderer, ttf, color.to_SDL_Color());
#endif
}

#ifdef NFONT_USE_SDL_GPU
bool NFont::load(const char* filename_ttf, Uint32 pointSize)
#else
bool NFont::load(NFont_Target* renderer, const char* filename_ttf, Uint32 pointSize)
#endif
{
#ifdef NFONT_USE_SDL_GPU
	return load(filename_ttf, pointSize, Color(0,0,0,255));
#else
	return load(renderer, filename_ttf, pointSize, Color(0,0,0,255));
#endif
}

#ifdef NFONT_USE_SDL_GPU
bool NFont::load(const char* filename_ttf, Uint32 pointSize, const NFont::Color& color, int style)
#else
bool NFont::load(NFont_Target* renderer, const char* filename_ttf, Uint32 pointSize, const NFont::Color& color, int style)
#endif
{
	FC_ClearFont(font);
#ifdef NFONT_USE_SDL_GPU
	return FC_LoadFont(font, filename_ttf, pointSize, color.to_SDL_Color(), style);
#else
	return FC_LoadFont(font, renderer, filename_ttf, pointSize, color.to_SDL_Color(), style);
#endif
}

#ifdef NFONT_USE_SDL_GPU
bool NFont::load(SDL_RWops* file_rwops_ttf, Uint8 own_rwops, Uint32 pointSize, const NFont::Color& color, int style)
#else
bool NFont::load(NFont_Target* renderer, SDL_RWops* file_rwops_ttf, Uint8 own_rwops, Uint32 pointSize, const NFont::Color& color, int style)
#endif
{
	FC_ClearFont(font);
#ifdef NFONT_USE_SDL_GPU
	return FC_LoadFont_RW(font, file_rwops_ttf, own_rwops, pointSize, color.to_SDL_Color(), style);
#else
	return FC_LoadFont_RW(font, renderer, file_rwops_ttf, own_rwops, pointSize, color.to_SDL_Color(), style);
#endif
}



void NFont::free() {
	FC_ClearFont(font);
}



NFont::Rectf NFont::draw(NFont_Target* dest, float x, float y, const char* formatted_text, ...) {
	if (formatted_text == NULL) {
		return Rectf(x, y, 0, 0);
	}

	va_list lst;
	va_start(lst, formatted_text);
	vsprintf(buffer, formatted_text, lst);
	va_end(lst);

	return FC_Draw(font, dest, x, y, "%s", buffer);
}

/*static int getIndexPastWidth(const char* text, int width, const int* charWidth)
{
    int charnum;
    int len = strlen(text);

    for (int index = 0; index < len; index++)
    {
        char c = text[index];
        charnum = (unsigned char)(c) - 33;

        // spaces and nonprintable characters
        if (c == ' ' || charnum > 222)
        {
            width -= charWidth[0];
        }
        else
            width -= charWidth[charnum];

        if(width <= 0)
            return index;
    }
    return 0;
}*/



/*static list<string> explode(const string& str, char delimiter)
{
    list<string> result;

    size_t oldPos = 0;
    size_t pos = str.find_first_of(delimiter);
    while(pos != string::npos)
    {
        result.push_back(str.substr(oldPos, pos - oldPos));
        oldPos = pos+1;
        pos = str.find_first_of(delimiter, oldPos);
    }

    result.push_back(str.substr(oldPos, string::npos));

    return result;
}*/

NFont::Rectf NFont::drawBox(NFont_Target* dest, const Rectf& box, const char* formatted_text, ...) {
	if (formatted_text == NULL) {
		return Rectf(box.x, box.y, 0, 0);
	}

	va_list lst;
	va_start(lst, formatted_text);
	vsprintf(buffer, formatted_text, lst);
	va_end(lst);

#ifdef NFONT_USE_SDL_GPU
	return FC_DrawBox(font, dest, box.to_GPU_Rect(), "%s", buffer);
#else
	return FC_DrawBox(font, dest, box.to_SDL_Rect(), "%s", buffer);
#endif
}

static FC_AlignEnum translate_enum_NFont_to_FC(NFont::AlignEnum align) {
	switch (align) {
	case NFont::LEFT:
		return FC_ALIGN_LEFT;
	case NFont::CENTER:
		return FC_ALIGN_CENTER;
	case NFont::RIGHT:
		return FC_ALIGN_RIGHT;
	default:
		return FC_ALIGN_LEFT;
	}
}

NFont::Rectf NFont::drawBox(NFont_Target* dest, const Rectf& box, AlignEnum align, const char* formatted_text, ...) {
	if (formatted_text == NULL) {
		return Rectf(box.x, box.y, 0, 0);
	}

	va_list lst;
	va_start(lst, formatted_text);
	vsprintf(buffer, formatted_text, lst);
	va_end(lst);

#ifdef NFONT_USE_SDL_GPU
	return FC_DrawBoxAlign(font, dest, box.to_GPU_Rect(), translate_enum_NFont_to_FC(align), "%s", buffer);
#else
	return FC_DrawBoxAlign(font, dest, box.to_SDL_Rect(), translate_enum_NFont_to_FC(align), "%s", buffer);
#endif
}

NFont::Rectf NFont::drawBox(NFont_Target* dest, const Rectf& box, const Scale& scale, const char* formatted_text, ...) {
	if (formatted_text == NULL) {
		return Rectf(box.x, box.y, 0, 0);
	}

	va_list lst;
	va_start(lst, formatted_text);
	vsprintf(buffer, formatted_text, lst);
	va_end(lst);

#ifdef NFONT_USE_SDL_GPU
	return FC_DrawBoxScale(font, dest, box.to_GPU_Rect(), FC_MakeScale(scale.x, scale.y), "%s", buffer);
#else
	return FC_DrawBoxScale(font, dest, box.to_SDL_Rect(), FC_MakeScale(scale.x, scale.y), "%s", buffer);
#endif
}

NFont::Rectf NFont::drawBox(NFont_Target* dest, const Rectf& box, const Color& color, const char* formatted_text, ...) {
	if (formatted_text == NULL) {
		return Rectf(box.x, box.y, 0, 0);
	}

	va_list lst;
	va_start(lst, formatted_text);
	vsprintf(buffer, formatted_text, lst);
	va_end(lst);

#ifdef NFONT_USE_SDL_GPU
	return FC_DrawBoxColor(font, dest, box.to_GPU_Rect(), color.to_SDL_Color(), "%s", buffer);
#else
	return FC_DrawBoxColor(font, dest, box.to_SDL_Rect(), color.to_SDL_Color(), "%s", buffer);
#endif
}

NFont::Rectf NFont::drawBox(NFont_Target* dest, const Rectf& box, const Effect& effect, const char* formatted_text, ...) {
	if (formatted_text == NULL) {
		return Rectf(box.x, box.y, 0, 0);
	}

	va_list lst;
	va_start(lst, formatted_text);
	vsprintf(buffer, formatted_text, lst);
	va_end(lst);

#ifdef NFONT_USE_SDL_GPU
	return FC_DrawBoxEffect(font, dest, box.to_GPU_Rect(), FC_MakeEffect(translate_enum_NFont_to_FC(effect.alignment), FC_MakeScale(effect.scale.x, effect.scale.y), effect.color.to_SDL_Color()), "%s", buffer);
#else
	return FC_DrawBoxEffect(font, dest, box.to_SDL_Rect(), FC_MakeEffect(translate_enum_NFont_to_FC(effect.alignment), FC_MakeScale(effect.scale.x, effect.scale.y), effect.color.to_SDL_Color()), "%s", buffer);
#endif
}

NFont::Rectf NFont::drawColumn(NFont_Target* dest, float x, float y, Uint16 width, const char* formatted_text, ...) {
	if (formatted_text == NULL) {
		return Rectf(x, y, 0, 0);
	}

	va_list lst;
	va_start(lst, formatted_text);
	vsprintf(buffer, formatted_text, lst);
	va_end(lst);

	return FC_DrawColumn(font, dest, x, y, width, "%s", buffer);
}

NFont::Rectf NFont::drawColumn(NFont_Target* dest, float x, float y, Uint16 width, AlignEnum align, const char* formatted_text, ...) {
	if (formatted_text == NULL) {
		return Rectf(x, y, 0, 0);
	}

	va_list lst;
	va_start(lst, formatted_text);
	vsprintf(buffer, formatted_text, lst);
	va_end(lst);

	return FC_DrawColumnAlign(font, dest, x, y, width, translate_enum_NFont_to_FC(align), "%s", buffer);
}

NFont::Rectf NFont::drawColumn(NFont_Target* dest, float x, float y, Uint16 width, const Scale& scale, const char* formatted_text, ...) {
	if (formatted_text == NULL) {
		return Rectf(x, y, 0, 0);
	}

	va_list lst;
	va_start(lst, formatted_text);
	vsprintf(buffer, formatted_text, lst);
	va_end(lst);

	return FC_DrawColumnScale(font, dest, x, y, width, FC_MakeScale(scale.x, scale.y), "%s", buffer);
}

NFont::Rectf NFont::drawColumn(NFont_Target* dest, float x, float y, Uint16 width, const Color& color, const char* formatted_text, ...) {
	if (formatted_text == NULL) {
		return Rectf(x, y, 0, 0);
	}

	va_list lst;
	va_start(lst, formatted_text);
	vsprintf(buffer, formatted_text, lst);
	va_end(lst);

	return FC_DrawColumnColor(font, dest, x, y, width, color.to_SDL_Color(), "%s", buffer);
}

NFont::Rectf NFont::drawColumn(NFont_Target* dest, float x, float y, Uint16 width, const Effect& effect, const char* formatted_text, ...) {
	if (formatted_text == NULL) {
		return Rectf(x, y, 0, 0);
	}

	va_list lst;
	va_start(lst, formatted_text);
	vsprintf(buffer, formatted_text, lst);
	va_end(lst);

#ifdef NFONT_USE_SDL_GPU
	return FC_DrawColumnEffect(font, dest, x, y, width, FC_MakeEffect(translate_enum_NFont_to_FC(effect.alignment), FC_MakeScale(effect.scale.x, effect.scale.y), effect.color.to_SDL_Color()), "%s", buffer);
#else
	return FC_DrawColumnEffect(font, dest, x, y, width, FC_MakeEffect(translate_enum_NFont_to_FC(effect.alignment), FC_MakeScale(effect.scale.x, effect.scale.y), effect.color.to_SDL_Color()), "%s", buffer);
#endif
}



NFont::Rectf NFont::draw(NFont_Target* dest, float x, float y, const Scale& scale, const char* formatted_text, ...) {
	if (formatted_text == NULL) {
		return Rectf(x, y, 0, 0);
	}

	va_list lst;
	va_start(lst, formatted_text);
	vsprintf(buffer, formatted_text, lst);
	va_end(lst);

	return FC_DrawScale(font, dest, x, y, FC_MakeScale(scale.x, scale.y), "%s", buffer);
}

NFont::Rectf NFont::draw(NFont_Target* dest, float x, float y, AlignEnum align, const char* formatted_text, ...) {
	if (formatted_text == NULL) {
		return Rectf(x, y, 0, 0);
	}

	va_list lst;
	va_start(lst, formatted_text);
	vsprintf(buffer, formatted_text, lst);
	va_end(lst);

	return FC_DrawAlign(font, dest, x, y, translate_enum_NFont_to_FC(align), "%s", buffer);
}

NFont::Rectf NFont::draw(NFont_Target* dest, float x, float y, const Color& color, const char* formatted_text, ...) {
	if (formatted_text == NULL) {
		return Rectf(x, y, 0, 0);
	}

	va_list lst;
	va_start(lst, formatted_text);
	vsprintf(buffer, formatted_text, lst);
	va_end(lst);

	return FC_DrawColor(font, dest, x, y, color.to_SDL_Color(), "%s", buffer);
}


NFont::Rectf NFont::draw(NFont_Target* dest, float x, float y, const Effect& effect, const char* formatted_text, ...) {
	if (formatted_text == NULL) {
		return Rectf(x, y, 0, 0);
	}

	va_list lst;
	va_start(lst, formatted_text);
	vsprintf(buffer, formatted_text, lst);
	va_end(lst);

	return FC_DrawEffect(font, dest, x, y, FC_MakeEffect(translate_enum_NFont_to_FC(effect.alignment), FC_MakeScale(effect.scale.x, effect.scale.y), effect.color.to_SDL_Color()), "%s", buffer);
}




// Getters


NFont::FilterEnum NFont::getFilterMode() const {
	FC_FilterEnum f = FC_GetFilterMode(font);
	if (f == FC_FILTER_LINEAR) {
		return NFont::LINEAR;
	}
	return NFont::NEAREST;
}

Uint16 NFont::getHeight() const {
	return FC_GetLineHeight(font);
}

Uint16 NFont::getHeight(const char* formatted_text, ...) const {
	if (formatted_text == NULL) {
		return 0;
	}

	va_list lst;
	va_start(lst, formatted_text);
	vsprintf(buffer, formatted_text, lst);
	va_end(lst);

	return FC_GetHeight(font, "%s", buffer);
}

Uint16 NFont::getWidth(const char* formatted_text, ...) {
	if (formatted_text == NULL) {
		return 0;
	}

	va_list lst;
	va_start(lst, formatted_text);
	vsprintf(buffer, formatted_text, lst);
	va_end(lst);

	return FC_GetWidth(font, "%s", buffer);
}


NFont::Rectf NFont::getCharacterOffset(Uint16 position_index, int column_width, const char* formatted_text, ...) {
	if (formatted_text == NULL) {
		return Rectf(0,0,0,0);
	}

	va_list lst;
	va_start(lst, formatted_text);
	vsprintf(buffer, formatted_text, lst);
	va_end(lst);

	return FC_GetCharacterOffset(font, position_index, column_width, "%s", buffer);
}


Uint16 NFont::getColumnHeight(Uint16 width, const char* formatted_text, ...) {
	if (formatted_text == NULL || width == 0) {
		return 0;
	}

	va_list lst;
	va_start(lst, formatted_text);
	vsprintf(buffer, formatted_text, lst);
	va_end(lst);

	return FC_GetColumnHeight(font, width, "%s", buffer);
}

int NFont::getAscent(const char character) {
	return FC_GetAscent(font, "%c", character);
}

int NFont::getAscent() const {
	return FC_GetAscent(font, NULL);
}

int NFont::getAscent(const char* formatted_text, ...) {
	if (formatted_text == NULL) {
		return FC_GetAscent(font, NULL);
	}

	va_list lst;
	va_start(lst, formatted_text);
	vsprintf(buffer, formatted_text, lst);
	va_end(lst);

	return FC_GetAscent(font, "%s", buffer);
}

int NFont::getDescent(const char character) {
	return FC_GetDescent(font, "%c", character);
}

int NFont::getDescent() const {
	return FC_GetDescent(font, NULL);
}

int NFont::getDescent(const char* formatted_text, ...) {
	if (formatted_text == NULL) {
		return FC_GetDescent(font, NULL);
	}

	va_list lst;
	va_start(lst, formatted_text);
	vsprintf(buffer, formatted_text, lst);
	va_end(lst);

	return FC_GetDescent(font, "%s", buffer);
}

int NFont::getSpacing() const {
	return FC_GetSpacing(font);
}

int NFont::getLineSpacing() const {
	return FC_GetLineSpacing(font);
}

Uint16 NFont::getBaseline() const {
	return FC_GetBaseline(font);
}

Uint16 NFont::getMaxWidth() const {
	return FC_GetMaxWidth(font);
}

NFont::Color NFont::getDefaultColor() const {
	return FC_GetDefaultColor(font);
}





// Setters

void NFont::setFilterMode(NFont::FilterEnum filter) {
	if (filter == NFont::LINEAR) {
		FC_SetFilterMode(font, FC_FILTER_LINEAR);
	}
	else {
		FC_SetFilterMode(font, FC_FILTER_NEAREST);
	}
}

void NFont::setSpacing(int LetterSpacing) {
	FC_SetSpacing(font, LetterSpacing);
}

void NFont::setLineSpacing(int LineSpacing) {
	FC_SetLineSpacing(font, LineSpacing);
}

void NFont::setBaseline() {

}

void NFont::setBaseline(Uint16 Baseline) {

}

void NFont::setDefaultColor(const Color& color) {
	FC_SetDefaultColor(font, color.to_SDL_Color());
}

void NFont::enableTTFOwnership() {

}




