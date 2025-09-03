#ifndef PMERGEME_HPP
#define PMERGEME_HPP

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
        
        void createPairs(std::vector<std::pair<int, int> >& pairs, std::vector<int>& remaining) const;
        void sortPairs(std::vector<std::pair<int, int> >& pairs) const;

        // needed for recursive sorting
        std::vector<int> extractLargerElements(const std::vector<std::pair<int, int> >& pairs) const;

    public:
        PmergeMe();
        ~PmergeMe();
        
        void simpleSort(int ac, char **av);
        void testPairs(int ac, char **av);

};

#endif
