case AST_VAR: {
	astVar* expr = (astVar*)ast;
	return makeQuotedVar(
		expr->name.start,
		expr->name.line,
		expr->name.len,
		expr->name.type
	);
}
case AST_UNARY: {
	astUnary* expr = (astUnary*)ast;
	return makeQuotedUnary(
		makeQuote(engine, expr->val),
		expr->op.start,
		expr->op.line,
		expr->op.len,
		expr->op.type
	);
}
case AST_BINARY: {
	astBinary* expr = (astBinary*)ast;
	return makeQuotedBinary(
		makeQuote(engine, expr->lhs),
		makeQuote(engine, expr->rhs),
		expr->op.start,
		expr->op.line,
		expr->op.len,
		expr->op.type
	);
}
case AST_EVAL: {
	astEval* expr = (astEval*)ast;
	return makeQuotedEval(
		makeQuote(engine, expr->expr)
	);
}
case AST_VARDECL: {
	astVarDecl* expr = (astVarDecl*)ast;
	return makeQuotedVarDecl(
		expr->name.start,
		expr->name.line,
		expr->name.len,
		expr->name.type,
		makeQuote(engine, expr->expr),
		expr->constant
	);
}
