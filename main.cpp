//
// Created by smash on 15.10.2023.
//
#include "./LexicalAnalyzer/LexicalAnalyzer.cpp"
#include "./SemanticAnalyzer/SemanticAnalyzer.cpp"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }

    std::ifstream inputFile(argv[1]);
    if (!inputFile) {
        std::cerr << "Error opening input file\n";
        return 1;
    }

    std::cout << "Starting lexical analysis...\n";
    std::vector<Token> tokens = tokenize(inputFile);

    for (const Token& token : tokens) {
        std::cout << "Type: " << token.type << ", Lexeme: " << token.lexeme;
        std::cout << ", Line: " << token.line;

        // Print values for INTEGER_LITERAL, REAL_LITERAL, and BOOLEAN_LITERAL
        if (token.type == INTEGER_LITERAL) {
            std::cout << ", Value: " << token.intValue;
        } else if (token.type == REAL_LITERAL) {
            std::cout << ", Value: " << token.realValue;
        } else if (token.type == BOOLEAN_LITERAL) {
            std::cout << ", Value: " << (token.boolValue ? "true" : "false");
        }

        std::cout << std::endl;
    }

    std::cout << "\nStarting syntax analysis...\n";

    SyntaxAnalyzer syntaxAnalyzer(tokens);
    Tree *syntax_tree = syntaxAnalyzer.parse();
    syntax_tree->print("; ");

    std::cout << "\nStarting semantic analysis...\n";
    SemanticAnalyzer semanticAnalyzer(syntax_tree);
    Tree *semantic_tree = semanticAnalyzer.analyze();
    semantic_tree->print("");

    //true+5=real
    //true+5.5=real
    //bool(true+5.5)=true


    return 0;
}