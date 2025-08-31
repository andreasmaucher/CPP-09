#include "RPN.hpp"

int main(int ac, char **av)
{
    if (ac != 2) 
    {
        std::cerr << "Error: Expected exactly one argument" << std::endl;
        std::cerr << "Usage: ./RPN \"expression\"" << std::endl;
        std::cerr << "Example: ./RPN \"8 9 * 9 - 9 - 9 - 4 - 1 +\"" << std::endl;
        return 1;
    }
    try
    {
        // create RPN calculator and run the calculation
        RPN calculator;
        int result = calculator.calculate(av[1]);
        // output the result to standard output
        std::cout << result << std::endl;
    } 
    catch (const std::exception& e) 
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
