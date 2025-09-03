#ifndef PMERGEME_HPP
#define PMERGEME_HPP

// Debug macro - comment out to disable all debug output
#define DEBUG(x) std::cout << x << std::endl
// #define DEBUG(x)  // Uncomment this line and comment the above to disable debug

#include <vector>
#include <deque>
#include <string>
#include <iostream>
#include <sstream>
#include <ctime>
#include <algorithm>
#include <stdexcept>

class PmergeMe 
{
    private:
        std::vector<int> vecNumbers;  // first container
        std::deque<int> deqNumbers;   // second container
        
        // private helper methods
        bool isValidNumber(const std::string& str) const;
        void parseArguments(int ac, char **av);

    public:
        PmergeMe();
        ~PmergeMe();
        
        // Step 1: Create pairs from consecutive elements
        void createPairs(std::vector<std::pair<int, int> >& pairs, std::vector<int>& remaining) const;
        
        // Step 1: Recursively sort pairs by their larger elements
        void sortPairsRecursively(std::vector<std::pair<int, int> >& pairs, int recursionDepth = 1) const;
        
        // Step 1: Main function to test pair creation
        void runAlgo(int ac, char **av);

};

#endif
