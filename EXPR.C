/****************************************************************************/
/*                  PROYECTO FINAL. PROGRAMACION DE SISTEMAS.               */
/*                     ENSAMBLADOR 8080/8085 DE DOS PASOS.                  */
/*                                                                          */
/*    Realizado por: RODOLFO GONZALEZ GONZALEZ.                             */
/*                                                                          */
/****************************************************************************/

/*-------------------------------------------------------------------------*/
/*   EXPR.C - M¢dulo de an lisis y evaluaci¢n de expresiones sencillas,    */
/*            que utiliza un analizador descendente recursivo general,     */
/*            multiusos.                                                   */
/*-------------------------------------------------------------------------*/

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

extern char *expr;                     /* contiene expresion */
extern char NomArchASM [MAXPATH];
extern unsigned int  ActLinea;

char token[80];
char tipo_token;

int  ObtenerExpresion (TipoNum *resultado);
void nivel2 (TipoNum *resultado);
void nivel3 (TipoNum *resultado);
void nivel4 (TipoNum *resultado);
void nivel5 (TipoNum *resultado);
void nivel6 (TipoNum *resultado);
void primitiva (TipoNum *resultado);
void unario (char o, TipoNum *r);
void arith (char o, TipoNum *r, TipoNum *h);
boolean isdelim (char c);

/* devuelve un token a su lugar */

void Regresar (void)
{
    char *t;
    t = token;
    for (; *t; t++) expr--;
}

/* obtener siguiente token */

void ObtenerToken (void)
{
    register char *temp;

    tipo_token = 0;
    temp = token;

    while (isspace(*expr)) ++expr;  /* saltar espacios */

    if (strchr("+-*/%^=()",*expr)) {
       tipo_token = DELIMITADOR;
       *temp++=*expr++;                /* ir a la sgte. posici¢n */
    }
    else if (isalpha(*expr)) {
            while (!isdelim(*expr)) *temp++=*expr++;
            tipo_token = VARIABLE;
         }
         else if (isdigit(*expr)) {
                 while (!isdelim(*expr)) *temp++=*expr++;
                 tipo_token = NUMERO;
              }
    *temp = '\0';
}

/* devuelve TRUE si c es delimitador */

boolean isdelim (char c)
{
    if (strchr("+-*/%^=()",c) || c==9 || c=='\r' || c== 0)
       return TRUE;
    return FALSE;
}

/* punto de entrada del analizador */

int ObtenerExpresion (TipoNum *resultado)
{
     ObtenerToken ();
     if (!*token) {
        Errores (32,NomArchASM,ActLinea,NULL);
        return 32;
     }
     nivel2 (resultado);
     return 0;
}

/* suma o resta dos terminos */

void nivel2 (TipoNum *resultado)
{
    register char op;
    TipoNum temp;

    nivel3 (resultado);
    while ((op=*token)=='+' || op=='-') {
       ObtenerToken ();
       nivel3 (&temp);
       arith (op,resultado,&temp);
    }
}

/* multiplica o divide dos factores */

void nivel3 (TipoNum *resultado)
{
    register char op;
    TipoNum temp;

    nivel4 (resultado);
    while ((op=*token)=='*' || op=='/' || op=='%') {
       ObtenerToken ();
       nivel4 (&temp);
       arith (op,resultado,&temp);
    }
}

/* procesar un exponente */

void nivel4 (TipoNum *resultado)
{
    TipoNum temp;

    nivel5 (resultado);
    if (*token=='^') {
       ObtenerToken ();
       nivel4 (&temp);
       arith ('^',resultado,&temp);
    }
}

/* + o - unario */

void nivel5 (TipoNum *resultado)
{
    register char op;

    op = 0;
    if ((tipo_token==DELIMITADOR) && *token=='+' || *token=='-') {
       op = *token;
       ObtenerToken ();
    }
    nivel6 (resultado);
    if (op)
       unario (op,resultado);
}

/* expresi¢n con par‚ntesis */

void nivel6 (TipoNum *resultado)
{
    if ((*token=='(') && (tipo_token==DELIMITADOR)) {
       ObtenerToken ();
       nivel2 (resultado);
       if (*token != ')')
          Errores (31,NomArchASM,ActLinea,NULL);
       ObtenerToken ();
    }
    else
       primitiva (resultado);
}

/* obtiene el valor de un n£mero */

void primitiva (TipoNum *resultado)
{
   if (tipo_token==NUMERO) {
      *resultado = atoi (token);
      ObtenerToken ();
      return;
   }
   Errores (30,NomArchASM,ActLinea,NULL);
}

/* ejecuta el operador aritm‚tico */

void arith (char o, TipoNum *r, TipoNum *h)
{
    register TipoNum t, ex;

    switch (o) {
       case '-':
          *r = *r - *h;
          break;
       case '+':
          *r = *r + *h;
          break;
       case '*':
          *r = (*r) * (*h);
          break;
       case '/':
          *r = (*r) / (*h);
          break;
       case '%':
          t = (*r) / (*h);
          *r = *r - (t * (*h));
          break;
       case '^':
          ex = *r;
          if (*h == 0) {
             *r = 1;
             break;
          }
          for (t=*h-1; t>0; --t) *r = (*r) * ex;
          break;
    }
}

/* procesa un operador unario */

void unario (char o, TipoNum *r)
{
    if (o=='-') *r = -(*r);
}

