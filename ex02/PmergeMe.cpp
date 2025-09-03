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
    // ADD AGAIN IN THE END SUBJECT SAYS POSITIVE AND ZERO IS NOT???
    // Check if number is positive (not zero)
    /* if (str == "0") {
        return false;
    } */
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
    // Display the original sequence
    DEBUG("Starting sequence: ");
    for (size_t i = 0; i < vecNumbers.size(); ++i) {
        std::cout << vecNumbers[i];
        if (i < vecNumbers.size() - 1) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
}

// Step 1: Create pairs from consecutive elements
void PmergeMe::createPairs(std::vector<std::pair<int, int> >& pairs, std::vector<int>& remaining) const 
{
    pairs.clear();
    remaining.clear();
    
    DEBUG("Creating pairs from " << vecNumbers.size() << " elements...");

    // Create pairs from consecutive elements
    for (size_t i = 0; i < vecNumbers.size() - 1; i += 2) {
        int first = vecNumbers[i];
        int second = vecNumbers[i + 1];
        
        DEBUG("Creating pair from elements " << i << " and " << (i+1) << ": " << first << " and " << second);
        
        // Create a pair with the smaller value first & larger second
        if (first <= second) {
            pairs.push_back(std::make_pair(first, second));
            DEBUG("Created pair: (" << first << "," << second << ") [no swap needed]");
        } else {
            pairs.push_back(std::make_pair(second, first));
            DEBUG("Created pair: (" << second << "," << first << ") [swapped]");
        }
    }
    
    // Handle odd number of elements - the last one goes to remaining
    if (vecNumbers.size() % 2 != 0) {
        remaining.push_back(vecNumbers[vecNumbers.size() - 1]);
        DEBUG("Added remaining element: " << vecNumbers[vecNumbers.size() - 1]);
    }
    DEBUG("Created " << pairs.size() << " pairs and " << remaining.size() << " remaining elements");

    // Display created pairs
    DEBUG("Pairs created: ");
    for (size_t i = 0; i < pairs.size(); ++i) {
        std::cout << "(" << pairs[i].first << "," << pairs[i].second << ")";
        if (i < pairs.size() - 1) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
    // Print remaining elements 
    if (!remaining.empty()) {
        DEBUG("Remaining element: ");
        for (size_t i = 0; i < remaining.size(); ++i) {
            std::cout << remaining[i];
            if (i < remaining.size() - 1) {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    }
}

// Step 1: Recursively sort pairs by their larger elements using Ford-Johnson grouping
void PmergeMe::sortPairsRecursively(std::vector<std::pair<int, int> >& pairs, int recursionDepth) const {
    DEBUG("Recursion depth " << recursionDepth << ": Processing " << pairs.size() << " pairs");
    
    // Calculate how many pairs to group together at this recursion level
    // Level 1: group 2 pairs, Level 2: group 4 pairs, Level 3: group 8 pairs, etc.
    size_t groupSize = 1 << recursionDepth; // 2^recursionDepth
    
    DEBUG("Recursion depth " << recursionDepth << ": Grouping " << groupSize << " pairs together");
    
    // stop the recursion if there are not enough pairs to group
    if (pairs.size() < groupSize) {
        DEBUG("Recursion depth " << recursionDepth << ": Base case - not enough pairs to group");
        return;
    }
    
    // Process pairs in groups of groupSize
    for (size_t i = 0; i < pairs.size() - groupSize + 1; i += groupSize) {
        DEBUG("Recursion depth " << recursionDepth << ": Processing group starting at index " << i);
        
        // Each group contains two subgroups of groupSize/2 pairs
        size_t subgroupSize = groupSize / 2;
        
        // Compare the larger elements (second element) of the last pair in each subgroup
        int firstSubgroupLastLarger = pairs[i + subgroupSize - 1].second;
        int secondSubgroupLastLarger = pairs[i + groupSize - 1].second;
        
        DEBUG("Recursion depth " << recursionDepth << ": Comparing " << firstSubgroupLastLarger << " vs " << secondSubgroupLastLarger);
        
        // If the second subgroup's last larger element is smaller, swap the entire groups
        if (secondSubgroupLastLarger < firstSubgroupLastLarger) {
            DEBUG("Recursion depth " << recursionDepth << ": Swapping subgroups - second subgroup has smaller last element");
            
            // Swap the two subgroups
            for (size_t j = 0; j < subgroupSize; ++j) {
                std::swap(pairs[i + j], pairs[i + subgroupSize + j]);
            }
        } else {
            DEBUG("Recursion depth " << recursionDepth << ": No swap needed - first subgroup has smaller last element");
        }
    }
    
    // Display current sequence after each recursion level
    DEBUG("Recursion depth " << recursionDepth << ": After grouping, pairs are: ");
    for (size_t i = 0; i < pairs.size(); ++i) {
        std::cout << "(" << pairs[i].first << "," << pairs[i].second << ")";
        if (i < pairs.size() - 1) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
    
    // Recursively process the next level
    sortPairsRecursively(pairs, recursionDepth + 1);
}

// Utility function to print sequences
void PmergeMe::printSequence(const std::string& label, const std::vector<int>& sequence) const {
    DEBUG(label);
    for (size_t i = 0; i < sequence.size(); ++i) {
        std::cout << sequence[i];
        if (i < sequence.size() - 1) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
}

void PmergeMe::printSequence(const std::string& label, const std::deque<int>& sequence) const {
    DEBUG(label);
    for (size_t i = 0; i < sequence.size(); ++i) {
        std::cout << sequence[i];
        if (i < sequence.size() - 1) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
}

// Utility function to reconstruct containers from pairs
void PmergeMe::reconstructContainers(const std::vector<std::pair<int, int> >& pairs, const std::vector<int>& remaining) {
    // Clear and rebuild vector container
    vecNumbers.clear();
    for (size_t i = 0; i < pairs.size(); ++i) {
        vecNumbers.push_back(pairs[i].first);
        vecNumbers.push_back(pairs[i].second);
    }
    if (!remaining.empty()) {
        vecNumbers.push_back(remaining[0]);
    }
    
    // Clear and rebuild deque container
    deqNumbers.clear();
    for (size_t i = 0; i < pairs.size(); ++i) {
        deqNumbers.push_back(pairs[i].first);
        deqNumbers.push_back(pairs[i].second);
    }
    if (!remaining.empty()) {
        deqNumbers.push_back(remaining[0]);
    }
    
    DEBUG("Containers updated with sorted sequence");
}

// main function that organizes the whole algo
void PmergeMe::runAlgo(int ac, char **av) 
{
    parseArguments(ac, av);
    // Create the pairs
    std::vector<std::pair<int, int> > pairs;
    std::vector<int> remaining;
    createPairs(pairs, remaining);
    
    DEBUG("\n=== RECURSIVE SORTING OF PAIRS ===");
    sortPairsRecursively(pairs);
    
    // Display sorted pairs
    std::cout << std::endl;
    // Add remaining element back to the sequence (there's always at most 1)
    if (!remaining.empty()) {
        DEBUG("Adding remaining element back to sequence: " << remaining[0]);
        
        DEBUG("->>> Sorted sequence after recursion: ");
        for (size_t i = 0; i < pairs.size(); ++i) {
            std::cout << "(" << pairs[i].first << "," << pairs[i].second << ")";
            if (i < pairs.size() - 1) {
                std::cout << " ";
            }
        }
        std::cout << " " << remaining[0] << std::endl;
    }
    
    // Reconstruct the original containers with the updated sequence
    reconstructContainers(pairs, remaining);
    
    // Print the content of both containers
    printSequence("Vector container content: ", vecNumbers);
    printSequence("Deque container content: ", deqNumbers);
}
