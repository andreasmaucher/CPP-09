#include "PmergeMe.hpp"

//! add more debug printouts!

//! memory allocation questions (check eval sheet)

//! make sure vector is same as deque

int main(int ac, char **av)
{
    if (ac < 2) 
    {
        std::cerr << "Error: No arguments provided" << std::endl;
        std::cerr << "Usage: ./PmergeMe <positive_integer1> <positive_integer2> ..." << std::endl;
        std::cerr << "Example: ./PmergeMe 11 2 17 0 16 8 6 15 10 3 21 1 18 9 14 19 12 5 4 20 13" << std::endl;
        return 1;
    }
    PmergeMe mergeInsertSort;
    try
    {
        mergeInsertSort.runMergeInsertSort(ac, av);
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
