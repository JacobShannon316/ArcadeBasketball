#pragma once
#include "World.h"
#include "Projectile.h"
#include "Pickup.h"
#include "TextNode.h"
#include "ParticleNode.h"
#include "SoundNode.h"
#include "Utility.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <iostream>
#include <fstream>

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

	getHeadPaths();
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
	adaptBallPosition();

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
	updateText();
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

bool World::isGameOver()
{
	if (game.isWinner())
	{
		return true;
	}
	return false;
}

void World::getHeadPaths()
{
	std::string line;
	std::ifstream myfile("HeadPath.txt");
	if (myfile.is_open())
	{
		getline(myfile, line);
		head1 = line;
		getline(myfile, line);
		head2 = line;
		getline(myfile, line);
		head3 = line; 
		getline(myfile, line);
		head4 = line;

		myfile.close();
	}
}

void World::loadTextures()
{
	mTextures.load(Textures::Background, "Media/Textures/temp_background.jpg");
	mTextures.load(Textures::Court, "Media/Textures/basketballCourt.png");
	mTextures.load(Textures::Hoop, "Media/Textures/Scoring_Animation2.png");
	mTextures.load(Textures::Ball, "Media/Textures/basketball.png");
	mTextures.load(Textures::RedTeam, "Media/Textures/RedPlayer.png");
	mTextures.load(Textures::BlueTeam, "Media/Textures/BluePlayer.png");

	mTextures.load(Textures::Head1, "Media/Textures/" + head1);
	mTextures.load(Textures::Head2, "Media/Textures/" + head2);
	mTextures.load(Textures::Head3, "Media/Textures/" + head3);
	mTextures.load(Textures::Head4, "Media/Textures/" + head4);

	//characters->clear();

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
	position.y = std::min(position.y, 430.f);
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
			position.y = std::min(position.y, 430.f);

			AI.setPosition(position);
		});

	mCommandQueue.push(adaptPosition);
}

void World::adaptBallPosition()
{
	sf::Vector2f position = ActiveBall->getPosition();

	if (position.x < 400 || position.x > 3600)
	{
		ActiveBall->stopMovingToo();
		ActiveBall->setPosition(BALL_SPAWN);
	}
	else if (position.y < -700 || position.y > 450)
	{
		ActiveBall->stopMovingToo();
		ActiveBall->setPosition(BALL_SPAWN);
	}
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
				score(&first);
			}
			else
			{
				rebound(&first);
			}
		}
		if (matchesCategories(pair, Category::Player, Category::AI))
		{ 
			auto& first = static_cast<Actor&>(*pair.first);
			auto& second = static_cast<Actor&>(*pair.second);
			if (first.getBall() != nullptr)
			{
				std::uniform_int_distribution<unsigned int> random{ 1, 200 };

				int randomSteal = random(REng);
				if (randomSteal == 1)
				{
					second.giveBall(ActiveBall);
				}
			}

		}
		if (matchesCategories(pair, Category::AI, Category::Ball)) {
			auto& first = static_cast<Actor&>(*pair.first);
			auto& second = static_cast<Actor&>(*pair.second);

			if (first.getBall() == nullptr && (!ActiveBall->isMovingToo() || ActiveBall->isPassing()))
			{
				if (ActiveBall->getTeam() == Actor::Team::None || first.isAttack())
				{
					first.stopMovingToo();
					first.giveBall(&second);
					second.setTeam(first.getTeam());
					freeBall = false;
				}
			}
		}
		if (matchesCategories(pair, Category::Player, Category::Ball)) {
			auto& first = static_cast<Actor&>(*pair.first);
			auto& second = static_cast<Actor&>(*pair.second);

			if (first.getBall() == nullptr && (!ActiveBall->isMovingToo() || ActiveBall->isPassing()))
			{
				if (ActiveBall->getTeam() == Actor::Team::None || first.isAttack())
				{
					first.giveBall(&second);
					second.setTeam(first.getTeam());
					freeBall = false;
				}
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
				Actor* assignment = AI->getAssignment();
				AI->setGameState(Actor::GameState::Defense);
				sf::Vector2f MoveTooPosition;

				if (AI->getTeam() == Actor::Team::Team_2)
					MoveTooPosition = sf::Vector2f((100 + assignment->getPosition().x), assignment->getPosition().y);
				else 
					MoveTooPosition = sf::Vector2f((assignment->getPosition().x - 100), assignment->getPosition().y);

				if (InArea(MoveTooPosition, AI))
					AI->stopMovingToo();
				else
					AI->moveTo(MoveTooPosition);
			}
			else if (AI->getTeam() == ActiveBall->getTeam())
			{
				AI->setGameState(Actor::GameState::Offense);

				sf::Vector2f moveTooPosition;
				if (AI->isShootingPosition())
				{
					moveTooPosition = sf::Vector2f(AI->getShootingPosition());
				}
				else
				{
					moveTooPosition = sf::Vector2f(AI->getNewShootingPosition());
					AI->stopMovingToo();
				}
				if (InArea(moveTooPosition, AI) && AI->getState() != Actor::State::Shoot)
				{
					std::uniform_int_distribution<unsigned int> random{ 1, 5 };

					int randomAction = random(REng);
					AI->stopMovingToo();

					if (randomAction > 2)
						AI->attack();
					else
						AI->passBall();
				}
				else if (AI->getState() == Actor::State::Shoot)
				{

				}
				else
					AI->moveTo(moveTooPosition);
			}
			if (ActiveBall->isShooting() || ActiveBall->isRebounding())
			{
				AI->stopMovingToo();
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

void World::updateText()
{
	if (mPlayerActor->getPosition().x > 1200 && mPlayerActor->getPosition().x < 3000)
	{
		blueTeamScore->setPosition((mPlayerActor->getPosition().x + 500), blueTeamScore->getPosition().y);
		redTeamScore->setPosition((mPlayerActor->getPosition().x - 500), redTeamScore->getPosition().y);
	}

	blueTeamScore->setString("Blue " + std::to_string(game.get_Team_2_Score()));
	redTeamScore->setString("Red " + std::to_string(game.get_Team_1_Score()));
}

void World::score(Actor* hoop)
{
	ActiveBall->setShooting(false);
	std::uniform_int_distribution<unsigned int> random{ 1, 2 };

	int randomSound = random(REng);
	if (randomSound == 1)
		mSounds.play(SoundEffect::Swish);
	else
		mSounds.play(SoundEffect::Rim);

	hoop->setState(Actor::State::score);

	if (hoop->getDirection() == Actor::Direction::Right)
	{
		ActiveBall->setPosition(AI_PG->getPosition());
		game.Team_1_Score(2);
	}
	else
	{
		mPlayerActor->giveBall(ActiveBall);
		game.Team_2_Score(2);
	}
	freeBall = false;
}

void World::rebound(Actor* hoop)
{
	mSounds.play(SoundEffect::Rim);
	hoop->rebound(ActiveBall);
	freeBall = true;
}

Actor* World::getRandomPlayer(Actor::Team team)
{
	std::uniform_int_distribution<unsigned int> random{ 1, 2 };
	int randomPlayer = random(REng);
	int i = 1;
	for (Actor* player : Players) 
	{
		
		if (player->getTeam() == team)
		{
			return player;
			if (1 == i)
				return player;
			else
				++i;
		}
	}
}

void World::buildScene()
{
	// Initialize the different layers
	for (std::size_t i = 0; i < LayerCount; ++i)
	{
		Category::Type category = (i == World::Layer::FrontCourt) ? Category::SceneAirLayer : Category::None;

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

	std::unique_ptr<Actor> team_1_Hoop(new Actor(Actor::Type::hoop, mTextures, mFonts));
	team_1_Hoop->setPosition(TEAM_1_HOOP_SPAWN);
	team_1_Hoop->setTeam(Actor::Team::Team_1);
	
	std::unique_ptr<Actor> team_2_Hoop(new Actor(Actor::Type::hoop, mTextures, mFonts));
	team_2_Hoop->setPosition(TEAM_2_HOOP_SPAWN);
	team_2_Hoop->setTeam(Actor::Team::Team_2);
	team_2_Hoop->setDirection(Actor::Direction::Left);

	std::unique_ptr<Actor> ball(new Actor(Actor::Type::ball, mTextures, mFonts));
	ball->setPosition(BALL_SPAWN);
	ball->scale(0.1f, 0.1f);
	ball->setTeam(Actor::Team::None);
	ActiveBall = ball.get();

	spawnPlayers();

	mSceneLayers[World::Layer::Hoop]->attachChild(std::move(team_1_Hoop));
	mSceneLayers[World::Layer::Hoop]->attachChild(std::move(team_2_Hoop));
	mSceneLayers[World::Layer::Court]->attachChild(std::move(ball));

	std::unique_ptr<TextNode> tmpBlueTeamScore(new TextNode(mFonts, ""));
	std::unique_ptr<TextNode> tmpRedTeamScore(new TextNode(mFonts, ""));

	blueTeamScore = tmpBlueTeamScore.get();
	blueTeamScore->setPosition(BLUE_TEXT_SPAWN);

	redTeamScore = tmpRedTeamScore.get();
	redTeamScore->setPosition(RED_TEXT_SPAWN);

	mSceneGraph.attachChild(std::move(tmpBlueTeamScore));
	mSceneGraph.attachChild(std::move(tmpRedTeamScore));
	// Add enemy aircraft
}

void World::spawnPlayers()
{
	std::unique_ptr<Actor> player(new Actor(Actor::Type::Player, mTextures, mFonts));
	std::unique_ptr<Actor> AI_player_2(new Actor(Actor::Type::team_2_Player, mTextures, mFonts));
	std::unique_ptr<Actor> AI_player_3(new Actor(Actor::Type::team_2_Player, mTextures, mFonts));
	std::unique_ptr<Actor> AI_player_1(new Actor(Actor::Type::team_1_Player, mTextures, mFonts));
	
	player->scale(0.7, 0.5);
	mPlayerActor = player.get();
	mPlayerActor->setPosition(mSpawnPosition);
	mPlayerActor->setTeam(Actor::Team::Team_1);
	mPlayerActor->setTeamate(AI_player_1.get());

	AI_player_2->scale(0.7, 0.5);
	AI_player_2->setPosition(2500.f, 35.f);
	AI_player_2->setTeam(Actor::Team::Team_2);
	AI_player_2->setAssignment(AI_player_1.get());
	AI_player_2->setTeamate(AI_player_3.get());
	
	AI_player_3->scale(0.7, 0.5);
	AI_player_3->setPosition(2800.f, 100.f);
	AI_player_3->setTeam(Actor::Team::Team_2);
	AI_player_3->setAssignment(player.get());
	AI_player_3->setTeamate(AI_player_2.get());
	AI_PG = AI_player_3.get();

	AI_player_1->scale(0.7, 0.5);
	AI_player_1->setPosition(1200, 100);
	AI_player_1->setTeam(Actor::Team::Team_1);
	AI_player_1->setAssignment(AI_player_2.get());
	AI_player_1->setTeamate(player.get());

	AI_Players.push_back(AI_player_2.get());
	AI_Players.push_back(AI_player_3.get());
	AI_Players.push_back(AI_player_1.get());

	Players.push_back(player.get());
	Players.push_back(AI_player_1.get());
	Players.push_back(AI_player_2.get());
	Players.push_back(AI_player_3.get());

	sf::Texture& Head1Texture = mTextures.get(Textures::Head1);
	Head1Texture.setRepeated(false);
	std::unique_ptr<SpriteNode> Head1(new SpriteNode(Head1Texture, HEAD_SIZE));

	sf::Texture& Head2Texture = mTextures.get(Textures::Head2);
	Head2Texture.setRepeated(false);
	std::unique_ptr<SpriteNode> Head2(new SpriteNode(Head2Texture, HEAD_SIZE));

	sf::Texture& Head3Texture = mTextures.get(Textures::Head3);
	Head3Texture.setRepeated(false);
	std::unique_ptr<SpriteNode> Head3(new SpriteNode(Head3Texture, HEAD_SIZE));

	sf::Texture& Head4Texture = mTextures.get(Textures::Head4);
	Head4Texture.setRepeated(false);
	std::unique_ptr<SpriteNode> Head4(new SpriteNode(Head4Texture, HEAD_SIZE));

	player->setHead(Head1.get());
	AI_player_1->setHead(Head2.get());
	AI_player_2->setHead(Head3.get());
	AI_player_3->setHead(Head4.get());
	

	mSceneLayers[World::Layer::FrontCourt]->attachChild(std::move(player));
	mSceneLayers[World::Layer::FrontCourt]->attachChild(std::move(AI_player_2));
	mSceneLayers[World::Layer::BackCourt]->attachChild(std::move(AI_player_3));
	mSceneLayers[World::Layer::BackCourt]->attachChild(std::move(AI_player_1));

	mSceneLayers[Head]->attachChild(std::move(Head1));
	mSceneLayers[Head]->attachChild(std::move(Head2));
	mSceneLayers[Head]->attachChild(std::move(Head3));
	mSceneLayers[Head]->attachChild(std::move(Head4));
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
