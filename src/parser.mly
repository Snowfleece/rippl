%{  %}

%token EOF LET IN IF THEN ELSE OVER FUN LBRACK RBRACK LPAREN RPAREN COMMA
%token LRANGE RARROW TLIT FLIT PLUS MINUS DIVIDE TIMES POW MOD PLUSF MINUSF
%token DIVIDEF TIMESF POWF OR AND NOT EQ EQF NEQ NEQF LESS LESSF GREATER 
%token GREATERF LEQ LEQF GEQ GEQF CONS HEAD CAT TAIL ASSIGN BAR NEWLINE
%token DOUBLECOL INTTYPE FLOATTYPE BOOLTYPE CHARTYPE


%token <char> CHARLIT
%token <int> INTLIT
%token <string> STRLIT
%token <float> FLOATLIT
%token <string> ID

%start expr
%type <int> expr

%%

expr: PLUS      { 1 }
