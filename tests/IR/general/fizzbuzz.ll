; ModuleID = 'fizzbuzz_module'
source_filename = "fizzbuzz.ll"

; Define the format string constant with a newline (\0A) and null terminator (\00) 
@.str_int = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@.str_fizz = private unnamed_addr constant [5 x i8] c"Fizz\00", align 1 ; divisible by 3
@.str_buzz = private unnamed_addr constant [5 x i8] c"Buzz\00", align 1 ; divisible by 5
@.str_newline = private unnamed_addr constant [2 x i8] c"\0A\00", align 1

; Declare the external C printf function using modern opaque pointers
; IG ... is similar to va_args
declare i32 @printf(ptr, ...)

define void @fizzbuzz(i32 %max) {
    entry:
      br label %loop_entry ; br -> branch (since its only 1 label, its uncondiitonal)

    loop_entry:
      %i = phi i32 [1, %entry], [%new.i, %increment] ; phi -> join thing (read the paper)
      %is_gt = icmp sgt i32 %i, %max ; icmp -> integer compare, sgt -> signed greater than
      br i1 %is_gt, label %exit, label %if_div_3

    if_div_3:
      %mod_3 = srem i32 %i, 3 ; srem -> signed remainder
      %is_div_3 = icmp eq i32 %mod_3, 0 ; icmp -> integer compare, eq -> equals
      br i1 %is_div_3, label %print_fizz, label %if_div_5 
      ; br -> branch
      ; i1 -> 1 bit
      ; 1st is "if true"
      ; 2nd is "if false"

    print_fizz:
      %call.0 = call i32 (ptr, ...) @printf(ptr @.str_fizz)
      br label %if_div_5 ; unconditional branch

    if_div_5:
      %printed = phi i1 [0, %if_div_3], [1, %print_fizz]
      %mod_5 = srem i32 %i, 5
      %is_div_5 = icmp eq i32 %mod_5, 0 
      br i1 %is_div_5, label %print_buzz, label %if_printed

    print_buzz:
      %call.1 = call i32 (ptr, ...) @printf(ptr @.str_buzz)
      br label %if_printed

    if_printed:
      %has_printed = phi i1 [%printed, %if_div_5], [1, %print_buzz] 
      br i1 %has_printed, label %increment, label %print_num
    
    print_num:
      %call.2 = call i32 (ptr, ...) @printf(ptr @.str_int, i32 %i)
      br label %increment

    increment:
      %call.3 = call i32 (ptr, ...) @printf(ptr @.str_newline)
      %new.i = add nsw i32 %i, 1
      br label %loop_entry

    exit:
      ret void
}

define i32 @main() {
    call void @fizzbuzz(i32 100)
    ret i32 0
}
