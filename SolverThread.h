#pragma once

#include "Maze.h"
#include "CollectorSolverShare.h"
#include <optional>

class SolverThread {

public:
	enum class SolverID {
		A,
		B
	};

	SolverThread(Maze* _pMaze, SolverID _id, CollectorSolverShare& _share_sc,
		std::promise<std::vector<Direction>*> &&_mProm,std::vector<Position>&positionPath);
	SolverThread(Maze* _pMaze, SolverID _id, CollectorSolverShare& _share_sc, std::vector<Position>& _mPath);

	~SolverThread();
	SolverThread(SolverThread const&) = delete;
	void operator=(SolverThread const&) = delete;
	void operator()();

	void Launch();
	void SearchFromStart();
	void SearchFromEnd();

	void BuildPathA(std::vector<Choice>& pChoiceStack,std::vector<Direction>* finalPath);
	void BuildPathB(std::vector<Choice>& pChoiceStack);


	Choice follow(Position at, Direction dir);  //throws SolutionFoundSkip()
	Choice firstChoice(Position pos); //throw SolutionFound
	void CreateFullDirectionPath(std::vector<Direction>*pFinalPath);

private:
	std::thread mThread;
	std::optional<std::promise<std::vector<Direction>*>> mProm;
	Maze* pMaze;
	CollectorSolverShare& share_sc;
	SolverID id;
	char pad[4];
	std::vector<Position> &mPath;
};