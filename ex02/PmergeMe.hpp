#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <vector>
#include <deque>
#include <string>
#include <iostream>
#include <sstream>
#include <ctime>
#include <algorithm> // for std::sort and other algorithms
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

};

#endif
