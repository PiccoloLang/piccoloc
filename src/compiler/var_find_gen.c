case AST_LITERAL: {
	astLiteral* ast = (astLiteral*)expr;
	(void)ast;
	break;
}
case AST_VAR: {
	astVar* ast = (astVar*)expr;
	(void)ast;
	break;
}
case AST_UNARY: {
	astUnary* ast = (astUnary*)expr;
	(void)ast;
	findVars(comp, ast->val);
	break;
}
case AST_BINARY: {
	astBinary* ast = (astBinary*)expr;
	(void)ast;
	findVars(comp, ast->lhs);
	findVars(comp, ast->rhs);
	break;
}
case AST_EVAL: {
	astEval* ast = (astEval*)expr;
	(void)ast;
	findVars(comp, ast->expr);
	break;
}
case AST_INQUOTE: {
	astInquote* ast = (astInquote*)expr;
	(void)ast;
	findVars(comp, ast->expr);
	break;
}
