/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2015 Poul Sander

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/

Source information and contacts persons can be found at
http://blockattack.net
===========================================================================
*/

#include "icon.hpp"
#include "icon.inc"

void SetSDLIcon(SDL_Window* window) {
	SDL_Surface* surface = SDL_CreateSurfaceFrom(
	                           gimp_image.width,
	                           gimp_image.height,
	                           SDL_PIXELFORMAT_RGBA32,
	                           (void*)gimp_image.pixel_data,
	                           gimp_image.bytes_per_pixel * gimp_image.width
	                       );
	SDL_SetWindowIcon(window, surface);
	SDL_DestroySurface(surface);
}
