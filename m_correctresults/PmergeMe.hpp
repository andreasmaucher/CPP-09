#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <deque>
#include <vector>
#include <string>
#include <exception>
#include <iostream>

class PmergeMe
{
private:
    std::deque<unsigned int> pmerge_deque;
    std::vector<unsigned int> pmerge_vector;

    // Copy constructor
    PmergeMe(const PmergeMe &other);
    // Copy assignment operator
    PmergeMe &operator=(const PmergeMe &other);
    
    // Static counter for comparisons
    static int comparison_count;

    void checkArgs(int argc, char *argv[]);
    
    // Debug printout functions
    void printSequence(const std::string& label, const std::vector<unsigned int>& seq);
    void printSequence(const std::string& label, const std::deque<unsigned int>& seq);
    void printDeque(std::deque<unsigned int> pmerge_deque);
    void printVector(std::vector<unsigned int> pmerge_vector);

    std::vector<unsigned int> getJacobsthalIndexes(unsigned int n);
    std::vector<unsigned int> buildInsertOrder(int numPending, const std::vector<unsigned int>& jacSeq);

    // Binary insert functions (used in recursive implementations)
    void binaryInsertDeque(std::deque<unsigned int>& chain, unsigned int value);
    void binaryInsertVector(std::vector<unsigned int>& chain, unsigned int value);
    
    // Recursive algorithm implementations (not used in main flow but still implemented)
    std::deque<unsigned int> mergeInsertSortRecursiveDeque(const std::deque<unsigned int>& input);
    std::vector<unsigned int> mergeInsertSortRecursiveVector(const std::vector<unsigned int>& input);
    
    // Debug functions (used in recursive implementations)
    void printMainPendingNonParticipating(const std::string& step, 
                                         const std::vector<unsigned int>& main, 
                                         const std::vector<unsigned int>& pending, 
                                         const std::vector<unsigned int>& nonParticipating);
    void printMainPendingNonParticipating(const std::string& step, 
                                         const std::deque<unsigned int>& main, 
                                         const std::deque<unsigned int>& pending, 
                                         const std::deque<unsigned int>& nonParticipating);

    // Proper Ford-Johnson in-place algorithm methods
    std::vector<unsigned int> sortVecFordJohnson(const std::vector<unsigned int>& input);
    std::deque<unsigned int> sortDequeFordJohnson(const std::deque<unsigned int>& input);
    int sortPairsRecursivelyVec(std::vector<unsigned int>& vec, int recDepth);
    void insertPendingBlocksVec(std::vector<unsigned int>& vec, int blockSize, int numPending, const std::vector<unsigned int>& jacSeq);
    size_t binaryInsertBlockVec(const std::vector<unsigned int>& vec, unsigned int value, size_t blockSize, size_t numBlocks);
    int rearrangeVec(std::vector<unsigned int>& vec, int blockSize);
    bool isMainChain(int index, int blockSize, int totalSize);
    
    // Deque Ford-Johnson functions
    int sortPairsRecursivelyDeque(std::deque<unsigned int>& deq, int recDepth);
    void insertPendingBlocksDeque(std::deque<unsigned int>& deq, int blockSize, int numPending, const std::vector<unsigned int>& jacSeq);
    size_t binaryInsertBlockDeque(const std::deque<unsigned int>& deq, unsigned int value, size_t blockSize, size_t numBlocks);
    int rearrangeDeque(std::deque<unsigned int>& deq, int blockSize);
    int getNumPending(int numBlocks);
    size_t computeUsefulMainEnd(int k, size_t posPending, size_t blockSize);
    int computeK(int pendIdx, const std::vector<unsigned int>& jacSeq);
    size_t countSmallerPending(const std::vector<unsigned int>& insertionOrder, std::vector<unsigned int>::const_iterator endIt, int pendIdx);

public:
    PmergeMe(void);
    ~PmergeMe(void);
    void runMergeInsertSort(int argc, char *argv[]);
    
    // Static method to get comparison count
    static int getComparisonCount();
    static void resetComparisonCount();
    
    // Calculate maximum comparisons according to Ford-Johnson algorithm
    static int maxComparisonsFJ(int n);
};

#endif