#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <vector>
# include <sys/time.h>	// timeval

// utils.cpp

timeval		getCurrentTimeStruct();
void		printElapsedTime(const timeval& start, const timeval& end, int elements,
				const std::string& contName);
void		printNumComp(int numComp, int maxComp, const std::string& contName);
std::string	toString(int n);
int			maxComparisonsFJ(int n);
std::string	formatWithCommas(long value);

// utils.tpp

void		printIsSorted(const std::vector<int>& c, const std::string& contName);
void		printContainer(const std::vector<int>& c);
std::string	returnContainerDebug(const std::vector<int>& c, const std::string& prefix);
void		printBeforeAfter(char** argv, const std::vector<int>& afterSort);

#endif
