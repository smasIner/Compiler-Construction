#include <iostream>
#include <string>
#include <vector>
#include "SemanticClasses.cpp"


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

    bool checkRoutineExists(const std::string &name, bool should_exists) {
        bool exists = false;
        for (const Routine *routine: this->global_context->routines) {
            if (routine->name == name) {
                exists = true;
                break;
            }
        }

        if (should_exists == exists and exists == true or should_exists == exists and exists == false) {
            return true;
        } else if (should_exists == false and exists == true) {
            std::cout << "Routine with name " << name << " is already declared! Terminating..." << std::endl;
        } else if (should_exists == true and exists == false) {
            std::cout << "Routine " << name << " does not exist! Terminating..." << std::endl;
        }
        exit(1);
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

        //TODO
        if (!exists) {
            return nullptr;
        } else {
            std::cout << "Variable with name " << name << " is already declared! Terminating..." << std::endl;
            exit(1);
        }

    }

    Type *checkTypeExists(const std::string &name, bool should_exists, Context *context) {
        bool exists = false;
        if (should_exists) {
            for (Type *type: context->types) {
                if (type->name == name) {
                    return type;//should exists = true; exists = true;
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

        //TODO
        if (!exists) {
            return nullptr;
        } else {
            std::cout << "Type with name " << name << " is already declared! Terminating..." << std::endl;
            exit(1);
        }

    }

    Tree *analyze() {
        for (Tree *child: syntax_tree->children) {
            if (child->type == TokenType::ROUTINE) {
                result_tree->children.push_back(new Tree(analyzeRoutineDeclaration(child, global_context)->repr(1)));
            } else if (child->type == TokenType::VAR) {
                result_tree->children.push_back(
                        new Tree(variableToStr(analyzeVariableDeclaration(child, global_context), 1)));
            } else if (child->type == TokenType::TYPE) {
                result_tree->children.push_back(new Tree(typeToStr(analyzeTypeDeclaration(child, global_context), 1)));
            }
        }

        return result_tree;
    }

// -----------------------------------------------------------------------------------
    Variable *analyzeVariableDeclaration(Tree *root, Context *context) {
        print("var");
        checkVariableExists(root->name, false, context);
        auto *variable = new Variable();
        variable->name = root->name;

        for (Tree *child: root->children) {
            if (child->name == "Type") {
                Type *cand_type = analyzeType(child, context);
                variable->type = cand_type;
            } else if (child->name == "Expression") {
                // TODO: check value type
                variable->value = analyzeExpression(child, context);
            }
        }
        context->variables.push_back(variable);

        return variable;
    }

//TODO: id is already declared
    Type *analyzeTypeDeclaration(Tree *root, Context *context) {
        print("typedec");
        checkTypeExists(root->name, false, context);
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
        print("type");
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
                type->array_length = analyzeExpression(child, context);
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
                analyzeVariableDeclaration(child, type->record_context);
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
            } else if (child->type == TokenType::INTEGER_LITERAL) {
                primary->value_type = "integer";
                int value = std::stoi(child->name);
                primary->integer_value = value;
            } else if (child->type == TokenType::REAL_LITERAL) {
                primary->value_type = "real";
                double value = std::stod(child->name);
                primary->real_value = value;
            } else if (child->name == "true") {
                primary->value_type = "boolean";
                primary->boolean_value = true;
            } else if (child->name == "false") {
                primary->value_type = "boolean";
                primary->boolean_value = false;
            }
        }

        return primary;
    }

    // std::string name;
    //    std::string value;
    //    Type *type = nullptr;
    //    std::vector<Expression *> expressions = std::vector<Expression *>();

    ModifiablePrimary *analyzeModifiablePrimary(Tree *root, Context *context) {
        print("modifiableprimary");
        checkVariableExists(root->children[0]->name, true, context);
        auto *modifiable_primary = new ModifiablePrimary();
        //assignment->name = context->findVariable(child->children[0]->name)->name;
        modifiable_primary->name = root->children[0]->name;

        for (Tree *child: root->children) {
            if (child->name == ".") {
                //modifiable_primary->variable = analyzeVariable(child);
            } else if (child->name == "[") {
                //modifiable_primary->expression = analyzeExpression(child);
            }
        }

        return modifiable_primary;
    }

    // -----------------------------------------------------------------------------------
    Body *analyzeBody(Tree *root, Context *context) {
        Body *body = new Body();

        for (Tree *child: root->children) {
            if (child->type == TokenType::VAR) {
                Variable *variable = analyzeVariableDeclaration(child, context->joinContexts(body->context));
                body->context->variables.push_back(variable);
                body->body_parts.push_back(new BodyPart(variable));
            } else if (child->type == TokenType::TYPE) {
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
            if (child->type == TokenType::IDENTIFIER) {
                //for_loop->variable = context->findVariable(child->name);
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

    RoutineCall *analyzeRoutineCall(Tree *root, Context *context) {
        print("routinecall");
        checkRoutineExists(root->children[0]->name, true);
        RoutineCall *routine_call = new RoutineCall();

        for (Tree *child: root->children) {
            if (child->type == TokenType::IDENTIFIER) {
                routine_call->name = child->name;
            } else if (child->name == "Expression") {
                routine_call->expressions.push_back(analyzeExpression(child, context));
            }
        }

        return routine_call;
    }

    Assignment *analyzeAssignment(Tree *root, Context *context) {
        print("assignment");
        Assignment *assignment = new Assignment();
        for (Tree *child: root->children) {
            if (child->name == "Modifiable Primary") {
                //assignment->name = context->findVariable(child->children[0]->name)->name;
                assignment->name = checkVariableExists(child->children[0]->name, true, context)->name;
                assignment->name = child->children[0]->name;
            } else if (child->name == "Expression") {
                // TODO: check value type is same as variable type
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
