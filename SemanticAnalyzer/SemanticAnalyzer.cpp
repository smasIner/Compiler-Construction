#include <iostream>
#include <string>
#include <vector>
#include "../SyntaxAnalyzer/SyntaxAnalyzer.cpp"


void print(std::string s) {
    std::cout << s << std::endl;
}


class SemanticAnalyzer {
private:
    Tree *result_tree;
    Tree *syntax_tree;
    Context *global_context;

public:
    SemanticAnalyzer(Tree *syntax_tree) {
        this->syntax_tree = syntax_tree;
        this->result_tree = new Tree("Semantic Tree");
        this->global_context = new Context();

        // Add built-in types
        this->global_context->types.push_back(new Type("integer"));
        this->global_context->types.push_back(new Type("real"));
        this->global_context->types.push_back(new Type("boolean"));
        this->global_context->types.push_back(new Type("array"));
        this->global_context->types.push_back(new Type("record"));
    }

    Routine *checkRoutineExists(const std::string &name, bool should_exists) {
        bool exists = false;
        if (should_exists) {
            for (Routine *routine: this->global_context->routines) {
                if (routine->name == name) {
                    return routine;//should exists = true; exists = true;
                }
            }
            std::cout << "Routine " << name << " is not declared" << std::endl;
            exit(1);
        } else {
            for (Routine *routine: this->global_context->routines) {
                if (routine->name == name) {
                    exists = true;
                    break;
                }
            }

        }

        if (!exists) {
            return nullptr;
        } else {
            std::cout << "Routine with name " << name << " is already declared! Terminating..." << std::endl;
            exit(1);
        }
    }

    Variable *checkVariableExists(const std::string &name, bool should_exists, Context *context) {
        bool exists = false;
        if (should_exists) {
            for (Variable *variable: context->variables) {
                if (variable->name == name) {
                    return variable;//should exists = true; exists = true;
                }
            }
            for (Variable *parameter: context->parameters) {
                if (parameter->name == name) {
                    return parameter;//should exists = true; exists = true;
                }
            }
            std::cout << "Variable " << name << " is not declared" << std::endl;
            exit(1); //should exists = true; exists = false;
        } else {
            for (const Variable *variable: context->variables) {
                if (variable->name == name) {
                    exists = true;
                    break;
                }
            }
        }

        if (!exists) {
            return nullptr;
        } else {
            std::cout << "Variable with name " << name << " is already declared! Terminating..." << std::endl;
            exit(1);
        }

    }

    Type *checkTypeExists(Type *check_type, bool should_exists, Context *context) {
        std::string name = check_type->name;
        bool exists = false;
        if (should_exists) {
            for (Type *type: context->types) {
                if (type->name == name) {
                    return check_type;//should exists = true; exists = true;
                }
            }
            std::cout << "Type " << name << " is not declared" << std::endl;
            exit(1); //should exists = true; exists = false;
        } else {
            for (Type *type: context->types) {
                if (type->name == name) {
                    exists = true;
                    break;
                }
            }
        }

        if (!exists) {
            return nullptr;
        } else {
            std::cout << "Type with name " << name << " is already declared! Terminating..." << std::endl;
            exit(1);
        }

    }

    int checkArraySize(Expression *expression) {
        double result;
        result = expression->calculate();
        if (compile_time == false) {
            std::cout << "Array size must be compile-time" << std::endl;
            exit(1);
        }
        print("ARRAY SIZE: " + std::to_string(result));
        compile_time = true;
        return result;

    }

    int checkArrayIndex(Expression *expression, int array_length) {
        double result;
        result = expression->calculate();
        if (compile_time == true and (array_length < result or result <= 0)) {
            std::cout << "Array index is out of bounds" << std::endl;
            exit(1);
        }

        print("ARRAY INDEX: " + std::to_string(result) + (!compile_time ? " + some variable(s)" : ""));
        compile_time = true;
        return result;
    }


    Tree *analyze() {
        for (Tree *child: syntax_tree->children) {
            if (child->tokenType == TokenType::ROUTINE) {
                result_tree->children.push_back(new Tree(analyzeRoutineDeclaration(child, global_context)));
            } else if (child->tokenType == TokenType::VAR) {
                result_tree->children.push_back(
                        new Tree(analyzeVariableDeclaration(child, global_context)));
            } else if (child->tokenType == TokenType::TYPE) {
                result_tree->children.push_back(new Tree(analyzeTypeDeclaration(child, global_context)));
            }
        }

        return result_tree;
    }

//    Expression *conversion (Expression *value, Type *tokenType) {
//        if (value->tokenType() == tokenType->name) {
//            return value;
//        } else if (value->tokenType() == "integer" and tokenType->name == "real") {
//            return new Expression(std::to_string(std::stod(value->repr(0))));
//
//        } else if (value->tokenType->name == "real" and tokenType->name == "integer") {
//            auto *new_value = new Expression();
//            new_value->tokenType = tokenType;
//            new_value->value = value->value;
//            return new_value;
//        } else {
//            std::cout << "Cannot convert " << value->tokenType->name << " to " << tokenType->name << std::endl;
//            exit(1);
//        }
//    }
//
//    }
//
// -----------------------------------------------------------------------------------
    Variable *analyzeVariableDeclaration(Tree *root, Context *context) {
        print("var");
        checkVariableExists(root->name, false, context);
        auto *variable = new Variable();
        variable->name = root->name;

        for (Tree *child: root->children) {
            if (child->name == "Type") {
                Type *cand_type = checkTypeExists(analyzeType(child, context), true, context);
                variable->type = cand_type;
            } else if (child->name == "Expression") {
                variable->value = analyzeExpression(child, context);
                //variable->value=conversion(variable->value, variable->tokenType);
            }
        }
        context->variables.push_back(variable);

        return variable;
    }

    Type *analyzeTypeDeclaration(Tree *root, Context *context) {
        print("typedec");
        checkTypeExists(new Type(root->name), false, context);
        Type *type = new Type();

        for (Tree *child: root->children) {
            if (child->name == "Type") {
                type = analyzeType(child, context);
            }
        }
        type->name = root->name;

        context->types.push_back(type);
        return type;
    }

    Routine *analyzeRoutineDeclaration(Tree *root, Context *context) {
        print("routine");
        checkRoutineExists(root->children[1]->name, false);
        Routine *routine = new Routine(root->name);
        auto *routine_context = new Context();

        for (Tree *child: root->children) {
            if (child->name == "Routine Parameters") {
                routine->parameters = analyzeParameters(child);
                //routine->parameters[i]->tokenType
                routine_context->parameters = routine->parameters;
            } else if (child->name == "Type") {
                routine->return_type = analyzeType(child, context);
            } else if (child->name == "Body") {
                routine->body = analyzeBody(child, context->joinContexts(routine_context));
            }
        }

        context->routines.push_back(routine);
        return routine;
    }

    std::vector<Variable *> analyzeParameters(Tree *root) {
        std::vector<Variable *> parameters;

        for (Tree *child: root->children) {
            auto *param = new Variable();
            param->name = child->children[0]->name;
            param->type = analyzeType(child->children[2], this->global_context);
            parameters.push_back(param);
        }

        return parameters;
    }

// -----------------------------------------------------------------------------------
    Type *analyzeType(Tree *root, Context *context) {
        print("tokenType");
        Type *type = new Type();

        if (root->children[0]->name == "Array Type") {
            type = analyzeArrayType(root->children[0], context);
        } else if (root->children[0]->name == "Record Type") {
            type = analyzeRecordType(root->children[0], context);
        } else {
            type->name = root->children[0]->name;
            type->type = type->name;
        }

        return type;
    }

    Type *analyzeArrayType(Tree *root, Context *context) {
        print("arraytype");
        auto *type = new Type();
        type->name = "array";
        type->type = "array";

        for (Tree *child: root->children) {
            if (child->name == "Expression") {
                type->array_length_expr = analyzeExpression(child, context);
                type->array_length_int = checkArraySize(type->array_length_expr);
            } else if (child->name == "Type") {
                type->array_type = analyzeType(child, context);
            }

        }

        return type;
    }

    Type *analyzeRecordType(Tree *root, Context *context) {
        print("recordtype");

        Type *type = new Type();
        type->name = "record";
        type->type = "record";

        for (Tree *child: root->children) {
            if (child->name == "record" || child->name == "end") {
                continue;
            } else {
                analyzeVariableDeclaration(child, context->joinContexts(type->record_context));
            }
        }

        return type;
    }

// -----------------------------------------------------------------------------------
    Expression *analyzeExpression(Tree *root, Context *context) {
        print("expr");
        auto *expression = new Expression();

        for (Tree *child: root->children) {
            if (child->name == "Relation") {
                expression->relations.push_back(analyzeRelation(child, context));
            } else {
                expression->operators.push_back(child->name);
            }
        }

        return expression;
    }

    Relation *analyzeRelation(Tree *root, Context *context) {
        print("rel");
        auto *relation = new Relation();

        for (Tree *child: root->children) {
            if (child->name == "Simple") {
                relation->simples.push_back(analyzeSimple(child, context));
            } else {
                relation->operators.push_back(child->name);
            }
        }

        return relation;
    }

    Simple *analyzeSimple(Tree *root, Context *context) {
        print("simple");
        auto *simple = new Simple();

        for (Tree *child: root->children) {
            if (child->name == "Factor") {
                simple->factors.push_back(analyzeFactor(child, context));
            } else {
                simple->operators.push_back(child->name);
            }
        }

        return simple;
    }

    Factor *analyzeFactor(Tree *root, Context *context) {
        print("factor");
        auto *factor = new Factor();

        for (Tree *child: root->children) {
            if (child->name == "Summand") {
                factor->summands.push_back(analyzeSummand(child, context));
            } else {
                factor->operators.push_back(child->name);
            }
        }

        return factor;
    }

    Summand *analyzeSummand(Tree *root, Context *context) {
        print("summand");
        auto *summand = new Summand();

        for (Tree *child: root->children) {
            if (child->name == "Primary") {
                summand->primary = analyzePrimary(child, context);
            } else if (child->name == "Expression") {
                summand->expression = analyzeExpression(child, context);
            }
        }

        return summand;
    }

    Primary *analyzePrimary(Tree *root, Context *context) {
        print("primary");
        auto *primary = new Primary();

        for (Tree *child: root->children) {
            if (child->name == "Modifiable Primary") {
                primary->modifiable_primary = analyzeModifiablePrimary(child, context);
            } else if (child->name == "Routine Call") {
                primary->routine_call = analyzeRoutineCall(child, context);
            } else if (child->tokenType == TokenType::INTEGER_LITERAL) {
                primary->value_type = "integer";
                int value = std::stoi(child->name);
                primary->value = value;
            } else if (child->tokenType == TokenType::REAL_LITERAL) {
                primary->value_type = "real";
                double value = std::stod(child->name);
                primary->value = value;
            } else if (child->name == "true") {
                primary->value_type = "boolean";
                primary->value = true;
            } else if (child->name == "false") {
                primary->value_type = "boolean";
                primary->value = false;
            } else if (child->name == "Sign") {
                primary->sign = (child->children[0]->name == "-" ? -1 : 1);
            }
        }

        return primary;
    }

    ModifiablePrimary *analyzeModifiablePrimary(Tree *root, Context *context) {
        print("modifiableprimary");
        Variable *variable = checkVariableExists(root->children[0]->name, true, context);
        auto *modifiable_primary = new ModifiablePrimary();

        modifiable_primary->name = variable->name;

        //for (Tree *child: root->children) {
        for (int i = 0; i < root->children.size(); i++) {
            if (root->children[i]->name == ".") {
                modifiable_primary->modifiable_primary_parts.push_back(
                        new ModifiablePrimaryPart(analyzeModifiablePrimary(root->children[i + 1], context)));
            } else if (root->children[i]->name == "[") {
                Expression *expr = analyzeExpression(root->children[i + 1], context);
                checkArrayIndex(expr, variable->type->array_length_int);
                modifiable_primary->modifiable_primary_parts.push_back(
                        new ModifiablePrimaryPart(expr));
            }
        }

        return modifiable_primary;
    }

    // -----------------------------------------------------------------------------------
    Body *analyzeBody(Tree *root, Context *context) {
        Body *body = new Body();

        for (Tree *child: root->children) {
            if (child->tokenType == TokenType::VAR) {
                Variable *variable = analyzeVariableDeclaration(child, context->joinContexts(body->context));
                body->context->variables.push_back(variable);
                body->body_parts.push_back(new BodyPart(variable));
            } else if (child->tokenType == TokenType::TYPE) {
                Type *type = analyzeTypeDeclaration(child, context->joinContexts(body->context));
                body->context->types.push_back(type);
                body->body_parts.push_back(new BodyPart(type));
            } else if (child->name == "Statement" and child->children[0]->name == "For Loop") {
                body->body_parts.push_back(
                        new BodyPart(analyzeForLoop(child->children[0], context->joinContexts(body->context))));
            } else if (child->name == "Statement" and child->children[0]->name == "If Statement") {
                body->body_parts.push_back(
                        new BodyPart(analyzeIfStatement(child->children[0], context->joinContexts(body->context))));
            } else if (child->name == "Statement" and child->children[0]->name == "While Loop") {
                body->body_parts.push_back(
                        new BodyPart(analyzeWhileLoop(child->children[0], context->joinContexts(body->context))));
            } else if (child->name == "Statement" and child->children[0]->name == "Routine Call") {
                body->body_parts.push_back(
                        new BodyPart(analyzeRoutineCall(child->children[0], context->joinContexts(body->context))));
            } else if (child->name == "Statement" and child->children[0]->name == "Assignment") {
                body->body_parts.push_back(
                        new BodyPart(analyzeAssignment(child->children[0], context->joinContexts(body->context))));
            } else if (child->name == "Return Statement") {
                body->body_parts.push_back(
                        new BodyPart(analyzeReturnStatement(child, context->joinContexts(body->context))));
            }
        }
        return body;
    }

    Range *analyzeRange(Tree *root, Context *context) {
        Range *range = new Range();
        range->start = analyzeExpression(root->children[0], context);
        range->end = analyzeExpression(root->children[2], context);
        return range;
    }

    ForLoop *analyzeForLoop(Tree *root, Context *context) {
        print("forloop");
        auto *for_loop = new ForLoop();

        for (Tree *child: root->children) {
            if (child->tokenType == TokenType::IDENTIFIER) {
                for_loop->variable = checkVariableExists(child->name, true, context);
            } else if (child->name == "Range") {
                for_loop->range = analyzeRange(child, context);
            } else if (child->name == "Body") {
                for_loop->body = analyzeBody(child, context);
            }
        }

        return for_loop;
    }

    IfStatement *analyzeIfStatement(Tree *root, Context *context) {
        print("ifstatement");
        IfStatement *if_statement = new IfStatement();

        for (int i = 0; i < root->children.size(); i++) {
            if (root->children[i]->name == "Expression") {
                if_statement->condition = analyzeExpression(root->children[i], context);
            } else if (root->children[i]->name == "then") {
                if_statement->then_body = analyzeBody(root->children[i + 1], context);
            } else if (root->children[i]->name == "else") {
                if_statement->else_body = analyzeBody(root->children[i + 1], context);
            }
        }

        return if_statement;
    }

    WhileLoop *analyzeWhileLoop(Tree *root, Context *context) {
        print("whileloop");
        WhileLoop *while_loop = new WhileLoop();

        for (Tree *child: root->children) {
            if (child->name == "Expression") {
                while_loop->condition = analyzeExpression(child, context);
            } else if (child->name == "Body") {
                while_loop->body = analyzeBody(child, context);
            }
        }

        return while_loop;
    }

    void checkParametersArguments(std::vector<Expression *> expressions, std::vector<Variable *> parameters,
                                  std::string name) {
        if (expressions.size() != parameters.size()) {
            std::cout << "Number of parameters and arguments in " << name << " do not match" << std::endl;
            exit(1);
        }
//        for (int i = 0; i < expressions.size(); i++) {
//            if (expressions[i]->tokenType != parameters[i]->tokenType) {
//                std::cout<<"Parameter and argument types in "<<name<<"  do not match"<<std::endl;
//                exit(1);
//            }
//        }
    }

    RoutineCall *analyzeRoutineCall(Tree *root, Context *context) {
        print("routinecall");
        Routine *routine = checkRoutineExists(root->children[0]->name, true);
        RoutineCall *routine_call = new RoutineCall();

        int num_expressions = 0;
        for (Tree *child: root->children) {
            if (child->tokenType == TokenType::IDENTIFIER) {
                routine_call->name = child->name;
            } else if (child->name == "Expression") {
                routine_call->expressions.push_back(analyzeExpression(child, context));
                num_expressions++;
            }
        }

        checkParametersArguments(routine_call->expressions, routine->parameters, routine->name);

        return routine_call;
    }

    Assignment *analyzeAssignment(Tree *root, Context *context) {
        print("assignment");
        Assignment *assignment = new Assignment();
        for (Tree *child: root->children) {
            if (child->name == "Modifiable Primary") {
                Variable *variable = checkVariableExists(child->children[0]->name, true, context);
                assignment->name = variable->name;
                if (variable->type->array_length_int != -1) {
                    Expression *index = analyzeExpression(child->children[2], context);
                    checkArrayIndex(index, variable->type->array_length_int);
                    assignment->index = index;
                }
            } else if (child->name == "Expression") {
                assignment->value = analyzeExpression(child, context);
            }
        }


        return assignment;
    }

    ReturnStatement *analyzeReturnStatement(Tree *root, Context *context) {
        print("returnstatement");
        ReturnStatement *return_statement = new ReturnStatement();

        for (Tree *child: root->children) {
            if (child->name == "Expression") {
                return_statement->value = analyzeExpression(child, context);
            }
        }

        return return_statement;
    }


};
