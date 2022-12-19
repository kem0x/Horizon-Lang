#pragma once
#include "../../Safety.h"
#include "../../Frontend/AST.h"
#include "../Enviroment.h"

#include "../Values/NumberValue.h"
#include "../Values/ObjectValue.h"

auto EvalNumericBinaryExpr(Shared<NumberValue> left, Shared<NumberValue> right, String Operator) -> Shared<NumberValue>;

auto EvalBinaryExpr(Shared<BinaryExpr> biexpr, Shared<Enviroment> env) -> Shared<RuntimeValue>;

auto EvalIdentifier(Shared<Identifier> ident, Shared<Enviroment> env) -> Shared<RuntimeValue>;

auto EvalAssignment(Shared<AssignmentExpr> node, Shared<Enviroment> env) -> Shared<RuntimeValue>;

auto EvalObjectExpr(Shared<ObjectLiteral> node, Shared<Enviroment> env) -> Shared<RuntimeValue>;