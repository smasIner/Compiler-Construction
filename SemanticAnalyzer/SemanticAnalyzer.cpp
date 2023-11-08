#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <cctype>
#include "SemanticClasses.cpp"


void print(std::string s) {
    std::cout << s << std::endl;
}


class SemanticAnalyzer {
private:
    Tree *result_tree;
    Tree *syntax_tree;
    std::vector<Routine *> routines;
    std::vector<Variable *> variables;
    std::vector<Type *> types;

public:
    SemanticAnalyzer(Tree *syntax_tree) {
        this->syntax_tree = syntax_tree;
        this->result_tree = new Tree("Semantic Tree");

        // Add built-in types
        types.push_back(new Type("integer"));
        types.push_back(new Type("real"));
        types.push_back(new Type("boolean"));
        types.push_back(new Type("array"));
        types.push_back(new Type("record"));
    }

    void printRoutines() {
        for (Routine *routine: routines) {
            std::cout << routine->repr() << std::endl;
        }
    }

    bool checkRoutineExists(const std::string &name) {
        for (const Routine *routine: routines) {
            if (routine->name == name) {
                return true;
            }
        }

        std::cout << "Routine " << name << " does not exist! Terminating..." << std::endl;
        exit(1);
    }

    bool checkVariableExists(const std::string &name) {
        for (const Variable *variable: variables) {
            if (variable->name == name) {
                return true;
            }
        }

        std::cout << "Variable " << name << " does not exist! Terminating..." << std::endl;
        exit(1);
    }

    bool checkTypeExists(const std::string &name) {
        for (const Type *type: types) {
            if (type->name == name) {
                return true;
            }
        }

        std::cout << "Type " << name << " does not exist! Terminating..." << std::endl;
        exit(1);
    }

    Tree *analyze() {
        for (Tree *child: syntax_tree->children) {
            if (child->type == TokenType::ROUTINE) {
                result_tree->children.push_back(new Tree(analyzeRoutineDeclaration(child)->repr()));
            } else if (child->type == TokenType::VAR) {
                result_tree->children.push_back(new Tree(variableToStr(analyzeVariableDeclaration(child, true))));
            } else if (child->type == TokenType::TYPE) {
                result_tree->children.push_back(new Tree(typeToStr(analyzeTypeDeclaration(child))));
            }
        }

//        printRoutines();
        return result_tree;
    }

// -----------------------------------------------------------------------------------
    Variable *analyzeVariableDeclaration(Tree *root, bool global = false) {
        print("var");
        auto *variable = new Variable();
        variable->name = root->name;

        for (Tree *child: root->children) {
            if (child->name == "Type") {
                Type *cand_type = analyzeType(child);
                checkTypeExists(cand_type->name);
                variable->type = cand_type;
            } else if (child->name == "Expression") {
                variable->value = analyzeExpression(child);
            }
        }


        if (global) {
            variables.push_back(variable);
        }
        return variable;
    }

    Type *analyzeTypeDeclaration(Tree *root) {
        print("typedec");
        auto *type = new Type();

        for (Tree *child: root->children) {
            if (child->name == "Type") {
                type = analyzeType(child);
            }
        }

        type->name = root->name;

        types.push_back(type);
        return type;
    }

    Routine *analyzeRoutineDeclaration(Tree *root) {
        print("routine");
        auto *routine = new Routine(root->children[1]->name);

        for (Tree *child: root->children) {
            if (child->name == "Routine Parameters") {
                routine->parameters = analyzeParameters(child);
            } else if (child->name == "Type") {
                routine->return_type = analyzeType(child);
            }
        }

        routines.push_back(routine);
        return routine;
    }

    std::vector<Parameter *> analyzeParameters(Tree *root) {
        std::vector<Parameter *> parameters;

        for (Tree *child: root->children) {
            if (child->name == "Parameter Declaration") {
                auto *param = new Parameter();
                param->name = child->children[0]->name;
                param->type = analyzeType(child->children[2]);
                parameters.push_back(param);
            }
        }

        return parameters;
    }

// -----------------------------------------------------------------------------------
    Type *analyzeType(Tree *root) {
        print("type");
        Type *return_type = new Type();

        if (root->children[0]->name == "Array Type") {
            return_type = analyzeArrayType(root->children[0]);
        } else if (root->children[0]->name == "Record Type") {
            return_type = analyzeRecordType(root->children[0]);
        } else {
            return_type->name = root->children[0]->name;
            return_type->type = return_type->name;
        }

        return return_type;
    }

    Type *analyzeArrayType(Tree *root) {
        print("arraytype");
        auto *type = new Type();
        type->name = "array";
        type->type = "array";

        for (Tree *child: root->children) {
            if (child->name == "Expression") {
                type->array_length = analyzeArrayLength(child);
            } else if (child->name == "Type") {
                type->array_type = analyzeType(child);
            }

        }

        return type;
    }

    int analyzeArrayLength(Tree *root) {
        return 0;
    }

    Type *analyzeRecordType(Tree *root) {
        print("recordtype");
        auto *type = new Type();
        type->name = "record";
        type->type = "record";

        for (Tree *child: root->children) {
            if (child->name == "record" || child->name == "end") {
                continue;
            } else {
                type->record_fields.push_back(analyzeVariableDeclaration(child));
            }
        }

        return type;
    }

// -----------------------------------------------------------------------------------
    Expression *analyzeExpression(Tree *root) {
        print("expr");
        auto *expression = new Expression();

        return expression;
    }
};