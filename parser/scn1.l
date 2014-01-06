%{
/*
 * AT&T lex can't handle this lexer due to lex bugs.  It works with flex
 * 2.3.7, pclex 2.0.5, and MKS lex 3.1a.
 * http://flex.sourceforge.net/manual/Bison-Bridge.html
 */
#include "da.h"
#include "sql1.h"

#include <string.h>
#include <stdlib.h>
int lineno = 1;
void yyerror(char *s);
%}
	/* MKS needs the next line to increase the NFA table */
%e 1200
%option bison-bridge bison-locations

%%

	/* literal keyword tokens */

ADA		{ return ADA; }
ALL		{ return ALL; }
AND		{ return AND; }
AVG		{ return AMMSC; }
MIN		{ return AMMSC; }
MAX		{ return AMMSC; }
sum |
SUM		{ return AMMSC; }
COUNT		{ return AMMSC; }
MONTH       { return AMMSC;}
YEAR        { return AMMSC;}

ANY		{ return ANY; }
as  |
AS		{ return AS; }
ASC		{ return ASC; }
AUTHORIZATION	{ return AUTHORIZATION; }
BETWEEN		{ return BETWEEN; }
by      |
BY		{ return BY; }
C		{ return C; }
CHAR(ACTER)?	{ return CHARACTER; }
CHECK		{ return CHECK; }
CLOSE		{ return CLOSE; }
COBOL		{ return COBOL; }
COMMIT		{ return COMMIT; }
CONTINUE	{ return CONTINUE; }
CREATE		{ return CREATE; }
CURRENT		{ return CURRENT; }
CURSOR		{ return CURSOR; }
DECIMAL		{ return DECIMAL; }
DECLARE		{ return DECLARE; }
DEFAULT		{ return DEFAULT; }
delete  |
DELETE		{ return DELETE; }
DESC		{ return DESC; }
DISTINCT	{ return DISTINCT; }
DOUBLE		{ return DOUBLE; }
ESCAPE		{ return ESCAPE; }
EXISTS		{ return EXISTS; }
FETCH		{ return FETCH; }
FLOAT		{ return FLOAT; }
FOR		{ return FOR; }
FOREIGN		{ return FOREIGN; }
FORTRAN		{ return FORTRAN; }
FOUND		{ return FOUND; }
from    |
FROM		{ return FROM; }
GO[ \t]*TO	{ return GOTO; }
GRANT		{ return GRANT; }
group   |
GROUP		{ return GROUP; }
HAVING		{ return HAVING; }
IN		{ return IN; }
INDICATOR	{ return INDICATOR; }
INSERT		{ return INSERT; }
INT(EGER)?	{ return INTEGER; }
INTO		{ return INTO; }
IS		{ return IS; }
KEY		{ return KEY; }
LANGUAGE	{ return LANGUAGE; }
LIKE		{ return LIKE; }
MODULE		{ return MODULE; }
NOT		{ return NOT; }
NULL		{ return NULLX; }
NUMERIC		{ return NUMERIC; }
OF		{ return OF; }
ON		{ return ON; }
OPEN		{ return OPEN; }
OPTION		{ return OPTION; }
OR		{ return OR; }
order   |
ORDER		{ return ORDER; }
PASCAL		{ return PASCAL; }
PLI		{ return PLI; }
PRECISION	{ return PRECISION; }
PRIMARY		{ return PRIMARY; }
PRIVILEGES	{ return PRIVILEGES; }
PROCEDURE	{ return PROCEDURE; }
PUBLIC		{ return PUBLIC; }
REAL		{ return REAL; }
REFERENCES	{ return REFERENCES; }
ROLLBACK	{ return ROLLBACK; }
SCHEMA		{ return SCHEMA; }
select |
SELECT		{ return SELECT; }
SET		{ return SET; }
SMALLINT	{ return SMALLINT; }
SOME		{ return SOME; }
SQLCODE		{ return SQLCODE; }
TABLE		{ return TABLE; }
TO		{ return TO; }
UNION		{ return UNION; }
UNIQUE		{ return UNIQUE; }
UPDATE		{ return UPDATE; }
USER		{ return USER; }
VALUES		{ return VALUES; }
VIEW		{ return VIEW; }
WHENEVER	{ return WHENEVER; }
where   |
WHERE		{ return WHERE; }
WITH		{ return WITH; }
WORK		{ return WORK; }
limit   |
LIMIT       { return LIMIT; }
offset  |
OFFSET      { return OFFSET; }
	/* punctuation */

"="	|
"<>" 	|
"<"	|
">"	|
"<="	|
">="		{ return COMPARISON; }



	/* names */
[A-Za-z][A-Za-z0-9_]*	{
        _ly *c = calloc(1, sizeof(_ly));
        c->tab = yytext;
        c->len = yyleng;
		yylval->lys = c;
        return NAME; 
        }

	/* numbers */

[0-9]+	|
[0-9]+"."[0-9]*	{ return INTNUM; }

[0-9]+[eE][+-]?[0-9]+	|
[0-9]+"."[0-9]*[eE][+-]?[0-9]+ |
"."[0-9]*[eE][+-]?[0-9]+	{ return APPROXNUM; }
[-+*/:(),.;]	{ return yytext[0]; }
	/* strings */

'[^'\n]*'	{
		int c = input();

		unput(c);	/* just peeking */
		if(c != '\'') {
			return STRING;
		} else
			yymore();
	}
		
'[^'\n]*$	{/* yyerror("Unterminated string");*/ }

\n		lineno++;

[ \t\r]+	;	/* white space */

"--".*$		;	/* comment */

%%

void
yyerror(char *s)
{
	/*printf("%d: %s at %s\n", lineno, s, yytext);*/
}

main(int ac, char **av)
{
    extern int yydebug ;
    yydebug = 1;
    ly = _init_ly();
    tail = ly;
    _ly *tly;

	if(ac > 1 && (yyin = fopen(av[1], "r")) == NULL) {
		perror(av[1]);
		exit(1);
	}

	if(!yyparse())
		printf("SQL parse worked\n");
	else
		printf("SQL parse failed\n");
    tly = _get();
    for(;tly; tly=tly->next)
        printf("tab: %s\n", tly->tab);
} /* main */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  parser_do
 *  Description:  
 * =====================================================================================
 */
_ly *parser_do (char *str, int len){
    char *p = str;
    ly = _init_ly();
    tail = ly;

    YY_BUFFER_STATE state = yy_scan_bytes(p, len);
    yy_switch_to_buffer(state);
    yyparse();
    yy_delete_buffer(state);

    return _get();
}		/* -----  end of function parser_do  ----- */

_ly *_init_ly(){
    _ly *_l;
    _l = calloc(1, sizeof(_ly));
    _l->tab = NULL;
    _l->next = NULL;
    _l->len = 0;
    return _l;
}

void _len(int l){
    if(ly != NULL){
       ly->len = l;
    }
}

void _save(char *s, int len){
    _ly *_l;

    if( s == NULL || len < 0) return;

    if(tail->tab == NULL){
        tail->tab = calloc(1, len*sizeof(char));
        memcpy(tail->tab, s, len);
        tail->len = len;
    }
    /* because only parser change sql
    else{
        _l = _init_ly(); 
        _l->tab = calloc(1, len*sizeof(char));
        memcpy(_l->tab, s, len);
        _l->len = len;
        tail->next = _l;
        tail = _l;
    }*/
}

_ly *_get(){
    if(ly != NULL && ly->tab != NULL){
        return ly;
    }else{
        return NULL;
    }
}
