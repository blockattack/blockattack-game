/*
===========================================================================
 * Sago Multi Scrambler Puzzle
Copyright (C) 2022 Poul Sander

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
https://github.com/sago007/saland
===========================================================================
*/

#include "../../sago/GameStateInterface.hpp"
#include <vector>
#include <string>
#include "editor_loader.hpp"

#pragma once

class SagoTextureSelector : public sago::GameStateInterface {
public:
	SagoTextureSelector();
	SagoTextureSelector(const SagoTextureSelector& orig) = delete;
	virtual ~SagoTextureSelector();
	
	bool IsActive() override;
	void ProcessInput(const SDL_Event& event, bool &processed) override;
	void Draw(SDL_Renderer* target) override;
	void Update() override;
	void Init();

	void runTextureSelectorFrame(SDL_Renderer* target);
	void runSpriteSelectorFrame(SDL_Renderer* target);

private:
	bool isActive = true;
	std::vector<std::string> textures;
	std::map<std::string,SagoSprite> sprites;
	std::string selected_texture;
	std::string selected_sprite;
};