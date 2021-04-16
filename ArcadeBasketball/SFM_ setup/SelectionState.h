#pragma once
#include "State.h"
#include "Container.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>


class SelectionState : public State
{
public:
	SelectionState(StateStack& stack, Context context);

	virtual void			draw();
	virtual bool			update(sf::Time dt);
	virtual bool			handleEvent(const sf::Event& event);

	void					saveHeadPath();
	void					setHeadPath(std::string path);

private:
	sf::Sprite headShow1;
	sf::Sprite headShow2;
	sf::Sprite headShow3;
	sf::Sprite headShow4;
	sf::Sprite headShow5;

	sf::Sprite					mBackgroundSprite;
	GUI::Container				mGUIContainer;
	int							index = 0;
	
};

