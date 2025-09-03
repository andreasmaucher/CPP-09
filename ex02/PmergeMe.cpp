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

// Step 1: first check that the input is valid and then sort the int sequence 
void PmergeMe::simpleSort(int ac, char **av) {
    // parse and validate arguments
    parseArguments(ac, av);
    
    // display the original sequence
    std::cout << "Before: ";
    for (size_t i = 0; i < vecNumbers.size(); ++i) {
        std::cout << vecNumbers[i];
        if (i < vecNumbers.size() - 1) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
    
    std::sort(vecNumbers.begin(), vecNumbers.end());
    
    std::cout << "After: ";
    for (size_t i = 0; i < vecNumbers.size(); ++i) {
        std::cout << vecNumbers[i];
        if (i < vecNumbers.size() - 1) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
    
    vecNumbers.clear();
    deqNumbers.clear();
}

// Step 2: Create pairs from consecutive elements
void PmergeMe::createPairs(std::vector<std::pair<int, int> >& pairs, std::vector<int>& remaining) const {
    pairs.clear();
    remaining.clear();
    
    // store two numbers in temp variables
    for (size_t i = 0; i < vecNumbers.size() - 1; i += 2) {
        int first = vecNumbers[i];
        int second = vecNumbers[i + 1];
        
        // create a pair with the smaller value first & larger second
        if (first <= second) {
            pairs.push_back(std::make_pair(first, second));
        } else {
            pairs.push_back(std::make_pair(second, first));
        }
    }
    
    // in case of odd number of ints, the last one goes to remaining
    if (vecNumbers.size() % 2 != 0) {
        remaining.push_back(vecNumbers[vecNumbers.size() - 1]);
    }
}

// compare pairs by their larger element which is in second place since the pairs are already sorted
bool comparePairs(const std::pair<int, int>& a, const std::pair<int, int>& b) {
    return a.second < b.second;
}

// extract the larger elements from pairs for recursive sorting -> store every second number of a pair in largerElements
std::vector<int> PmergeMe::extractLargerElements(const std::vector<std::pair<int, int> >& pairs) const {
    std::vector<int> largerElements;
    for (size_t i = 0; i < pairs.size(); ++i) {
        largerElements.push_back(pairs[i].second);
    }
    return largerElements;
}

void PmergeMe::sortPairs(std::vector<std::pair<int, int> >& pairs) const {
    // in case of 0 or 1 pair, no sorting needed
    if (pairs.size() <= 1) {
        return;
    }
    
    std::cout << "  [DEBUG] Recursive call with " << pairs.size() << " pairs" << std::endl;
    
    // extract larger elements from pairs for recursive sorting
    std::vector<int> largerElements = extractLargerElements(pairs);
    
    std::cout << "  [DEBUG] Extracted larger elements: ";
    for (size_t i = 0; i < largerElements.size(); ++i) {
        std::cout << largerElements[i] << " ";
    }
    std::cout << std::endl;
    
    // recursively sort the larger elements using Ford-Johnson
    if (largerElements.size() > 1) {
        std::cout << "  [DEBUG] Creating sub-pairs from larger elements..." << std::endl;
        
        // create pairs from larger elements
        std::vector<std::pair<int, int> > subPairs;
        std::vector<int> subRemaining;
        
        // manually create pairs from larger elements
        for (size_t i = 0; i < largerElements.size() - 1; i += 2) {
            int first = largerElements[i];
            int second = largerElements[i + 1];
            
            std::cout << "  [DEBUG] Creating sub-pair from elements " << i << " and " << (i+1) << ": " << first << " and " << second << std::endl;
            
            if (first <= second) {
                subPairs.push_back(std::make_pair(first, second));
                std::cout << "  [DEBUG] Created sub-pair: (" << first << "," << second << ")" << std::endl;
            } else {
                subPairs.push_back(std::make_pair(second, first));
                std::cout << "  [DEBUG] Created sub-pair: (" << second << "," << first << ") [swapped]" << std::endl;
            }
        }
        
        if (largerElements.size() % 2 != 0) {
            subRemaining.push_back(largerElements[largerElements.size() - 1]);
            std::cout << "  [DEBUG] Added remaining element: " << largerElements[largerElements.size() - 1] << std::endl;
        }
        
        std::cout << "  [DEBUG] Created " << subPairs.size() << " sub-pairs" << std::endl;
        
        // recursive call to sort the sub-pairs
        sortPairs(subPairs);
        
        // extract the sorted larger elements from sorted sub-pairs
        largerElements.clear();
        for (size_t i = 0; i < subPairs.size(); ++i) {
            largerElements.push_back(subPairs[i].second);
        }
        // add any remaining larger elements
        for (size_t i = 0; i < subRemaining.size(); ++i) {
            largerElements.push_back(subRemaining[i]);
        }
        
        // in case of fewer sub-pairs than original pairs handle the missing elements
        // this happens when there is an even number of larger elements
        if (subPairs.size() < pairs.size()) {
            std::cout << "  [DEBUG] Handling missing elements (sub-pairs: " << subPairs.size() << " < original: " << pairs.size() << ")" << std::endl;
            
            // find the missing larger elements that weren't processed in sub-pairs
            std::vector<bool> processed(pairs.size(), false);
            for (size_t i = 0; i < subPairs.size(); ++i) {
                for (size_t j = 0; j < pairs.size(); ++j) {
                    if (!processed[j] && pairs[j].second == subPairs[i].second) {
                        processed[j] = true;
                        break;
                    }
                }
            }
            
            // add the unprocessed larger elements in their original order but no elements that are already in subRemaining
            for (size_t i = 0; i < pairs.size(); ++i) {
                if (!processed[i]) {
                    // check if element is already in subRemaining
                    bool alreadyInRemaining = false;
                    for (size_t k = 0; k < subRemaining.size(); ++k) {
                        if (pairs[i].second == subRemaining[k]) {
                            alreadyInRemaining = true;
                            break;
                        }
                    }
                    
                    if (!alreadyInRemaining) {
                        largerElements.push_back(pairs[i].second);
                        std::cout << "  [DEBUG] Added missing element: " << pairs[i].second << std::endl;
                    } else {
                        std::cout << "  [DEBUG] Skipped element " << pairs[i].second << " (already in remaining)" << std::endl;
                    }
                }
            }
        }
        
        std::cout << "  [DEBUG] After recursion, larger elements: ";
        for (size_t i = 0; i < largerElements.size(); ++i) {
            std::cout << largerElements[i] << " ";
        }
        std::cout << std::endl;
    }
    
    // reconstruct pairs with sorted larger elements
    std::vector<std::pair<int, int> > sortedPairs;
    std::vector<bool> used(pairs.size(), false);
    
    // find original pair for each sorted larger element
    for (size_t i = 0; i < largerElements.size(); ++i) {
        for (size_t j = 0; j < pairs.size(); ++j) {
            if (!used[j] && pairs[j].second == largerElements[i]) {
                sortedPairs.push_back(pairs[j]);
                used[j] = true;
                break;
            }
        }
    }
    
    std::cout << "  [DEBUG] Reconstructed " << sortedPairs.size() << " pairs" << std::endl;
    
    // update the original pairs vector
    pairs = sortedPairs;
}

// Step 2: Create pairs from consecutive elements and use debug prints
void PmergeMe::testPairs(int ac, char **av) {
    // Parse and validate arguments
    parseArguments(ac, av);
    // print original int sequence
    std::cout << "Original sequence: ";
    for (size_t i = 0; i < vecNumbers.size(); ++i) {
        std::cout << vecNumbers[i];
        if (i < vecNumbers.size() - 1) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
    
    // create the pairs
    std::vector<std::pair<int, int> > pairs;
    std::vector<int> remaining;
    createPairs(pairs, remaining);
    
    // print pairs
    std::cout << "Pairs created: ";
    for (size_t i = 0; i < pairs.size(); ++i) {
        std::cout << "(" << pairs[i].first << "," << pairs[i].second << ")";
        if (i < pairs.size() - 1) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
    
    // Step 3: Sort each pair by putting the higher int first within the pair
    sortPairs(pairs);
    
    std::cout << "Pairs sorted by larger elements: ";
    for (size_t i = 0; i < pairs.size(); ++i) {
        std::cout << "(" << pairs[i].first << "," << pairs[i].second << ")";
        if (i < pairs.size() - 1) {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
    
    // print remaining elements if any
    if (!remaining.empty()) {
        std::cout << "Remaining element: ";
        for (size_t i = 0; i < remaining.size(); ++i) {
            std::cout << remaining[i];
            if (i < remaining.size() - 1) {
                std::cout << " ";
            }
        }
        std::cout << std::endl;
    } else {
        std::cout << "No remaining elements (even number of elements)" << std::endl;
    }
    
    vecNumbers.clear();
    deqNumbers.clear();
}
