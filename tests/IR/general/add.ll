; ModuleID = 'add_module'
source_filename = "add.ll"

; Define the format string constant with a newline (\0A) and null terminator (\00) 
@.str_int = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Declare the external C printf function using modern opaque pointers
; IG ... is similar to va_args
declare i32 @printf(ptr, ...)

define i32 @add(i32 %a, i32 %b) {
    ; Add the two input parameters and store the result in an unnamed temporary variable
    %result = add nsw i32 %a, %b
    
    ; Return the result of the addition
    ret i32 %result
}

define i32 @main() {
    ; Call printf passing the pointer to our string constant
    %result = call i32 @add(i32 5, i32 10)
    %call = call i32 (ptr, ...) @printf(ptr @.str_int, i32 %result)
    ret i32 0
}
