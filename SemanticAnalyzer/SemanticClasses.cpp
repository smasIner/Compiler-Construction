#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <cctype>


// Definitions
class Type;

class RoutineCall;

class ModifiablePrimary;

class Primary;

class Parameter;

class Routine;

class Variable;

class Summand;

class Factor;

class Simple;

class Relation;

class Expression;

class Body;

class Range;

class Assignment;

class IfStatement;

class WhileLoop;

class ForLoop;

class Range;

class Body;

class ReturnStatement;

class Context {
public:
    std::vector<Variable *> parameters = std::vector<Variable *>();
    std::vector<Variable *> variables = std::vector<Variable *>();
    std::vector<Type *> types = std::vector<Type *>();
    std::vector<Routine *> routines = std::vector<Routine *>();

    Context() = default;

    Context *joinContexts (Context *other) {
        Context *result = new Context();
        result->parameters = parameters;
        result->variables = variables;
        result->types = types;
        for (Variable *parameter: other->parameters) {
            result->parameters.push_back(parameter);
        }
        for (Variable *variable: other->variables) {
            result->variables.push_back(variable);
        }
        for (Type *type: other->types) {
            result->types.push_back(type);
        }
        return result;
    }

};

class BodyPart {
public:
    Assignment *assignment = nullptr;
    IfStatement *if_statement = nullptr;
    WhileLoop *while_loop = nullptr;
    ForLoop *for_loop = nullptr;
    RoutineCall *routine_call = nullptr;
    Variable *variable = nullptr;
    Type *type = nullptr;
    ReturnStatement *return_statement = nullptr;

    BodyPart() = default;

    explicit BodyPart(Assignment *assignment) {
        this->assignment = assignment;
    }

    explicit BodyPart(IfStatement *if_statement) {
        this->if_statement = if_statement;
    }

    explicit BodyPart(WhileLoop *while_loop) {
        this->while_loop = while_loop;
    }

    explicit BodyPart(ForLoop *for_loop) {
        this->for_loop = for_loop;
    }

    explicit BodyPart(RoutineCall *routine_call) {
        this->routine_call = routine_call;
    }

    explicit BodyPart(Variable *variable) {
        this->variable = variable;
    }

    explicit BodyPart(Type *type) {
        this->type = type;
    }

    explicit BodyPart(ReturnStatement *return_statement) {
        this->return_statement = return_statement;
    }

};

class ModifiablePrimaryPart {
public:
    Variable *variable = nullptr;
    Expression *expression = nullptr;

    ModifiablePrimaryPart() = default;

    explicit ModifiablePrimaryPart(Variable *variable) {
        this->variable = variable;
    }

    explicit ModifiablePrimaryPart(Expression *expression) {
        this->expression = expression;
    }
};

std::string variableToStr(Variable *variable, int num);

std::string typeToStr(Type *type, int num);

std::string exprToStr(Expression *expr, int num);

std::string bodyToStr(Body *body, int num);

// Declarations
class Variable : public BodyPart {
public:
    std::string name;
    Type *type = nullptr;
    Expression *value = nullptr;

    Variable() = default;

    explicit Variable(std::string name) {
        this->name = std::move(name);
    }

    std::string repr(int num) {
        return variableToStr(this, num);
    }
};

class Type : public BodyPart {
public:
    std::string type;
    std::string name;
    Expression *array_length = nullptr;
    Type *array_type = nullptr;
    Context *record_context = new Context();
    //std::vector<Variable *> record_fields = std::vector<Variable *>();

    Type() = default;

    explicit Type(std::string name) {
        this->name = std::move(name);
    }

    explicit Type(std::string name, std::string type) {
        this->name = std::move(name);
        this->type = std::move(type);
    }

    std::string repr(int num) {
        return typeToStr(this, num);
    }
};

class RoutineCall : public BodyPart {
public:
    std::string name;
    std::vector<Expression *> expressions = std::vector<Expression *>();

    std::string repr(int num) {
        std::string result = "\n" + std::string(num, '\t') + "RoutineCall: " + name + "\n";
        result += std::string(num + 1, '\t') + "Arguments:\n";
        for (Expression *expression: expressions) {
            result += std::string(num + 2, '\t') + exprToStr(expression, num + 2);
        }
        result.erase(result.end() - 1);
        return result;
    }
};

class ModifiablePrimary {
public:
    std::string name;
    std::vector<ModifiablePrimaryPart *> modifiable_primary_parts = std::vector<ModifiablePrimaryPart *>();

    std::string repr(int num) {
        std::string result = "\n" + std::string(num, '\t') + "ModifiablePrimary: " + name + "\n";
        for (ModifiablePrimaryPart *modifiable_primary_part: modifiable_primary_parts) {
            if (modifiable_primary_part->variable != nullptr) {
                result += std::string(num + 1, '\t') + modifiable_primary_part->variable->repr(num + 1) + "\n";
            } else if (modifiable_primary_part->expression != nullptr) {
                result += std::string(num + 1, '\t') + exprToStr(modifiable_primary_part->expression, num + 1) + "\n";
            }
        }
        return result;
    }
};

//TODO: убрать "expression", остальное сдвинуть влево (assignment)

class Primary {
public:
    ModifiablePrimary *modifiable_primary = nullptr;
    RoutineCall *routine_call = nullptr;
    int integer_value;
    double real_value;
    bool boolean_value;
    std::string value_type;

    std::string repr(int num) {
        if (value_type == "integer") {
            return std::to_string(integer_value);
        } else if (value_type == "real") {
            return std::to_string(real_value);
        } else if (value_type == "boolean") {
            if (boolean_value == true) {
                return "true";
            } else {
                return "false";
            }
        } else if (modifiable_primary != nullptr) {
            return modifiable_primary->repr(num);
        } else if (routine_call != nullptr) {
            return routine_call->repr(num);
        }
    }

};

class Summand {
public:
    Primary *primary = nullptr;
    Expression *expression = nullptr;

    std::string repr(int num) {
        if (primary != nullptr) {
            return primary->repr(num);
        } else {
            return exprToStr(expression, num);
        }
    }
};

class Factor {
public:
    std::vector<Summand *> summands = std::vector<Summand *>();
    std::vector<std::string> operators = std::vector<std::string>();

    std::string repr(int num) {
        if (summands.size() == 1) {
            return summands[0]->repr(num);
        } else {
//            Summand *result = new Summand();
//            for (int i=0; i<summands.size()-1; i++) {
//                if (operators[0] == "+") {
//                    result = addition (summands[0], summands[1]);
//                } else {
//                    //subtraction (summands[0], summands[1]);
//                }
//                summands.erase(summands.begin());
//                operators.erase(operators.begin());
//                summands[0]=result;
//            }
//            return summands[0]->repr(num);
        }
    }

//    Summand *addition (Summand *summand1, Summand *summand2) {
//        Summand *result = new Summand();
//        if (summand1->primary != nullptr) {
//            if (summand2->primary != nullptr) {
//                result->primary->value_type = "integer";
//                result->primary->integer_value = summand1->primary->integer_value + summand2->primary->integer_value;
//            } else {
//                result->primary->value_type = "integer";
//                result->primary->integer_value = summand1->primary->integer_value + summand2->expression->integer_value;
//            }
//        } else {
//            if (summand2->primary != nullptr) {
//                result->primary->value_type = "integer";
//                result->primary->integer_value = summand1->expression->integer_value + summand2->primary->integer_value;
//            } else {
//                result->primary->value_type = "integer";
//                result->primary->integer_value = summand1->expression->integer_value + summand2->expression->integer_value;
//            }
//        }
//        return result;
//    }

//my addition:
//    Summand *addition (Summand *summand1, Summand *summand2) {
//        Summand *result = new Summand();
//        if (summand1->primary->value_type == summand2->primary->value_type
//        and summand2->primary->value_type == "integer") {
//            result->primary->value_type = "integer";
//            result->primary->integer_value = summand1->primary->integer_value+summand2->primary->integer_value;
//            return result;
//        }
//    }
};

class Simple {
public:
    std::vector<Factor *> factors = std::vector<Factor *>();
    std::vector<std::string> operators = std::vector<std::string>();

    std::string repr(int num) {
        if (factors.size() == 1) {
            return factors[0]->repr(num);
        } else {
            return "not implemented";
        }
    }
};

class Relation {
public:
    std::vector<Simple *> simples = std::vector<Simple *>();
    std::vector<std::string> operators = std::vector<std::string>();

    std::string repr(int num) {
        if (simples.size() == 1) {
            return simples[0]->repr(num);
        } else {
            return "not implemented";
        }
    }
};

class Expression {
public:
    std::vector<Relation *> relations = std::vector<Relation *>();
    std::vector<std::string> operators = std::vector<std::string>();

    std::string repr(int num) {
        if (relations.size() == 1) {
            return relations[0]->repr(num);
        }
//        std::string represesntation = "";
//        for (Relation *relation: relations) {
//            represesntation += relation-;
//        }
        else {
            return "not implemented";
        }
    }
};

class ReturnStatement {
public:
    Expression *value = nullptr;

    std::string repr(int num) {
        return "\n" + std::string(num, '\t') + "Return Statement: " + value->repr(num);
    }
};

class Parameter {
public:
    std::string name;
    Type *type = nullptr;

    Parameter() = default;

    explicit Parameter(std::string name) {
        this->name = std::move(name);
    }

    std::string repr(int num) {
        return "\n" + std::string(num, '\t') + name + ": " + type->name;
    }
};

class Assignment : public BodyPart {
public:
    std::string name;
    Expression *value = nullptr;

    std::string repr(int num) {
        std::string result =
                "\n" + std::string(num, '\t') + "Assignment:\n" + std::string(num + 1, '\t') +
                "Name: " + name + "\n" + std::string(num + 1, '\t') +
                "Expression: " + value->repr(num + 2);
        return result;
    }
};

class WhileLoop : public BodyPart {
public:
    Expression *condition = nullptr;
    Body *body = nullptr;

    std::string repr(int num) {
        std::string result = "\nWhile loop:";
        result += "\n" + std::string(num, '\t') + "Condition: " + condition->repr(num + 1);
        result += "\n" + std::string(num, '\t') + "Body: " + bodyToStr(body, num + 2);
        return result;
    }
};

class Range {
public:
    Expression *start = nullptr;
    Expression *end = nullptr;

    std::string repr(int num) {
        return start->repr(num + 1) + " .. " + end->repr(num + 1);
    }
};

class ForLoop : public BodyPart {
public:
    Variable *variable = nullptr;
    Range *range = nullptr;
    Body *body = nullptr;

    std::string repr(int num) {
        std::string result = "\n" + std::string(num, '\t') + "For loop:";
        result += "\n" + std::string(num + 1, '\t') + "Identifier: " + variable->name;
        result += "\n" + std::string(num + 1, '\t') + "Range: " + range->repr(num + 1);
        result += "\n" + std::string(num + 1, '\t') + "Body: " + bodyToStr(body, num + 2);
        return result;
    }
};

class IfStatement : public BodyPart {
public:
    Expression *condition = nullptr;
    Body *then_body = nullptr;
    Body *else_body = nullptr;

    std::string repr(int num) {
        std::string result = "\n" + std::string(num, '\t') + "If statement:";
        result += "\n" + std::string(num + 1, '\t') + "Condition: " + condition->repr(num + 1);
        result += "\n" + std::string(num + 1, '\t') + "If body: " + bodyToStr(then_body, num + 2);
        if (else_body != nullptr) {
            result += "\n" + std::string(num + 1, '\t') + "Else body: " + bodyToStr(else_body, num + 2);
        }
        return result;
    }
};

class Body {
public:
    std::vector<BodyPart *> body_parts = std::vector<BodyPart *>();
    Context *context = new Context();

//    Assignment
//    RoutineCall
//    WhileLoop
//    ForLoop
//    IfStatement
//    Variable
//    Type

    std::string repr(int num) {
        return "\n" + bodyToStr(this, num);
    }
};

class Routine {
public:
    std::string name;
    Type *return_type = nullptr;
    std::vector<Variable *> parameters;
    Body *body = nullptr;


    explicit Routine(std::string name) {
        this->name = std::move(name);
    }

    std::string repr(int num) {
        std::string result = std::string(num, '\t') + "Routine: " + name;

        if (return_type == nullptr) {
            result += "\n" + std::string(num + 1, '\t') + "Return type: void";
        } else {
            result += "\n" + std::string(num + 1, '\t') + "Return type: " + return_type->name;
        }

        if (parameters.empty()) {
            result += "\n" + std::string(num + 1, '\t') + "Parameters: none";
        } else {
            result += "\n" + std::string(num + 1, '\t') + "Parameters:";
            for (Variable *parameter: parameters) {
                result += parameter->repr(num + 2);
            }
        }

        if (body != nullptr) {
            result += "\n" + std::string(num + 1, '\t') + "Body: " + body->repr(num + 2);
        } else {
            result += "\n" + std::string(num + 1, '\t') + "Body: none";
        }

        return result;
    }
};

// Helping functions

std::string variableToStr(Variable *variable, int num) {
    std::string result = std::string(num, '\t') + "Variable: " + variable->name;
    result += typeToStr(variable->type, num + 1);
    if (variable->value != nullptr) {
        result += "\n" + std::string(num + 1, '\t') + "Value: " + variable->value->repr(num + 1);
    }
    return result;
}

std::string typeToStr(Type *type, int num) {
    std::string result = "\n" + std::string(num, '\t') + "Type: " + type->name;

    if (!type->type.empty() && (type->type != type->name)) {
        result += "\n" + std::string(num + 1, '\t') + "Type: " + type->type;
    }

    if (type->type == "array") {
        result += "[" + (type->array_length == nullptr ? " " : type->array_length->repr(num + 1)) + "] " +
                  type->array_type->name;
    } else if (type->type == "record") {
        result += "\n" + std::string(num + 1, '\t') + "Fields:\n";
        for (Variable *field: type->record_context->variables) {
            result += variableToStr(field, num + 2) + "\n";
        }
    }

    return result;
}

std::string exprToStr(Expression *expr, int num) {
    std::string result = "Expression: " + expr->repr(num + 1);
    return result;
}

std::string bodyToStr(Body *body, int num) {
    std::string result = "";
    for (BodyPart *body_part: body->body_parts) {
        if (body_part->assignment != nullptr) {
            result += body_part->assignment->repr(num);
        } else if (body_part->routine_call != nullptr) {
            result += body_part->routine_call->repr(num);
        } else if (body_part->while_loop != nullptr) {
            result += body_part->while_loop->repr(num);
        } else if (body_part->for_loop != nullptr) {
            result += body_part->for_loop->repr(num);
        } else if (body_part->if_statement != nullptr) {
            result += body_part->if_statement->repr(num);
        } else if (body_part->variable != nullptr) {
            result += body_part->variable->repr(num);
        } else if (body_part->type != nullptr) {
            result += body_part->type->repr(num);
        } else if (body_part->return_statement != nullptr) {
            result += body_part->return_statement->repr(num);
        }

    }

    return result;
}
