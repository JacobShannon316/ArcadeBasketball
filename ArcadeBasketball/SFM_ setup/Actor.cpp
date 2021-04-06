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
	case Type::team_1_Player:
		return Category::Player;
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

void Actor::stuff()
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
				direction_ = Direction::Right;
			else
				direction_ = Direction::Left;
		}
	}
	else
	{
		if (state_ != State::Dead) {
			if (direction_ == Direction::Left && getVelocity().x > 0)
				direction_ = Direction::Right;
			if (direction_ == Direction::Right && getVelocity().x < 0)
				direction_ = Direction::Left;
		}
		
	}
}

bool Actor::isMarkedForRemoval() const
{
	return false;
}

void Actor::attack()
{
	attack_ = true;
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

void Actor::giveBall(Actor* ball)
{
	
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

Actor* Actor::getPlayer()
{
	return player;
}

Actor* Actor::getBall()
{
	return ball;
}


void Actor::dropBall()
{
	ball->setPlayer(nullptr);
	ball = nullptr;
}

void Actor::moveTo(sf::Vector2f position)
{
	//assert(movingToo);
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
	std::uniform_int_distribution<unsigned int> randomXShooting{ 600,1000 };
	std::uniform_int_distribution<unsigned int> randomYShooting{ 1,450 };
	int randomXPosition = randomXShooting(rEng);
	int randomYPosition = randomYShooting(rEng);

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

Actor::Direction Actor::getDirection()
{
	return direction_;
}


void Actor::updateStates()
{
	if (isDestroyed())
		state_ = Actor::State::Dead;

	if (state_ == Actor::State::Attack && animations_[state_].isFinished())
	{
		if (ball != nullptr)
		{
			ball->shoot(direction_);
			dropBall();
		}
		state_ = Actor::State::Walk;
	}
	if (Actor::State::score == state_ && animations_[state_].isFinished())
	{
		state_ = Actor::State::Idle;
	}

	if (attack_ && state_ != Actor::State::Attack) {
		state_ = Actor::State::Attack;
		animations_[state_].restart();
		attack_ = false;
	}
	if (state_ == Actor::State::Idle && length(getVelocity()) > 0.1f)
		state_ = Actor::State::Walk;
}

void Actor::updateDirection(sf::Time dt)
{
	auto rec = animations_.at(state_).update(dt);
	if (getCategory() == Category::Ball)
	{

		stuff();
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

	if (state_ != State::Dead)
		Entity::updateCurrent(dt, commands);

	updateMovementPattern(dt);

	if (Actor::type_ == Actor::Type::ball)
		updateMovement(dt);

	if (ball != nullptr)
		ball->setPosition(this->getPosition());

	if (type_ == Actor::Type::ball) 
		updateBallMovement();
}

void Actor::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(sprite_, states);
}
