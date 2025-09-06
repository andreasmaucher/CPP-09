#include "PmergeMe.hpp"

//! Step V.2 (after merge insert main function)
// execution of the Ford-Johnson algorithm on std::vector
std::vector<unsigned int> PmergeMe::sortVecFordJohnson(const std::vector<unsigned int>& input) {
    std::vector<unsigned int> vec = input;
    if (vec.size() <= 1) // Already sorted
        return vec;

    // Step 1: determine how many insertion rounds we need to run and recursively swap blocks
    int recDepth = sortPairsRecursivelyVec(vec, 1);
    // Step 2: calculate maxPending elements to know where to cut off Jacobsthal Sequence
    int maxPending = vec.size() / 2 + 1; // '+1' to accommodate for potential leftover
    // Step 3: calculate Jacobsthal sequence
    std::vector<unsigned int> jacSeq = getJacobsthalIndexes(maxPending);

    while (recDepth > 0) 
    {
        int blockSize = 1u << (recDepth - 1); // '1<<n' -> '2^n' e.g. lu << (4 - 1) = lu << 3 = 2^3 = 8
        int numBlocks = vec.size() / blockSize;
        int numPending = getNumPending(numBlocks); // calculate the number of pending blocks

        // Step 4: insert pending elements into the sequence
        if (numPending > 1) // no need to insert anything if there's only 1 pending element
            insertPendingBlocksVec(vec, blockSize, numPending, jacSeq);
        --recDepth;
    }
    return vec;
}

//! Step V.2.1
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
int PmergeMe::sortPairsRecursivelyVec(std::vector<unsigned int>& vec, int recDepth) {
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

//! Step V.2.2
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
// inserts pending elements into the main chain using the optimal Ford Johnson insertion order
void PmergeMe::insertPendingBlocksVec(std::vector<unsigned int>& vec, int blockSize, int numPending, const std::vector<unsigned int>& jacSeq) {
    // Step 1: Separate main chain from pending elements
    int posPending = rearrangeVec(vec, blockSize);
    // Step 2: create optimal insertion sequence using Jacobsthal numbers
    std::vector<unsigned int> insertionOrder = buildInsertOrder(numPending, jacSeq);
    // Step 3: insert pending elements one by one
    for (size_t i = 0; i < insertionOrder.size(); ++i) 
    {
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
                          ? binaryInsertBlockVec(vec, vec[end-1], blockSize, usefulMainElements)
                          : 0; // first pending element can be inserted right away
        // insert the pending block at the correct position (only if insertion point != current pos)
        if (insertPos < start) // do nothing when insertPos == start
            std::rotate(vec.begin() + insertPos, vec.begin() + start, vec.begin() + end);
        posPending += blockSize; // main chain grew by one block
    }
}

//! Step V.2.2.1

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

//! V 2.2.2
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
