#ifndef BITCOINEXCHANGE_HPP
#define BITCOINEXCHANGE_HPP

#include <map> // to store the btc exchange rates
#include <string>
#include <fstream> // for reading the data.csv file
#include <iostream>
#include <sstream> // for parsing the csv file (like splitting data by comma)

class BitcoinExchange 
{
    private:
        std::map<std::string, float> database;
        
        bool isValidDate(const std::string& date) const;
        bool isValidValue(const float value) const;
        std::string findClosestDate(const std::string& date) const;
        void loadDatabase(const std::string& filename);
        std::pair<bool, std::string> parseDate(const std::string& date) const;

    public:
        BitcoinExchange(const std::string& dbFile);
        ~BitcoinExchange();

        void processInputFile(const std::string& inputFile);
};

#endif
