#!/usr/bin/python3
#But keep it working with python2 if possible please.

from ctypes import *
import clang.enumerations
import clang.cindex
import sys
import glob
import sys

arch = ""
target_platform = ""
build_path = ""
includes = ["-I./", "-I./loader", "-I./thunks"]

def display_diagnostic(diag):
    severity = "Error" if diag.severity >= 3 else "Warning"
    if diag.location.file:
        print(f"{severity}: {diag.location.file.name}:{diag.location.line}: {diag.spelling}")
    else:
        print(f"{severity}: {diag.spelling}")


def handle_diagnostics(diags):
    should_error = False
    for diag in diags:
        display_diagnostic(diag)
        if diag.severity >= 3:
            should_error = True

    if should_error:
        print("Fatal error processing file.")
        exit(-1)

def set_arch(new_arch):
    global arch, target_platform, build_path

    arch=new_arch
    target_platform="--target={arch}".format(arch=arch)
    build_path="build/{arch}/".format(arch=arch)

conf = clang.cindex.conf
impl = {}
avail = []
variadic = []

ambiguous = []
c_protos = []

class ExceptionSpecificationKind(clang.cindex.BaseEnumeration):
    """
    An ExceptionSpecificationKind describes the kind of exception specification
    that a function has.
    """

    # The required BaseEnumeration declarations.
    _kinds = []
    _name_map = None

    def __repr__(self):
        return 'ExceptionSpecificationKind.{}'.format(self.name)

ExceptionSpecificationKind.NONE = ExceptionSpecificationKind(0)
ExceptionSpecificationKind.DYNAMIC_NONE = ExceptionSpecificationKind(1)
ExceptionSpecificationKind.DYNAMIC = ExceptionSpecificationKind(2)
ExceptionSpecificationKind.MS_ANY = ExceptionSpecificationKind(3)
ExceptionSpecificationKind.BASIC_NOEXCEPT = ExceptionSpecificationKind(4)
ExceptionSpecificationKind.COMPUTED_NOEXCEPT = ExceptionSpecificationKind(5)
ExceptionSpecificationKind.UNEVALUATED = ExceptionSpecificationKind(6)
ExceptionSpecificationKind.UNINSTANTIATED = ExceptionSpecificationKind(7)
ExceptionSpecificationKind.UNPARSED = ExceptionSpecificationKind(8)


def get_node_text(node):
    start = node.extent.start.offset
    end = node.extent.end.offset
    with open(node.extent.start.file.name) as source_file:
        source_code = source_file.read()
        return source_code[start:end]

def is_variadic(cursor):
    return cursor.type.kind == clang.cindex.TypeKind.FUNCTIONPROTO and cursor.type.is_function_variadic()

def get_exception_specification_kind(this):
    """
    Return the kind of the exception specification; a value from
    the ExceptionSpecificationKind enumeration.
    """
    return ExceptionSpecificationKind.from_id(
            conf.lib.clang_getExceptionSpecificationType(this))

def except_type(node):
    exc_type = get_exception_specification_kind(node)
    if exc_type == ExceptionSpecificationKind.NONE: return ""
    if exc_type == ExceptionSpecificationKind.DYNAMIC_NONE: return " throw()"
    if exc_type == ExceptionSpecificationKind.BASIC_NOEXCEPT: return " noexcept"
    return ""

# Traverse the AST (Abstract Syntax Tree) to extract function signatures
def extract_function_signatures(node):
    if node.kind == clang.cindex.CursorKind.FUNCTION_DECL:
        # Get the function name
        function_name = node.spelling
        if function_name in ambiguous:
            return

        # Get the function signature and print it as an extern symbol
        return_type = node.result_type.spelling

        # Get and print function arguments
        arguments = [p for p in node.get_children() if p.kind == clang.cindex.CursorKind.PARM_DECL]
        argument_list = ', '.join(["{}".format(arg.type.spelling) for arg in arguments])

        var="// " if is_variadic(node) else ""
        exc = except_type(node.type)

        # Print the complete function signature
        # print("{}extern {} {}({}){};".format(var, return_type, function_name, argument_list, exc))
        if node.type.get_num_template_arguments() > 0:
            ambiguous.append(function_name)

        if function_name in avail:
            avail.remove(function_name)
            ambiguous.append(function_name)

        if not is_variadic(node):
            avail.append(function_name)
        else:
            variadic.append(function_name)
            ambiguous.append(function_name)

    for child in node.get_children():
        extract_function_signatures(child)

def is_extern_c(func_decl):
    node = func_decl
    while node is not None:
        if node.kind == clang.cindex.CursorKind.LINKAGE_SPEC:
            print("found linkage {}".format(node))
            if node.displayname == 'extern "C"':
                return True
        node = node.semantic_parent
    return False

def extract_impl_functions(node):
    if node.kind == clang.cindex.CursorKind.FUNCTION_DECL:
        # Get the function signature and print it as an extern symbol
        return_type = node.result_type.spelling

        # Get the function name
        function_name = node.spelling

        # Get and print function arguments
        arguments = [p for p in node.get_children() if p.kind == clang.cindex.CursorKind.PARM_DECL]
        argument_list = ', '.join(["{}".format(arg.type.spelling) for arg in arguments])

        linkage = 'extern "C"' if function_name == node.mangled_name else "extern"

        # Print the complete function signature
        # print("{}extern {} {}({}){};".format(var, return_type, function_name, argument_list, exc))
        if function_name.endswith("_impl"):
            impl[function_name[:-5]] = {
                "ret": return_type,
                "name": function_name,
                "args": argument_list,
                "va_args": is_variadic(node),
                "linkage": linkage
            }

    for child in node.get_children():
        extract_impl_functions(child)

def extract_c_impl_functions(node):
    if node.kind == clang.cindex.CursorKind.FUNCTION_DECL:
        # Get the function signature and print it as an extern symbol
        return_type = node.result_type.spelling

        # Get the function name
        function_name = node.spelling

        # Get and print function arguments
        arguments = [p for p in node.get_children() if p.kind == clang.cindex.CursorKind.PARM_DECL]
        argument_list = ', '.join(["{}".format(arg.type.spelling) for arg in arguments])

        # Print the complete function signature
        # print("{}extern {} {}({}){};".format(var, return_type, function_name, argument_list, exc))
        if not is_variadic(node):
            c_protos.append(function_name)

    for child in node.get_children():
        extract_c_impl_functions(child)


if __name__ == '__main__':
    if sys.argv[1]:
        set_arch(sys.argv[1])

    print("{call}: settings:".format(call=sys.argv[0]))
    print("# arch={}".format(arch))
    print("# target_platform={}".format(target_platform))
    print("# build_path={}".format(build_path))

    if not arch:
        raise Exception("No architecture defined.")

    # clang.cindex.Config.set_library_file("libclang-15.so")
    index = clang.cindex.Index.create()
    
    # Symbols we want to export (e.g. everything on libc.so)
    symtab = []
    symtab_db = open("thunks/libc/symtab", "r")
    for l in symtab_db.readlines():
        if l.startswith("#"):
            continue
        if l not in symtab:
            symtab.append(l[:-1])

    
    # Symbols we don't want to export (e.g. present in places like libm.so etc)
    symtab_excluded = []
    symtab_exclude_db = open("thunks/libc/symtab_exclude", "r")
    for l in symtab_exclude_db.readlines():
        if l.startswith("#"):
            continue
        if l not in symtab:
            symtab_excluded.append(l[:-1])

    for i in glob.glob("thunks/libc/*.cpp"):
        # Ignore the table source
        if "libc_table.cpp" in i:
            continue

        print("Processing {}".format(i))
        translation_unit = index.parse(i, args=["-std=c++17", *includes, target_platform])
        handle_diagnostics(translation_unit.diagnostics)
        extract_impl_functions(translation_unit.cursor)

    # Parse easy exports - things we can thunk directly
    print("Processing thunks/libc/common.hpp")
    translation_unit = index.parse("thunks/libc/common.hpp", args=["-std=c++17", *includes, target_platform])
    handle_diagnostics(translation_unit.diagnostics)
    extract_function_signatures(translation_unit.cursor)

    # Parse more easy exports - those are defined as extern "C" and put into a namespace
    # in order to avoid the overloaded/ambiguous C++ headers.
    print("Processing thunks/libc/c_prototypes.h")
    translation_unit = index.parse("thunks/libc/c_prototypes.h", args=["-std=c11", *includes, "-DIS_TAB_GEN=1", target_platform])
    handle_diagnostics(translation_unit.diagnostics)
    extract_c_impl_functions(translation_unit.cursor)

    # Prepare our thunk list.
    output = []
    for func in symtab:
        # Already present in other libs, e.g. libm.so
        if func in symtab_excluded:
            continue

        if func in impl:
            output.append("NO_THUNK(\"{}\", (uintptr_t)&{}_impl),".format(func, func))
        elif func in c_protos:
            output.append("THUNK_SPECIFIC(\"{name}\", C_FUNCS::{name}),".format(name=func))
        elif func in variadic:
            output.append("// (TODO impl variadic) THUNK_DIRECT({}),".format(func))
        elif func in ambiguous:
            output.append("// (TODO impl ambiguous) THUNK_DIRECT({}),".format(func))
        elif func in avail:
            output.append("THUNK_DIRECT({}),".format(func))
    for func in symtab:
        if func not in avail and func not in variadic and func not in impl and func not in c_protos:
            output.append("// THUNK_MISSING({}),".format(func))
    output = sorted(set(output))

    # Write our thunk table header
    impl_tab = open("{}thunks/libc/impl_tab.h".format(build_path), "w")
    impl_tab.write("// THIS FILE IS AUTOMATICALLY GENERATED\n")
    impl_tab.write("// PLEASE CHECK OUT `generate_libc.py` TO REGENERATE IT\n")
    for l in output:
        impl_tab.write("{}\n".format(l))

    # Write our function signature header
    impl_header = open("{}thunks/libc/impl_header.h".format(build_path), "w")
    impl_header.write("// THIS FILE IS AUTOMATICALLY GENERATED\n")
    impl_header.write("// PLEASE CHECK OUT `generate_libc.py` TO REGENERATE IT\n")
    impl_header.write("\n")
    impl_header.write("#include \"platform.h\"\n")
    for func in impl:
        impl_header.write("{linkage} ABI_ATTR {ret} {name}({args}{va_args});\n".format(
            linkage = impl[func]["linkage"],
            ret = impl[func]["ret"],
            name = impl[func]["name"],
            args = impl[func]["args"],
            va_args = ", ..." if impl[func]["va_args"] else ""))