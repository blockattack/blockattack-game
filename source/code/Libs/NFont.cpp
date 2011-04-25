/*
NFont v2.0.0: A bitmap font class for SDL
by Jonathan Dearborn 2-4-10
(class adapted from Florian Hufsky)

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
#include "NFont.h"

char* NFont::buffer = NULL;
NFont::AnimData NFont::data;

#define MIN(a,b) (a < b? a : b)
#define MAX(a,b) (a > b? a : b)

// Static setters
void NFont::setAnimData(void* data)
{
    NFont::data.userVar = data;
}

void NFont::setBuffer(unsigned int size)
{
    delete[] buffer;
    if(size > 0)
        buffer = new char[size];
    else
        buffer = new char[1024];
}


// Static functions
char* NFont::copyString(const char* c)
{
    if(c == NULL) return NULL;

    int count = 0;
    for(; c[count] != '\0'; count++);

    char* result = new char[count+1];

    for(int i = 0; i < count; i++)
    {
        result[i] = c[i];
    }

    result[count] = '\0';
    return result;
}

Uint32 NFont::getPixel(SDL_Surface *Surface, int x, int y)  // No Alpha?
{
    Uint8* bits;
    Uint32 bpp;

    if(x < 0 || x >= Surface->w)
        return 0;  // Best I could do for errors

    bpp = Surface->format->BytesPerPixel;
    bits = ((Uint8*)Surface->pixels) + y*Surface->pitch + x*bpp;

    switch (bpp)
    {
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

    return 0;  // Best I could do for errors
}

SDL_Rect NFont::rectUnion(const SDL_Rect& A, const SDL_Rect& B)
{
    Sint16 x,x2,y,y2;
    x = MIN(A.x, B.x);
    y = MIN(A.y, B.y);
    x2 = MAX(A.x+A.w, B.x+B.w);
    y2 = MAX(A.y+A.h, B.y+B.h);
    SDL_Rect result = {x, y, x2 - x, y2 - y};
    return result;
}

SDL_Surface* NFont::copySurface(SDL_Surface *Surface)
{
    return SDL_ConvertSurface(Surface, Surface->format, Surface->flags);
}

SDL_Surface* NFont::verticalGradient(SDL_Surface* targetSurface, Uint32 top, Uint32 bottom, int heightAdjust)
{
    SDL_Surface* surface = targetSurface;
    if(surface == NULL)
        return NULL;
    
    Uint8 tr, tg, tb;

    SDL_GetRGB(top, surface->format, &tr, &tg, &tb);

    Uint8 br, bg, bb;

    SDL_GetRGB(bottom, surface->format, &br, &bg, &bb);

    bool useCK = (surface->flags & SDL_SRCALPHA) != SDL_SRCALPHA;  // colorkey if no alpha
    Uint32 colorkey = surface->format->colorkey;

    Uint8 r, g, b, a;
    float ratio;
    Uint32 color;
    int temp;

    for (int x = 0, y = 0; y < surface->h; x++)
    {
        if (x >= surface->w)
        {
            x = 0;
            y++;

            if (y >= surface->h)
                break;
        }

        ratio = (y - 2)/float(surface->h - heightAdjust);  // the neg 3s are for full color at top and bottom

        if(!useCK)
        {
            color = getPixel(surface, x, y);
            SDL_GetRGBA(color, surface->format, &r, &g, &b, &a);  // just getting alpha
        }
        else
            a = SDL_ALPHA_OPAQUE;

        // Get and clamp the new values
        temp = int(tr*(1-ratio) + br*ratio);
        r = temp < 0? 0 : temp > 255? 255 : temp;

        temp = int(tg*(1-ratio) + bg*ratio);
        g = temp < 0? 0 : temp > 255? 255 : temp;

        temp = int(tb*(1-ratio) + bb*ratio);
        b = temp < 0? 0 : temp > 255? 255 : temp;


        color = SDL_MapRGBA(surface->format, r, g, b, a);


        if(useCK)
        {
            if(getPixel(surface, x, y) == colorkey)
                continue;
            if(color == colorkey)
                color == 0? color++ : color--;
        }

        // make sure it isn't pink
        if(color == SDL_MapRGBA(surface->format, 0xFF, 0, 0xFF, a))
            color--;
        if(getPixel(surface, x, y) == SDL_MapRGBA(surface->format, 0xFF, 0, 0xFF, SDL_ALPHA_OPAQUE))
            continue;

        int bpp = surface->format->BytesPerPixel;
        Uint8* bits = ((Uint8 *)surface->pixels) + y*surface->pitch + x*bpp;

        /* Set the pixel */
        switch(bpp) {
            case 1:
                *((Uint8 *)(bits)) = (Uint8)color;
                break;
            case 2:
                *((Uint16 *)(bits)) = (Uint16)color;
                break;
            case 3: { /* Format/endian independent */
                r = (color >> surface->format->Rshift) & 0xFF;
                g = (color >> surface->format->Gshift) & 0xFF;
                b = (color >> surface->format->Bshift) & 0xFF;
                *((bits)+surface->format->Rshift/8) = r;
                *((bits)+surface->format->Gshift/8) = g;
                *((bits)+surface->format->Bshift/8) = b;
                }
                break;
            case 4:
                *((Uint32 *)(bits)) = (Uint32)color;
                break;
        }

    }
    return surface;
}


// Constructors
NFont::NFont()
{
    init();
}

NFont::NFont(SDL_Surface* src)
{
    init();
    load(src);
}

NFont::NFont(SDL_Surface* dest, SDL_Surface* src)
{
    init();
    load(src);
    setDest(dest);
}

#ifdef NFONT_USE_TTF
NFont::NFont(TTF_Font* ttf, SDL_Color fg)
{
    init();
    load(ttf, fg);
}
NFont::NFont(TTF_Font* ttf, SDL_Color fg, SDL_Color bg)
{
    init();
    load(ttf, fg, bg);
}
NFont::NFont(const char* filename_ttf, Uint32 pointSize, SDL_Color fg, int style)
{
    init();
    load(filename_ttf, pointSize, fg, style);
}
NFont::NFont(const char* filename_ttf, Uint32 pointSize, SDL_Color fg, SDL_Color bg, int style)
{
    init();
    load(filename_ttf, pointSize, fg, bg, style);
}

NFont::NFont(SDL_Surface* dest, TTF_Font* ttf, SDL_Color fg)
{
    init();
    load(ttf, fg);
    setDest(dest);
}
NFont::NFont(SDL_Surface* dest, TTF_Font* ttf, SDL_Color fg, SDL_Color bg)
{
    init();
    load(ttf, fg, bg);
    setDest(dest);
}
NFont::NFont(SDL_Surface* dest, const char* filename_ttf, Uint32 pointSize, SDL_Color fg, int style)
{
    init();
    load(filename_ttf, pointSize, fg, style);
    setDest(dest);
}
NFont::NFont(SDL_Surface* dest, const char* filename_ttf, Uint32 pointSize, SDL_Color fg, SDL_Color bg, int style)
{
    init();
    load(filename_ttf, pointSize, fg, bg, style);
    setDest(dest);
}
#endif

void NFont::init()
{
    src = NULL;
    dest = NULL;

    maxPos = 0;

    height = 0; // ascent+descent

    maxWidth = 0;
    baseline = 0;
    ascent = 0;
    descent = 0;

    lineSpacing = 0;
    letterSpacing = 0;
    
    if(buffer == NULL)
        buffer = new char[1024];
}

NFont::~NFont()
{}


// Loading
bool NFont::load(SDL_Surface* FontSurface)
{
    src = FontSurface;
    if (src == NULL)
    {
        printf("\n ERROR: NFont given a NULL surface\n");
        return false;
    }

    int x = 1, i = 0;
    
    // memset would be faster
    for(int j = 0; j < 256; j++)
    {
        charWidth[j] = 0;
        charPos[j] = 0;
    }

    SDL_LockSurface(src);

    Uint32 pixel = SDL_MapRGB(src->format, 255, 0, 255); // pink pixel
    
    maxWidth = 0;
    
    // Get the character positions and widths
    while (x < src->w)
    {
        if(getPixel(src, x, 0) != pixel)
        {
            charPos[i] = x;
            charWidth[i] = x;
            while(x < src->w && getPixel(src, x, 0) != pixel)
                x++;
            charWidth[i] = x - charWidth[i];
            if(charWidth[i] > maxWidth)
                maxWidth = charWidth[i];
            i++;
        }

        x++;
    }

    maxPos = x - 1;


    pixel = getPixel(src, 0, src->h - 1);
    int j;
    setBaseline();
    
    // Get the max ascent
    j = 1;
    while(j < baseline && j < src->h)
    {
        x = 0;
        while(x < src->w)
        {
            if(getPixel(src, x, j) != pixel)
            {
                ascent = baseline - j;
                j = src->h;
                break;
            }
            x++;
        }
        j++;
    }
    
    // Get the max descent
    j = src->h - 1;
    while(j > 0 && j > baseline)
    {
        x = 0;
        while(x < src->w)
        {
            if(getPixel(src, x, j) != pixel)
            {
                descent = j - baseline+1;
                j = 0;
                break;
            }
            x++;
        }
        j--;
    }
    
    
    height = ascent + descent;
    

    if((src->flags & SDL_SRCALPHA) != SDL_SRCALPHA)
    {
        pixel = getPixel(src, 0, src->h - 1);
        SDL_UnlockSurface(src);
        SDL_SetColorKey(src, SDL_SRCCOLORKEY, pixel);
    }
    else
        SDL_UnlockSurface(src);

    return true;
}

bool NFont::load(SDL_Surface* destSurface, SDL_Surface* FontSurface)
{
    setDest(destSurface);
    return load(FontSurface);
}

#ifdef NFONT_USE_TTF
bool NFont::load(TTF_Font* ttf, SDL_Color fg, SDL_Color bg)
{
    if(ttf == NULL)
        return false;
    SDL_Surface* surfs[127 - 33];
    int width = 0;
    int height = 0;
    
    char buff[2];
    buff[1] = '\0';
    for(int i = 0; i < 127 - 33; i++)
    {
        buff[0] = i + 33;
        surfs[i] = TTF_RenderText_Shaded(ttf, buff, fg, bg);
        width += surfs[i]->w;
        height = (height < surfs[i]->h)? surfs[i]->h : height;
    }
    
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        SDL_Surface* result = SDL_CreateRGBSurface(SDL_SWSURFACE,width + 127 - 33 + 1,height,24, 0xFF0000, 0x00FF00, 0x0000FF, 0);
    #else
        SDL_Surface* result = SDL_CreateRGBSurface(SDL_SWSURFACE,width + 127 - 33 + 1,height,24, 0x0000FF, 0x00FF00, 0xFF0000, 0);
    #endif
    Uint32 pink = SDL_MapRGB(result->format, 255, 0, 255);
    Uint32 bgcolor = SDL_MapRGB(result->format, bg.r, bg.g, bg.b);
    
    SDL_Rect pixel = {1, 0, 1, 1};
    SDL_Rect line = {1, 0, 1, result->h};
    
    int x = 1;
    SDL_Rect dest = {x, 0, 0, 0};
    for(int i = 0; i < 127 - 33; i++)
    {
        pixel.x = line.x = x-1;
        SDL_FillRect(result, &line, bgcolor);
        SDL_FillRect(result, &pixel, pink);
        
        SDL_BlitSurface(surfs[i], NULL, result, &dest);
        
        x += surfs[i]->w + 1;
        dest.x = x;
        
        SDL_FreeSurface(surfs[i]);
    }
    pixel.x = line.x = x-1;
    SDL_FillRect(result, &line, bgcolor);
    SDL_FillRect(result, &pixel, pink);
    
    return load(result);
}


bool NFont::load(TTF_Font* ttf, SDL_Color fg)
{
    if(ttf == NULL)
        return false;
    SDL_Surface* surfs[127 - 33];
    int width = 0;
    int height = 0;
    
    char buff[2];
    buff[1] = '\0';
    for(int i = 0; i < 127 - 33; i++)
    {
        buff[0] = i + 33;
        surfs[i] = TTF_RenderText_Blended(ttf, buff, fg);
        width += surfs[i]->w;
        height = (height < surfs[i]->h)? surfs[i]->h : height;
    }
    
    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        SDL_Surface* result = SDL_CreateRGBSurface(SDL_SWSURFACE,width + 127 - 33 + 1,height,32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    #else
        SDL_Surface* result = SDL_CreateRGBSurface(SDL_SWSURFACE,width + 127 - 33 + 1,height,32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
    #endif
    Uint32 pink = SDL_MapRGBA(result->format, 255, 0, 255, SDL_ALPHA_OPAQUE);
    
    SDL_SetAlpha(result, 0, SDL_ALPHA_OPAQUE);
    
    SDL_Rect pixel = {1, 0, 1, 1};
    
    int x = 1;
    SDL_Rect dest = {x, 0, 0, 0};
    for(int i = 0; i < 127 - 33; i++)
    {
        pixel.x = x-1;
        SDL_FillRect(result, &pixel, pink);
        
        SDL_SetAlpha(surfs[i], 0, SDL_ALPHA_OPAQUE);
        SDL_BlitSurface(surfs[i], NULL, result, &dest);
        
        x += surfs[i]->w + 1;
        dest.x = x;
        
        SDL_FreeSurface(surfs[i]);
    }
    pixel.x = x-1;
    SDL_FillRect(result, &pixel, pink);
    
    SDL_SetAlpha(result, SDL_SRCALPHA, SDL_ALPHA_OPAQUE);
    
    return load(result);
}


bool NFont::load(const char* filename_ttf, Uint32 pointSize, SDL_Color fg, int style)
{
    if(!TTF_WasInit() && TTF_Init() < 0)
    {
        printf("Unable to initialize SDL_ttf: %s \n", TTF_GetError());
        return false;
    }
    
    TTF_Font* ttf = TTF_OpenFont(filename_ttf, pointSize);
    
    if(ttf == NULL)
    {
        printf("Unable to load TrueType font: %s \n", TTF_GetError());
        return false;
    }
    TTF_SetFontStyle(ttf, style);
    bool result = load(ttf, fg);
    TTF_CloseFont(ttf);
    return result;
}

bool NFont::load(const char* filename_ttf, Uint32 pointSize, SDL_Color fg, SDL_Color bg, int style)
{
    if(!TTF_WasInit() && TTF_Init() < 0)
    {
        printf("Unable to initialize SDL_ttf: %s \n", TTF_GetError());
        return false;
    }
    
    TTF_Font* ttf = TTF_OpenFont(filename_ttf, pointSize);
    
    if(ttf == NULL)
    {
        printf("Unable to load TrueType font: %s \n", TTF_GetError());
        return false;
    }
    TTF_SetFontStyle(ttf, style);
    bool result = load(ttf, fg, bg);
    TTF_CloseFont(ttf);
    return result;
}

#endif



void NFont::freeSurface()
{
    SDL_FreeSurface(src);
}



// Drawing
SDL_Rect NFont::drawToSurface(int x, int y, const char* text) const
{
    const char* c = text;
    unsigned char num;
    SDL_Rect srcRect, dstRect, copyS, copyD;
    data.dirtyRect = makeRect(x, y, 0, 0);
    
    if(c == NULL || src == NULL || dest == NULL)
        return data.dirtyRect;
    
    srcRect.y = baseline - ascent;
    srcRect.h = dstRect.h = height;
    dstRect.x = x;
    dstRect.y = y;
    
    int newlineX = x;
    
    for(; *c != '\0'; c++)
    {
        if(*c == '\n')
        {
            dstRect.x = newlineX;
            dstRect.y += height + lineSpacing;
            continue;
        }
        
        if (*c == ' ')
        {
            dstRect.x += charWidth[0] + letterSpacing;
            continue;
        }
        unsigned char ctest = (unsigned char)(*c);
        // Skip bad characters
        if(ctest < 33 || (ctest > 126 && ctest < 161))
            continue;
        if(dstRect.x >= dest->w)
            continue;
        if(dstRect.y >= dest->h)
            continue;
        
        num = ctest - 33;  // Get array index
        if(num > 126) // shift the extended characters down to the correct index
            num -= 34;
        srcRect.x = charPos[num];
        srcRect.w = dstRect.w = charWidth[num];
        copyS = srcRect;
        copyD = dstRect;
        SDL_BlitSurface(src, &srcRect, dest, &dstRect);
        if(data.dirtyRect.w == 0 || data.dirtyRect.h == 0)
            data.dirtyRect = dstRect;
        else
            data.dirtyRect = rectUnion(data.dirtyRect, dstRect);
        srcRect = copyS;
        dstRect = copyD;
        
        dstRect.x += dstRect.w + letterSpacing;
    }
    
    return data.dirtyRect;
}

SDL_Rect NFont::drawToSurfacePos(int x, int y, NFont::AnimFn posFn) const
{
    data.font = this;
    data.dest = dest;
    data.src = src;
    data.text = buffer;  // Buffer for efficient drawing
    data.height = height;
    data.charPos = charPos;
    data.charWidth = charWidth;
    data.maxX = maxPos;
    data.dirtyRect = makeRect(x,y,0,0);

    data.index = -1;
    data.letterNum = 0;
    data.wordNum = 1;
    data.lineNum = 1;
    data.startX = x;  // used as reset value for line feed
    data.startY = y;
    
    int preFnX = x;
    int preFnY = y;
    
    const char* c = buffer;
    unsigned char num;
    SDL_Rect srcRect, dstRect, copyS, copyD;
    
    if(c == NULL || src == NULL || dest == NULL)
        return makeRect(x,y,0,0);
    
    srcRect.y = baseline - ascent;
    srcRect.h = dstRect.h = height;
    dstRect.x = x;
    dstRect.y = y;
    
    for(; *c != '\0'; c++)
    {
        data.index++;
        data.letterNum++;
        
        if(*c == '\n')
        {
            data.letterNum = 1;
            data.wordNum = 1;
            data.lineNum++;

            x = data.startX;  // carriage return
            y += height + lineSpacing;
            continue;
        }
        if (*c == ' ')
        {
            data.letterNum = 1;
            data.wordNum++;
            
            x += charWidth[0] + letterSpacing;
            continue;
        }
        unsigned char ctest = (unsigned char)(*c);
        // Skip bad characters
        if(ctest < 33 || (ctest > 126 && ctest < 161))
            continue;
        //if(x >= dest->w) // This shouldn't be used with position control
        //    continue;
        num = ctest - 33;
        if(num > 126) // shift the extended characters down to the array index
            num -= 34;
        srcRect.x = charPos[num];
        srcRect.w = dstRect.w = charWidth[num];
        
        preFnX = x;  // Save real position
        preFnY = y;

        // Use function pointer to get final x, y values
        posFn(x, y, data);
        
        dstRect.x = x;
        dstRect.y = y;
        
        copyS = srcRect;
        copyD = dstRect;
        SDL_BlitSurface(src, &srcRect, dest, &dstRect);
        if(data.dirtyRect.w == 0 || data.dirtyRect.h == 0)
            data.dirtyRect = dstRect;
        else
            data.dirtyRect = rectUnion(data.dirtyRect, dstRect);
        srcRect = copyS;
        dstRect = copyD;
        
        x = preFnX;  // Restore real position
        y = preFnY;
        
        x += dstRect.w + letterSpacing;
    }
    
    return data.dirtyRect;
}

SDL_Rect NFont::draw(int x, int y, const char* formatted_text, ...) const
{
    if(formatted_text == NULL)
        return makeRect(x, y, 0, 0);

    va_list lst;
    va_start(lst, formatted_text);
    vsprintf(buffer, formatted_text, lst);
    va_end(lst);

    return drawToSurface(x, y, buffer);
}

SDL_Rect NFont::drawCenter(int x, int y, const char* formatted_text, ...) const
{
    if(formatted_text == NULL)
        return makeRect(x, y, 0, 0);

    va_list lst;
    va_start(lst, formatted_text);
    vsprintf(buffer, formatted_text, lst);
    va_end(lst);

    char* str = copyString(buffer);
    char* del = str;

    // Go through str, when you find a \n, replace it with \0 and print it
    // then move down, back, and continue.
    for(char* c = str; *c != '\0';)
    {
        if(*c == '\n')
        {
            *c = '\0';
            drawToSurface(x - getWidth("%s", str)/2, y, str);
            *c = '\n';
            c++;
            str = c;
            y += height;
        }
        else
            c++;
    }
    char s[strlen(str)+1];
    strcpy(s, str);
    delete[] del;
    
    return drawToSurface(x - getWidth("%s", s)/2, y, s);
}

SDL_Rect NFont::drawRight(int x, int y, const char* formatted_text, ...) const
{
    if(formatted_text == NULL)
        return makeRect(x, y, 0, 0);

    va_list lst;
    va_start(lst, formatted_text);
    vsprintf(buffer, formatted_text, lst);
    va_end(lst);

    char* str = copyString(buffer);
    char* del = str;

    for(char* c = str; *c != '\0';)
    {
        if(*c == '\n')
        {
            *c = '\0';
            drawToSurface(x - getWidth("%s", str), y, str);
            *c = '\n';
            c++;
            str = c;
            y += height;
        }
        else
            c++;
    }
    char s[strlen(str)+1];
    strcpy(s, str);
    delete[] del;
    
    return drawToSurface(x - getWidth("%s", s), y, s);
}

SDL_Rect NFont::drawPos(int x, int y, NFont::AnimFn posFn, const char* text, ...) const
{
    va_list lst;
    va_start(lst, text);
    vsprintf(buffer, text, lst);
    va_end(lst);

    return drawToSurfacePos(x, y, posFn);
}

SDL_Rect NFont::drawAll(int x, int y, NFont::AnimFn allFn, const char* text, ...) const
{
    va_list lst;
    va_start(lst, text);
    vsprintf(buffer, text, lst);
    va_end(lst);

    allFn(x, y, data);
    return data.dirtyRect;
}




// Getters
SDL_Surface* NFont::getDest() const
{
    return dest;
}

SDL_Surface* NFont::getSurface() const
{
    return src;
}

int NFont::getHeight(const char* formatted_text, ...) const
{
    if(formatted_text == NULL)
        return height;

    va_list lst;
    va_start(lst, formatted_text);
    vsprintf(buffer, formatted_text, lst);
    va_end(lst);

    int numLines = 1;
    const char* c;

    for (c = buffer; *c != '\0'; c++)
    {
        if(*c == '\n')
            numLines++;
    }

    //   Actual height of letter region + line spacing
    return height*numLines + lineSpacing*(numLines - 1);  //height*numLines;
}

int NFont::getWidth(const char* formatted_text, ...) const
{
    if (formatted_text == NULL)
        return 0;

    va_list lst;
    va_start(lst, formatted_text);
    vsprintf(buffer, formatted_text, lst);
    va_end(lst);

    const char* c;
    int charnum = 0;
    int width = 0;
    int bigWidth = 0;  // Allows for multi-line strings

    for (c = buffer; *c != '\0'; c++)
    {
        charnum = (unsigned char)(*c) - 33;

        // skip spaces and nonprintable characters
        if(*c == '\n')
        {
            bigWidth = bigWidth >= width? bigWidth : width;
            width = 0;
        }
        else if (*c == ' ' || charnum > 222)
        {
            width += charWidth[0];
            continue;
        }

        width += charWidth[charnum];
    }
    bigWidth = bigWidth >= width? bigWidth : width;

    return bigWidth;
}

int NFont::getAscent(const char character) const
{
    unsigned char test = (unsigned char)character;
    if(test < 33 || test > 222 || (test > 126 && test < 161))
        return 0;
    unsigned char num = (unsigned char)character - 33;
    // Get the max ascent
    int x = charPos[num];
    int i, j = 1, result = 0;
    Uint32 pixel = getPixel(src, 0, src->h - 1); // bg pixel
    while(j < baseline && j < src->h)
    {
        i = charPos[num];
        while(i < x + charWidth[num])
        {
            if(getPixel(src, i, j) != pixel)
            {
                result = baseline - j;
                j = src->h;
                break;
            }
            i++;
        }
        j++;
    }
    return result;
}

int NFont::getAscent(const char* formatted_text, ...) const
{
    if(formatted_text == NULL)
        return ascent;
    
    va_list lst;
    va_start(lst, formatted_text);
    vsprintf(buffer, formatted_text, lst);
    va_end(lst);
    
    int max = 0;
    const char* c = buffer;
    
    for (; *c != '\0'; c++)
    {
        int asc = getAscent(*c);
        if(asc > max)
            max = asc;
    }
    return max;
}

int NFont::getDescent(const char character) const
{
    unsigned char test = (unsigned char)character;
    if(test < 33 || test > 222 || (test > 126 && test < 161))
        return 0;
    unsigned char num = (unsigned char)character - 33;
    // Get the max descent
    int x = charPos[num];
    int i, j = src->h - 1, result = 0;
    Uint32 pixel = getPixel(src, 0, src->h - 1); // bg pixel
    while(j > 0 && j > baseline)
    {
        i = charPos[num];
        while(i < x + charWidth[num])
        {
            if(getPixel(src, i, j) != pixel)
            {
                result = j - baseline;
                j = 0;
                break;
            }
            i++;
        }
        j--;
    }
    return result;
}

int NFont::getDescent(const char* formatted_text, ...) const
{
    if(formatted_text == NULL)
        return descent;
    
    va_list lst;
    va_start(lst, formatted_text);
    vsprintf(buffer, formatted_text, lst);
    va_end(lst);
    
    int max = 0;
    const char* c = buffer;
    
    for (; *c != '\0'; c++)
    {
        int des = getDescent(*c);
        if(des > max)
            max = des;
    }
    return max;
}

int NFont::getSpacing() const
{
    return letterSpacing;
}

int NFont::getLineSpacing() const
{
    return lineSpacing;
}

int NFont::getBaseline() const
{
    return baseline;
}

int NFont::getMaxWidth() const
{
    return maxWidth;
}





// Setters
void NFont::setSpacing(int LetterSpacing)
{
    letterSpacing = LetterSpacing;
}

void NFont::setLineSpacing(int LineSpacing)
{
    lineSpacing = LineSpacing;
}

void NFont::setDest(SDL_Surface* Dest)
{
    dest = Dest;
}

int NFont::setBaseline(int Baseline)
{
    if(Baseline >= 0)
        baseline = Baseline;
    else
    {
        // Get the baseline by checking a, b, and c and averaging their lowest y-value.
        // Is there a better way?
        Uint32 pixel = getPixel(src, 0, src->h - 1);
        int heightSum = 0;
        int x, i, j;
        for(unsigned char avgChar = 64; avgChar < 67; avgChar++)
        {
            x = charPos[avgChar];
            
            j = src->h - 1;
            while(j > 0)
            {
                i = x;
                while(i - x < charWidth[64])
                {
                    if(getPixel(src, i, j) != pixel)
                    {
                        heightSum += j;
                        j = 0;
                        break;
                    }
                    i++;
                }
                j--;
            }
        }
        baseline = int(heightSum/3.0f + 0.5f);  // Round up and cast
    }
    return baseline;
}







