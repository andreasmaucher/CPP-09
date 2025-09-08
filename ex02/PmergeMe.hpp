#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <deque>
#include <vector>
#include <string>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <cstdio>
#include <algorithm>
#include <cmath>

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
    
    // Debug printout functions
    void printSequence(const std::string& label, const std::vector<unsigned int>& seq);
    void printSequence(const std::string& label, const std::deque<unsigned int>& seq);
    
    // Ford-Johnson std::vector
    std::vector<unsigned int> sortVecFordJohnson(const std::vector<unsigned int>& input);
    int sortPairsRecursivelyVec(std::vector<unsigned int>& vec, int recDepth);
    void insertPendingBlocksVec(std::vector<unsigned int>& vec, int blockSize, int numPending, const std::vector<unsigned int>& JTseq);
    size_t binaryInsertBlockVec(const std::vector<unsigned int>& vec, unsigned int value, size_t blockSize, size_t numBlocks);
    int sortMainPendb2b(std::vector<unsigned int>& vec, int blockSize);
    
    // Ford-Johnson std::deque
    std::deque<unsigned int> sortDequeFordJohnson(const std::deque<unsigned int>& input);
    int sortPairsRecursivelyDeque(std::deque<unsigned int>& deq, int recDepth);
    void insertPendingBlocksDeque(std::deque<unsigned int>& deq, int blockSize, int numPending, const std::vector<unsigned int>& JTseq);
    size_t binaryInsertBlockDeque(const std::deque<unsigned int>& deq, unsigned int value, size_t blockSize, size_t numBlocks);
    int sortMainPendb2b(std::deque<unsigned int>& deq, int blockSize);

    // algo helper functions
    void checkArgs(int ac, char **av);
    std::vector<unsigned int> getJacobsthalIndexes(unsigned int n);
    std::vector<unsigned int> buildInsertOrder(int numPending, const std::vector<unsigned int>& JTseq);
    size_t computeUsefulMainEnd(int k, size_t pendingPos, size_t blockSize);
    size_t computeK(unsigned int pendIndex, const std::vector<unsigned int>& JTseq);
    bool isMainChain(int index, int blockSize, int totalSize);
    int getNumPending(int numBlocks);
    size_t countSmallerPending(const std::vector<unsigned int>& insertionOrder, std::vector<unsigned int>::const_iterator endIt, unsigned int pendIndex);

public:
    // Constructor
    PmergeMe(void);
    // Deconstructor
    ~PmergeMe(void);

    // main function running the whole algo
    void runMergeInsertSort(int ac, char **av);
    
    // static functions to calculate maximum comparisons according to Ford-Johnson algorithm
    static int getComparisonCount();
    static void resetComparisonCount();
    static int maxComparisonsFJ(int n);
    
    // sorting verification functions
    static bool isSorted(const std::vector<unsigned int>& vec);
    static bool isSorted(const std::deque<unsigned int>& deq);
    static void verifySorting(const std::vector<unsigned int>& vec, const std::deque<unsigned int>& deq);
};

#endif