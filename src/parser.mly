%{  %}

%token EOF LET IN IF THEN ELSE OVER FUN MAIN LBRACK RBRACK LPAREN RPAREN COMMA
%token LRANGE RARROW TLIT FLIT PLUS MINUS DIVIDE TIMES POW MOD PLUSF MINUSF
%token DIVIDEF TIMESF POWF OR AND NOT EQ EQF NEQ NEQF LESS LESSF GREATER 
%token GREATERF LEQ LEQF GEQ GEQF LEN CONS HEAD CAT TAIL ASSIGN BAR NEWLINE
%token DOUBLECOL INTTYPE FLOATTYPE BOOLTYPE CHARTYPE
%token MAYBE JUST NONE


%token <char> CHARLIT
%token <int> INTLIT
%token <string> STRLIT
%token <float> FLOATLIT
%token <string> ID

##exponents more tightly than unary, pow below unary, mult div, mod, addition, subtraction (less obv). comparison operators least precedence, float operators same thing. 


%left ASSIGN
%left OR AND NOT EQ EQF NEQ NEQF LESS LESSF GREATER GREATERF LEQ LEQF GEQF GEQ
%left PLUS MINUS PLUSF MINUSF
%left TIMES DIVIDE MOD TIMESF DIVIDEF
%nonassoc UMINUS
%left POW
$left POWF
%left CONS HEAD TAIL CAT LEN

%start expr
%type <int> expr

%%

expr: PLUS      { 1 }
    | MINUS expr %prec UMINUS { - $2 }
