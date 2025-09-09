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

// create pairs from consecutive elements
void PmergeMe::createPairs(std::vector<std::pair<int, int> >& pairs, std::vector<int>& remaining) {
    // in case of odd number of elements, the last value goes to remaining
    size_t size = vecNumbers.size();
    size_t pairCount = size / 2;
    
    for (size_t i = 0; i < pairCount; ++i) {
        int first = vecNumbers[i * 2];
        int second = vecNumbers[i * 2 + 1];
        
        // Store as (smaller, larger) pair
        if (first <= second) {
            pairs.push_back(std::make_pair(first, second));
        } else {
            pairs.push_back(std::make_pair(second, first));
        }
    }
    
    // Handle remaining element if odd number
    if (size % 2 == 1) {
        remaining.push_back(vecNumbers[size - 1]);
    }
}

// Ford-Johnson algorithm implementation for std::vector
void PmergeMe::fordJohnsonSortVector() {
    // check if already sorted
    if (vecNumbers.size() <= 1) {
        return;
    }
    // Step 1: Create pairs and find remaining element
    std::vector<std::pair<int, int> > pairs;
    std::vector<int> remaining;
    createPairs(pairs, remaining);
    
    // Step 2: Sort pairs by their larger elements
    sortPairs(pairs);
    
    // Step 3: Build the main chain and insert remaining elements
    std::vector<int> mainChain;
    
    // Start with the first pair
    if (!pairs.empty()) {
        mainChain.push_back(pairs[0].first);
        mainChain.push_back(pairs[0].second);
    }
    
    // Insert remaining pairs using binary insertion
    for (size_t i = 1; i < pairs.size(); ++i) {
        // Insert the larger element (second) using binary search
        int larger = pairs[i].second;
        size_t insertPos = 0;
        size_t left = 0;
        size_t right = mainChain.size();
        
        while (left < right) {
            size_t mid = (left + right) / 2;
            if (mainChain[mid] < larger) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
        insertPos = left;
        
        // Insert the larger element
        mainChain.insert(mainChain.begin() + insertPos, larger);
        
        // Insert the smaller element (first) using binary search
        int smaller = pairs[i].first;
        left = 0;
        right = insertPos; // Only search up to where we inserted the larger element
        
        while (left < right) {
            size_t mid = (left + right) / 2;
            if (mainChain[mid] < smaller) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
        
        // Insert the smaller element
        mainChain.insert(mainChain.begin() + left, smaller);
    }
    
    // Step 4: Insert any remaining single element
    if (!remaining.empty()) {
        int value = remaining[0];
        size_t insertPos = 0;
        size_t left = 0;
        size_t right = mainChain.size();
        
        while (left < right) {
            size_t mid = (left + right) / 2;
            if (mainChain[mid] < value) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
        insertPos = left;
        mainChain.insert(mainChain.begin() + insertPos, value);
    }
    
    // Step 5: Copy back to the original vector
    vecNumbers = mainChain;
}

// Helper function for sorting pairs (C++98 compatible)
bool comparePairs(const std::pair<int, int>& a, const std::pair<int, int>& b) {
    return a.second < b.second;
}

void PmergeMe::sortPairs(std::vector<std::pair<int, int> >& pairs) {
    // Sort pairs by their larger elements (second element of each pair)
    std::sort(pairs.begin(), pairs.end(), comparePairs);
}



// Ford-Johnson algorithm implementation for std::deque
void PmergeMe::fordJohnsonSortDeque() {
    if (deqNumbers.size() <= 1) {
        return; // Already sorted
    }
    
    // Step 1: Create pairs and find remaining element
    std::deque<std::pair<int, int> > pairs;
    std::deque<int> remaining;
    createPairs(pairs, remaining);
    
    // Step 2: Sort pairs by their larger elements
    sortPairs(pairs);
    
    // Step 3: Build the main chain and insert remaining elements
    std::deque<int> mainChain;
    
    // Start with the first pair
    if (!pairs.empty()) {
        mainChain.push_back(pairs[0].first);
        mainChain.push_back(pairs[0].second);
    }
    
    // Insert remaining pairs using binary insertion
    for (size_t i = 1; i < pairs.size(); ++i) {
        // Insert the larger element (second) using binary search
        int larger = pairs[i].second;
        size_t insertPos = 0;
        size_t left = 0;
        size_t right = mainChain.size();
        
        while (left < right) {
            size_t mid = (left + right) / 2;
            if (mainChain[mid] < larger) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
        insertPos = left;
        
        // Insert the larger element
        mainChain.insert(mainChain.begin() + insertPos, larger);
        
        // Insert the smaller element (first) using binary search
        int smaller = pairs[i].first;
        left = 0;
        right = insertPos; // Only search up to where we inserted the larger element
        
        while (left < right) {
            size_t mid = (left + right) / 2;
            if (mainChain[mid] < smaller) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
        
        // Insert the smaller element
        mainChain.insert(mainChain.begin() + left, smaller);
    }
    
    // Step 4: Insert any remaining single element
    if (!remaining.empty()) {
        int value = remaining[0];
        size_t insertPos = 0;
        size_t left = 0;
        size_t right = mainChain.size();
        
        while (left < right) {
            size_t mid = (left + right) / 2;
            if (mainChain[mid] < value) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
        insertPos = left;
        mainChain.insert(mainChain.begin() + insertPos, value);
    }
    
    // Step 5: Copy back to the original deque
    deqNumbers = mainChain;
}

// create pairs from consecutive elements
void PmergeMe::createPairs(std::deque<std::pair<int, int> >& pairs, std::deque<int>& remaining) {
    // If odd number of elements, the last one goes to remaining
    size_t size = deqNumbers.size();
    size_t pairCount = size / 2;
    
    for (size_t i = 0; i < pairCount; ++i) {
        int first = deqNumbers[i * 2];
        int second = deqNumbers[i * 2 + 1];
        
        // Store as (smaller, larger) pair
        if (first <= second) {
            pairs.push_back(std::make_pair(first, second));
        } else {
            pairs.push_back(std::make_pair(second, first));
        }
    }
    
    // Handle remaining element if odd number
    if (size % 2 == 1) {
        remaining.push_back(deqNumbers[size - 1]);
    }
}

// sort pairs by their larger elements
void PmergeMe::sortPairs(std::deque<std::pair<int, int> >& pairs) {
    // Sort pairs by their larger elements (second element of each pair)
    std::sort(pairs.begin(), pairs.end(), comparePairs);
}

// main method to process and sort the sequence
void PmergeMe::processSequence(int ac, char **av) {
    // Parse arguments and populate containers
    parseArguments(ac, av);
    
    // Display unsorted sequence
    std::cout << "Before: ";
    for (size_t i = 0; i < vecNumbers.size(); ++i) {
        std::cout << vecNumbers[i];
        if (i < vecNumbers.size() - 1) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
    
    // Sort using vector and measure time
    clock_t start = clock();
    fordJohnsonSortVector();
    clock_t end = clock();
    double vectorTime = ((double)(end - start)) / CLOCKS_PER_SEC * 1000000; // Convert to microseconds
    
    // Display sorted sequence
    std::cout << "After: ";
    for (size_t i = 0; i < vecNumbers.size(); ++i) {
        std::cout << vecNumbers[i];
        if (i < vecNumbers.size() - 1) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
    
    // Display vector timing
    std::cout << "Time to process a range of " << vecNumbers.size() 
              << " elements with std::vector: " << vectorTime << " us" << std::endl;
    
    // Sort using deque and measure time
    start = clock();
    fordJohnsonSortDeque();
    end = clock();
    double dequeTime = ((double)(end - start)) / CLOCKS_PER_SEC * 1000000; // Convert to microseconds
    
    // Display deque timing
    std::cout << "Time to process a range of " << deqNumbers.size() 
              << " elements with std::deque: " << dequeTime << " us" << std::endl;
}
