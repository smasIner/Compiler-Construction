//
// Created by smash on 15.10.2023.
//
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cctype>

//Tree representation
class Tree {
public:
    std::string op;
    std::string name;
    std::string type;
    int intValue;
    double realValue;
    bool boolValue;
    std::vector<Tree *> children;

    Tree() {
        this->op = "";
        this->name = "";
        this->type = "";
        this->intValue = 0;
        this->realValue = 0;
        this->boolValue = false;
    }

    Tree(std::string name, std::string type) {
        this->op = "";
        this->name = name;
        this->type = type;
        this->intValue = 0;
        this->realValue = 0;
        this->boolValue = false;
    }

    Tree(std::string type) {
        this->op = "";
        this->name = "";
        this->type = type;
        this->intValue = 0;
        this->realValue = 0;
        this->boolValue = false;
    }

    void print(int level = 0) {
        for (int i = 0; i < level; i++) {
            std::cout << "  ";
        }
        if (this->op != "") {
            std::cout << "op: " << this->op << std::endl;
        }
        if (this->name != "") {
            std::cout << "name: " << this->name << std::endl;
        }
        if (this->type != "") {
            std::cout << this->type << std::endl;
        }
        if (this->intValue != 0) {
            std::cout << "intValue: " << this->intValue << std::endl;
        }
        if (this->realValue != 0) {
            std::cout << "realValue: " << this->realValue << std::endl;
        }
        if (this->boolValue) {
            std::cout << "boolValue: " << (this->boolValue ? "true" : "false") << std::endl;
        }
        for (Tree *child : this->children) {
            if (this->type != "")
                child->print(level + 1);
            else child->print(level);
        }
    }
};


class SyntaxAnalyzer {
private:
    std::vector<Token> tokens;
    int currentToken = 0;
    Tree *analyzedTree = new Tree("PROGRAM");

public:
    SyntaxAnalyzer(std::vector<Token> tokens) {
        this->tokens = tokens;
    }

    Tree *parse() {
        return parseDeclaration();
    }

    void checkToken(TokenType expected, Tree *result) {
        if (tokens[currentToken].type != expected) {
            std::cout << "Error: expected " << expected << ", got " << tokens[currentToken].lexeme << std::endl;
            exit(1);
        }
        result->children.push_back((new Tree(tokens[currentToken].lexeme)));
        currentToken++;
    }

    bool parseTypeIfExists(Tree *result) {
        if (tokens[currentToken].type == TokenType::COLON) {
            checkToken(TokenType::COLON, result);
            switch (tokens[currentToken].type) {
                case TokenType::BOOLEAN:
                    checkToken(TokenType::BOOLEAN, result);
                    break;
                case TokenType::INTEGER:
                    checkToken(TokenType::INTEGER, result);
                    break;
                case TokenType::REAL:
                    checkToken(TokenType::REAL, result);
                    break;
                default:
                    std::cout << "Error: expected 'boolean', 'integer' or 'real', got "
                              << tokens[currentToken].lexeme << std::endl;
                    exit(1);
            }
            return true;
        }
        return false;
    }

    void parseInitialValueIfExists(Tree *result) {
        if (tokens[currentToken].type == TokenType::IS) {
            checkToken(TokenType::IS, result);
            result->children.push_back(parseExpression());
        }
    }

    Tree *parseDeclaration() {
        while (true) {
            switch (tokens[currentToken].type) {
                case TokenType::ROUTINE:
                    std::cout << "parse Routine Declaration\n";
                    analyzedTree->children.push_back(parseRoutineDeclaration());
                    break;
                case TokenType::VAR:
                    std::cout << "parse Variable Declaration\n";
                    analyzedTree->children.push_back(parseVariableDeclaration());
                    break;
                case TokenType::TYPE:
                    std::cout << "parse Type Declaration\n";
                    analyzedTree->children.push_back(parseTypeDeclaration());
                    break;
                case TokenType::END_OF_INPUT:
                    std::cout << "parse End of Input\n";
                    return analyzedTree;//это что?
                default:
                    std::cout << "Error: expected 'routine', 'var', 'type' or end of input, got "
                              << tokens[currentToken].lexeme << std::endl;
                    exit(1);
            }
        }
    }

    Tree *parseRoutineDeclaration() {
//        RoutineDeclaration
//        : routine Identifier '(' Parameters ')' [ ':' Type ] is Body end

        Tree *result = new Tree("Routine Declaration");

        checkToken(TokenType::ROUTINE, result);
        checkToken(TokenType::IDENTIFIER, result);
        checkToken(TokenType::LEFT_PAREN, result);
        result->children.push_back(parseRoutineParameters());
        checkToken(TokenType::RIGHT_PAREN, result);
        parseTypeIfExists(result);
        checkToken(TokenType::IS, result);
        result->children.push_back(parseBody(false));
        checkToken(TokenType::END, result);

        return result;
    }

    Tree *parseRoutineParameters() {
//        Parameters
//        : ParameterDeclaration { ',' ParameterDeclaration }
//        ParameterDeclaration
//        : Identifier : Type
        Tree *result = new Tree("Routine Parameters");
        while (tokens[currentToken].type != TokenType::RIGHT_PAREN) {
            result->children.push_back(parseParameterDeclaration());
            if (tokens[currentToken].type == TokenType::COMMA) {
                checkToken(TokenType::COMMA, result);
            }
        }

        return result;

    }

    Tree *parseParameterDeclaration() {
        Tree *result = new Tree("Parameter Declaration");

        checkToken(TokenType::IDENTIFIER, result);
        checkToken(TokenType::COLON, result);
        result->children.push_back(parseType());

        return result;
    }

    Tree *parseBody(bool ifStatement) {
        //Body : { SimpleDeclaration | Statement }
        //SimpleDeclaration : VariableDeclaration | TypeDeclaration

        Tree *result = new Tree("Body");

        while (tokens[currentToken].type != TokenType::END and
               (!ifStatement || tokens[currentToken].type != TokenType::ELSE)) {
            switch (tokens[currentToken].type) {
                case TokenType::VAR:
                    result->children.push_back(parseVariableDeclaration());
                    std::cout << "parse Variable Declaration\n";
                    break;
                case TokenType::TYPE:
                    result->children.push_back(parseTypeDeclaration());
                    std::cout << "parse Type Declaration\n";
                    break;
                default:
                    result->children.push_back(parseStatement());
                    std::cout << "parse Statement\n";
                    break;
            }
        }


        return result;
    }

    Tree *parseVariableDeclaration() {
        Tree *result = new Tree("Variable Declaration");

        checkToken(TokenType::VAR, result);
        checkToken(TokenType::IDENTIFIER, result);

        bool typeExists = parseTypeIfExists(result);

        if (typeExists) {
            parseInitialValueIfExists(result);
        } else {
            checkToken(TokenType::IS, result);
            result->children.push_back(parseExpression());
        }

        return result;
    }

    Tree *parseTypeDeclaration() {
        // TypeDeclaration : type Identifier is Type
        Tree *result = new Tree("TypeDeclaration");

        checkToken(TokenType::TYPE, result);
        checkToken(TokenType::IDENTIFIER, result);
        checkToken(TokenType::IS, result);
        result->children.push_back(parseType());

        return result;
    }

    Tree *parseType() {
        // Type : PrimitiveType | UserType | Identifier
        Tree *result = new Tree("Type");
        // if PrimitiveType
        switch (tokens[currentToken].type) {
            case TokenType::BOOLEAN:
                checkToken(TokenType::BOOLEAN, result);
                break;
            case TokenType::INTEGER:
                checkToken(TokenType::INTEGER, result);
                break;
            case TokenType::REAL:
                checkToken(TokenType::REAL, result);
                break;
            case TokenType::IDENTIFIER:
                checkToken(TokenType::IDENTIFIER, result);
                break;
            default:
                std::cout << "Error: expected 'boolean', 'integer', 'real' or identifier, got "
                          << tokens[currentToken].lexeme << std::endl;
                exit(1);
        }
        //TODO: UserType | Identifier

        return result;
    }

    Tree *parseStatement() {
//        Statement : Assignment | RoutineCall
//                    | WhileLoop | ForLoop
//                    | IfStatement
        Tree *result = new Tree("Statement");

        switch (tokens[currentToken].type) {
            case TokenType::IDENTIFIER:
                currentToken++;
                //RoutineCall
                if (tokens[currentToken].type == TokenType::LEFT_PAREN) {
                    currentToken--;
                    result->children.push_back(parseRoutineCall());
                //Assignment
                } else {
                    currentToken--;
                    result->children.push_back(parseAssignment());
                }
                break;
            //WhileLoop
            case TokenType::WHILE:
                result->children.push_back(parseWhileLoop());
                break;
            //ForLoop
            case TokenType::FOR:
                result->children.push_back(parseForLoop());
                break;
            //IfStatement
            case TokenType::IF:
                result->children.push_back(parseIfStatement());
                break;
            default:
                std::cout << "Error: expected assignment,  routineCall, 'while', 'for', or 'if', got "
                          << tokens[currentToken].lexeme << std::endl;
                exit(1);
        }

        return result;
    }

    Tree *userType() {
        // UserType : ArrayType | RecordType
        Tree *result = new Tree("User Type");
        switch (tokens[currentToken].type) {
            case TokenType::ARRAY:
                result->children.push_back(arrayType());
                break;
            case TokenType::RECORD:
                result->children.push_back(recordType());
                break;
            default:
                std::cout << "Error: expected 'array' or 'record', got "
                          << tokens[currentToken].lexeme << std::endl;
                exit(1);
        }
        return result;
    }

    Tree *recordType() {
        Tree *result = new Tree("Record Type");

        checkToken(TokenType::RECORD, result);
        while (tokens[currentToken].type != TokenType::END) {
            result->children.push_back(parseVariableDeclaration());
        }
        checkToken(TokenType::END, result);

        return result;
    }

    //TODO: может не быть размера, если параметры рутины
    Tree *arrayType() {
        //ArrayType : array '[' [ Expression ] ']' Type
        Tree *result = new Tree("Array Type");
        checkToken(TokenType::ARRAY, result);
        checkToken(TokenType::LEFT_BRACKET, result);
        //может не быть, если параметры рутины
        result->children.push_back(parseExpression());
        checkToken(TokenType::RIGHT_BRACKET, result);
        result->children.push_back(parseType());
        return result;
    }

    Tree *parseAssignment() {
//      Assignment : ModifiablePrimary ':=' Expression
        Tree *result = new Tree("Assignment");
        result->children.push_back(parseModifiablePrimary());
        checkToken(TokenType::ASSIGNMENT, result);
        result->children.push_back(parseExpression());
        return result;
    }

    Tree *parseModifiablePrimary() {
//        ModifiablePrimary
//        : Identifier { . Identifier | [ Expression ] }
        Tree *result = new Tree("Modifiable Primary");

        checkToken(TokenType::IDENTIFIER, result);
        if (tokens[currentToken].type == TokenType::ACCESS_RECORD) {
            checkToken(TokenType::ACCESS_RECORD, result);
            checkToken(TokenType::IDENTIFIER, result);
        } else if (tokens[currentToken].type == TokenType::LEFT_BRACKET) {
            checkToken(TokenType::LEFT_BRACKET, result);
            result->children.push_back(parseExpression());
            checkToken(TokenType::RIGHT_BRACKET, result);
        }

        return result;
    }

    Tree *parsePrimary() {
//        Primary : [ Sign | not ] IntegerLiteral
//                    | [ Sign ] RealLiteral
//                    | true
//                    | false
//                    | ModifiablePrimary
//                    | RoutineCall

        Tree *result = new Tree("Primary");

        switch (tokens[currentToken].type) {
            case TokenType::ADD:
            case TokenType::SUBTRACT:
                result->children.push_back(parseSign());
                break;
            case TokenType::NOT:
                checkToken(TokenType::NOT, result);
                break;
            case TokenType::INTEGER_LITERAL:
                checkToken(TokenType::INTEGER_LITERAL, result);
                break;
            case TokenType::REAL_LITERAL:
                checkToken(TokenType::REAL_LITERAL, result);
                break;
            case TokenType::TRUE:
                checkToken(TokenType::TRUE, result);
                break;
            case TokenType::FALSE:
                checkToken(TokenType::FALSE, result);
                break;
            case TokenType::IDENTIFIER:
                result->children.push_back(parseModifiablePrimary());
                break;
            case TokenType::LEFT_PAREN:
                result->children.push_back(parseRoutineCall());
                break;
            default:
                std::cout << "Error: expected sign, 'not', integer literal, real literal, 'true', 'false', identifier or routine call, got "
                          << tokens[currentToken].lexeme << std::endl;
                exit(1);
        }

        return result;
    }

    Tree *parseRoutineCall() {
//        RoutineCall
//          : Identifier [ '(' Expression { ',' Expression } ')' ]
        Tree *result = new Tree("Routine Call");

        checkToken(TokenType::IDENTIFIER, result);
        if (tokens[currentToken].type == TokenType::LEFT_PAREN) {
            checkToken(TokenType::LEFT_PAREN, result);
            result->children.push_back(parseExpression());
            while (tokens[currentToken].type == TokenType::COMMA) {
                checkToken(TokenType::COMMA, result);
                result->children.push_back(parseExpression());
            }
            checkToken(TokenType::RIGHT_PAREN, result);
        }

        return result;
    }

    Tree *parseSign() {
//        Sign : '+' | '-'
        Tree *result = new Tree("Sign");

        switch (tokens[currentToken].type) {
            case TokenType::ADD:
                checkToken(TokenType::ADD, result);
                break;
            case TokenType::SUBTRACT:
                checkToken(TokenType::SUBTRACT, result);
                break;
            default:
                std::cout << "Error: expected '+' or '-', got "
                          << tokens[currentToken].lexeme << std::endl;
                exit(1);
        }

        return result;
    }

    Tree *parseIfStatement() {
        //IfStatement : if Expression then Body [ else Body ] end
        std::cout << "parse If Statement\n";
        Tree *result = new Tree("If Statement");
        checkToken(TokenType::IF, result);
        result->children.push_back(parseExpression());
        checkToken(TokenType::THEN, result);
        result->children.push_back(parseBody(true));
        if (tokens[currentToken].type == TokenType::ELSE) {
            checkToken(TokenType::ELSE, result);
            result->children.push_back(parseBody(false));
        }
        checkToken(TokenType::END, result);
        return result;
    }

    Tree *parseWhileLoop() {
        // WhileLoop : while Expression loop Body end
        std::cout << "parse While Loop\n";

        Tree *result = new Tree("while Loop");

        checkToken(TokenType::WHILE, result);
        result->children.push_back(parseExpression());
        checkToken(TokenType::LOOP, result);
        result->children.push_back(parseBody(false));
        checkToken(TokenType::END, result);

        return result;
    }

    Tree *parseExpression() {
        //    Expression : Relation { ( and | or | xor ) Relation } Relation : Simple
        std::cout << "parse Expression\n";

        Tree *result = new Tree("Expression");

        result->children.push_back(parseRelation());
        while (tokens[currentToken].type == TokenType::AND ||
               tokens[currentToken].type == TokenType::OR ||
               tokens[currentToken].type == TokenType::XOR) {
            switch (tokens[currentToken].type) {
                case TokenType::AND:
                    checkToken(TokenType::AND, result);
                    break;
                case TokenType::OR:
                    checkToken(TokenType::OR, result);
                    break;
                case TokenType::XOR:
                    checkToken(TokenType::XOR, result);
                    break;
                default:
                    std::cout << "Error: expected 'and', 'or' or 'xor', got "
                              << tokens[currentToken].lexeme << std::endl;
                    exit(1);
            }
            result->children.push_back(parseRelation());
        }
        return result;
    };

    Tree *parseRelation () {
        //Relation : Simple [ ( '<' | '<=' | '>' | '>=' | '=' | '/=' ) Simple ]
        std::cout << "parse Relation\n";

        Tree *result = new Tree("Relation");

        result->children.push_back(parseSimple());
        if (tokens[currentToken].type == TokenType::LESS_THAN ||
            tokens[currentToken].type == TokenType::LESS_THAN_OR_EQUAL ||
            tokens[currentToken].type == TokenType::GREATER_THAN ||
            tokens[currentToken].type == TokenType::GREATER_THAN_OR_EQUAL ||
            tokens[currentToken].type == TokenType::EQUALS ||
            tokens[currentToken].type == TokenType::NOT_EQUALS) {
            switch (tokens[currentToken].type) {
                case TokenType::LESS_THAN:
                    checkToken(TokenType::LESS_THAN, result);
                    break;
                case TokenType::LESS_THAN_OR_EQUAL:
                    checkToken(TokenType::LESS_THAN_OR_EQUAL, result);
                    break;
                case TokenType::GREATER_THAN:
                    checkToken(TokenType::GREATER_THAN, result);
                    break;
                case TokenType::GREATER_THAN_OR_EQUAL:
                    checkToken(TokenType::GREATER_THAN_OR_EQUAL, result);
                    break;
                case TokenType::EQUALS:
                    checkToken(TokenType::EQUALS, result);
                    break;
                case TokenType::NOT_EQUALS:
                    checkToken(TokenType::NOT_EQUALS, result);
                    break;
                default:
                    std::cout << "Error: expected '<', '<=', '>', '>=', '=' or '/=', got "
                              << tokens[currentToken].lexeme << std::endl;
                    exit(1);
            }
            result->children.push_back(parseSimple());
        }

        return result;
    }

    Tree *parseSimple() {
        //Simple : Factor { ( '*' | '/' | '%' ) Factor }

        std::cout << "parse Simple\n";

        Tree *result = new Tree("Simple");

        result->children.push_back(parseFactor());
        while (tokens[currentToken].type == TokenType::MULTIPLY ||
               tokens[currentToken].type == TokenType::DIVIDE ||
               tokens[currentToken].type == TokenType::MODULO) {
            switch (tokens[currentToken].type) {
                case TokenType::MULTIPLY:
                    checkToken(TokenType::MULTIPLY, result);
                    break;
                case TokenType::DIVIDE:
                    checkToken(TokenType::DIVIDE, result);
                    break;
                case TokenType::MODULO:
                    checkToken(TokenType::MODULO, result);
                    break;
                default:
                    std::cout << "Error: expected '*', '/' or '%', got "
                              << tokens[currentToken].lexeme << std::endl;
                    exit(1);
            }
            result->children.push_back(parseFactor());
        }

        return result;
    }

    Tree *parseFactor() {
        //Factor : Summand { ( '+' | '-' ) Summand }

        std::cout << "parse Factor\n";

        Tree *result = new Tree("Factor");

        result->children.push_back(parseSummand());
        while (tokens[currentToken].type == TokenType::ADD ||
               tokens[currentToken].type == TokenType::SUBTRACT) {
            switch (tokens[currentToken].type) {
                case TokenType::ADD:
                    checkToken(TokenType::ADD, result);
                    break;
                case TokenType::SUBTRACT:
                    checkToken(TokenType::SUBTRACT, result);
                    break;
                default:
                    std::cout << "Error: expected '+' or '-', got "
                              << tokens[currentToken].lexeme << std::endl;
                    exit(1);
            }
            result->children.push_back(parseSummand());
        }

        return result;
    }

    Tree *parseSummand() {
        //Summand : Primary | '(' Expression ')'

        std::cout << "parse Summand\n";

        Tree *result = new Tree("Summand");

        if (tokens[currentToken].type == TokenType::LEFT_PAREN) {
            checkToken(TokenType::LEFT_PAREN, result);
            result->children.push_back(parseExpression());
            checkToken(TokenType::RIGHT_PAREN, result);
        } else {
            result->children.push_back(parsePrimary());
        }

        return result;
    }

    Tree *parseRange() {
        //Range : Expression '..' Expression
        std::cout << "parse Range\n";

        Tree *result = new Tree("Range");

        result->children.push_back(parseExpression());
        checkToken(TokenType::RANGE, result);
        result->children.push_back(parseExpression());

        return result;
    };

    Tree *parseForLoop() {
        // ForLoop : for Identifier in [ reverse ] Range loop Body end
        std::cout << "parse For Loop\n";

        Tree *result = new Tree("For Loop");

        checkToken(TokenType::FOR, result);
        checkToken(TokenType::IDENTIFIER, result);
        checkToken(TokenType::IN, result);
        if (tokens[currentToken].type == TokenType::REVERSE) {
            checkToken(TokenType::REVERSE, result);
        }
        result->children.push_back(parseRange());
        checkToken(TokenType::LOOP, result);
        result->children.push_back(parseBody(false));
        checkToken(TokenType::END, result);
        return result;
    }
};