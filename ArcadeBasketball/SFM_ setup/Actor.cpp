#pragma once
#include "Actor.h"
#include "ResourceHolder.h"
#include "ResourceIdentifiers.h"
#include "jsonFrameParser.h"
#include "Utility.h"
#include "DataTables.h"
#include "TextNode.h"

#include <SFML/Graphics/RenderTarget.hpp>
#include <random>
#include <iostream>
namespace 
{
	const std::map<Actor::Type, ActorData> TABLE = InitializeActorData();
}

std::mt19937 rEng{ std::random_device{}() };
std::uniform_int_distribution<unsigned int> randomXL{ 900,1500 };
std::uniform_int_distribution<unsigned int> randomXR{ 2550,3150 };
std::uniform_int_distribution<unsigned int> randomY{ 1,450 };

Actor::Actor(Type type, const TextureHolder& textures, const FontHolder& fonts)
	: Entity(100)
	, type_(type)
	, state_(State::Idle)
	, sprite_(textures.get(TABLE.at(type).texture))
	, direction_(Direction::Right)
	, travelDistance_(0.f)
	, directionIndex_(0)
	, attack_(false)
{
	for (auto a : TABLE.at(type).animations)
	{
		animations_[a.first] = a.second;
	}
	gameState_ == GameState::FreeBall;
	sprite_.setTextureRect(sf::IntRect());
	centerOrigin(sprite_);

	std::unique_ptr<TextNode> gameScore(new TextNode(fonts, ""));
	attachChild(std::move(gameScore));
}

unsigned int Actor::getCategory() const
{
	switch (type_)
	{
	case Type::ball:
		return Category::Ball;
		break;
	case Type::hoop:
		return Category::Hoop;
		break;
	case Type::Player:
		return Category::Player;
		break;
	case Type::head:
		return Category::Head;
		break;
	default:
		return Category::AI;
		break;
	}
}

void Actor::updateMovementPattern(sf::Time dt)
{
	auto directions = TABLE.at(type_).directions;

	if (!directions.empty()) {
		if (travelDistance_ > (directions[directionIndex_].distance))
		{
			directionIndex_ = (++directionIndex_) % directions.size();
			travelDistance_ = 0;
		}

		float radians = toRadian(directions[directionIndex_].angle + 90.f);
		float vx = getMaxSpeed() * std::cos(radians);
		float vy = getMaxSpeed() * std::sin(radians);

		setVelocity(vx, vy);
		travelDistance_ += getMaxSpeed() * dt.asSeconds();
	}
}


sf::FloatRect Actor::getBoundingRect() const
{
	auto box = getWorldTransform().transformRect(sprite_.getGlobalBounds());
	if (type_ == Type::hoop || type_ == Type::ball)
	{
		box.width -= 80;
		box.height -= 40;
		box.left += 15;
	}
	else 
	{
		box.width -= 30; // tighten up bounding box for more realistic collisions
		box.left += 15;
	}
	
	return box;
}

float Actor::getMaxSpeed() const
{
	return TABLE.at(type_).speed;
}

bool Actor::isMarkedForRemoval() const
{
	return false;
}

void Actor::attack()
{
	if (ball != nullptr) 
	{
		state_ = Actor::State::Shoot;
		animations_[state_].restart();
	}
	else 
	{
		attack_ = true;
		hasShootingPosition = false;
	}
}

void Actor::pass()
{
	if (ball != nullptr)
	{
		passBall();
	}
	else if (teammate->ball != nullptr)
	{
		teammate->passBall();
	}
}

void Actor::passBall()
{
	if (ball != nullptr)
	{
		ball->setPassing(true);

		if (teammate->getPosition().y > getPosition().y)
			ball->setPosition(getPosition().x, getPosition().y + 125);
		else
			ball->setPosition(getPosition().x, getPosition().y - 125);

		ball->moveTo(teammate->getPosition());
		dropBall();
	}
}

void Actor::setHead(SpriteNode* head)
{
	this->head = head;
}

void Actor::setState(State state)
{
	state_ = state;
	animations_[state_].restart();
}

void Actor::setDirection(Direction direction)
{
	this->direction_ = direction;
}

void Actor::setRebounding(bool rebounding)
{
	this->rebounding = rebounding;
}

void Actor::setShooting(bool shooting)
{
	this->shooting = shooting;
}

void Actor::setPassing(bool passing)
{
	this->passing = passing;
}

void Actor::giveBall(Actor* ball)
{
	ball->stopMovingToo();
	ball->setPassing(false);
	if (ball->getPlayer() != nullptr)
	{
		ball->getPlayer()->dropBall();
	}
	this->ball = ball; 
	ball->setPlayer(this);
}

void Actor::setPlayer(Actor* player)
{
	this->player = player;
}

void Actor::setAssignment(Actor* assignment)
{
	this->assignment = assignment;
}

void Actor::setTeamate(Actor* teammate)
{
	this->teammate = teammate;
}

Actor* Actor::getAssignment()
{
	return assignment;
}

Actor* Actor::getPlayer()
{
	return player;
}

Actor* Actor::getBall()
{
	return ball;
}

Actor* Actor::getTeammate()
{
	return teammate;
}


void Actor::dropBall()
{
	ball->setPlayer(nullptr);
	ball->setTeam(Actor::Team::None);
	ball = nullptr;
	state_ = Actor::State::Idle;
}

void Actor::moveTo(sf::Vector2f position)
{

	if (type_ != Type::ball)
	moveTooPosition = position;

	movingToo = true;
	targetDirection = unitVector(position - getWorldPosition());
}

bool Actor::isMovingToo()
{
	return movingToo;
}

void Actor::updateMovement(sf::Time dt)
{
	if (movingToo)
	{
		const float approachRate = 200.f;

		sf::Vector2f newVelocity = unitVector(approachRate * dt.asSeconds() * targetDirection + getVelocity());
		newVelocity *= getMaxSpeed();
		float angle = std::atan2(newVelocity.y, newVelocity.x);

		setRotation(toDegree(angle) + 90.f);
		setVelocity(newVelocity);
	}
	
}

void Actor::updateAIMovement()
{
	if (movingToo)
	{
		float vx = 0;
		float vy = 0;
		float differenceX = getPosition().x - moveTooPosition.x;
		float differenceY = getPosition().y - moveTooPosition.y;

		if (!differenceY)
		{
			vx = getMaxSpeed();
			if (differenceX > 0)
				vx *= -1;
		}
		if (differenceX < 0 && differenceY < 0)
		{
			float radians = std::cosf(differenceX / std::hypot(differenceX, differenceY));
			vx = getMaxSpeed() * std::acosf(radians);
			vy = getMaxSpeed() * std::asinf(radians);

		}
		else if (differenceX < 0)
		{
			float radians = std::cosf(differenceY / std::hypot(differenceX, differenceY));
			vx = getMaxSpeed() * std::asinf(radians);
			vy = getMaxSpeed() * std::acosf(radians);
			vy *= -1;
		}
		else if (differenceY < 0)
		{
			float radians = std::cosf(differenceX / std::hypot(differenceX, differenceY));
			vx = getMaxSpeed() * std::acosf(radians);
			vy = getMaxSpeed() * std::asinf(radians);
			vx *= -1;
		}
		else
		{
			float radians = std::cosf(differenceX / std::hypot(differenceX, differenceY));
			vx = getMaxSpeed() * std::acosf(radians);
			vy = getMaxSpeed() * std::asinf(radians);
			vx *= -1;
			vy *= -1;
		}

		setVelocity(vx, vy);
	}
	else
	{
		setVelocity(0, 0);
	}
}

void Actor::stopMovingToo()
{
	movingToo = false;
	setVelocity(0, 0);
}

void Actor::ballMove(sf::Vector2f position)
{
	moveTooPosition = position;
	moveTo(sf::Vector2f(((position.x + getPosition().x) / 2), -500));
	if (getPosition().x < position.x)
		direction_ = Direction::Right;
	else
		direction_ = Direction::Left;
	score = shotResult();
}

void Actor::shoot(Direction direction)
{
	shooting = true;
	setPosition(getPosition().x, getPosition().y - 100);
	if (direction == Direction::Right)
		ballMove(TEAM_1_HOOP_SPAWN);
	else
		ballMove(TEAM_2_HOOP_SPAWN);
}

bool Actor::shotResult()
{
	int distance = std::abs(std::hypot(moveTooPosition.x - getPosition().x, moveTooPosition.y - getPosition().y));
	std::uniform_int_distribution<unsigned int> random{ 0, 4000};
	int randomI = random(rEng);
	randomI += distance;
	if (randomI < 3000) 
	{
		return true;
	}
	return false;
}

void Actor::rebound(Actor* activeBall)
{
	activeBall->setRebounding(true);
	activeBall->setShooting(false);
	float randomXf;
	float randomYf;

	if (direction_ == Direction::Left)
		randomXf = randomXL(rEng);
	else
		randomXf = randomXR(rEng);

	randomYf = randomY(rEng);
	//randomYf -= 35;

	activeBall->setPosition(this->getPosition().x, this->getPosition().y - 175);
	activeBall->ballMove(sf::Vector2f(randomXf, randomYf));
}

bool Actor::isScore()
{
	return score;
}

void Actor::setTeam(Team team)
{
	this->team_ = team;
}

void Actor::setGameState(GameState gameState)
{
	this->gameState_ = gameState;
}

Actor::Team Actor::getTeam()
{
	return team_;
}

sf::Vector2f Actor::getNewShootingPosition()
{
	int randomXPosition;
	int randomYPosition;
	std::uniform_int_distribution<unsigned int> randomYShooting{ 1,450 };
	if (team_ == Actor::Team::Team_2)
	{
		std::uniform_int_distribution<unsigned int> randomXShooting{ 900,1300 };
		randomXPosition = randomXShooting(rEng);
		randomYPosition = randomYShooting(rEng);
	}
	 else
	{
		std::uniform_int_distribution<unsigned int> randomXShooting{ 2750,3150 };
		randomXPosition = randomXShooting(rEng);
		randomYPosition = randomYShooting(rEng);
	}

	shootingPosition = sf::Vector2f(randomXPosition, randomYPosition);
	hasShootingPosition = true;

	return shootingPosition;
}

sf::Vector2f Actor::getShootingPosition() const
{
	return shootingPosition;
}

bool Actor::isShootingPosition() const
{
	return hasShootingPosition;
}

Actor::State Actor::getState() const
{
	return state_;
}

Actor::GameState Actor::getGameState() const
{
	return gameState_;
}

Actor::Direction Actor::getDirection()
{
	return direction_;
}

bool Actor::isAttack() const
{
	return attack_;
}

bool Actor::isShooting() const
{
	return shooting;
}

bool Actor::isRebounding() const
{
	return rebounding;
}

bool Actor::isPassing() const
{
	return passing;
}


void Actor::updateStates()
{
	if (attack_)
	{
		attack_ = false;
	}
	if (state_ == Actor::State::Shoot && animations_[state_].isFinished())
	{
		if (ball != nullptr)
		{
			
			ball->shoot(direction_);
			hasShootingPosition = false;
			dropBall();
		}
		animations_[state_].restart();
		state_ = Actor::State::Walk;
	}
	if (Actor::State::score == state_ && animations_[state_].isFinished())
	{
		state_ = Actor::State::Idle;
	}

	if (ball != nullptr)
	{
		if (state_ == Actor::State::Idle || state_ == Actor::State::Walk)
			state_ = Actor::State::IdleWithBall;

		if (length(getVelocity()) > 0.1f)
			state_ = Actor::State::WalkWithBall;

		if (state_ == Actor::State::WalkWithBall && length(getVelocity()) < 0.1f)
			state_ = Actor::State::Idle;
	}
	else
	{
		if (state_ == Actor::State::Idle && length(getVelocity()) > 0.1f)
			state_ = Actor::State::Walk;
		if (state_ == Actor::State::Walk && length(getVelocity()) < 0.1f)
			state_ = Actor::State::Idle;
	}
}

void Actor::updateDirection(sf::Time dt)
{
	auto rec = animations_.at(state_).update(dt);
	if (getCategory() == Category::AI)
	{
		if (gameState_ != GameState::FreeBall)
		{
			if (gameState_ == GameState::Offense)
			{
				if (team_ == Team::Team_2)
				{
					direction_ = Direction::Left;
				}
				else
				{
					direction_ = Direction::Right;
				}
			}
			else
			{
				if (team_ == Team::Team_2)
					direction_ = Direction::Left;
				else
					direction_ = Direction::Right;
			}
		}
		else
		{
			if (direction_ == Direction::Left && getVelocity().x > 0)
				direction_ = Direction::Right;
			if (direction_ == Direction::Right && getVelocity().x < 0)
				direction_ = Direction::Left;
		}
	}
	else
	{
		if (direction_ == Direction::Left && getVelocity().x > 0)
			direction_ = Direction::Right;
		if (direction_ == Direction::Right && getVelocity().x < 0)
			direction_ = Direction::Left;
	}
	

	if (direction_ == Direction::Left)
		rec = flip(rec);

	sprite_.setTextureRect(rec);
	centerOrigin(sprite_);
}

void Actor::updateBallMovement()
{
	if (getPosition().y < -301)
	{
		stopMovingToo();
		setPosition(getPosition().x, -300);
		moveTo(moveTooPosition);
	}
	else if (rebounding && getPosition().y > moveTooPosition.y)
	{
		if (direction_ == Actor::Direction::Right && getPosition().x > moveTooPosition.x)
		{
			rebounding = false;
			stopMovingToo();
		}
		else if (direction_ == Actor::Direction::Left && getPosition().x < moveTooPosition.x)
		{
			rebounding = false;
			stopMovingToo();
		}
	}
}

void Actor::updateCurrent(sf::Time dt, CommandQueue& commands)
{
	updateStates();
	updateDirection(dt);

	Entity::updateCurrent(dt, commands);

	updateMovementPattern(dt);

	if (Actor::type_ == Actor::Type::ball)
		updateMovement(dt);

	if (ball != nullptr)
		ball->setPosition(this->getPosition());

	if (type_ == Actor::Type::ball) 
		updateBallMovement();
	updateHead();
}

void Actor::updateHead()
{
	if (head != nullptr)
	{
		head->setPosition(getPosition().x - 25, getPosition().y - 60);
	}
}

void Actor::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (type_ == Actor::Type::ball && team_ != Actor::Team::None) 
	{
		return;
	}
	target.draw(sprite_, states);
}
