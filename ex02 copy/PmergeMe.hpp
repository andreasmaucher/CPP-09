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
        void fordJohnsonSortVector();
        void fordJohnsonSortDeque();
        
        // Ford-Johnson algorithm helper methods
        void createPairs(std::vector<std::pair<int, int> >& pairs, std::vector<int>& remaining);
        void sortPairs(std::vector<std::pair<int, int> >& pairs);
        
        void createPairs(std::deque<std::pair<int, int> >& pairs, std::deque<int>& remaining);
        void sortPairs(std::deque<std::pair<int, int> >& pairs);

    public:
        PmergeMe();
        ~PmergeMe();

        void processSequence(int ac, char **av);
};

#endif
