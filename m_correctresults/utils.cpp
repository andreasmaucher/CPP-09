#include "PmergeMe.hpp"

// parse through the input and check that it only includes valid integers
void PmergeMe::checkArgs(int ac, char **av) 
{
    for (int i = 1; i < ac; i++) 
    {
        std::istringstream iss(av[i]);
        int pmerge_int;
        
        // iss.peek() == EOF ensures no extra characters after the number (e.g. 12a would pass otherwise)
        if (iss >> pmerge_int && iss.peek() == EOF && pmerge_int >= 0)
        {
            pmerge_deque.push_back(pmerge_int);
        }
        else
            throw std::runtime_error("Please provide valid numeric positive arguments.");
    }
}

// print the sequence for vector
void PmergeMe::printSequence(const std::string& label, const std::vector<unsigned int>& seq) {
    std::cout << label;
    for (size_t i = 0; i < seq.size(); ++i) {
        std::cout << seq[i];
        if (i < seq.size() - 1) std::cout << " ";
    }
    std::cout << std::endl;
}

// print the sequence for deque
void PmergeMe::printSequence(const std::string& label, const std::deque<unsigned int>& seq) {
    std::cout << label;
    for (size_t i = 0; i < seq.size(); ++i) {
        std::cout << seq[i];
        if (i < seq.size() - 1) std::cout << " ";
    }
    std::cout << std::endl;
}