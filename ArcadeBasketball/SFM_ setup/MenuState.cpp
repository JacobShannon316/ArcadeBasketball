#include "MenuState.h"
#include "Button.h"
#include "Utility.h"
#include "MusicPlayer.h"
#include "ResourceHolder.h"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>


MenuState::MenuState(StateStack& stack, Context context)
	: State(stack, context)
	, mGUIContainer()
{
	sf::Texture& texture = context.textures->get(Textures::TitleScreen);
	mBackgroundSprite.setTexture(texture);
	sf::Vector2f windowSize(context.window->getSize());

	auto playButton = std::make_shared<GUI::Button>(context);
	playButton->setPosition(0.5f * windowSize.x - 50, 0.4f * windowSize.y + 75);
	playButton->setText("Play");
	playButton->setCallback([this]()
		{
			requestStackPop();
			requestStackPush(States::Selection);
		});

	auto settingsButton = std::make_shared<GUI::Button>(context);
	settingsButton->setPosition(0.5f * windowSize.x - 50, 0.4f * windowSize.y + 185);
	settingsButton->setText("Settings");
	settingsButton->setCallback([this]()
		{
			requestStackPush(States::Settings);
		});

	auto exitButton = std::make_shared<GUI::Button>(context);
	exitButton->setPosition(0.5f * windowSize.x - 50, 0.4f * windowSize.y + 295);
	exitButton->setText("Exit");
	exitButton->setCallback([this]()
		{
			requestStackPop();
		});

	mGUIContainer.pack(playButton);
	mGUIContainer.pack(settingsButton);
	mGUIContainer.pack(exitButton);

	// Play menu theme
	context.music->play(Music::MenuTheme);
}

void MenuState::draw()
{
	sf::RenderWindow& window = *getContext().window;

	window.setView(window.getDefaultView());

	window.draw(mBackgroundSprite);
	window.draw(mGUIContainer);
}

bool MenuState::update(sf::Time)
{
	return true;
}

bool MenuState::handleEvent(const sf::Event& event)
{
	mGUIContainer.handleEvent(event);
	return false;
}