#ifndef NODE_HPP
#define NODE_HPP

#include <vector>
#include <string>
#include <iostream>
#include "Token.hpp"

// Enum for different node types
enum class NodeType {
    PROGRAM_ALL,
    PROCEDURES,
    MAIN,
    COMMANDS,
    COMMAND,
    PROC_HEAD,
    PROC_CALL,
    DECLARATIONS,
    ARGS_DECL,
    ARGS,
    EXPRESSION,
    CONDITION,
    VALUE,
    IDENTIFIER
};

class Node {
public:
    NodeType type;  // Type of the node
    std::vector<Node*> children;  // Child nodes
    Token* token;  // Associated token (if any)

    // Constructors
    Node(NodeType type) : type(type), token(nullptr) {}
    Node(NodeType type, Token* token) : type(type), token(token) {}

    // Add child node
    void addChild(Node* child) {
        children.push_back(child);
    }

    // Add a token as a child (helper function)
    void addTokenChild(Token* t) {
        Node* tokenNode = new Node(NodeType::IDENTIFIER, t);  // Treat token as an IDENTIFIER node
        addChild(tokenNode);
    }

    // Helper function to return a string representation of the node type
    std::string nodeTypeToString() const {
        switch (type) {
            case NodeType::PROGRAM_ALL: return "PROGRAM_ALL";
            case NodeType::PROCEDURES: return "PROCEDURES";
            case NodeType::MAIN: return "MAIN";
            case NodeType::COMMANDS: return "COMMANDS";
            case NodeType::COMMAND: return "COMMAND";
            case NodeType::PROC_HEAD: return "PROC_HEAD";
            case NodeType::PROC_CALL: return "PROC_CALL";
            case NodeType::DECLARATIONS: return "DECLARATIONS";
            case NodeType::ARGS_DECL: return "ARGS_DECL";
            case NodeType::ARGS: return "ARGS";
            case NodeType::EXPRESSION: return "EXPRESSION";
            case NodeType::CONDITION: return "CONDITION";
            case NodeType::VALUE: return "VALUE";
            case NodeType::IDENTIFIER: return "IDENTIFIER";
            default: return "UNKNOWN";
        }
    }

    // Print the node and its children recursively
    void print(int level = 0) const {
        // Print indentation based on level
        for (int i = 0; i < level; ++i) {
            std::cout << "  ";
        }
        std::cout << level;

        // Print the node type description
        std::cout << " NodeType: " << nodeTypeToString() << " -> ";

        // If the node has an associated token, print its details
        if (token) {
            token->print();
        }
        std::cout << std::endl;

        // Recursively print children
        for (const auto& child : children) {
            child->print(level + 1);
        }
    }
};

#endif
