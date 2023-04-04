/****************************************************************************/
/*                     ENSAMBLADOR 8080/8085 DE DOS PASOS.                  */
/*                                                                          */
/*    Realizado por: RODOLFO GONZALEZ GONZALEZ.                             */
/*                                                                          */
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*    ASM.C - M¢dulo principal de ensamble de ASM ensamblador 8080/8085.    */
/*  El programa ASM es un ensablador de dos pasos para el 8080/8085. Este   */
/*  m¢dulo realiza la tarea de ensamblaje, apoy ndose en otros m¢dulos      */
/*  incluidos.                                                              */
/*--------------------------------------------------------------------------*/

/*                     Inclusi¢n de headers estandar                        */

#include <stdio.h>
#include <conio.h>
#include <dir.h>
#include <string.h>
#include <dos.h>
#include <ctype.h>
#include <alloc.h>

/*                    Inclusi¢n de m¢dulos .C de apoyo                      */

#include "defs.h"
#include "errores.c"
#include "lenguaje.c"
#include "expr.c"

/*           Variables globales en ASM.C para el ensamblador                */

char NomArchASM [MAXPATH], NomArchLST [MAXPATH], Temp [MAXPATH];
char Drive [MAXDRIVE], Dir [MAXDIR], File [MAXFILE], Ext [MAXEXT];
FILE *Entrada;
FILE *Salida;
TipoLinea *TextoFuente, *TextoAEnsamblar;
TipoSimbolo *SimTabla = NULL;
char aux[MAXLINELEN] = " ";
char Cad [MAXLINELEN] = " ";
unsigned int ActLinea = 0, Cont = 0, DirBase = 0, L = 0, N = 0, Pasada = 1;
unsigned int Cod = 0;
boolean Final = FALSE, MoverCont = TRUE;
int SeImprimieronErrores;
TipoTablaCont *TablaCont;
char CAux[MAXLINELEN] = " ";
TipoLinea *Listado = NULL;
char *expr;

extern TipoTablaError *TablaErrores;

/*                            Otras variables                               */

struct text_info ti;          /* mantiene caracter¡sticas iniciales del CRT */

/*                              Prototipos                                  */

void Presenta (void);
void Inicializa (void);
int  AnalizaNomArchs (int cont, char *noms[]);
int AbreArchivos (char *F1, char *F2);
int CargaArchivoFuente (char LineaArr[MAXLINELEN],TipoLinea **Raiz);

/*                         Funciones adicionales                            */

void Presenta (void)
/* pone la presentaci¢n de ASM en pantalla */
{
    gettextinfo (&ti); textcolor (WHITE); textbackground (BLACK);
    cprintf ("\n\r Ensamblador de dos pasos 8085/8080, Versi¢n 1.0;");
    cprintf ("\n\r Realizado por: RODOLFO GONZALEZ GONZALEZ.");
    cprintf ("\n\r Memoria RAM disponible: %u bytes.\n\r",coreleft());
}

void Salir (void)
{
     cprintf ("\n\r\n\r Memoria RAM disponible: %u bytes.\n\r",coreleft());
     cprintf ("\n\r\n\r (c) RODOLFO GONZALEZ GONZALEZ.");
     sound (1440); delay (100); nosound ();
     textattr (ti.attribute);
}

/*                          Funciones de ASM.C                              */

boolean SiPideAyuda (int cont, char *args[])
{
    int i;
    if (cont > 1)
       for (i=1; i < cont; i++) {
          if (strcmpi(args[i],"/?") == 0) {
             Errores (0,NULL,0,NULL);
             sound(1440); delay (100); nosound(); Salir ();
             return TRUE;
          }
       }
    return FALSE;
}

int strpos (char *cad,char c)
/* Regresa la posici¢n de la primera ocurrencia del caracter 'c' dado en la
   cadena "cad" o -1 si no se encuentra en ella. Esta funci¢n deber¡a ser
   implementada en la librer¡a estandar, pero como no lo ha sido, y¢ la hice*/
{
    int i;
    if (strchr(cad,c)!= NULL)
       for (i=0; cad[i]!=NULL; i++)
          if (cad[i]==c) return i;
    return -1;
}

char *strtrn (char *cad,int n)
{
    cad[n] = '\0';
    return cad;
}

boolean EsNumCad (char *s)
{
    int i;
    boolean SiNum = TRUE;

    for (i = 0; (i < strlen(s)) && (SiNum); i++)
       if (isdigit(s[i]) ||
           (strchr("+-*/%=^ ()",s[i])==NULL) ||
           (strchr("aAbBcCdDeEfF",s[i])!=NULL)) SiNum = TRUE;
       else SiNum = FALSE;
   return SiNum;
}

char *conv (char *s, int b)
{
   int i = 0, v = 0;
   char *st;

   for (i = 0; i <= strlen(s); i++)
      if (isdigit(s[i]))
         v = b * v + (s[i] - '0');
      /*else if (strchr("aAbBcCdDeEfF",s[i]) != NULL)
              switch (b) {
                 case 8,16: v = b * v + (s[i] - '0' - 7); break;
              } switch */
   st = itoa (v,s,10);
   return st;
}
void InvierteWord (int n, char *c1, char *c2)
{
    int temp;
    temp = n << 8;
    *c1 = temp >> 8;
    temp = n >> 8;
    *c2 = temp;
}

int AnalizaNomArchs (int cont, char *noms[])
/* procesa los nombres dados en la l¡nea de comandos si los hay; sin¢, escribe
   un mensaje de error.                                                     */
{
    if ((cont >= 4)||(cont < 2)) {
         /* si hay m s de 2 comandos en la l¡nea, o no hay...error */
         cprintf ("\n\r"); Errores (0,NULL,0,NULL); cprintf ("\n\r");
         return 1;
       }
    else {
       /* sin¢... */
       strcpy (NomArchASM,strupr(noms[1]));        /* salvar nombre .ASM */
       fnsplit (NomArchASM,Drive,Dir,File,Ext);    /* analizarlo            */
       if (!(*Ext))                                     /* extensi¢n */
          fnmerge (NomArchASM,Drive,Dir,File,".ASM");   /* .ASM por defecto */
       if (cont == 3) {                                 /* si hay nombre de */
          strcpy (NomArchLST,strupr(noms[2]));          /* listado, salvarlo*/
          fnsplit (NomArchLST,Drive,Dir,File,Ext);      /* analizarlo       */
          if (!(*Ext))                                    /* extensi¢n */
             fnmerge (NomArchLST,Drive,Dir,File,".LST");  /*.LST por defecto*/
       }
       else fnmerge (NomArchLST,Drive,Dir,File,".LST");  /* nom. por defecto*/
    }
    return 0;
}

int AbreArchivos (char *F1, char *F2)
/* abre los archivos .ASM (lectura) y .LST (escritura) */
{
     if ((Entrada = fopen(F1,"rt")) == NULL) {
        Errores (18,NomArchASM,ActLinea,NULL); Error = 18;
        return 18;
     }
     if ((Salida = fopen(F2,"wt")) == NULL) {
        Errores (18,NomArchASM,ActLinea,F1); Error = 18;
        return 18;
     }
     fseek (Entrada,0L,SEEK_SET);
     fseek (Salida,0L,SEEK_SET);
     return 0;
}

int CargaArchivoFuente (char LineaArr[MAXLINELEN],TipoLinea **Raiz)
/* carga de disco a una lista el archivo fuente .ASM */
{
     if ((*Raiz) == NULL) {
         if ((*Raiz = (TipoLinea *) malloc (sizeof (TipoLinea)) ) != NULL) {
            (*Raiz)->NumLinea = ActLinea;
            strcpy ((*Raiz)->Linea,LineaArr);
            (*Raiz)->Sig = NULL;
         }
         else {
            Error = 21;
            Errores (21,NomArchASM,ActLinea,NULL);
            exit (1);
         }
     }
     else
        CargaArchivoFuente (LineaArr,&((*Raiz)->Sig));
     return 0;
}

int CopiaTexto (TipoLinea *Fuente, TipoLinea **Destino)
{
    while (Fuente != NULL) {
       if ((*Destino = (TipoLinea *) malloc (sizeof (TipoLinea)) ) != NULL) {
          (*Destino)->NumLinea = Fuente->NumLinea;
          strcpy ((*Destino)->Linea,Fuente->Linea);
          (*Destino)->Sig = NULL;
          CopiaTexto (Fuente->Sig,&((*Destino)->Sig));
          return 0;
         }
         else {
            Error = 21;
            Errores (21,NomArchASM,ActLinea,NULL);
            exit (1);
         }
    }
    return 0;
}

boolean MuyLargo (char **ptr)
{
   if (strlen(*ptr)>=MAXSIMLEN) {
      *ptr = strtrn (*ptr,MAXSIMLEN-1);
      Errores (22,NomArchASM,ActLinea,*ptr);
      return TRUE;
   }
   return FALSE;
}

boolean BuscaSimboloEnTabla (TipoSimbolo *Raiz,char *sim)
{
    boolean Encontrado = FALSE;
    while ((Raiz != NULL)&&(Encontrado == FALSE)) {
       if (strcmpi (Raiz->Nombre,sim) == 0) Encontrado = TRUE;
       else {
          Raiz = Raiz->Sig;
          Encontrado = FALSE;
       }
    }
    return Encontrado;
}

void ObtenerValor (TipoSimbolo *Raiz,char **val)
{
    boolean Encontrado = FALSE;
    while ((Raiz != NULL)&&(Encontrado == FALSE)) {
       if (strcmpi (Raiz->Nombre,*val) == 0) Encontrado = TRUE;
       else {
          Raiz = Raiz->Sig;
          Encontrado = FALSE;
       }
    }
    itoa (Raiz->Valor,*val,10);
}

void AgregaSimboloATabla (TipoSimbolo **RaizTabla,char *sim,char *val)
/* agrega un nuevo simbolo a la tabla de s¡mbolos */
{
     if ((*RaizTabla) == NULL) {
         if (( *RaizTabla = (TipoSimbolo *) malloc (sizeof (TipoSimbolo)) ) != NULL) {
            strcpy ((*RaizTabla)->Nombre,sim);
            if (val != NULL) {
               (*RaizTabla)->Valor = atoi (val);
               (*RaizTabla)->Resuelto = TRUE; }
            else {
               (*RaizTabla)->Valor = 0;
               (*RaizTabla)->Resuelto = FALSE; }
         (*RaizTabla)->Sig = NULL;
         }
         else {                  /* se agot¢ la memoria */
            Error = 21;
            Errores (21,NomArchASM,ActLinea,NULL);
            exit (1);
         }
     }
     else
        AgregaSimboloATabla (&((*RaizTabla)->Sig),sim,val);
}

void AgregaLineaAListado (TipoLinea **RaizTabla,char NL[MAXLINELEN])
/* agrega una nueva linea al listado */
{
     if ((*RaizTabla) == NULL) {
         if ((*RaizTabla = (TipoLinea *) malloc (sizeof (TipoLinea)) ) != NULL) {
            strcpy ((*RaizTabla)->Linea,NL);
            (*RaizTabla)->NumLinea = ActLinea;
            (*RaizTabla)->Sig = NULL;
         }
         else {                  /* se agot¢ la memoria */
            Error = 21;
            Errores (21,NomArchASM,ActLinea,NULL);
            exit (1);
         }
     }
     else
        AgregaLineaAListado (&((*RaizTabla)->Sig),NL);
}

void AgregaContATabla (TipoTablaCont **RaizTabla,unsigned int Cont)
{
    if ((*RaizTabla) == NULL) {
       if ((*RaizTabla = (TipoTablaCont *) malloc (sizeof (TipoTablaCont)) ) != NULL) {
         (*RaizTabla)->C = Cont;
         (*RaizTabla)->Sig = NULL;
         }
         else {                  /* se agot¢ la memoria */
            Error = 21;
            Errores (21,NomArchASM,ActLinea,NULL);
            exit (1);
         }
     }
     else
        AgregaContATabla (&((*RaizTabla)->Sig),Cont);
}

int Ensamblar_PrimerPaso (char *cad)
{
   char *ptr = NULL, *ant = NULL;
   boolean YaEntroAMnemo = FALSE, OpPres = FALSE, Precedido = FALSE;
   boolean SiEntro = TRUE;
   int NO,i=0;
   char CadAux [MAXINSTRLEN];
   TipoNum res;

   ptr = strtok (cad,", \n\r\t");
   MuyLargo (&ptr);
   while ((ptr!=NULL)&&(strcmpi(ptr,"END")!=0)) {
      if ((strpos(ptr,';')==0)||(strpos(ant,';')==0)) {
         return 0;
      }
      else {
         if ((EsMnemonico(ptr)==FALSE)&&(EsPseudoOperador(ptr)==FALSE)) {  /* es s¡mbolo */
            ant = ptr;
            if (BuscaSimboloEnTabla(SimTabla,ptr)&&(Pasada==1)&&
                (!YaEntroAMnemo)) {
               Errores (28,NomArchASM,ActLinea,ptr);
               return 28;
            }
               ptr = strtok (NULL,", \n\r\t");
               MuyLargo (&ptr);
               if (strcmpi(ptr,"EQU") == 0) {
                  if (((ptr=strtok(NULL,", \n\r\t"))==NULL) || (strpos(ptr,';')==0)) {
                     Errores (8,NomArchASM,ActLinea,NULL);
                     MuyLargo (&ptr);
                  }
                  else
                     if (BuscaSimboloEnTabla (SimTabla,ant)&&(Pasada==1)&&
                        (!YaEntroAMnemo)) {
                        Errores (28,NomArchASM,ActLinea,ant);
                        L = N = 0; Error = 28;
                     }
                     else {
                        expr = ptr;
                        ObtenerExpresion (&res); itoa (res,ptr,10);
                        AgregaSimboloATabla (&SimTabla,ant,ptr);
                        ant = ptr; ptr=strtok (NULL,", \n\r\t");
                     }
               }
            else {
                  if (strcmpi(ptr,"DW") == 0) { OpPres = FALSE;
                     while (((ptr=strtok (NULL,", \n\r\t")) != NULL) &&
                           (strpos (ptr,';') != 0)) {
                           MuyLargo (&ptr);
                           N += 2; OpPres = TRUE;
                     }
                     if (OpPres == FALSE) Errores (8,NomArchASM,ActLinea,NULL);
                     else {
                        OpPres = FALSE;
                        itoa (Cont,CadAux,10);
                        if (BuscaSimboloEnTabla (SimTabla,ant)&&(Pasada==1)&&
                           (!YaEntroAMnemo)) {
                           Errores (28,NomArchASM,ActLinea,ant);
                           N = 0; Cont -= L; Error = 28;
                        }
                        else {
                           AgregaSimboloATabla (&SimTabla,ant,CadAux);
                           ant = ptr; ptr=strtok (NULL,", \n\r\t");
                        }
                     }
                  }
                  else if (strcmpi(ptr,"DB") == 0) { OpPres = FALSE;
                          while (((ptr=strtok (NULL,", \n\r\t")) != NULL) &&
                                 (strpos (ptr,';') != 0)) {
                                 MuyLargo (&ptr);
                                 N += 1; OpPres = TRUE;
                          }
                          if (OpPres == FALSE) Errores (8,NomArchASM,ActLinea,NULL);
                          else {
                             OpPres = FALSE;
                             itoa(Cont,CadAux,10);
                             if (BuscaSimboloEnTabla (SimTabla,ant)&&(Pasada==1)&&
                                (!YaEntroAMnemo)) {
                                Errores (28,NomArchASM,ActLinea,ant);
                                N = 0; Cont -= L; Error = 28;
                             }
                          else {
                             AgregaSimboloATabla (&SimTabla,ant,CadAux);
                             ant = ptr; ptr=strtok (NULL,", \n\r\t");
                          }
                       }
                  }
                  else if (strcmpi(ptr,"DS") == 0) {
                          if ((ptr=strtok (NULL,", \n\r\t")) != NULL) {
                             MuyLargo (&ptr);
                             N = atoi (ptr);
                          }
                          else Errores (8,NomArchASM,ActLinea,NULL);
                          if (BuscaSimboloEnTabla (SimTabla,ant)&&(Pasada==1)&&
                             (!YaEntroAMnemo)) {
                             Errores (28,NomArchASM,ActLinea,ant);
                             N = 0; Cont -= L; Error = 28;
                          }
                          else {
                             itoa (Cont,CadAux,10);
                             AgregaSimboloATabla (&SimTabla,ant,CadAux);
                             ant = ptr; ptr=strtok (NULL,", \n\r\t");
                          }
                  }
                  else if (strcmpi(ptr,"ORG") == 0) {
                          ant = ptr = strtok (NULL,", \n\r\t");
                          MuyLargo (&ptr);
                          Cont = DirBase = atoi (ptr);
                       }
                  else if (strpos (ant,':')==(strlen(ant)-1)) {
                          ant = strtrn (ant,strpos(ant,':'));
                          itoa(Cont,CadAux,10);
                          if (BuscaSimboloEnTabla (SimTabla,ant)) {
                             Errores (13,NomArchASM,ActLinea,ant);
                             N = 0; Error = 13;
                          }
                          else {
                             AgregaSimboloATabla (&SimTabla,ant,CadAux);
                             Precedido = TRUE;
                             ant = ptr; ptr=strtok (NULL,", \n\r\t");
                             if (EsMnemonico(ant)) {
                                ObtenerCodigo_Y_Longitud (ant,&Cod,&L,&NO);
                                Error = -1; i = 1; SiEntro = FALSE;
                                while (((ptr=strtok(NULL,", \n\r\t"))!=NULL)&&
                                       (strpos(ptr,';')!=0)) {
                                   MuyLargo (&ptr);
                                   if (ptr!=NULL) {
                                      ant = ptr;
                                      SiEntro = TRUE;
                                   }
                                   if (!EsOperando(ptr)) {
                                      Errores (7,NomArchASM,ActLinea,ptr);
                                      Error = 7;
                                   }
                                   i++;
                                }
                                if (Error != -1) {
                                   return 7;
                                }
                                if ((i > NO)&&SiEntro) {
                                   Errores (10,NomArchASM,ActLinea,ant);
                                   return 2;
                                }
                                else if ((i < NO)&&SiEntro) {
                                        Errores (8,NomArchASM,ActLinea,NULL);
                                        return 8;
                                     }
                                Cont += L;
                                Precedido = YaEntroAMnemo = TRUE;
                             }
                          }
                       }
                  else if (BuscaSimboloEnTabla (SimTabla,ptr)) {
                          Errores (13,NomArchASM,ActLinea,ptr);
                          N = 0; Error = 13;
                       }
                  else {
                     if (ptr==NULL) ptr = ant;
                     if ((strpos(ptr,';') != 0) &&
                        (!BuscaSimboloEnTabla(SimTabla,ptr)&&(YaEntroAMnemo))
                        &&(!EsOperando(ptr))){
                         Errores (7,NomArchASM,ActLinea,ptr);
                         return 7;
                     }
                     else if ((strpos(ant,';') != 0)) {
                             Errores (7,NomArchASM,ActLinea,ptr);
                             return 7;
                          }
                     else ptr=strtok (NULL,", \n\r\t");
                  }
               Cont += N; N = L = 0;
             }
           }
         else
            if (EsPseudoOperador(ptr)) {
               if (strcmpi(ptr,"DW") == 0) {
                  L = N = 0;
                  while (((ptr=strtok(NULL,", \n\r\t"))!=NULL) &&
                         (strpos (ptr,';') != 0)) {
                     MuyLargo (&ptr);
                     L += 2; Cont +=L; OpPres = TRUE;
                  }
                  if (OpPres == FALSE) Errores (8,NomArchASM,ActLinea,NULL);
                  else OpPres = FALSE;
               }
               else if (strcmpi(ptr,"DB") == 0) {
                       L = N = 0;
                       while (((ptr=strtok (NULL,", \n\r\t"))!=NULL) &&
                              (strpos (ptr,';') != 0)) {
                          MuyLargo (&ptr);
                          L += 1; Cont += L; OpPres = TRUE;
                       }
                       if (OpPres == FALSE) Errores (8,NomArchASM,ActLinea,NULL);
                       else OpPres = FALSE;
                    }
               else if (strcmpi(ptr,"DS") == 0) {
                       L = 0;
                       if (((ptr=strtok (NULL,", \n\r\t"))!=NULL) &&
                           (strpos (ptr,';') != 0)) {
                          MuyLargo (&ptr);
                          L = atoi (ptr); Cont += L; OpPres = TRUE;
                       }
                       if (OpPres == FALSE) Errores (8,NomArchASM,ActLinea,NULL);
                       else OpPres = FALSE;
                    }
               else if (strcmpi(ptr,"ORG") == 0) {
                       if (((ant = ptr = strtok (NULL,", \n\r\t"))==NULL)) {
                          OpPres = FALSE;
                          Errores (8,NomArchASM,ActLinea,NULL);
                          return 8;
                       }
                       else {
                          Cont = DirBase = atoi (ptr);
                          ptr = strtok (NULL,", \n\r\t");
                       }
                    }
            }
         else
            if (EsMnemonico(ptr)) {
               ObtenerCodigo_Y_Longitud (ptr,&Cod,&L,&NO);
               Error = -1; i = 0; SiEntro = FALSE;
               while (((ptr=strtok(NULL,", \n\r\t"))!=NULL)&&
                  (strpos(ptr,';')!=0)) {
                  MuyLargo (&ptr);
                  if (ptr!=NULL) {
                     ant = ptr; SiEntro = TRUE;
                  }
                  if (!EsOperando(ptr)) {
                     Errores (7,NomArchASM,ActLinea,ptr);
                     Error = 7;
                  }
                  i++;
               }
               if (Error != -1) {
                  return 7;
               }
               if ((i>NO)&&SiEntro) {
                  Errores (10,NomArchASM,ActLinea,ant);
                  return 10;
               }
               else if ((i<NO)&&SiEntro) {
                  Errores(8,NomArchASM,ActLinea,NULL);
                  return 8;
               }
               Cont += L;
               YaEntroAMnemo = TRUE;
               if ((Pasada==1)&&YaEntroAMnemo) {
                  while ((ptr = strtok (NULL,", \n\r\t"))!=NULL) {
                     MuyLargo (&ptr);
                     if ((EsOperando(ptr))&&(YaEntroAMnemo || Precedido)) {
                        return -1;
                     }
                     else if (EsOperando(ant)) {
                             return -1;
                          }
                     else if (ptr!=NULL) {
                             Errores (7,NomArchASM,ActLinea,ptr);
                             return 7;
                          }
                  }
               }
               else {
                  return -1;
               }
            }
            else
               if (EsOperando(ant)) {
                  return -1;
               }
         }
   }
   if (strcmpi(ptr,"END")==0) {
      Final = TRUE;
      return -2;
   }
   return 0;
} /* fin del primer paso */


int Ensamblar_SegundoPaso (char *cad,TipoTablaError *TE,TipoTablaCont **TC)
{
    char *ptr, *sig, c1, c2;
    char c[MAXSIMLEN], mnemo[MAXSIMLEN];
    int NO = 0, NOps = 0;
    char *st, ch;
    TipoNum Val;
    boolean Precedido = FALSE;

    SeImprimieronErrores = 0;
    if ((TE!=NULL)&&(TE->Num == ActLinea)) {     /* si es linea erronea */
       while ((TE!=NULL)&&(TE->Num) == ActLinea) {    /*mientras haya error*/
          strcat (TE->Mensaje,"\n");
          AgregaLineaAListado (&Listado,TE->Mensaje);
          TE->Mensaje[strlen(TE->Mensaje)-1] = '\0';
          TE = TE->Sig;
          *TC = (*TC)->Sig;
          SeImprimieronErrores++;
       } /* agregar mensajes de error de la linea al listado */
       return -1;
    } /* fin de linea erronea, no ensamblada */

    ptr = strtok (cad,", \n\r\t");

    while ((ptr!=NULL) && (strcmpi(ptr,"END") != 0) && (strpos(ptr,';') != 0)) {

       if (strpos(ptr,':') == strlen(ptr)-1) ptr = strtok (NULL,", \n\r\t");

       if (BuscaSimboloEnTabla(SimTabla,ptr)) {
          strcpy (c,ptr);
          ptr = strtok (NULL,", \n\r\t");
       }

      if (EsMnemonico(ptr)) {
          sprintf (mnemo,"%s ",ptr);
          ObtenerCodigo_Y_Longitud (ptr,&Cod,&L,&NO);
          switch (NO) {
             case 0: {
                      ObtenerCodigo_Y_Longitud (mnemo,&Cod,&L,&NOps);
                      sprintf (CAux,"  %04X  %02X  ",(*TC)->C,Cod);
                      AgregaLineaAListado (&Listado,CAux);
                      while ((ptr = strtok (NULL,", \n\r\t")) != NULL);
                      break;
             } /* no operandos */

             case 1: {
                if ((ptr=strtok(NULL,", \n\r\t"))!=NULL)
                   if (BuscaSimboloEnTabla(SimTabla,ptr)) {   /*¨s¡mbolo?*/
                      if (EsConDATA(mnemo)) strcat (mnemo,"DATA");
                      if (EsConDATA16(mnemo)) strcat (mnemo,"DATA16");
                      if (EsConADDR(mnemo)) strcat (mnemo,"ADDR");
                      if (EsConPORT(mnemo)) strcat (mnemo,"PORT");
                      ObtenerCodigo_Y_Longitud (mnemo,&Cod,&L,&NOps);
                      ObtenerValor (SimTabla,&ptr); Val = atoi (ptr);
                      InvierteWord (Val,&c1,&c2);
                      sprintf (CAux,"  %04X  %02X  %02X%02X ",(*TC)->C,Cod,c1,c2);
                      AgregaLineaAListado (&Listado,CAux);
                      while ((ptr = strtok (NULL,", \n\r\t")) != NULL);
                   } /* si es s¡mbolo */
                   else if ((ptr[0]=='(')||isdigit(ptr[0])) {     /* ¨valor? */
                           if (EsConDATA(mnemo)) strcat (mnemo,"DATA");
                           if (EsConDATA16(mnemo)) strcat (mnemo,"DATA16");
                           if (EsConADDR(mnemo)) strcat (mnemo,"ADDR");
                           if (EsConPORT(mnemo)) strcat (mnemo,"PORT");
                           ch = ptr[strlen(ptr)-1];
                           if (EsNumCad(ptr))
                              switch (toupper(ch)) {
                                 case 'H': st = conv (ptr,16); break;
                                 case 'O': st = conv (ptr,8);  break;
                                 case 'D': st = conv (ptr,10); break;
                                 default:  st = ptr;
                              } /* switch */
                           else st = ptr;
                           expr = ptr;
                           ObtenerExpresion (&Val);
                           InvierteWord (Val,&c1,&c2);
                           ObtenerCodigo_Y_Longitud (mnemo,&Cod,&L,&NOps);
                           sprintf (CAux,"  %04X  %02X  %02X%02X",(*TC)->C,Cod,c1,c2);
                           AgregaLineaAListado (&Listado,CAux);
                           while ((ptr = strtok (NULL,", \n\r\t")) != NULL);
                        }
                        else if (ptr[0]=='\'' && isascii(ptr[1]) &&
                                 ptr[2]=='\'') {
                                if (EsConDATA(mnemo)) strcat (mnemo,"DATA");
                                if (EsConDATA16(mnemo)) strcat (mnemo,"DATA16");
                                if (EsConADDR(mnemo)) strcat (mnemo,"ADDR");
                                if (EsConPORT(mnemo)) strcat (mnemo,"PORT");
                                ObtenerCodigo_Y_Longitud (mnemo,&Cod,&L,&NOps);
                                sprintf (CAux,"  %04X  %02X  %02X",(*TC)->C,Cod,ptr[1]);
                                AgregaLineaAListado (&Listado,CAux);
                                while ((ptr = strtok (NULL,", \n\r\t")) != NULL);
                             } /* es caracter ascii */
                             else {
                                strcat (mnemo,ptr);
                                ObtenerCodigo_Y_Longitud (mnemo,&Cod,&L,&NOps);
                                sprintf (CAux,"  %04X  %02X ",(*TC)->C,Cod);
                                AgregaLineaAListado (&Listado,CAux);
                                while ((ptr = strtok (NULL,", \n\r\t")) != NULL);
                             }
                break;
             } /* si es un operando */
             case 2: {
                Precedido = FALSE;
                while ((ptr=strtok(NULL,", \n\r\t"))!=NULL) {
                   if (BuscaSimboloEnTabla(SimTabla,ptr)) {   /*¨s¡mbolo?*/
                      if (EsConDATA(mnemo)) strcat (mnemo,"DATA");
                      if (EsConDATA16(mnemo)) strcat (mnemo,"DATA16");
                      if (EsConADDR(mnemo)) strcat (mnemo,"ADDR");
                      if (EsConPORT(mnemo)) strcat (mnemo,"PORT");
                      if (!Precedido) strcat (mnemo,",");
                      else {
                         ObtenerCodigo_Y_Longitud (mnemo,&Cod,&L,&NOps);
                         ObtenerValor (SimTabla,&ptr); Val = atoi (ptr);
                         InvierteWord (Val,&c1,&c2);
                         sprintf (CAux,"  %04X  %02X  %02X%02X ",(*TC)->C,Cod,c1,c2);
                         AgregaLineaAListado (&Listado,CAux);
                         while ((ptr = strtok (NULL,", \n\r\t")) != NULL);
                      } /* else */
                   } /* if es s¡mbolo */
                   else if ((ptr[0]=='(')||isdigit(ptr[0])) {     /* ¨valor? */
                           if (EsConDATA(mnemo)) strcat (mnemo,"DATA");
                           if (EsConDATA16(mnemo)) strcat (mnemo,"DATA16");
                           if (EsConADDR(mnemo)) strcat (mnemo,"ADDR");
                           if (EsConPORT(mnemo)) strcat (mnemo,"PORT");
                           if (!Precedido) strcat (mnemo,",");
                           else {
                              ch = ptr[strlen(ptr)-1];
                              if (EsNumCad(ptr))
                                 switch (toupper(ch)) {
                                    case 'H': st = conv (ptr,16); break;
                                    case 'O': st = conv (ptr,8);  break;
                                    case 'D': st = conv (ptr,10); break;
                                    default:  st = ptr;
                                 } /* switch */
                              else st = ptr;
                              expr = st;
                              ObtenerExpresion (&Val);
                              InvierteWord (Val,&c1,&c2);
                              ObtenerCodigo_Y_Longitud (mnemo,&Cod,&L,&NOps);
                              sprintf (CAux,"  %04X  %02X  %02X%02X",(*TC)->C,Cod,c1,c2);
                              AgregaLineaAListado (&Listado,CAux);
                              while ((ptr = strtok (NULL,", \n\r\t")) != NULL);
                           } /* else */
                        } /* if expresion/valor */
                        else if (ptr[0]=='\'' && isascii(ptr[1]) &&
                                 ptr[2]=='\'') {
                                if (EsConDATA(mnemo)) strcat (mnemo,"DATA");
                                if (EsConDATA16(mnemo)) strcat (mnemo,"DATA16");
                                if (EsConADDR(mnemo)) strcat (mnemo,"ADDR");
                                if (EsConPORT(mnemo)) strcat (mnemo,"PORT");
                                ObtenerCodigo_Y_Longitud (mnemo,&Cod,&L,&NOps);
                                sprintf (CAux,"  %04X  %02X  %02X",(*TC)->C,Cod,ptr[1]);
                                AgregaLineaAListado (&Listado,CAux);
                                while ((ptr = strtok (NULL,", \n\r\t")) != NULL);
                             } /* si op. es caracter ascii */
                        else {
                           strcat (mnemo,ptr);
                           if (!Precedido) strcat (mnemo,",");
                           else {
                              ObtenerCodigo_Y_Longitud (mnemo,&Cod,&L,&NOps);
                              sprintf (CAux,"  %04X  %02X ",(*TC)->C,Cod);
                              AgregaLineaAListado (&Listado,CAux);
                              while ((ptr = strtok (NULL,", \n\r\t")) != NULL);
                           } /* 2§ */
                        } /* else */
                   Precedido = TRUE;
                } /* while haya tokens */
                break;
             } /* case dos operandos */
          } /* switch */
      } /* proceso de mnem¢nicos */

       if (EsPseudoOperador(ptr)) {       /* procesar pseudo ops. */
          sprintf (CAux,"  %04X  ",(*TC)->C);

          if (strcmpi("ORG",ptr)==0) {
             sprintf (CAux,"  %04X  ",(*TC)->C);
             AgregaLineaAListado (&Listado,CAux);
             while ((ptr = strtok (NULL,", \n\r\t")) != NULL);
          }

          if (strcmpi("DW",ptr) == 0) {
             while (((ptr=strtok(NULL,", \n\r\t"))!=NULL) &&
                    (strpos(ptr,';') != 0)) {
                expr = ptr;
                ObtenerExpresion (&Val);
                InvierteWord (Val,&c1,&c2);
                sprintf (c," %02X%02X",c1,c2);
                strcat (CAux,c);
             } /* while */
             strcat (CAux,"     ");
             strcat (CAux,aux);
             AgregaLineaAListado (&Listado,CAux);
             while ((ptr = strtok (NULL,", \n\r\t")) != NULL);
          } /*if DW */

          if (strcmpi("DB",ptr) == 0) {
             while (((ptr=strtok(NULL,", \n\r\t"))!=NULL) &&
                    (strpos(ptr,';') != 0)) {
                expr = ptr;
                ObtenerExpresion (&Val);
                sprintf (c," %02X",Val);
                strcat (CAux,c);
             } /* while */
             strcat (CAux,"     ");
             AgregaLineaAListado (&Listado,CAux);
             while ((ptr = strtok (NULL,", \n\r\t")) != NULL);
          } /* if DB */

          if (strcmpi("DS",ptr) == 0) {
             if (((ptr=strtok(NULL,", \n\r\t"))!=NULL) &&
                 (strpos(ptr,';') != 0)) {
                expr = ptr;
                ObtenerExpresion (&Val);
                sprintf (c," %04X",Val);
                strcat (CAux,c);
             } /*if*/
             AgregaLineaAListado (&Listado,CAux);
             while ((ptr = strtok (NULL,", \n\r\t")) != NULL);
          } /* if DS */

          if (strcmpi("EQU",ptr) == 0) {
             if ((BuscaSimboloEnTabla (SimTabla,c))==TRUE) {
                sig = (char *) malloc (strlen(c)+1);
                strcpy (sig,c);
                ObtenerValor (SimTabla,&sig); Val = atoi (sig);
                free (sig);
                sprintf (CAux,"  %04X  = %02X ",(*TC)->C,Val);
                MoverCont = FALSE;
                AgregaLineaAListado (&Listado,CAux);
                while ((ptr = strtok (NULL,", \n\r\t")) != NULL);
             } /* if existe simbolo */
          } /* if EQU */
       } /*if pseudooperador */
    } /* while haya tokens y no END */

    if (strcmpi("END",ptr) == 0) {
       sprintf (CAux," %s  ","    ");
       AgregaLineaAListado (&Listado,CAux);
       Final = TRUE;
    }
    return 0;
} /* fin del segundo paso */

void PrimerPaso (TipoLinea *Texto)
{
    while ((Texto != NULL)&&(!Final)) {
       AgregaContATabla (&TablaCont,Cont);
       Ensamblar_PrimerPaso (Texto->Linea);
       ActLinea++;
       Texto = Texto->Sig;
    }
}

void SegundoPaso (TipoLinea *TextoAEnsamblar,TipoTablaError *TE)
{
    int k = 0;

    Final = FALSE;
    while ((TextoAEnsamblar != NULL) && (!Final)) {
        Ensamblar_SegundoPaso (TextoAEnsamblar->Linea,TE,&TablaCont);
        TextoAEnsamblar = TextoAEnsamblar->Sig;
        if (MoverCont) TablaCont = TablaCont->Sig;
        else MoverCont =  TRUE;
        for (k = 0; k < SeImprimieronErrores; k++)
           TE = TE->Sig;
        SeImprimieronErrores = 0;
        ActLinea++;
    }
}

void CreaListado (TipoLinea *L,TipoLinea *T)
{
    struct time hora;
    struct date dia;
    fprintf (Salida,"\n %s \n\n",Titulo);
    fprintf (Salida," %s\n\n",NomArchASM);
    gettime (&hora);
    getdate (&dia);
    fprintf(Salida,"  Fecha:  %d/%d/%d        ",dia.da_day,dia.da_mon,dia.da_year);
    fprintf(Salida,"Hora:  %2d:%02d:%02d.%02d\n",hora.ti_hour, hora.ti_min, hora.ti_sec, hora.ti_hund);
    ActLinea = 1;
    while (L!=NULL) {
          fprintf (Salida," \n %d  %s %40.90s",L->NumLinea,L->Linea,T->Linea);
          L = L->Sig;
          T = T->Sig;
          ActLinea++;
    }
}

void AgregaSimTabla_a_LST (void)
{
    fprintf (Salida,"\n %s \n\n",Titulo);
    fprintf (Salida," Tabla de S¡mbolos y Etiquetas:\n\n ");
    fprintf (Salida," Nombre del Simbolo/Etiqueta:            Valor:\n");
    while (SimTabla != NULL) {
        fprintf (Salida,"\n  %-40s %04X",SimTabla->Nombre,SimTabla->Valor);
        SimTabla = SimTabla->Sig;
    }
    fprintf (Salida,"\n");
}

void AgregaResumenErrores_a_LST_y_CRT (void)
{
    fprintf (Salida,"\n %s \n\n",Titulo);
    fprintf (Salida," Resumen de Errores:\n");
    while (TablaErrores != NULL) {
        fprintf (Salida,"\n %d %s",TablaErrores->Num,TablaErrores->Mensaje);
        cprintf ("\r\n %s",TablaErrores->Mensaje);
        TablaErrores = TablaErrores->Sig;
    }
    cprintf (" \n\r\n\r Total de errores:      %d",TotalErrores);
    fprintf (Salida," \n\n Total de errores:      %d",TotalErrores);
    cprintf (" \n\r Total de advertencias: %d",TotalAdvertencias);
    fprintf (Salida," \n Total de advertencias: %d\n",TotalAdvertencias);
    fprintf (Salida,"\n\n (c) RODOLFO GONZALEZ GONZALEZ.");
}

/*                            Programa Principal                            */

main (int argc, char *argv[])
{
    Presenta();
    if (SiPideAyuda (argc,argv)) return 1;
    if (AnalizaNomArchs(argc,argv) == 1) {
       sound(1440); delay (100); nosound(); Salir ();
       return 1;
    }
    else
       if ((AbreArchivos (NomArchASM,NomArchLST)) != 0) {
          sound(1440); delay (100); nosound(); Salir ();
          return 1;
       }
    while ((fgets(Cad,MAXLINELEN,Entrada) != NULL)) {
       ActLinea++;
       if (CargaArchivoFuente (Cad,&TextoFuente) == 41) {
          Salir (); return 1;
       }
    }
    fclose (Entrada);
    if (ActLinea == 0) {
       Errores (19,NomArchASM,0,"Listado no creado.");
       cprintf (" \n\r\n\r Total de errores:      %d",TotalErrores);
       cprintf (" \n\r Total de advertencias: %d",TotalAdvertencias);
       Salir ();
       return 1;
    }
    cprintf ("\n\r Ensamblando %d lineas. Archivo fuente: %s\n\r",ActLinea,NomArchASM);
    cprintf (" \n\r Paso 1 de 2 . . .\n\r");
    Cont = 0; Pasada = ActLinea = 1;
    CopiaTexto (TextoFuente,&TextoAEnsamblar);
    PrimerPaso (TextoAEnsamblar);
    Pasada = 2; Cont = 0; ActLinea = 1;
    TextoAEnsamblar = NULL;
    cprintf (" Paso 2 de 2 . . .\n\r");
    CopiaTexto (TextoFuente,&TextoAEnsamblar);
    SegundoPaso (TextoAEnsamblar,TablaErrores);
    cprintf ("\n\r Fin de ensamble.\n\r");
    CreaListado (Listado,TextoFuente);
    AgregaSimTabla_a_LST ();
    AgregaResumenErrores_a_LST_y_CRT ();
    fclose (Salida);
    Salir ();
    return 0;
}
