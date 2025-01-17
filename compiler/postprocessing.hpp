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
        std::regex labelRegex(R"(^\*(\w+) )"); // Match labels starting with '*'

        if (std::regex_search(line, match, labelRegex)) {
            labelPositions[match[1]] = lineIndex;

            // Remove the label from the line
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
        std::regex jumpRegex(R"(\b(JUMP|JPOS|JZERO|JNEG)\s+\*(\w+))"); // Match JUMP commands with labels

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
    }

    // Combine the processed lines back into a single string
    std::ostringstream output;
    for (const std::string& processedLine : lines) {
        output << processedLine << "\n";
    }

    return output.str();
}

#endif // POSTPROCESSING_HPP
