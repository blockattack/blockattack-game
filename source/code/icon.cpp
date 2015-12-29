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
#include "icon.h"

void SetSDLIcon(SDL_Window* window)
{
	SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(
			(void *)gimp_image.pixel_data,
			gimp_image.width,
			gimp_image.height,
			gimp_image.bytes_per_pixel * 8,
			gimp_image.bytes_per_pixel * gimp_image.width,
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF
#else
			0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000
#endif
			);
	SDL_SetWindowIcon(window, surface);
	SDL_FreeSurface(surface);

}