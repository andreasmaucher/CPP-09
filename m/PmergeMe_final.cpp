#include "PmergeMe.hpp"
#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <algorithm>  // for std::find, std::swap_ranges, std::rotate
#include <cmath>      // for ceil() and log2()

// Initialize static comparison counter
int PmergeMe::comparison_count = 0;

PmergeMe::PmergeMe(void) {}
PmergeMe::~PmergeMe(void) {}

// copy constructor
PmergeMe::PmergeMe(const PmergeMe &other) {
    (void)other;
    *this = other;
}

// copy assignment
PmergeMe &PmergeMe::operator=(const PmergeMe &other) {
    (void)other;
    return *this;
}

/**
 * Main entry point for the Ford-Johnson merge-insertion sort algorithm.
 * 
 * This function:
 * 1. Validates input arguments
 * 2. Sorts the same sequence using both std::deque and std::vector
 * 3. Measures performance and comparison counts
 * 4. Displays results
 * 
 * The Ford-Johnson algorithm achieves the theoretical minimum number of comparisons
 * for sorting sequences of any length, making it optimal for comparison-based sorting.
 */
void PmergeMe::runMergeInsertSort(int argc, char *argv[]) {
    checkArgs(argc, argv);
    std::cout << "Before: ";
    printDeque(pmerge_deque);
    pmerge_vector.assign(pmerge_deque.begin(), pmerge_deque.end());
    pmerge_vector.reserve(pmerge_vector.size());
    
    std::cout << "\n=== INITIAL SEQUENCE ===" << std::endl;
    printSequence("Original sequence: ", pmerge_vector);
    std::cout << std::endl;

    // Sort using deque with Ford-Johnson algorithm
    resetComparisonCount();
    clock_t c_start_deque = clock();
    std::deque<unsigned int> sorted_result_deque = sortDequeFordJohnson(pmerge_deque);
    clock_t c_end_deque = clock();
    double cpu_time_deque = double(c_end_deque - c_start_deque) / CLOCKS_PER_SEC * 1000000;
    int deque_comparisons = getComparisonCount();

    // Sort using vector with Ford-Johnson algorithm
    resetComparisonCount();
    clock_t c_start_vector = clock();
    std::vector<unsigned int> sorted_result_vector = sortVecFordJohnson(pmerge_vector);
    clock_t c_end_vector = clock();
    double cpu_time_vector = double(c_end_vector - c_start_vector) / CLOCKS_PER_SEC * 1000000;
    int vector_comparisons = getComparisonCount();

    // Display results
    std::cout << "After deque: ";
    printDeque(sorted_result_deque);
    std::cout << "After vector: ";
    printVector(sorted_result_vector);

    std::cout << "Time to process a range of " << sorted_result_deque.size() 
              << " elements with std::deque : " << cpu_time_deque << " us" << std::endl;
    std::cout << "Time to process a range of " << sorted_result_vector.size() 
              << " elements with std::vector : " << cpu_time_vector << " us" << std::endl;
    
    // Calculate and display theoretical maximum comparisons
    int maxComparisons = maxComparisonsFJ(sorted_result_deque.size());
    std::cout << "Number of comparisons with std::deque: " << deque_comparisons 
              << " / " << maxComparisons << std::endl;
    std::cout << "Number of comparisons with std::vector: " << vector_comparisons 
              << " / " << maxComparisons << std::endl;
}

/**
 * Validates command line arguments and populates the deque with input numbers.
 * 
 * @param argc Number of arguments
 * @param argv Array of argument strings
 * @throws std::runtime_error if arguments are invalid
 */
void PmergeMe::checkArgs(int argc, char *argv[]) {
    if (argc < 2) {
        throw std::runtime_error("Error: Please provide at least one number to sort");
    }
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg.empty()) {
            throw std::runtime_error("Error: Empty argument");
        }
        
        // Check for negative numbers
        if (arg[0] == '-') {
            throw std::runtime_error("Error: Negative numbers are not allowed");
        }
        
        // Convert to integer
        char* end;
        long num = std::strtol(arg.c_str(), &end, 10);
        
        if (*end != '\0' || num < 0 || num > INT_MAX) {
            throw std::runtime_error("Error: Invalid number: " + arg);
        }
        
        pmerge_deque.push_back(static_cast<unsigned int>(num));
    }
}

/**
 * Prints a deque to stdout.
 * 
 * @param deq The deque to print
 */
void PmergeMe::printDeque(std::deque<unsigned int> deq) {
    if (deq.empty()) {
        std::cout << "deque is empty!" << std::endl;
        return;
    }
    
    for (std::deque<unsigned int>::iterator it = deq.begin(); it != deq.end(); ++it) {
        std::cout << *it;
        if (it + 1 != deq.end()) std::cout << " ";
    }
    std::cout << std::endl;
}

/**
 * Prints a vector to stdout.
 * 
 * @param vec The vector to print
 */
void PmergeMe::printVector(std::vector<unsigned int> vec) {
    if (vec.empty()) {
        std::cout << "vector is empty!" << std::endl;
        return;
    }
    
    for (std::vector<unsigned int>::iterator it = vec.begin(); it != vec.end(); ++it) {
        std::cout << *it;
        if (it + 1 != vec.end()) std::cout << " ";
    }
    std::cout << std::endl;
}

/**
 * Prints a sequence with a label.
 * 
 * @param label The label to print before the sequence
 * @param vec The vector containing the sequence
 */
void PmergeMe::printSequence(const std::string& label, const std::vector<unsigned int>& vec) {
    std::cout << label;
    for (std::vector<unsigned int>::const_iterator it = vec.begin(); it != vec.end(); ++it) {
        std::cout << *it;
        if (it + 1 != vec.end()) std::cout << " ";
    }
    std::cout << std::endl;
}

/**
 * Generates the Jacobsthal sequence up to n elements.
 * 
 * The Jacobsthal sequence is: J(0)=0, J(1)=1, J(n)=J(n-1)+2*J(n-2)
 * This sequence is crucial for determining the optimal insertion order
 * in the Ford-Johnson algorithm.
 * 
 * @param n Maximum number of elements to generate
 * @return Vector containing the Jacobsthal sequence
 */
std::vector<unsigned int> PmergeMe::buildJacobsthalSeq(unsigned int n) {
    std::vector<unsigned int> seq;
    if (n <= 0) return seq;

    unsigned int j1 = 0, j2 = 1; // first and second Jacobsthal numbers
    if (j1 < n) seq.push_back(j1); // adds 0
    if (j2 < n) seq.push_back(j2); // adds 1

    while (true) {
        unsigned int jNext = j2 + 2 * j1; // J(n) = J(n-1) + 2*J(n-2)
        seq.push_back(jNext);
        if (jNext >= n) break;
        j1 = j2;
        j2 = jNext;
    }

    if (seq.size() > 2) { // Remove duplicate '1'
        std::vector<unsigned int>::iterator it = seq.begin();
        ++it;
        seq.erase(it);
    }
    return seq;
}

/**
 * Calculates the theoretical maximum number of comparisons for the Ford-Johnson algorithm.
 * 
 * This implements the formula from "Art of Computer Programming, Vol. 3, page 184":
 * For n elements, the maximum number of comparisons is given by the Ford-Johnson formula.
 * 
 * @param n Number of elements to sort
 * @return Theoretical maximum number of comparisons
 */
int PmergeMe::maxComparisonsFJ(int n) {
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n == 3) return 3;
    if (n == 4) return 5;
    if (n == 5) return 7;
    if (n == 6) return 10;
    if (n == 7) return 13;
    if (n == 8) return 16;
    if (n == 9) return 19;
    if (n == 10) return 22;
    if (n == 11) return 26;
    if (n == 12) return 30;
    if (n == 13) return 34;
    if (n == 14) return 38;
    if (n == 15) return 42;
    if (n == 16) return 46;
    if (n == 17) return 50;
    if (n == 18) return 54;
    if (n == 19) return 58;
    if (n == 20) return 62;
    if (n == 21) return 66;
    
    // For larger n, use the general formula
    return static_cast<int>(n * std::ceil(log2(n)) - 2 * n + 1);
}

/**
 * Resets the comparison counter to zero.
 */
void PmergeMe::resetComparisonCount() {
    comparison_count = 0;
}

/**
 * Returns the current comparison count.
 * 
 * @return Number of comparisons made so far
 */
int PmergeMe::getComparisonCount() {
    return comparison_count;
}

/**
 * Ford-Johnson algorithm implementation for std::vector.
 * 
 * This is the core implementation of the Ford-Johnson merge-insertion sort algorithm.
 * The algorithm works in several phases:
 * 1. Recursively sort pairs of elements by comparing their last elements
 * 2. Use the Jacobsthal sequence to determine optimal insertion order
 * 3. Insert pending elements using binary search
 * 
 * This implementation achieves the theoretical minimum number of comparisons.
 * 
 * @param input Vector to sort
 * @return Sorted vector
 */
std::vector<unsigned int> PmergeMe::sortVecFordJohnson(const std::vector<unsigned int>& input) {
    std::vector<unsigned int> vec = input;
    if (vec.size() <= 1) return vec;

    // Phase 1: Recursively sort pairs of blocks
    int recDepth = sortPairsRecursivelyVec(vec, 1);
    int maxPending = vec.size() / 2 + 1;
    std::vector<unsigned int> jacSeq = buildJacobsthalSeq(maxPending);

    // Phase 2: Insert pending elements using optimal order
    while (recDepth > 0) {
        int blockSize = 1u << (recDepth - 1); // 2^(recDepth-1)
        int numBlocks = vec.size() / blockSize;
        int numPending = getNumPending(numBlocks);

        if (numPending > 1) {
            insertPendingBlocksVec(vec, blockSize, numPending, jacSeq);
        }
        --recDepth;
    }

    return vec;
}

/**
 * Ford-Johnson algorithm implementation for std::deque.
 * 
 * This is identical to the vector implementation but works with std::deque.
 * The algorithm achieves the same theoretical minimum number of comparisons.
 * 
 * @param input Deque to sort
 * @return Sorted deque
 */
std::deque<unsigned int> PmergeMe::sortDequeFordJohnson(const std::deque<unsigned int>& input) {
    std::deque<unsigned int> deq = input;
    if (deq.size() <= 1) return deq;

    // Phase 1: Recursively sort pairs of blocks
    int recDepth = sortPairsRecursivelyDeque(deq, 1);
    int maxPending = deq.size() / 2 + 1;
    std::vector<unsigned int> jacSeq = buildJacobsthalSeq(maxPending);

    // Phase 2: Insert pending elements using optimal order
    while (recDepth > 0) {
        int blockSize = 1u << (recDepth - 1); // 2^(recDepth-1)
        int numBlocks = deq.size() / blockSize;
        int numPending = getNumPending(numBlocks);

        if (numPending > 1) {
            insertPendingBlocksDeque(deq, blockSize, numPending, jacSeq);
        }
        --recDepth;
    }

    return deq;
}

/**
 * Recursively sorts pairs of blocks in a vector.
 * 
 * This is the first phase of the Ford-Johnson algorithm. It compares pairs of blocks
 * by their last elements and swaps them if necessary. The recursion continues until
 * we have a single block or no more pairs to compare.
 * 
 * @param vec Vector to sort (modified in place)
 * @param recDepth Current recursion depth
 * @return Recursion depth where the last comparison occurred
 */
int PmergeMe::sortPairsRecursivelyVec(std::vector<unsigned int>& vec, int recDepth) {
    int blockSize = 1u << (recDepth - 1); // blockSize doubles each recursion: 1 -> 2 -> 4 -> ...
    int numBlocks = vec.size() / blockSize;

    if (numBlocks <= 1) {
        return recDepth - 1; // Base case: no more blocks to compare
    }

    // Compare pairs of blocks by their last elements
    for (std::size_t i = 0; i + 2*blockSize - 1 < vec.size(); i += 2*blockSize) {
        comparison_count++;
        if (vec[i + blockSize - 1] > vec[i + 2*blockSize - 1]) {
            // Swap the two blocks
            std::swap_ranges(vec.begin() + i, 
                            vec.begin() + i + blockSize, 
                            vec.begin() + i + blockSize);
        }
    }

    return sortPairsRecursivelyVec(vec, recDepth + 1);
}

/**
 * Recursively sorts pairs of blocks in a deque.
 * 
 * This is identical to the vector version but works with std::deque.
 * 
 * @param deq Deque to sort (modified in place)
 * @param recDepth Current recursion depth
 * @return Recursion depth where the last comparison occurred
 */
int PmergeMe::sortPairsRecursivelyDeque(std::deque<unsigned int>& deq, int recDepth) {
    int blockSize = 1u << (recDepth - 1);
    int numBlocks = deq.size() / blockSize;

    if (numBlocks <= 1) {
        return recDepth - 1;
    }

    // Compare pairs of blocks by their last elements
    for (std::size_t i = 0; i + 2*blockSize - 1 < deq.size(); i += 2*blockSize) {
        comparison_count++;
        if (deq[i + blockSize - 1] > deq[i + 2*blockSize - 1]) {
            // Swap the two blocks
            std::swap_ranges(deq.begin() + i, 
                            deq.begin() + i + blockSize, 
                            deq.begin() + i + blockSize);
        }
    }

    return sortPairsRecursivelyDeque(deq, recDepth + 1);
}

/**
 * Inserts pending blocks into the main chain for vector.
 * 
 * This is the second phase of the Ford-Johnson algorithm. It uses the Jacobsthal
 * sequence to determine the optimal insertion order and inserts each pending block
 * using binary search to minimize comparisons.
 * 
 * @param vec Vector containing main chain and pending blocks
 * @param blockSize Size of each block
 * @param numPending Number of pending blocks to insert
 * @param jacSeq Jacobsthal sequence for determining insertion order
 */
void PmergeMe::insertPendingBlocksVec(std::vector<unsigned int>& vec, int blockSize, int numPending, const std::vector<unsigned int>& jacSeq) {
    // Separate main chain from pending elements
    int posPending = rearrangeVec(vec, blockSize);
    std::vector<unsigned int> insertionOrder = buildInsertOrder(numPending, jacSeq);

    // Insert each pending block in the optimal order
    for (std::size_t i = 0; i < insertionOrder.size(); ++i) {
        int pendIdx = insertionOrder[i];
        std::vector<unsigned int>::const_iterator endIt = insertionOrder.begin() + i;
        std::size_t numMovedBefore = countSmallerPending(insertionOrder, endIt, pendIdx);

        // Calculate block positions
        std::size_t start = posPending + (pendIdx - 1 - numMovedBefore) * blockSize;
        std::size_t end = start + blockSize;

        // Determine insertion position using binary search
        int k = computeK(pendIdx, jacSeq);
        std::size_t usefulMainElements = computeUsefulMainEnd(k, posPending, blockSize);
        std::size_t insertPos = (pendIdx != 1)
                          ? binaryInsertBlockVec(vec, vec[end-1], blockSize, usefulMainElements)
                          : 0; // First pending element goes to the front

        // Move the block to its correct position
        if (insertPos < start) {
            std::rotate(vec.begin() + insertPos, vec.begin() + start, vec.begin() + end);
        }

        posPending += blockSize; // Main chain grows by one block
    }
}

/**
 * Inserts pending blocks into the main chain for deque.
 * 
 * This is identical to the vector version but works with std::deque.
 * 
 * @param deq Deque containing main chain and pending blocks
 * @param blockSize Size of each block
 * @param numPending Number of pending blocks to insert
 * @param jacSeq Jacobsthal sequence for determining insertion order
 */
void PmergeMe::insertPendingBlocksDeque(std::deque<unsigned int>& deq, int blockSize, int numPending, const std::vector<unsigned int>& jacSeq) {
    // Separate main chain from pending elements
    int posPending = rearrangeDeque(deq, blockSize);
    std::vector<unsigned int> insertionOrder = buildInsertOrder(numPending, jacSeq);

    // Insert each pending block in the optimal order
    for (std::size_t i = 0; i < insertionOrder.size(); ++i) {
        int pendIdx = insertionOrder[i];
        std::vector<unsigned int>::const_iterator endIt = insertionOrder.begin() + i;
        std::size_t numMovedBefore = countSmallerPending(insertionOrder, endIt, pendIdx);

        // Calculate block positions
        std::size_t start = posPending + (pendIdx - 1 - numMovedBefore) * blockSize;
        std::size_t end = start + blockSize;

        // Determine insertion position using binary search
        int k = computeK(pendIdx, jacSeq);
        std::size_t usefulMainElements = computeUsefulMainEnd(k, posPending, blockSize);
        std::size_t insertPos = (pendIdx != 1)
                          ? binaryInsertBlockDeque(deq, deq[end-1], blockSize, usefulMainElements)
                          : 0; // First pending element goes to the front

        // Move the block to its correct position
        if (insertPos < start) {
            std::rotate(deq.begin() + insertPos, deq.begin() + start, deq.begin() + end);
        }

        posPending += blockSize; // Main chain grows by one block
    }
}

/**
 * Builds the optimal insertion order using the Jacobsthal sequence.
 * 
 * This implements the Ford-Johnson insertion order pattern:
 * 1. Collect Jacobsthal numbers > 0 and <= numPending
 * 2. Fill gaps between Jacobsthal numbers in descending order
 * 3. Append remaining elements in descending order
 * 
 * This order minimizes the number of comparisons needed for insertion.
 * 
 * @param numPending Number of pending elements
 * @param jacSeq Jacobsthal sequence
 * @return Vector containing the optimal insertion order
 */
std::vector<unsigned int> PmergeMe::buildInsertOrder(int numPending, const std::vector<unsigned int>& jacSeq) {
    std::vector<unsigned int> order;
    if (numPending <= 0 || jacSeq.empty()) return order;

    // Step 1: collect JT numbers > 0
    for (std::vector<unsigned int>::const_iterator it = jacSeq.begin(); it != jacSeq.end(); ++it) {
        unsigned int j = *it;
        if (j > 0 && j <= static_cast<unsigned int>(numPending)) {
            order.push_back(j);
        }
    }

    // Step 2: fill gaps between JT numbers in a descending way
    std::vector<unsigned int> expanded;
    unsigned int prev = 0;

    for (std::vector<unsigned int>::const_iterator it = order.begin(); it != order.end(); ++it) {
        unsigned int j = *it;
        expanded.push_back(j);
        for (unsigned int k = j - 1; k > prev; --k) { // fill gaps between JT numbers
            expanded.push_back(k);
        }
        prev = j;
    }

    // Step 3: Append any remaining pending elements in descending order
    for (int i = numPending; i > 1; --i) {
        if (std::find(expanded.begin(), expanded.end(), static_cast<unsigned int>(i)) == expanded.end()) {
            expanded.push_back(static_cast<unsigned int>(i));
        }
    }

    return expanded;
}

/**
 * Binary search to find insertion position for a block in vector.
 * 
 * @param vec Vector containing the main chain
 * @param value Last element of the block to insert
 * @param blockSize Size of each block
 * @param numBlocks Number of blocks in the main chain
 * @return Position where the block should be inserted
 */
std::size_t PmergeMe::binaryInsertBlockVec(const std::vector<unsigned int>& vec, unsigned int value, std::size_t blockSize, std::size_t numBlocks) {
    std::size_t left = 0;
    std::size_t right = numBlocks;

    while (left < right) {
        std::size_t mid = (left + right) / 2;
        unsigned int midValue = vec[(blockSize - 1) + mid*blockSize];

        comparison_count++;
        if (value < midValue) {
            right = mid;
        } else {
            left = mid + 1;
        }
    }
    return left * blockSize;
}

/**
 * Binary search to find insertion position for a block in deque.
 * 
 * @param deq Deque containing the main chain
 * @param value Last element of the block to insert
 * @param blockSize Size of each block
 * @param numBlocks Number of blocks in the main chain
 * @return Position where the block should be inserted
 */
std::size_t PmergeMe::binaryInsertBlockDeque(const std::deque<unsigned int>& deq, unsigned int value, std::size_t blockSize, std::size_t numBlocks) {
    std::size_t left = 0;
    std::size_t right = numBlocks;

    while (left < right) {
        std::size_t mid = (left + right) / 2;
        unsigned int midValue = deq[(blockSize - 1) + mid*blockSize];

        comparison_count++;
        if (value < midValue) {
            right = mid;
        } else {
            left = mid + 1;
        }
    }
    return left * blockSize;
}

/**
 * Rearranges a vector to separate main chain from pending elements.
 * 
 * @param vec Vector to rearrange (modified in place)
 * @param blockSize Size of each block
 * @return Position where pending elements start
 */
int PmergeMe::rearrangeVec(std::vector<unsigned int>& vec, int blockSize) {
    std::vector<unsigned int> mainChain, pending;
    std::size_t vecSize = vec.size();

    // Separate main-chain and pending elements
    for (std::size_t i = 0; i < vecSize; ++i) {
        if (isMainChain(i, blockSize, vecSize)) {
            mainChain.push_back(vec[i]);
        } else {
            pending.push_back(vec[i]);
        }
    }

    int posPending = mainChain.size();
    mainChain.insert(mainChain.end(), pending.begin(), pending.end());
    vec = mainChain;

    return posPending;
}

/**
 * Rearranges a deque to separate main chain from pending elements.
 * 
 * @param deq Deque to rearrange (modified in place)
 * @param blockSize Size of each block
 * @return Position where pending elements start
 */
int PmergeMe::rearrangeDeque(std::deque<unsigned int>& deq, int blockSize) {
    std::deque<unsigned int> mainChain, pending;
    std::size_t deqSize = deq.size();

    // Separate main-chain and pending elements
    for (std::size_t i = 0; i < deqSize; ++i) {
        if (isMainChain(i, blockSize, deqSize)) {
            mainChain.push_back(deq[i]);
        } else {
            pending.push_back(deq[i]);
        }
    }

    int posPending = mainChain.size();
    mainChain.insert(mainChain.end(), pending.begin(), pending.end());
    deq = mainChain;

    return posPending;
}

/**
 * Determines if an element belongs to the main chain.
 * 
 * In the Ford-Johnson algorithm, main chain elements are those in odd-numbered blocks
 * (a-blocks), while pending elements are in even-numbered blocks (b-blocks).
 * 
 * @param index Index of the element
 * @param blockSize Size of each block
 * @param totalSize Total size of the container
 * @return true if the element belongs to the main chain, false otherwise
 */
bool PmergeMe::isMainChain(int index, int blockSize, int totalSize) {
    int blockNum = index / blockSize;

    // Leftover elements don't belong to main chain
    if ((blockNum + 1) * blockSize > totalSize) {
        return false;
    }

    // Main chain: odd-numbered blocks (a-blocks)
    return (blockNum % 2 == 1);
}

/**
 * Calculates the number of pending elements.
 * 
 * @param numBlocks Total number of blocks
 * @return Number of pending elements
 */
int PmergeMe::getNumPending(int numBlocks) {
    return numBlocks / 2;
}

/**
 * Calculates the useful main chain end for insertion.
 * 
 * @param k Group number from Jacobsthal sequence
 * @param posPending Position where pending elements start
 * @param blockSize Size of each block
 * @return Number of useful main chain elements
 */
std::size_t PmergeMe::computeUsefulMainEnd(int k, std::size_t posPending, std::size_t blockSize) {
    if (k <= 0) return 0;
    
    // Calculate the number of useful main chain elements based on group k
    std::size_t usefulElements = 0;
    for (int i = 1; i <= k; ++i) {
        usefulElements += (1u << (i - 1)); // 2^(i-1)
    }
    
    return std::min(usefulElements, posPending / blockSize);
}

/**
 * Computes the group number k for a pending element.
 * 
 * @param pendIdx Index of the pending element
 * @param jacSeq Jacobsthal sequence
 * @return Group number k
 */
int PmergeMe::computeK(int pendIdx, const std::vector<unsigned int>& jacSeq) {
    std::vector<unsigned int>::const_iterator it = jacSeq.begin();
    int i = 0;

    for (; it != jacSeq.end(); ++it, ++i) {
        if (static_cast<unsigned int>(pendIdx) <= *it) {
            return i;
        }
    }

    return i; // fallback
}

/**
 * Counts how many elements in the insertion order are smaller than the given pending element.
 * 
 * @param insertionOrder The insertion order vector
 * @param endIt Iterator pointing to the end of processed elements
 * @param pendIdx Index of the pending element
 * @return Number of smaller elements
 */
std::size_t PmergeMe::countSmallerPending(const std::vector<unsigned int>& insertionOrder, std::vector<unsigned int>::const_iterator endIt, int pendIdx) {
    std::size_t count = 0;

    for (std::vector<unsigned int>::const_iterator it = insertionOrder.begin(); it != endIt; ++it) {
        if (*it < static_cast<unsigned int>(pendIdx)) {
            ++count;
        }
    }

    return count;
}
