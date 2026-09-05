#include "vm.h"
#include <stdio.h>
#include <string.h>

vm_t vm_init() {
    vm_t vm = { 0 };
    vm.cpu = cpu_init();
    vm.ram = memory_init();
    
    cpu_reset(&vm.cpu); 
    
    return vm;
}

int32_t vm_load_bin(vm_t* vm, const char* fpath) {
    FILE* file = fopen(fpath, "rb");
    uint64_t offset = 0;
    char buffer[256];
    size_t bytes_read;
    
    do {
        bytes_read = fread(buffer, 256, 256, file);
        memcpy(vm->ram.mem + offset, buffer, bytes_read);
        offset += bytes_read;
    } while (bytes_read > 0);

    fclose(file);
}

