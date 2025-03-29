/*
Copyright (c) 2025 Poul Sander

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

#pragma once
#include <algorithm>
#include "SDL.h"


namespace sago {

class SagoLogicalResize
{
public:
	SagoLogicalResize() : logical_width_(1), logical_height_(1), physical_width_(1), physical_height_(1) { SetScaleFactor(); }
	SagoLogicalResize(int logical_width, int logical_height)
		: logical_width_(std::max(1, logical_width)), logical_height_(std::max(1, logical_height)), physical_width_(1), physical_height_(1) { SetScaleFactor(); }

	void SetPhysicalSize(int physical_width, int physical_height)
	{
		//Physical size must be at least 1. Less than 1 is not drawn anyway and it prevents division by zero.
		physical_width_ = std::max(1, physical_width);
		physical_height_ = std::max(1, physical_height);
		SetScaleFactor();
	}

	void LogicalToPhysical(int logical_x, int logical_y, int &physical_x, int &physical_y) const
	{
		physical_x = logical_x;
		physical_y = logical_y;
		LogicalToPhysical(&physical_x, &physical_y);
	}

	void LogicalToPhysical(int *x, int *y) const
	{
		if (x) {
			*x = *x * scale_factor_ + left_margin_;
		}
		if (y) {
			*y = *y * scale_factor_ + top_margin_;
		}
	}

	void LogicalToPhysical(SDL_Rect &inout) const
	{
		SDL_Rect input = inout;
		LogicalToPhysical(&inout.x, &inout.y);
		LogicalToPhysical(input.x + input.w + 1, input.y + input.h + 1, inout.w, inout.h);
		inout.w -= inout.x - 1;
		inout.h -= inout.y - 1;
	}

	void PhysicalToLogical(int physical_x, int physical_y, int &logical_x, int &logical_y) const
	{
		logical_x = (physical_x - left_margin_) / scale_factor_;
		logical_y = (physical_y - top_margin_) / scale_factor_;
	}

	int GetTopMargin() const
	{
		return top_margin_;
	}

	int GetLeftMargin() const
	{
		return left_margin_;
	}

private:
	void SetScaleFactor()
	{
		scale_factor_ = std::min(physical_width_ / logical_width_, physical_height_ / logical_height_);
		left_margin_ = (physical_width_ - logical_width_ * scale_factor_) / 2;
		top_margin_ = (physical_height_ - logical_height_ * scale_factor_) / 2;
	}

	double scale_factor_;
	int top_margin_;
	int left_margin_;
	double logical_width_;
	double logical_height_;
	double physical_width_;
	double physical_height_;
};

}  // namespace sago