#ifndef INCLUDE_INSTRUCTIONS_ITYPE_H_
#define INCLUDE_INSTRUCTIONS_ITYPE_H_

#include "instructions/decoding.h"
#include "memory.h"

class CPU;

void executeI(CPU& cpu, Memory& mem, IType instruction);

#endif
