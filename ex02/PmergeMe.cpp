#include "PmergeMe.hpp"

// constructor
PmergeMe::PmergeMe() {}

// destructor
PmergeMe::~PmergeMe() {}

// validate positive integer strings
bool PmergeMe::isValidNumber(const std::string& str) const {
    if (str.empty()) {
        return false;
    }
    // Check if all characters are digits
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] < '0' || str[i] > '9') {
            return false;
        }
    }
    // Check if number is positive (not zero)
    if (str == "0") {
        return false;
    }
    return true;
}

// parse command line arguments into both containers
void PmergeMe::parseArguments(int ac, char **av) {
    for (int i = 1; i < ac; ++i) {
        std::string arg = av[i];
        if (!isValidNumber(arg)) {
            throw std::runtime_error("Error: Invalid argument - must be a positive integer");
        }
        int num = std::atoi(arg.c_str());
        // add number to both containers
        vecNumbers.push_back(num);
        deqNumbers.push_back(num);
    }
}
