b gdb_push
commands
eval "add-symbol-file %s -o %p", name, load_addr
c
end

b load_gles2_funcs
r

