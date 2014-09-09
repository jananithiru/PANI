%{
#include "heading.h"
#include "agdevku_parser.hpp"
int lineno = 1;
void yyerror(char *s);
%}

%option noyywrap
/* MKS needs the next line to increase the NFA table */
%e 1200

%%

	/* literal keyword tokens */

ADA		{ return ADA; }
ALL		{ return ALL; }
AND		{ strcpy(yylval.strval, yytext);return AND; }
AVG		{ return AMMSC; }
MIN		{ return AMMSC; }
MAX		{ return AMMSC; }
SUM		{ return AMMSC; }
COUNT		{ return AMMSC; }
ANY		{ return ANY; }
AS		{ return AS; }
ASC		{ return ASC; }
AUTHORIZATION	{ return AUTHORIZATION; }
BETWEEN		{ return BETWEEN; }
BY		{ return BY; }
C		{ return C; }
CHAR(ACTER)?	{ strcpy(yylval.strval, yytext);return CHARACTER; }
CHECK		{ return CHECK; }
CLOSE		{ return CLOSE; }
COBOL		{ return COBOL; }
COMMIT		{ return COMMIT; }
CONTINUE	{ return CONTINUE; }
CREATE		{return CREATE; }
CURRENT		{ return CURRENT; }
CURSOR		{ return CURSOR; }
DATABASE		{ return DATABASE; }
DECIMAL		{ return DECIMAL; }
DECLARE		{ return DECLARE; }
DEFAULT		{ return DEFAULT; }
DELETE		{ return DELETE; }
DESC		{ return DESC; }
DESCRIBE	{ return DESCRIBE; }
DISTINCT	{ return DISTINCT; }
DOUBLE		{ return DOUBLE; }
DROP		{ return DROP; }
ESCAPE		{ return ESCAPE; }
EXISTS		{ return EXISTS; }
FETCH		{ return FETCH; }
FLOAT		{ strcpy(yylval.strval, yytext);return FLOAT; }
FOR		{ return FOR; }
FOREIGN		{ return FOREIGN; }
FORTRAN		{ return FORTRAN; }
FOUND		{ return FOUND; }
FROM		{ return FROM; }
GO[ \t]*TO	{ return GOTO; }
GRANT		{ return GRANT; }
GROUP		{ return GROUP; }
HAVING		{ return HAVING; }
IN		{ return IN; }
INDEX		{ return INDEX; }
INDICATOR	{ return INDICATOR; }
INSERT		{ return INSERT; }
INT(EGER)?	{ strcpy(yylval.strval, yytext);return INTEGER; }
INTO		{ return INTO; }
IS		{ return IS; }
KEY		{ strcpy(yylval.strval, yytext);return KEY; }
LANGUAGE	{ return LANGUAGE; }
LIKE		{ return LIKE; }
MODULE		{ return MODULE; }
NOT		{ strcpy(yylval.strval, yytext);return NOT; }
NULL		{ strcpy(yylval.strval, yytext);return NULLX; }
NUMERIC		{ return NUMERIC; }
OF		{ return OF; }
ON		{ return ON; }
OPEN		{ return OPEN; }
OPTION		{ return OPTION; }
OR		{ strcpy(yylval.strval, yytext);return OR; }
ORDER		{ return ORDER; }
PASCAL		{ return PASCAL; }
PLI		{ return PLI; }
PRECISION	{ return PRECISION; }
PRIMARY		{ strcpy(yylval.strval, yytext);return PRIMARY; }
PRIVILEGES	{ return PRIVILEGES; }
PROCEDURE	{ return PROCEDURE; }
PUBLIC		{ return PUBLIC; }
REAL		{ strcpy(yylval.strval, yytext);return REAL; }
REFERENCES	{ return REFERENCES; }
ROLLBACK	{ return ROLLBACK; }
SCHEMA		{ return SCHEMA; }
SELECT		{ return SELECT; }
SET		{ return SET; }
SHOW		{ return SHOW; }
SMALLINT	{ return SMALLINT; }
SOME		{ return SOME; }
SQLCODE		{ return SQLCODE; }
TABLE		{ return TABLE; }
TABLES		{ return TABLES; }
TO		{ return TO; }
UNION		{ return UNION; }
UNIQUE		{ return UNIQUE; }
UPDATE		{ return UPDATE; }
USER		{ return USER; }
VALUES		{ return VALUES; }
VARCHAR		{ strcpy(yylval.strval, yytext);return VARCHAR; }
VIEW		{ return VIEW; }
WHENEVER	{ return WHENEVER; }
WHERE		{ return WHERE; }
WITH		{ return WITH; }
WORK		{ return WORK; }
DATABASES	{ return DATABASES;}
USE			{ return USE;}

	/* punctuation */
"="		|	{ strcpy(yylval.strval, yytext);return COMPARISON; }
"<>" 	|   { strcpy(yylval.strval, yytext);return COMPARISON; }
"<"	|		{ strcpy(yylval.strval, yytext);return COMPARISON; }
">"	|		{ strcpy(yylval.strval, yytext);return COMPARISON; }
"<="	|	{ strcpy(yylval.strval, yytext);return COMPARISON; }
">="		{ strcpy(yylval.strval, yytext);return COMPARISON; }

[-+*/:(),.;]	{ return yytext[0]; }

	/* names */


[A-Za-z][A-Za-z0-9_]*	{ strcpy(yylval.strval, yytext);return NAME; }

	/* numbers */

[0-9]+	|
[0-9]+"."[0-9]* |
"."[0-9]*	{ strcpy(yylval.strval, yytext);return INTNUM; }

[0-9]+[eE][+-]?[0-9]+	|
[0-9]+"."[0-9]*[eE][+-]?[0-9]+ |
"."[0-9]*[eE][+-]?[0-9]+	{ return APPROXNUM; }

	/* strings */

'[A-Za-z0-9_"."@-]*[ \t\n\r\f]*'	{
			int len = strlen(yytext);
			//printf("yytext = %s,strlen(yytext)=%d\n",yytext,strlen(yytext));
			strncpy(yylval.strval, &yytext[1],len-2);
			memset(&yylval.strval[len-2],'\0',len-(len-2));
			//printf("yylval.strval = %s\n",yylval.strval);
			//printf("strlen(yylval.strval)=%d\n",strlen(yylval.strval));
			return STRING;
	}
	
'[^'\n]*$	{ yyerror("Unterminated string"); }

\n		lineno++;

[ \t\r]+	;	/* white space */

"--".*$		;	/* comment */

%%



void
yyerror(char *s)
{
	printf("%d: %s at %s\n", lineno, s, yytext);
	//sprintf(ExecuteStructures::errorMessage,"%d: %s at %s\n", lineno, s, yytext);
}

