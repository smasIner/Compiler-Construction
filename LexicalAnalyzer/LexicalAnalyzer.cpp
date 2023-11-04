//
// Created by Murat Shaikhutdinov on 26.09.2023.
//
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cctype>

// Token type enumeration
enum TokenType {
    //0-10
    VAR, TYPE, ROUTINE, RECORD, ARRAY, INTEGER, REAL, BOOLEAN, TRUE, FALSE, WHILE,
    //11-24
    LOOP, END, IF, THEN, ELSE, FOR, IN, REVERSE, AND, OR, XOR, NOT, COLON, EQUALS,
    //25-29
    NOT_EQUALS, GREATER_THAN, GREATER_THAN_OR_EQUAL, LESS_THAN, LESS_THAN_OR_EQUAL,
    //30-38
    DIVIDE, MODULO, MULTIPLY, ADD, SUBTRACT, RANGE, IS, ACCESS_RECORD, INTEGER_LITERAL,
    //39-45
    REAL_LITERAL, BOOLEAN_LITERAL, IDENTIFIER, LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    //46-52
    LEFT_BRACKET, RIGHT_BRACKET, COMMA, INVALID_CHARACTER, END_OF_INPUT, ASSIGNMENT, RETURN
};

// Token structure
struct Token {
    TokenType type;
    std::string lexeme;
    int line; // Line number
    int intValue; // Integer value
    double realValue; // Real value
    bool boolValue; // Boolean value

    Token(TokenType type, const std::string& lexeme, int line, int intValue = 0, double realValue = 0.0, bool boolValue = false)
            : type(type), lexeme(lexeme), line(line), intValue(intValue), realValue(realValue), boolValue(boolValue) {}
};

std::unordered_map<std::string, TokenType> tokenMap = {
        {"var", VAR}, {"type", TYPE}, {"routine", ROUTINE}, {"record", RECORD},
        {"array", ARRAY}, {"integer", INTEGER}, {"real", REAL}, {"boolean", BOOLEAN},
        {"true", TRUE}, {"false", FALSE}, {"while", WHILE}, {"loop", LOOP}, {"end", END},
        {"if", IF}, {"then", THEN}, {"else", ELSE}, {"for", FOR}, {"in", IN},
        {"reverse", REVERSE}, {"and", AND}, {"or", OR}, {"xor", XOR}, {"not", NOT},
        {":=", ASSIGNMENT},
        {":", COLON}, {"=", EQUALS}, {"/=", NOT_EQUALS}, {">", GREATER_THAN},
        {">=", GREATER_THAN_OR_EQUAL}, {"<", LESS_THAN}, {"<=", LESS_THAN_OR_EQUAL},
        {"/", DIVIDE}, {"%", MODULO}, {"*", MULTIPLY}, {"+", ADD}, {"-", SUBTRACT},
        {"..", RANGE}, {"is", IS}, {".", ACCESS_RECORD}, {"(", LEFT_PAREN},
        {")", RIGHT_PAREN}, {"{", LEFT_BRACE}, {"}", RIGHT_BRACE},
        {"[", LEFT_BRACKET}, {"]", RIGHT_BRACKET}, {",", COMMA}, {"return", RETURN}
};

// Function to tokenize input
std::vector<Token> tokenize(std::istream& input) {
    std::vector<Token> tokens;
    int currentLine = 1; // Line counter
    std::string line;

    while (std::getline(input, line)) {
        std::size_t currentIndex = 0;

        while (currentIndex < line.size()) {
            char currentChar = line[currentIndex];

            // Handle whitespace and newlines
            if (std::isspace(currentChar)) {
                currentIndex++;
                continue;
            }

            // Check for valid identifiers
            if (std::isalpha(currentChar) || currentChar == '_') {
                size_t tokenStart = currentIndex;
                while (currentIndex < line.size() &&
                       (std::isalnum(line[currentIndex]) || line[currentIndex] == '_')) {
                    currentIndex++;
                }
                std::string identifier = line.substr(tokenStart, currentIndex - tokenStart);

                // Lookup identifier in tokenMap
                if (tokenMap.find(identifier) != tokenMap.end()) {
                    tokens.push_back({tokenMap[identifier], identifier, currentLine});
                } else {
                    tokens.push_back({IDENTIFIER, identifier, currentLine});
                }

                continue;
            }

            // Check for numeric literalsч
            if (std::isdigit(currentChar)) {
                size_t tokenStart = currentIndex;
                while (currentIndex < line.size() &&
                       (std::isdigit(line[currentIndex]) or line[currentIndex]=='.')
                       and !(line[currentIndex]=='.' and line[currentIndex+1] == '.' )) {
                    std::cout<<line[currentIndex];
                    currentIndex++;
                }
                std::string literal = line.substr(tokenStart, currentIndex - tokenStart);

                if (literal.find('.') != std::string::npos) {
                        double realValue = std::stod(literal); // Convert to double
                        tokens.push_back({REAL_LITERAL, literal, currentLine, 0, realValue});
                    //}
                } else {
                    int intValue = std::stoi(literal); // Convert to integer
                    tokens.push_back({INTEGER_LITERAL, literal, currentLine, intValue});
                }

                continue;
            }

            //Handle range token
            if (currentChar == '.' && line[currentIndex + 1] == '.') {
                std::string declaration = "..";
                tokens.push_back({RANGE, declaration, currentLine});
                currentIndex += 2;
                continue;
            }

            //Handle declaration token
            if (currentChar == ':' && line[currentIndex + 1] == '=') {
                std::string declaration = ":=";
                tokens.push_back({ASSIGNMENT, declaration, currentLine});
                currentIndex += 2;
                continue;
            }

            //Handle less than or equal token
            if (currentChar == '<' && line[currentIndex + 1] == '=') {
                std::string lessThanOrEqual = "<=";
                tokens.push_back({LESS_THAN_OR_EQUAL, lessThanOrEqual, currentLine});
                currentIndex += 2;
                continue;
            }

            //Handle greater than or equal token
            if (currentChar == '>' && line[currentIndex + 1] == '=') {
                std::string greaterThanOrEqual = ">=";
                tokens.push_back({GREATER_THAN_OR_EQUAL, greaterThanOrEqual, currentLine});
                currentIndex += 2;
                continue;
            }

            // Handle single-character tokens
            std::string currentCharStr(1, currentChar);
            if (tokenMap.find(currentCharStr) != tokenMap.end()) {
                tokens.push_back({tokenMap[currentCharStr], currentCharStr, currentLine});
                currentIndex++;
                continue;
            }

            // Handle invalid characters
            tokens.push_back({INVALID_CHARACTER, currentCharStr, currentLine});
            currentIndex++;
        }

        currentLine++; // Increment the line counter for the next line
    }

    // Add an end-of-input token
    tokens.push_back({END_OF_INPUT, "", currentLine});

    return tokens;
}

//int main(int argc, char* argv[]) {
//    if (argc != 2) {
//        std::cerr << "Usage: " << argv[0] << " <input_file>\n";
//        return 1;
//    }
//
//    std::ifstream inputFile(argv[1]);
//    if (!inputFile) {
//        std::cerr << "Error opening input file\n";
//        return 1;
//    }
//
//    std::vector<Token> tokens = tokenize(inputFile);
//
//    for (const Token& token : tokens) {
//        std::cout << "Type: " << token.type << ", Lexeme: " << token.lexeme;
//        std::cout << ", Line: " << token.line;
//
//        // Print values for INTEGER_LITERAL, REAL_LITERAL, and BOOLEAN_LITERAL
//        if (token.type == INTEGER_LITERAL) {
//            std::cout << ", Value: " << token.intValue;
//        } else if (token.type == REAL_LITERAL) {
//            std::cout << ", Value: " << token.realValue;
//        } else if (token.type == BOOLEAN_LITERAL) {
//            std::cout << ", Value: " << (token.boolValue ? "true" : "false");
//        }
//
//        std::cout << std::endl;
//    }
//
//    return 0;
//}
