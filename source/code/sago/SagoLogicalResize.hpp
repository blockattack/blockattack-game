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

/**
 * @brief Provides coordinate transformation between logical and physical screen coordinates.
 *
 * This class helps create resolution-independent layouts by defining coordinates in a fixed
 * "logical" resolution and automatically converting them to the actual "physical" screen size.
 * It maintains aspect ratio by adding letterboxing/pillarboxing margins when needed.
 *
 * Key concepts:
 * - Logical coordinates: Fixed coordinate system you design for (e.g., 1920x1080)
 * - Physical coordinates: Actual screen/window size (e.g., 1280x720, 3840x2160, etc.)
 * - Scale factor: Calculated to fit logical size into physical size while preserving aspect ratio
 * - Margins: Black bars added to maintain aspect ratio (letterboxing/pillarboxing)
 * - Tile alignment: As long as tiles are resized with LogicalToPhysical, they will align correctly. No gaps or overlaps.
 *
 * Example usage:
 * @code
 * // Design your UI for 1920x1080
 * SagoLogicalResize resize(1920, 1080);
 * resize.SetPhysicalSize(window_width, window_height);
 *
 * // Draw at logical position (100, 100)
 * int phys_x, phys_y;
 * resize.LogicalToPhysical(100, 100, phys_x, phys_y);
 * DrawSprite(renderer, phys_x, phys_y);
 *
 * // Handle mouse input
 * int log_x, log_y;
 * resize.PhysicalToLogical(mouse_x, mouse_y, log_x, log_y);
 * if (log_x > 100 && log_x < 200) { ... }
 * @endcode
 */
class SagoLogicalResize {
public:
	/**
	 * @brief Default constructor with minimal 1x1 logical size.
	 */
	SagoLogicalResize() : logical_width_(1), logical_height_(1), physical_width_(1), physical_height_(1) {
		SetScaleFactor();
	}

	/**
	 * @brief Constructor with specified logical resolution.
	 * @param logical_width The width of the logical coordinate system (minimum 1)
	 * @param logical_height The height of the logical coordinate system (minimum 1)
	 */
	SagoLogicalResize(int logical_width, int logical_height)
		: logical_width_(std::max(1, logical_width)), logical_height_(std::max(1, logical_height)), physical_width_(1), physical_height_(1) {
		SetScaleFactor();
	}

	/**
	 * @brief Updates the physical (actual) screen size and recalculates scaling.
	 *
	 * Call this when the window is resized or when initializing the physical display size.
	 * This recalculates the scale factor and margins to fit the logical size into the physical size.
	 *
	 * @param physical_width The actual width of the screen/window (minimum 1)
	 * @param physical_height The actual height of the screen/window (minimum 1)
	 */
	void SetPhysicalSize(int physical_width, int physical_height) {
		//Physical size must be at least 1. Less than 1 is not drawn anyway and it prevents division by zero.
		physical_width_ = std::max(1, physical_width);
		physical_height_ = std::max(1, physical_height);
		SetScaleFactor();
	}

	/**
	 * @brief Converts logical coordinates to physical screen coordinates.
	 *
	 * This applies the scale factor and adds the appropriate margins.
	 * Use this to convert positions where you want to draw elements on screen.
	 *
	 * @param logical_x The x coordinate in logical space
	 * @param logical_y The y coordinate in logical space
	 * @param physical_x Output: the x coordinate in physical screen space
	 * @param physical_y Output: the y coordinate in physical screen space
	 */
	void LogicalToPhysical(int logical_x, int logical_y, int& physical_x, int& physical_y) const {
		physical_x = logical_x;
		physical_y = logical_y;
		LogicalToPhysical(&physical_x, &physical_y);
	}

	/**
	 * @brief Converts logical coordinates to physical screen coordinates (pointer version).
	 *
	 * This applies the scale factor and adds the appropriate margins.
	 * Null pointers are safely ignored.
	 *
	 * @param x Pointer to x coordinate (will be converted in place), can be null
	 * @param y Pointer to y coordinate (will be converted in place), can be null
	 */
	void LogicalToPhysical(int* x, int* y) const {
		if (x) {
			*x = *x * scale_factor_ + left_margin_;
		}
		if (y) {
			*y = *y * scale_factor_ + top_margin_;
		}
	}

	/**
	 * @brief Converts a logical rectangle to physical screen coordinates.
	 *
	 * This converts both the position AND size of a rectangle. The rectangle
	 * is transformed in place. Use this for converting areas/regions rather than
	 * just single points.
	 *
	 * Note: This properly handles the size conversion by converting the bottom-right
	 * corner and calculating the new width/height from the difference.
	 *
	 * @param inout The rectangle in logical coordinates (input), converted to physical coordinates (output)
	 */
	void LogicalToPhysical(SDL_Rect& inout) const {
		SDL_Rect input = inout;
		LogicalToPhysical(&inout.x, &inout.y);
		LogicalToPhysical(input.x + input.w + 1, input.y + input.h + 1, inout.w, inout.h);
		inout.w -= inout.x - 1;
		inout.h -= inout.y - 1;
	}

	/**
	 * @brief Converts physical screen coordinates to logical coordinates.
	 *
	 * Use this to convert mouse input or other physical positions back to your
	 * logical coordinate system for hit detection and input handling.
	 * This removes margins and applies inverse scaling.
	 *
	 * @param physical_x The x coordinate in physical screen space
	 * @param physical_y The y coordinate in physical screen space
	 * @param logical_x Output: the x coordinate in logical space
	 * @param logical_y Output: the y coordinate in logical space
	 */
	void PhysicalToLogical(int physical_x, int physical_y, int& logical_x, int& logical_y) const {
		logical_x = (physical_x - left_margin_) / scale_factor_;
		logical_y = (physical_y - top_margin_) / scale_factor_;
	}

	/**
	 * @brief Gets the top margin (letterboxing/pillarboxing offset).
	 *
	 * This is the black bar size at the top when the aspect ratio requires vertical margins.
	 * Useful for debugging or custom rendering that needs to know the drawable area.
	 *
	 * @return The top margin in physical pixels
	 */
	int GetTopMargin() const {
		return top_margin_;
	}

	/**
	 * @brief Gets the left margin (letterboxing/pillarboxing offset).
	 *
	 * This is the black bar size on the left when the aspect ratio requires horizontal margins.
	 * Useful for debugging or custom rendering that needs to know the drawable area.
	 *
	 * @return The left margin in physical pixels
	 */
	int GetLeftMargin() const {
		return left_margin_;
	}

private:
	/**
	 * @brief Calculates the scale factor and margins based on current logical and physical sizes.
	 *
	 * The scale factor is chosen to fit the logical size into the physical size while
	 * maintaining aspect ratio. The margins center the content on screen.
	 *
	 * Scale factor = min(physical_width/logical_width, physical_height/logical_height)
	 * This ensures the content fits in both dimensions without distortion.
	 */
	void SetScaleFactor() {
		scale_factor_ = std::min(physical_width_ / logical_width_, physical_height_ / logical_height_);
		left_margin_ = (physical_width_ - logical_width_ * scale_factor_) / 2;
		top_margin_ = (physical_height_ - logical_height_ * scale_factor_) / 2;
	}

	double scale_factor_;      ///< Multiplier to convert logical size to physical size
	int top_margin_;           ///< Vertical offset for centering (letterboxing)
	int left_margin_;          ///< Horizontal offset for centering (pillarboxing)
	double logical_width_;     ///< Width of the logical coordinate system
	double logical_height_;    ///< Height of the logical coordinate system
	double physical_width_;    ///< Actual width of the screen/window
	double physical_height_;   ///< Actual height of the screen/window
};

}  // namespace sago