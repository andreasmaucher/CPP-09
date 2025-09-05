#include "PmergeMe.hpp"

int main(int ac, char **av)
{
    if (ac < 2) 
    {
        std::cerr << "Error: No arguments provided" << std::endl;
        std::cerr << "Usage: ./PmergeMe <positive_integer1> <positive_integer2> ..." << std::endl;
        std::cerr << "Example: ./PmergeMe 3 5 9 7 4" << std::endl;
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
