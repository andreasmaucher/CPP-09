#include "BitcoinExchange.hpp"

int main(int ac, char **av)
{
    if (ac != 2) 
    {
        std::cout << "Error: could not open file. Expected input: <./btc file_to_parse>" << std::endl;
        return 1;
    }

    try
    {
        // load the reference database that is the same for all inputs
        BitcoinExchange exchange("data.csv");
        // process user input file
        exchange.processInputFile(av[1]);
    } 
    catch (const std::exception& e) 
    {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
