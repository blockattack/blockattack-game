
// Define this here or in your project settings if you want to use the SDL_ttf features.
#define NFONT_USE_TTF

/*
NFont v2.0.0: A bitmap font class for SDL
by Jonathan Dearborn 2-4-10
(class originally adapted from Florian Hufsky)

Requires:
    SDL ("SDL.h") [www.libsdl.org]

Optionally Requires:
    SDL_ttf ("SDL_ttf.h") [www.libsdl.org]

Notes:
    NFont is a bitmap font class with text-block alignment, full
    support for the newline character ('\n'), animation, and extended ASCII 
    support.  It accepts SDL_Surfaces so that any image format you can load 
    can be used as an NFont.
    
    NFont has the ability to animate the font in two ways: It's position or 
    it's everything.  By using drawPos(), you can use a function you create 
    to handle the final positions of the drawn characters.  With drawAll(), 
    you can handle everything that the font does (please use my 
    drawToSurface() function as a reference).

    Internally, NFont uses a pointer (SDL_Surface*) to handle the destination
    surface.  You have to set the destination before the font can be used.  Be 
    aware that you will need to use setDest() if you replace the memory that 
    it points to (like when using screen = SDL_SetVideoMode()).
    
    NFont can use standard SFont bitmaps or extended bitmaps.  The standard bitmaps
    have the following characters (ASCII 33-126) separated by pink (255, 0, 255) pixels in the topmost
    row:
    ! " # $ % & ' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [ \ ] ^ _ ` a b c d e f g h i j k l m n o p q r s t u v w x y z { | } ~
    
    And the extended bitmaps have these (ASCII 161-255):
    ! " # $ % & ' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F G H I J K L M N O P Q R S T U V W X Y Z [ \ ] ^ _ ` a b c d e f g h i j k l m n o p q r s t u v w x y z { | } ~ � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � � �
    
    NFont can also load SDL_ttf fonts.  Define NFONT_USE_TTF before including 
    NFont.h to use the flexibility of NFont with TrueType fonts.

    If you come up with something cool using NFont, I'd love to hear about it.
    Any comments can be sent to GrimFang4 [at] gmail [dot] com

License:
    The short:
    Use it however you'd like, but keep the copyright and license notice 
    whenever these files or parts of them are distributed in uncompiled form.
    
    The long:
Copyright (c) 2010 Jonathan Dearborn

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
#include "stdarg.h"

#ifdef NFONT_USE_TTF
    #include "SDL_ttf.h"
#endif

class NFont
{
public:

    // Nested struct
    struct AnimData
    {
        const NFont* font;
        
        SDL_Surface* dest;
        SDL_Surface* src;
        char* text;  // Buffer for efficient drawing
        int height;
        const int* charPos;
        const int* charWidth;
        int maxX;
        
        int index;
        int letterNum;
        int wordNum;
        int lineNum;
        int startX;
        int startY;
        void* userVar;
        
        SDL_Rect dirtyRect;
    };
    
    // Function pointer
    typedef void (*AnimFn)(int&, int&, AnimData&);
    
protected:
    

    SDL_Surface* src;  // bitmap source of characters
    SDL_Surface* dest; // Destination to blit to

    int height;

    int maxWidth;
    int baseline;
    int ascent;
    int descent;

    int lineSpacing;
    int letterSpacing;

    int charPos[256];
    int charWidth[256];
    int maxPos;
    
    void init();

    SDL_Rect drawToSurface(int x, int y, const char* text) const;
    SDL_Rect drawToSurfacePos(int x, int y, NFont::AnimFn posFn) const;
    
    // Static variables
    static char* buffer;  // Buffer for efficient drawing
    static AnimData data;  // Data is wrapped in a struct so it can all be passed to 
                                 // the function pointers for animation

public:
    
    // Static functions
    static char* copyString(const char* c);
    static Uint32 getPixel(SDL_Surface *Surface, int x, int y);
    static inline SDL_Rect makeRect(Sint16 x, Sint16 y, Uint16 w, Uint16 h)
    {
        SDL_Rect r = {x, y, w, h};
        return r;
    }
    static SDL_Rect rectUnion(const SDL_Rect& A, const SDL_Rect& B);
    static SDL_Surface* copySurface(SDL_Surface *Surface);
    static SDL_Surface* verticalGradient(SDL_Surface* targetSurface, Uint32 topColor, Uint32 bottomColor, int heightAdjust = 0);
    
    // Static accessors
    static void setAnimData(void* data);
    static void setBuffer(unsigned int size);
    
    // Constructors
    NFont();
    NFont(SDL_Surface* src);
    NFont(SDL_Surface* dest, SDL_Surface* src);
    #ifdef NFONT_USE_TTF
        NFont(TTF_Font* ttf, SDL_Color fg);  // Alpha bg
        NFont(TTF_Font* ttf, SDL_Color fg, SDL_Color bg);
        NFont(const char* filename_ttf, Uint32 pointSize, SDL_Color fg, int style = TTF_STYLE_NORMAL);  // Alpha bg
        NFont(const char* filename_ttf, Uint32 pointSize, SDL_Color fg, SDL_Color bg, int style = TTF_STYLE_NORMAL);
        NFont(SDL_Surface* dest, TTF_Font* ttf, SDL_Color fg);  // Alpha bg
        NFont(SDL_Surface* dest, TTF_Font* ttf, SDL_Color fg, SDL_Color bg);
        NFont(SDL_Surface* dest, const char* filename_ttf, Uint32 pointSize, SDL_Color fg, int style = TTF_STYLE_NORMAL);  // Alpha bg
        NFont(SDL_Surface* dest, const char* filename_ttf, Uint32 pointSize, SDL_Color fg, SDL_Color bg, int style = TTF_STYLE_NORMAL);
    #endif

    ~NFont();

    // Loading
    bool load(SDL_Surface* FontSurface);
    bool load(SDL_Surface* destSurface, SDL_Surface* FontSurface);
    #ifdef NFONT_USE_TTF
        bool load(TTF_Font* ttf, SDL_Color fg);  // Alpha bg
        bool load(TTF_Font* ttf, SDL_Color fg, SDL_Color bg);
        bool load(const char* filename_ttf, Uint32 pointSize, SDL_Color fg, int style = TTF_STYLE_NORMAL);  // Alpha bg
        bool load(const char* filename_ttf, Uint32 pointSize, SDL_Color fg, SDL_Color bg, int style = TTF_STYLE_NORMAL);
    #endif

    void freeSurface();

    // Drawing
    SDL_Rect draw(int x, int y, const char* formatted_text, ...) const;
    SDL_Rect drawCenter(int x, int y, const char* formatted_text, ...) const;
    SDL_Rect drawRight(int x, int y, const char* formatted_text, ...) const;
    SDL_Rect drawPos(int x, int y, NFont::AnimFn posFn, const char* text, ...) const;
    SDL_Rect drawAll(int x, int y, NFont::AnimFn allFn, const char* text, ...) const;
    
    // Getters
    SDL_Surface* getDest() const;
    SDL_Surface* getSurface() const;
    int getHeight(const char* formatted_text = NULL, ...) const;
    int getWidth(const char* formatted_text, ...) const;
    int getSpacing() const;
    int getLineSpacing() const;
    int getBaseline() const;
    int getAscent(const char character) const;
    int getAscent(const char* formatted_text = NULL, ...) const;
    int getDescent(const char character) const;
    int getDescent(const char* formatted_text = NULL, ...) const;
    int getMaxWidth() const;
    
    // Setters
    void setDest(SDL_Surface* Dest);
    void setSpacing(int LetterSpacing);
    void setLineSpacing(int LineSpacing);
    int setBaseline(int Baseline = -1);

};



#endif // _NFONT_H__
