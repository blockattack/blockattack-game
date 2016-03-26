/*
NFont v4.0.0: A font class for SDL and SDL_Renderer
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

#ifndef _NFONT_H__
#define _NFONT_H__

#include "SDL.h"

#if defined(FC_USE_SDL_GPU) && !defined(NFONT_USE_SDL_GPU)
#define NFONT_USE_SDL_GPU
#endif

#ifdef NFONT_USE_SDL_GPU
    #include "SDL_gpu.h"
#endif

#ifndef NFONT_FORMAT

#if ( (defined(__GNUC__) && (__GNUC__ >= 4)) || defined(__clang__) )
#define NFONT_FORMAT(X) __attribute__ ((format (printf, X, X+1)))
#else
#define NFONT_FORMAT(X)
#endif

#endif 

#include "stdarg.h"

// Let's pretend this exists...
#ifndef TTF_STYLE_OUTLINE
    #define TTF_STYLE_OUTLINE	16
#endif

struct FC_Font;

typedef struct _TTF_Font TTF_Font;

#if defined(NFONT_DLL) || defined(NFONT_DLL_EXPORT)
	#ifdef NFONT_DLL_EXPORT
	#define NFONT_EXPORT __declspec(dllexport)
	#else
	#define NFONT_EXPORT __declspec(dllimport)
	#endif
#else
	#define NFONT_EXPORT
#endif

class NFONT_EXPORT NFont
{
  public:

	class NFONT_EXPORT Color
    {
        public:
        
        Uint8 r, g, b, a;
        
        Color();
        Color(Uint8 r, Uint8 g, Uint8 b);
        Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
        Color(const SDL_Color& color);
        
        Color& rgb(Uint8 R, Uint8 G, Uint8 B);
        Color& rgba(Uint8 R, Uint8 G, Uint8 B, Uint8 A);
        Color& color(const SDL_Color& color);
        
        SDL_Color to_SDL_Color() const;
    };
    
	class NFONT_EXPORT Rectf
    {
        public:
        float x, y;
        float w, h;
        
        Rectf();
        Rectf(float x, float y);
        Rectf(float x, float y, float w, float h);
        Rectf(const SDL_Rect& rect);
        
        SDL_Rect to_SDL_Rect() const;
        
        #ifdef NFONT_USE_SDL_GPU
        Rectf(const GPU_Rect& rect);
        GPU_Rect to_GPU_Rect() const;
        #endif
    };

    
    enum AlignEnum {LEFT, CENTER, RIGHT};
    enum FilterEnum {NEAREST, LINEAR};
    
	class NFONT_EXPORT Scale
    {
        public:
        
        float x;
        float y;
        
        enum ScaleTypeEnum {NEAREST};
        ScaleTypeEnum type;
        
        Scale()
            : x(1.0f), y(1.0f), type(NEAREST)
        {}
        Scale(float xy)
            : x(xy), y(xy), type(NEAREST)
        {}
        Scale(float xy, ScaleTypeEnum type)
            : x(xy), y(xy), type(type)
        {}
        Scale(float x, float y)
            : x(x), y(y), type(NEAREST)
        {}
        Scale(float x, float y, ScaleTypeEnum type)
            : x(x), y(y), type(type)
        {}
    };
    
	class NFONT_EXPORT Effect
    {
        public:
        AlignEnum alignment;
        Scale scale;
        bool use_color;
        Color color;
        
        Effect()
            : alignment(LEFT), use_color(false), color(255, 255, 255, 255)
        {}
        
        Effect(const Scale& scale)
            : alignment(LEFT), scale(scale), use_color(false), color(255, 255, 255, 255)
        {}
        Effect(AlignEnum alignment)
            : alignment(alignment), use_color(false), color(255, 255, 255, 255)
        {}
        Effect(const Color& color)
            : alignment(LEFT), use_color(true), color(color)
        {}
        
        Effect(AlignEnum alignment, const Scale& scale)
            : alignment(alignment), scale(scale), use_color(false), color(255, 255, 255, 255)
        {}
        Effect(AlignEnum alignment, const Color& color)
            : alignment(alignment), use_color(true), color(color)
        {}
        Effect(const Scale& scale, const Color& color)
            : alignment(LEFT), scale(scale), use_color(true), color(color)
        {}
        Effect(AlignEnum alignment, const Scale& scale, const Color& color)
            : alignment(alignment), scale(scale), use_color(true), color(color)
        {}
    };
    
    
    // Constructors
    NFont();
    NFont(const NFont& font);
    #ifdef NFONT_USE_SDL_GPU
    NFont(SDL_Surface* src);
    NFont(TTF_Font* ttf);
    NFont(TTF_Font* ttf, const NFont::Color& color);
    NFont(const char* filename_ttf, Uint32 pointSize);
    NFont(const char* filename_ttf, Uint32 pointSize, const NFont::Color& color, int style = 0);
    NFont(SDL_RWops* file_rwops_ttf, Uint8 own_rwops, Uint32 pointSize, const NFont::Color& color, int style = 0);
    #else
    NFont(SDL_Renderer* renderer, SDL_Surface* src);
    NFont(SDL_Renderer* renderer, TTF_Font* ttf);
    NFont(SDL_Renderer* renderer, TTF_Font* ttf, const NFont::Color& color);
    NFont(SDL_Renderer* renderer, const char* filename_ttf, Uint32 pointSize);
    NFont(SDL_Renderer* renderer, const char* filename_ttf, Uint32 pointSize, const NFont::Color& color, int style = 0);
    NFont(SDL_Renderer* renderer, SDL_RWops* file_rwops_ttf, Uint8 own_rwops, Uint32 pointSize, const NFont::Color& color, int style = 0);
    #endif
    
    ~NFont();
    
    NFont& operator=(const NFont& font);

    // Loading
    void setLoadingString(const char* str);
    
    #ifdef NFONT_USE_SDL_GPU
    bool load(SDL_Surface* FontSurface);
    bool load(TTF_Font* ttf);
    bool load(TTF_Font* ttf, const NFont::Color& color);
    bool load(const char* filename_ttf, Uint32 pointSize);
    bool load(const char* filename_ttf, Uint32 pointSize, const NFont::Color& color, int style = 0);
    bool load(SDL_RWops* file_rwops_ttf, Uint8 own_rwops, Uint32 pointSize, const NFont::Color& color, int style = 0);
    #else
    bool load(SDL_Renderer* renderer, SDL_Surface* FontSurface);
    bool load(SDL_Renderer* renderer, TTF_Font* ttf);
    bool load(SDL_Renderer* renderer, TTF_Font* ttf, const NFont::Color& color);
    bool load(SDL_Renderer* renderer, const char* filename_ttf, Uint32 pointSize);
    bool load(SDL_Renderer* renderer, const char* filename_ttf, Uint32 pointSize, const NFont::Color& color, int style = 0);
    bool load(SDL_Renderer* renderer, SDL_RWops* file_rwops_ttf, Uint8 own_rwops, Uint32 pointSize, const NFont::Color& color, int style = 0);
    #endif
    
    void free();

    // Drawing
    #ifdef NFONT_USE_SDL_GPU
    Rectf draw(GPU_Target* dest, float x, float y, const char* formatted_text, ...) NFONT_FORMAT(5);
    Rectf draw(GPU_Target* dest, float x, float y, AlignEnum align, const char* formatted_text, ...) NFONT_FORMAT(6);
    Rectf draw(GPU_Target* dest, float x, float y, const Scale& scale, const char* formatted_text, ...) NFONT_FORMAT(6);
    Rectf draw(GPU_Target* dest, float x, float y, const Color& color, const char* formatted_text, ...) NFONT_FORMAT(6);
    Rectf draw(GPU_Target* dest, float x, float y, const Effect& effect, const char* formatted_text, ...) NFONT_FORMAT(6);
    
    Rectf drawBox(GPU_Target* dest, const Rectf& box, const char* formatted_text, ...) NFONT_FORMAT(4);
    Rectf drawBox(GPU_Target* dest, const Rectf& box, AlignEnum align, const char* formatted_text, ...) NFONT_FORMAT(5);
    Rectf drawBox(GPU_Target* dest, const Rectf& box, const Scale& scale, const char* formatted_text, ...) NFONT_FORMAT(5);
    Rectf drawBox(GPU_Target* dest, const Rectf& box, const Color& color, const char* formatted_text, ...) NFONT_FORMAT(5);
    Rectf drawBox(GPU_Target* dest, const Rectf& box, const Effect& effect, const char* formatted_text, ...) NFONT_FORMAT(5);
    
    Rectf drawColumn(GPU_Target* dest, float x, float y, Uint16 width, const char* formatted_text, ...) NFONT_FORMAT(6);
    Rectf drawColumn(GPU_Target* dest, float x, float y, Uint16 width, AlignEnum align, const char* formatted_text, ...) NFONT_FORMAT(7);
    Rectf drawColumn(GPU_Target* dest, float x, float y, Uint16 width, const Scale& scale, const char* formatted_text, ...) NFONT_FORMAT(7);
    Rectf drawColumn(GPU_Target* dest, float x, float y, Uint16 width, const Color& color, const char* formatted_text, ...) NFONT_FORMAT(7);
    Rectf drawColumn(GPU_Target* dest, float x, float y, Uint16 width, const Effect& effect, const char* formatted_text, ...) NFONT_FORMAT(7);
    #else
    Rectf draw(SDL_Renderer* dest, float x, float y, const char* formatted_text, ...) NFONT_FORMAT(5);
    Rectf draw(SDL_Renderer* dest, float x, float y, AlignEnum align, const char* formatted_text, ...) NFONT_FORMAT(6);
    Rectf draw(SDL_Renderer* dest, float x, float y, const Scale& scale, const char* formatted_text, ...) NFONT_FORMAT(6);
    Rectf draw(SDL_Renderer* dest, float x, float y, const Color& color, const char* formatted_text, ...) NFONT_FORMAT(6);
    Rectf draw(SDL_Renderer* dest, float x, float y, const Effect& effect, const char* formatted_text, ...) NFONT_FORMAT(6);
    
    Rectf drawBox(SDL_Renderer* dest, const Rectf& box, const char* formatted_text, ...) NFONT_FORMAT(4);
    Rectf drawBox(SDL_Renderer* dest, const Rectf& box, AlignEnum align, const char* formatted_text, ...) NFONT_FORMAT(5);
    Rectf drawBox(SDL_Renderer* dest, const Rectf& box, const Scale& scale, const char* formatted_text, ...) NFONT_FORMAT(5);
    Rectf drawBox(SDL_Renderer* dest, const Rectf& box, const Color& color, const char* formatted_text, ...) NFONT_FORMAT(5);
    Rectf drawBox(SDL_Renderer* dest, const Rectf& box, const Effect& effect, const char* formatted_text, ...) NFONT_FORMAT(5);
    
    Rectf drawColumn(SDL_Renderer* dest, float x, float y, Uint16 width, const char* formatted_text, ...) NFONT_FORMAT(6);
    Rectf drawColumn(SDL_Renderer* dest, float x, float y, Uint16 width, AlignEnum align, const char* formatted_text, ...) NFONT_FORMAT(7);
    Rectf drawColumn(SDL_Renderer* dest, float x, float y, Uint16 width, const Scale& scale, const char* formatted_text, ...) NFONT_FORMAT(7);
    Rectf drawColumn(SDL_Renderer* dest, float x, float y, Uint16 width, const Color& color, const char* formatted_text, ...) NFONT_FORMAT(7);
    Rectf drawColumn(SDL_Renderer* dest, float x, float y, Uint16 width, const Effect& effect, const char* formatted_text, ...) NFONT_FORMAT(7);
    #endif
    
    // Getters
    #ifdef NFONT_USE_SDL_GPU
    GPU_Image* getImage() const;
    #else
    SDL_Texture* getImage() const;
    #endif
    SDL_Surface* getSurface() const;
    FilterEnum getFilterMode() const;
    Uint16 getHeight() const;
    Uint16 getHeight(const char* formatted_text, ...) const NFONT_FORMAT(2);
    Uint16 getWidth(const char* formatted_text, ...) NFONT_FORMAT(2);
    Rectf getCharacterOffset(Uint16 position_index, int column_width, const char* formatted_text, ...) NFONT_FORMAT(4);
    Uint16 getPositionFromOffset(float x, float y, int column_width, NFont::AlignEnum align, const char* formatted_text, ...) NFONT_FORMAT(6);
    Uint16 getColumnHeight(Uint16 width, const char* formatted_text, ...) NFONT_FORMAT(3);
    int getSpacing() const;
    int getLineSpacing() const;
    Uint16 getBaseline() const;
    int getAscent() const;
    int getAscent(const char character);
    int getAscent(const char* formatted_text, ...) NFONT_FORMAT(2);
    int getDescent() const;
    int getDescent(const char character);
    int getDescent(const char* formatted_text, ...) NFONT_FORMAT(2);
    Uint16 getMaxWidth() const;
    Color getDefaultColor() const;
    
    // Setters
    void setFilterMode(FilterEnum filter);
    void setSpacing(int LetterSpacing);
    void setLineSpacing(int LineSpacing);
    void setBaseline();
    void setBaseline(Uint16 Baseline);
    void setDefaultColor(const Color& color);
    
    void enableTTFOwnership();
    
  private:
    
    static char* buffer;
    FC_Font* font;
    
    void init();  // Common constructor

};



#endif // _NFONT_H__
