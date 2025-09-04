#include "PmergeMe.hpp"

int main(int ac, char **av) {
    if (ac < 2) 
    {
        std::cerr << "Error: No arguments provided" << std::endl;
        std::cerr << "Usage: ./PmergeMe <positive_integer1> <positive_integer2> etc. ..." << std::endl;
        return 1;
    }
    try {
        PmergeMe pmerge;
        pmerge.runAlgo(ac, av);
        
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
