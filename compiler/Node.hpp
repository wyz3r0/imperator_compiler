#ifndef NODE_HPP
#define NODE_HPP

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include "Token.hpp"
#include "ErrorHandler.hpp"

class Node {
public:
    std::vector<Node*> children;
    std::vector<Token*> tokens;
    Token* token;
    long long id;

    explicit Node(Token* token = nullptr, long long id = -1) : token(token), id(id) {}

    virtual ~Node() {
        for (auto child : children) {
            delete child;
        }
    }

    void addChild(Node* child) {
        children.push_back(child);
    }

    void addToken(Token* token) {
        tokens.push_back(token);
    }

    virtual void print(int level = 0) const {
        for (int i = 0; i < level; ++i) {
            std::cout << "  ";
        }
        std::cout << level;

        std::cout << " NodeType: " << getNodeType() << " -> ";

        if (token) {
            token->print();
        }

        if (!tokens.empty()) {
            for (const auto& token : tokens) {
                for (int i = 0; i < level; ++i) {
                    std::cout << "  ";
                }
                std::cout << level << "-> ";
                token->print();
            }
        }

        std::cout << std::endl;

        for (const auto& child : children) {
            child->print(level + 1);
        }
    }

    virtual std::string getNodeType() const = 0;

    virtual std::string build(std::vector<Token*> *tokens = nullptr) const = 0;
};


class ProgramAllNode : public Node {
public:
    explicit ProgramAllNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "PROGRAM_ALL"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;

        // TODO : determine whether bools and constants are used to optimise 75 overhead.
        // INIT bools.
        assembly << "SET " << 1 << std::endl;
        assembly << "STORE " << 6 << std::endl;
        assembly << "HALF" << std::endl;
        assembly << "STORE " << 5 << std::endl;

        // INIT constants
        std::vector<Token*> subvec(tokens->begin() + 2, tokens->end());

        for (auto token : subvec) {
            if (token->getType() == TokenType::NUMBER) {
                assembly << "SET " << token->getValue() << std::endl;
                assembly << "STORE " << token->getAddress() << std::endl;
            }
        }

        assembly << "JUMP " << "*MAIN" << std::endl;    // Skip procedures before main.
        assembly << children[0]->build();               // Insert procedures.
        assembly << "*MAIN ";                           // Label Main.
        assembly << children[1]->build();               // Insert Main.
        assembly << "HALT" << std::endl;                // Finish the program.

        return assembly.str();
    }
};

class ProceduresNode : public Node {
public:
    explicit ProceduresNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "PROCEDURES"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;
        // 0 - procedures, 1 - proc_head, 2 - commands, 3 - declarations (optional)

        if (children.empty()) {
            return assembly.str();
        }

        if (children[0]->getNodeType() == "PROCEDURES") {
            assembly << children[0]->build();
        }

        // Ignore because sometimes children[3] somehow gets object that is not in children
        try {
            children.at(3)->getNodeType();
        } catch (const std::out_of_range& e) { }

        if (children[1]->getNodeType() == "PROC_HEAD" && children[2]->getNodeType() == "COMMANDS") {
            assembly << "*PROC_" + children[1]->token->getValue() << " ";           // Label procedure
            children[1]->build();                                                   // Build proc_head
            assembly << children[2]->build();                                       // Build procedure
            for (auto arg : children[1]->token->getArgs()){

            }
            assembly << "RTRN " << children[1]->token->getAddress() << std::endl;   // Return to the caller
        }

        return assembly.str();
    }
};

class ProcHeadNode : public Node {
public:
    explicit ProcHeadNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "PROC_HEAD"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;
        // 0 - ard_declaration
        // token - procedure_identifier

        std::vector<Token*> *args = new std::vector<Token*>();

        for (auto node : children) {
            assembly << node->build(args);
        }

        token->setArgs(*args);

        return assembly.str();
    }
};

class ArgsDeclNode : public Node {
public:
    explicit ArgsDeclNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "ARGS_DECL"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;

        tokens->push_back(token);

        for (auto node : children) {
            assembly << node->build(tokens);
        }

        return assembly.str();
    }
};

class ProcCallCommandNode : public Node {
public:
    explicit ProcCallCommandNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "PROC_CALL_COMMAND"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;
        // 0 - proc_call

        for (auto node : children) {
            assembly << node->build();
        }

        return assembly.str();
    }
};

class ProcCallNode : public Node {
public:
    explicit ProcCallNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "PROC_CALL"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;
        // 0 - args
        // token - procedure_identifier

        std::vector<Token*> *passed_args = new std::vector<Token*>();
        std::vector<Token*> *args = new std::vector<Token*>(token->getArgs());

        children[0]->build(passed_args);    // Gather passed arguments

        if (args->size() != passed_args->size()){
            if (args->size() > passed_args->size()){
                LOG_ERROR("Not enough arguments passed.", token);
            } else {
                LOG_ERROR("Too many arguments passed.", token);
            }
            return assembly.str();
        }

        for (long long i = 0; i < args->size(); i++) {
            if (!((args->at(i)->getFunction() == TokenFunction::T_ARG && passed_args->at(i)->getFunction() == TokenFunction::TABLE)
                || (args->at(i)->getFunction() == TokenFunction::T_ARG && passed_args->at(i)->getFunction() == TokenFunction::T_ARG)
                || (args->at(i)->getFunction() == TokenFunction::ARG && passed_args->at(i)->getFunction() == TokenFunction::DEFAULT)
                || (args->at(i)->getFunction() == TokenFunction::ARG && passed_args->at(i)->getFunction() == TokenFunction::ARG))) {
                LOG_ERROR("Missmatched argument types.", token);
            }

            if (passed_args->at(i)->getFunction() == TokenFunction::ARG || passed_args->at(i)->getFunction() == TokenFunction::T_ARG) {
                assembly << "LOAD " << passed_args->at(i)->getAddress() << std::endl;
                assembly << "STORE " << args->at(i)->getAddress() << std::endl;
            }
            else {
                assembly << "SET " << passed_args->at(i)->getAddress() << std::endl;
                assembly << "STORE " << args->at(i)->getAddress() << std::endl;
            }
        }

        assembly << "SET " << "&3" << std::endl;                            // Set return address 3 lines forward
        assembly << "STORE " << token->getAddress() << std::endl;           // Store return address in procedure's variable
        assembly << "JUMP " << "*PROC_" + token->getValue() << std::endl;   // Jump to the procedure

        delete passed_args;
        delete args;

        return assembly.str();
    }
};

class ArgsNode : public Node {
public:
    explicit ArgsNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "ARGS"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        // 0 - following args
        // token - this argument
        std::ostringstream assembly;

        tokens->push_back(token);

        for (auto node : children) {
            assembly << node->build(tokens);
        }

        return assembly.str();
    }
};

class MainNode : public Node {
public:
    explicit MainNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "MAIN"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;
        // 0 - declarations, 1 - commands
        // 0 - commands

        // Build declarations and commands.
        for (auto node : children) {
            assembly << node->build();
        }

        return assembly.str();
    }
};

class CommandsNode : public Node {
public:
    explicit CommandsNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "COMMANDS"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;
        // 0 - prev commands, 1 - command
        // 1 - command
        // %empty%

        // Build all the commands.
        for (auto node : children) {
            assembly << node->build();
        }

        return assembly.str();
    }
};

class AssignmentCommandNode : public Node {
public:
    explicit AssignmentCommandNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "ASSIGNMENT_COMMAND"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;
        // 0 - identifier, 1 - expression

        if (children[0]->token->getFunction() == TokenFunction::ARG || children[0]->token->getFunction() == TokenFunction::T_ARG){
            if (children[0]->getNodeType() == "IDENTIFIER") {
                assembly << children[1]->build();                                       // Put value into R4
                assembly << "LOAD " << children[0]->token->getAddress() << std::endl;   // Load address from arg's address
                assembly << "STORE " << 3 << std::endl;                                 // Store address in R3
                assembly << "LOAD " << 4 << std::endl;
                assembly << "STOREI " << 3 << std::endl;                                // Store value into variable's addres
            }
            else {
                // TODO
                assembly << children[0]->children[0]->build();                          // Store index in R4
                assembly << "LOAD " << children[0]->token->getAddress() << std::endl;   // Get address of index0
                assembly << "ADD " << 4 << std::endl;                                   // Calculate absolute address
                assembly << "STORE " << 3 << std::endl;                                 // Store value in R3
                assembly << children[1]->build();                                       // Put value into R4
                assembly << "LOAD " << 4 << std::endl;                                  // Load value from R4
                assembly << "STOREI " << 3 << std::endl;                                // Store value in table
            }
        }
        else {
            if (children[0]->getNodeType() == "IDENTIFIER") {
                assembly << children[1]->build();                                       // Put value into R4
                assembly << "LOAD " << 4 << std::endl;
                assembly << "STORE " << children[0]->token->getAddress() << std::endl;  // Store value into variable's addres
            }
            else {
                assembly << children[1]->build();                                       // Put value into R4
                assembly << "LOAD " << 4 << std::endl;                                  // Load value from R4
                assembly << "STORE " << 1 << std::endl;                                 // Store value in R1
                assembly << children[0]->children[0]->build();                          // Store index in R4
                assembly << "SET " << children[0]->token->getAddress() << std::endl;    // Get address of index0
                assembly << "ADD " << 4 << std::endl;                                   // Calculate absolute address
                assembly << "STORE " << 3 << std::endl;                                 // Store value in R3
                assembly << "LOAD " << 1 << std::endl;                                  // Load value from R1
                assembly << "STOREI " << 3 << std::endl;                                // Store value in table
            }
        }

        return assembly.str();
    }
};

class IfElseCommandNode : public Node {
public:
    explicit IfElseCommandNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "IF_ELSE_COMMAND"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;
        // 0 - condition, 1 - then, 2 - else

        assembly << children[0]->build();                       // In R4 will be 1 if True or 0 if False
        assembly << "LOAD " << 4 << std::endl;
        assembly << "JPOS " << "*THEN_IF_" << id << std::endl;  // If True jump to THEN label
        assembly << children[2]->build();                       // Insert ELSE commands
        assembly << "JUMP " << "*END_IF_" << id << std::endl;   // Jump to the END of the if
        assembly << "*THEN_IF_" << id << " ";                   // Label THEN block
        assembly << children[1]->build();                       // Insert THEN block
        assembly << "*END_IF_" << id << " ";                    // Label END of the if

        return assembly.str();
    }
};

class IfCommandNode : public Node {
public:
    explicit IfCommandNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "IF_COMMAND"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;
        // 0 - condition, 1 - then

        assembly << children[0]->build();                       // In R4 will be 1 if True or 0 if False
        assembly << "LOAD " << 4 << std::endl;
        assembly << "JZERO " << "*END_IF_" << id << std::endl;  // If False jump to END label
        assembly << children[1]->build();                       // Insert ELSE commands
        assembly << "*END_IF_" << id << " ";                    // Label END of the if

        return assembly.str();
    }
};

class WhileCommandNode : public Node {
public:
    explicit WhileCommandNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "WHILE_COMMAND"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;
        // 0 - condition, 1 - command

        assembly << "*COND_WHILE_" << id << " ";                    // Label CONDITION of the while
        assembly << children[0]->build();                           // In R4 will be 1 if True or 0 if False
        assembly << "LOAD " << 4 << std::endl;
        assembly << "JZERO " << "*END_WHILE_" << id << std::endl;   // If False jump to END label
        assembly << children[1]->build();                           // Insert COMMAND block
        assembly << "JUMP " << "*COND_WHILE_" << id << std::endl;   // Jump to the CONDITION of the while
        assembly << "*END_WHILE_" << id << " ";                     // Label END of the while

        return assembly.str();
    }
};

class RepeatCommandNode : public Node {
public:
    explicit RepeatCommandNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "REPEAT_COMMAND"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;
        // 0 - command, 1 - condition

        assembly << "*REPEAT_START_" << id << " ";                  // Label START of the if
        assembly << children[0]->build();                           // Insert COMMAND block
        assembly << children[1]->build();                           // Insert CONDITION of the repeat
        assembly << "LOAD " << 4 << std::endl;
        assembly << "JZERO " << "*REPEAT_START_" << id << std::endl; // If True jump to START label

        return assembly.str();
    }
};

class ForToCommandNode : public Node {
public:
    explicit ForToCommandNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "FORTO_COMMAND"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;
        // 0 - lower_bound, 1 - upper_bound, 2 - commands
        // token - identifier

        assembly << children[0]->build();                                       // Store lower_bound in R4
        assembly << "LOAD " << 4 << std::endl;                                  // Load lower_bound
        assembly << "STORE " << token->getAddress() << std::endl;               // Set iterator to lower_bound
        assembly << "*FOR_BODY_" << id  << " ";                                 // Label BODY of for
        assembly << children[1]->build();                                       // Store upper_bound in R4
        assembly << "LOAD " << token->  getAddress() << std::endl;              // Load iterator
        assembly << "SUB " << 4 << std::endl;                                   // Check whether iterator - upper_bound > 0
        assembly << "JPOS " << "*FOR_END_" << id << std::endl;                  // If iterator - upper_bound > 0
        assembly << children[2]->build();                                       // Insert for body and label
        assembly << "LOAD " << token->getAddress() << std::endl;                // Load iterator
        assembly << "ADD " << 6 << std::endl;                                   // ADD 1 to iterator
        assembly << "STORE " << token->getAddress() << std::endl;               // Store increased iterator
        assembly << "JUMP " << "*FOR_BODY_" << id << std::endl;                 // Jump to FOR_BODY block
        assembly << "*FOR_END_" << id << " ";                                   // Label END of for

        return assembly.str();
    }
};

class ForDownToCommandNode : public Node {
public:
    explicit ForDownToCommandNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "FORDOWNTO_COMMAND"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;
        // 0 - upper_bound, 1 - lower_bound, 2 - commands
        // token - identifier

        assembly << children[0]->build();                                       // Store upper_bound in R4
        assembly << "LOAD " << 4 << std::endl;                                  // Load upper_bound
        assembly << "STORE " << token->getAddress() << std::endl;               // Set iterator to upper_bound
        assembly << "*FOR_BODY_" << id  << " ";                                 // Label BODY of for
        assembly << children[1]->build();                                       // Store lower_bound in R4
        assembly << "LOAD " << token->  getAddress() << std::endl;              // Load iterator
        assembly << "SUB " << 4 << std::endl;                                   // Check whether iterator - lowe_bound < 0
        assembly << "JNEG " << "*FOR_END_" << id << std::endl;                  // If iterator - upper_bound < 0
        assembly << children[2]->build();                                       // Insert for body and label
        assembly << "LOAD " << token->getAddress() << std::endl;                // Load iterator
        assembly << "SUB " << 6 << std::endl;                                   // SUB 1 from iterator
        assembly << "STORE " << token->getAddress() << std::endl;               // Store increased iterator
        assembly << "JUMP " << "*FOR_BODY_" << id << std::endl;                 // Jump to FOR_BODY block
        assembly << "*FOR_END_" << id << " ";                                   // Label END of for

        return assembly.str();
    }
};

class ReadCommandNode : public Node {
public:
    explicit ReadCommandNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "READ_COMMAND"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;
        // 0 - identifier

        assembly << "GET " << children[0]->token->getAddress() << std::endl;

        return assembly.str();
    }
};

class WriteCommandNode : public Node {
public:
    explicit WriteCommandNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "WRITE_COMMAND"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;
        // 0 - value

        assembly << children[0]->build();
        assembly << "PUT " << 4 << std::endl;

        return assembly.str();
    }
};

class DeclarationsNode : public Node {
public:
    explicit DeclarationsNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "DECLARATIONS"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;

        for (auto node : children) {
            assembly << node->build();
        }

        return assembly.str();
    }
};

class ExpressionNode : public Node {
public:
    explicit ExpressionNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "EXPRESSION"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;
        // 0 - a, 1 - b
        // token - operator

        if (token == nullptr) {
            assembly << children[0]->build();           // Store value in R4
            return assembly.str();                      // Return early cause there is no token.
        }

        std::string operation = token->getValue();

        // a *operator* b
        if (operation == "+") {
            assembly << children[1]->build();           // Get b into R4
            assembly << "LOAD " << 4 << std::endl;
            assembly << "STORE " << 1 << std::endl;     // Store b in R1
            assembly << children[0]->build();           // Get a into R4
            assembly << "LOAD " << 4 << std::endl;
            assembly << "ADD " << 1 << std::endl;
            assembly << "STORE " << 4 << std::endl;     // Store result in R4
        } else if (operation == "-") {
            assembly << children[1]->build();           // Get b into R4
            assembly << "LOAD " << 4 << std::endl;
            assembly << "STORE " << 1 << std::endl;     // Store b in R1
            assembly << children[0]->build();           // Get a into R4
            assembly << "LOAD " << 4 << std::endl;
            assembly << "SUB " << 1 << std::endl;
            assembly << "STORE " << 4 << std::endl;     // Store result in R4
        } else if (operation == "*") {
            assembly << "LOAD " << 5 << std::endl;      // Load 0
            assembly << "STORE " << 3 << std::endl;     // Set sign to positive
            assembly << children[1]->build();           // Get b into R4
            assembly << "LOAD " << 4 << std::endl;
            assembly << "STORE " << 2 << std::endl;     // Store b in R2
            assembly << "JNEG " << 2 << std::endl;      // If b < 0 jump 2 lines forward
            assembly << "JUMP " << 6 << std::endl;      // Jump 6 lines to a check
            assembly << "SUB " << 2 << std::endl;       //! -b
            assembly << "SUB " << 2 << std::endl;       // Make number positive
            assembly << "STORE " << 2 << std::endl;     // Store positive b
            assembly << "LOAD " << 6 << std::endl;      // Load 1
            assembly << "STORE " << 3 << std::endl;     // Set sign to negative
            assembly << children[0]->build();           // Get a into R4
            assembly << "LOAD " << 4 << std::endl;
            assembly << "STORE " << 1 << std::endl;     // Store a in R1
            assembly << "JNEG " << 2 << std::endl;      // If a < 0 jump 2 lines forward
            assembly << "JUMP " << 11 << std::endl;     // Jump _ lines to a check
            assembly << "SUB " << 1 << std::endl;       //! -a
            assembly << "SUB " << 1 << std::endl;       // Make number positive
            assembly << "STORE " << 1 << std::endl;     // Store positive a
            assembly << "LOAD " << 3 << std::endl;      // Load sign
            assembly << "JPOS " << 4 << std::endl;      // If sign = 1 jump 4 lines forward
            assembly << "ADD " << 6 << std::endl;       // Set sign to negative
            assembly << "STORE " << 3 << std::endl;     // Store negative sign
            assembly << "JUMP " << 3 << std::endl;      // Jump 3 lines to the end of the sign check
            assembly << "HALF" << std::endl;            // Set sign to positive
            assembly << "STORE " << 3 << std::endl;     // Store positive sign
            assembly << "LOAD " << 1 << std::endl;      // Load a
            assembly << "SUB " << 2 << std::endl;       // Subtract b from a
            assembly << "JPOS " << 7 << std::endl;      // If a > b skip swapping a and b
            assembly << "LOAD " << 1 << std::endl;      // Load a
            assembly << "STORE " << 4 << std::endl;     // Store a in b
            assembly << "LOAD " << 2 << std::endl;      // Load b
            assembly << "STORE " << 1 << std::endl;     // Store b in a
            assembly << "LOAD " << 4 << std::endl;      // Load a
            assembly << "STORE " << 2 << std::endl;     // Store b in a
            assembly << "LOAD " << 5 << std::endl;      // Load 0
            assembly << "STORE " << 4 << std::endl;     // Zero result
            assembly << "LOAD " << 2 << std::endl;      //! a >= b
            assembly << "HALF" << std::endl;
            assembly << "ADD " << 0 << std::endl;
            assembly << "SUB " << 2 << std::endl;       // Check if 2|b
            assembly << "JZERO " << 4 << std::endl;     // If 2|b jump 4 lines forward
            assembly << "LOAD " << 4 << std::endl;      // Load reminder sum
            assembly << "ADD " << 1 << std::endl;       // Add a to the sum
            assembly << "STORE " << 4 << std::endl;     // Store result in R4
            assembly << "LOAD " << 2 << std::endl;      // Load halfed b
            assembly << "HALF" << std::endl;            // Half b again
            assembly << "JZERO " << 6 << std::endl;     // If b = 0 jump to the end
            assembly << "STORE " << 2 << std::endl;     // Store halfed b
            assembly << "LOAD " << 1 << std::endl;      // Load a
            assembly << "ADD " << 1 << std::endl;       // Double a
            assembly << "STORE " << 1 << std::endl;     // Store doubled a
            assembly << "JUMP " << -15 << std::endl;    // Jump to the beginning
            assembly << "LOAD " << 3 << std::endl;      // Load sign
            assembly << "JZERO " << 5 << std::endl;     // If sign = 0 jump 4 lines to the end
            assembly << "LOAD " << 4 << std::endl;      // Load result
            assembly << "SUB " << 4 << std::endl;
            assembly << "SUB " << 4 << std::endl;       // Negate result
            assembly << "JUMP " << 2 << std::endl;      // Jump 2 lines to avoid unnecessary load
            assembly << "LOAD " << 4 << std::endl;      // Load result
            assembly << "STORE " << 4 << std::endl;     // Store result in R4
        } else if (operation == "/") {
            /*
            1 - a
            2 - b
            3 - b_temp
            4 - counter
            7 - sign
            8 - temp_counter
            */
            assembly << "LOAD " << 5 << std::endl;
            assembly << "STORE " << 7 << std::endl;     // Zero sign
            assembly << children[1]->build();           // Get b into R4
            assembly << "LOAD " << 4 << std::endl;
            assembly << "JZERO " << "*DIV_BY_ZERO_" << id << std::endl;      // If b = 0 return 0
            assembly << "STORE " << 2 << std::endl;     // Store b in R1
            assembly << "JNEG " << 2 << std::endl;      // If b < 0 jump 2 lines forward
            assembly << "JUMP " << 7 << std::endl;      // Jump 7 lines to a check
            assembly << "SUB " << 2 << std::endl;       //! -b
            assembly << "SUB " << 2 << std::endl;       // Make number positive
            assembly << "STORE " << 2 << std::endl;     // Store positive b
            assembly << "LOAD " << 6 << std::endl;      // Load 1
            assembly << "ADD " << 6 << std::endl;       // Make it 2
            assembly << "STORE " << 7 << std::endl;     // Set sign to negative (R7)
            assembly << children[0]->build();           // Get a into R4
            assembly << "LOAD " << 4 << std::endl;
            assembly << "JZERO " << "*DIV_BY_ZERO_" << id << std::endl;      // If b = 0 return 0
            assembly << "STORE " << 1 << std::endl;     // Store a in R1
            assembly << "JNEG " << 2 << std::endl;      // If a < 0 jump 2 lines forward
            assembly << "JUMP " << 7 << std::endl;      // Jump 7 lines to a check
            assembly << "SUB " << 1 << std::endl;       //! -a
            assembly << "SUB " << 1 << std::endl;       // Make number positive
            assembly << "STORE " << 1 << std::endl;     // Store positive a
            assembly << "LOAD " << 7 << std::endl;      // Load sign
            assembly << "ADD " << 6 << std::endl;       // Add 1
            assembly << "STORE " << 7 << std::endl;     // Store sign

            assembly << "LOAD " << 6 << std::endl;      // Load 1
            assembly << "STORE " << 8 << std::endl;     // Set temp_counter to 1
            assembly << "HALF" << std::endl;            // Set 0
            assembly << "STORE " << 4 << std::endl;     // Set counter to 0

            assembly << "*DIV_START_LOOP_" << id << " ";// Label START of the division
            assembly << "LOAD " << 2 << std::endl;      // Load b
            assembly << "STORE " << 3 << std::endl;     // Store temp_b
            assembly << "*DIV_LOOP_" << id << " ";      // Label START of the division loop
            assembly << "LOAD " << 1 << std::endl;      // Load a
            assembly << "SUB " << 3 << std::endl;       // Subtract temp_b from a
            assembly << "JNEG " << "*DIV_END_LOOP_" << id << std::endl;      // If a < temp_b jump to the end
            assembly << "LOAD " << 8 << std::endl;      // Load temp_counter
            assembly << "ADD " << 8 << std::endl;       // Double temp_counter
            assembly << "STORE " << 8 << std::endl;     // Store doubled temp_counter
            assembly << "LOAD " << 3 << std::endl;      // Load temp_b
            assembly << "ADD " << 3 << std::endl;       // Double temp_b
            assembly << "STORE " << 3 << std::endl;     // Store doubled temp_b
            assembly << "JUMP " << "*DIV_LOOP_" << id << std::endl;         // Jump to the end of the loop
            assembly << "*DIV_END_LOOP_" << id << " ";  // Label END of the division
            assembly << "LOAD " << 8 << std::endl;      // Load temp_counter
            assembly << "HALF" << std::endl;            // Half temp_counter
            assembly << "STORE " << 8 << std::endl;     // Store halfed temp_counter
            assembly << "ADD " << 4 << std::endl;       // Add counter
            assembly << "STORE " << 4 << std::endl;     // Store counter
            assembly << "LOAD " << 3 << std::endl;      // Load temp_b
            assembly << "HALF" << std::endl;            // Half temp_b
            assembly << "STORE " << 3 << std::endl;     // Store halfed temp_b
            assembly << "LOAD " << 1 << std::endl;      // Load a
            assembly << "SUB " << 3 << std::endl;       // Subtract temp_b from a
            assembly << "STORE " << 1 << std::endl;     // Store a
            assembly << "SUB " << 2 << std::endl;       // Subtract b from a
            assembly << "JNEG " << "*DIV_SIGN_" << id << std::endl;         // If a < b jump to the end
            assembly << "LOAD " << 6 << std::endl;      // Load 1
            assembly << "STORE " << 8 << std::endl;     // Reset temp_counter
            assembly << "JUMP " << "*DIV_START_LOOP_" << id << std::endl;   // Jump to the start of the loop

            assembly << "*DIV_SIGN_" << id << " ";      // Label END of the division
            assembly << "LOAD " << 7 << std::endl;      // Load sign
            assembly << "JZERO " << "*DIV_pp_" << id << std::endl; // Jump to a, b > 0
            assembly << "SUB " << 6 << std::endl;       // Substract 1
            assembly << "JZERO " << "*DIV_np_" << id << std::endl; // Jump to a > 0, b < 0
            assembly << "SUB " << 6 << std::endl;       // Substract 1
            assembly << "JZERO " << "*DIV_pn_" << id << std::endl; // Jump to a < 0, b > 0

            assembly << "*DIV_pp_" << id << " ";        // Case a, b > 0 and a, b < 0
            assembly << "LOAD " << 4 << std::endl;      // Load result
            assembly << "JUMP " << "*DIV_RETURN_" << id << std::endl;       // Jump to the return
            assembly << "*DIV_pn_" << id << " ";
            assembly << "LOAD " << 4 << std::endl;      // Load result
            assembly << "ADD " << 6 << std::endl;       // Add 1
            assembly << "STORE " << 8 << std::endl;     // Temporarly store value
            assembly << "SUB " << 8 << std::endl;
            assembly << "SUB " << 8 << std::endl;       // Make it negative
            assembly << "JUMP " << "*DIV_RETURN_" << id << std::endl;       // Jump to the return
            assembly << "*DIV_np_" << id << " ";
            assembly << "LOAD " << 4 << std::endl;      // Load result
            assembly << "ADD " << 6 << std::endl;       // Add 1
            assembly << "STORE " << 8 << std::endl;     // Temporarly store value
            assembly << "SUB " << 8 << std::endl;
            assembly << "SUB " << 8 << std::endl;       // Make it negative
            assembly << "JUMP " << "*DIV_RETURN_" << id << std::endl;       // Jump to the return

            assembly << "*DIV_RETURN_" << id << " ";
            assembly << "JUMP " << 2 << std::endl;      // Jump over the division by 0
            assembly << "*DIV_BY_ZERO_" << id << " ";   // Label END of the division
            assembly << "LOAD " << 5 << std::endl;      // Load 0
            assembly << "STORE " << 4 << std::endl;     // Store result in R4
        } else if (operation == "%") {                  // TODO : make it work for -1/2
            /*
            1 - a
            2 - b
            3 - b_temp
            4 - counter
            7 - sign
            8 - temp_counter
            */
            assembly << "LOAD " << 5 << std::endl;
            assembly << "STORE " << 7 << std::endl;     // Zero sign
            assembly << children[1]->build();           // Get b into R4
            assembly << "LOAD " << 4 << std::endl;
            assembly << "JZERO " << "*MOD_BY_ZERO_" << id << std::endl;      // If b = 0 return 0
            assembly << "STORE " << 2 << std::endl;     // Store b in R1
            assembly << "JNEG " << 2 << std::endl;      // If b < 0 jump 2 lines forward
            assembly << "JUMP " << 7 << std::endl;      // Jump 7 lines to a check
            assembly << "SUB " << 2 << std::endl;       //! -b
            assembly << "SUB " << 2 << std::endl;       // Make number positive
            assembly << "STORE " << 2 << std::endl;     // Store positive b
            assembly << "LOAD " << 6 << std::endl;      // Load 1
            assembly << "ADD " << 6 << std::endl;       // Make it 2
            assembly << "STORE " << 7 << std::endl;     // Set sign to negative (R7)
            assembly << children[0]->build();           // Get a into R4
            assembly << "LOAD " << 4 << std::endl;
            assembly << "JZERO " << "*MOD_BY_ZERO_" << id << std::endl;      // If b = 0 return 0
            assembly << "STORE " << 1 << std::endl;     // Store a in R1
            assembly << "JNEG " << 2 << std::endl;      // If a < 0 jump 2 lines forward
            assembly << "JUMP " << 7 << std::endl;      // Jump 7 lines to a check
            assembly << "SUB " << 1 << std::endl;       //! -a
            assembly << "SUB " << 1 << std::endl;       // Make number positive
            assembly << "STORE " << 1 << std::endl;     // Store positive a
            assembly << "LOAD " << 7 << std::endl;      // Load sign
            assembly << "ADD " << 6 << std::endl;       // Add 1
            assembly << "STORE " << 7 << std::endl;     // Store sign

            assembly << "LOAD " << 6 << std::endl;      // Load 1
            assembly << "STORE " << 8 << std::endl;     // Set temp_counter to 1
            assembly << "HALF" << std::endl;            // Set 0
            assembly << "STORE " << 4 << std::endl;     // Set counter to 0

            assembly << "*MOD_START_LOOP_" << id << " ";// Label START of the division
            assembly << "LOAD " << 2 << std::endl;      // Load b
            assembly << "STORE " << 3 << std::endl;     // Store temp_b
            assembly << "*MOD_LOOP_" << id << " ";      // Label START of the division loop
            assembly << "LOAD " << 1 << std::endl;      // Load a
            assembly << "SUB " << 3 << std::endl;       // Subtract temp_b from a
            assembly << "JNEG " << "*MOD_END_LOOP_" << id << std::endl;      // If a < temp_b jump to the end
            assembly << "LOAD " << 8 << std::endl;      // Load temp_counter
            assembly << "ADD " << 8 << std::endl;       // Double temp_counter
            assembly << "STORE " << 8 << std::endl;     // Store doubled temp_counter
            assembly << "LOAD " << 3 << std::endl;      // Load temp_b
            assembly << "ADD " << 3 << std::endl;       // Double temp_b
            assembly << "STORE " << 3 << std::endl;     // Store doubled temp_b
            assembly << "JUMP " << "*MOD_LOOP_" << id << std::endl;         // Jump to the end of the loop
            assembly << "*MOD_END_LOOP_" << id << " ";  // Label END of the division
            assembly << "LOAD " << 8 << std::endl;      // Load temp_counter
            assembly << "HALF" << std::endl;            // Half temp_counter
            assembly << "STORE " << 8 << std::endl;     // Store halfed temp_counter
            assembly << "ADD " << 4 << std::endl;       // Add counter
            assembly << "STORE " << 4 << std::endl;     // Store counter
            assembly << "LOAD " << 3 << std::endl;      // Load temp_b
            assembly << "HALF" << std::endl;            // Half temp_b
            assembly << "STORE " << 3 << std::endl;     // Store halfed temp_b
            assembly << "LOAD " << 1 << std::endl;      // Load a
            assembly << "SUB " << 2 << std::endl;       // Subtract b from a
            assembly << "JNEG " << "*MOD_SIGN_" << id << std::endl;         // If a < b jump to the end
            assembly << "LOAD " << 1 << std::endl;      // Load a
            assembly << "SUB " << 3 << std::endl;       // Subtract temp_b from a
            assembly << "STORE " << 1 << std::endl;     // Store a
            assembly << "LOAD " << 6 << std::endl;      // Load 1
            assembly << "STORE " << 8 << std::endl;     // Reset temp_counter
            assembly << "JUMP " << "*MOD_START_LOOP_" << id << std::endl;   // Jump to the start of the loop

            assembly << "*MOD_SIGN_" << id << " ";      // Label END of the division
            assembly << "LOAD " << 7 << std::endl;      // Load sign
            assembly << "JZERO " << "*MOD_pp_" << id << std::endl; // Jump to a, b > 0
            assembly << "SUB " << 6 << std::endl;       // Substract 1
            assembly << "JZERO " << "*MOD_np_" << id << std::endl; // Jump to a > 0, b < 0
            assembly << "SUB " << 6 << std::endl;       // Substract 1
            assembly << "JZERO " << "*MOD_pn_" << id << std::endl; // Jump to a < 0, b > 0

            assembly << "LOAD " << 5 << std::endl;      // Case a, b < 0
            assembly << "SUB " << 1 << std::endl;       // Negate result
            assembly << "JUMP " << "*MOD_RETURN_" << id << std::endl;       // Jump to the return
            assembly << "*MOD_pp_" << id << " ";
            assembly << "LOAD " << 1 << std::endl;      // Load result
            assembly << "JUMP " << "*MOD_RETURN_" << id << std::endl;       // Jump to the return
            assembly << "*MOD_pn_" << id << " ";
            assembly << "LOAD " << 1 << std::endl;      // Load result
            assembly << "SUB " << 2 << std::endl;       // Sub b
            assembly << "JUMP " << "*MOD_RETURN_" << id << std::endl;       // Jump to the return
            assembly << "*MOD_np_" << id << " ";
            assembly << "LOAD " << 2 << std::endl;      // Load b
            assembly << "SUB " << 1 << std::endl;       // Sub result
            assembly << "JUMP " << "*MOD_RETURN_" << id << std::endl;       // Jump to the return

            assembly << "*MOD_RETURN_" << id << " ";
            assembly << "JUMP " << 2 << std::endl;      // Jump over the division by 0
            assembly << "*MOD_BY_ZERO_" << id << " ";   // Label END of the division
            assembly << "LOAD " << 5 << std::endl;      // Load 0
            assembly << "STORE " << 4 << std::endl;     // Store result in R4
        }

        return assembly.str();
    }
};

class ConditionNode : public Node {
public:
    explicit ConditionNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "CONDITION"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;

        std::string operation = token->getValue();

        // a *operator* b
        if (operation == "<") {
            assembly << children[1]->build();
            assembly << "LOAD " << 4 << std::endl;
            assembly << "STORE " << 1 << std::endl;
            assembly << children[0]->build();
            assembly << "LOAD " << 4 << std::endl;
            assembly << "SUB " << 1 << std::endl;       // a - b
            assembly << "JNEG " << 3 << std::endl;      // If a - b < 0 jump 3 lines forward
            assembly << "LOAD " << 5 << std::endl;      // Load 0
            assembly << "JUMP " << 2 << std::endl;      // Jump 2 lines to go around else
            assembly << "LOAD " << 6 << std::endl;      // If a - b < 0 load 1
            assembly << "STORE " << 4 << std::endl;     // Store result in R4
        } else if (operation == "<=") {
            assembly << children[1]->build();
            assembly << "LOAD " << 4 << std::endl;
            assembly << "STORE " << 1 << std::endl;
            assembly << children[0]->build();
            assembly << "LOAD " << 4 << std::endl;
            assembly << "SUB " << 1 << std::endl;       // a - b
            assembly << "JPOS " << 3 << std::endl;      // If a - b > 0 jump 3 lines forward
            assembly << "LOAD " << 6 << std::endl;      // Load 1
            assembly << "JUMP " << 2 << std::endl;      // Jump 2 lines to go around else
            assembly << "LOAD " << 5 << std::endl;      // If a - b > 0 load 0
            assembly << "STORE " << 4 << std::endl;     // Store result in R4
        } else if (operation == "=") {
            assembly << children[1]->build();
            assembly << "LOAD " << 4 << std::endl;
            assembly << "STORE " << 1 << std::endl;
            assembly << children[0]->build();
            assembly << "LOAD " << 4 << std::endl;
            assembly << "SUB " << 1 << std::endl;       // a - b
            assembly << "JZERO " << 3 << std::endl;     // If a - b = 0 jump 3 lines forward
            assembly << "LOAD " << 5 << std::endl;      // Load 0
            assembly << "JUMP " << 2 << std::endl;      // Jump 2 lines to go around else
            assembly << "LOAD " << 6 << std::endl;      // If a - b = 0 load 1
            assembly << "STORE " << 4 << std::endl;     // Store result in R4
        } else if (operation == ">=") {
            assembly << children[1]->build();
            assembly << "LOAD " << 4 << std::endl;
            assembly << "STORE " << 1 << std::endl;
            assembly << children[0]->build();
            assembly << "LOAD " << 4 << std::endl;
            assembly << "SUB " << 1 << std::endl;       // a - b
            assembly << "JNEG " << 3 << std::endl;      // If a - b < 0 jump 3 lines forward
            assembly << "LOAD " << 6 << std::endl;      // Load 1
            assembly << "JUMP " << 2 << std::endl;      // Jump 2 lines to go around else
            assembly << "LOAD " << 5 << std::endl;      // If a - b < 0 load 0
            assembly << "STORE " << 4 << std::endl;     // Store result in R4
        } else if (operation == ">") {
            assembly << children[1]->build();
            assembly << "LOAD " << 4 << std::endl;
            assembly << "STORE " << 1 << std::endl;
            assembly << children[0]->build();
            assembly << "LOAD " << 4 << std::endl;
            assembly << "SUB " << 1 << std::endl;       // a - b
            assembly << "JPOS " << 3 << std::endl;      // If a - b > 0 jump 3 lines forward
            assembly << "LOAD " << 5 << std::endl;      // Load 0
            assembly << "JUMP " << 2 << std::endl;      // Jump 2 lines to go around else
            assembly << "LOAD " << 6 << std::endl;      // If a - b > 0 load 1
            assembly << "STORE " << 4 << std::endl;     // Store result in R4
        } else if (operation == "!=") {
            assembly << children[1]->build();
            assembly << "LOAD " << 4 << std::endl;
            assembly << "STORE " << 1 << std::endl;
            assembly << children[0]->build();
            assembly << "LOAD " << 4 << std::endl;
            assembly << "SUB " << 1 << std::endl;       // a - b
            assembly << "JZERO " << 3 << std::endl;     // If a - b = 0 jump 3 lines forward
            assembly << "LOAD " << 6 << std::endl;      // Load 1
            assembly << "JUMP " << 2 << std::endl;      // Jump 2 lines to go around else
            assembly << "LOAD " << 5 << std::endl;      // If a - b = 0 load 0
            assembly << "STORE " << 4 << std::endl;     // Store result in R4
        }

        return assembly.str();
    }
};

class ValueNode : public Node {
public:
    explicit ValueNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "VALUE"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;

        assembly << children[0]->build();

        return assembly.str();
    }
};

class NumberNode : public Node {
public:
    explicit NumberNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "NUMBER"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;

        if (token->getFunction() == TokenFunction::ARG) {
            assembly << "LOADI " << token->getAddress() << std::endl;
            assembly << "STORE " << 4 << std::endl;
        } else {
            assembly << "LOAD " << token->getAddress() << std::endl;
            assembly << "STORE " << 4 << std::endl;
        }


        return assembly.str();
    }
};

class IdentifierNode : public Node {
public:
    explicit IdentifierNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "IDENTIFIER"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;

        if (token->getFunction() == TokenFunction::ARG) {
            assembly << "LOADI " << token->getAddress() << std::endl;
            assembly << "STORE " << 4 << std::endl;
        } else {
            assembly << "LOAD " << token->getAddress() << std::endl;
            assembly << "STORE " << 4 << std::endl;
        }

        return assembly.str();
    }
};

class TableNode : public Node {
public:
    explicit TableNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "TABEL"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;

        if (token->getFunction() == TokenFunction::T_ARG) {
            assembly << children[0]->build();                           // Store index in R4
            assembly << "LOAD " << token->getAddress() << std::endl;    // Get address of index0
            assembly << "ADD " << 4 << std::endl;                       // Calculate absolute address
            assembly << "LOADI " << 0 << std::endl;                     // Load value from table
            assembly << "STORE " << 4 << std::endl;                     // Store value in R4
        } else {
            assembly << children[0]->build();                           // Store index in R4
            assembly << "SET " << token->getAddress() << std::endl;     // Get address of index0
            assembly << "ADD " << 4 << std::endl;                       // Calculate absolute address
            assembly << "LOADI " << 0 << std::endl;                     // Load value from table
            assembly << "STORE " << 4 << std::endl;                     // Store value in R4
        }

        return assembly.str();
    }
};

#endif
