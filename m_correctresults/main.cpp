#include "PmergeMe.hpp"

int main(int argc, char *argv[])
{
    PmergeMe mergeInsertSort;
    try
    {
        mergeInsertSort.runMergeInsertSort(argc, argv);
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    return (0);
}