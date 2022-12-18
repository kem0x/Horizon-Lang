#pragma once
#include "Enviroment.h"
#include "Evaluation/Expressions.h"
#include "Evaluation/Statements.h"

auto Evaluate(StatementPtr Node, EnviromentPtr env) -> RuntimeValuePtr;