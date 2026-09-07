#include "cli.h"
#include "instructions/decoding.h"
#include "disassembler.h"
#include "breakpoint.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

static void cmd_run(cpu_t* cpu, memory_t* mem, int8_t argc, char** argv);
static void cmd_step(cpu_t* cpu, memory_t* mem, int8_t argc, char** argv);
static void cmd_state(cpu_t* cpu, memory_t* mem, int8_t argc, char** argv);
static void cmd_mem(cpu_t* cpu, memory_t* mem, int8_t argc, char** argv);
static void cmd_write(cpu_t* cpu, memory_t* mem, int8_t argc, char** argv);
static void cmd_reset(cpu_t* cpu, memory_t* mem, int8_t argc, char** argv);
static void cmd_load(cpu_t* cpu, memory_t* mem, int8_t argc, char** argv);
static void cmd_break(cpu_t* cpu, memory_t* mem, int8_t argc, char** argv);
static void cmd_next(cpu_t* cpu, memory_t* mem, int8_t argc, char** argv);
static void cmd_save(cpu_t* cpu, memory_t* mem, int8_t argc, char** argv);
static void cmd_reload(cpu_t* cpu, memory_t* mem, int8_t argc, char** argv);
static void cmd_disassemble(cpu_t* cpu, memory_t* mem, int8_t argc, char** argv);
static void cmd_script(cpu_t* cpu, memory_t* mem, int8_t argc, char** argv);
static void cmd_watch(cpu_t* cpu, memory_t* mem, int8_t argc, char** argv);
static void cmd_stack(cpu_t* cpu, memory_t* mem, int8_t argc, char** argv);

static const command_t commands[] = {
    { "run", cmd_run },
    { "step", cmd_step },
    { "state", cmd_state },
    { "mem", cmd_mem },
    { "write", cmd_write },
    { "reset", cmd_reset },
    { "load", cmd_load },
    { "break", cmd_break },
    { "next", cmd_next },
    { "save", cmd_save },
    { "reload", cmd_reload },
    { "disassemble", cmd_disassemble },
    { "script", cmd_script },
};

static const char* reg_names[32] = {
    "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

static uint8_t dispatch_command(cpu_t* cpu, memory_t* mem, char* buffer) {
        // get argc and argv
        char* argv[MAX_ARGS] = { 0 };
        
        char* token = strtok(buffer, " ");
        int8_t argc = 0;
        
        while (token) {
            argv[argc++] = token;
            token = strtok(NULL, " ");
        }
        
        // dispatch commands
        if (strcmp(buffer, "exit") == 0)
            return 0;
        
        uint8_t found_function = 0;
        for (uint32_t i = 0; i < sizeof(commands) / sizeof(command_t); i++) {
            if (strcmp(buffer, commands[i].name) == 0) {
                commands[i].function(cpu, mem, argc, argv);
                found_function = 1;
                break;
            }
        }

        if (!found_function)
            printf("Did not recognize command: %s\n", argv[0]);

        return 1;
}

static void cmd_script(cpu_t* cpu, memory_t* mem, int8_t argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "help: %s <path>\n", argv[0]);
        return;
    }
    
    errno = 0;
    FILE* script = fopen(argv[1], "r");
    if (!script) {
        perror("fopen");
        return;
    }
    
    char buffer[256];
    uint64_t i = 0;
    while (fgets(buffer, 256, script)) {
        size_t len = strlen(buffer);
        buffer[len - 1] = '\0';
        printf("[%llu] %s\n", i++, buffer);
        
        dispatch_command(cpu, mem, buffer);
    }

    fclose(script);
}

static void cmd_disassemble(cpu_t* cpu, memory_t* mem, int8_t argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "help: %s <address> [lines]\n", argv[0]);
        return;
    }
    
    char* end;
    uint64_t addr = (uint64_t)strtoll(argv[1], &end, 0);

    if (*end != '\0') {
        fprintf(stderr, "help: %s <address> [lines]\n", argv[0]);
        return;
    }

    uint32_t lines = 1;
    if (argc >= 3) {
        char* end;
        lines = (uint32_t)strtol(argv[2], &end, 0);

        if (*end != '\0') {
            fprintf(stderr, "help: %s <address> [lines]\n", argv[0]);
            return;
        }
    }

    for (uint32_t i = 0; i < lines; ++i) {
        char* line = disassemble_line(mem, addr);
        printf("[0x%016llx]: %s\n", addr, line); 
        free(line);
        addr += 4;
    }
}

static void cmd_save(cpu_t* cpu, memory_t* mem, int8_t argc, char** argv) {
    (void)argc;
    (void)argv;

    FILE* save_file = fopen("save.dump", "wb");

    fwrite(mem->mem, sizeof(uint8_t), MEM_SIZE, save_file);

    fclose(save_file);
}

static void cmd_reload(cpu_t* cpu, memory_t* mem, int8_t argc, char** argv) {
    (void)argc;
    (void)argv;

    FILE* save_file = fopen("save.dump", "rb");

    fread(mem->mem, sizeof(uint8_t), MEM_SIZE, save_file);
    cpu_reset(cpu);

    fclose(save_file);
}

static void cmd_next(cpu_t* cpu, memory_t* mem, int8_t argc, char** argv) {
    (void)argc;
    (void)argv;

    uint32_t instruction = cpu_fetch(cpu, mem);
    uint8_t raw_opcode = instruction & 0x7F;
    opcode_t opcode = (opcode_t)raw_opcode; 

    // makes sure current instruction is JAL or JALR and the target regisers are either 1 or 5
    if (opcode == JAL || opcode == JALR) {
        jtype_t decoded = decodeJ(instruction);
        if (decoded.rd == 1 || decoded.rd == 5) {
            uint64_t target_addr = cpu->pc + 4; 
            while (cpu->pc != target_addr)
                cpu_step(cpu, mem);
        }
    }
}

static void cmd_write(cpu_t* cpu, memory_t* mem, int8_t argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "help: %s <address> <value>\n", argv[0]);
        return;
    }

    char* end;
    uint64_t addr = (uint64_t)strtoll(argv[1], &end, 0);

    if (*end != '\0') {
        fprintf(stderr, "help: %s <address> <value>\n", argv[0]);
        return;
    }

    uint64_t value = (uint64_t)strtoll(argv[2], &end, 0);

    if (*end != '\0') {
        fprintf(stderr, "help: %s <address> <value>\n", argv[0]);
        return;
    }

    mem_write64(mem, addr, value);
}

static void cmd_break(cpu_t* cpu, memory_t* mem, int8_t argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "help: %s <address>\n", argv[0]);
        return;
    }
    
    char* end;
    uint64_t addr = (uint64_t)strtoll(argv[1], &end, 0);

    if (*end != '\0') {
        fprintf(stderr, "help: %s <address>\n", argv[0]);
        return;
    }

    add_breakpoint(addr);
}

static void cmd_run(cpu_t* cpu, memory_t* mem, int8_t argc, char** argv) {
    while (1) {
        if (is_breakpoint(cpu->pc)) {
            printf("Breakpoint reached\n");
            uint64_t addr = cpu->pc;
            char* line = disassemble_line(mem, addr);
            printf("[0x%016llx]: %s\n", addr, line);
            free(line);
            return;
        } 
        
        /*
        int32_t addr = watch_point_is_active();
        if (addr != -1) {
            printf("Watch Point activated\n");
            watch_point_reset(addr);
            uint16_t pc = cpu->previous_instruction;
            char* line = disassemble_line(mem, pc);
            printf("[0x%016llx]: %s\n", line);
            free(line);

            return;
        }
        */
            
        cpu_step(cpu, mem);
    }
}

static void cmd_load(cpu_t* cpu, memory_t* mem, int8_t argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "help: %s <path> [address]\n", argv[0]);
        return;
    }
    
    uint64_t addr = MEM_BASE;
    if (argc >= 3) {
        char* end;
        addr = (uint64_t)strtoll(argv[2], &end, 0);

        if (*end != '\0') {
            fprintf(stderr, "help: %s <path> [address]\n", argv[0]);
            return;
        }

        cpu->pc = addr;
    } 
    
    
    errno = 0;
    FILE* file = fopen(argv[1], "rb");
    if (!file) {
        perror("fopen");
        return;
    } 
    
    uint32_t bytes_read;
    uint8_t buffer[1024];
    uint64_t offset = addr - MEM_BASE;
    
    do {
        bytes_read = fread(buffer, sizeof(uint8_t), 1024, file);
        memcpy(mem->mem + offset, buffer, bytes_read);
        offset += bytes_read;
    } while (bytes_read == 1024);

    fclose(file);

    printf("Loaded %s at 0x%016llx\n", argv[1], addr);
}

static void cmd_state(cpu_t* cpu, memory_t* mem, int8_t argc, char** argv) {
    (void)argc;
    (void)argv;

    printf("PC: 0x%016llx\n", cpu->pc);
    printf("Instruction: 0x%08x\t", cpu_fetch(cpu, mem));

    uint64_t addr = cpu->pc;
    char* line = disassemble_line(mem, addr);
    printf("[0x%016llx]: %s\n", addr, line);
    free(line);

    for (uint8_t i = 0; i < 8; i++) {
        printf("x%-2u %-4s: 0x%016llx\t", i, reg_names[i], cpu_read_reg(cpu, i));
        printf("x%-2u %-4s: 0x%016llx\n", i + 8, reg_names[i + 8], cpu_read_reg(cpu, i + 8));
    }

    printf("\n");

    for (uint8_t i = 16; i < 24; i++) {
        printf("x%-2u %-4s: 0x%016llx\t", i, reg_names[i], cpu_read_reg(cpu, i));
        printf("x%-2u %-4s: 0x%016llx\n", i + 8, reg_names[i + 8], cpu_read_reg(cpu, i + 8));
    } 
}

static void cmd_mem(cpu_t* cpu, memory_t* mem, int8_t argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "help: %s <address>\n", argv[0]);
        return;
    }
    
    char* end;
    uint64_t addr = (uint64_t)strtoll(argv[1], &end, 0);

    if (*end != '\0') {
        fprintf(stderr, "help: %s <address>\n", argv[0]);
        return;
    }

    printf("0x%016llx\n", mem_read64(mem, addr));
}

static void cmd_reset(cpu_t* cpu, memory_t* mem, int8_t argc, char** argv) {
    (void)argc;
    (void)argv;

    cpu_reset(cpu);
}

static void cmd_step(cpu_t* cpu, memory_t* mem, int8_t argc, char** argv) {
    uint32_t steps = 1;

    if (argc > 1) {
        char* end; 
        steps = (uint32_t)strtol(argv[1], &end, 0);

        if (*end != '\0') {
            fprintf(stderr, "help: %s [steps]\n", argv[0]);
            return;
        }
    }

    for (uint32_t step = 0; step < steps; ++step) {
        uint64_t addr = cpu->pc;
        char* line = disassemble_line(mem, addr);
        printf("[0x%016llx]: %s\n", addr, line);
        free(line);
        cpu_step(cpu, mem);
    }
}

int8_t cli_run(cpu_t* cpu, memory_t* mem) {
    char buffer[INPUT_SIZE];
    while (1) {
        printf(">> ");
        fflush(stdout);

        errno = 0; 
        void* ret = fgets(buffer, sizeof(buffer), stdin);
        if (!ret) {
            perror("fgets");
            return -1;
        } 
        
        // strip newline
        buffer[strcspn(buffer, "\n")] = '\0';
        if (!dispatch_command(cpu, mem, buffer))
            break;
        
        memset(buffer, 0, sizeof(buffer));
    }

    return 0;
}
