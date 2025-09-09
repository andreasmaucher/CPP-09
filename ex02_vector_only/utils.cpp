#include "utils.hpp"
#include "define.hpp"	// for color codes, WIDTH_N, WIDTH_C

#include <iostream>
#include <iomanip>		// setw()
#include <sstream>		// for std::ostringstream
#include <cmath>		// ceil(), log2()
#include <vector>

// Returns `timeval` struct representing the current time.
timeval	getCurrentTimeStruct()
{
	struct timeval	tv;

	gettimeofday(&tv, 0);
	return (tv);
}

// Returns the elapsed time in microseconds between two `timeval` structs.
static long	getElapsedTime(const timeval& start, const timeval& end)
{
	long	sec = end.tv_sec - start.tv_sec;
	long	usec = end.tv_usec - start.tv_usec;

	return (sec * 1000000 + usec); // sec * 1000 = msec * 1000 = usec
}

// Prints a formatted elapsed time message.
void	printElapsedTime(const timeval& start, const timeval& end, int elements,
			const std::string& contName)
{
	long	elapsed = getElapsedTime(start, end);

	std::cout	<< "Time to process " << YELLOW << std::right << std::setw(WIDTH_N)
				<< formatWithCommas(elements) << " numbers" << RESET << " with "
				<< YELLOW << std::left << std::setw(WIDTH_C) << contName << RESET << ": "
				<< YELLOW << formatWithCommas(elapsed) << " us" << RESET << std::endl;
}

// Prints the number of comparisons made and checks if this is within the limit
// of comparisons in a Ford-Johnson implementation
void	printNumComp(int numComp, int maxComp, const std::string& contName)
{
	std::cout	<< "Number of comparisons (" << YELLOW
				<< std::left << std::setw(WIDTH_C) << contName << RESET
				<< "): " << YELLOW << formatWithCommas(numComp) << RESET
				<< " / " << formatWithCommas(maxComp);

	if (numComp <= maxComp)
		std::cout << GREEN << " OK" << RESET << std::endl;
	else
		std::cout << RED << " NOT OK" << RESET << std::endl;
}

// Converts a `int` to a string.
// Used for debugging.
std::string	toString(int n)
{
	std::ostringstream	oss;
	
	oss << n;
	return oss.str();
}

// Gives the maximum number of comparisons with the Ford-Johnson algorithm to
// sort a sequence of `n` integers.
// See p. 186, The Art of Computer Programming (vol. 3, 1998), Donald E. Knuth
int	maxComparisonsFJ(int n)
{
	int	sum = 0;

	for (int k = 1; k <= n; ++k)
	{
		double	value = (3.0 / 4.0) * k;
		sum += static_cast<int>(ceil(log2(value)));
	}

	return sum;
}

// Formats a long integer with commas as thousands separators.
std::string	formatWithCommas(long value)
{
	std::string	num = toString(value);
	int			insertPosition = static_cast<int>(num.length()) - 3;
	
	while (insertPosition > 0)
	{
		num.insert(insertPosition, ",");
		insertPosition -= 3;
	}
	return num;
}

// Prints if the vector is sorted in a non-decreasing order or not.
void	printIsSorted(const std::vector<int>& c, const std::string& contName)
{
	std::vector<int>::const_iterator	it = c.begin();
	std::vector<int>::const_iterator	next = it;
	++next;

	std::cout	<< "Container " << YELLOW << std::left << std::setw(WIDTH_C)
				<< contName << RESET << " is ";

	while (next != c.end())
	{
		if (*it > *next) // Pair is not sorted
		{
			std::cout << RED << "NOT SORTED" << RESET << std::endl;
			return;
		}
		++it;
		++next;
	}
	std::cout << GREEN << "SORTED" << RESET << std::endl; // All elements sorted
}

// Prints the content of a vector.
void	printContainer(const std::vector<int>& c)
{
	std::vector<int>::const_iterator	it = c.begin();

	while (it != c.end())
	{
		std::cout << *it;
		++it;
		if (it != c.end())
			std::cout << " ";
	}
	std::cout << std::endl;
}

// Returns the content of a vector with formatting as a string.
// Great for debugging!
std::string	returnContainerDebug(const std::vector<int>& c, const std::string& prefix)
{
	std::stringstream	ss;
	ss << prefix << GREEN;
	for (std::vector<int>::const_iterator it = c.begin(); it != c.end(); ++it)
		ss << *it << " ";
	ss << RESET;
	return ss.str();
}

// Prints out numbers before sorting (`argv`) and after sorting (sorted vector).
void	printBeforeAfter(char** argv, const std::vector<int>& afterSort)
{
	size_t	n = afterSort.size();

	std::cout << "Before: ";
	for (size_t i = 0; i < n; ++i)
	{
		std::cout << argv[i + 1] << " ";
	}
	
	std::cout << "\nAfter:  ";
	printContainer(afterSort);
	std::cout << std::endl;
}
