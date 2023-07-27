/*
===========================================================================
blockattack - Block Attack - Rise of the Blocks
Copyright (C) 2005-2012 Poul Sander

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


#ifndef _MENUSYSTEM_H
#define	_MENUSYSTEM_H

#include <string>
#include "SDL.h"
#include <map>
#include <vector>
#include "sago/SagoSprite.hpp"
#include "sago/GameStateInterface.hpp"
#include <memory>
#include "sago/SagoTextField.hpp"

//The ButtonGfx object hold common media for all buttons, so we can reskin them by only changeing one pointer
struct ButtonGfx
{
	//The size of the buttons, so we don't have to ask w and h from the SDL Surfaces each time
	int xsize = 0;
	int ysize = 0;
	sago::SagoTextField* getLabel(const std::string& text, bool marked);
	void setSurfaces();
private:
	std::map<std::string, std::shared_ptr<sago::SagoTextField> > labels;
	std::map<std::string, std::shared_ptr<sago::SagoTextField> > labels_marked;
};

extern ButtonGfx standardButton;

//A button
class Button
{
private:
	//Pointer to a callback function.
	void (*action)(void) = nullptr;

	//If true the menu should also be closed then the button is clicked
	bool popOnRun = false;

	//The label. This is written on the button
	std::string label;
public:
	//Is the button marked?
	bool marked = false;
	//Where is the button on the screen
	int x = 0;
	int y = 0;

	Button() = default;
	Button(const Button& b);
	Button& operator=(const Button& other);
	virtual ~Button() = default;


	//Set the text to write on the button
	void setLabel(const std::string& text);
	//Set the action to run
	void setAction(void (*action2run)(void));

	virtual void doAction(); //Run the callback function
	void setPopOnRun(bool popOnRun);
	bool isPopOnRun() const;
	virtual const std::string& getLabel() const {return this->label; };

	//May hold any other information the callback might need
	int iGeneric1 = 0;
};


class Menu : public sago::GameStateInterface
{
private:
	std::vector<Button*> buttons; //Vector holder the buttons
	Button exit; //The exit button is special since it does not have a callback function
	bool isSubmenu = false; //True if the menu is a submenu
	int marked = 0; //The index of the marked button (for keyboard up/down)
	bool running = true; //The menu is running. The menu will terminate then this is false
	SDL_Renderer *screen = nullptr; //Pointer to the screen to draw to
	std::string title;
	void drawSelf(SDL_Renderer* target);        //Private function to draw the screen
	void placeButtons(); //Rearanges the buttons to the correct place.
	bool bMouseUp = false;
public:
	//numberOfItems is the expected numberOfItems for vector initialization
	//SubMenu is true by default
	Menu(SDL_Renderer *screen,bool isSubmenu);
	explicit Menu(SDL_Renderer *screen);
	Menu(SDL_Renderer *screen, const std::string& title, bool isSubmenu);
	virtual ~Menu() {}

	//Add a button to the menu
	void addButton(Button *b);

	bool IsActive() override;
	void Draw(SDL_Renderer* target) override;
	void ProcessInput(const SDL_Event& event, bool &processed) override;
	void Update() override;
};

class FileMenu
{
private:
	std::string pm_path;
	std::string pm_fileending;
	bool pm_hidden_files = false;
public:
	FileMenu(const std::string& path, const std::string& fileending, bool hidden_files = false);

	std::string getFile(SDL_Surface **screen);
};

bool isUpEvent(const SDL_Event& event);

bool isDownEvent(const SDL_Event& event);

bool isLeftEvent(const SDL_Event& event);

bool isRightEvent(const SDL_Event& event);

bool isEscapeEvent(const SDL_Event& event);

bool isConfirmEvent(const SDL_Event& event);

#endif	/* _MENUSYSTEM_H */

