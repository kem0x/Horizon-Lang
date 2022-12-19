#pragma once
#include "../Values/RuntimeValue.h"
#include "../Values/NullValue.h"
#include "../Interpreter.h"
#include "../../Frontend/AST.h"

auto EvalProgram(Shared<Program> program, Shared<Enviroment> env) -> Shared<RuntimeValue>;

auto EvalVariableDeclaration(Shared<VariableDeclaration> declaration, Shared<Enviroment> env) -> Shared<RuntimeValue>;