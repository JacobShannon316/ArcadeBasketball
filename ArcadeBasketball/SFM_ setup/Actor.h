#pragma once

#include "Entity.h"
#include "ResourceHolder.h"
#include "ResourceIdentifiers.h"
#include "Animation2.h"
#include "TextNode.h"

#include <SFML/Graphics/Sprite.hpp>

class Actor : public Entity
{
public:
	enum class Type {
		team_1_Player, team_2_Player, ball, hoop, shadow
	};

	enum class State {
		Attack, Dead, Idle, Rise, Walk, Run, Jump, count, score
	};
	enum class GameState {
		Offense, Defense, FreeBall, Reset
	};
	enum class Team {
		Team_1, Team_2, None
	};

	enum class Direction {
		Left, Right, Up, Down
	};

public:
	Actor(Type type, const TextureHolder& textures, const FontHolder& fonts);
	~Actor() = default;

	unsigned int			getCategory() const override;
	sf::FloatRect			getBoundingRect() const override;
	float					getMaxSpeed() const;

	void					stuff();
	bool					isMarkedForRemoval() const override;
	void					attack();
	void					setState(State state);
	void					setGameState(GameState gameState);
	void					setDirection(Direction direction);
	void					setRebounding(bool rebounding);

	void					giveBall(Actor* ball);
	void					setPlayer(Actor* player);
	Actor*					getPlayer();
	Actor*					getBall();
	void					dropBall();
	

	void					moveTo(sf::Vector2f position);
	bool					isMovingToo();
	void					updateMovement(sf::Time dt);
	void					updateAIMovement();
	void					stopMovingToo();
	void					ballMove(sf::Vector2f position);
	void					shoot(Direction direction);
	bool					shotResult();

	void					rebound(Actor* activeBall);
	bool					isScore();

	void					setTeam(Team team);
	Actor::Team				getTeam();

	sf::Vector2f			getNewShootingPosition();
	sf::Vector2f			getShootingPosition() const;
	bool					isShootingPosition() const;

	Actor::State getState() const;
	Actor::Direction getDirection();



private:
	void					updateStates();
	void					updateDirection(sf::Time dt);
	void					updateBallMovement();
	void					updateCurrent(sf::Time dt, CommandQueue& commands) override;
	void					drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;
	void					updateMovementPattern(sf::Time dt);

private:
	Type								type_;
	State								state_;
	mutable sf::Sprite					sprite_;
	std::map<State, Animation2>			animations_;
	Team								team_;
	Direction							direction_;
	GameState							gameState_;
	Actor*								ball;
	Actor*								player;

	float								travelDistance_;
	std::size_t							directionIndex_;
	bool								attack_;
	bool								movingToo = false;
	bool								rebounding = false;
	bool								hasShootingPosition = false;
	bool								score;
	sf::Vector2f						targetDirection;
	sf::Vector2f						moveTooPosition;
	sf::Vector2f						shootingPosition;
	const sf::Vector2f					TEAM_1_HOOP_SPAWN = sf::Vector2f(3450.f, -20.f);
	const sf::Vector2f					TEAM_2_HOOP_SPAWN = sf::Vector2f(600.f, -20.f);

};

