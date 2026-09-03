#ifndef INCLUDE_INSTRUCTIONS_JTYPE_H_
#define INCLUDE_INSTRUCTIONS_JTYPE_H_

#include "instructions/decoding.h"
#include "memory.h"

class CPU;

void executeJ(CPU& cpu, Memory& mem, JType instruction);



#endif
