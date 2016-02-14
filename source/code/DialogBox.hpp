/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DialogBox.hpp
 * Author: poul
 *
 * Created on 14. februar 2016, 14:56
 */

#ifndef DIALOGBOX_HPP
#define DIALOGBOX_HPP

#include "sago/GameStateInterface.hpp"
#include <string>
#include "ReadKeyboard.h"
#include <memory>
#include "scopeHelpers.hpp"

bool OpenDialogbox(int x, int y, std::string& name);

class DialogBox : public sago::GameStateInterface {
public:
	DialogBox(int x, int y, const std::string& name);
	DialogBox(const DialogBox& orig) = delete;
	virtual ~DialogBox();
	
	bool IsActive() override;
	void Draw(SDL_Renderer* target) override;
	void ProcessInput(const SDL_Event& event, bool &processed) override;
    void SetName(const std::string& name);
    std::string GetName() const;
    bool IsUpdated() const;
private:
	bool isActive = true;
	std::shared_ptr<ReadKeyboard> rk;
	int x; 
	int y;
	std::string name;
	bool updated = false;
	SDL_TextInput textInputScope;
};

#endif /* DIALOGBOX_HPP */

