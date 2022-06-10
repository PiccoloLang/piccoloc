
print('======= GENERATING AST BOILERPLATE =======')

ast_types = [
    {
        'name': 'Literal',
        'fields': [
            ['token', 'literal'],
            ['val', 'value', 'used by runtime quoting'],
            ['bool', 'useValue']
        ],
        'exclude_quote_gen': True
    },
    {
        'name': 'Var',
        'fields': [
            ['token', 'name']
        ]
    },
    {
        'name': 'Unary',
        'fields': [
            ['ast*', 'val'],
            ['token', 'op']
        ]
    },
    {
        'name': 'Binary',
        'fields': [
            ['ast*', 'lhs'],
            ['ast*', 'rhs'],
            ['token', 'op']
        ]
    },
    {
        'name': 'Block',
        'fields': [
            ['ast*', 'first']
        ],
        'exclude_quote_gen': True,
        'exclude_varfind_gen': True
    },
    {
        'name': 'Quote',
        'fields': [
            ['ast*', 'first']
        ],
        'exclude_quote_gen': True
    },
    {
        'name': 'Eval',
        'fields': [
            ['ast*', 'expr']
        ]
    },
    {
        'name': 'Inquote',
        'fields': [
            ['ast*', 'expr'],
            ['token', 'op']
        ],
        'exclude_quote_gen': True
    },
    {
        'name': 'VarDecl',
        'fields': [
            ['token', 'name'],
            ['ast*', 'expr']
        ],
        'non_quoted_fields': [
            ['int', 'idx']
        ],
        'exclude_varfind_gen': True
    }
]

# ast.h
with open('src/ast.h', 'w') as f:
    f.write('\n')
    f.write('#ifndef AST_H\n')
    f.write('#define AST_H\n')
    f.write('\n')
    f.write('#include "token.h"\n')
    f.write('#include "value.h"\n')
    f.write('#include "obj.h"\n')
    f.write('#include <stdbool.h>\n')
    f.write('\n')

    f.write('typedef enum {\n')
    for ast in ast_types:
        f.write('\tAST_' + ast['name'].upper() + ',\n')
    f.write('} astType;\n\n')

    f.write('typedef struct ast {\n')
    f.write('\tobj obj; // header used for runtime quoting\n')
    f.write('\tastType type;\n')
    f.write('\tstruct ast* next;\n')
    f.write('\tstruct ast* nextToFree; // used to free nodes at compiletime\n')
    f.write('} ast;\n')

    f.write('\n')
    for ast in ast_types:
        f.write('typedef struct {\n')
        f.write('\tast ast;\n')
        for field in ast['fields'] + (ast['non_quoted_fields'] if 'non_quoted_fields' in ast else []):
            f.write('\t' + field[0] + ' ' + field[1] + ';')
            if len(field) == 3:
                f.write(' // ' + field[2])
            f.write('\n')
        f.write('} ast' + ast['name'] + ';\n\n')

    f.write('#endif\n')
    f.write('\n')

args_per_type = {
    'ast*': 1,
    'token': 4
}

# compiler/builders/quote.c
with open('src/compiler/builders/quote_gen.c', 'w') as f:

    f.write('\n')

    for ast in ast_types:
        
        if not 'exclude_quote_gen' in ast:

            lower_name = ast['name'].lower()
            
            f.write('static LLVMValueRef buildQuoted' + ast['name'] + '(compiler* comp, ast' + ast['name'] + '* ' + lower_name + ') {\n')

            f.write('\treturn buildCall(comp, comp->rtlib->makeQuoted' + ast['name'] + '.type, comp->rtlib->makeQuoted' + ast['name'] + '.func, ')

            num_args = sum(map(lambda field: args_per_type[field[0]], ast['fields']))
            f.write(str(num_args) + '')

            for field in ast['fields']:
                type = field[0]
                name = field[1]
                if type == 'ast*':
                    f.write(',\n\t\tbuildExprQuote(comp, ' + lower_name + '->' + name + ')')
                if type == 'token':
                    f.write(',\n\t\tbuildStrOffset(comp, comp->pkgSrcStr, buildInt(comp, ' + lower_name + '->' + name + '.start - comp->pkg->src))')
                    f.write(',\n\t\tbuildInt(comp, ' + lower_name + '->' + name + '.line)')
                    f.write(',\n\t\tbuildInt(comp, ' + lower_name + '->' + name + '.len)')
                    f.write(',\n\t\tbuildInt(comp, ' + lower_name + '->' + name + '.type)')

            f.write('\n\t);\n')

            f.write('}\n\n')
    
    f.write('\n')

# quote.h

type_to_quote_param = {
    'ast*': ['val'],
    'token': ['const char*', 'int', 'int', 'int']
}

def make_quote_fn_signature(ast):
    result = 'val makeQuoted' + ast['name'] + '('
    put_comma = False # i hate you C :)
    for field in ast['fields']:
        type = field[0]
        i = 0
        for param_type in type_to_quote_param[type]:
            if put_comma:
                result += ', '
            put_comma = True
            result += param_type + ' ' + field[1] + str(i)
            i += 1
    result += ')'
    return result

with open('src/quote_gen.h', 'w') as f:
    f.write('\n')

    for ast in ast_types:
        if not 'exclude_quote_gen' in ast:
            f.write(make_quote_fn_signature(ast) + ';\n')

# quote.c
with open('src/quote_gen.c', 'w') as f:
    f.write('\n')
    
    for ast in ast_types:
        if not 'exclude_quote_gen' in ast:
            f.write(make_quote_fn_signature(ast) + ' {\n')
            f.write('\tast' + ast['name'] + '* quoteObj = ALLOC_AST(' + ast['name'] + ');\n')
            f.write('\tquoteObj->ast.type = AST_' + ast['name'].upper() + ';\n\n')

            for field in ast['fields']:
                type = field[0]
                name = field[1]
                if type == 'ast*':
                    f.write('\tquoteObj->' + name + ' = valToAst(' + name + '0);\n')
                elif type == 'token':
                    f.write('\tquoteObj->' + name + '.start = ' + name + '0;\n')
                    f.write('\tquoteObj->' + name + '.line = ' + name + '1;\n')
                    f.write('\tquoteObj->' + name + '.len = ' + name + '2;\n')
                    f.write('\tquoteObj->' + name + '.type = ' + name + '3;\n')
                f.write('\n')

            f.write('\treturn OBJ_VAL(quoteObj);\n')
            f.write('}\n\n')
    
# eval/quote.c
with open('src/eval/quote_gen.c', 'w') as f:
    
    for ast in ast_types:
        if not 'exclude_quote_gen' in ast:

            f.write('case AST_' + ast['name'].upper() + ': {\n')
            f.write('\tast' + ast['name'] + '* expr = (ast' + ast['name'] + '*)ast;\n')
            f.write('\treturn makeQuoted' + ast['name'] + '(\n')
            put_comma = False
            for field in ast['fields']:
                type = field[0]
                name = field[1]
                if type == 'ast*':
                    if put_comma:
                        f.write(',\n')
                    put_comma = True
                    f.write('\t\tmakeQuote(engine, expr->' + name + ')')
                elif type == 'token':
                    if put_comma:
                        f.write(',\n')
                    put_comma = True
                    f.write('\t\texpr->' + name + '.start,\n')
                    f.write('\t\texpr->' + name + '.line,\n')
                    f.write('\t\texpr->' + name + '.len,\n')
                    f.write('\t\texpr->' + name + '.type')
            f.write('\n\t);\n')

            f.write('}\n')

# compiler/varFind.c
with open('src/compiler/var_find_gen.c', 'w') as f:
    
    for ast in ast_types:
        if not 'exclude_varfind_gen' in ast:
            
            f.write('case AST_' + ast['name'].upper() + ': {\n')
            f.write('\tast' + ast['name'] + '* ast = (ast' + ast['name'] + '*)expr;\n')
            f.write('\t(void)ast;\n')
            for field in ast['fields']:
                type = field[0]
                name = field[1]
                if type == 'ast*':
                    f.write('\tfindVars(comp, ast->' + name + ');\n')
            f.write('\tbreak;\n')
            f.write('}\n')