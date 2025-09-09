// Implementing the Ford-Johnson algorithm in place was done following these articles:
// https://dev.to/emuminov/human-explanation-and-step-by-step-visualisation-of-the-ford-johnson-algorithm-5g91
// https://medium.com/@mohammad.ali.ibrahim.525/ford-johnson-algorithm-merge-insertion-4b024f0c3d42

#include "PmergeMe.hpp"
#include "debug.hpp" // debug print fcts

#include <cstdlib>		// atoi()
#include <cstddef>		// size_t
#include <algorithm>	// swap_ranges(), std::rotate(), std::find()
#include <vector>
#include <string>
#include <climits>		// INT_MAX
#include <cerrno>		// errno, ERANGE
#include <stdexcept>	// runtime_error()

static std::vector<int>	buildVecFromArgs(int argc, char** argv);

/**
Sorts a vector of integers using the Ford–Johnson (Merge-Insertion) algorithm.

Steps:
 1.	Vector Initialization: Converts the command-line arguments (argv) into a vector of integers.
	If only one number is provided, it returns immediately since it is already sorted.

 2.	Pair Division & Recursive Sorting: Divides the vector into pairs and sorts them recursively.
	This step produces a "main chain" and determines the recursion depth needed for pending insertions.

 3.	Pending Elements Initialization: Generates the Jacobsthal sequence up to the maximum number
	of pending elements. This sequence determines the optimal order for inserting elements
	not yet in the main chain.

 4.	Insertion of Pending Elements: Iterates from the deepest recursion level down to 1.
	- Computes block size and the number of blocks for the current recursion depth.
	- Determines the number of pending elements at this level.
	- Inserts pending elements according to the Jacobsthal sequence if any exist.

 5.	Returns the fully sorted vector.

 @param argc	Number of command-line arguments.
 @param argv	Command-line argument strings representing integers to sort.
 @param numComp	Reference to an integer counting the number of comparisons made.
 @return		The sorted vector of integers.
*/
std::vector<int>	PmergeMe::sortVec(int argc, char** argv, int& numComp)
{
	std::vector<int>	vec = buildVecFromArgs(argc, argv);
	if (vec.size() <= 1) // Already sorted
		return vec;

	int					recDepth = sortPairsRecursivelyVec(vec, numComp, 1);
	int					maxPending = vec.size() / 2 + 1; // '+1' to accommodate for potential leftover
	std::vector<int>	jacSeq = buildJacobsthalSeq(maxPending);

	while (recDepth > 0)
	{
		int	blockSize = 1u << (recDepth - 1); // '1<<n' -> '2^n'; u makes 1 unsigned (safer)
		int	numBlocks = vec.size() / blockSize;
		int	numPending = getNumPending(numBlocks);

		debugMainChainSorted(vec, recDepth, blockSize, numBlocks, numPending);
		if (numPending > 1) // no need to insert anything if there's only 1 pending element, already sorted! -> 'b1 < a1'
			insertPendingBlocksVec(vec, blockSize, numPending, jacSeq, numComp);

		--recDepth;
	}

	return vec;
}

/**
Recursively sorts the main chain in the Ford–Johnson algorithm by comparing
pairs, then pairs of pairs, and so on.
At each level, pairs of blocks are compared by their last elements,
and blocks are swapped if needed. Leftover elements are not touched.

Let's say, there are 13 numbers (n = 13):
 - Lvl 1: 13 -> 6 pairs -> 6 comparisons
 - Lvl 2: 6 -> 3 pairs -> 3 comparisons
 - Lvl 3: 3 -> 1 pair -> 1 comparison
-> General formula: At each level i, ⌊n_i / 2⌋ (floor down) comparisons
-> Using formula: ⌊13/2⌋ + ⌊6/2⌋ + ⌊3/2⌋ = 6 + 3 + 1 = 10 comparisons

 @param vec			The vector to sort (in place)
 @param numComp		Counter for number of comparisons
 @param recDepth	Current recursion depth (starting at 1)
 @return			The recursion depth in which the last comparison took place.
*/
int	PmergeMe::sortPairsRecursivelyVec(std::vector<int>& vec, int& numComp, int recDepth)
{
	int	blockSize =  1u << (recDepth - 1);	// blockSize doubles each recursion: 1 -> 2 -> 4 -> ...
	int	numBlocks = vec.size() / blockSize;	// number of blocks to process

	if (numBlocks <= 1)			// base case, no more blocks to compare with one another
		return recDepth - 1;	// returns recursion level in which the last comparison took place

	// Iterate over all adjacent block pairs
	for (size_t i = 0; i + 2*blockSize - 1 < vec.size(); i += 2*blockSize)
	{
		// Compare the last element of the two blocks, swap blocks if needed
		++numComp;
		if (vec[i + blockSize - 1] > vec[i + 2*blockSize - 1])
		{
			swap_ranges(	vec.begin() + i,				// start first block
							vec.begin() + i + blockSize,	// one past the end of first block
							vec.begin() + i + blockSize);	// start second block
		}
	}

	return PmergeMe::sortPairsRecursivelyVec(vec, numComp, recDepth + 1);
}

/**
Rearranges a vector so that main chain elements are in front
and pending elements (+ leftovers) are at the back.

 @param vec			The interleaved input vector `[b1 a1 b2 a2 b3 a3 ...]`,
 					possibly with leftover elements. This vector is modified
					in place to contain `[mainChain | pending | (leftovers)]`
 @param blockSize	Number of elements per block.
 @return			The index at which the pending elements start in `vec`.
					All elements from this index onward belong to the pending chain.
*/
int	PmergeMe::rearrangeVec(std::vector<int>& vec, int blockSize)
{
	std::vector<int>	mainChain, pending;
	size_t				vecSize = vec.size();
	int					posPending;

	mainChain.reserve(vecSize); // overestimating, but oh well
	pending.reserve(vecSize);

	// Separate main-chain and pending
	for (size_t i = 0; i < vecSize; ++i)
	{
		if (isMainChain(i, blockSize, vecSize))
			mainChain.push_back(vec[i]);
		else
			pending.push_back(vec[i]);
	}

	posPending = mainChain.size();
	mainChain.insert(mainChain.end(), pending.begin(), pending.end()); // merge main-chain + pending (+ leftovers)
	vec = mainChain; // Reassign vec

	return posPending;
}

/**
Inserts pending blocks into the main chain within a vector.

The function uses the Jacobsthal sequence and Ford–Johnson logic to
determine the order and number of comparisons needed for insertion.
Each pending block is inserted in binary-search order based on the
last element of each block. The main chain grows as blocks are inserted.

 @param vec			The vector containing main chain blocks and pending blocks.
 @param blockSize	Number of elements per block.
 @param numPending	Number of pending blocks to insert.
 @param jacSeq		Jacobsthal sequence used to compute insertion group `k`.
 @param numComp		Reference counter for the number of comparisons performed.
*/
void	PmergeMe::insertPendingBlocksVec(std::vector<int>& vec, int blockSize, int numPending,
			const std::vector<int>& jacSeq, int& numComp)
{
	int					posPending = rearrangeVec(vec, blockSize);
	std::vector<int>	insertionOrder = buildInsertOrder(numPending, jacSeq);

	debugVecRearranged(vec, insertionOrder, posPending, blockSize);

	for (size_t i = 0; i < insertionOrder.size(); ++i)
	{
		int		pendIdx = insertionOrder[i]; // basically the b_x: pendIdx = 3 -> b3
		std::vector<int>::const_iterator	endIt = insertionOrder.begin();
		std::advance(endIt, i);
		size_t	numMovedBefore = PmergeMe::countSmallerPending(insertionOrder, endIt, pendIdx);

		// Compute start and end indices for the pending block	
		size_t	start = posPending + (pendIdx - 1 - numMovedBefore) * blockSize;
		size_t	end = start + blockSize;

		int		k = computeK(pendIdx, jacSeq);
		size_t	usefulMainElements = computeUsefulMainEnd(k, posPending, blockSize);

		debugPreInsert(vec, pendIdx, end, k, usefulMainElements, numComp);
		size_t	insertPos =	(pendIdx != 1)
							? binaryInsertBlockVec(vec, vec[end-1], blockSize, usefulMainElements, numComp)
							: 0; // first pending element (b1) can be inserted right away to top of main chain

		if (insertPos < start) // do nothing when insertPos == start; insertPos > start is not possible (would insert in pending, not main)
			std::rotate(vec.begin() + insertPos, vec.begin() + start, vec.begin() + end); // moves main chain elements to right to make space

		debugPostInsert(vec, pendIdx, insertPos, numComp);
		posPending += blockSize; // main chain grew by one block -> posPending starts at +blockSize
	}
}

/**
Finds the insertion index for a pending block in a main chain of blocks.

Uses binary search to locate the position of the pending block based on its
last element, comparing it only with the last element of each main chain block.
Counts comparisons in `numComp`.

 @param vec			Vector containing main chain blocks (assumed sorted by last element of each block).
 @param value		The representative value of the pending block (last element).
 @param blockSize	Number of elements per block.
 @param numBlocks	Number of useful main chain blocks to consider for insertion.
 @param numComp		Counter for the number of comparisons made.
 @return			Index in `vec` at which the pending block should be inserted.
*/
size_t	PmergeMe::binaryInsertBlockVec(	const std::vector<int>& vec, int value,
										size_t blockSize, size_t numBlocks, int& numComp)
{
	// use block indices instead of block number
	size_t	left = 0;			// inclusive
	size_t	right = numBlocks;	// exclusive

	while (left < right)
	{
		size_t	mid = (left + right) / 2;
		int		midValue = vec[(blockSize - 1) + mid*blockSize];

		DEBUG_PRINT("comparing with val: " + toString(midValue) + " (block: " + toString(mid + 1)
			+ ", pos: " + toString((mid + 1) * blockSize - 1) + ")");
		++numComp;
		if (value < midValue)
			right = mid;
		else
			left = mid + 1;
	} // when the loop ends, 'left' is the correct insertion index
	return left * blockSize;
}

////////////
// UTILITY FUNCTIONS //
////////////

// Checks if the provided arguments are valid integers.
// Throws exceptions for non-numeric values, non-positive numbers,
// and integers exceeding `INT_MAX`.
void	PmergeMe::checkArgs(int argc, char** argv)
{
	for (int i = 1; i < argc; ++i)
	{
		errno = 0; // Reset errno before call to strtol()
		char*		endptr;
		std::string	arg(argv[i]); // Convert C-style string (const char*) to std::string
		long		val = strtol(argv[i], &endptr, 10); // Convert to long (base 10); might set errno

		if (errno == ERANGE || val > INT_MAX)
			throw std::runtime_error("Integer overflow: value exceeds INT_MAX: " + arg);

		if (*argv[i] == '\0' || *endptr != '\0')
			throw std::runtime_error("Invalid input, not an integer: '" + arg + "'");

		if (val < 0)
			throw std::runtime_error("Only positive integers are allowed [0, INT_MAX]: " + arg);
	}
}

/**
 Computes the number of pending 'b' blocks in the Ford–Johnson sorting algorithm.

 - Blocks are paired as (a, b).
 - All 'b' blocks are considered pending.
 - If the total number of blocks is odd, there is a leftover 'b' without
   a corresponding 'a', which is also counted as pending.
*/
int PmergeMe::getNumPending(int numBlocks)
{
	int	numPending = (numBlocks / 2); // each pair contributes one 'b'
	if (numBlocks % 2 != 0)	// if odd number of blocks, leftover 'b' also pending
		++numPending;

	return numPending;
}

/**
Checks whether an element belongs to the main chain in the interleaved block layout.

Layout: [b1][a1][b2][a2]...<leftover>;
-> Main chain -> all 'a' blocks

 @param index		Index of the element in the sequence
 @param blockSize	Number of elements per block.
 @param totalSize	Total number of elements in the sequence
 @return			`true` if the element is in the main chain, `false` if not (pending/leftover)
*/
bool	PmergeMe::isMainChain(int index, int blockSize, int totalSize)
{
	int	blockNum = index / blockSize;  // which block does this element belong to

	// leftover -> not main chain
	if ((blockNum + 1) * blockSize > totalSize)
		return false;

	// main chain: odd-numbered blocks (a-blocks)
	if (blockNum % 2 == 1)
		return true;

	return false;	// all b-blocks and leftover
}

/**
Computes the number of main chain blocks that should be considered when inserting
a pending block.

The number of blocks in the main chain to be used during insertion is defined by
the insertion group `k` (`2^k - 1`), and cannot exceed the number of blocks currently
available in the main chain.

 @param k			Insertion group index, also the number of comparisons allowed
 @param posPending	Number of elements currently in main chain
 @param blockSize	Size of each block
 @return			Number of "useful" blocks to consider in binary search
*/
size_t	PmergeMe::computeUsefulMainEnd(int k, size_t posPending, size_t blockSize)
{
	size_t	usefulEnd = (1u << k) - 1; // 2^k - 1 blocks allowed according to FJ
	size_t	availableBlocks = posPending / blockSize; // blocks in main chain

	if (usefulEnd > availableBlocks)
		usefulEnd = availableBlocks;

	return usefulEnd;
}

////////////
// FORD-JOHNSON UTILITY FUNCTIONS //
////////////

/**
Generates the Jacobsthal sequence needed for insertion order
in the Ford–Johnson merge-insertion algorithm.
-> `[0, 1, 3, 5, 11, 21, ...]`

The Jacobsthal sequence is defined as:
J(0) = 0, J(1) = 1; J(n) = J(n-1) + 2 * J(n-2) for n > 1

It's probably an overkill to have the sequence dynamically computed,
just hardcoding the first few numbers would do the trick, but this is more fun ;)

 @param numPending	Number of pending elements
 @return			Vector with Jacobsthal sequence (no duplicates)
*/
std::vector<int>	PmergeMe::buildJacobsthalSeq(int numPending)
{
	std::vector<int>	seq;
	if (numPending <= 0)
		return seq;

	int	j1 = 0, j2 = 1; // first and second Jacobsthal numbers
	if (j1 < numPending) seq.push_back(j1); // adds 0
	if (j2 < numPending) seq.push_back(j2); // adds 1

	while (true)
	{
		int	jNext = j2 + 2 * j1; // J(n) = J(n-1) + 2*J(n-2)
		seq.push_back(jNext);
		if (jNext >= numPending)
			break;
		j1 = j2;
		j2 = jNext;
	}

	if (seq.size() > 2) // Remove duplicate '1'
	{
		std::vector<int>::iterator	it = seq.begin();
		++it;
		seq.erase(it);
	}
	return seq;
}

/**
Builds the optimal insertion order of pending elements for the
Ford–Johnson merge-insertion algorithm, based on a precomputed
Jacobsthal sequence.

The procedure works as follows:
 1.	Collect Jacobsthal numbers > 0 and <= numPending.
 2.	For each consecutive pair of Jacobsthal numbers, first add
	the larger one, then fill the gap below it in descending order.
 3.	Finally, append any remaining indices (those greater than the
 	last Jacobsthal number) in descending order.

Example: numPending = 9
	Jacobsthal sequence (cap included): [0, 1, 3, 5, 11]
	Step 1: [1, 3, 5]
 	Step 2: [1, 3, 2, 5, 4]
 	Step 3: [1, 3, 2, 5, 4, 9, 8, 7, 6]

The resulting vector represents the insertion order of the n-th
pending elements (b1, b3, b2, ...).

 @param numPending	Number of pending elements
 @param jacSeq		Precomputed Jacobsthal sequence (no duplicates)
 @return			Vector with optimal insertion order (n-th pending element)
*/
std::vector<int>	PmergeMe::buildInsertOrder(int numPending, const std::vector<int>& jacSeq)
{
	std::vector<int>	order;
	if (numPending <= 0 || jacSeq.empty())
		return order;

	// Step 1: collect JT numbers > 0
	for (std::vector<int>::const_iterator it = jacSeq.begin(); it != jacSeq.end(); ++it)
	{
		int	j = *it;
		if (j > 0 && j <= numPending)
			order.push_back(j);
	}

	// Step 2: fill gaps between JT numbers in a descending way
	std::vector<int>	expanded;
	int					prev = 0;

	for (std::vector<int>::const_iterator it = order.begin(); it != order.end(); ++it)
	{
		int	j = *it;
		expanded.push_back(j);
		for (int k = j - 1; k > prev; --k) // fill gaps between JT numbers
			expanded.push_back(k);
		prev = j;
	}

	// Step 3: Append any remaining pending elements in descending order
	for (int i = numPending; i > 1; --i)
	{
		if (std::find(expanded.begin(), expanded.end(), i) == expanded.end())
			expanded.push_back(i);
	}

	return expanded;
}

// Counts how many elements in the insertion order are smaller than the pending element at pendIdx
size_t	PmergeMe::countSmallerPending(const std::vector<int>& insertionOrder, std::vector<int>::const_iterator endIt, int pendIdx)
{
	size_t	count = 0;

	for (std::vector<int>::const_iterator it = insertionOrder.begin(); it != endIt; ++it)
	{
		if (*it < pendIdx)
			++count;
	}

	return count;
}

/**
Computes the "insertion group" `k` for a given pending element index.
This value is equivalent to the number of comparisons allowed by Ford-Johnson
for this element during the insertion process.

The maximum number of main chain elements that may need to be compared when
inserting this element is `(2^k) - 1`; this also defines the "useful main chain"
considered for binary search.

 @param pendIdx	1-based index of the pending element (starting from b1)
 @param jacSeq	Jacobsthal sequence (no duplicates, starting with 0: 0, 1, 3, 5, ...)
 @return		1-based index of the insertion group
*/
int	PmergeMe::computeK(int pendIdx, const std::vector<int>& jacSeq)
{
	std::vector<int>::const_iterator	it = jacSeq.begin();
	int									i = 0;

	for (; it != jacSeq.end(); ++it, ++i)
		if (pendIdx <= *it)
			return i;

	// fallback (should not happen)
	return i;
}

////////////
// HELPER //
////////////

// Builds a vector from command line arguments
static std::vector<int>	buildVecFromArgs(int argc, char** argv)
{
	std::vector<int>	vec;
	vec.reserve(argc - 1); // helps with performance, avoids repeated reallocation in growing vec

	for (int i = 1; i < argc; ++i) // fill vector
		vec.push_back(std::atoi(argv[i]));

	return vec;
}
