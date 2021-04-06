 #include "DataTables.h"
#include "Aircraft.h"
#include "Pickup.h"
#include "Particle.h"
#include "jsonFrameParser.h"


// For std::bind() placeholders _1, _2, ...
using namespace std::placeholders;

//std::vector<AircraftData> initializeAircraftData()
//{
//	std::vector<AircraftData> data(Aircraft::TypeCount);
//
//	data[Aircraft::Eagle].hitpoints = 100;
//	data[Aircraft::Eagle].speed = 200.f;
//	data[Aircraft::Eagle].fireInterval = sf::seconds(1);
//	data[Aircraft::Eagle].texture = Textures::Entities;
//	data[Aircraft::Eagle].textureRect = sf::IntRect(0, 0, 48, 64);
//	data[Aircraft::Eagle].hasRollAnimation = true;
//
//	data[Aircraft::Raptor].hitpoints = 20;
//	data[Aircraft::Raptor].speed = 80.f;
//	data[Aircraft::Raptor].texture = Textures::Entities;
//	data[Aircraft::Raptor].textureRect = sf::IntRect(144, 0, 84, 64);
//	data[Aircraft::Raptor].directions.push_back(Direction(+45.f, 80.f));
//	data[Aircraft::Raptor].directions.push_back(Direction(-45.f, 160.f));
//	data[Aircraft::Raptor].directions.push_back(Direction(+45.f, 80.f));
//	data[Aircraft::Raptor].fireInterval = sf::Time::Zero;
//	data[Aircraft::Raptor].hasRollAnimation = false;
//
//	data[Aircraft::Avenger].hitpoints = 40;
//	data[Aircraft::Avenger].speed = 50.f;
//	data[Aircraft::Avenger].texture = Textures::Entities;
//	data[Aircraft::Avenger].textureRect = sf::IntRect(228, 0, 60, 59);
//	data[Aircraft::Avenger].directions.push_back(Direction(+45.f, 50.f));
//	data[Aircraft::Avenger].directions.push_back(Direction(0.f, 50.f));
//	data[Aircraft::Avenger].directions.push_back(Direction(-45.f, 100.f));
//	data[Aircraft::Avenger].directions.push_back(Direction(0.f, 50.f));
//	data[Aircraft::Avenger].directions.push_back(Direction(+45.f, 50.f));
//	data[Aircraft::Avenger].fireInterval = sf::seconds(2);
//	data[Aircraft::Avenger].hasRollAnimation = false;
//
//	return data;
//}

//std::vector<projectiledata> initializeprojectiledata()
//{
//	std::vector<projectiledata> data(projectile::typecount);
//
//	data[projectile::alliedbullet].damage = 10;
//	data[projectile::alliedbullet].speed = 300.f;
//	data[projectile::alliedbullet].texture = textures::entities;
//	data[projectile::alliedbullet].texturerect = sf::intrect(175, 64, 3, 14);
//
//	data[projectile::enemybullet].damage = 10;
//	data[projectile::enemybullet].speed = 300.f;
//	data[projectile::enemybullet].texture = textures::entities;
//	data[projectile::enemybullet].texturerect = sf::intrect(178, 64, 3, 14);
//
//	data[projectile::missile].damage = 200;
//	data[projectile::missile].speed = 150.f;
//	data[projectile::missile].texture = textures::entities;
//	data[projectile::missile].texturerect = sf::intrect(160, 64, 15, 32);
//
//	return data;
//}

//std::vector<PickupData> initializePickupData()
//{
//	std::vector<PickupData> data(Pickup::TypeCount);
//
//	data[Pickup::HealthRefill].texture = Textures::Entities;
//	data[Pickup::HealthRefill].textureRect = sf::IntRect(0, 64, 40, 40);
//	data[Pickup::HealthRefill].action = [](Aircraft& a) { a.repair(25); };
//
//	data[Pickup::MissileRefill].texture = Textures::Entities;
//	data[Pickup::MissileRefill].textureRect = sf::IntRect(40, 64, 40, 40);
//	data[Pickup::MissileRefill].action = std::bind(&Aircraft::collectMissiles, _1, 3);
//
//	data[Pickup::FireSpread].texture = Textures::Entities;
//	data[Pickup::FireSpread].textureRect = sf::IntRect(80, 64, 40, 40);
//	data[Pickup::FireSpread].action = std::bind(&Aircraft::increaseSpread, _1);
//
//	data[Pickup::FireRate].texture = Textures::Entities;
//	data[Pickup::FireRate].textureRect = sf::IntRect(120, 64, 40, 40);
//	data[Pickup::FireRate].action = std::bind(&Aircraft::increaseFireRate, _1);
//
//	return data;
//}

//std::vector<ParticleData> initializeParticleData()
//{
//	std::vector<ParticleData> data(Particle::ParticleCount);
//
//	data[Particle::Propellant].color = sf::Color(255, 255, 50);
//	data[Particle::Propellant].lifetime = sf::seconds(0.6f);
//
//	data[Particle::Smoke].color = sf::Color(50, 50, 50);
//	data[Particle::Smoke].lifetime = sf::seconds(4.f);
//
//	return data;
//}

std::map<Actor::Type, ActorData> InitializeActorData()
{
	std::map<Actor::Type, ActorData> data;

	//Player character - lumberjack
	//assign texture
	data[Actor::Type::team_1_Player].texture = Textures::ID::Hero2;
	//assign speed
	data[Actor::Type::team_1_Player].speed = 700;

	//get the sprite sheet through the json frame parser
	JsonFrameParser frames = JsonFrameParser("Media/Textures/hero2.json");

	//assign the frames for the animation of the respective name, in this case, idle
	data[Actor::Type::team_1_Player].animations[Actor::State::Idle].addFrameSet(frames.getFramesFor("idle"));
	//set the duration per frams
	data[Actor::Type::team_1_Player].animations[Actor::State::Idle].setDuration(sf::seconds(1.f));
	//chose to repeat or not
	data[Actor::Type::team_1_Player].animations[Actor::State::Idle].setRepeating(true);

	data[Actor::Type::team_1_Player].animations[Actor::State::Jump].addFrameSet(frames.getFramesFor("jump"));
	data[Actor::Type::team_1_Player].animations[Actor::State::Jump].setDuration(sf::seconds(1.f));
	data[Actor::Type::team_1_Player].animations[Actor::State::Jump].setRepeating(true);

	data[Actor::Type::team_1_Player].animations[Actor::State::Attack].addFrameSet(frames.getFramesFor("attack"));
	data[Actor::Type::team_1_Player].animations[Actor::State::Attack].setDuration(sf::seconds(1.f));
	data[Actor::Type::team_1_Player].animations[Actor::State::Attack].setRepeating(false);
					  
	data[Actor::Type::team_1_Player].animations[Actor::State::Walk].addFrameSet(frames.getFramesFor("walk"));
	data[Actor::Type::team_1_Player].animations[Actor::State::Walk].setDuration(sf::seconds(1.f));
	data[Actor::Type::team_1_Player].animations[Actor::State::Walk].setRepeating(true);
					  
	data[Actor::Type::team_1_Player].animations[Actor::State::Run].addFrameSet(frames.getFramesFor("run"));
	data[Actor::Type::team_1_Player].animations[Actor::State::Run].setDuration(sf::seconds(1.f));
	data[Actor::Type::team_1_Player].animations[Actor::State::Run].setRepeating(true);
					  
	data[Actor::Type::team_1_Player].animations[Actor::State::Dead].addFrameSet(frames.getFramesFor("dead"));
	data[Actor::Type::team_1_Player].animations[Actor::State::Dead].setDuration(sf::seconds(1.f));
	data[Actor::Type::team_1_Player].animations[Actor::State::Dead].setRepeating(false);


	data[Actor::Type::team_2_Player].texture = Textures::ID::Hero2;
	//assign speed
	data[Actor::Type::team_2_Player].speed = 400;

	//assign the frames for the animation of the respective name, in this case, idle
	data[Actor::Type::team_2_Player].animations[Actor::State::Idle].addFrameSet(frames.getFramesFor("idle"));
	//set the duration per rams
	data[Actor::Type::team_2_Player].animations[Actor::State::Idle].setDuration(sf::seconds(1.f));
	//chose to repeat or no
	data[Actor::Type::team_2_Player].animations[Actor::State::Idle].setRepeating(true);
						   
	data[Actor::Type::team_2_Player].animations[Actor::State::Jump].addFrameSet(frames.getFramesFor("jump"));
	data[Actor::Type::team_2_Player].animations[Actor::State::Jump].setDuration(sf::seconds(1.f));
	data[Actor::Type::team_2_Player].animations[Actor::State::Jump].setRepeating(true);
						   
	data[Actor::Type::team_2_Player].animations[Actor::State::Attack].addFrameSet(frames.getFramesFor("attack"));
	data[Actor::Type::team_2_Player].animations[Actor::State::Attack].setDuration(sf::seconds(1.f));
	data[Actor::Type::team_2_Player].animations[Actor::State::Attack].setRepeating(false);
						   
	data[Actor::Type::team_2_Player].animations[Actor::State::Walk].addFrameSet(frames.getFramesFor("walk"));
	data[Actor::Type::team_2_Player].animations[Actor::State::Walk].setDuration(sf::seconds(1.f));
	data[Actor::Type::team_2_Player].animations[Actor::State::Walk].setRepeating(true);
						   
	data[Actor::Type::team_2_Player].animations[Actor::State::Run].addFrameSet(frames.getFramesFor("run"));
	data[Actor::Type::team_2_Player].animations[Actor::State::Run].setDuration(sf::seconds(1.f));
	data[Actor::Type::team_2_Player].animations[Actor::State::Run].setRepeating(true);
						   
	data[Actor::Type::team_2_Player].animations[Actor::State::Dead].addFrameSet(frames.getFramesFor("dead"));
	data[Actor::Type::team_2_Player].animations[Actor::State::Dead].setDuration(sf::seconds(1.f));
	data[Actor::Type::team_2_Player].animations[Actor::State::Dead].setRepeating(false);


	data[Actor::Type::hoop].texture = Textures::ID::Hoop;

	data[Actor::Type::ball].texture = Textures::ID::Ball;
	data[Actor::Type::ball].speed = 750.f; 
	data[Actor::Type::ball].animations[Actor::State::Walk].addFrame(sf::IntRect(-100, -100, 600, 600));
	data[Actor::Type::ball].animations[Actor::State::Walk].setRepeating(false);
	data[Actor::Type::ball].animations[Actor::State::Idle].addFrame(sf::IntRect(-100, -100, 600, 600));
	data[Actor::Type::ball].animations[Actor::State::Idle].setRepeating(false);
	
	//assign speed

	//get the sprite sheet through the json frame parser
	JsonFrameParser Scoringframes = JsonFrameParser("Media/Textures/ScoringAninmation.json");

	//assign the frames for the animation of the respective name, in this case, idle
	data[Actor::Type::hoop].animations[Actor::State::Idle].addFrameSet(Scoringframes.getFramesFor("Idle"));
	data[Actor::Type::hoop].animations[Actor::State::Idle].setDuration(sf::seconds(1.f));
	data[Actor::Type::hoop].animations[Actor::State::Idle].setRepeating(true);

	data[Actor::Type::hoop].animations[Actor::State::score].addFrameSet(Scoringframes.getFramesFor("Scoring"));
	data[Actor::Type::hoop].animations[Actor::State::score].setDuration(sf::seconds(0.5f));
	data[Actor::Type::hoop].animations[Actor::State::score].setRepeating(false);

	return data;

}							
