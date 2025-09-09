#include "PmergeMe.hpp"
#include "utils.hpp"
#include "define.hpp"

#include <iostream>
#include <vector>

/**
Handles sorting of integer arguments using VECTOR container only.
 - Checks passed arguments (integers [1, INT_MAX])
 - Sort integers implementing the merge-insertion sort algorithm (Ford-Johnson),
   using `std::vector<int>`
 - Print the sorted results and performance metrics (process time, number of comparisons made)

This implementation minimizes computation time by using in-place operations within the container.
*/
int	main(int ac, char** av)
{
	if (ac < 2) 
    {
        std::cerr << "Error: No arguments provided" << std::endl;
        std::cerr << "Usage: ./PmergeMe <positive_integer1> <positive_integer2> etc. ..." << std::endl;
        return 1;
    }

	timeval				start, end;	// structs to hold time info
	int					numCompVec = 0;	// keeps track of comparisons made
	std::vector<int>	sortedVec;

	try
	{
		PmergeMe::checkArgs(ac, av);

		// === Sorting using 'vector' ===
		start = getCurrentTimeStruct();
		sortedVec = PmergeMe::sortVec(ac, av, numCompVec);
		end = getCurrentTimeStruct();

		printBeforeAfter(av, sortedVec);
		printElapsedTime(start, end, ac - 1, "std::vector<int>");

		// === Printing additional info ===
		std::cout << std::endl;

		// Is result really sorted?
		printIsSorted(sortedVec, "std::vector<int>");

		// How many comparisons were made?
		int	maxComps = maxComparisonsFJ(ac - 1);
		printNumComp(numCompVec, maxComps, "std::vector<int>");
	}
	catch (const std::exception& e) 
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
