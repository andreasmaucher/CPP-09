#include "PmergeMe.hpp"

//! Step 1
// this function organizes the whole algorithm
void PmergeMe::runMergeInsertSort(int ac, char *av[]) 
{
    // Step 1: Parse and validate input arguments
    checkArgs(ac, av);
    printSequence("Before: ", pmerge_deque);
    
    // Step 2: Copy deque to vector and allocate memory for it upfront
    // -> eliminate memory allocation overhead between the two containers
    pmerge_vector.assign(pmerge_deque.begin(), pmerge_deque.end());
    pmerge_vector.reserve(pmerge_vector.size());

    // Step 3: Sort using deque and measure performance
    clock_t c_start_deque = clock();
    std::deque<unsigned int> sorted_result_deque = sortDequeFordJohnson(pmerge_deque);
    clock_t c_end_deque = clock();
    double cpu_time_deque = double(c_end_deque - c_start_deque) / CLOCKS_PER_SEC * 1000000; // double to keep precision in microseconds
    int deque_comparisons = getComparisonCount();

    resetComparisonCount();

    // Step 4: Sort using vector and measure performance
    clock_t c_start_vector = clock();
    std::vector<unsigned int> sorted_result_vector = sortVecFordJohnson(pmerge_vector);
    clock_t c_end_vector = clock();
    double cpu_time_vector = double(c_end_vector - c_start_vector) / CLOCKS_PER_SEC * 1000000;
    int vector_comparisons = getComparisonCount();

    // Step 5: Display results
    printSequence("After deque: ", sorted_result_deque);
    printSequence("After vector: ", sorted_result_vector);

    std::cout << "Time to process a range of " << sorted_result_deque.size() << " elements with std::deque : " << cpu_time_deque << " us" << std::endl;
    std::cout << "Time to process a range of " << sorted_result_vector.size() << " elements with std::vector : " << cpu_time_vector << " us" << std::endl;
    
    // Step 6: Calculate and display theoretical maximum comparisons
    int max_comparisons = maxComparisonsFJ(sorted_result_deque.size());
    
    // Display comparison counts vs theoretical maximum
    std::cout << "Number of comparisons with std::deque vs. theoretical limit:  " << deque_comparisons << " / " << max_comparisons << std::endl;
    std::cout << "Number of comparisons with std::vector vs. theoretical limit: " << vector_comparisons << " / " << max_comparisons << std::endl;
}


//! Step 2
// execution of the Ford-Johnson algorithm on std::deque
std::deque<unsigned int> PmergeMe::sortDequeFordJohnson(const std::deque<unsigned int>& input) {
    std::deque<unsigned int> deq = input;
    if (deq.size() <= 1) // Already sorted
        return deq;

    // Step 1: determine how many insertion rounds we need to run and recursively swap blocks
    int recDepth = sortPairsRecursivelyDeque(deq, 1);
    // Step 2: calculate maxPending elements to know where to cut off Jacobsthal Sequence
    int maxPending = deq.size() / 2 + 1; // '+1' to cover cases where total is odd
    // Step 3: calculate Jacobsthal sequence
    std::vector<unsigned int> jacSeq = getJacobsthalIndexes(maxPending);

    while (recDepth > 0) 
    {
        int blockSize = 1u << (recDepth - 1); // '1<<n' -> '2^n' e.g. lu << (4 - 1) = lu << 3 = 2^3 = 8
        int numBlocks = deq.size() / blockSize;
        int numPending = getNumPending(numBlocks); // calculate the number of pending blocks

        // Step 4: insert pending elements into the sequence
        if (numPending > 1) // no need to insert anything if there's only 1 pending element
            insertPendingBlocksDeque(deq, blockSize, numPending, jacSeq);
        --recDepth;
    }
    return deq;
}


//! Step 2.1
// function recursively compares pairs of blocks by their last elements and swaps them if needed
/**
 * Example with sequence [11, 2, 17, 0, 16, 8, 6, 15, 10, 3, 21, 1, 18, 9, 14, 19, 12, 5, 4, 20, 13]:
 * 
 * Level 1 (blockSize=1): Compare individual elements
 *   [11,2] → 11>2? YES → swap → [2,11]
 *   [17,0] → 17>0? YES → swap → [0,17] 
 *   [16,8] → 16>8? YES → swap → [8,16]
 *   Result: [2,11,0,17,8,16,6,15,3,10,1,21,9,18,14,19,5,12,4,20,13]
 * 
 * Level 2 (blockSize=2): Compare blocks of size 2
 *   [2,11] vs [0,17] → 11>17? NO → no swap
 *   [8,16] vs [6,15] → 16>15? YES → swap → [6,15,8,16]
 *   Result: [2,11,0,17,6,15,8,16,3,10,1,21,9,18,14,19,5,12,4,20,13]
 * 
 * Level 3 (blockSize=4): Compare blocks of size 4
 *   [2,11,0,17] vs [6,15,8,16] → 17>16? YES → swap entire blocks
 *   Result: [6,15,8,16,2,11,0,17,3,10,1,21,9,18,14,19,5,12,4,20,13].
 */
int PmergeMe::sortPairsRecursivelyDeque(std::deque<unsigned int>& deq, int recDepth) {
    int blockSize = 1u << (recDepth - 1); // blockSize doubles each recursion: 1 -> 2 -> 4 -> ...
    int numBlocks = deq.size() / blockSize; // number of blocks to process

    if (numBlocks <= 1) // base case, no more blocks to compare with one another
        return recDepth - 1; // returns recursion level in which the last comparison took place

    // Iterate through the blocks, compare the last element & swap blocks if needed
    for (size_t i = 0; i + 2*blockSize - 1 < deq.size(); i += 2*blockSize) 
    {
        comparison_count++;
        if (deq[i + blockSize - 1] > deq[i + 2*blockSize - 1]) 
        {
            std::swap_ranges(deq.begin() + i, deq.begin() + i + blockSize, deq.begin() + i + blockSize);
        }
    }
    return sortPairsRecursivelyDeque(deq, recDepth + 1);
}

//! Step 2.2
/*
Example calculation to show how this works:

Sequence: 11 2 17 0 16 8 6 15 10 3 21 1 18 9 14 19 12 5 4 20 13
maxPending = 21 / 2 + 1 = 10 + 1 = 11

Calculate Jacobsthal sequence:
J(0) = 0
J(1) = 1
J(2) = J(1) + 2*J(0) = 1 + 2*0 = 1
J(3) = J(2) + 2*J(1) = 1 + 2*1 = 3
J(4) = J(3) + 2*J(2) = 3 + 2*1 = 5
J(5) = J(4) + 2*J(3) = 5 + 2*3 = 11
J(6) = J(5) + 2*J(4) = 11 + 2*5 = 21

Original: [0, 1, 1, 3, 5, 11, 21]
After removing duplicates: [0, 1, 3, 5, 11, 21]

Filter for values ≤ maxPending (11) -> JT never higher than maxPending!

Collect Jacobsthal numbers > 0 and ≤ 11: [1, 3, 5, 11]
Build insertion order:
J=1 → [1]
J=3 → [1, 3, 2] (fill gap between 1 and 3)
J=5 → [1, 3, 2, 5, 4] (fill gap between 3 and 5)  
J=11 → [1, 3, 2, 5, 4, 11, 10, 9, 8, 7, 6] (fill gap between 5 and 11)
*/
// calculate the Jacobsthal sequence
std::vector<unsigned int> PmergeMe::getJacobsthalIndexes(unsigned int n) {
    std::vector<unsigned int> jacobsthal;
    unsigned int j0 = 0, j1 = 1;
    if (j0 < n) jacobsthal.push_back(j0);
    if (j1 < n) jacobsthal.push_back(j1);
    // Continue generating until we exceed n
    while (true) 
    {
        unsigned int jNext = j1 + 2 * j0; // J(n) = J(n-1) + 2*J(n-2)
        jacobsthal.push_back(jNext);
        if (jNext >= n) break;
        j0 = j1;
        j1 = jNext;
    }
    // Remove duplicate '1' if it exists (happens because J(1)=1 and J(2)=1)
    if (jacobsthal.size() > 2) {
        std::vector<unsigned int>::iterator it = jacobsthal.begin();
        ++it;
        jacobsthal.erase(it);
    }
    return jacobsthal;
}


//! Step 2.3
/*
Calculation example:

Sequence: 11 2 17 0 16 8 6 15 10 3 21 1 18 9 14 19 12 5 4 20 13
blockSize = 1u << (4 - 1) = 8
numBlocks = 21 / 8 = 2
numPending = getNumPending(2)

numPending = (2 / 2) = 1        // 2 blocks → 1 pair → 1 pending
if (2 % 2 != 0) → false         // No leftover block
Result: 1 pending element

blockSize = 1u << (3 - 1) = 4
numBlocks = 21 / 4 = 5
numPending = getNumPending(5)

numPending = (5 / 2) = 2        // 5 blocks → 2 pairs → 2 pending
if (5 % 2 != 0) → true          // Odd number of blocks
    ++numPending                // +1 for leftover block
Result: 2 + 1 = 3 pending elements

and so on ...
*/
// calculate how many elements need to be inserted for the Ford Johnson algorithm
int PmergeMe::getNumPending(int numBlocks) 
{
    int numPending = (numBlocks / 2); // each pair contributes one 'b'
    if (numBlocks % 2 != 0) // if odd number of blocks, leftover 'b' also pending
        ++numPending;
    return numPending;
}

//! Step 2.4
/*
Calculation Example:

Sequence: 11 2 17 0 16 8 6 15 10 3 21 1 18 9 14 19 12 5 4 20 13

int posPending = rearrangeDeque(deq, 4);
After Phase 1: [2, 11, 0, 17, 8, 16, 6, 15, 3, 10, 1, 21, 9, 18, 14, 19, 5, 12, 4, 20, 13]
After rearrange: [2, 11, 0, 17][8, 16, 6, 15][3, 10, 1, 21][9, 18, 14, 19][5, 12, 4, 20][13]
posPending = 4 (main chain ends at position 4)

std::vector<unsigned int> insertionOrder = buildInsertOrder(3, jacSeq);
jacSeq = [0, 1, 3, 5, 11, 21]
insertionOrder = [1, 3, 2] (Jacobsthal-based optimal order)

//! missing stuff
*/
// inserts pending elements into the main chain using the optimal Ford Johnson insertion order
void PmergeMe::insertPendingBlocksDeque(std::deque<unsigned int>& deq, int blockSize, int numPending, const std::vector<unsigned int>& jacSeq) {
    // Step 1: separate main chain from pending elements
    int posPending = rearrangeDeque(deq, blockSize);
    // Step 2: create optimal insertion sequence using Jacobsthal numbers
    std::vector<unsigned int> insertionOrder = buildInsertOrder(numPending, jacSeq);
    // Step 3: insert pending elements one by one
    for (size_t i = 0; i < insertionOrder.size(); ++i) {
        int pendIdx = insertionOrder[i];
        // count how many smaller pending elements where already inserted (main chain grows with each insertion)
        std::vector<unsigned int>::const_iterator endIt = insertionOrder.begin() + i;
        size_t numMovedBefore = countSmallerPending(insertionOrder, endIt, pendIdx);

        // find exact start/end positions of the pending block to insert
        size_t start = posPending + (pendIdx - 1 - numMovedBefore) * blockSize;
        size_t end = start + blockSize;

        // calculate how many main chain elements to consider for binary search (limit comparisons to relevant ones)
        int k = computeK(pendIdx, jacSeq);
        size_t usefulMainElements = computeUsefulMainEnd(k, posPending, blockSize);
        // use binary search to find optimal insertion position
        size_t insertPos = (pendIdx != 1)
                          ? binaryInsertBlockDeque(deq, deq[end-1], blockSize, usefulMainElements)
                          : 0; // first pending element can be inserted right away
        // insert the pending block at the correct position (only if insertion point != current pos)
        if (insertPos < start)
            std::rotate(deq.begin() + insertPos, deq.begin() + start, deq.begin() + end);
        posPending += blockSize; // main chain grew by one block
    }
}

//! Step 2.4.1
// rearranges deque to separate the main chain from pending elements
// Result: [main chain elements][pending elements]
int PmergeMe::rearrangeDeque(std::deque<unsigned int>& deq, int blockSize) {
    std::deque<unsigned int> mainChain, pending;
    size_t deqSize = deq.size();
    int posPending;
    // Separate main-chain and pending and add elements to the corresponding deque
    for (size_t i = 0; i < deqSize; ++i) {
        if (isMainChain(i, blockSize, deqSize))
            mainChain.push_back(deq[i]);
        else
            pending.push_back(deq[i]);
    }
    // track where pending elements will start, meaning where the main chain will end
    posPending = mainChain.size();
    // append pending elements to main chain
    mainChain.insert(mainChain.end(), pending.begin(), pending.end());
    deq = mainChain;
    return posPending;
}

//! Step 2.4.1.1
/*
Example:
Block 0: [11, 2, 17, 0]     → Even block → Pending
Block 1: [16, 8, 6, 15]     → Odd block  → Main chain
Block 2: [10, 3, 21, 1]     → Even block → Pending  
Block 3: [18, 9, 14, 19]    → Odd block  → Main chain
Block 4: [12, 5, 4, 20]     → Even block → Pending
Block 5: [13]               → Leftover   → Pending
*/
// identify which elements are part of the main chain (winners) vs. pending elements (losers)
bool PmergeMe::isMainChain(int index, int blockSize, int totalSize) {
    // determine to which block the element belongs to
    int blockNum = index / blockSize;
    // leftover elements (incomplete blocks) -> not main chain (if the next block would exceed total size)
    if ((blockNum + 1) * blockSize > totalSize)
        return false;
    // main chain: odd-numbered blocks (a-blocks) are always winners
    if (blockNum % 2 == 1)
        return true;
    return false; // all b-blocks and leftover are losers and added in pending
}

//! Step 2.4.2
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

//! Step 2.4.3
size_t PmergeMe::countSmallerPending(const std::vector<unsigned int>& insertionOrder, std::vector<unsigned int>::const_iterator endIt, int pendIdx) {
    size_t count = 0;

    for (std::vector<unsigned int>::const_iterator it = insertionOrder.begin(); it != endIt; ++it) {
        if (*it < static_cast<unsigned int>(pendIdx))
            ++count;
    }

    return count;
}

//! Step 2.4.4
int PmergeMe::computeK(int pendIdx, const std::vector<unsigned int>& jacSeq) {
    std::vector<unsigned int>::const_iterator it = jacSeq.begin();
    int i = 0;

    for (; it != jacSeq.end(); ++it, ++i)
        if (static_cast<unsigned int>(pendIdx) <= *it)
            return i;

    return i; // fallback
}

//! Step 2.4.5
size_t PmergeMe::computeUsefulMainEnd(int k, size_t posPending, size_t blockSize) {
    size_t usefulEnd = (1u << k) - 1; // 2^k - 1 blocks allowed according to FJ
    size_t availableBlocks = posPending / blockSize; // blocks in main chain

    if (usefulEnd > availableBlocks)
        usefulEnd = availableBlocks;

    return usefulEnd;
}


//! Step 2.4.6
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

