#include "PmergeMe.hpp"

// Constructor
PmergeMe::PmergeMe(void) {}

// Destructor
PmergeMe::~PmergeMe(void) {}

// Copy constructor
PmergeMe::PmergeMe(const PmergeMe &other)
{
   (void)other;
   *this = other;
}

// Copy assignment Operator
PmergeMe &PmergeMe::operator=(const PmergeMe &other)
{
   (void)other;
   return *this;
}

// tracks the total number of comparisons made during sorting
int PmergeMe::comparison_count = 0;

int PmergeMe::getComparisonCount() 
{
    return comparison_count;
}

void PmergeMe::resetComparisonCount() 
{
    comparison_count = 0;
}

/*
Calculates the theoretical maximum number of comparisons for the Ford-Johnson algorithm.
This implements the formula from "Art of Computer Programming, Vol. 3, page 186" by Donald Knuth.

Formula: For each k from 1 to n, add ceil(log2(3k/4))
Practical example for n=5:
k=1: ceil(log2(3*1/4)) = ceil(log2(0.75)) = ceil(-0.415) = 0
k=2: ceil(log2(3*2/4)) = ceil(log2(1.5)) = ceil(0.585) = 1
k=3: ceil(log2(3*3/4)) = ceil(log2(2.25)) = ceil(1.17) = 2
k=4: ceil(log2(3*4/4)) = ceil(log2(3)) = ceil(1.585) = 2
k=5: ceil(log2(3*5/4)) = ceil(log2(3.75)) = ceil(1.906) = 2
Total: 0+1+2+2+2 = 7 comparisons maximum

maxComparisons = Σ(k=1 to n) ceil(log2(3k/4))
*/
int PmergeMe::maxComparisonsFJ(int n) 
{
    int sum = 0;
    for (int k = 1; k <= n; ++k) {
        double value = (3.0 / 4.0) * k;
        sum += static_cast<int>(ceil(log2(value)));
    }
    return sum;
}

// parse through the input and check that it only includes valid integers
void PmergeMe::checkArgs(int ac, char **av) 
{
    for (int i = 1; i < ac; i++) 
    {
        std::istringstream iss(av[i]);
        int pmerge_int;
        
        // iss.peek() == EOF ensures no extra characters after the number (e.g. 12a would pass otherwise)
        if (iss >> pmerge_int && iss.peek() == EOF && pmerge_int >= 0)
        {
            pmerge_deque.push_back(pmerge_int);
        }
        else
            throw std::runtime_error("Please provide valid numeric positive arguments.");
    }
}

// print the sequence for vector
void PmergeMe::printSequence(const std::string& label, const std::vector<unsigned int>& seq) 
{
    std::cout << label;
    for (size_t i = 0; i < seq.size(); ++i) {
        std::cout << seq[i];
        if (i < seq.size() - 1) std::cout << " ";
    }
    std::cout << std::endl;
}

// print the sequence for deque
void PmergeMe::printSequence(const std::string& label, const std::deque<unsigned int>& seq) 
{
    std::cout << label;
    for (size_t i = 0; i < seq.size(); ++i) {
        std::cout << seq[i];
        if (i < seq.size() - 1) std::cout << " ";
    }
    std::cout << std::endl;
}

// check if vector is sorted in ascending order
bool PmergeMe::isSorted(const std::vector<unsigned int>& vec) 
{
    for (size_t i = 1; i < vec.size(); ++i) {
        if (vec[i] < vec[i-1]) {
            return false;
        }
    }
    return true;
}

// check if deque is sorted in ascending order
bool PmergeMe::isSorted(const std::deque<unsigned int>& deq) 
{
    for (size_t i = 1; i < deq.size(); ++i) {
        if (deq[i] < deq[i-1]) {
            return false;
        }
    }
    return true;
}

// verify both containers are sorted and print results
void PmergeMe::verifySorting(const std::vector<unsigned int>& vec, const std::deque<unsigned int>& deq) 
{
    bool vector_sorted = isSorted(vec);
    bool deque_sorted = isSorted(deq);
    
    std::cout << "Vector is sorted: " << (vector_sorted ? "YES" : "NO") << std::endl;
    std::cout << "Deque is sorted:  " << (deque_sorted ? "YES" : "NO") << std::endl;
}