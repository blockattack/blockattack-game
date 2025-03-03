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
		logical_x = physical_x * logical_width_ / physical_width_ + left_margin_;
		logical_y = physical_y * logical_height_ / physical_height_ + top_margin_;
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