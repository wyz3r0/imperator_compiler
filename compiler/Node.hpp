#ifndef NODE_HPP
#define NODE_HPP

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include "Token.hpp"


class Node {
public:
    std::vector<Node*> children;
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

    virtual void print(int level = 0) const {
        for (int i = 0; i < level; ++i) {
            std::cout << "  ";
        }
        std::cout << level;

        std::cout << " NodeType: " << getNodeType() << " -> ";

        if (token) {
            token->print();
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

        // TODO : determine whether bools are used to optimise 75 overhead.
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

        // Build procedures and main.
        for (auto node : children) {
            assembly << node->build();
        }

        assembly << "HALT" << std::endl;    // Finish the program.

        return assembly.str();
    }
};

class ProceduresNode : public Node {
public:
    explicit ProceduresNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "PROCEDURES"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;

        for (auto node : children) {
            assembly << node->build();
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

        assembly << children[1]->build();                                       // Put value into R4
        assembly << "LOAD " << 4 << std::endl;
        assembly << "STORE " << children[0]->token->getAddress() << std::endl;  // Store value into variable's addres

        return assembly.str();
    }
};

class IfElseCommandNode : public Node {
public:
    explicit IfElseCommandNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "IF_ELSE_COMMAND"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;

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

        for (auto node : children) {
            assembly << node->build();
        }

        return assembly.str();
    }
};

class RepeatCommandNode : public Node {
public:
    explicit RepeatCommandNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "REPEAT_COMMAND"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;

        for (auto node : children) {
            assembly << node->build();
        }

        return assembly.str();
    }
};

class ForToCommandNode : public Node {
public:
    explicit ForToCommandNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "FORTO_COMMAND"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;

        assembly << "LOAD " << children[0]->token->getAddress() << std::endl;   // Load lower_bound
        assembly << "STORE " << token->getAddress() << std::endl;               // Set iterator to lower_bound
        assembly << "*FOR_BODY_" << id << " " << children[2]->build();          // Insert for body and label
        assembly << "LOAD " << token->getAddress() << std::endl;                // Load iterator
        assembly << "ADD " << 6 << std::endl;                                   // ADD 1 to iterator
        assembly << "SUB " << children[1]->token->getAddress() << std::endl;    // Check whether iterator - upper_bound = 0
        assembly << "JNEG " << "*FOR_BODY_" << id << std::endl;                 // If iterator - upper_bound < 0 jump to FOR_BODY block

        return assembly.str();
    }
};

class ForDownToCommandNode : public Node {
public:
    explicit ForDownToCommandNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "FORDOWNTO_COMMAND"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;

        assembly << "LOAD " << children[1]->token->getAddress() << std::endl;   // Load upper_bound
        assembly << "STORE " << token->getAddress() << std::endl;               // Set iterator to upper_bound
        assembly << "*FOR_BODY_" << id << " " << children[2]->build();          // Insert for body and label
        assembly << "LOAD " << token->getAddress() << std::endl;                // Load iterator
        assembly << "SUB " << 6 << std::endl;                                   // SUB 1 from iterator
        assembly << "SUB " << children[0]->token->getAddress() << std::endl;    // Check whether iterator - lower_bound = 0
        assembly << "JPOS " << "*FOR_BODY_" << id << std::endl;                 // If iterator - lower_bound > 0 jump to FOR_BODY block

        return assembly.str();
    }
};

class ProcCallCommandNode : public Node {
public:
    explicit ProcCallCommandNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "PROC_CALL_COMMAND"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;

        for (auto node : children) {
            assembly << node->build();
        }

        return assembly.str();
    }
};

class ReadCommandNode : public Node {
public:
    explicit ReadCommandNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "READ_COMMAND"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;

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

        assembly << children[0]->build();
        assembly << "PUT " << 4 << std::endl;

        return assembly.str();
    }
};

class ProcHeadNode : public Node {
public:
    explicit ProcHeadNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "PROC_HEAD"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;

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

        for (auto node : children) {
            assembly << node->build();
        }

        return assembly.str();
    }
};

class DeclarationsNode : public Node {
public:
    explicit DeclarationsNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "DECLARATIONS"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;

        // TODO : set variavle values

        for (auto node : children) {
            assembly << node->build();
        }

        return assembly.str();
    }
};

class ArgsDeclNode : public Node {
public:
    explicit ArgsDeclNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "ARGS_DECL"; }
    std::string build(std::vector<Token*> *tokens = nullptr) const override {
        std::ostringstream assembly;

        for (auto node : children) {
            assembly << node->build();
        }

        return assembly.str();
    }
};

class ArgsNode : public Node {
public:
    explicit ArgsNode(Token* token = nullptr, long long id = -1) : Node(token, id) {}
    std::string getNodeType() const override { return "ARGS"; }
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
        assembly << std::endl;

        std::string operation = token->getValue();

        // a *operator* b
        if (operation == "+") {
            assembly << children[1]->build();       // Get b into R4
            assembly << "LOAD " << 4 << std::endl;
            assembly << "STORE " << 1 << std::endl; // Store b in R1
            assembly << children[0]->build();       // Get a into R4
            assembly << "LOAD " << 4 << std::endl;
            assembly << "ADD " << 1 << std::endl;
            assembly << "STORE " << 4 << std::endl; // Store result in R4
        } else if (operation == "-") {
            assembly << children[1]->build();       // Get b into R4
            assembly << "LOAD " << 4 << std::endl;
            assembly << "STORE " << 1 << std::endl; // Store b in R1
            assembly << children[0]->build();       // Get a into R4
            assembly << "LOAD " << 4 << std::endl;
            assembly << "SUB " << 1 << std::endl;
            assembly << "STORE " << 4 << std::endl; // Store result in R4
        } else if (operation == "*") {              // TODO : implement MUL
            assembly << children[1]->build();       // Get b into R4
            assembly << "LOAD " << 4 << std::endl;
            assembly << "STORE " << 1 << std::endl; // Store b in R1
            assembly << children[0]->build();       // Get a into R4
            assembly << "LOAD " << 4 << std::endl;
            assembly << "MUL " << 1 << std::endl;
            assembly << "STORE " << 4 << std::endl; // Store result in R4
        } else if (operation == "/") {              // TODO : implement DIV
            assembly << children[1]->build();       // Get b into R4
            assembly << "LOAD " << 4 << std::endl;
            assembly << "STORE " << 1 << std::endl; // Store b in R1
            assembly << children[0]->build();       // Get a into R4
            assembly << "LOAD " << 4 << std::endl;
            assembly << "DIV " << 1 << std::endl;
            assembly << "STORE " << 4 << std::endl; // Store result in R4
        } else if (operation == "%") {              // TODO : implement MOD
            assembly << children[1]->build();       // Get b into R4
            assembly << "LOAD " << 4 << std::endl;
            assembly << "STORE " << 1 << std::endl; // Store b in R1
            assembly << children[0]->build();       // Get a into R4
            assembly << "LOAD " << 4 << std::endl;
            assembly << "MOD " << 1 << std::endl;
            assembly << "STORE " << 4 << std::endl; // Store result in R4
        } else {
            assembly << children[0]->build();       // Store value in R4
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

        if (token) {
            // get constant number
            assembly << "LOAD " << token->getAddress() << std::endl;
            assembly << "STORE " << 4 << std::endl;
        }
        else {
            // get address of the variable
            assembly << children[0]->build();
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

        assembly << "LOAD " << token->getAddress() << std::endl;
        assembly << "STORE " << 4 << std::endl;

        return assembly.str();
    }
};


#endif
