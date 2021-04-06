#pragma once
#include <string>
#include <iostream>
class BasketBallGame
{
public:
	int get_Team_1_Score() const;
	int get_Team_2_Score() const;
	std::string getGameScore() const;

	void set_Team_1_Score(int score);
	void set_Team_2_Score(int score);
	
	void Team_1_Score(int points);
	void Team_2_Score(int points);

	bool isWinner();
	std::string getWinner();
private:
	int team_1_Score = 0;
	int team_2_Score = 0;
	int const WinScore = 21;
};

