#include "BasketBallGame.h"

int BasketBallGame::get_Team_1_Score() const
{
	return team_1_Score;
}

int BasketBallGame::get_Team_2_Score() const
{
	return team_2_Score;
}

std::string BasketBallGame::getGameScore() const
{
	std::string GameScoreString;
	if (team_1_Score > team_2_Score)
	{
		GameScoreString = "Team 1 leads " +
			std::to_string(team_1_Score) +
			"-" +
			std::to_string(team_2_Score);
	}
	else if (team_2_Score > team_1_Score)
	{
		GameScoreString = "Team 2 leads " +
			std::to_string(team_2_Score) +
			"-" +
			std::to_string(team_1_Score);
	}
	else 
	{
		GameScoreString = "Tied " +
			std::to_string(team_2_Score) +
			"-" +
			std::to_string(team_1_Score);
	}
	return GameScoreString;
}

void BasketBallGame::set_Team_1_Score(int score)
{
	team_1_Score = score;
}

void BasketBallGame::set_Team_2_Score(int score)
{
	team_2_Score = score;
}

void BasketBallGame::Team_1_Score(int points)
{
	team_1_Score += points;
	std::cout << getGameScore() << std::endl;
}

void BasketBallGame::Team_2_Score(int points)
{
	team_2_Score += points;
	std::cout << getGameScore() << std::endl;
}

bool BasketBallGame::isWinner()
{
	if (team_1_Score >= 21 || team_2_Score >= 21) 
	{
		return true;
	}
}

std::string BasketBallGame::getWinner()
{
	if (isWinner()) 
	{
		if (team_1_Score > team_2_Score)
		{
			return "Team 1 Wins!";
		}
		else if (team_2_Score > team_1_Score)
		{
			return "Team 2 Wins!";
		}
		else
			return "Its a tie!";
	}
}
