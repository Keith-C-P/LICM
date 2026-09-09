; ModuleID = 'nop_module'
source_filename = "nop.c"

define void @nop() {
    ret void 
}

define i32 @main() {
    call void @nop()
    ret i32 0
}
