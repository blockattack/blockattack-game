/*
Copyright (c) 2018 Poul Sander

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge,
publish, distribute, sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef SAGOTEXTFIELD_HPP
#define SAGOTEXTFIELD_HPP

#include "SagoDataHolder.hpp"

namespace sago {

/**
 * This is a text field.
 * It represents a line of text to be drawn on screen. It is not possible to have line breaks.
 * If line breaks are needed use SagoTextBox instead.
 * 
 * This object renderes to a texture and cahces the texture. The texture will be automatically refreshed if the text changes, the SagoDataHolder is invalidated or ClearCache is called.
 * Normally all values will be set at the beginning before text is drawn.
 * SetHolder MUST be called before the field is drawn!
 */
class SagoTextField final {
public:
	SagoTextField();
	SagoTextField(SagoTextField&& o) noexcept;
	SagoTextField& operator=(const SagoTextField&& base) = delete;
	SagoTextField& operator=(const SagoTextField& base) = delete;
	~SagoTextField();
	/**
	 * This method creates a copy of a given font.
	 * The cache will not be copied.
	 * This is ALMOST like the "= operator" but given its own name to prevent implicit calling. 
	 * @param base The object to copy from
	 * @return A reference to this object.
	 */
	SagoTextField& CopyFrom(const SagoTextField& base);
	/**
	 * Sets the data holder. This is MANDATORY
	 * @param holder The data holder to fetch the fonts from
	 */
	void SetHolder(const SagoDataHolder* holder);
	/**
	 * Set the text to display.
	 * @param text The actual UTF-8 encoded text
	 */
	void SetText(const char* text);
	/**
	 * Set the text to display.
	 * @param text The actual UTF-8 encoded text
	 */
	void SetText(const std::string& text);
	void SetColor(const SDL_Color& color);
	/**
	 * Set the name of the font. Must be known to the data holder.
	 * The name could for instance be "freeserif".
	 * @param fontName Name of the font as required by SagoDataHolder
	 */
	void SetFont(const char* fontName);
	void SetFontSize(int fontSize);
	/**
	 * Enable outline against the font.
	 * @param outlineSize Number of pixels of outline.
	 * @param color The color of the outline.
	 */
	void SetOutline(int outlineSize, const SDL_Color& color);
	/**
	 * Get the text we are currently drawing
	 * @return The text
	 */
	const std::string& GetText() const;
	/**
	 * A Shorthand for calling TTF_SizeUTF8 on the right font
	 * The size is measuered WITHOUT the outline!
	 * Will fail silently on error (except writing to stderr) and set w and h to 0 if they are not null.
	 * @param text The text to check the rendered size for
	 * @param w Pointer to an int where the width of the text will be stored. Maybe null.
	 * @param h Pointer to an int where the hight of the text will be stored. Maybe null.
	 */
	void GetRenderedSize(const char* text, int* w = nullptr, int* h = nullptr);
	enum class Alignment { left = 0, right=1, center = 2 };
	enum class VerticalAlignment { top = 0, center = 1};
	void Draw(SDL_Renderer* target, int x, int y, Alignment alignment = Alignment::left, VerticalAlignment verticalAlignment = VerticalAlignment::top);
	/**
	 * Updates the cache.
	 * You normally do not want to call this from the outside as it is done just in time.
	 * Unless you want to precache of course....
	 * @param target Target the the text will eventually be rendered to
	 */
	void UpdateCache(SDL_Renderer* target);
	/**
	 * Clears the cache and forces the SagoTextField to render it again the next time it is drawn.
	 * Can be used if you have changed font, color or sizes.
	 * Changing the text implices a cache clear.
	 */
	void ClearCache();
private:
	SagoTextField(const SagoTextField& orig) = delete;
	struct SagoTextFieldData;
	SagoTextFieldData *data;
};

}  //namespace sago

#endif /* SAGOTEXTFIELD_HPP */

