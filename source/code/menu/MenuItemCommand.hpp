/*
 * File:   MenuItemCommand.hpp
 * Author: poul
 *
 * Created on 11. marts 2011, 20:46
 */

#ifndef _MENUITEMCOMMAND_HPP
#define	_MENUITEMCOMMAND_HPP

#include "MenuItem.hpp"

class MenuItemCommand : MenuItem
{
public:
	MenuItemCommand();
	MenuItemCommand(string text, void (*pt2Function)(void));
	MenuItemCommand(const MenuItemCommand& orig);
	virtual ~MenuItemCommand();
	void SetSelected(bool selected);
	void Activated();
	string GetText();
	string GetType();
	void SetText(string text);
private:
	bool selected;
	string text;
	void (*command)(void);
};

#endif	/* _MENUITEMCOMMAND_HPP */

