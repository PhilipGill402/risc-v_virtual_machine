#ifndef INCLUDE_INSTRUCTIONS_UTYPE_H_
#define INCLUDE_INSTRUCTIONS_UTYPE_H_

#include "instructions/decoding.h"

class CPU;

void executeU(CPU& cpu, Memory& mem, UType instruction);

#endif
