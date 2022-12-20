export module Runtime.Interpreter;

import<format>;
import Types.Core;
import Safety;
import AST;
import Runtime.Enviroment;
import Runtime.RuntimeValue;
import Runtime.NumberValue;
import Runtime.NullValue;
import Runtime.ObjectValue;

auto Evaluate(Shared<Statement> node, Shared<Enviroment> env) -> Shared<RuntimeValue>;

auto EvalNumericBinaryExpr(Shared<NumberValue> left, Shared<NumberValue> right, String Operator) -> Shared<NumberValue>
{
    float result = 0;

    if (Operator == "+")
    {
        result = left->Value + right->Value;
    }
    else if (Operator == "-")
    {
        result = left->Value - right->Value;
    }
    else if (Operator == "*")
    {
        result = left->Value * right->Value;
    }
    else if (Operator == "/")
    {
        //@todo: division by zero fail-safe
        result = left->Value / right->Value;
    }
    else if (Operator == "%")
    {
        result = (int)left->Value % (int)right->Value;
    }
    else
    {
        Safety::Throw("Tried to evalute an unknown numeric operator!!");
    }

    return std::make_shared<NumberValue>(result);
}

auto EvalBinaryExpr(Shared<BinaryExpr> biexpr, Shared<Enviroment> env) -> Shared<RuntimeValue>
{
    auto left = Evaluate(biexpr->Left, env);
    auto right = Evaluate(biexpr->Right, env);

    if (left->Is<NumberValue>() && right->Is<NumberValue>())
    {
        return EvalNumericBinaryExpr(left->As<NumberValue>(), right->As<NumberValue>(), biexpr->Operator);
    }

    return std::make_shared<NullValue>();
}

auto EvalIdentifier(Shared<Identifier> ident, Shared<Enviroment> env) -> Shared<RuntimeValue>
{
    return env->LookupVar(ident->Name);
}

auto EvalAssignment(Shared<AssignmentExpr> node, Shared<Enviroment> env) -> Shared<RuntimeValue>
{
    if (node->Assigne->Type != ASTNodeType::Identifier)
    {
        Safety::Throw(std::format("Invalid assignment target of {}!", node->ToString().c_str()));
    }

    const auto Name = node->Assigne->As<Identifier>()->Name;

    return env->AssignVar(Name, Evaluate(node->Value, env));
}

auto EvalObjectExpr(Shared<ObjectLiteral> node, Shared<Enviroment> env) -> Shared<RuntimeValue>
{
    auto Object = std::make_shared<ObjectValue>();

    for (auto& Prop : node->Properties)
    {
        auto Value = Prop->Value.has_value() ? Evaluate(Prop->Value.value(), env) : env->LookupVar(Prop->Key);

        Object->Properties.set(Prop->Key, Value);
    }

    return Object;
}

auto EvalProgram(Shared<Program> program, Shared<Enviroment> env) -> Shared<RuntimeValue>
{
    Shared<RuntimeValue> LastEvaluatedValue = std::make_shared<NullValue>();

    for (auto&& stmt : program->Body)
    {
        LastEvaluatedValue = std::move(Evaluate(stmt, env));
    }

    return LastEvaluatedValue;
}

auto EvalVariableDeclaration(Shared<VariableDeclaration> declaration, Shared<Enviroment> env) -> Shared<RuntimeValue>
{
    const auto Value = declaration->Value.has_value()
        ? Evaluate(declaration->Value.value(), env)
        : std::make_shared<NullValue>();

    return env->DeclareVar(declaration->Identifier, Value, declaration->IsConst);
}

export
{
    auto Evaluate(Shared<Statement> node, Shared<Enviroment> env)->Shared<RuntimeValue>
    {
        switch (node->Type)
        {
        case ASTNodeType::NumericLiteral:
        {
            auto Value = node->As<NumericLiteral>()->Value;
            return std::make_shared<NumberValue>(Value);
        }

        case ASTNodeType::Identifier:
        {
            return EvalIdentifier(node->As<Identifier>(), env);
        }

        case ASTNodeType::ObjectLiteral:
        {
            return EvalObjectExpr(node->As<ObjectLiteral>(), env);
        }

        case ASTNodeType::BinaryExpr:
        {
            return EvalBinaryExpr(node->As<BinaryExpr>(), env);
        }

        case ASTNodeType::Program:
        {
            return EvalProgram(node->As<Program>(), env);
        }

        case ASTNodeType::VariableDeclaration:
        {
            return EvalVariableDeclaration(node->As<VariableDeclaration>(), env);
        }

        case ASTNodeType::AssignmentExpr:
        {
            return EvalAssignment(node->As<AssignmentExpr>(), env);
        }

        default:
        {
            return Safety::Throw<Shared<RuntimeValue>>(std::format("Unsupported AST Node {}.", node->ToString()));
        }
        }
    }
}
