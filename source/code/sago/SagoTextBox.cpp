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

#include "SagoTextBox.hpp"
#include "SagoTextField.hpp"
#include <vector>
#include <iostream>
#include "utf8.h"
#include <algorithm>

namespace sago {

struct SagoTextBox::SagoTextBoxData {
	const sago::SagoDataHolder* tex = nullptr;
	std::string fontName = "freeserif";
	SDL_Color color = { 255, 255, 255, 0 };
	SDL_Color outlineColor = { 255, 255, 0, 0 };
	int fontSize = 16;
	int outline = 0;
	std::string text = "";
	std::string renderedText = "";
	std::vector<SagoTextField> lines;
	int maxWidth = 0;
};

SagoTextBox::SagoTextBox() {
	data = new SagoTextBoxData();
}

SagoTextBox::~SagoTextBox() {
	delete data;
}

void SagoTextBox::SetHolder(const SagoDataHolder* holder) {
	data->tex = holder;
}

void SagoTextBox::SetText(const char* text) {
	data->text = text;
}

void SagoTextBox::SetText(const std::string& text) {
	data->text = text;
}

void SagoTextBox::SetColor(const SDL_Color& color) {
	data->color = color;
}

void SagoTextBox::SetFont(const char* fontName) {
	data->fontName = fontName;
}

void SagoTextBox::SetFontSize(int fontSize) {
	data->fontSize = fontSize;
}

void SagoTextBox::SetOutline(int outlineSize, const SDL_Color& color) {
	data->outline = outlineSize;
	data->outlineColor = color;
}

void SagoTextBox::SetMaxWidth(int width) {
	data->maxWidth = width;
}

const std::string& SagoTextBox::GetText() const {
	return data->text;
}

void SagoTextBox::AppendLineToCache(const std::string& text) {
	data->lines.resize(data->lines.size()+1);
	SagoTextField& tf = data->lines.back();
	tf.SetHolder(data->tex);
	tf.SetFont(data->fontName.c_str());
	tf.SetFontSize(data->fontSize);
	tf.SetColor(data->color);
	tf.SetOutline(data->outline, data->outlineColor);
	tf.SetText(text);
}


void SagoTextBox::SplitAndAppendLineToCache(TTF_Font* font, const std::string& text) {
	if (text.length() == 0) {
		return;
	}
	int width = data->maxWidth;
	TTF_SizeUTF8(font, text.c_str(),&width, nullptr);
	if (data->maxWidth <= 0 || width <= data->maxWidth || text.length() == 1) {
		AppendLineToCache(text);
		return;
	}
	std::string::const_iterator splitLocation = text.begin()+1;
	bool splitDone = false;
	while (!splitDone) {
		std::string::const_iterator nextSearchStart = splitLocation+1;
		if (nextSearchStart == text.end()) {
			splitDone = true;
			continue;
		}
		std::string::const_iterator nextSpace = std::find(nextSearchStart, text.end(), ' ');
		std::string attemptSubString(text.begin(), nextSpace);
		TTF_SizeUTF8(font, attemptSubString.c_str(),&width, nullptr);
		if (width <= data->maxWidth && nextSpace != text.end()) {
			splitLocation = nextSpace;
		}
		else {
			splitDone = true;
		}
	}
	if (splitLocation == text.begin()+1) {
		splitLocation = text.begin();
		utf8::advance(splitLocation, 1, text.end());
		splitDone = false;
		while (!splitDone && splitLocation != text.end()) {
			std::string::const_iterator nextSplit = splitLocation;
			utf8::advance(nextSplit, 1, text.end());
			std::string attemptSubString(text.begin(), nextSplit);
			TTF_SizeUTF8(font, attemptSubString.c_str(), &width, nullptr);
			if (width <= data->maxWidth) {
				splitLocation = nextSplit;
			}
			else {
				splitDone = true;
			}
		}
	}
	std::string firstPart(text.begin(), splitLocation);
	AppendLineToCache(firstPart);
	while (splitLocation != text.end() && *splitLocation == ' ') {
		//Trim spaces after an automatic line break.
		++splitLocation;
	}
	if (splitLocation == text.end()) {
		return;
	}
	std::string secondPart(splitLocation, text.end());
	SplitAndAppendLineToCache(font, secondPart);
}

void SagoTextBox::UpdateCache() {
	if (!data->tex) {
		std::cerr << "FATAL: SagoTextBox::UpdateCache - DataHolder not set!\n";
		abort();
	}
	TTF_Font *font = data->tex->getFontPtr(data->fontName, data->fontSize);
	const char delim = '\n';
	const std::string& s = data->text;
	auto start = 0U;
	auto end = s.find(delim);
	data->lines.clear();
	while (end != std::string::npos)
	{
		const std::string& theSubString = s.substr(start, end - start);
		SplitAndAppendLineToCache(font, theSubString);
		start = end + 1;
		end = s.find(delim, start);
	}
	SplitAndAppendLineToCache(font, s.substr(start, end));
	data->renderedText = data->text;
}

void SagoTextBox::Draw(SDL_Renderer* target, int x, int y) {
	if (data->text != data->renderedText) {
		UpdateCache();
	}
	TTF_Font *font = data->tex->getFontPtr(data->fontName, data->fontSize);
	int lineSkip = TTF_FontLineSkip(font);
	for (size_t i = 0; i < data->lines.size(); ++i) {
		data->lines[i].Draw(target, x, y+i*lineSkip);
	}
}

}  //namespace sago