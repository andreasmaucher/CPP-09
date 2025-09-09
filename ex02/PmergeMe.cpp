#include "PmergeMe.hpp"

// this function organizes the whole algorithm
void PmergeMe::runMergeInsertSort(int ac, char **av) 
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
    printSequence("After deque:  ", sorted_result_deque);
    printSequence("After vector: ", sorted_result_vector);
    std::cout << "Time to process a range of " << sorted_result_deque.size() << " elements with std::deque : " << cpu_time_deque << " us" << std::endl;
    std::cout << "Time to process a range of " << sorted_result_vector.size() << " elements with std::vector : " << cpu_time_vector << " us" << std::endl;
    
    // Step 6: Calculate and display theoretical maximum comparisons
    int max_comparisons = maxComparisonsFJ(sorted_result_deque.size());
    std::cout << "Number of comparisons with std::deque vs. theoretical limit:  " << deque_comparisons << " / " << max_comparisons << std::endl;
    std::cout << "Number of comparisons with std::vector vs. theoretical limit: " << vector_comparisons << " / " << max_comparisons << std::endl;
    
    // Step 7: Verify that both containers are correctly sorted
    verifySorting(sorted_result_vector, sorted_result_deque);
}

// execution of the Ford-Johnson algorithm on std::deque
std::deque<unsigned int> PmergeMe::sortDequeFordJohnson(const std::deque<unsigned int>& input) 
{
    std::deque<unsigned int> deq = input;
    if (deq.size() <= 1) // already sorted
        return deq;

    // Step 1: determine how many insertion rounds we need to run and recursively swap blocks
    int recDepth = sortPairsRecursivelyDeque(deq, 1);
    // Step 2: calculate maxPending elements to know where to cut off Jacobsthal Sequence
    int maxPending = deq.size() / 2 + 1; // '+1' to cover cases where total is odd
    // Step 3: calculate Jacobsthal sequence
    std::vector<unsigned int> JTseq = getJacobsthalIndexes(maxPending);
    // Step 4: insert pending elements into the sequence
    while (recDepth > 0) 
    {
        int blockSize = 1u << (recDepth - 1); // '1<<n' -> '2^n' e.g. 1u << (4 - 1) = 1u << 3 = 2^3 = 8
        int numBlocks = deq.size() / blockSize;
        int numPending = getNumPending(numBlocks); // calculate the number of pending blocks
        if (numPending > 1) // no need to insert anything if there's only 1 pending element
            insertPendingBlocksDeque(deq, blockSize, numPending, JTseq);
        --recDepth;
    }
    return deq;
}

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
 *     ...
 *   Result: [2,11,0,17,6,15,8,16,3,10,1,21,9,18,14,19,5,12,4,20,13]
 * 
 * Level 3 (blockSize=4): Compare blocks of size 4
 *   [2,11,0,17] vs [6,15,8,16] → 17>16? YES → swap entire blocks
 *   [3,10,1,21] vs [9,18,14,19] → 21>19? YES → swap entire blocks  
 *   [5,12,4,20] vs [13] → (only one block, no comparison needed)
 *   Result: [6,15,8,16,2,11,0,17,3,10,1,21,9,18,14,19,5,12,4,20,13]
 */
int PmergeMe::sortPairsRecursivelyDeque(std::deque<unsigned int>& deq, int recDepth) 
{
    int blockSize = 1u << (recDepth - 1); // blockSize doubles each recursion: 1 -> 2 -> 4 -> ...
    int numBlocks = deq.size() / blockSize; // number of blocks to process

    if (numBlocks <= 1) // base case, no more blocks to compare with one another
        return recDepth - 1; // returns recursion level in which the last comparison took place

    // iterate through the blocks, compare the last element & swap blocks if needed
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

// calculate the Jacobsthal sequence
/*
Example calculation to show how this works:

Sequence: 11 2 17 0 16 8 6 15 10 3 21 1 18 9 14 19 12 5 4 20 13 -> 21 elements
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
std::vector<unsigned int> PmergeMe::getJacobsthalIndexes(unsigned int n) 
{
    std::vector<unsigned int> jacobsthal;
    unsigned int j0 = 0, j1 = 1;
    if (j0 < n) jacobsthal.push_back(j0);
    if (j1 < n) jacobsthal.push_back(j1);
    // continue generating JT until we exceed n (n == max_pending elements)
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

// calculate how many elements need to be inserted for the Ford Johnson algorithm
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
int PmergeMe::getNumPending(int numBlocks) 
{
    int numPending = (numBlocks / 2); // each pair contributes one 'b'
    if (numBlocks % 2 != 0) // if odd number of blocks, leftover 'b' also pending
        ++numPending;
    return numPending;
}

// inserts pending elements into the main chain using the optimal Ford Johnson insertion order
void PmergeMe::insertPendingBlocksDeque(std::deque<unsigned int>& deq, int blockSize, int numPending, const std::vector<unsigned int>& JTseq) 
{
    // Step 1: separate main chain from pending elements -> return where pending element starts in the deque
    int pendingPos = sortMainPendb2b(deq, blockSize);
    // Step 2: create optimal insertion sequence using Jacobsthal numbers
    std::vector<unsigned int> insertionOrder = buildInsertOrder(numPending, JTseq);
    // Step 3: insert pending elements one by one
    for (size_t i = 0; i < insertionOrder.size(); ++i) 
    {
        unsigned int pendIndex = insertionOrder[i]; // returns the index of the pending element to insert
        // count how many smaller pending elements where already inserted (main chain grows with each insertion)
        std::vector<unsigned int>::const_iterator endIt = insertionOrder.begin() + i;
        size_t numMovedBefore = countSmallerPending(insertionOrder, endIt, pendIndex);

        // find exact start/end positions of the pending block to insert
        size_t start = pendingPos + (pendIndex - 1 - numMovedBefore) * blockSize;
        size_t end = start + blockSize;

        // calculate how many main chain elements to consider for binary search (limit comparisons to relevant ones)
        int k = computeK(pendIndex, JTseq);
        size_t usefulMainElements = computeUsefulMainEnd(k, pendingPos, blockSize);
        // use binary search to find optimal insertion position for each pending block (no moves yet)
        size_t insertPos;
        if (pendIndex == 1) { // first pending element can be inserted right away
            insertPos = 0;
        } else {
            insertPos = binaryInsertBlockDeque(deq, deq[end-1], blockSize, usefulMainElements);
        }
        // insert the pending block at the correct position (only if insertion point != current pos)
        if (insertPos < start)
            std::rotate(deq.begin() + insertPos, deq.begin() + start, deq.begin() + end);
        pendingPos += blockSize; // main chain grew by one block
    }
}

// rearranges deque to make the main chain and pending elements continguous to make insertion easier
// Before (scattered): [Pending][Main][Pending][Main][Pending][Main]
// After (contiguous):  [Main Chain][Pending Elements]
/*
Example:
* Before: [2, 11, 0, 17][8, 16, 6, 15][3, 10, 1, 21][9, 18, 14, 19][5, 12, 4, 20][13]
*          ↑ Main Chain ↑  ↑ Pending ↑  ↑ Main Chain ↑  ↑ Pending ↑  ↑ Main Chain ↑  ↑ Pending ↑
* 
* After:  [2, 11, 0, 17, 3, 10, 1, 21, 5, 12, 4, 20][8, 16, 6, 15, 9, 18, 14, 19, 13]
*          ↑                    Main Chain                    ↑  ↑        Pending        ↑
*/
int PmergeMe::sortMainPendb2b(std::deque<unsigned int>& deq, int blockSize) 
{
    std::deque<unsigned int> mainChain, pending;
    size_t deqSize = deq.size();
    int pendingPos;
    // Separate main-chain and pending and add elements to the corresponding deque
    for (size_t i = 0; i < deqSize; ++i) {
        if (isMainChain(i, blockSize, deqSize))
            mainChain.push_back(deq[i]);
        else
            pending.push_back(deq[i]);
    }
    // track where pending elements will start, meaning where the main chain will end
    pendingPos = mainChain.size();
    // append pending elements to main chain
    mainChain.insert(mainChain.end(), pending.begin(), pending.end());
    deq = mainChain;
    return pendingPos;
}

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
bool PmergeMe::isMainChain(int index, int blockSize, int totalSize) 
{
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

/*
Example:
Input: numPending = 6; JTseq = [0, 1, 1, 3, 5, 11, 21]
Collect valid JT numbers <= numPending: jacobsthalNumbers = [1, 1, 3, 5]
Build insertion order: insertionOrder = [1, 1, 3, 2, 5, 4, 6]
*/
std::vector<unsigned int> PmergeMe::buildInsertOrder(int numPending, const std::vector<unsigned int>& JTseq) 
{
    std::vector<unsigned int> insertionOrder;
    if (numPending <= 0 || JTseq.empty())
        return insertionOrder;

    // Step 1: Collect Jacobsthal numbers > 0 and <= numPending
    std::vector<unsigned int> jacobsthalNumbers;
    for (std::vector<unsigned int>::const_iterator it = JTseq.begin(); it != JTseq.end(); ++it) {
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

// counts amount of smaller pending elements that have already been inserter before the current element
// compares indices not actual values!
size_t PmergeMe::countSmallerPending(const std::vector<unsigned int>& insertionOrder, std::vector<unsigned int>::const_iterator endIt, unsigned int pendIndex) 
{
    size_t count = 0;

    for (std::vector<unsigned int>::const_iterator it = insertionOrder.begin(); it != endIt; ++it) 
    {
        if (*it < (pendIndex))
            ++count;
    }
    return count;
}

// finds the index k in the Jacobsthal sequence where JTseq[k] >= pendIndex
// determine which group of pending elements is currently being processed
size_t PmergeMe::computeK(unsigned int pendIndex, const std::vector<unsigned int>& JTseq) 
{
    for (unsigned int k = 0; k < JTseq.size(); ++k)
    {
        if (pendIndex <= JTseq[k])
            return static_cast<int>(k);
    }
    // if pendIndex is larger than all JTseq values, return the last index
    return JTseq.size();
}

// calculate maximum number of blocks in the main chain there are relevant for inserting the pending element
size_t PmergeMe::computeUsefulMainEnd(int k, size_t pendingPos, size_t blockSize) 
{
    size_t usefulEnd = (1u << k) - 1; // 2^k - 1 blocks allowed according to FJ
    size_t availableBlocks = pendingPos / blockSize; // blocks in main chain

    if (usefulEnd > availableBlocks)
        usefulEnd = availableBlocks;

    return usefulEnd;
}

// returns the postition where the new element should be inserted in the main chain
// instead of searching each individual only searches blocks
size_t PmergeMe::binaryInsertBlockDeque(const std::deque<unsigned int>& deq, unsigned int value, size_t blockSize, size_t numBlocks) 
{
    // define the search range
    size_t left = 0;
    size_t right = numBlocks;

    while (left < right) {
        size_t mid = (left + right) / 2; // find the middle block
        unsigned int midValue = deq[(blockSize - 1) + mid*blockSize]; // get the last value of the middle block

        comparison_count++;
        if (value < midValue)
            right = mid;
        else
            left = mid + 1;
    }
    return left * blockSize;
}
