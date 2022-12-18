#pragma once
#include "../../Error.h"
#include "../../Frontend/AST.h"
#include "../Enviroment.h"

#include "../Values/NumberValue.h"

auto EvalNumericBinaryExpr(NumberValuePtr left, NumberValuePtr right, std::string Operator) -> NumberValuePtr;

auto EvalBinaryExpr(BinaryExprPtr biexpr, EnviromentPtr env) -> RuntimeValuePtr;

auto EvalIdentifier(IdentifierPtr ident, EnviromentPtr env) -> RuntimeValuePtr;

auto EvalAssignment(AssignmentExprPtr node, EnviromentPtr env) -> RuntimeValuePtr;
