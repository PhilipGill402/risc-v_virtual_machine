#ifndef INCLUDE_INSTRUCTIONS_BTYPE_H_
#define INCLUDE_INSTRUCTIONS_BTYPE_H_

#include "instructions/decoding.h"
#include "memory.h"

class CPU;

void executeB(CPU& cpu, Memory& mem, BType instruction);

#endif
