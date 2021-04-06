#pragma once
#include "World.h"
#include "Projectile.h"
#include "Pickup.h"
#include "TextNode.h"
#include "ParticleNode.h"
#include "SoundNode.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

std::mt19937 REng{ std::random_device{}() };
float getRightLineBound(float y)
{
	float x;
	float slope = 0.6;
	x = (y + 1850) / slope;
	return x;
}
float getLeftLineBound(float y)
{
	float x;
	float slope = -0.6;
	x = (y - 580) / slope;
	return x;
}

World::World(sf::RenderTarget& outputTarget, FontHolder& fonts, SoundPlayer& sounds)
	: mTarget(outputTarget)
	, mSceneTexture()
	, mWorldView(outputTarget.getDefaultView())
	, mTextures()
	, mFonts(fonts)
	, mSounds(sounds)
	, mSceneGraph()
	, mSceneLayers()
	, mWorldBounds(0.f, 0.f, 4000, 1865.f)
	, mSpawnPosition(1500, 35)
	, mScrollSpeed(-50.f)
	, mPlayerActor(nullptr)
	, mEnemySpawnPoints()
	, mActiveEnemies()
{
	mSceneTexture.create(mTarget.getSize().x, mTarget.getSize().y);

	loadTextures();
	buildScene();

	// Prepare the view
	mWorldView.setCenter(mSpawnPosition);
}

void World::update(sf::Time dt)
{
	// Scroll the world, reset player velocity
	//mWorldView.move(0.f, mScrollSpeed * dt.asSeconds());
	mPlayerActor->setVelocity(0.f, 0.f);
	

	// Setup commands to destroy entities, and guide missiles
	updateAI();
	adaptNPCPosition();

	// Forward commands to scene graph, adapt velocity (scrolling, diagonal correction)
	while (!mCommandQueue.isEmpty())
		mSceneGraph.onCommand(mCommandQueue.pop(), dt);
	adaptActorVelocity(mPlayerActor);

	// Collision detection and response (may destroy entities)
	handleCollisions();

	// Remove all destroyed entities, create new ones
	mSceneGraph.removeWrecks();

	// Regular update step, adapt position (correct if outside view)
	mSceneGraph.update(dt, mCommandQueue);
	adaptPlayerPosition();

	updateBallState();
	updateSounds();
 	adaptWorldView();
}

void World::draw()
{
		mTarget.setView(mWorldView);
		mTarget.draw(mSceneGraph);
}

CommandQueue& World::getCommandQueue()
{
	return mCommandQueue;
}

bool World::hasAlivePlayer() const
{
	return !mPlayerActor->isMarkedForRemoval();
}

void World::loadTextures()
{
	mTextures.load(Textures::Background, "Media/Textures/temp_background.jpg");
	mTextures.load(Textures::Court, "Media/Textures/basketballCourt.png");
	mTextures.load(Textures::Hero2, "Media/Textures/hero2.png");
	mTextures.load(Textures::Hoop, "Media/Textures/Scoring_Animation2.png");
	mTextures.load(Textures::Ball, "Media/Textures/basketball.png");



}

void World::adaptPlayerPosition()
{
	// Keep player's position inside the screen bounds, at least borderDistance units from the border
	sf::FloatRect viewBounds = getViewBounds();
	const float borderDistance = 40.f;

	sf::Vector2f position = mPlayerActor->getPosition();
	position.x = std::max(position.x, getLeftLineBound(mPlayerActor->getPosition().y) + borderDistance);
	position.x = std::min(position.x, getRightLineBound(mPlayerActor->getPosition().y) - borderDistance);
	position.y = std::max(position.y, -10.f);
	position.y = std::min(position.y, 455.f);
	mPlayerActor->setPosition(position);
}

void World::adaptNPCPosition()
{
	sf::FloatRect viewBounds(mWorldView.getCenter() - mWorldView.getSize() / 2.f, mWorldView.getSize());
	const float borderDistance = 40.f;

	Command adaptPosition;
	adaptPosition.category = Category::AI;
	adaptPosition.action = derivedAction<Actor>([this](Actor& AI, sf::Time)
		{

			sf::FloatRect viewBounds(mWorldView.getCenter() - mWorldView.getSize() / 2.f, mWorldView.getSize());
			const float borderDistance = 40.f;


			sf::Vector2f position = AI.getPosition();

			position.x = std::max(position.x, getLeftLineBound(AI.getPosition().y) + borderDistance);
			position.x = std::min(position.x, getRightLineBound(AI.getPosition().y) - borderDistance);
			position.y = std::max(position.y, -10.f);
			position.y = std::min(position.y, 455.f);

			AI.setPosition(position);
		});

	mCommandQueue.push(adaptPosition);
}

void World::adaptActorVelocity(Actor* actor)
{
	sf::Vector2f velocity = actor->getVelocity();

	// If moving diagonally, reduce velocity (to have always same velocity)
	if (velocity.x != 0.f && velocity.y != 0.f)
		actor->setVelocity(velocity / std::sqrt(2.f));

	// Add scrolling velocity
	
}

void World::adaptWorldView()
{
	if (mPlayerActor->getPosition().x > 1200 && mPlayerActor->getPosition().x < 3000)
	mWorldView.setCenter(mPlayerActor->getPosition().x ,mWorldView.getCenter().y);
}

bool matchesCategories(SceneNode::Pair& colliders, Category::Type type1, Category::Type type2)
{
	unsigned int category1 = colliders.first->getCategory();
	unsigned int category2 = colliders.second->getCategory();

	// Make sure first pair entry has category type1 and second has type2
	if (type1 & category1 && type2 & category2)
	{
		return true;
	}
	else if (type1 & category2 && type2 & category1)
	{
		std::swap(colliders.first, colliders.second);
		return true;
	}
	else
	{
		return false;
	}
}

void World::handleCollisions()
{
	std::set<SceneNode::Pair> collisionPairs;
	mSceneGraph.checkSceneCollision(mSceneGraph, collisionPairs);

	for (auto pair : collisionPairs)
	{
		if (matchesCategories(pair, Category::Hoop, Category::Ball)) {
			auto& first = static_cast<Actor&>(*pair.first);
			auto& second = static_cast<Actor&>(*pair.second);

			second.stopMovingToo();
			if (second.isScore())
			{
				std::uniform_int_distribution<unsigned int> random{ 1, 2 };

				int randomSound = random(REng);
				if (randomSound == 1)
					mSounds.play(SoundEffect::Swish);
				else
					mSounds.play(SoundEffect::Rim);

				first.setState(Actor::State::score);
				second.setPosition(BALL_SPAWN);
				if (first.getDirection() == Actor::Direction::Left)
					game.Team_1_Score(2);
				else
					game.Team_2_Score(2);
			}
			else
			{
				mSounds.play(SoundEffect::Rim);
				first.rebound(ActiveBall);
				freeBall = true;
			}
		}
		if (matchesCategories(pair, Category::Player, Category::AI))
		{ /*
			auto& first = static_cast<Actor&>(*pair.first);
			auto& second = static_cast<Actor&>(*pair.second);
			if (first.getDirection() == Actor::Direction::Left && first.getPosition().x > second.getPosition().x)
			{
				first.setPosition(second.getPosition().x + 50, first.getPosition().y);
				second.setPosition(first.getPosition().x - 50, second.getPosition().y);
			}
			if (first.getDirection() == Actor::Direction::Left && first.getPosition().x < second.getPosition().x)
			{
				first.setPosition(second.getPosition().x - 50, first.getPosition().y);
				second.setPosition(first.getPosition().x + 50, second.getPosition().y);
			}
			*/
		}
		if (matchesCategories(pair, Category::AI, Category::Ball)) {
			auto& first = static_cast<Actor&>(*pair.first);
			auto& second = static_cast<Actor&>(*pair.second);

			if (first.getBall() == nullptr)
			{
				first.stopMovingToo();
				first.giveBall(&second);
				second.setTeam(first.getTeam());
				freeBall = false;
			}
		}
		else if (matchesCategories(pair, Category::Player, Category::Ball)) {
			auto& first = static_cast<Actor&>(*pair.first);
			auto& second = static_cast<Actor&>(*pair.second);

			if (first.getBall() == nullptr)
			{
				first.giveBall(&second);
				second.setTeam(first.getTeam());
				freeBall = false;
			}

		}
	}
}

void World::updateSounds()
{
	// Set listener's position to player position
	mSounds.setListenerPosition(mPlayerActor->getWorldPosition());

	// Remove unused sounds
	mSounds.removeStoppedSounds();
	
}

void World::updateAI()
{
	std::for_each(AI_Players.cbegin(), AI_Players.cend(), [&](Actor* AI) 
		{
			if (freeBall)
			{
				AI->setGameState(Actor::GameState::FreeBall);
				AI->moveTo(ActiveBall->getPosition());
			}
			else if (AI->getTeam() != ActiveBall->getTeam())
			{
				AI->setGameState(Actor::GameState::Defense);
				sf::Vector2f MoveTooPosition((100 + ActiveBall->getPosition().x), ActiveBall->getPosition().y);

				if (InArea(MoveTooPosition, AI))
					AI->stopMovingToo();
				else
					AI->moveTo(MoveTooPosition);
			}
			else if (AI->getTeam() == ActiveBall->getTeam())
			{
				AI->setGameState(Actor::GameState::Offense);

				if (AI->getBall() != nullptr)
				{
					sf::Vector2f moveTooPosition;
					if (AI->isShootingPosition())
						moveTooPosition = sf::Vector2f(AI->getShootingPosition());
					else
					{
						moveTooPosition = sf::Vector2f(AI->getNewShootingPosition());
						AI->stopMovingToo();
					}
					if (InArea(moveTooPosition, AI))
					{
						AI->attack();
					}
					else if (!AI->isMovingToo())
					{
						AI->moveTo(moveTooPosition);
					}
				}
			}
			
			AI->updateAIMovement();
		});
}

void World::updateBallState()
{
	if (mPlayerActor->getBall() != nullptr)
	{
		ActiveBall->setTeam(mPlayerActor->getTeam());
		return;
	}
	for (Actor* AI : AI_Players) 
	{
		if (AI->getBall() != nullptr) 
		{
			ActiveBall->setTeam(AI->getTeam());
			return;
		}
	}
	ActiveBall->setTeam(Actor::Team::None);
	freeBall = true;
}

void World::buildScene()
{
	// Initialize the different layers
	for (std::size_t i = 0; i < LayerCount; ++i)
	{
		Category::Type category = (i == World::Layer::Player) ? Category::SceneAirLayer : Category::None;

		SceneNode::Ptr layer(new SceneNode(category));
		mSceneLayers[i] = layer.get();

		mSceneGraph.attachChild(std::move(layer));
	}

	// Prepare the tiled background
	sf::Texture& CourtTexture = mTextures.get(Textures::Court);
	CourtTexture.setRepeated(true);

	sf::Texture& BackgroundTexture = mTextures.get(Textures::Background);
	BackgroundTexture.setRepeated(false);

	float viewHeight = mWorldView.getSize().y;
	sf::IntRect textureRect(mWorldBounds);
	textureRect.height += static_cast<int>(viewHeight);

	// Add the background sprite to the scene
	
	std::unique_ptr<SpriteNode> background(new SpriteNode(BackgroundTexture, textureRect));
	background->setPosition(mWorldBounds.left, mWorldBounds.top - viewHeight);
	mSceneLayers[Background]->attachChild(std::move(background));
	

	std::unique_ptr<SpriteNode> court(new SpriteNode(CourtTexture, textureRect));
	court->setPosition(mWorldBounds.left, mWorldBounds.top - viewHeight + 570);
	mSceneLayers[Court]->attachChild(std::move(court));

	// Add player's aircraft
	std::unique_ptr<Actor> player(new Actor(Actor::Type::team_1_Player, mTextures, mFonts));
	mPlayerActor = player.get();
	mPlayerActor->setPosition(mSpawnPosition);
	mPlayerActor->setTeam(Actor::Team::Team_1);
	mSceneLayers[World::Layer::Player ]->attachChild(std::move(player));

	std::unique_ptr<Actor> AI_player_3(new Actor(Actor::Type::team_2_Player, mTextures, mFonts));
	AI_player_3->setPosition(2500.f, 35.f);
	AI_Players.push_back(AI_player_3.get());
	AI_player_3->setTeam(Actor::Team::Team_2);
	mSceneLayers[World::Layer::Player]->attachChild(std::move(AI_player_3));

	std::unique_ptr<Actor> team_1_Hoop(new Actor(Actor::Type::hoop, mTextures, mFonts));
	team_1_Hoop->setPosition(TEAM_1_HOOP_SPAWN);
	mSceneLayers[World::Layer::Hoop]->attachChild(std::move(team_1_Hoop));

	std::unique_ptr<Actor> team_2_Hoop(new Actor(Actor::Type::hoop, mTextures, mFonts));
	team_2_Hoop->setPosition(TEAM_2_HOOP_SPAWN);
	team_2_Hoop->setDirection(Actor::Direction::Left);
	mSceneLayers[World::Layer::Hoop]->attachChild(std::move(team_2_Hoop));

	std::unique_ptr<Actor> ball(new Actor(Actor::Type::ball, mTextures, mFonts));
	ball->setPosition(BALL_SPAWN);
	ball->scale(0.1f, 0.1f);
	ball->setTeam(Actor::Team::None);
	ActiveBall = ball.get();
	mSceneLayers[World::Layer::Court]->attachChild(std::move(ball));

	// Add enemy aircraft
}


void World::destroyEntitiesOutsideView()
{
	Command command;
	command.category = Category::Projectile | Category::EnemyAircraft;
	command.action = derivedAction<Entity>([this](Entity& e, sf::Time)
		{
			if (!getBattlefieldBounds().intersects(e.getBoundingRect()))
				e.remove();
		});

	mCommandQueue.push(command);
}

sf::FloatRect World::getViewBounds() const
{
	return sf::FloatRect(mWorldView.getCenter() - mWorldView.getSize() / 2.f, mWorldView.getSize());
}

sf::FloatRect World::getBattlefieldBounds() const
{
	// Return view bounds + some area at top, where enemies spawn
	sf::FloatRect bounds = getViewBounds();
	bounds.top -= 100.f;
	bounds.height += 100.f;

	return bounds;
}

bool World::InArea(sf::Vector2f area, Actor* actor)
{
	float actorX = actor->getPosition().x;
	float actorY = actor->getPosition().y;
	const float AREA = 50;

	if ((actorX > area.x - AREA && actorX < area.x + AREA) && (actorY > area.y - AREA && actorY < area.y + AREA))
	{
		return true;
	}
	return false;
}
