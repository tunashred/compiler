## Grammar specification

Note: The `Backus-Naur` notation requires the nonterminal symbols to be underlined, but I cannot do this in *Markdown* without touching _html_. So I will put my rules in a code block, having the terminals written in uppercase, and the nonterminals in lowercase.

```
program ::= (varDef | funcPrototype | funcDeclaration | block)* FINISH

baseType ::= TYPE_INT
      | TYPE_FLOAT
      | TYPE_CHARRAY

varDef ::= baseType ID (ASSIGN expr)? (COMMA ID (ASSIGN expr)? )* SEMICOLON 

funcArg ::= baseType ID

funcArgs ::= funcArg (COMMA funcArg)*

funcDefinition ::= FUN ID L_ROUND_PAR funcArgs? R_ROUND_PAR FUNC_RET_OP (baseType | TYPE_VOID)

funcPrototype ::= funcDefinition SEMICOLON

instrBody ::= L_BRACKET block R_BRACKET

funcDeclaration ::= funcDefinition instrBody

instr ::= expr? SEMICOLON
       | WHILE (L_ROUND_PAR type? ID ASSIGN expr R_ROUND_PAR)? expr instrBody
       | IF L_ROUND_PAR expr R_ROUND_PAR instrBody? (ELIF L_ROUND_PAR expr R_ROUND_PAR instrBody? )? (ELSE instrBody? )?
       | RETURN expr SEMICOLON

expr ::= exprLogic

exprLogic ::= exprAssign ((LOGICAL_AND | LOGICAL_OR) exprAssign)*

exprAssign ::= (ID ASSIGN (L_SQUARE_PAR expr R_SQUARE_PAR)? )? exprComp

exprComp ::= exprBitwise ((EQUAL | NOT_EQ | LESS | LESS_EQ | GREATER | GREATER_EQ) exprBitwise)?

exprBitwise ::= exprAdd ((BITWISE_AND | BITWISE_XOR | BITWISE_OR) exprAdd)*

exprAdd ::= exprMul ((ADD|SUB) exprMul)*

exprMul ::= exprPrefix ((MUL | DIV) exprPrefix)*

exprPrefix ::= (ADD | SUB | LOGICAL_NOT | BITWISE_NOT)? factor

factor ::= LITERAL_INT
        | LITERAL_FLOAT
        | LITERAL_CHARRAY
        | L_ROUND_PAR expr R_ROUND_PAR
        | ID (L_ROUND_PAR (expr (COMMA expr)* )? R_ROUND_PAR)?

```

