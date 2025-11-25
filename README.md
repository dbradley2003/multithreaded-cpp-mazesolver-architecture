# C++ Multithreaded Maze Solver

## Table of Contents

* [Objective](#objective)

* [Technical Details](#technical-details)

## Objective:

The challenge was to solve very large mazes(10kx10k, 15kx15k, 20kx20k) using multiple threads, focusing on speed and performance.

## Technical Details:

My implementation involved spawning 2 additional threads to do a bidirectional depth first search, Thread A runs dfs from the top, whereas Thread B runs dfs from the bottom. Using dfs, both threads will search the maze deeply until a collision is found.

**Resolving a Path**: Each thread marks a cell in the grid by using the lower 2 bits of the cell's address space. I use atomic operations on the grid to avoid race conditions between both threads, enabling safe and concurrent modifications to the grid's data.

```C++
if (id == SolverID::A && (pMaze->poMazeData[intersectIndex].fetch_or(A_FLAG) & B_FLAG) == B_FLAG)
{
	share_sc.intersection.compare_exchange_strong(expected, intersectIndex);
	throw SolutionFoundSkip(at, reverseDir(go_to));
}

if (id == SolverID::B && (pMaze->poMazeData[intersectIndex].fetch_or(B_FLAG) & A_FLAG) == A_FLAG)
{
	share_sc.intersection.compare_exchange_strong(expected, intersectIndex);
	throw SolutionFoundSkip(at, reverseDir(go_to));
}
```
**Creating the Solution:**
The final solution is a result of merging both Thread A and Thread B's own paths into 1 final consolidated path. I delegate this process to Thread A, which after creating it's own path will either start merging the paths if Thread B is already finished, or wait until Thread B's path is ready.


```C++
	this->BuildPathA(pChoiceStack, finalPath);
	while (true)
	{
		std::unique_lock<std::mutex> lck(share_sc.mtx);
		if (share_sc.cv.wait_for(lck, 0ms, [&]() {
			return share_sc.status == CollectorSolverShare::Status::B_DONE;
			}))
		{
			lck.unlock();
			break;
		}
	}
	CreateFullDirectionPath(finalPath);

	if (mProm.has_value())
	{
		mProm->set_value(finalPath);
	}
```

## Results

This implementation outperformed the single-threaded version by over 2x one every maze tested.









