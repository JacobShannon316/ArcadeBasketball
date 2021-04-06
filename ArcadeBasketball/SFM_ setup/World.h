#pragma once
#include "ResourceHolder.h"
#include "ResourceIdentifiers.h"
#include "SceneNode.h"
#include "SpriteNode.h"
#include "Aircraft.h"
#include "Actor.h"
#include "CommandQueue.h"
#include "Command.h"
#include "BloomEffect.h"
#include "SoundPlayer.h"
#include "BasketBallGame.h"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <array>
#include <queue>
#include <random>


// Forward declaration
namespace sf
{
	class RenderTarget;
}

class World : private sf::NonCopyable
{
public:
	World(sf::RenderTarget& outputTarget, FontHolder& fonts, SoundPlayer& sounds);
	void								update(sf::Time dt);
	void								draw();

	CommandQueue& getCommandQueue();

	bool 								hasAlivePlayer() const;


private:
	void								loadTextures();
	void								adaptPlayerPosition();
	void								adaptNPCPosition();
	void								adaptActorVelocity(Actor* actor);
	void								adaptWorldView();
	void								handleCollisions();
	void								updateSounds();
	void								updateAI();
	void								updateBallState();

	void								buildScene();
	void								addEnemies();
	void								addEnemy(Actor::Type type, float relX, float relY);
	void								spawnEnemies();
	void								destroyEntitiesOutsideView();
	void								guideMissiles();
	sf::FloatRect						getViewBounds() const;
	sf::FloatRect						getBattlefieldBounds() const;

	bool								InArea(sf::Vector2f area, Actor* actor);


private:
	enum Layer
	{
		Background,
		Court,
		Player,
		Hoop,
		LayerCount
	};

	struct SpawnPoint
	{
		SpawnPoint(Actor::Type type, float x, float y)
			: type(type)
			, x(x)
			, y(y)
		{
		}

		Actor::Type type;
		float x;
		float y;
	};


private:
	sf::RenderTarget& mTarget;
	sf::RenderTexture					mSceneTexture;
	sf::View							mWorldView;
	TextureHolder						mTextures;
	FontHolder&							mFonts;
	SoundPlayer&						mSounds;
	BasketBallGame						game;
	

	SceneNode							mSceneGraph;
	std::array<SceneNode*, LayerCount>	mSceneLayers;
	CommandQueue						mCommandQueue;

	sf::FloatRect						mWorldBounds;
	sf::Vector2f						mSpawnPosition;
	float								mScrollSpeed;
	Actor*								mPlayerActor;
	Actor*								ActiveBall;

	std::vector<Actor*>					AI_Players;

	std::vector<SpawnPoint>				mEnemySpawnPoints;
	std::vector<Actor*>					mActiveEnemies;

	bool								freeBall = true;

	BloomEffect							mBloomEffect;
	const sf::Vector2f					TEAM_1_HOOP_SPAWN = sf::Vector2f(3450.f, 35.f);
	const sf::Vector2f					TEAM_2_HOOP_SPAWN = sf::Vector2f(600.f, 35.f);
	const sf::Vector2f					BALL_SPAWN = sf::Vector2f(2000.f, 175.f);
};
