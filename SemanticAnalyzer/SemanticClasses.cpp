#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <cctype>


// Definitions
class Type;

struct Parameter;

class Routine;

class Variable;

class Summand;

class Factor;

class Simple;

class Relation;

class Expression;


// Declarations
class Variable {
public:
    std::string name;
    Type *type = nullptr;
    Expression* value = nullptr;

    Variable() = default;

    explicit Variable(std::string name) {
        this->name = std::move(name);
    }
};

class Type {
public:
    std::string type;
    std::string name;
    int array_length = 0;
    Type *array_type = nullptr;
    std::vector<Variable *> record_fields = std::vector<Variable *>();

    Type() = default;

    explicit Type(std::string name) {
        this->name = std::move(name);
    }

    explicit Type(std::string name, std::string type) {
        this->name = std::move(name);
        this->type = std::move(type);
    }
};

class RoutineCall {
public:
    std::string name;
    std::vector<Expression *> expressions = std::vector<Expression *>();
};

class ModifiablePrimary {
public:
    std::string name;
    std::string value;
    Type *type = nullptr;
    std::vector<Expression *> expressions = std::vector<Expression *>();
};

class Primary {
public:
    ModifiablePrimary *modifiable_primary = nullptr;
    RoutineCall *routine_call = nullptr;
    std::string value;
};

class Summand {
public:
    Primary *primary = nullptr;
    Expression *expression = nullptr;
};

class Factor {
public:
    std::vector<Summand *> summands = std::vector<Summand *>();
    std::vector<std::string> operators = std::vector<std::string>();
};

class Simple {
public:
    std::vector<Factor *> factors = std::vector<Factor *>();
    std::vector<std::string> operators = std::vector<std::string>();
};

class Relation {
public:
    std::vector<Simple *> simples = std::vector<Simple *>();
    std::vector<std::string> operators = std::vector<std::string>();
};

class Expression {
public:
    std::vector<Relation *> relations = std::vector<Relation *>();
    std::vector<std::string> operators = std::vector<std::string>();

    std::string repr() {
        return "not implemented";
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

    std::string repr() {
        return name + ": " + type->name;
    }
};

class Routine {
public:
    std::string name;
    Type *return_type = nullptr;
    std::vector<Parameter *> parameters = std::vector<Parameter *>();

    explicit Routine(std::string name) {
        this->name = std::move(name);
    }

    std::string repr() {
        std::string result = "Routine: " + name + "\n";

        if (return_type == nullptr) {
            result += "\tReturn type: void\n";
        } else {
            result += "\tReturn type: " + return_type->name + "\n";
        }

        if (parameters.empty()) {
            result += "\tParameters: none\n";
            return result;
        } else {
            result += "\tParameters:\n";
            for (Parameter *parameter: parameters) {
                result += "\t\t" + parameter->repr() + "\n";
            }
        }

        return result;
    }
};


// Helping functions
std::string typeToStr(Type *type);

std::string variableToStr(Variable *variable) {
    std::string result = "Variable: " + variable->name;
    result += "\t" + typeToStr(variable->type);
    if (variable->value != nullptr) {
        result += "\tValue: " + variable->value->repr();
    }
    return result;
}

std::string typeToStr(Type *type) {
    std::string result = "Type: " + type->name;

    if (!type->type.empty() && (type->type != type->name)) {
        result += "\n\tType: " + type->type;
    }

    if (type->type == "array") {
        result += "[" + std::to_string(type->array_length) + "] ";
        result += type->array_type->name;
    } else if (type->type == "record") {
        result += "\n\tFields:\n";
        for (Variable *field: type->record_fields) {
            result += "\t\t" + variableToStr(field) + "\n";
        }
    }

    return result;
}