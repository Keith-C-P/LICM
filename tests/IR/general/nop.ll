; ModuleID = 'nop_module'
source_filename = "nop.c"

; Define the format string constant with a null terminator (\00)
@.str_int = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Declare the external C printf function using modern opaque pointers
declare i32 @printf(ptr, ...)

define void @nop() {
    ret void 
}

define i32 @main() {
    %result = call void @nop()
    ret i32 0
}
