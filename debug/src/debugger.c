#include <stdio.h>
#include "memory.h"
#include "cpu.h"
#include "cli.h"

int main() {
    cpu_t cpu = cpu_init();
    
    memory_t ram = memory_init();
    if (ram.mem == NULL)
        return -1;
    
    // hard coding reset vectors
    cpu_reset(&cpu);
    
    int8_t ret = cli_run(&cpu, &ram); 

    return ret;
}
