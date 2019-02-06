.global main

.data
str:
    .string "Hello, ASM world!! x = %d\n"

.text
main:
    mov $str, %rdi # 1st argument
    mov $123, %rsi # 2nd agument
    mov $0, %eax   # number of vector registers
    call printf

    mov $0, %eax
    ret
