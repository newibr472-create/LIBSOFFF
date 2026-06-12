/*
 * core/syscalls.S — Raw ARM64 syscall stub
 * Bypasses libc wrappers entirely — AC hooks on libc can't intercept these
 */

.text
.global raw_syscall
.type raw_syscall, @function

raw_syscall:
    // x0 = syscall number
    // x1-x5 = arguments (shifted)
    mov     x8, x0          // syscall number → x8
    mov     x0, x1          // arg1
    mov     x1, x2          // arg2
    mov     x2, x3          // arg3
    mov     x3, x4          // arg4
    mov     x4, x5          // arg5
    mov     x5, x6          // arg6
    svc     #0              // syscall
    ret

.size raw_syscall, .-raw_syscall
