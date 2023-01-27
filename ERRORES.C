/****************************************************************************/
/*                  PROYECTO FINAL. PROGRAMACION DE SISTEMAS.               */
/*                     ENSAMBLADOR 8080/8085 DE DOS PASOS.                  */
/*                                                                          */
/*    Realizado por: RODOLFO GONZALEZ GONZALEZ.                             */
/*                                                                          */
/****************************************************************************/


/* ERRORES.C - Este m¢dulo maneja los errores del ensamble en ASM */

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

/*                         Tabla de errores                               */

TipoError ListaError[] =
   {{ 0," ",0,"Sint xis:       ASM NomArchFuente [ NomArchListado ]"},
    { 1," ",0,"Error 1: Fin de linea no esperado."},
    { 2," ",0,"Error 2: S¡mbolo u Operando no esperado: "},
    { 3," ",0,"Error 3: Caracter token no esperado: "},
    { 4," ",0,"Error 4: S¡mbolo/Literal contiene un caracter no v lido: "},
    { 5," ",0,"Error 5: Tama¤o de comando operador no v lido: "},
    { 6," ",0,"Error 6: Modo de direccionamiento no v lido: "},
    { 7," ",0,"Error 7: Comando o Identificador desconocido: "},
    { 8," ",0,"Error 8: Operandos del Comando o PseudoOperador requeridos."},
    { 9," ",0,"Error 9: Referencia hacia adelante no permitida aqu¡."},
    {10," ",0,"Error 10: Demasiados operandos: "},
    {11," ",0,"Error 11: Etiqueta de linea requerida: "},
    {12," ",0,"Error 12: Etiqueta encontrada en lista externa: "},
    {13," ",0,"Error 13: Etiqueta ya definida: "},
    {14," ",0,"Error 14: S¡mbolo operando ya resuelto: "},
    {15," ",0,"Error 15: Colisi¢n de direcciones."},
    {16," ",0,"Error 16: Contador de localizaci¢n es non."},
    {17," ",0,"Error 17: S¡mbolo no resuelto: "},
    {18," ",0,"Error Fatal 18: Archivo no encontrado o no se puede abrir."},
    {19," ",0,"Error Fatal 19: Fin de archivo no esperado."},
    {20," ",0,"Error Fatal 20: Disco lleno al escribir archivo. Proceso terminado!."},
    {21," ",0,"Error Fatal 21: Memoria din mica agotada. Proceso terminado!."},
    {22," ",0,"Advertencia 22: S¡mbolo demasiado largo. Truncado: "},
    {23," ",0,"Advertencia 23: Valor fuera de rango."},
    {24," ",0,"Advertencia 24: Expresi¢n/Sint xis imprecisa."},
    {25," ",0,"Advertencia 25: Tama¤o de comando op no especificado."},
    {26," ",0,"Advertencia 26: Etiqueta de linea no permitida. Ignorada."},
    {27," ",0,"Advertencia 27: Operandos de comando no permitidos. Ignorados."},
    {28," ",0,"Advertencia 28: S¡mbolo ya existente en lista global. Ignorado: "},
    {29," ",0,"Advertencia 29: S¡mbolo ya existente en lista externa. Ignorado: "},
    {30," ",0,"Advertencia 30: Error de sint xis en expresi¢n operando."},
    {31," ",0,"Advertencia 31: Par‚ntesis desequilibrados."},
    {32," ",0,"Advertencia 32: Expresi¢n Inexistente."}};

/*                                 Variables                                */

int TotalErrores = 0, TotalAdvertencias = 0;
int Error = 0;
TipoTablaError *TablaErrores = NULL;

extern TipoSimbolo *SimTabla;
extern unsigned int ActLinea;

/*                                 Prototipos                              */

void Errores (int NumError,char File[MAXPATH],int NumLine,char *CadExtra);

/*                   Funciones para el manejo de errores                    */

void AgregaErrorATabla (TipoTablaError **RaizTabla,char ErrorMen[ERRORLONG],int NLinea)
/* agrega un nuevo error a la tabla de errores */
{
     if ((*RaizTabla) == NULL) {
         if (( *RaizTabla = (TipoTablaError *) malloc (sizeof (TipoTablaError)) ) != NULL) {
            strcpy ((*RaizTabla)->Mensaje,ErrorMen);
            (*RaizTabla)->Num = NLinea;
            (*RaizTabla)->Sig = NULL;
         }
         else {                  /* se agot¢ la memoria */
            Error = 21;
            Errores (21,NULL,ActLinea,NULL);
            exit (1);
         }
     }
     else
        AgregaErrorATabla (&((*RaizTabla)->Sig),ErrorMen,NLinea);
}

void Errores (int NumError,char File[MAXPATH],int NumLine,char *CadExtra)
{
    char ErrorLine [ERRORLONG] = " ";
    char Caux[MAXSIMLEN];
    if (NumError <= MAXNUMERRORS) {
       if (File != NULL) {
          strcat (ErrorLine,File);
          strcat (ErrorLine,"(");
          strcat (ErrorLine,itoa(NumLine,Caux,10));
          strcat (ErrorLine,")-> ");
       }
       strcat (ErrorLine,ListaError[NumError].Mensa);
       if (CadExtra != NULL) {
          strcat (ErrorLine,"\" ");
          strcat (ErrorLine,CadExtra);
          strcat (ErrorLine," \"");
       }
       if ((NumError!=0)||(NumError!=18)||(NumError!=19)||(NumError!=20)||
           (NumError!=21)) {
           AgregaErrorATabla (&TablaErrores,ErrorLine,NumLine);
       }
       if ((NumError==0)||(NumError==18)||(NumError==19)||(NumError==20)||
           (NumError==21))
           cprintf ("\n\r %s \n\r",ErrorLine);
       if (NumError <= 21) TotalErrores++;
       else TotalAdvertencias++;

    }
}

