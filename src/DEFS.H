/****************************************************************************/
/*                  PROYECTO FINAL. PROGRAMACION DE SISTEMAS.               */
/*                     ENSAMBLADOR 8080/8085 DE DOS PASOS.                  */
/*                                                                          */
/*    Realizado por: RODOLFO GONZALEZ GONZALEZ.                             */
/*                                                                          */
/****************************************************************************/

/* DEFS.H - Este m¢dulo header contiene definiciones de constantes y tipos
   para AMS */

#include <dir.h>

#define MAXINSTRLEN 15
#define MAXSIMLEN 32
#define ERRORLONG 255
#define MAXNUMERRORS 32
#define MAXLINELEN 255
#define DELIMITADOR 1
#define VARIABLE 2
#define NUMERO 3

typedef enum bool {FALSE,TRUE} boolean;

typedef int TipoNum;

typedef struct SimTab {
     char Nombre [MAXSIMLEN];
     unsigned int Valor;
     boolean Resuelto;
     struct SimTab *Sig;
     } TipoSimbolo;

typedef struct {                                    /* tipo para mnem¢nicos */
     unsigned int Codigo;              /* c¢digo hexadecimal del mnem¢nico */
     char Nemotecnico [MAXINSTRLEN];                           /* mnem¢nico */
     unsigned int LongitudOBJ;                   /* longitud c¢digo objeto */
     int NOperandos;
     } TipoInstr;

typedef struct {
         int NumErr;
         char ArchASM [MAXPATH];
         int NumLin;
         char Mensa [ERRORLONG];
         } TipoError;

typedef struct TablasErrores {
         int Num;
         char Mensaje [ERRORLONG];
         struct TablasErrores *Sig;
         } TipoTablaError;

const char Titulo[] = "\n Ensamblador de dos pasos 8080/8085 versi¢n 1.0 \n Realizado por: RODOLFO GONZALEZ GONZALEZ.";

typedef struct EstLineas {
         int  NumLinea;
         char Linea [MAXLINELEN];
         struct EstLineas *Sig;
       } TipoLinea;

typedef struct CT {
           unsigned int C;
           struct CT *Sig;
        } TipoTablaCont;

void AgregaLineaAListado (TipoLinea **RaizTabla,char NL[MAXLINELEN]);
boolean BuscaSimboloEnTabla (TipoSimbolo *Raiz,char *sim);
