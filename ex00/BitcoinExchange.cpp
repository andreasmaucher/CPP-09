#include "BitcoinExchange.hpp"

// constructor
BitcoinExchange::BitcoinExchange(const std::string& datacsv) {
    loadDatabase(datacsv); // this is where the container is created
}

// destructor
BitcoinExchange::~BitcoinExchange() {}

// loads data.csv content into a map
void BitcoinExchange::loadDatabase(const std::string& filename) {
    // open file
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        throw std::runtime_error("Error: could not open file.");
    }

    std::string line;
    // reads the first line in the data.csv to skip it in the loop
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string date;
        std::string value;
        
        // there need to be two values in each line separated by a comma
        if (std::getline(ss, date, ',') && std::getline(ss, value)) {
            try {
                // Convert the string value to a floating-point number
                std::string valueString = value;
                const char* cStyleString = valueString.c_str(); // c style conversion because atof expects c format
                double doubleValue = std::atof(cStyleString); // atof returns a double
                float exchangeRate = static_cast<float>(doubleValue); // static cast to make conversion safe

                // Store the exchange rate in the database map
                std::string dateKey = date;
                database[dateKey] = exchangeRate; // database["2011-01-01"] = 0.3f
            } catch (const std::exception& e) {
                std::cerr << "Warning: Invalid conversion in database for date: " << date << std::endl;
            }
        }
    }
}

// parsing the date and running a multitude of checks to ensure it is valid 
std::pair<bool, std::string> BitcoinExchange::parseDate(const std::string& date) const {
    if (date.length() != 10) return std::make_pair(false, "Invalid date format");
    try {
        // extract year / month / day
        int year = std::atoi(date.substr(0, 4).c_str());
        int month = std::atoi(date.substr(5, 2).c_str());
        int day = std::atoi(date.substr(8, 2).c_str());

        // check if hyphens are in the correct place
        if (date[4] != '-' || date[7] != '-') return std::make_pair(false, "Invalid date format");
        if (month < 1 || month > 12) return std::make_pair(false, "Invalid month");
        if (day < 1 || day > 31) return std::make_pair(false, "Invalid day");
        if (year < 2009) return std::make_pair(false, "Satoshi created Bitcoin only after the given date.");

        // Check all months with 30 days
        if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30)
            return std::make_pair(false, "Invalid day for month");
        // leap year check (divisible by 4 AND not divisible by 100 unless divisible by 400)
        if (month == 2) {
            bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
            if ((isLeap && day > 29) || (!isLeap && day > 28))
                return std::make_pair(false, "Invalid day for February, leap year calculation returned an error.");
        }
        return std::make_pair(true, "");
    } catch (const std::exception& e) {
        return std::make_pair(false, "Invalid date format detected in parseData.");
    }
}

bool BitcoinExchange::isValidDate(const std::string& date) const {
    return parseDate(date).first;
}

bool BitcoinExchange::isValidValue(const float value) const {
    return value >= 0 && value <= 1000;
}

// find the closest date in the database that is less than or equal to the input date
std::string BitcoinExchange::findClosestDate(const std::string& date) const {
    std::string closestDate = ""; // starting with nothing found yet
    std::map<std::string, float>::const_iterator it;
    for (it = database.begin(); it != database.end(); ++it) {
        std::string currentDate = it->first;
        // only consider dates before or equal to the given date
        if (currentDate <= date && currentDate > closestDate) {
            closestDate = currentDate;
        }
    }
    return closestDate;
}

// takes the input file and calculates the bitcoin exchange rate for every given date in the file
void BitcoinExchange::processInputFile(const std::string& inputFile) {
    std::ifstream file(inputFile.c_str());
    if (!file.is_open()) {
        std::cout << "Error: could not open file." << std::endl;
        return;
    }

    std::string line;
    std::getline(file, line); // to skip the header line

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string date;
        std::string valueStr;

        if (std::getline(ss, date, '|') && std::getline(ss, valueStr)) {
            // remove whitespace (spaces and tabs)
            date.erase(0, date.find_first_not_of(" \t"));
            date.erase(date.find_last_not_of(" \t") + 1);
            valueStr.erase(0, valueStr.find_first_not_of(" \t"));
            valueStr.erase(valueStr.find_last_not_of(" \t") + 1);

            // check that the date is valid
            if (!isValidDate(date)) {
                std::cout << "Error: bad input, date is not valid  => " << date << std::endl;
                continue;
            }

            // validate the given value (think about it in amount of coins)
            float value;
            try {
                value = static_cast<float>(std::atof(valueStr.c_str()));
                if (!isValidValue(value)) {
                    if (value < 0)
                        std::cout << "Error: not a positive number." << std::endl;
                    else
                        std::cout << "Error: too large a number." << std::endl;
                    continue;
                }
            } catch (const std::exception& e) {
                std::cout << "Error: invalid value." << std::endl;
                continue;
            }

            // find the closest date
            std::string closestDate = findClosestDate(date);
            if (database.find(closestDate) != database.end()) {
                float rate = database[closestDate]; // returns the value associated with the provided key (date)
                float result = value * rate;
                std::cout << date << " => " << value << " = " << result << std::endl;
            }
        } else {
            std::cout << "Error: bad input => " << line << std::endl;
        }
    }
}
