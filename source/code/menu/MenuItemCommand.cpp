/*
 * File:   MenuItemCommand.cpp
 * Author: poul
 *
 * Created on 11. marts 2011, 20:46
 */

#include "MenuItemCommand.hpp"

MenuItemCommand::MenuItemCommand()
{
	selected = false;
}

MenuItemCommand::MenuItemCommand(string text, void(*pt2Function)() )
{
	this->text = text;
	command = pt2Function;
	selected = false;
}

MenuItemCommand::MenuItemCommand(const MenuItemCommand& orig)
{
}

MenuItemCommand::~MenuItemCommand()
{
}

void MenuItemCommand::SetSelected(bool selected)
{
	this->selected = selected;
}

void MenuItemCommand::Activated()
{
	//Add execode
	command();
}

string MenuItemCommand::GetType()
{
	return "MenuItemCommand";
}

string MenuItemCommand::GetText()
{
	return text;
}

void MenuItemCommand::SetText(string text)
{
	this->text = text;
}