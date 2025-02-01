#ifndef POSTPROCESSING_HPP
#define POSTPROCESSING_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <regex>

// Function to process assembly code by resolving labels and replacing them with relative jumps
std::string calculate_jumps(const std::string& assembly) {
    std::istringstream input(assembly);
    std::string line;
    std::vector<std::string> lines;
    std::unordered_map<std::string, int> labelPositions;

    // First pass: Store all labels and their corresponding line indices
    int lineIndex = 0;
    while (std::getline(input, line)) {
        std::smatch match;
        std::regex labelRegex(R"((\*\w+ )+)"); // Match multiple labels starting with '*'

        if (std::regex_search(line, match, labelRegex)) {
            std::string labels = match[0].str(); // Use match[0] to get the full matched string
            std::istringstream labelStream(labels);
            std::string label;

            // Store each label's position
            while (labelStream >> label) {
                labelPositions[label.substr(1)] = lineIndex; // Remove '*' from label
            }

            // Remove the labels from the line
            line = std::regex_replace(line, labelRegex, "");

            // Add the modified line if it contains other instructions
            if (!line.empty() && line.find_first_not_of(" \t") != std::string::npos) {
                lines.push_back(line);
                lineIndex++;
            }
        } else {
            lines.push_back(line);
            lineIndex++;
        }
    }

    // Second pass: Replace label references with calculated relative jumps
    for (std::string& instruction : lines) {
        std::smatch match;
        std::regex jumpRegex(R"(\b(JUMP|JPOS|JZERO|JNEG)\s+\*(\w+))");  // Match JUMP commands with labels
        std::regex setRegex(R"(\b(SET)\s+\&(\d+))");                    // Match SET commands with &number

        while (std::regex_search(instruction, match, jumpRegex)) {
            std::string label = match[2];
            std::string jumpType = match[1];

            if (labelPositions.find(label) != labelPositions.end()) {
                int targetLine = labelPositions[label];
                int currentLine = &instruction - &lines[0];
                int relativeJump = targetLine - currentLine;

                instruction = std::regex_replace(
                    instruction,
                    jumpRegex,
                    jumpType + " " + std::to_string(relativeJump)
                );
            } else {
                throw std::runtime_error("Undefined label: " + label);
            }
        }

        while (std::regex_search(instruction, match, setRegex)) {
            std::string numberStr = match[2];
            int number = std::stoi(numberStr);
            int currentLine = &instruction - &lines[0];
            int calculatedValue = currentLine + number;

            instruction = std::regex_replace(
                instruction,
                setRegex,
                "SET " + std::to_string(calculatedValue)
            );
        }
    }

    // Combine the processed lines back into a single string
    std::ostringstream output;
    for (const std::string& processedLine : lines) {
        output << processedLine << "\n";
    }

    return output.str();
}

#endif // POSTPROCESSING_HPP
