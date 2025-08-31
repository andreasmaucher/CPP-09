#ifndef RPN_HPP
#define RPN_HPP

#include <stack>
#include <string> 
#include <iostream>
#include <sstream>

/*
Why using a stack container for this exercise:

In computer science, the UPN is of interest because it enables stack-based processing: 
operands are placed on the stack when read, an operator retrieves the number of operands 
from the stack that corresponds to its arity, and places the result of the operation back 
on the stack. At the end, the result of the term is at the top of the stack. This is why 
the UPN forms the basis for stack-based programming languages such as Forth, RPL, PostScript,
or the instruction list in the PLC area.
*/

class RPN 
{
    private:
        std::stack<int> numbers;  // stack to store numbers during calculation
        
        bool isOperator(const std::string& token) const;
        bool isNumber(const std::string& token) const;
        int performOperation(int a, int b, const std::string& op) const;
        void processToken(const std::string& token);

    public:
        RPN();
        ~RPN();

        int calculate(const std::string& expression);
};

#endif
