#pragma once

#include "object.h"
#include "tokenizer.h"

Object* Read(Tokenizer* tokenizer);

Object* ReadQuoted(Tokenizer* tokenizer);

Object* ReadList(Tokenizer* tokenizer);

void PrintToken(Token token);