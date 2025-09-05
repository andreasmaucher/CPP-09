#include "PmergeMe.hpp"
#include <stdexcept>
#include <string>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <algorithm>  // for std::find, std::swap_ranges, std::rotate
#include <cmath>  // for ceil() and log2()

// Initialize static comparison counter
// This tracks the total number of comparisons made during sorting
int PmergeMe::comparison_count = 0;



PmergeMe::PmergeMe(void) {}

PmergeMe::~PmergeMe(void) {}

// copy constructor
PmergeMe::PmergeMe(const PmergeMe &other)
{
   (void)other;
   *this = other;
}

// copy assignment
PmergeMe &PmergeMe::operator=(const PmergeMe &other)
{
   (void)other;
   return *this;
}

/**
 * Main entry point for the Ford-Johnson merge-insertion sort algorithm.
 * 
 * This function orchestrates the entire sorting process:
 * 1. Validates and parses command line arguments
 * 2. Creates copies of the input for both deque and vector sorting
 * 3. Measures performance (time and comparisons) for both containers
 * 4. Displays results including comparison counts vs theoretical maximum
 * 
 * The Ford-Johnson algorithm is optimal for comparison-based sorting,
 * achieving the theoretical minimum number of comparisons.
 * 
 * Example usage: ./PmergeMe 3 1 4 1 5 9 2 6
 * 
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 */
void PmergeMe::runMergeInsertSort(int argc, char *argv[]) 
{
    // Step 1: Parse and validate input arguments
    checkArgs(argc, argv);
    std::cout << "Before: ";
    printDeque(pmerge_deque);
    
    // Step 2: Create vector copy for comparison
    pmerge_vector.assign(pmerge_deque.begin(), pmerge_deque.end());
    pmerge_vector.reserve(pmerge_vector.size());
    
    std::cout << "\n=== INITIAL SEQUENCE ===" << std::endl;
    printSequence("Original sequence: ", pmerge_vector);
    std::cout << std::endl;

    // Step 3: Sort using deque and measure performance
    resetComparisonCount(); // Reset counter before deque sorting
    clock_t c_start_deque = clock();
    std::deque<unsigned int> sorted_result_deque = sortDequeFordJohnson(pmerge_deque);
    clock_t c_end_deque = clock();
    double cpu_time_deque = double(c_end_deque - c_start_deque) / CLOCKS_PER_SEC * 1000000;
    int deque_comparisons = getComparisonCount();

    // Step 4: Sort using vector and measure performance
    resetComparisonCount(); // Reset counter before vector sorting
    clock_t c_start_vector = clock();
    std::vector<unsigned int> sorted_result_vector = sortVecFordJohnson(pmerge_vector);
    clock_t c_end_vector = clock();
    double cpu_time_vector = double(c_end_vector - c_start_vector) / CLOCKS_PER_SEC * 1000000;
    int vector_comparisons = getComparisonCount();

    // Step 5: Display results
    std::cout << "After deque: ";
    printDeque(sorted_result_deque);
    std::cout << "After vector: ";
    printVector(sorted_result_vector);

    std::cout << "Time to process a range of " << sorted_result_deque.size() << " elements with std::deque : " << cpu_time_deque << " us" << std::endl;
    std::cout << "Time to process a range of " << sorted_result_vector.size() << " elements with std::vector : " << cpu_time_vector << " us" << std::endl;
    
    // Step 6: Calculate and display theoretical maximum comparisons
    int max_comparisons = maxComparisonsFJ(sorted_result_deque.size());
    
    // Display comparison counts vs theoretical maximum
    std::cout << "Number of comparisons with std::deque: " << deque_comparisons << " / " << max_comparisons << std::endl;
    std::cout << "Number of comparisons with std::vector: " << vector_comparisons << " / " << max_comparisons << std::endl;
}

/**
 * Validates command line arguments and populates the deque with input numbers.
 * 
 * This function parses each command line argument, validates that it's a positive integer,
 * and adds it to the internal deque for sorting.
 * 
 * Validation rules:
 * - Must provide at least one argument
 * - Each argument must be a valid positive integer
 * - No negative numbers allowed
 * - No non-numeric characters allowed
 * 
 * Example valid inputs: "123", "0", "999"
 * Example invalid inputs: "-5", "abc", "12.5", ""
 * 
 * @param argc Number of command line arguments
 * @param argv Array of command line argument strings
 * @throws std::runtime_error if arguments are invalid
 */
void PmergeMe::checkArgs(int argc, char *argv[]) {
    if (argc == 1)
            throw std::runtime_error("Please provide arguments to be sorted in the format ./PmergeMe 1 2 3 4");
    
    // Parse each argument starting from index 1 (skip program name)
    for (int i = 1; i < argc; i++) 
    {
        std::istringstream iss(argv[i]);
        int pmerge_int;
        
        // Check if the argument is a valid positive integer
        // iss.peek() == EOF ensures no extra characters after the number
        if (iss >> pmerge_int && iss.peek() == EOF && pmerge_int >= 0)
        {
            pmerge_deque.push_back(pmerge_int);
        }
        else
            throw std::runtime_error("Please provide valid numeric positive arguments");
    }
}

void PmergeMe::printDeque(std::deque<unsigned int> pmerge_deque) {
   if (pmerge_deque.empty())
      std::cout << "pmerge_deque is empty!" << std::endl;
   while (!pmerge_deque.empty())
   {
      std::cout << pmerge_deque.front() << " ";
      pmerge_deque.pop_front();
   }
   std::cout << std::endl;
};

void PmergeMe::printVector(std::vector<unsigned int> pmerge_deque) {
   if (pmerge_deque.empty())
      std::cout << "pmerge_deque is empty!" << std::endl;
   while (!pmerge_deque.empty())
   {
      std::cout << pmerge_deque.front() << " ";
      pmerge_deque.erase(pmerge_deque.begin());
   }
   std::cout << std::endl;
};


/**
 * Generates the Jacobsthal sequence up to n elements.
 * 
 * The Jacobsthal sequence is crucial for the Ford-Johnson algorithm's optimal insertion order.
 * It determines which elements to insert first to minimize comparisons.
 * 
 * Sequence formula: J(0)=0, J(1)=1, J(n)=J(n-1)+2*J(n-2)
 * 
 * Example sequence for n=10:
 * J(0)=0, J(1)=1, J(2)=1+2*0=1, J(3)=1+2*1=3, J(4)=3+2*1=5, J(5)=5+2*3=11
 * Result: [0, 1, 1, 3, 5, 11] (duplicate 1 removed: [0, 1, 3, 5, 11])
 * 
 * Why this matters:
 * - The sequence determines the optimal order for inserting "pending" elements
 * - Each Jacobsthal number represents a group size for insertion
 * - This minimizes the total number of comparisons needed
 * 
 * @param n Maximum number of elements to generate
 * @return Vector containing the Jacobsthal sequence
 */
std::vector<unsigned int> PmergeMe::getJacobsthalIndexes(unsigned int n) {
    std::vector<unsigned int> jacobsthal;
    if (n <= 0) return jacobsthal;
    
    // Generate Jacobsthal sequence: J(0)=0, J(1)=1, J(n)=J(n-1)+2*J(n-2)
    unsigned int j0 = 0, j1 = 1;
    if (j0 < n) jacobsthal.push_back(j0);
    if (j1 < n) jacobsthal.push_back(j1);
    
    // Continue generating until we exceed n
    while (true) {
        unsigned int jNext = j1 + 2 * j0; // J(n) = J(n-1) + 2*J(n-2)
        jacobsthal.push_back(jNext);
        if (jNext >= n) break;
        j0 = j1;
        j1 = jNext;
    }
    
    // Remove duplicate '1' if it exists (same as working implementation)
    // This happens because J(1)=1 and J(2)=1, so we get [0,1,1,...]
    if (jacobsthal.size() > 2) {
        std::vector<unsigned int>::iterator it = jacobsthal.begin();
        ++it;
        jacobsthal.erase(it);
    }
    
    return jacobsthal;
}

void PmergeMe::binaryInsertDeque(std::deque<unsigned int>& chain, unsigned int value) {
    // Manual binary search to control comparison counting
    std::deque<unsigned int>::iterator left = chain.begin();
    std::deque<unsigned int>::iterator right = chain.end();
    
    while (left < right) {
        std::deque<unsigned int>::iterator mid = left + (right - left) / 2;
        comparison_count++; // Count only this comparison
        if (value < *mid) {
            right = mid;
        } else {
            left = mid + 1;
        }
    }
    chain.insert(left, value);
}

std::vector<unsigned int> PmergeMe::mergeInsertSortRecursiveVector(const std::vector<unsigned int>& input) {
    // Base case: 0 or 1 elements are already sorted
    if (input.size() <= 1)
        return input;
    
    std::cout << "\n=== RECURSIVE CALL (size: " << input.size() << ") ===" << std::endl;
    printSequence("Input: ", input);
    
    // Step 1: Pair elements into smalls and larges
    std::vector<unsigned int> smalls;
    std::vector<unsigned int> larges;
    int leftover = -1;
    
    for (size_t i = 0; i + 1 < input.size(); i += 2) {
        unsigned int a = input[i];
        unsigned int b = input[i + 1];
        if (a < b) {
            smalls.push_back(a);
            larges.push_back(b);
        } else {
            smalls.push_back(b);
            larges.push_back(a);
        }
    }
    
    if (input.size() % 2 != 0) {
        leftover = input.back();
    }
    
    std::cout << "Step 1 - Pairing:" << std::endl;
    printSequence("Smalls: ", smalls);
    printSequence("Larges: ", larges);
    if (leftover != -1) {
        std::cout << "Leftover: " << leftover << std::endl;
    }
    
    // Step 2: Create a deque from larges and recursively sort it
    std::vector<unsigned int> larges_deque(larges.begin(), larges.end());
    std::cout << "Step 2 - Recursively sorting larges:" << std::endl;
    printSequence("Larges to sort: ", larges_deque);
    
    std::vector<unsigned int> main_chain = mergeInsertSortRecursiveVector(larges_deque);
    std::cout << "Step 2 - Main chain after recursion:" << std::endl;
    printSequence("Main chain: ", main_chain);
    
    // Step 3: Insert smalls based on Jacobsthal order
    std::vector<unsigned int> insertion_order = getJacobsthalIndexes(smalls.size());
    std::cout << "Step 3 - Jacobsthal insertion order:" << std::endl;
    printSequence("Insertion order: ", insertion_order);
    printSequence("Smalls to insert: ", smalls);
    
    std::vector<unsigned int> pending = smalls;
    std::vector<unsigned int> nonParticipating;
    if (leftover != -1) {
        nonParticipating.push_back(leftover);
    }
    
    printMainPendingNonParticipating("Before Jacobsthal insertion", main_chain, pending, nonParticipating);
    
    for (size_t i = 0; i < insertion_order.size(); ++i) {
        int index = insertion_order[i];
        if (index < (int)smalls.size()) {
            unsigned int value_to_insert = smalls[index];
            std::cout << "Inserting " << value_to_insert << " (index " << index << " in smalls)" << std::endl;
            binaryInsertVector(main_chain, value_to_insert);
            
            // Remove from pending
            pending.erase(std::remove(pending.begin(), pending.end(), value_to_insert), pending.end());
            
            std::cout << "After insertion " << (i + 1) << ":" << std::endl;
            std::ostringstream oss;
            oss << "State after insertion " << (i + 1);
            printMainPendingNonParticipating(oss.str(), main_chain, pending, nonParticipating);
        }
    }
    
    // Step 4: Insert leftover if exists
    if (leftover != -1) {
        std::cout << "Step 4 - Inserting leftover: " << leftover << std::endl;
        binaryInsertVector(main_chain, leftover);
        nonParticipating.clear();
    }
    
    std::cout << "Final result for this recursive call:" << std::endl;
    printSequence("Final: ", main_chain);
    std::cout << "=== END RECURSIVE CALL ===" << std::endl;
    
    return main_chain;
}

std::deque<unsigned int> PmergeMe::mergeInsertSortRecursiveDeque(const std::deque<unsigned int>& input) {
    // Base case: 0 or 1 elements are already sorted
    if (input.size() <= 1)
        return input;
    
    std::cout << "\n=== RECURSIVE CALL DEQUE (size: " << input.size() << ") ===" << std::endl;
    printSequence("Input: ", input);
    
    // Step 1: Pair elements into smalls and larges
    std::deque<unsigned int> smalls;
    std::deque<unsigned int> larges;
    int leftover = -1;
    
    for (size_t i = 0; i + 1 < input.size(); i += 2) {
        unsigned int a = input[i];
        unsigned int b = input[i + 1];
        if (a < b) {
            smalls.push_back(a);
            larges.push_back(b);
        } else {
            smalls.push_back(b);
            larges.push_back(a);
        }
    }
    
    if (input.size() % 2 != 0) {
        leftover = input.back();
    }
    
    std::cout << "Step 1 - Pairing:" << std::endl;
    printSequence("Smalls: ", smalls);
    printSequence("Larges: ", larges);
    if (leftover != -1) {
        std::cout << "Leftover: " << leftover << std::endl;
    }
    
    // Step 2: Create a deque from larges and recursively sort it
    std::deque<unsigned int> larges_deque(larges.begin(), larges.end());
    std::cout << "Step 2 - Recursively sorting larges:" << std::endl;
    printSequence("Larges to sort: ", larges_deque);
    
    std::deque<unsigned int> main_chain = mergeInsertSortRecursiveDeque(larges_deque);
    std::cout << "Step 2 - Main chain after recursion:" << std::endl;
    printSequence("Main chain: ", main_chain);
    
    // Step 3: Insert smalls based on Jacobsthal order
    std::vector<unsigned int> insertion_order = getJacobsthalIndexes(smalls.size());
    std::cout << "Step 3 - Jacobsthal insertion order:" << std::endl;
    printSequence("Insertion order: ", insertion_order);
    printSequence("Smalls to insert: ", smalls);
    
    std::deque<unsigned int> pending = smalls;
    std::deque<unsigned int> nonParticipating;
    if (leftover != -1) {
        nonParticipating.push_back(leftover);
    }
    
    printMainPendingNonParticipating("Before Jacobsthal insertion", main_chain, pending, nonParticipating);
    
    for (size_t i = 0; i < insertion_order.size(); ++i) {
        int index = insertion_order[i];
        if (index < (int)smalls.size()) {
            unsigned int value_to_insert = smalls[index];
            std::cout << "Inserting " << value_to_insert << " (index " << index << " in smalls)" << std::endl;
            binaryInsertDeque(main_chain, value_to_insert);
            
            // Remove from pending
            pending.erase(std::remove(pending.begin(), pending.end(), value_to_insert), pending.end());
            
            std::cout << "After insertion " << (i + 1) << ":" << std::endl;
            std::ostringstream oss;
            oss << "State after insertion " << (i + 1);
            printMainPendingNonParticipating(oss.str(), main_chain, pending, nonParticipating);
        }
    }
    
    // Step 4: Insert leftover if exists
    if (leftover != -1) {
        std::cout << "Step 4 - Inserting leftover: " << leftover << std::endl;
        binaryInsertDeque(main_chain, leftover);
        nonParticipating.clear();
    }
    
    std::cout << "Final result for this recursive call:" << std::endl;
    printSequence("Final: ", main_chain);
    std::cout << "=== END RECURSIVE CALL DEQUE ===" << std::endl;
    
    return main_chain;
}

void PmergeMe::binaryInsertVector(std::vector<unsigned int>& chain, unsigned int value) {
    // Manual binary search to control comparison counting
    std::vector<unsigned int>::iterator left = chain.begin();
    std::vector<unsigned int>::iterator right = chain.end();
    
    while (left < right) {
        std::vector<unsigned int>::iterator mid = left + (right - left) / 2;
        comparison_count++; // Count only this comparison
        if (value < *mid) {
            right = mid;
        } else {
            left = mid + 1;
        }
    }
    chain.insert(left, value);
}

void PmergeMe::printSequence(const std::string& label, const std::vector<unsigned int>& seq) {
    std::cout << label;
    for (size_t i = 0; i < seq.size(); ++i) {
        std::cout << seq[i];
        if (i < seq.size() - 1) std::cout << " ";
    }
    std::cout << std::endl;
}

void PmergeMe::printSequence(const std::string& label, const std::deque<unsigned int>& seq) {
    std::cout << label;
    for (size_t i = 0; i < seq.size(); ++i) {
        std::cout << seq[i];
        if (i < seq.size() - 1) std::cout << " ";
    }
    std::cout << std::endl;
}

void PmergeMe::printMainPendingNonParticipating(const std::string& step, 
                                                const std::vector<unsigned int>& main, 
                                                const std::vector<unsigned int>& pending, 
                                                const std::vector<unsigned int>& nonParticipating) {
    std::cout << "\n=== " << step << " ===" << std::endl;
    printSequence("Main: ", main);
    printSequence("Pending: ", pending);
    printSequence("Non-participating: ", nonParticipating);
    std::cout << std::endl;
}

void PmergeMe::printMainPendingNonParticipating(const std::string& step, 
                                                const std::deque<unsigned int>& main, 
                                                const std::deque<unsigned int>& pending, 
                                                const std::deque<unsigned int>& nonParticipating) {
    std::cout << "\n=== " << step << " ===" << std::endl;
    printSequence("Main: ", main);
    printSequence("Pending: ", pending);
    printSequence("Non-participating: ", nonParticipating);
    std::cout << std::endl;
}

// Static methods for comparison counting
int PmergeMe::getComparisonCount() {
    return comparison_count;
}

void PmergeMe::resetComparisonCount() {
    comparison_count = 0;
}

// Comparison function removed - now using manual binary search for precise counting

/**
 * Calculates the theoretical maximum number of comparisons for the Ford-Johnson algorithm.
 * 
 * This implements the formula from "Art of Computer Programming, Vol. 3, page 186" by Donald Knuth.
 * The Ford-Johnson algorithm achieves the theoretical minimum number of comparisons
 * for comparison-based sorting algorithms.
 * 
 * Formula: For each k from 1 to n, add ceil(log2(3k/4))
 * 
 * Practical example for n=5:
 * k=1: ceil(log2(3*1/4)) = ceil(log2(0.75)) = ceil(-0.415) = 0
 * k=2: ceil(log2(3*2/4)) = ceil(log2(1.5)) = ceil(0.585) = 1
 * k=3: ceil(log2(3*3/4)) = ceil(log2(2.25)) = ceil(1.17) = 2
 * k=4: ceil(log2(3*4/4)) = ceil(log2(3)) = ceil(1.585) = 2
 * k=5: ceil(log2(3*5/4)) = ceil(log2(3.75)) = ceil(1.906) = 2
 * Total: 0+1+2+2+2 = 7 comparisons maximum
 * 
 * This is why we see "66 / 66" for 21 elements - we're achieving the theoretical optimum!
 * 
 * @param n Number of elements to sort
 * @return Theoretical maximum number of comparisons
 */
int PmergeMe::maxComparisonsFJ(int n) {
    int sum = 0;
    
    // Apply the Ford-Johnson formula for each element position
    for (int k = 1; k <= n; ++k) {
        double value = (3.0 / 4.0) * k; // 3k/4
        sum += static_cast<int>(ceil(log2(value))); // ceil(log2(3k/4))
    }
    
    return sum;
}

/**
 * Ford-Johnson algorithm implementation for std::vector.
 * 
 * This is the core implementation of the Ford-Johnson merge-insertion sort algorithm.
 * The algorithm works in two main phases:
 * 
 * Phase 1: Recursive Pair Sorting
 * - Compare pairs of elements by their last elements
 * - Swap pairs if necessary to establish partial order
 * - Recursively sort larger blocks
 * 
 * Phase 2: Optimal Insertion
 * - Use Jacobsthal sequence to determine insertion order
 * - Insert pending elements using binary search
 * - Achieve theoretical minimum comparisons
 * 
 * Example with input [3,1,4,1,5]:
 * Phase 1: Compare pairs -> [1,3], [1,4], [5] -> Main chain: [1,3,5], Pending: [1,4]
 * Phase 2: Insert pending using Jacobsthal order -> Final: [1,1,3,4,5]
 * 
 * @param input Vector to sort
 * @return Sorted vector
 */
std::vector<unsigned int> PmergeMe::sortVecFordJohnson(const std::vector<unsigned int>& input) {
    std::vector<unsigned int> vec = input;
    if (vec.size() <= 1) // Already sorted
        return vec;

    // Phase 1: Recursively sort pairs of blocks
    // This establishes a partial order by comparing last elements of blocks
    int recDepth = sortPairsRecursivelyVec(vec, 1);
    int maxPending = vec.size() / 2 + 1; // '+1' to accommodate for potential leftover
    std::vector<unsigned int> jacSeq = getJacobsthalIndexes(maxPending);

    // Phase 2: Insert pending elements using optimal order
    // Work backwards through recursion levels, inserting pending blocks
    while (recDepth > 0) {
        int blockSize = 1u << (recDepth - 1); // '1<<n' -> '2^n' (blockSize doubles each level)
        int numBlocks = vec.size() / blockSize;
        int numPending = getNumPending(numBlocks);

        if (numPending > 1) // no need to insert anything if there's only 1 pending element
            insertPendingBlocksVec(vec, blockSize, numPending, jacSeq);

        --recDepth;
    }

    return vec;
}

std::deque<unsigned int> PmergeMe::sortDequeFordJohnson(const std::deque<unsigned int>& input) {
    std::deque<unsigned int> deq = input;
    if (deq.size() <= 1) // Already sorted
        return deq;

    int recDepth = sortPairsRecursivelyDeque(deq, 1);
    int maxPending = deq.size() / 2 + 1; // '+1' to accommodate for potential leftover
    std::vector<unsigned int> jacSeq = getJacobsthalIndexes(maxPending);

    while (recDepth > 0) {
        int blockSize = 1u << (recDepth - 1); // '1<<n' -> '2^n'
        int numBlocks = deq.size() / blockSize;
        int numPending = getNumPending(numBlocks);

        if (numPending > 1) // no need to insert anything if there's only 1 pending element
            insertPendingBlocksDeque(deq, blockSize, numPending, jacSeq);

        --recDepth;
    }

    return deq;
}

int PmergeMe::sortPairsRecursivelyDeque(std::deque<unsigned int>& deq, int recDepth) {
    int blockSize = 1u << (recDepth - 1); // blockSize doubles each recursion: 1 -> 2 -> 4 -> ...
    int numBlocks = deq.size() / blockSize; // number of blocks to process

    if (numBlocks <= 1) // base case, no more blocks to compare with one another
        return recDepth - 1; // returns recursion level in which the last comparison took place

    // Iterate over all adjacent block pairs
    for (size_t i = 0; i + 2*blockSize - 1 < deq.size(); i += 2*blockSize) {
        // Compare the last element of the two blocks, swap blocks if needed
        comparison_count++;
        if (deq[i + blockSize - 1] > deq[i + 2*blockSize - 1]) {
            std::swap_ranges(deq.begin() + i, 
                            deq.begin() + i + blockSize, 
                            deq.begin() + i + blockSize);
        }
    }

    return sortPairsRecursivelyDeque(deq, recDepth + 1);
}

/**
 * Recursively sorts pairs of blocks in a vector.
 * 
 * This is Phase 1 of the Ford-Johnson algorithm. It compares pairs of blocks
 * by their last elements and swaps them if necessary. The recursion continues
 * until we have a single block or no more pairs to compare.
 * 
 * How it works:
 * 1. Start with blockSize = 1 (individual elements)
 * 2. Compare adjacent pairs by their last elements
 * 3. Swap pairs if the first block's last element > second block's last element
 * 4. Double the blockSize and recurse
 * 
 * Example with [3,1,4,1,5]:
 * Level 1 (blockSize=1): Compare 3>1? Yes -> swap -> [1,3,4,1,5]
 *                       Compare 4>1? Yes -> swap -> [1,3,1,4,5]
 * Level 2 (blockSize=2): Compare [1,3] vs [1,4] -> 3>1? Yes -> swap -> [1,1,3,4,5]
 * 
 * @param vec Vector to sort (modified in place)
 * @param recDepth Current recursion depth (starts at 1)
 * @return Recursion depth where the last comparison occurred
 */
int PmergeMe::sortPairsRecursivelyVec(std::vector<unsigned int>& vec, int recDepth) {
    int blockSize = 1u << (recDepth - 1); // blockSize doubles each recursion: 1 -> 2 -> 4 -> ...
    int numBlocks = vec.size() / blockSize; // number of blocks to process

    if (numBlocks <= 1) // base case, no more blocks to compare with one another
        return recDepth - 1; // returns recursion level in which the last comparison took place

    // Iterate over all adjacent block pairs
    for (size_t i = 0; i + 2*blockSize - 1 < vec.size(); i += 2*blockSize) {
        // Compare the last element of the two blocks, swap blocks if needed
        comparison_count++;
        if (vec[i + blockSize - 1] > vec[i + 2*blockSize - 1]) {
            // Swap the two blocks using std::swap_ranges
            std::swap_ranges(vec.begin() + i, vec.begin() + i + blockSize, vec.begin() + i + blockSize);
        }
    }

    return sortPairsRecursivelyVec(vec, recDepth + 1);
}

/**
 * Inserts pending blocks into the main chain for vector.
 * 
 * This is Phase 2 of the Ford-Johnson algorithm. It uses the Jacobsthal
 * sequence to determine the optimal insertion order and inserts each pending block
 * using binary search to minimize comparisons.
 * 
 * How it works:
 * 1. Rearrange the vector to separate main chain from pending elements
 * 2. Build insertion order using Jacobsthal sequence
 * 3. For each pending block, find its optimal insertion position using binary search
 * 4. Move the block to its correct position using std::rotate
 * 
 * Example with main chain [1,3,5] and pending [2,4]:
 * Insertion order from Jacobsthal: [2, 4] (or [1, 2] if using indices)
 * Insert 2: binary search finds position 1 -> [1,2,3,5]
 * Insert 4: binary search finds position 3 -> [1,2,3,4,5]
 * 
 * @param vec Vector containing main chain and pending blocks
 * @param blockSize Size of each block
 * @param numPending Number of pending blocks to insert
 * @param jacSeq Jacobsthal sequence for determining insertion order
 */
void PmergeMe::insertPendingBlocksVec(std::vector<unsigned int>& vec, int blockSize, int numPending, const std::vector<unsigned int>& jacSeq) {
    // Step 1: Separate main chain from pending elements
    int posPending = rearrangeVec(vec, blockSize);
    
    // Step 2: Build optimal insertion order using Jacobsthal sequence
    std::vector<unsigned int> insertionOrder = buildInsertOrder(numPending, jacSeq);

    // Step 3: Insert each pending block in the optimal order
    for (size_t i = 0; i < insertionOrder.size(); ++i) {
        int pendIdx = insertionOrder[i];
        std::vector<unsigned int>::const_iterator endIt = insertionOrder.begin() + i;
        size_t numMovedBefore = countSmallerPending(insertionOrder, endIt, pendIdx);

        // Compute start and end indices for the pending block
        size_t start = posPending + (pendIdx - 1 - numMovedBefore) * blockSize;
        size_t end = start + blockSize;

        // Step 4: Find optimal insertion position using binary search
        int k = computeK(pendIdx, jacSeq);
        size_t usefulMainElements = computeUsefulMainEnd(k, posPending, blockSize);

        size_t insertPos = (pendIdx != 1)
                          ? binaryInsertBlockVec(vec, vec[end-1], blockSize, usefulMainElements)
                          : 0; // first pending element can be inserted right away

        // Step 5: Move the block to its correct position
        if (insertPos < start) // do nothing when insertPos == start
            std::rotate(vec.begin() + insertPos, vec.begin() + start, vec.begin() + end);

        posPending += blockSize; // main chain grew by one block
    }
}

/**
 * Binary search to find insertion position for a block in vector.
 * 
 * This function performs a binary search on the main chain to find the optimal
 * position to insert a pending block. It compares the last element of the pending
 * block with the last elements of main chain blocks.
 * 
 * How it works:
 * 1. Use binary search on the main chain blocks
 * 2. Compare the pending block's last element with each block's last element
 * 3. Find the position where the pending block should be inserted
 * 4. Return the position in terms of element index (not block index)
 * 
 * Example: Main chain blocks: [1,3], [5,7], [9,11] (last elements: 3, 7, 11)
 *          Pending block: [4,6] (last element: 6)
 *          Binary search: 6 < 7? Yes -> search left half
 *                        6 < 3? No -> insert after first block
 *          Result: position 2 (after [1,3])
 * 
 * @param vec Vector containing the main chain
 * @param value Last element of the block to insert
 * @param blockSize Size of each block
 * @param numBlocks Number of blocks in the main chain
 * @return Position where the block should be inserted
 */
size_t PmergeMe::binaryInsertBlockVec(const std::vector<unsigned int>& vec, unsigned int value, size_t blockSize, size_t numBlocks) {
    size_t left = 0; // inclusive
    size_t right = numBlocks; // exclusive

    while (left < right) {
        size_t mid = (left + right) / 2;
        unsigned int midValue = vec[(blockSize - 1) + mid*blockSize]; // Last element of block at position mid

        comparison_count++;
        if (value < midValue)
            right = mid;
        else
            left = mid + 1;
    }
    return left * blockSize; // Convert block position to element position
}

/**
 * Rearranges a vector to separate main chain from pending elements.
 * 
 * In the Ford-Johnson algorithm, after the recursive pair sorting phase,
 * we need to separate the "main chain" (a-blocks) from "pending" elements (b-blocks).
 * 
 * How it works:
 * 1. Iterate through all elements in the vector
 * 2. Use isMainChain() to determine if an element belongs to main chain or pending
 * 3. Collect main chain elements first, then pending elements
 * 4. Return the position where pending elements start
 * 
 * Example with blockSize=2 and vector [1,3,2,4,5]:
 * Block 0: [1,3] (even block -> pending)
 * Block 1: [2,4] (odd block -> main chain)  
 * Block 2: [5] (leftover -> pending)
 * Result: mainChain=[2,4], pending=[1,3,5] -> [2,4,1,3,5]
 * Return: posPending=2 (where pending elements start)
 * 
 * @param vec Vector to rearrange (modified in place)
 * @param blockSize Size of each block
 * @return Position where pending elements start
 */
int PmergeMe::rearrangeVec(std::vector<unsigned int>& vec, int blockSize) {
    std::vector<unsigned int> mainChain, pending;
    size_t vecSize = vec.size();
    int posPending;

    mainChain.reserve(vecSize);
    pending.reserve(vecSize);

    // Separate main-chain and pending elements
    for (size_t i = 0; i < vecSize; ++i) {
        if (isMainChain(i, blockSize, vecSize))
            mainChain.push_back(vec[i]);
        else
            pending.push_back(vec[i]);
    }

    // Combine main chain and pending elements
    posPending = mainChain.size();
    mainChain.insert(mainChain.end(), pending.begin(), pending.end());
    vec = mainChain;

    return posPending;
}

/**
 * Determines if an element belongs to the main chain.
 * 
 * In the Ford-Johnson algorithm, after pair sorting, elements are divided into:
 * - Main chain (a-blocks): odd-numbered blocks
 * - Pending (b-blocks): even-numbered blocks and leftover elements
 * 
 * How it works:
 * 1. Calculate which block the element belongs to: blockNum = index / blockSize
 * 2. Check if it's a leftover element (incomplete block)
 * 3. Check if it's in an odd-numbered block (main chain)
 * 
 * Example with blockSize=2:
 * Index 0: blockNum=0 (even -> pending)
 * Index 1: blockNum=0 (even -> pending)  
 * Index 2: blockNum=1 (odd -> main chain)
 * Index 3: blockNum=1 (odd -> main chain)
 * Index 4: blockNum=2 (even -> pending, but also leftover if totalSize=5)
 * 
 * @param index Index of the element
 * @param blockSize Size of each block
 * @param totalSize Total size of the container
 * @return true if the element belongs to the main chain, false otherwise
 */
bool PmergeMe::isMainChain(int index, int blockSize, int totalSize) {
    int blockNum = index / blockSize; // which block does this element belong to

    // leftover elements (incomplete blocks) -> not main chain
    if ((blockNum + 1) * blockSize > totalSize)
        return false;

    // main chain: odd-numbered blocks (a-blocks)
    if (blockNum % 2 == 1)
        return true;

    return false; // all b-blocks and leftover
}

int PmergeMe::getNumPending(int numBlocks) {
    int numPending = (numBlocks / 2); // each pair contributes one 'b'
    if (numBlocks % 2 != 0) // if odd number of blocks, leftover 'b' also pending
        ++numPending;

    return numPending;
}

size_t PmergeMe::computeUsefulMainEnd(int k, size_t posPending, size_t blockSize) {
    size_t usefulEnd = (1u << k) - 1; // 2^k - 1 blocks allowed according to FJ
    size_t availableBlocks = posPending / blockSize; // blocks in main chain

    if (usefulEnd > availableBlocks)
        usefulEnd = availableBlocks;

    return usefulEnd;
}

int PmergeMe::computeK(int pendIdx, const std::vector<unsigned int>& jacSeq) {
    std::vector<unsigned int>::const_iterator it = jacSeq.begin();
    int i = 0;

    for (; it != jacSeq.end(); ++it, ++i)
        if (static_cast<unsigned int>(pendIdx) <= *it)
            return i;

    return i; // fallback
}

size_t PmergeMe::countSmallerPending(const std::vector<unsigned int>& insertionOrder, std::vector<unsigned int>::const_iterator endIt, int pendIdx) {
    size_t count = 0;

    for (std::vector<unsigned int>::const_iterator it = insertionOrder.begin(); it != endIt; ++it) {
        if (*it < static_cast<unsigned int>(pendIdx))
            ++count;
    }

    return count;
}

// Deque Ford-Johnson implementation functions
void PmergeMe::insertPendingBlocksDeque(std::deque<unsigned int>& deq, int blockSize, int numPending, const std::vector<unsigned int>& jacSeq) {
    int posPending = rearrangeDeque(deq, blockSize);
    std::vector<unsigned int> insertionOrder = buildInsertOrder(numPending, jacSeq);

    for (size_t i = 0; i < insertionOrder.size(); ++i) {
        int pendIdx = insertionOrder[i];
        std::vector<unsigned int>::const_iterator endIt = insertionOrder.begin() + i;
        size_t numMovedBefore = countSmallerPending(insertionOrder, endIt, pendIdx);

        // Compute start and end indices for the pending block
        size_t start = posPending + (pendIdx - 1 - numMovedBefore) * blockSize;
        size_t end = start + blockSize;

        int k = computeK(pendIdx, jacSeq);
        size_t usefulMainElements = computeUsefulMainEnd(k, posPending, blockSize);

        size_t insertPos = (pendIdx != 1)
                          ? binaryInsertBlockDeque(deq, deq[end-1], blockSize, usefulMainElements)
                          : 0; // first pending element can be inserted right away

        if (insertPos < start) // do nothing when insertPos == start
            std::rotate(deq.begin() + insertPos, deq.begin() + start, deq.begin() + end);

        posPending += blockSize; // main chain grew by one block
    }
}

size_t PmergeMe::binaryInsertBlockDeque(const std::deque<unsigned int>& deq, unsigned int value, size_t blockSize, size_t numBlocks) {
    size_t left = 0; // inclusive
    size_t right = numBlocks; // exclusive

    while (left < right) {
        size_t mid = (left + right) / 2;
        unsigned int midValue = deq[(blockSize - 1) + mid*blockSize];

        comparison_count++;
        if (value < midValue)
            right = mid;
        else
            left = mid + 1;
    }
    return left * blockSize;
}

int PmergeMe::rearrangeDeque(std::deque<unsigned int>& deq, int blockSize) {
    std::deque<unsigned int> mainChain, pending;
    size_t deqSize = deq.size();
    int posPending;

    // Separate main-chain and pending
    for (size_t i = 0; i < deqSize; ++i) {
        if (isMainChain(i, blockSize, deqSize))
            mainChain.push_back(deq[i]);
        else
            pending.push_back(deq[i]);
    }

    posPending = mainChain.size();
    mainChain.insert(mainChain.end(), pending.begin(), pending.end());
    deq = mainChain;

    return posPending;
}

/**
 * Builds the optimal insertion order using the Jacobsthal sequence.
 * 
 * This is the heart of the Ford-Johnson algorithm's optimality. It determines
 * the exact order in which pending elements should be inserted to minimize
 * the total number of comparisons.
 * 
 * The insertion order follows this pattern:
 * 1. Collect Jacobsthal numbers that are <= numPending
 * 2. For each Jacobsthal number J, add J and fill gaps in descending order
 * 3. Add remaining elements in descending order
 * 
 * Example with numPending=5 and Jacobsthal sequence [0,1,3,5,11]:
 * Step 1: Collect J <= 5: [1, 3, 5]
 * Step 2: Build order: J=1 -> [1]
 *                     J=3 -> [1, 3, 2] (fill gap between 1 and 3)
 *                     J=5 -> [1, 3, 2, 5, 4] (fill gap between 3 and 5)
 * Step 3: No remaining elements (5 is the last)
 * Result: [1, 3, 2, 5, 4]
 * 
 * This order minimizes comparisons because:
 * - Jacobsthal numbers represent optimal group sizes
 * - Filling gaps ensures we don't waste comparisons
 * - Descending order for remaining elements is optimal
 * 
 * @param numPending Number of pending elements
 * @param jacSeq Jacobsthal sequence
 * @return Vector containing the optimal insertion order
 */
std::vector<unsigned int> PmergeMe::buildInsertOrder(int numPending, const std::vector<unsigned int>& jacSeq) {
    std::vector<unsigned int> insertionOrder;
    if (numPending <= 0 || jacSeq.empty())
        return insertionOrder;

    // Step 1: Collect Jacobsthal numbers > 0 and <= numPending
    std::vector<unsigned int> jacobsthalNumbers;
    for (std::vector<unsigned int>::const_iterator it = jacSeq.begin(); it != jacSeq.end(); ++it) {
        unsigned int j = *it;
        if (j > 0 && j <= static_cast<unsigned int>(numPending))
            jacobsthalNumbers.push_back(j);
    }

    // Step 2: Build insertion order following Ford-Johnson pattern
    // Pattern: J1, J1-1, J2, J2-1, J2-2, ..., J3, J3-1, ..., remaining in reverse
    unsigned int prev = 0;
    for (std::vector<unsigned int>::const_iterator it = jacobsthalNumbers.begin(); it != jacobsthalNumbers.end(); ++it) {
        unsigned int j = *it;
        
        // Add the Jacobsthal number itself
        insertionOrder.push_back(j);
        
        // Fill gaps in descending order from j-1 down to prev+1
        for (unsigned int k = j - 1; k > prev; --k) {
            insertionOrder.push_back(k);
        }
        prev = j;
    }

    // Step 3: Add remaining elements in descending order
    // These are elements > the last Jacobsthal number
    for (int i = numPending; i > static_cast<int>(prev); --i) {
        insertionOrder.push_back(static_cast<unsigned int>(i));
    }

    return insertionOrder;
}