#pragma once
#include "Command.h"

#include <SFML/Window/Event.hpp>

#include <map>


class CommandQueue;

class Player
{
public:
	enum Action
	{
		MoveLeft,
		MoveRight,
		MoveUp,
		MoveDown,
		AttackAndShoot,
		Pass,
		ActionCount
	};

	enum MissionStatus
	{
		MissionRunning,
		MissionSuccess,
		MissionFailure
	};


public:
	Player();

	void					handleEvent(const sf::Event& event, CommandQueue& commands);
	void					handleRealtimeInput(CommandQueue& commands);

	void					assignKey(Action action, sf::Keyboard::Key key);
	sf::Keyboard::Key		getAssignedKey(Action action) const;

	void 					setMissionStatus(MissionStatus status);
	MissionStatus 			getMissionStatus() const;

private:
	void					initializeActions();
	static bool				isRealtimeAction(Action action);


private:
	std::map<sf::Keyboard::Key, Action>		mKeyBinding;
	std::map<Action, Command>				mActionBinding;
	MissionStatus 							mCurrentMissionStatus;
};