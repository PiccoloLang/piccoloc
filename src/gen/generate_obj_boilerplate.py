
print('======= GENERATING OBJ BOILERPLATE =======')

def upper_first_letter(str):
    return str[0].upper() + str[1:]

obj_types = [
    {
        'name': 'Str',
        'fields': [
            ['int', 'len'],
            ['char*', 'str']
        ]
    },
    {
        'name': 'Fn',
        'fields': [
            ['fnType', 'fn'],
            ['int', 'arity'],
            ['void*', 'rtVarEnv']
        ]
    },
    {
        'name': 'RtFn',
        'fields': [
            ['void*', 'fnAst'],
            ['int', 'arity']
        ]
    }
]

# obj.h
with open('src/obj_gen.h', 'w') as f:

    for obj in obj_types:
        f.write('typedef struct {\n')
        f.write('\tobj obj;\n')
        for field in obj['fields']:
            f.write('\t' + field[0] + ' ' + field[1] + ';\n')

        f.write('} obj' + obj['name'] + ';\n')

with open('src/obj_enum_gen.h', 'w') as f:

    f.write('typedef enum {\n');
    for obj in obj_types:
        f.write('\tOBJ_' + obj['name'].upper() + ',\n')
    f.write('\tOBJ_QUOTE\n')
    f.write('} objType;\n')

# types.h
with open('src/compiler/types_obj_gen.h', 'w') as f:

    for obj in obj_types:
        f.write('LLVMTypeRef obj' + obj['name'] + 'Type;\n')
        f.write('LLVMTypeRef obj' + obj['name'] + 'PtrType;\n')

# types.c

llvm_type_names = {
    'bool': 'boolType',
    'int': 'intType',
    'val': 'valType',
    'char*': 'strType',
    'void*': 'voidPtrType',
    'fnType': 'fnPtrType'
}

with open('src/compiler/types_obj_gen.c', 'w') as f:

    for obj in obj_types:
        f.write('t->obj' + obj['name'] + 'Type = LLVMStructType((LLVMTypeRef[]){t->objType')
        for field in obj['fields']:
            f.write(', t->' + llvm_type_names[field[0]])
        f.write('}, ' + str(1 + len(obj['fields'])) + ', false);\n')
        f.write('t->obj' + obj['name'] + 'PtrType = LLVMPointerType(t->obj' + obj['name'] + 'Type, 0);\n')

# compiler/builders/obj_builders.h
with open('src/compiler/builders/obj_builders_gen.h', 'w') as f:
    for obj in obj_types:
        for field in obj['fields']:
            f.write('LLVMValueRef buildObj' + obj['name'] + 'Get' + upper_first_letter(field[1]) + 'Ptr(compiler* comp, LLVMValueRef val);\n')
        f.write('LLVMValueRef buildIs' + obj['name'] + 'Check(compiler* comp, LLVMValueRef val);\n')

# compiler/builders/obj_builders.c
with open('src/compiler/builders/obj_builders_gen.c', 'w') as f:
    
    for obj in obj_types:
        
        i = 0
        for field in obj['fields']:

            f.write('LLVMValueRef buildObj' + obj['name'] + 'Get' + upper_first_letter(field[1]) + 'Ptr(compiler* comp, LLVMValueRef val) {\n')
            f.write('\treturn LLVMBuildGEP2(comp->builder, comp->types->obj' + obj['name'] + 'Type, val, (LLVMValueRef[]){LLVMConstInt(LLVMInt32Type(), 0, false), LLVMConstInt(LLVMInt32Type(), ' + str(i + 1) + ', false)}, 2, "");\n')
            f.write('}\n')

            i += 1
        
        f.write('LLVMValueRef buildIs' + obj['name'] + 'Check(compiler* comp, LLVMValueRef val) {\n')
        f.write('\tLLVMValueRef obj = buildValToPtr(comp, val);\n')
        f.write('\treturn LLVMBuildICmp(comp->builder, LLVMIntEQ, buildGetObjType(comp, obj), LLVMConstInt(LLVMInt8Type(), OBJ_' + obj['name'].upper() + ', false), "");\n')
        f.write('}\n');
            