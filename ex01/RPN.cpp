#include "RPN.hpp"

// Constructor
RPN::RPN() {}

// Destructor
RPN::~RPN() {}

// check if token is one of the allowed operators
bool RPN::isOperator(const std::string& token) const {
    return (token == "+" || token == "-" || token == "*" || token == "/");
}

bool RPN::isNumber(const std::string& token) const {
    // Check if token is a single digit number
    if (token.length() != 1) {
        return false;
    }
    // Check if the character is a digit
    if (token[0] >= '0' && token[0] <= '9') {
        return true;
    } else {
        return false;
    }
}

// function takes two numbers and an operator and then performs the calculation
int RPN::performOperation(int a, int b, const std::string& op) const {
    // Perform the specified operation on two numbers
    if (op == "+") {
        return a + b;
    } else if (op == "-") {
        return a - b;
    } else if (op == "*") {
        return a * b;
    } else if (op == "/") {
        // Check for division by zero
        if (b == 0) {
            throw std::runtime_error("Error: Division by zero");
        }
        return a / b;
    } else {
        throw std::runtime_error("Error: Invalid operator");
    }
}

// function takes a single token and processes it according to RPN rules
void RPN::processToken(const std::string& token) {
    if (isNumber(token)) {
        int num = token[0] - '0';  // Convert char to int
        // if it's a number, push it onto the stack
        numbers.push(num);
    } 
    else if (isOperator(token)) {
        // check if there are at least two numbers on the stack
        if (numbers.size() < 2) {
            throw std::runtime_error("Error: Not enough operands for operator");
        }
        // get the two top numbers from the stack
        int b = numbers.top();  // second operand (top of stack)
        numbers.pop(); // remove top element from the stack
        int a = numbers.top();  // first operand
        numbers.pop();
        // perform the operation
        int result = performOperation(a, b, token);
        // push the result back onto the stack
        numbers.push(result);
    }
    else {
        throw std::runtime_error("Error: Invalid token");
    }
}

// function takes a complete RPN input string and processes the calculation
int RPN::calculate(const std::string& expression) {
    // empty the stack for new calculation
    while (!numbers.empty()) {
        numbers.pop();
    }
    // create a string stream to parse the expression
    std::stringstream ss(expression);
    std::string token;
    // process each token in the expression
    while (ss >> token) {
        processToken(token);
    }
    // RPN rules demand that exactly one token is left on the stack in the end
    if (numbers.size() != 1) {
        throw std::runtime_error("Error: Invalid expression");
    }
    return numbers.top(); // the token left on the stack
}
