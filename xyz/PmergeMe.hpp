#ifndef PMERGEME_HPP
#define PMERGEME_HPP
#include <algorithm>
#include <deque>
#include <vector>
#include <iostream>
class PmergeMe
{
  public:
    PmergeMe();
    PmergeMe(const PmergeMe& pm);
    PmergeMe& operator=(const PmergeMe& pm);
    ~PmergeMe();

    void sort_vec(std::vector<int>& vec);
    void sort_deque(std::deque<int>& deque);

	static int nbr_of_comps;

  private:
    template <typename T> void _merge_insertion_sort(T& container, int pair_level);

    template <typename T> void _swap_pair(T it, int pair_level);
};

long _jacobsthal_number(long n);

template <typename T> bool _comp(T lv, T rv) {
	PmergeMe::nbr_of_comps++;
	return *lv < *rv;
}

template <typename T> T next(T it, int steps)
{
    std::advance(it, steps);
    return it;
}

template <typename T> void PmergeMe::_swap_pair(T it, int pair_level)
{
    T start = next(it, -pair_level + 1);
    T end = next(start, pair_level);
    while (start != end)
    {
        std::iter_swap(start, next(start, pair_level));
        start++;
    }
}

template <typename T> void PmergeMe::_merge_insertion_sort(T& container, int pair_level)
{
    typedef typename T::iterator Iterator;

    int pair_units_nbr = container.size() / pair_level;
    
    // === DEBUG OUTPUT ADDED BY ASSISTANT ===
    // Print container state before processing with pair groupings
    std::cout << "\n=== RECURSION LEVEL " << pair_level << " ===" << std::endl;
    std::cout << "Container before processing: ";
    
    // Show pairs with brackets
    typename T::iterator it = container.begin();
    for (int i = 0; i < pair_units_nbr; ++i) {
        std::cout << "(";
        for (int j = 0; j < pair_level && it != container.end(); ++j) {
            std::cout << *it;
            if (j < pair_level - 1) std::cout << " ";
            ++it;
        }
        std::cout << ") ";
    }
    
    // Show remaining unpaired elements
    if (it != container.end()) {
        std::cout << "| ";
        while (it != container.end()) {
            std::cout << *it << " ";
            ++it;
        }
    }
    
    std::cout << std::endl;
    std::cout << "Pair units number: " << pair_units_nbr << std::endl;
    // === END DEBUG OUTPUT ADDED BY ASSISTANT ===
    
    if (pair_units_nbr < 2) {
        // === DEBUG OUTPUT ADDED BY ASSISTANT ===
        std::cout << "Base case reached (less than 2 pair units)" << std::endl;
        // === END DEBUG OUTPUT ADDED BY ASSISTANT ===
        return;
    }

    /* If there is an odd pair, we ignore it during swapping.
       It will go to the pend as the last pair. */
    bool is_odd = pair_units_nbr % 2 == 1;

    /* It's important to caluclate the end position until which we should iterate.
       We can have a set of values like:
       ((1 2) (3 4)) ((3 8) (2 6)) | 0
	   where the smallest possible element is a pair of two numbers, like (1 2) or (3 4). Those elements form pairs.
       However, there could be numbers (0 in this case) which can't even form a single element necessary to form a pair.
	   It's not even enough to be an odd element.
       Those values should be ignored. */
    Iterator start = container.begin();
    Iterator last = next(container.begin(), pair_level * (pair_units_nbr));
    Iterator end = next(last, -(is_odd * pair_level));

    /* Swap pairs of numbers, pairs, pairs of pairs etc by the biggest pair
       number. After each swap we recurse. */
    int jump = 2 * pair_level;
    for (Iterator it = start; it != end; std::advance(it, jump))
    {
        Iterator this_pair = next(it, pair_level - 1);
        Iterator next_pair = next(it, pair_level * 2 - 1);
        if (_comp(next_pair, this_pair))
        {
            _swap_pair(this_pair, pair_level);
        }
    }
    
    // === DEBUG OUTPUT ADDED BY ASSISTANT ===
    std::cout << "After pair swapping, before recursion: ";
    
    // Show pairs with brackets after swapping
    typename T::iterator it_swap = container.begin();
    for (int i = 0; i < pair_units_nbr; ++i) {
        std::cout << "(";
        for (int j = 0; j < pair_level && it_swap != container.end(); ++j) {
            std::cout << *it_swap;
            if (j < pair_level - 1) std::cout << " ";
            ++it_swap;
        }
        std::cout << ") ";
    }
    
    // Show remaining unpaired elements
    if (it_swap != container.end()) {
        std::cout << "| ";
        while (it_swap != container.end()) {
            std::cout << *it_swap << " ";
            ++it_swap;
        }
    }
    
    std::cout << std::endl;
    // === END DEBUG OUTPUT ADDED BY ASSISTANT ===
    
    _merge_insertion_sort(container, pair_level * 2);
    
    // === DEBUG OUTPUT ADDED BY ASSISTANT ===
    std::cout << "After recursion (pair_level " << pair_level << "): ";
    
    // Show pairs with brackets after recursion
    typename T::iterator it_rec = container.begin();
    for (int i = 0; i < pair_units_nbr; ++i) {
        std::cout << "(";
        for (int j = 0; j < pair_level && it_rec != container.end(); ++j) {
            std::cout << *it_rec;
            if (j < pair_level - 1) std::cout << " ";
            ++it_rec;
        }
        std::cout << ") ";
    }
    
    // Show remaining unpaired elements
    if (it_rec != container.end()) {
        std::cout << "| ";
        while (it_rec != container.end()) {
            std::cout << *it_rec << " ";
            ++it_rec;
        }
    }
    
    std::cout << std::endl;
    // === END DEBUG OUTPUT ADDED BY ASSISTANT ===

    /* Main contains an already sorted sequence.
       Pend stores yet to be sorted numbers.
       They contain iterators instead of the numbers themselves because
       iterators + pair_level contain all the information about any stored
       range of numbers. */
    std::vector<Iterator> main;
    std::vector<Iterator> pend;

    /* Initialize the main chain with the {b1, a1}.
	   Remember that we already know that b1 is going to be the smallest element in the main chain. */
    main.insert(main.end(), next(container.begin(), pair_level - 1));
    main.insert(main.end(), next(container.begin(), pair_level * 2 - 1));

    /* Insert the rest of a's into the main chain.
       Insert the rest of b's into the pend. */
    for (int i = 4; i <= pair_units_nbr; i += 2)
    {
        pend.insert(pend.end(), next(container.begin(), pair_level * (i - 1) - 1));
        main.insert(main.end(), next(container.begin(), pair_level * i - 1));
    }

    /* Insert an odd element to the pend, if there are any. */
    if (is_odd)
    {
        pend.insert(pend.end(), next(end, pair_level - 1));
    }
    
    // === DEBUG OUTPUT ADDED BY ASSISTANT ===
    std::cout << "Main chain (iterators): ";
    for (size_t i = 0; i < main.size(); ++i) {
        std::cout << "[" << i << "]=" << *main[i] << " ";
    }
    std::cout << std::endl;
    
    std::cout << "Pend chain (iterators): ";
    for (size_t i = 0; i < pend.size(); ++i) {
        std::cout << "[" << i << "]=" << *pend[i] << " ";
    }
    std::cout << std::endl;
    // === END DEBUG OUTPUT ADDED BY ASSISTANT ===

    /* Insert the pend into the main in the order determined by the
       Jacobsthal numbers. For example: 3 2 -> 5 4 -> 11 10 9 8 7 6 -> etc.
       During insertion, elements from the main chain serve as an upper bound for inserting elements,
       in order to save number of comparisons, as we know already that, for example,
       b5 is lesser than a5, we binary search only until a5, not until the end of the container.

	   We can calculate the index of the bound element. With the way I do it,
	   the index of the bound is inserted_numbers + current_jacobsthal_number. */
    int prev_jacobsthal = _jacobsthal_number(1);
    int inserted_numbers = 0;
    
    // === DEBUG OUTPUT ADDED BY ASSISTANT ===
    std::cout << "\n--- JACOBSTHAL INSERTION PROCESS ---" << std::endl;
    std::cout << "Starting Jacobsthal insertion with prev_jacobsthal=" << prev_jacobsthal << std::endl;
    // === END DEBUG OUTPUT ADDED BY ASSISTANT ===
    
    for (int k = 2;; k++)
    {
        int curr_jacobsthal = _jacobsthal_number(k);
        int jacobsthal_diff = curr_jacobsthal - prev_jacobsthal;
		int offset = 0;
        
        // === DEBUG OUTPUT ADDED BY ASSISTANT ===
        std::cout << "\nJacobsthal iteration k=" << k << ": curr_jacobsthal=" << curr_jacobsthal 
                  << ", diff=" << jacobsthal_diff << ", pend.size()=" << pend.size() << std::endl;
        // === END DEBUG OUTPUT ADDED BY ASSISTANT ===
        
        if (jacobsthal_diff > static_cast<int>(pend.size()))
            break;
        int nbr_of_times = jacobsthal_diff;
        typename std::vector<Iterator>::iterator pend_it = next(pend.begin(), jacobsthal_diff - 1);
        typename std::vector<Iterator>::iterator bound_it =
            next(main.begin(), curr_jacobsthal + inserted_numbers);
            
        // === DEBUG OUTPUT ADDED BY ASSISTANT ===
        std::cout << "Inserting " << nbr_of_times << " elements from pend, starting at index " 
                  << (jacobsthal_diff - 1) << std::endl;
        std::cout << "Bound iterator points to main[" << (curr_jacobsthal + inserted_numbers) << "]" << std::endl;
        // === END DEBUG OUTPUT ADDED BY ASSISTANT ===
        
        while (nbr_of_times)
        {
            // === DEBUG OUTPUT ADDED BY ASSISTANT ===
            std::cout << "  Step " << (jacobsthal_diff - nbr_of_times + 1) << ": Inserting pend element " 
                      << **pend_it << " into main chain" << std::endl;
            std::cout << "    Main before insertion: ";
            for (size_t i = 0; i < main.size(); ++i) {
                std::cout << "[" << i << "]=" << *main[i] << " ";
            }
            std::cout << std::endl;
            std::cout << "    Pend before insertion: ";
            for (size_t i = 0; i < pend.size(); ++i) {
                std::cout << "[" << i << "]=" << *pend[i] << " ";
            }
            std::cout << std::endl;
            // === END DEBUG OUTPUT ADDED BY ASSISTANT ===
            
            typename std::vector<Iterator>::iterator idx =
                std::upper_bound(main.begin(), bound_it, *pend_it, _comp<Iterator>);
            typename std::vector<Iterator>::iterator inserted = main.insert(idx, *pend_it);
            
            // === DEBUG OUTPUT ADDED BY ASSISTANT ===
            std::cout << "    Binary search found insertion position at main[" << (inserted - main.begin()) << "]" << std::endl;
            std::cout << "    Main after insertion: ";
            for (size_t i = 0; i < main.size(); ++i) {
                std::cout << "[" << i << "]=" << *main[i] << " ";
            }
            std::cout << std::endl;
            // === END DEBUG OUTPUT ADDED BY ASSISTANT ===
            
            nbr_of_times--;
            pend_it = pend.erase(pend_it);
            std::advance(pend_it, -1);
            /* Sometimes the inserted number in inserted at the exact index of where the bound should be.
			   When this happens, it eclipses the bound of the next pend, and it does more comparisons
			   than it should. We need to offset when this happens. */
            offset += (inserted - main.begin()) == curr_jacobsthal + inserted_numbers;
			bound_it = next(main.begin(), curr_jacobsthal + inserted_numbers - offset);
        }
        prev_jacobsthal = curr_jacobsthal;
        inserted_numbers += jacobsthal_diff;
		offset = 0;
        
        // === DEBUG OUTPUT ADDED BY ASSISTANT ===
        std::cout << "Completed Jacobsthal iteration k=" << k << ", inserted_numbers now=" << inserted_numbers << std::endl;
        // === END DEBUG OUTPUT ADDED BY ASSISTANT ===
    }

    /* Insert the remaining elements in the reversed order. Here we also want to
       perform as less comparisons as possible, so we calculate the starting bound
       to insert pend number to be the pair of the first pend number. If the first
       pend number is b8, the bound is a8, if the pend number is b7, the bound is a7 etc.
       With the way I do it the index of bound is
       size_of_main - size_of_pend + index_of_current_pend. */
    
    // === DEBUG OUTPUT ADDED BY ASSISTANT ===
    std::cout << "\n--- FINAL INSERTION OF REMAINING ELEMENTS ---" << std::endl;
    std::cout << "Inserting " << pend.size() << " remaining elements in reverse order" << std::endl;
    // === END DEBUG OUTPUT ADDED BY ASSISTANT ===
    
    for (ssize_t i = pend.size() - 1; i >= 0; i--)
    {
        typename std::vector<Iterator>::iterator curr_pend = next(pend.begin(), i);
        typename std::vector<Iterator>::iterator curr_bound =
            next(main.begin(), main.size() - pend.size() + i + is_odd);
            
        // === DEBUG OUTPUT ADDED BY ASSISTANT ===
        std::cout << "  Final step " << (pend.size() - i) << ": Inserting remaining element " 
                  << **curr_pend << " (index " << i << ")" << std::endl;
        std::cout << "    Bound calculated at main[" << (main.size() - pend.size() + i + is_odd) << "]" << std::endl;
        std::cout << "    Main before insertion: ";
        for (size_t j = 0; j < main.size(); ++j) {
            std::cout << "[" << j << "]=" << *main[j] << " ";
        }
        std::cout << std::endl;
        // === END DEBUG OUTPUT ADDED BY ASSISTANT ===
        
        typename std::vector<Iterator>::iterator idx =
            std::upper_bound(main.begin(), curr_bound, *curr_pend, _comp<Iterator>);
        main.insert(idx, *curr_pend);
        
        // === DEBUG OUTPUT ADDED BY ASSISTANT ===
        std::cout << "    Main after insertion: ";
        for (size_t j = 0; j < main.size(); ++j) {
            std::cout << "[" << j << "]=" << *main[j] << " ";
        }
        std::cout << std::endl;
        // === END DEBUG OUTPUT ADDED BY ASSISTANT ===
    }

    /* Use copy vector to store all the numbers, in order not to overwrite the
       original iterators. */
    std::vector<int> copy;
    copy.reserve(container.size());
    for (typename std::vector<Iterator>::iterator it = main.begin(); it != main.end(); it++)
    {
        for (int i = 0; i < pair_level; i++)
        {
            Iterator pair_start = *it;
            std::advance(pair_start, -pair_level + i + 1);
            copy.insert(copy.end(), *pair_start);
        }
    }

    /* Replace values in the original container. */
    Iterator container_it = container.begin();
    std::vector<int>::iterator copy_it = copy.begin();
    while (copy_it != copy.end())
    {
        *container_it = *copy_it;
        container_it++;
        copy_it++;
    }
    
    // === DEBUG OUTPUT ADDED BY ASSISTANT ===
    std::cout << "FINAL RESULT for recursion level " << pair_level << ": ";
    
    // Show final result with brackets
    typename T::iterator it_final = container.begin();
    for (int i = 0; i < pair_units_nbr; ++i) {
        std::cout << "(";
        for (int j = 0; j < pair_level && it_final != container.end(); ++j) {
            std::cout << *it_final;
            if (j < pair_level - 1) std::cout << " ";
            ++it_final;
        }
        std::cout << ") ";
    }
    
    // Show remaining unpaired elements
    if (it_final != container.end()) {
        std::cout << "| ";
        while (it_final != container.end()) {
            std::cout << *it_final << " ";
            ++it_final;
        }
    }
    
    std::cout << std::endl;
    std::cout << "=== END RECURSION LEVEL " << pair_level << " ===" << std::endl;
    // === END DEBUG OUTPUT ADDED BY ASSISTANT ===
}
#endif