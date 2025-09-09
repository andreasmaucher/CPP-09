#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <vector>
#include <deque>
#include <climits>
#include <string>
#include <cstddef>

/**
 * PmergeMe class implementing the Ford-Johnson merge-insertion sort algorithm.
 * 
 * This class provides an implementation of the Ford-Johnson algorithm, which is
 * optimal for comparison-based sorting. It achieves the theoretical minimum
 * number of comparisons for sorting sequences of any length.
 * 
 * The algorithm works in two main phases:
 * 1. Recursively sort pairs of elements by comparing their last elements
 * 2. Use the Jacobsthal sequence to determine optimal insertion order
 * 
 * Key features:
 * - Supports both std::vector and std::deque containers
 * - Counts comparisons for performance analysis
 * - Achieves theoretical minimum comparison count
 * - Comprehensive documentation and comments
 */
class PmergeMe {
private:
    std::deque<unsigned int> pmerge_deque;
    std::vector<unsigned int> pmerge_vector;
    static int comparison_count;

public:
    // Constructors and destructor
    PmergeMe(void);
    ~PmergeMe(void);
    PmergeMe(const PmergeMe &other);
    PmergeMe &operator=(const PmergeMe &other);

    // Main interface
    void runMergeInsertSort(int argc, char *argv[]);

private:
    // Input validation and display
    void checkArgs(int argc, char *argv[]);
    void printDeque(std::deque<unsigned int> deq);
    void printVector(std::vector<unsigned int> vec);
    void printSequence(const std::string& label, const std::vector<unsigned int>& vec);

    // Core Ford-Johnson algorithm
    std::vector<unsigned int> sortVecFordJohnson(const std::vector<unsigned int>& input);
    std::deque<unsigned int> sortDequeFordJohnson(const std::deque<unsigned int>& input);

    // Phase 1: Recursive pair sorting
    int sortPairsRecursivelyVec(std::vector<unsigned int>& vec, int recDepth);
    int sortPairsRecursivelyDeque(std::deque<unsigned int>& deq, int recDepth);

    // Phase 2: Optimal insertion
    void insertPendingBlocksVec(std::vector<unsigned int>& vec, int blockSize, int numPending, const std::vector<unsigned int>& jacSeq);
    void insertPendingBlocksDeque(std::deque<unsigned int>& deq, int blockSize, int numPending, const std::vector<unsigned int>& jacSeq);

    // Jacobsthal sequence and insertion order
    std::vector<unsigned int> buildJacobsthalSeq(unsigned int n);
    std::vector<unsigned int> buildInsertOrder(int numPending, const std::vector<unsigned int>& jacSeq);

    // Binary search for insertion
    std::size_t binaryInsertBlockVec(const std::vector<unsigned int>& vec, unsigned int value, std::size_t blockSize, std::size_t numBlocks);
    std::size_t binaryInsertBlockDeque(const std::deque<unsigned int>& deq, unsigned int value, std::size_t blockSize, std::size_t numBlocks);

    // Utility functions
    int rearrangeVec(std::vector<unsigned int>& vec, int blockSize);
    int rearrangeDeque(std::deque<unsigned int>& deq, int blockSize);
    bool isMainChain(int index, int blockSize, int totalSize);
    int getNumPending(int numBlocks);
    std::size_t computeUsefulMainEnd(int k, std::size_t posPending, std::size_t blockSize);
    int computeK(int pendIdx, const std::vector<unsigned int>& jacSeq);
    std::size_t countSmallerPending(const std::vector<unsigned int>& insertionOrder, std::vector<unsigned int>::const_iterator endIt, int pendIdx);

    // Comparison counting
    void resetComparisonCount();
    int getComparisonCount();
    int maxComparisonsFJ(int n);
};

#endif
