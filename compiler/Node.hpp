#ifndef NODE_HPP
#define NODE_HPP

#include <vector>
#include <string>
#include <iostream>
#include "Token.hpp"

// Abstract base class for all nodes
class Node {
public:
    std::vector<Node*> children; // Child nodes
    Token* token;               // Associated token (if any)

    // Constructor
    explicit Node(Token* token = nullptr) : token(token) {}

    // Destructor
    virtual ~Node() {
        for (auto child : children) {
            delete child;
        }
    }

    // Add child node
    void addChild(Node* child) {
        children.push_back(child);
    }

    // Print the node and its children recursively
    virtual void print(int level = 0) const {
        // Print indentation based on level
        for (int i = 0; i < level; ++i) {
            std::cout << "  ";
        }
        std::cout << level;

        // Print the node type description
        std::cout << " NodeType: " << getNodeType() << " -> ";


        // If the node has an associated token, print its details
        if (token) {
            token->print();
        }
        std::cout << std::endl;

        // std::cout << build() << std::endl;

        // Recursively print children
        for (const auto& child : children) {
            child->print(level + 1);
        }
    }

    // Pure virtual function to get the type of the node as a string
    virtual std::string getNodeType() const = 0;

    // Pure virtual function to generate assembly code
    virtual std::string build() const = 0;
};

// Concrete classes for each node type
class ProgramAllNode : public Node {
public:
    explicit ProgramAllNode(Token* token = nullptr) : Node(token) {}
    std::string getNodeType() const override { return "PROGRAM_ALL"; }
    std::string build() const override {
        return "// Assembly code for PROGRAM_ALL\n";
    }
};

class ProceduresNode : public Node {
public:
    explicit ProceduresNode(Token* token = nullptr) : Node(token) {}
    std::string getNodeType() const override { return "PROCEDURES"; }
    std::string build() const override {
        return "// Assembly code for PROCEDURES\n";
    }
};

class MainNode : public Node {
public:
    explicit MainNode(Token* token = nullptr) : Node(token) {}
    std::string getNodeType() const override { return "MAIN"; }
    std::string build() const override {
        return "// Assembly code for MAIN\n";
    }
};

class CommandsNode : public Node {
public:
    explicit CommandsNode(Token* token = nullptr) : Node(token) {}
    std::string getNodeType() const override { return "COMMANDS"; }
    std::string build() const override {
        return "// Assembly code for COMMANDS\n";
    }
};

class CommandNode : public Node {
public:
    explicit CommandNode(Token* token = nullptr) : Node(token) {}
    std::string getNodeType() const override { return "COMMAND"; }
    std::string build() const override {
        return "// Assembly code for COMMAND\n";
    }
};

class AssignmentCommandNode : public Node {
public:
    explicit AssignmentCommandNode(Token* token = nullptr) : Node(token) {}
    std::string getNodeType() const override { return "ASSIGNMENT_COMMAND"; }
    std::string build() const override {
        return "// Assembly code for ASSIGNMENT_COMMAND\n";
    }
};

class IfElseCommandNode : public Node {
public:
    explicit IfElseCommandNode(Token* token = nullptr) : Node(token) {}
    std::string getNodeType() const override { return "IF_ELSE_COMMAND"; }
    std::string build() const override {
        return "// Assembly code for IF_ELSE_COMMAND\n";
    }
};

class IfCommandNode : public Node {
public:
    explicit IfCommandNode(Token* token = nullptr) : Node(token) {}
    std::string getNodeType() const override { return "IF_COMMAND"; }
    std::string build() const override {
        return "// Assembly code for IF_COMMAND\n";
    }
};

class WhileCommandNode : public Node {
public:
    explicit WhileCommandNode(Token* token = nullptr) : Node(token) {}
    std::string getNodeType() const override { return "WHILE_COMMAND"; }
    std::string build() const override {
        return "// Assembly code for WHILE_COMMAND\n";
    }
};

class RepeatCommandNode : public Node {
public:
    explicit RepeatCommandNode(Token* token = nullptr) : Node(token) {}
    std::string getNodeType() const override { return "REPEAT_COMMAND"; }
    std::string build() const override {
        return "// Assembly code for REPEAT_COMMAND\n";
    }
};

class ForCommandNode : public Node {
public:
    explicit ForCommandNode(Token* token = nullptr) : Node(token) {}
    std::string getNodeType() const override { return "FOR_COMMAND"; }
    std::string build() const override {
        return "// Assembly code for FOR_COMMAND\n";
    }
};

class ProcCallCommandNode : public Node {
public:
    explicit ProcCallCommandNode(Token* token = nullptr) : Node(token) {}
    std::string getNodeType() const override { return "PROC_CALL_COMMAND"; }
    std::string build() const override {
        return "// Assembly code for PROC_CALL_COMMAND\n";
    }
};

class ReadCommandNode : public Node {
public:
    explicit ReadCommandNode(Token* token = nullptr) : Node(token) {}
    std::string getNodeType() const override { return "READ_COMMAND"; }
    std::string build() const override {
        return "// Assembly code for READ_COMMAND\n";
    }
};

class WriteCommandNode : public Node {
public:
    explicit WriteCommandNode(Token* token = nullptr) : Node(token) {}
    std::string getNodeType() const override { return "WRITE_COMMAND"; }
    std::string build() const override {
        return "// Assembly code for WRITE_COMMAND\n";
    }
};

class ProcHeadNode : public Node {
public:
    explicit ProcHeadNode(Token* token = nullptr) : Node(token) {}
    std::string getNodeType() const override { return "PROC_HEAD"; }
    std::string build() const override {
        return "// Assembly code for PROC_HEAD\n";
    }
};

class ProcCallNode : public Node {
public:
    explicit ProcCallNode(Token* token = nullptr) : Node(token) {}
    std::string getNodeType() const override { return "PROC_CALL"; }
    std::string build() const override {
        return "// Assembly code for PROC_CALL\n";
    }
};

class DeclarationsNode : public Node {
public:
    explicit DeclarationsNode(Token* token = nullptr) : Node(token) {}
    std::string getNodeType() const override { return "DECLARATIONS"; }
    std::string build() const override {
        return "// Assembly code for DECLARATIONS\n";
    }
};

class ArgsDeclNode : public Node {
public:
    explicit ArgsDeclNode(Token* token = nullptr) : Node(token) {}
    std::string getNodeType() const override { return "ARGS_DECL"; }
    std::string build() const override {
        return "// Assembly code for ARGS_DECL\n";
    }
};

class ArgsNode : public Node {
public:
    explicit ArgsNode(Token* token = nullptr) : Node(token) {}
    std::string getNodeType() const override { return "ARGS"; }
    std::string build() const override {
        return "// Assembly code for ARGS\n";
    }
};

class ExpressionNode : public Node {
public:
    explicit ExpressionNode(Token* token = nullptr) : Node(token) {}
    std::string getNodeType() const override { return "EXPRESSION"; }
    std::string build() const override {
        return "// Assembly code for EXPRESSION\n";
    }
};

class ConditionNode : public Node {
public:
    explicit ConditionNode(Token* token = nullptr) : Node(token) {}
    std::string getNodeType() const override { return "CONDITION"; }
    std::string build() const override {
        return "// Assembly code for CONDITION\n";
    }
};

class ValueNode : public Node {
public:
    explicit ValueNode(Token* token = nullptr) : Node(token) {}
    std::string getNodeType() const override { return "VALUE"; }
    std::string build() const override {
        return "// Assembly code for VALUE\n";
    }
};

class IdentifierNode : public Node {
public:
    explicit IdentifierNode(Token* token = nullptr) : Node(token) {}
    std::string getNodeType() const override { return "IDENTIFIER"; }
    std::string build() const override {
        return "// Assembly code for IDENTIFIER\n";
    }
};


#endif
