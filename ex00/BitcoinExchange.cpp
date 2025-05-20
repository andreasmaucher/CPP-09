#include "BitcoinExchange.hpp"
#include <cstdlib>

BitcoinExchange::BitcoinExchange(const std::string& dbFile) {
    loadDatabase(dbFile);
}

BitcoinExchange::~BitcoinExchange() {}

void BitcoinExchange::loadDatabase(const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        throw std::runtime_error("Error: could not open file.");
    }

    std::string line;
    std::getline(file, line); // Skip header line

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string date;
        std::string value;
        
        if (std::getline(ss, date, ',') && std::getline(ss, value)) {
            try {
                float rate = static_cast<float>(std::atof(value.c_str()));
                database[date] = rate;
            } catch (const std::exception& e) {
                std::cerr << "Warning: Invalid rate in database for date: " << date << std::endl;
            }
        }
    }
}

std::pair<bool, std::string> BitcoinExchange::parseDate(const std::string& date) const {
    if (date.length() != 10) return std::make_pair(false, "Invalid date format");
    
    try {
        int year = std::atoi(date.substr(0, 4).c_str());
        int month = std::atoi(date.substr(5, 2).c_str());
        int day = std::atoi(date.substr(8, 2).c_str());

        if (date[4] != '-' || date[7] != '-') return std::make_pair(false, "Invalid date format");
        if (month < 1 || month > 12) return std::make_pair(false, "Invalid month");
        if (day < 1 || day > 31) return std::make_pair(false, "Invalid day");
        if (year < 2009) return std::make_pair(false, "Date before Bitcoin's creation");

        // Basic month length validation
        if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30)
            return std::make_pair(false, "Invalid day for month");
        if (month == 2) {
            bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
            if ((isLeap && day > 29) || (!isLeap && day > 28))
                return std::make_pair(false, "Invalid day for February");
        }

        return std::make_pair(true, "");
    } catch (const std::exception& e) {
        return std::make_pair(false, "Invalid date format");
    }
}

bool BitcoinExchange::isValidDate(const std::string& date) const {
    return parseDate(date).first;
}

bool BitcoinExchange::isValidValue(const float value) const {
    return value >= 0 && value <= 1000;
}

std::string BitcoinExchange::findClosestDate(const std::string& date) const {
    std::map<std::string, float>::const_iterator it = database.upper_bound(date);
    if (it == database.begin()) {
        return it->first;
    }
    --it;
    return it->first;
}

void BitcoinExchange::processInputFile(const std::string& inputFile) {
    std::ifstream file(inputFile.c_str());
    if (!file.is_open()) {
        std::cout << "Error: could not open file." << std::endl;
        return;
    }

    std::string line;
    std::getline(file, line); // Skip header line

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string date;
        std::string separator;
        std::string valueStr;

        if (std::getline(ss, date, '|') && std::getline(ss, valueStr)) {
            // Trim whitespace
            date.erase(0, date.find_first_not_of(" \t"));
            date.erase(date.find_last_not_of(" \t") + 1);
            valueStr.erase(0, valueStr.find_first_not_of(" \t"));
            valueStr.erase(valueStr.find_last_not_of(" \t") + 1);

            // Validate date
            std::pair<bool, std::string> dateValidation = parseDate(date);
            if (!dateValidation.first) {
                std::cout << "Error: bad input => " << date << std::endl;
                continue;
            }

            // Validate value
            float value;
            try {
                value = static_cast<float>(std::atof(valueStr.c_str()));
                if (!isValidValue(value)) {
                    if (value < 0)
                        std::cout << "Error: not a positive number." << std::endl;
                    else
                        std::cout << "Error: number too large." << std::endl;
                    continue;
                }
            } catch (const std::exception& e) {
                std::cout << "Error: invalid value." << std::endl;
                continue;
            }

            // Find closest date and calculate result
            std::string closestDate = findClosestDate(date);
            if (database.find(closestDate) != database.end()) {
                float rate = database[closestDate];
                float result = value * rate;
                std::cout << date << " => " << value << " = " << result << std::endl;
            }
        } else {
            std::cout << "Error: bad input => " << line << std::endl;
        }
    }
}
