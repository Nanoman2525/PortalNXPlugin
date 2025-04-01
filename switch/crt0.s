.section ".crt0","ax"
.global __module_start

__module_start:
    .word 0
    .word __nx_mod0 - __module_start

.section ".rodata.mod0"
.global __nx_mod0
__nx_mod0:
    .ascii "MOD0"
    .word  __dynamic_start__    - __nx_mod0
    .word  __bss_start__        - __nx_mod0
    .word  __bss_end__          - __nx_mod0
    .word  0
    .word  0
    .word  __nx_module_runtime  - __nx_mod0

.section ".bss.module_runtime"
__nx_module_runtime:
    .space 0xD0