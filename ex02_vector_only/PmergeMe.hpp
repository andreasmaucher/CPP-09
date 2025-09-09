#ifndef PMERGEME_HPP
# define PMERGEME_HPP

# include <vector>
# include <cstddef>	// size_t

class	PmergeMe
{
	public:
		// Vector-specific methods
		static std::vector<int>	sortVec(int argc, char** argv, int& numComp);
		static int				sortPairsRecursivelyVec(std::vector<int>& vec, int& numComp, int recDepth);
		static int				rearrangeVec(std::vector<int>& vec, int blockSize);
		static void				insertPendingBlocksVec(std::vector<int>& vec, int blockSize, int numPending,
									const std::vector<int>& jacSeq, int& numComp);
		static size_t			binaryInsertBlockVec(const std::vector<int>& vec, int value,
									size_t blockSize, size_t numBlocks, int& numComp);

		// Utility methods
		static void				checkArgs(int argc, char** argv);
		static int				getNumPending(int numBlocks);
		static bool				isMainChain(int index, int blockSize, int totalSize);
		static size_t			computeUsefulMainEnd(int k, size_t posPending, size_t blockSize);

		// Utility methods for Ford-Johnson algorithm
		static std::vector<int>	buildJacobsthalSeq(int numPending);
		static std::vector<int>	buildInsertOrder(int numPending, const std::vector<int>& jacSeq);
		static size_t			countSmallerPending(const std::vector<int>& insertionOrder,
									std::vector<int>::const_iterator endIt, int pendIdx);
		static int				computeK(int pendIdx, const std::vector<int>& jacSeq);

	private:
		// Private as to prevent instantiation --> PmergeMe is pure utility / static class
		PmergeMe();
		PmergeMe(const PmergeMe& other);
		PmergeMe& operator=(const PmergeMe& other);
		~PmergeMe();
};


#endif
