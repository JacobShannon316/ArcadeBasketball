#pragma once

#include "Entity.h"
#include "ResourceHolder.h"
#include "ResourceIdentifiers.h"
#include "Animation2.h"
#include "TextNode.h"
#include "SpriteNode.h"

#include <SFML/Graphics/Sprite.hpp>

class Actor : public Entity
{
public:
	enum class Type {
		Player, team_1_Player, team_2_Player, ball, hoop, head
	};

	enum class State {
		Attack, Shoot, Idle, Rise, Walk, IdleWithBall, WalkWithBall, count, score
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

	bool					isMarkedForRemoval() const override;
	void					attack();
	void					pass();
	void					passBall();
	void					setHead(SpriteNode* head);
	void					setState(State state);
	void					setGameState(GameState gameState);
	void					setDirection(Direction direction);
	void					setRebounding(bool rebounding);
	void					setShooting(bool shooting);
	void					setPassing(bool passing);

	void					giveBall(Actor* ball);
	void					setPlayer(Actor* player);
	void					setAssignment(Actor* assignment);
	void					setTeamate(Actor* teammate);
	Actor*					getAssignment();
	Actor*					getPlayer();
	Actor*					getBall();
	Actor*					getTeammate();
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

	Actor::State			getState() const;
	Actor::GameState		getGameState() const;
	Actor::Direction		getDirection();

	bool					isAttack() const;
	bool					isShooting() const;
	bool					isRebounding() const;
	bool					isPassing() const;

private:
	void					updateStates();
	void					updateDirection(sf::Time dt);
	void					updateBallMovement();
	void					updateCurrent(sf::Time dt, CommandQueue& commands) override;
	void					updateHead();
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
	Actor*								assignment;
	Actor*								teammate;
	SpriteNode*							head;

	float								travelDistance_;
	std::size_t							directionIndex_;
	bool								attack_;
	bool								movingToo = false;
	bool								rebounding = false;
	bool								passing = false;
	bool								shooting = false;
	bool								hasShootingPosition = false;
	bool								score;
	sf::Vector2f						targetDirection;
	sf::Vector2f						moveTooPosition;
	sf::Vector2f						shootingPosition;
	const sf::Vector2f					TEAM_1_HOOP_SPAWN = sf::Vector2f(3450.f, -20.f);
	const sf::Vector2f					TEAM_2_HOOP_SPAWN = sf::Vector2f(600.f, -20.f);

};

