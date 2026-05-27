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

#include "SagoTextField.hpp"
#include <iostream>
#include <SDL3_ttf/SDL_ttf.h>

namespace sago {

class OutlineHandler {
	TTF_Font* font;
	int originalOutline = 0;
	int targetOutline;
	bool doChange = false;
public:
	OutlineHandler(TTF_Font* font, int outline) : font{font}, targetOutline{outline} {
		originalOutline = TTF_GetFontOutline(font);
		if (originalOutline == targetOutline) {
			return;
		}
		doChange = true;
		TTF_SetFontOutline(font, targetOutline);
	};

	void reset() {
		if (doChange) {
			TTF_SetFontOutline(font,originalOutline);
			doChange = false;
		}
	}

	~OutlineHandler() {
		reset();
	}
private:
	OutlineHandler(const OutlineHandler& orig) = delete;
	OutlineHandler& operator=(const OutlineHandler& base) = delete;
};

struct SagoTextField::SagoTextFieldData {
	const sago::SagoDataHolder* tex = nullptr;
	SDL_Surface* textSurface = nullptr;
	SDL_Texture* texture = nullptr;
	SDL_Surface* outlineTextSurface = nullptr;
	SDL_Texture* outlineTexture = nullptr;
	std::string fontName = "freeserif";
	SDL_Color color = { 255, 255, 255, 0 };
	SDL_Color outlineColor = { 255, 255, 0, 0 };
	int fontSize = 16;
	int outline = 0;
	std::string text = "";
	std::string renderedText = "";
	Uint64 renderedVersion = 0;
};

SagoTextField::SagoTextField() {
	data = new SagoTextFieldData();
}

SagoTextField::SagoTextField(SagoTextField&& o) noexcept {
	data = o.data;
	o.data = nullptr;
}

SagoTextField& SagoTextField::CopyFrom(const SagoTextField& base) {
	ClearCache();
	try {
		*data = *(base.data);
		//Copy all data but do not reuse the cache as it would result in a double free
		data->outlineTextSurface = nullptr;
		data->outlineTexture = nullptr;
		data->textSurface = nullptr;
		data->texture = nullptr;
		return *this;
	}
	catch (...) {
		delete data;
		throw;
	}
}

SagoTextField::~SagoTextField() {
	if (!data) {
		return;
	}
	ClearCache();
	delete data;
}

void SagoTextField::SetHolder(const SagoDataHolder* holder) {
	data->tex = holder;
}

void SagoTextField::SetText(const char* text) {
	data->text = text;
}

void SagoTextField::SetText(const std::string& text) {
	data->text = text;
}

void SagoTextField::SetColor(const SDL_Color& color) {
	data->color = color;
}

void SagoTextField::SetFont(const char* fontName) {
	data->fontName = fontName;
}

void SagoTextField::SetFontSize(int fontSize) {
	data->fontSize = fontSize;
}

void SagoTextField::SetOutline(int outlineSize, const SDL_Color& color) {
	data->outline = outlineSize;
	data->outlineColor = color;
}

const std::string& SagoTextField::GetText() const {
	return data->text;
}

void SagoTextField::ClearCache() {
	if (data->texture) {
		SDL_DestroyTexture(data->texture);
		data->texture = nullptr;
	}
	if (data->textSurface) {
		SDL_DestroySurface(data->textSurface);
		data->textSurface = nullptr;
	}
	if (data->outlineTexture) {
		SDL_DestroyTexture(data->outlineTexture);
		data->outlineTexture = nullptr;
	}
	if (data->outlineTextSurface) {
		SDL_DestroySurface(data->outlineTextSurface);
		data->outlineTextSurface = nullptr;
	}
}

void SagoTextField::UpdateCache(SDL_Renderer* target) {
	if (!data->tex) {
		std::cerr << "FATAL: DataHolder not set!\n";
		abort();
	}
	ClearCache();
	TTF_Font* font = data->tex->getFontPtr(data->fontName, data->fontSize);
	data->textSurface = TTF_RenderText_Blended (font, data->text.c_str(), 0, data->color);
	data->texture = SDL_CreateTextureFromSurface(target, data->textSurface);
	if (data->outline > 0) {
		OutlineHandler oh(font, data->outline);
		data->outlineTextSurface = TTF_RenderText_Blended (font, data->text.c_str(), 0, data->outlineColor);
		data->outlineTexture = SDL_CreateTextureFromSurface(target, data->outlineTextSurface);
		oh.reset();
	}
	data->renderedText = data->text;
	data->renderedVersion = data->tex->getVersion();
}

void SagoTextField::GetRenderedSize(const char* text, int* w, int* h) {
	TTF_Font* font = data->tex->getFontPtr(data->fontName, data->fontSize);
	if (!TTF_GetStringSize(font, text, 0, w, h)) {
		if (w) {
			*w = 0;
		}
		if (h) {
			*h = 0;
		}
		std::cerr << "GetRenderedSize failed to find size of " << text << "\n";
	}
}

void SagoTextField::Draw(SDL_Renderer* target, int x, int y, Alignment alignment, VerticalAlignment verticalAlignment, SagoLogicalResize* resize) {
	if (data->text.empty()) {
		return;
	}
	if (data->text != data->renderedText || data->renderedVersion != data->tex->getVersion()) {
		UpdateCache(target);
	}
	if (!data->texture) {
		return;
	}
	float texWf = 0;
	float texHf = 0;
	SDL_GetTextureSize(data->texture, &texWf, &texHf);
	int texW = (int)texWf;
	int texH = (int)texHf;
	if (alignment == Alignment::center) {
		x -= texW/2;
	}
	if (alignment == Alignment::right) {
		x -= texW;
	}
	if (verticalAlignment == VerticalAlignment::center) {
		y -= texH/2;
	}
	if (verticalAlignment == VerticalAlignment::bottom) {
		y -= texH;
	}
	SDL_Rect dstrect = { x, y, texW, texH };
	if (data->outlineTexture) {
		float outlineTexWf = 0;
		float outlineTexHf = 0;
		SDL_GetTextureSize(data->outlineTexture, &outlineTexWf, &outlineTexHf);
		int outlineTexW = (int)outlineTexWf;
		int outlineTexH = (int)outlineTexHf;
		SDL_Rect dstrectOutline = { x-(data->outline), y-(data->outline), outlineTexW, outlineTexH };
		if (resize) {
			resize->LogicalToPhysical(dstrectOutline);
		}
		SDL_FRect fdstrectOutline = {(float)dstrectOutline.x, (float)dstrectOutline.y, (float)dstrectOutline.w, (float)dstrectOutline.h};
		SDL_RenderTexture(target, data->outlineTexture, NULL, &fdstrectOutline);
	}
	if (resize) {
		resize->LogicalToPhysical(dstrect);
	}
	SDL_FRect fdstrect = {(float)dstrect.x, (float)dstrect.y, (float)dstrect.w, (float)dstrect.h};
	SDL_RenderTexture(target, data->texture, NULL, &fdstrect);
}

}  //namespace sago