#include "PmergeMe.hpp"

// execution of the Ford-Johnson algorithm on std::vector
std::vector<unsigned int> PmergeMe::sortVecFordJohnson(const std::vector<unsigned int>& input) 
{
    std::vector<unsigned int> vec = input;
    if (vec.size() <= 1) // Already sorted
        return vec;

    // Step 1: determine how many insertion rounds we need to run and recursively swap blocks
    int recDepth = sortPairsRecursivelyVec(vec, 1);
    // Step 2: calculate maxPending elements to know where to cut off Jacobsthal Sequence
    int maxPending = vec.size() / 2 + 1; // '+1' to accommodate for potential leftover
    // Step 3: calculate Jacobsthal sequence
    std::vector<unsigned int> JTseq = getJacobsthalIndexes(maxPending);

    while (recDepth > 0) 
    {
        int blockSize = 1u << (recDepth - 1); // '1<<n' -> '2^n' e.g. lu << (4 - 1) = lu << 3 = 2^3 = 8
        int numBlocks = vec.size() / blockSize;
        int numPending = getNumPending(numBlocks); // calculate the number of pending blocks

        // Step 4: insert pending elements into the sequence
        if (numPending > 1) // no need to insert anything if there's only 1 pending element
            insertPendingBlocksVec(vec, blockSize, numPending, JTseq);
        --recDepth;
    }
    return vec;
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
 *   Result: [2,11,0,17,6,15,8,16,3,10,1,21,9,18,14,19,5,12,4,20,13]
 * 
 * Level 3 (blockSize=4): Compare blocks of size 4
 *   [2,11,0,17] vs [6,15,8,16] → 17>16? YES → swap entire blocks
 *   Result: [6,15,8,16,2,11,0,17,3,10,1,21,9,18,14,19,5,12,4,20,13].
 */
int PmergeMe::sortPairsRecursivelyVec(std::vector<unsigned int>& vec, int recDepth) 
{
    int blockSize = 1u << (recDepth - 1); // blockSize doubles each recursion: 1 -> 2 -> 4 -> ...
    int numBlocks = vec.size() / blockSize; // number of blocks to process

    if (numBlocks <= 1) // base case, no more blocks to compare with one another
        return recDepth - 1; // returns recursion level in which the last comparison took place

    // Iterate through the blocks, compare the last element & swap blocks if needed
    for (size_t i = 0; i + 2*blockSize - 1 < vec.size(); i += 2*blockSize) {
        // Compare the last element of the two blocks, swap blocks if needed
        comparison_count++;
        if (vec[i + blockSize - 1] > vec[i + 2*blockSize - 1]) 
        {
            std::swap_ranges(vec.begin() + i, vec.begin() + i + blockSize, vec.begin() + i + blockSize);
        }
    }
    return sortPairsRecursivelyVec(vec, recDepth + 1);
}

// inserts pending elements into the main chain using the optimal Ford Johnson insertion order
void PmergeMe::insertPendingBlocksVec(std::vector<unsigned int>& vec, int blockSize, int numPending, const std::vector<unsigned int>& JTseq) 
{
    // Step 1: Separate main chain from pending elements
    int pendingPos = sortMainPendb2b(vec, blockSize);
    // Step 2: create optimal insertion sequence using Jacobsthal numbers
    std::vector<unsigned int> insertionOrder = buildInsertOrder(numPending, JTseq);
    // Step 3: insert pending elements one by one
    for (size_t i = 0; i < insertionOrder.size(); ++i) 
    {
        unsigned int pendIndex = insertionOrder[i];
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
            insertPos = binaryInsertBlockVec(vec, vec[end-1], blockSize, usefulMainElements);
        }
        // insert the pending block at the correct position (only if insertion point != current pos)
        if (insertPos < start) // do nothing when insertPos == start
            std::rotate(vec.begin() + insertPos, vec.begin() + start, vec.begin() + end);
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
int PmergeMe::sortMainPendb2b(std::vector<unsigned int>& vec, int blockSize) 
{
    std::vector<unsigned int> mainChain, pending;
    size_t vecSize = vec.size();
    int pendingPos;

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
    pendingPos = mainChain.size();
    mainChain.insert(mainChain.end(), pending.begin(), pending.end());
    vec = mainChain;

    return pendingPos;
}

// returns the postition where the new element should be inserted in the main chain
// instead of searching each individual only searches blocks
size_t PmergeMe::binaryInsertBlockVec(const std::vector<unsigned int>& vec, unsigned int value, size_t blockSize, size_t numBlocks) 
{
    // define the search range
    size_t left = 0;
    size_t right = numBlocks;

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
