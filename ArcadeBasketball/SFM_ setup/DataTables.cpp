 #include "DataTables.h"
#include "Aircraft.h"
#include "Pickup.h"
#include "Particle.h"
#include "jsonFrameParser.h"


// For std::bind() placeholders _1, _2, ...
using namespace std::placeholders;

std::map<Actor::Type, ActorData> InitializeActorData()
{
	std::map<Actor::Type, ActorData> data;

	data[Actor::Type::Player].texture = Textures::ID::RedTeam;
	//assign speed
	data[Actor::Type::Player].speed = 700;

	//get the sprite sheet through the json frame parser
	JsonFrameParser frames = JsonFrameParser("Media/Textures/RedPlayer.json");

	//assign the frames for the animation of the respective name, in this case, idle
	data[Actor::Type::Player].animations[Actor::State::Idle].addFrameSet(frames.getFramesFor("idle"));
	data[Actor::Type::Player].animations[Actor::State::Idle].setDuration(sf::seconds(1.f));
	data[Actor::Type::Player].animations[Actor::State::Idle].setRepeating(true);
					  
	data[Actor::Type::Player].animations[Actor::State::Shoot].addFrameSet(frames.getFramesFor("shooting"));
	data[Actor::Type::Player].animations[Actor::State::Shoot].setDuration(sf::seconds(0.5f));
	data[Actor::Type::Player].animations[Actor::State::Shoot].setRepeating(false);
					  
	data[Actor::Type::Player].animations[Actor::State::Walk].addFrameSet(frames.getFramesFor("Walk"));
	data[Actor::Type::Player].animations[Actor::State::Walk].setDuration(sf::seconds(1.f));
	data[Actor::Type::Player].animations[Actor::State::Walk].setRepeating(true);
					  
	data[Actor::Type::Player].animations[Actor::State::WalkWithBall].addFrameSet(frames.getFramesFor("walkWithBall"));
	data[Actor::Type::Player].animations[Actor::State::WalkWithBall].setDuration(sf::seconds(1.f));
	data[Actor::Type::Player].animations[Actor::State::WalkWithBall].setRepeating(true);
					  
	data[Actor::Type::Player].animations[Actor::State::IdleWithBall].addFrameSet(frames.getFramesFor("IdleWithBall"));
	data[Actor::Type::Player].animations[Actor::State::IdleWithBall].setDuration(sf::seconds(1.f));
	data[Actor::Type::Player].animations[Actor::State::IdleWithBall].setRepeating(true);

	//assign texture
	data[Actor::Type::team_1_Player].texture = Textures::ID::RedTeam;
	//assign speed
	data[Actor::Type::team_1_Player].speed = 400;

	//get the sprite sheet through the json frame parser

	//assign the frames for the animation of the respective name, in this case, idle
	data[Actor::Type::team_1_Player].animations[Actor::State::Idle].addFrameSet(frames.getFramesFor("idle"));
	data[Actor::Type::team_1_Player].animations[Actor::State::Idle].setDuration(sf::seconds(1.f));
	data[Actor::Type::team_1_Player].animations[Actor::State::Idle].setRepeating(true);

	data[Actor::Type::team_1_Player].animations[Actor::State::Shoot].addFrameSet(frames.getFramesFor("shooting"));
	data[Actor::Type::team_1_Player].animations[Actor::State::Shoot].setDuration(sf::seconds(0.5f));
	data[Actor::Type::team_1_Player].animations[Actor::State::Shoot].setRepeating(false);
					  
	data[Actor::Type::team_1_Player].animations[Actor::State::Walk].addFrameSet(frames.getFramesFor("Walk"));
	data[Actor::Type::team_1_Player].animations[Actor::State::Walk].setDuration(sf::seconds(1.f));
	data[Actor::Type::team_1_Player].animations[Actor::State::Walk].setRepeating(true);

	data[Actor::Type::team_1_Player].animations[Actor::State::WalkWithBall].addFrameSet(frames.getFramesFor("walkWithBall"));
	data[Actor::Type::team_1_Player].animations[Actor::State::WalkWithBall].setDuration(sf::seconds(1.f));
	data[Actor::Type::team_1_Player].animations[Actor::State::WalkWithBall].setRepeating(true);

	data[Actor::Type::team_1_Player].animations[Actor::State::IdleWithBall].addFrameSet(frames.getFramesFor("IdleWithBall"));
	data[Actor::Type::team_1_Player].animations[Actor::State::IdleWithBall].setDuration(sf::seconds(1.f));
	data[Actor::Type::team_1_Player].animations[Actor::State::IdleWithBall].setRepeating(true);

	frames = JsonFrameParser("Media/Textures/BluePlayer.json");
	data[Actor::Type::team_2_Player].texture = Textures::ID::BlueTeam;
	//assign speed
	data[Actor::Type::team_2_Player].speed = 400;

	//assign the frames for the animation of the respective name, in this case, idle
	data[Actor::Type::team_2_Player].animations[Actor::State::Idle].addFrameSet(frames.getFramesFor("idle"));
	//set the duration per rams
	data[Actor::Type::team_2_Player].animations[Actor::State::Idle].setDuration(sf::seconds(1.f));
	//chose to repeat or no
	data[Actor::Type::team_2_Player].animations[Actor::State::Idle].setRepeating(true);
						   
	data[Actor::Type::team_2_Player].animations[Actor::State::Shoot].addFrameSet(frames.getFramesFor("shooting"));
	data[Actor::Type::team_2_Player].animations[Actor::State::Shoot].setDuration(sf::seconds(0.5f));
	data[Actor::Type::team_2_Player].animations[Actor::State::Shoot].setRepeating(false);
						   
	data[Actor::Type::team_2_Player].animations[Actor::State::Walk].addFrameSet(frames.getFramesFor("Walk"));
	data[Actor::Type::team_2_Player].animations[Actor::State::Walk].setDuration(sf::seconds(1.f));
	data[Actor::Type::team_2_Player].animations[Actor::State::Walk].setRepeating(true);

	data[Actor::Type::team_2_Player].animations[Actor::State::WalkWithBall].addFrameSet(frames.getFramesFor("walkWithBall"));
	data[Actor::Type::team_2_Player].animations[Actor::State::WalkWithBall].setDuration(sf::seconds(1.f));
	data[Actor::Type::team_2_Player].animations[Actor::State::WalkWithBall].setRepeating(true);

	data[Actor::Type::team_2_Player].animations[Actor::State::IdleWithBall].addFrameSet(frames.getFramesFor("IdleWithBall"));
	data[Actor::Type::team_2_Player].animations[Actor::State::IdleWithBall].setDuration(sf::seconds(1.f));
	data[Actor::Type::team_2_Player].animations[Actor::State::IdleWithBall].setRepeating(true);
						  


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
