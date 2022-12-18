#pragma once
#include "../Values/RuntimeValue.h"
#include "../Values/NullValue.h"
#include "../Interpreter.h"
#include "../../Frontend/AST.h"

auto EvalProgram(ProgramPtr program, EnviromentPtr env) -> RuntimeValuePtr;

auto EvalVariableDeclaration(VariableDeclarationPtr declaration, EnviromentPtr env) -> RuntimeValuePtr;