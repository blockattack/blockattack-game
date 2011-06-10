/* 
 * File:   MenuItem.hpp
 * Author: poul
 *
 * Created on 11. marts 2011, 20:41
 */

#ifndef _MENUITEM_HPP
#define	_MENUITEM_HPP

#include <string>

using namespace std;

class MenuItem {
    virtual void SetSelected(bool selected) = 0;
    virtual void Activated() = 0;
    virtual string GetText() = 0;
    virtual void GetType() = 0;
    virtual void SetText(string text) = 0;
};


#endif	/* _MENUITEM_HPP */

