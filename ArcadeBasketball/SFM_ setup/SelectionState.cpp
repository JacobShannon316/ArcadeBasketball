#pragma once
#include "SelectionState.h"
#include "SpriteNode.h"
#include "Button.h"
#include "Utility.h"
#include "MusicPlayer.h"
#include "ResourceHolder.h"
#include "Player.h"
#include <iostream>
#include <fstream>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

SelectionState::SelectionState(StateStack& stack, Context context)
	: State(stack, context)
	, mGUIContainer()
{
	sf::Texture& texture = context.textures->get(Textures::CharacterSelectionScreen);
	mBackgroundSprite.setTexture(texture);
	sf::Vector2f windowSize(context.window->getSize());

	auto AnastasiiaButton = std::make_shared<GUI::Button>(context);
	AnastasiiaButton->setPosition(0.5f * windowSize.x - 100, 0.4f * windowSize.y);
	AnastasiiaButton->setText("Anastasiia");
	AnastasiiaButton->setCallback([this, context]()
		{
			setHeadPath("AnastasiiaHead.png");
		});
	
	headShow1.setTexture(context.textures->get(Textures::AnastasiiaHeadButtonSelection));
	headShow1.setPosition(0.5f * windowSize.x, 0.4f * windowSize.y);

	auto AlejandroButton = std::make_shared<GUI::Button>(context);
	AlejandroButton->setPosition(0.5f * windowSize.x - 100, 0.4f * windowSize.y + 110);
	AlejandroButton->setText("Alejandro");
	AlejandroButton->setCallback([this]()
		{
			setHeadPath("AlejandroHead.png");
		});
	headShow2.setTexture(context.textures->get(Textures::AlejandroHeadButtonSelection));
	headShow2.setPosition(0.5f * windowSize.x, 0.4f * windowSize.y + 110);

	auto JoshButton = std::make_shared<GUI::Button>(context);
	JoshButton->setPosition(0.5f * windowSize.x - 100, 0.4f * windowSize.y + 220);
	JoshButton->setText("Josh");
	JoshButton->setCallback([this]()
		{
			setHeadPath("JoshHead.png");
		});
	headShow3.setTexture(context.textures->get(Textures::JoshHeadButtonSelection));
	headShow3.setPosition(0.5f * windowSize.x, 0.4f * windowSize.y + 220);

	auto JacobButton = std::make_shared<GUI::Button>(context);
	JacobButton->setPosition(0.5f * windowSize.x - 100, 0.4f * windowSize.y + 330);
	JacobButton->setText("Jacob");
	JacobButton->setCallback([this]()
		{
			setHeadPath("JacobHead.png");
		});
	headShow4.setTexture(context.textures->get(Textures::JacobHeadButton));
	headShow4.setPosition(0.5f * windowSize.x, 0.4f * windowSize.y + 330);
	
	auto CowButton = std::make_shared<GUI::Button>(context);
	CowButton->setPosition(0.5f * windowSize.x - 100, 0.4f * windowSize.y + 440);
	CowButton->setText("Mystery");
	CowButton->setCallback([this]()
		{
			setHeadPath("CowHead.png");
		});
	headShow5.setTexture(context.textures->get(Textures::CowHeadButtonSelection));
	headShow5.setPosition(0.5f * windowSize.x, 0.4f * windowSize.y + 440);

	mGUIContainer.pack(AnastasiiaButton);
	mGUIContainer.pack(AlejandroButton);
	mGUIContainer.pack(JoshButton);
	mGUIContainer.pack(JacobButton);
	mGUIContainer.pack(CowButton);

	// Play menu theme
	//context.music->play(Music::MenuTheme);
}

void SelectionState::draw()
{
	sf::RenderWindow& window = *getContext().window;

	window.setView(window.getDefaultView());

	window.draw(mBackgroundSprite);
	window.draw(mGUIContainer);
	window.draw(headShow1);
	window.draw(headShow2);
	window.draw(headShow3);
	window.draw(headShow4);
	window.draw(headShow5);

}

bool SelectionState::update(sf::Time)
{
	if (index == 4)
	{
		saveHeadPath();
		requestStackPop();
		requestStackPush(States::Game);
		
	}
	return true;
}

bool SelectionState::handleEvent(const sf::Event& event)
{
	mGUIContainer.handleEvent(event);
	return false;
}

void SelectionState::saveHeadPath()
{
	std::ofstream myfile;
	myfile.open("HeadPath.txt");
	myfile.clear();
	myfile << headPath1 + "\n"
		+ headPath2 + "\n"
		+ headPath3 + "\n"
		+ headPath4 + "\n";
	myfile.close();

}

void SelectionState::setHeadPath(std::string path)
{
	switch (index)
	{
	case 0:
		headPath1 = path;
		break;
	case 1:
		headPath2 = path;
		break;
	case 2:
		headPath3 = path;
		break;
	default:
		headPath4 = path;
		break;
	}
	++index;
}
