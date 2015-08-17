/* r_interface.cpp
 *
 * SimplR - Basic Symbolic Expression Simplification
 * R interface to SimplR's C++ functions.
 * (c) 2015 Oliver Flasch, Felix Gorschlueter, sourcewerk UG
 */

#include "expression.h"

#include <R.h>
#include <Rinternals.h>

#include <string.h>

using namespace ev3;


struct SexpToEv3ExpressionContext {
  int nextVariableIndex;
  map<string, int> variableIndexMap;
};

Expression sexp_to_ev3_expression_rec(SEXP sexp, SexpToEv3ExpressionContext& ctx) {
  //Rprintf("expr type: %d\n", TYPEOF(sexp)); // DEBUG
  switch (TYPEOF(sexp)) {
    // base cases...
    case INTSXP: {
      Expression result = (double) INTEGER(sexp)[0];
      return result;
    }
    case REALSXP: {
      Expression result = REAL(sexp)[0];
      return result;
    }
    case SYMSXP: {
      Expression result;
	    result->SetOpType(VAR);
      string name = string(CHAR(PRINTNAME(sexp)));
      if (ctx.variableIndexMap.find(name) == ctx.variableIndexMap.end()) {
        // create new variable table entry...
        ctx.variableIndexMap[name] = ctx.nextVariableIndex;
        ctx.nextVariableIndex++;
      }
      result->SetVarIndex(ctx.variableIndexMap[name]);
      result->SetVarName(name);
      //Rprintf("created variable '%s' with index %d\n", name.c_str(), ctx.variableIndexMap[name]); // DEBUG
      return result;
    }
    // recursive cases...
    case LANGSXP: {
      const char *function_symbol = CHAR(PRINTNAME(CAR(sexp)));
      if (!strcmp("+", function_symbol)) { // +
        Expression result = sexp_to_ev3_expression_rec(CADR(sexp), ctx)
          + sexp_to_ev3_expression_rec(CADDR(sexp), ctx);
        return result;
      } else if (!strcmp("-", function_symbol) && (2 == length(sexp))) { // unary -
        Expression result = -sexp_to_ev3_expression_rec(CADR(sexp), ctx);
        return result;
      } else if (!strcmp("-", function_symbol) && (3 == length(sexp))) { // binary -
        Expression result = sexp_to_ev3_expression_rec(CADR(sexp), ctx)
          - sexp_to_ev3_expression_rec(CADDR(sexp), ctx);
        return result;
      } else if (!strcmp("*", function_symbol)) { // *
        Expression result = sexp_to_ev3_expression_rec(CADR(sexp), ctx)
          * sexp_to_ev3_expression_rec(CADDR(sexp), ctx);
        return result;
      } else if (!strcmp("/", function_symbol)) { // /
        Expression result = sexp_to_ev3_expression_rec(CADR(sexp), ctx)
          / sexp_to_ev3_expression_rec(CADDR(sexp), ctx);
        return result;
      } else if (!strcmp("^", function_symbol)) { // ^
        Expression result = sexp_to_ev3_expression_rec(CADR(sexp), ctx)
          ^ sexp_to_ev3_expression_rec(CADDR(sexp), ctx);
        return result;
      } else if (!strcmp("log", function_symbol)) { // log
        Expression result = Log(sexp_to_ev3_expression_rec(CADR(sexp), ctx));
        return result;
      } else if (!strcmp("exp", function_symbol)) { // exp 
        Expression result = Exp(sexp_to_ev3_expression_rec(CADR(sexp), ctx));
        return result;
      } else if (!strcmp("sin", function_symbol)) { // sin 
        Expression result = Sin(sexp_to_ev3_expression_rec(CADR(sexp), ctx));
        return result;
      } else if (!strcmp("cos", function_symbol)) { // cos 
        Expression result = Cos(sexp_to_ev3_expression_rec(CADR(sexp), ctx));
        return result;
      } else if (!strcmp("tan", function_symbol)) { // tan 
        Expression result = Tan(sexp_to_ev3_expression_rec(CADR(sexp), ctx));
        return result;
      } else if (!strcmp("sinh", function_symbol)) { // sinh 
        Expression result = Sinh(sexp_to_ev3_expression_rec(CADR(sexp), ctx));
        return result;
      } else if (!strcmp("cosh", function_symbol)) { // cosh 
        Expression result = Cosh(sexp_to_ev3_expression_rec(CADR(sexp), ctx));
        return result;
      } else if (!strcmp("tanh", function_symbol)) { // tanh 
        Expression result = Tanh(sexp_to_ev3_expression_rec(CADR(sexp), ctx));
        return result;
      } else if (!strcmp("sqrt", function_symbol)) { // sqrt 
        Expression result = Sqrt(sexp_to_ev3_expression_rec(CADR(sexp), ctx));
        return result;
      } else if (!strcmp("(", function_symbol)) { // sexp in parens
        Expression result = sexp_to_ev3_expression_rec(CADR(sexp), ctx);
        return result;
      } else {
        list<Expression> arg_list;
        for (SEXP i = CDR(sexp); i != R_NilValue; i = CDR(i)) {
          arg_list.push_back(sexp_to_ev3_expression_rec(CAR(i), ctx));
        }
        Expression result = Other(string(function_symbol), arg_list);
        return result;
      }
    }
    // fallback case...
    default: {
      error("sexp_to_ev3_expression: unsupported expression type");
    }
  }
}

Expression sexp_to_ev3_expression(SEXP sexp) {
  SexpToEv3ExpressionContext ctx;
  ctx.nextVariableIndex = 1;
  return sexp_to_ev3_expression_rec(sexp, ctx);
}

SEXP ev3_expression_to_sexp(Expression expr) {
  //Rprintf("ev3_expression_to_sexp: %s\n", expr->ToString().c_str()); // DEBUG 
  SEXP result, args1, args2, arg1, arg2, arg3, arg4;
  if (expr->IsLeaf()) { // leaf node: base case...
    if (expr->GetCoeff() == 0) {
      PROTECT(result = allocVector(REALSXP, 1));
      REAL(result)[0] = 0.0; 
      UNPROTECT(1);
      return result; 
    } else if (expr->GetOpType() == CONST) {
      PROTECT(result = allocVector(REALSXP, 1));
      REAL(result)[0] = expr->GetValue(); 
      UNPROTECT(1);
      return result; 
    } else if (expr->GetOpType() == VAR) {
      string name = expr->GetVarName();
      if (expr->GetCoeff() == 1) { // TODO may be near 1
        if (expr->GetExponent() == 1) {
          PROTECT(result = install(name.c_str()));
          UNPROTECT(1);
          return result;
        } else {
          PROTECT(arg1 = install(name.c_str()));
          PROTECT(arg2 = allocVector(REALSXP, 1));
          REAL(arg2)[0] = expr->GetExponent();
          PROTECT(args1 = CONS(arg1, CONS(arg2, R_NilValue)));
          PROTECT(result = LCONS(install("^"), args1));
          UNPROTECT(4);
          return result;
        }
      } else if (expr->GetCoeff() == -1) {
        if (expr->GetExponent() == 1) {
          PROTECT(arg1 = install(name.c_str()));
          PROTECT(args1 = CONS(arg1, R_NilValue));
          PROTECT(result = LCONS(install("-"), args1));
          UNPROTECT(3);
          return result;
        } else {
          PROTECT(arg1 = install(name.c_str()));
          PROTECT(arg2 = allocVector(REALSXP, 1));
          REAL(arg2)[0] = expr->GetExponent();
          PROTECT(args1 = CONS(arg1, CONS(arg2, R_NilValue)));
          PROTECT(arg4 = LCONS(install("^"), args1));
          PROTECT(args2 = CONS(arg4, R_NilValue));
          PROTECT(result = LCONS(install("-"), args2));
          UNPROTECT(6);
          return result;
        }
      } else { // expr->GetCoeff() is not 1 or -1
        if (expr->GetExponent() == 1) {
          PROTECT(arg1 = allocVector(REALSXP, 1));
          REAL(arg1)[0] = expr->GetCoeff();
          PROTECT(arg2 = install(name.c_str()));
          PROTECT(args1 = CONS(arg1, CONS(arg2, R_NilValue)));
          PROTECT(result = LCONS(install("*"), args1));
          UNPROTECT(4);
          return result;
        } else {
          PROTECT(arg1 = install(name.c_str()));
          PROTECT(arg2 = allocVector(REALSXP, 1));
          REAL(arg2)[0] = expr->GetExponent();
          PROTECT(args1 = CONS(arg1, CONS(arg2, R_NilValue)));
          PROTECT(arg4 = LCONS(install("^"), args1));
          PROTECT(arg3 = allocVector(REALSXP, 1));
          REAL(arg3)[0] = expr->GetCoeff();
          PROTECT(args2 = CONS(arg3, CONS(arg4, R_NilValue)));
          PROTECT(result = LCONS(install("*"), args2));
          UNPROTECT(7);
          return result;
        }
      }
    } else {
      error("ev3_expression_to_sexp: unknown Ev3 leaf expression");
    }
  } else { // operator node: recursive case...
    int protect_count = 0;
    Int n_args = expr->GetSize();
    Int op_type = expr->GetOpType();
    string op_label = "";
    switch (op_type) {
      case SUM:
        op_label = "+";
        break;
      case DIFFERENCE:
        op_label = "-";
        break;
      case PRODUCT:
        op_label = "*";
        break;
      case FRACTION:
        op_label = "/";
        break;
      case POWER:
        op_label = "^";
        break;
      case PLUS:
        op_label = "+";
        break;
      case MINUS:
        op_label = "-";
        break;
      case LOG:
        op_label = "log";
        break;
      case EXP:
        op_label = "exp";
        break;
      case SIN:
        op_label = "sin";
        break;
      case COS:
        op_label = "cos";
        break;
      case TAN:
        op_label = "tan";
        break;
      case COT:
        op_label = "cot";
        break;
      case SINH:
        op_label = "sinh";
        break;
      case COSH:
        op_label = "cosh";
        break;
      case TANH:
        op_label = "tanh";
        break;
      case COTH:
        op_label = "coth";
        break;
      case SQRT:
        op_label = "sqrt";
        break;
      case OTHER:
        op_label = expr->GetOpLabel();
        break;
      default:
        error("ev3_expression_to_sexp: unkown op_type");
    }

    //Rprintf("operator node '%s' of type %d with %d arguments\n", op_label.c_str(), op_type, n_args); // DEBUG

    if (n_args == 1) { // unary operator...
      PROTECT(result = LCONS(install(op_label.c_str()), CONS(ev3_expression_to_sexp(expr->GetNode(0)), R_NilValue)));
      protect_count++;
    } else { // n-ary operator...
      PROTECT(result = R_NilValue);
      protect_count++;
      for (Int i = n_args-1; i >= 0; i--) { // TODO BUGGY
        PROTECT(result = CONS(ev3_expression_to_sexp(expr->GetNode(i)), result));
        protect_count++;
        //Rprintf("i: %d, child expr: %s\n", i, expr->GetNode(i)->ToString().c_str()); // DEBUG
        if (i != n_args-1) {
          PROTECT(result = LCONS(install(op_label.c_str()), result)); // TODO
          protect_count++;
          if (i != 0) { // add another layer of parentheses...
            PROTECT(result = CONS(result, R_NilValue)); // TODO
            protect_count++;
          }
        }
      }
    }
    if (expr->GetCoeff() == -1) {
      PROTECT(args1 = CONS(result, R_NilValue));
      protect_count++;
      PROTECT(result = LCONS(install("-"), args1));
      protect_count++;
    } else if (expr->GetCoeff() != 1) {
      PROTECT(arg1 = allocVector(REALSXP, 1));
      protect_count++;
      REAL(arg1)[0] = expr->GetCoeff();
      PROTECT(args1 = CONS(arg1, CONS(result, R_NilValue)));
      protect_count++;
      PROTECT(result = LCONS(install("*"), args1));
      protect_count++;
    }
    assert(expr->GetExponent() == 1); // Ev3 does not use exponents in inner nodes, or does it?
    UNPROTECT(protect_count);
    return result;
  }
}

extern "C" { // R can only .Call C functions...

SEXP simplify(SEXP sexp) {
  try {
    Expression expr = sexp_to_ev3_expression(sexp);
    Simplify(&expr);
    return ev3_expression_to_sexp(expr); 
  } catch (ErrBase& e) {
    error(e.description.c_str());
  }
}

} // extern "C"

