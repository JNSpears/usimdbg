%{   
#include "parser.hpp"
#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno;

// sudo apt-get install libreadline-dev 

  #include <stdlib.h>
  #include <readline/readline.h>
  #include <readline/history.h>
  #define YY_INPUT(buf,result,max_size) result = mygetinput(buf, max_size);

  static int mygetinput(char *buf, unsigned long size) {
    char *line;
    if (feof(yyin))  return YY_NULL;
    line = readline(">> ");
    if(!line)        return YY_NULL;
    if(strlen(line) > size-2){
       fprintf(stderr,"input line too long\n"); return YY_NULL; }
    sprintf(buf,"%s\n",line);
    if (strlen(line))
      add_history(line);
    free(line);
    return strlen(buf);
  }
 
%}

%option nounput
%option noyywrap

 
%%

"#".*          { /* ignore comments */ }

"="            {return EQ;}
"~"            {return NOT;}
"-"            {return MINUS;}
"+"            {return PLUS;}
"*"            {return MULT;}
"/"            {return DIV;}
"("            {return L_PAREN;}
")"            {return R_PAREN;}
"%"		     	   {return MOD;}

"|"            {return OR;}
"^"            {return XOR;}
"&"            {return AND;}

"<<"           {return LSL;}
">>"           {return LSR;}

^(S(TEP)?)      {return STEP;}
(G(O)?)        {return GO;}
(R(EGS)?)      {return REGS;}
"RESET"        {return RESET;}
"EVAL"         {return EVAL;}
^(D(UMP)?)     {return DUMP;}
(L(EN)?)       {return LEN;}
"TO"           {return TO;}
(T(R(ACE)?)?)  {return TRACE;}
(CLEAR)        {return CLEAR;}
(BR(EAK)?)     {return BREAK;}
(TBF)          {return TBF;}
(HELP)         {return HELP;}
(QUIT)         {return QUIT;}
(BY(TE)?)      {return BYTE;}
(WO(RD)?)      {return WORD;}
(ASM)          {return ASM;}
(OCCURS)       {return OCCURS;}
(LOAD)         {return LOAD;}
(BASE)         {return BASE;}
(APPEND)       {return APPEND;}
(SYM(BOLS)?)   {return SYMBOLS;}

\'([[:alnum:][:punct:]]+) {
                  if (strlen(yytext) > sizeof(yylval.string))
                  {
                    fprintf(stderr,"FILENAME too long\n");
                    return YY_NULL;
                  }
                  yylval.string[0] = 0;
                  strncpy(yylval.string, yytext+1, sizeof(yylval.string));
                  return FILENAME;
               }
\"([[:alnum:]_]+) {
                  if (strlen(yytext) > sizeof(yylval.string))
                  {
                    fprintf(stderr,"Symbol too long\n");
                    return YY_NULL;
                  }
                  yylval.string[0] = 0;
                  strncpy(yylval.string, yytext+1, sizeof(yylval.string));
                  return SYMBOL;
               }




"Exec"         {yylval.brk_typ = Exec;   return XRWA;}
"Read"         {yylval.brk_typ = Read;   return XRWA;}
"Write"        {yylval.brk_typ = Write;  return XRWA;}
"Access"       {yylval.brk_typ = Access; return XRWA;}
 
"A"            {yylval.reg_id.id = 8;  return REGID;}
"B"            {yylval.reg_id.id = 9;  return REGID;}
"CC"           {yylval.reg_id.id = 10; return REGID;}
"DP"           {yylval.reg_id.id = 11; return REGID;}
"D"            {yylval.reg_id.id = 0;  return REGID;}
"X"            {yylval.reg_id.id = 1;  return REGID;}
"Y"            {yylval.reg_id.id = 2;  return REGID;}
"S"            {yylval.reg_id.id = 4;  return REGID;}
"U"            {yylval.reg_id.id = 3;  return REGID;}
"PC"           {yylval.reg_id.id = 5;  return REGID;}
"$"            {yylval.reg_id.id = 5;  return REGID;}

([0-9][0-9A-F]*)        {
                            yylval.ival = strtol(yytext, NULL, 16);
                            return NUMBER;
                        }

[ \t]+         {/* ignore spaces */}

"\n"           {return END; }


%%

/*
void abc(char *p, int l) {
  // note yy_scan_buffer is is looking for a double null string
  yy_scan_buffer(p, l);
}
*/