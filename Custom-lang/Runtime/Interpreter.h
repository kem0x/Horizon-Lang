#pragma once
#include "Enviroment.h"
#include "Evaluation/Expressions.h"
#include "Evaluation/Statements.h"

auto Evaluate(Shared<Statement> Node, Shared<Enviroment> env) -> Shared<RuntimeValue>;