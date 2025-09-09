#ifndef DEBUG_TPP
# define DEBUG_TPP

# include "utils.hpp"	// toString(), returnContainerDebug()
# include <iostream>	// std::cout, std::endl

// compile with 'make debug' to print debug output
// 'do/while(0)' ensures the macro behaves like a single statement
# ifdef DEBUG
#  define DEBUG_PRINT(x) do { std::cout << x << std::endl; } while (0)
# else
#  define DEBUG_PRINT(x) do {} while (0)
# endif

// Debug information for the main chain after sorting
template <typename Container>
void	debugMainChainSorted(const Container& c, int recDepth, int blockSize, int numBlocks, int numPending)
{
	(void)c; (void)recDepth; (void)blockSize; (void)numBlocks; (void)numPending;

	DEBUG_PRINT("\nRecDepth: " + toString(recDepth) + ", BlockSize: " + toString(blockSize) + ", NumBlocks: "
				+ toString(numBlocks) + ", NumPending: " + toString(numPending));
	DEBUG_PRINT(returnContainerDebug(c, "container: "));
	
	//! ADD: Show container with block groupings
	std::string containerGrouped = "container:    ";
	for (int i = 0; i < static_cast<int>(c.size()); ++i) {
		if (i % blockSize == 0) containerGrouped += "[";
		if (i > 0 && i % blockSize == 0) containerGrouped += " ";
		containerGrouped += toString(c[i]);
		if ((i + 1) % blockSize == 0) containerGrouped += "]";
		else containerGrouped += " ";
	}
	DEBUG_PRINT(containerGrouped);
}

// Debug information for the rearranged vector and insertion order
template <typename Container>
void	debugVecRearranged(const Container& c, const Container& insertionOrder, int posPending, int blockSize)
{
	(void)c; (void)insertionOrder; (void)posPending; (void)blockSize;

	DEBUG_PRINT(returnContainerDebug(c, "Rearranged [main|pend|(left)]: "));
	
	//! ADD: Show main chain section with block groupings
	std::string mainChain = "Main chain:   ";
	for (int i = 0; i < posPending; ++i) {
		if (i % blockSize == 0) mainChain += "[";
		if (i > 0 && i % blockSize == 0) mainChain += " ";
		mainChain += toString(c[i]);
		if ((i + 1) % blockSize == 0) mainChain += "]";
		else mainChain += " ";
	}
	DEBUG_PRINT(mainChain);
	
	//! ADD: Show pending section with block groupings
	std::string pending = "Pending:      ";
	int pendingStart = posPending;
	int pendingEnd = static_cast<int>(c.size()) - 1;
	for (int i = pendingStart; i < pendingEnd; ++i) {
		if ((i - pendingStart) % blockSize == 0) pending += "[";
		if (i > pendingStart && (i - pendingStart) % blockSize == 0) pending += " ";
		pending += toString(c[i]);
		if ((i - pendingStart + 1) % blockSize == 0) pending += "]";
		else pending += " ";
	}
	DEBUG_PRINT(pending);
	
	//! ADD: Show leftover section
	std::string leftover = "Leftover:     [";
	if (c.size() > 0) {
		leftover += toString(c[c.size() - 1]);
	}
	leftover += "]";
	DEBUG_PRINT(leftover);
	
	DEBUG_PRINT("posPending: " + toString(posPending));
	DEBUG_PRINT(returnContainerDebug(insertionOrder, "Insertion order: "));
	DEBUG_PRINT("....");
}

// Debug information before inserting a pending block
template <typename Container>
void	debugPreInsert(const Container& c, int pendIdx, int end, int k, int numMainBlocks, int numComp)
{
	(void)c; (void)pendIdx; (void)end; (void)k; (void)numMainBlocks; (void)numComp;

	// Get the element at "end-1" position generically
	typename Container::const_iterator	it = c.begin();
	std::advance(it, end - 1);

	DEBUG_PRINT("looking at b" + toString(pendIdx) + ", value: " + toString(*it));
	DEBUG_PRINT("insertion group k: " + toString(k));
	DEBUG_PRINT("last useful main chain block: " + toString(numMainBlocks));
	DEBUG_PRINT("num of comps BEFORE insert: " + toString(numComp));
}

// Debug information after inserting a pending block
template <typename Container>
void	debugPostInsert(const Container& c, int pendIdx, int insertPos, int numComp)
{
	(void)c; (void)pendIdx; (void)insertPos; (void)numComp;

	DEBUG_PRINT("number of comps AFTER insert: " + toString(numComp));
	DEBUG_PRINT("Inserting at position: " + toString(insertPos));
	DEBUG_PRINT(returnContainerDebug(c, "container after b" + toString(pendIdx)
		+ " is moved to main: "));
	DEBUG_PRINT("----");
}

#endif
