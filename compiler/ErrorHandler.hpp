#ifndef ERRORHANDLER_HPP
#define ERRORHANDLER_HPP

#include <string>
#include <vector>
#include <mutex>
#include <sstream>
#include <iostream>
#include "Token.hpp"


class ErrorHandler {
public:
    static ErrorHandler& getInstance() {
        static ErrorHandler instance;
        return instance;
    }

    void logError(const std::string& message, Token* token = nullptr) {
        std::lock_guard<std::mutex> lock(mtx);
        std::ostringstream error;

        if (token) {
            error << "ERROR: " << message << " - \'" << token->getValue() << "\' on line: " << token->getLine();
        } else {
            error << "ERROR: " << message;
        }

        errors.push_back(error.str());
    }

    void printErrors() const {
        std::lock_guard<std::mutex> lock(mtx);
        if (errors.empty()) {
            std::cout << "No errors logged.\n";
        } else {
            for (const auto& err : errors) {
                std::cout << err << std::endl;
            }
        }
    }

    void clearErrors() {
        std::lock_guard<std::mutex> lock(mtx);
        errors.clear();
    }

    const std::vector<std::string>& getErrors() const {
        return errors;
    }

private:
    ErrorHandler() = default;
    ~ErrorHandler() = default;

    ErrorHandler(const ErrorHandler&) = delete;
    ErrorHandler& operator=(const ErrorHandler&) = delete;

    std::vector<std::string> errors;
    mutable std::mutex mtx;
};

#define LOG_ERROR(msg, tok) ErrorHandler::getInstance().logError(msg, tok)

#endif // ERRORHANDLER_HPP
