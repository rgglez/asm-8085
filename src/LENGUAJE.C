/****************************************************************************/
/*                  PROYECTO FINAL. PROGRAMACION DE SISTEMAS.               */
/*                     ENSAMBLADOR 8080/8085 DE DOS PASOS.                  */
/*                                                                          */
/*    Realizado por: RODOLFO GONZALEZ GONZALEZ.                             */
/*                                                                          */
/****************************************************************************/

/* LENGUAJE.C - Este m¢dulo maneja el conjunto de instrucci¢nes (mnem¢nicos)
   del 8080/8085                                                            */

#include <conio.h>
#include <string.h>

/*                           LISTA DE MNEMONICOS                            */

/* Formato: CODIGO,MNEMONICO,LONGITUD_CODIGO_OBJETO . Total instr.: 261     */

const TipoInstr TablaInstr[] =
     {{0X40,"MOV B,B",1,2},
      {0X41,"MOV B,C",1,2},
      {0X42,"MOV B,D",1,2},
      {0X43,"MOV B,E",1,2},
      {0X44,"MOV B,H",1,2},
      {0X45,"MOV B,L",1,2},
      {0X46,"MOV B,M",1,2},
      {0X47,"MOV B,A",1,2},

      {0X48,"MOV C,B",1,2},
      {0X49,"MOV C,C",1,2},
      {0X4A,"MOV C,D",1,2},
      {0X4B,"MOV C,E",1,2},
      {0X4C,"MOV C,H",1,2},
      {0X4D,"MOV C,L",1,2},
      {0X4E,"MOV C,M",1,2},
      {0X4F,"MOV C,A",1,2},

      {0X50,"MOV D,B",1,2},
      {0X51,"MOV D,C",1,2},
      {0X52,"MOV D,D",1,2},
      {0X53,"MOV D,E",1,2},
      {0X54,"MOV D,H",1,2},
      {0X55,"MOV D,L",1,2},
      {0X56,"MOV D,M",1,2},
      {0X57,"MOV D,A",1,2},

      {0X58,"MOV E,B",1,2},
      {0X59,"MOV E,C",1,2},
      {0X5A,"MOV E,D",1,2},
      {0X5B,"MOV E,E",1,2},
      {0X5C,"MOV E,H",1,2},
      {0X5D,"MOV E,L",1,2},
      {0X5E,"MOV E,M",1,2},
      {0X5F,"MOV E,A",1,2},

      {0X60,"MOV H,B",1,2},
      {0X61,"MOV H,C",1,2},
      {0X62,"MOV H,D",1,2},
      {0X63,"MOV H,E",1,2},
      {0X64,"MOV H,H",1,2},
      {0X65,"MOV H,L",1,2},
      {0X66,"MOV H,M",1,2},
      {0X67,"MOV H,A",1,2},

      {0X68,"MOV L,B",1,2},
      {0X69,"MOV L,C",1,2},
      {0X6A,"MOV L,D",1,2},
      {0X6B,"MOV L,E",1,2},
      {0X6C,"MOV L,H",1,2},
      {0X6E,"MOV L,M",1,2},
      {0X6F,"MOV L,A",1,2},

      {0X70,"MOV M,B",1,2},
      {0X71,"MOV M,C",1,2},
      {0X72,"MOV M,D",1,2},
      {0X73,"MOV M,E",1,2},
      {0X74,"MOV M,H",1,2},
      {0X75,"MOV M,L",1,2},
      {0X77,"MOV M,A",1,2},

      {0X78,"MOV A,B",1,2},
      {0X79,"MOV A,C",1,2},
      {0X7A,"MOV A,D",1,2},
      {0X7B,"MOV A,E",1,2},
      {0X7C,"MOV A,H",1,2},
      {0X7D,"MOV A,L",1,2},
      {0X7E,"MOV A,M",1,2},
      {0X7F,"MOV A,A",1,2},

      {0X3E,"MVI A,DATA",2,2},
      {0X06,"MVI B,DATA",2,2},
      {0X0E,"MVI C,DATA",2,2},
      {0X16,"MVI D,DATA",2,2},
      {0X1E,"MVI E,DATA",2,2},
      {0X26,"MVI H,DATA",2,2},
      {0X2E,"MVI L,DATA",2,2},
      {0X36,"MVI M,DATA",2,2},

      {0X31,"LXI SP,DATA16",3,2},
      {0X01,"LXI B,DATA16",3,2},
      {0X11,"LXI D,DATA16",3,2},
      {0X21,"LXI H,DATA16",3,2},
      {0X2A,"LHLD ADDR",3,1},
      {0X3A,"LDA ADDR",3,1},
      {0X0A,"LDAX B",1,1},
      {0X1A,"LDAX D",1,1},
      {0X02,"STAX B",1,1},
      {0X12,"STAX D",1,1},
      {0X32,"STA ADDR",3,1},
      {0X22,"SHLD ADDR",3,1},
      {0XF9,"SPHL",1,0},
      {0X80,"ADD B",1,1},
      {0X81,"ADD C",1,1},
      {0X82,"ADD D",1,1},
      {0X83,"ADD E",1,1},
      {0X84,"ADD H",1,1},
      {0X85,"ADD L",1,1},
      {0X86,"ADD M",1,1},
      {0X87,"ADD A",1,1},
      {0X88,"ADC B",1,1},
      {0X89,"ADC C",1,1},
      {0X8A,"ADC D",1,1},
      {0X8B,"ADC E",1,1},
      {0X8C,"ADC H",1,1},
      {0X8D,"ADC L",1,1},
      {0X8E,"ADC M",1,1},
      {0X8F,"ADC A",1,1},
      {0XC6,"ADI DATA",2,1},
      {0XCE,"ACI DATA",2,1},
      {0X09,"DAD B",1,1},
      {0X19,"DAD D",1,1},
      {0X29,"DAD H",1,1},
      {0X39,"DAD SP",1,1},
      {0X90,"SUB B",1,1},
      {0X91,"SUB C",1,1},
      {0X92,"SUB D",1,1},
      {0X93,"SUB E",1,1},
      {0X94,"SUB H",1,1},
      {0X95,"SUB L",1,1},
      {0X96,"SUB M",1,1},
      {0X97,"SUB A",1,1},
      {0X98,"SBB B",1,1},
      {0X99,"SBB C",1,1},
      {0X9A,"SBB D",1,1},
      {0X9B,"SBB E",1,1},
      {0X9C,"SBB H",1,1},
      {0X9D,"SBB L",1,1},
      {0X9E,"SBB M",1,1},
      {0X9F,"SBB A",1,1},
      {0XD6,"SUI DATA",2,1},
      {0XDE,"SBI DATA",2,1},
      {0X3C,"INR A",1,1},
      {0X04,"INR B",1,1},
      {0X0C,"INR C",1,1},
      {0X14,"INR D",1,1},
      {0X1C,"INR E",1,1},
      {0X24,"INR H",1,1},
      {0X2C,"INR L",1,1},
      {0X03,"INX B",1,1},
      {0X13,"INX D",1,1},
      {0X23,"INX H",1,1},
      {0X34,"INR M",1,1},
      {0X33,"INX SP",1,1},
      {0X3D,"DCR A",1,1},
      {0X05,"DCR B",1,1},
      {0X0D,"DCR C",1,1},
      {0X15,"DCR D",1,1},
      {0X1D,"DCR E",1,1},
      {0X25,"DCR H",1,1},
      {0X2D,"DCR L",1,1},
      {0X35,"DCR M",1,1},
      {0X3B,"DCX SP",1,1},
      {0X0B,"DCX B",1,1},
      {0X1B,"DCX D",1,1},
      {0X2B,"DCX H",1,1},

      {0XA0,"ANA B",1,1},
      {0XA1,"ANA C",1,1},
      {0XA2,"ANA D",1,1},
      {0XA3,"ANA E",1,1},
      {0XA4,"ANA H",1,1},
      {0XA5,"ANA L",1,1},
      {0XA6,"ANA M",1,1},
      {0XA7,"ANA A",1,1},
      {0XE6,"ANI DATA",2,1},

      {0XB0,"ORA B",1,1},
      {0XB1,"ORA C",1,1},
      {0XB2,"ORA D",1,1},
      {0XB3,"ORA E",1,1},
      {0XB4,"ORA H",1,1},
      {0XB5,"ORA L",1,1},
      {0XB6,"ORA M",1,1},
      {0XB7,"ORA A",1,1},
      {0XF6,"ORI DATA",2,1},

      {0XA8,"XRA B",1,1},
      {0XA9,"XRA C",1,1},
      {0XAA,"XR D",1,1},
      {0XAB,"XR E",1,1},
      {0XAC,"XRA H",1,1},
      {0XAD,"XRA L",1,1},
      {0XAE,"XRA M",1,1},
      {0XAF,"XRA A",1,1},
      {0XEE,"XRI DATA",2,1},

      {0XB8,"CMP B",1,1},
      {0XB9,"CMP C",1,1},
      {0XBA,"CMP D",1,1},
      {0XBB,"CMP E",1,1},
      {0XBC,"CMP H",1,1},
      {0XBD,"CMP L",1,1},
      {0XBE,"CMP M",1,1},
      {0XBF,"CMP A",1,1},
      {0XFE,"CPI DATA",2,1},

      {0X0F,"RRC",1,0},
      {0X07,"RLC",1,0},
      {0X1F,"RAR",1,0},
      {0X17,"RAL",1,0},
      {0X2F,"CMA",1,0},
      {0X37,"STC",1,0},
      {0X3F,"CMC",1,0},
      {0X27,"DAA",1,0},

      {0XC3,"JMP ADDR",3,1},
      {0XDA,"JC ADDR",3,1},
      {0XD2,"JNC ADDR",3,1},
      {0XCA,"JZ ADDR",3,1},
      {0XC2,"JNZ ADDR",3,1},
      {0XE2,"JPO ADDR",3,1},
      {0XEA,"JPE ADDR",3,1},
      {0XF2,"JP ADDR",3,1},
      {0XFA,"JM ADDR",3,1},
      {0XE9,"PCHL 1",3,1},
      {0XCD,"CALL ADDR",3,1},
      {0XDC,"CC ADDR",3,1},
      {0XD4,"CNC ADDR",3,1},
      {0XCC,"CZ ADDR",3,1},
      {0XC4,"CNZ ADDR",3,1},
      {0XE4,"CPO ADDR",3,1},
      {0XEC,"CPE ADDR",3,1},
      {0XF4,"CP ADDR",3,1},
      {0XFC,"CM ADDR",3,1},
      {0XC9,"RET",1,0},
      {0XD8,"RC",1,0},
      {0XD0,"RNC",1,0},
      {0XC8,"RZ",1,0},
      {0XC0,"RNZ",1,0},
      {0XE0,"RPO",1,0},
      {0XE8,"RPE",1,0},
      {0XF0,"RP",1,0},
      {0XF8,"RM",1,0},
      {0XC7,"RST 0",1,1},
      {0XCF,"RST 1",1,1},
      {0XD7,"RST 2",1,1},
      {0XDF,"RST 3",1,1},
      {0XE7,"RST 4",1,1},
      {0XEF,"RST 5",1,1},
      {0XF7,"RST 6",1,1},
      {0XFF,"RST 7",1,1},

      {0XC5,"PUSH B",1,1},
      {0XD5,"PUSH D",1,1},
      {0XE5,"PUSH H",1,1},
      {0XF5,"PUSH PSW",1,1},
      {0XC1,"POP B",1,1},
      {0XD1,"POP D",1,1},
      {0XE1,"POP H",1,1},
      {0XF1,"POP PSW",1,1},
      {0XE3,"XTHL",1,0},
      {0XF9,"SPHL",1,0},
      {0XF3,"DI",1,0},
      {0XFB,"EI",1,0},
      {0X00,"NOP",1,0},
      {0X76,"HLT",1,0},
      {0XDB,"IN PORT",2,1},
      {0XD3,"OUT PORT",2,1},
      {0X20,"RIM",1,0},
      {0X30,"SIM",1,0},
      {0X00,"",0,0}};

/* TABLA GENERAL DE MNEMONICOS */

const char TablaMnemo [][MAXINSTRLEN] =
        {{"MOV"},{"MVI"},{"LXI"},{"LHLD"},{"LDA"},{"LDAX"},{"STAX"},{"STA"},
         {"SHLD"},{"SPHL"},{"ADD"},{"ADC"},{"ADI"},{"ACI"},{"DAD"},{"SUB"},
         {"SBB"},{"SUI"},{"SBI"},{"INR"},{"NR"},{"INX"},{"DCR"},{"DCX"},
         {"ANA"},{"ANI"},{"ORA"},{"ORI"},{"XRA"},{"XR"},{"XRI"},{"CMP"},
         {"RRC"},{"RLC"},{"RAR"},{"RAL"},{"CMA"},{"STC"},{"CMC"},{"DAA"},
         {"JMP"},{"JC"},{"JNC"},{"JZ"},{"JNZ"},{"JPO"},{"JPE"},{"JP"},{"JM"},
         {"PCHL"},{"CALL"},{"CC"},{"CNC"},{"CZ"},{"CNZ"},{"CPO"},{"CPE"},
         {"CP"},{"CM"},{"RET"},{"RC"},{"RNC"},{"RZ"},{"RNZ"},{"RPO"},{"RPE"},
         {"RP"},{"RM"},{"RST"},{"PUSH"},{"POP"},{"XTHL"},{"SPHL"},{"DI"},
         {"EI"},{"NOP"},{"HLT"},{"IN"},{"OUT"},{"RIM"},{"SIM"},{""}};

/* TABLA DE PSEUDOOPERADORES */

char TablaPseudo [][MAXINSTRLEN/2] =
     {{"DB"},{"DW"},{"DS"},{"EQU"},{"ORG"},{"END"}};

/* Variables externa */

extern TipoSimbolo *SimTabla;

boolean EsMnemonico (char *token)
{
     int i;
     for (i = 0; strcmpi(TablaMnemo[i],""); i++)
         if (strcmpi(TablaMnemo[i],token) == 0) return TRUE;
     return FALSE;
}

boolean EsPseudoOperador (char *token)
{
     int i;
     for (i = 0; strcmpi(TablaPseudo[i],""); i++)
         if (strcmpi(TablaPseudo[i],token) == 0) return TRUE;
     return FALSE;
}

boolean EsOperando (char *token)
{
     if ((token[0] == '(')||(isdigit(token[0]))) return TRUE;
     else
        if ((strcmpi(token,"A")   == 0) ||
            (strcmpi(token,"B")   == 0) ||
            (strcmpi(token,"C")   == 0) ||
            (strcmpi(token,"D")   == 0) ||
            (strcmpi(token,"E")   == 0) ||
            (strcmpi(token,"H")   == 0) ||
            (strcmpi(token,"L")   == 0) ||
            (strcmpi(token,"SP")  == 0) ||
            (strcmpi(token,"PSW") == 0)) return TRUE;
        else
           if (BuscaSimboloEnTabla (SimTabla,token)) return TRUE;
           else
              if (token[0] == '\'' && isascii(token[1]) && token[2] == '\'')
                 return TRUE;
              else return FALSE;
}

boolean EsConDATA (char mnemo[MAXSIMLEN])
{
    char *s;

    s = strupr (mnemo);
    if ((strstr(s,"MVI")!=NULL)||
       (strstr(s,"ADI")!=NULL)||
       (strstr(s,"ACI")!=NULL)||
       (strstr(s,"SUI")!=NULL)||
       (strstr(s,"SBI")!=NULL)||
       (strstr(s,"ANI")!=NULL)||
       (strstr(s,"ORI")!=NULL)||
       (strstr(s,"XRI")!=NULL)) return TRUE;
    else return FALSE;
}

boolean EsConDATA16 (char mnemo[MAXSIMLEN])
{
    char *s;

    s = strupr (mnemo);
    if ((strstr(s,"LXI")!=NULL))
       return TRUE;
    else return FALSE;
}

boolean EsConADDR (char mnemo[MAXSIMLEN])
{
    char *s;

    s = strupr (mnemo);
    if ((strstr(s,"LHLD")!=NULL)||
        (strstr(s,"LDA")!=NULL)||
        (strstr(s,"STA")!=NULL)||
        (strstr(s,"SHLD")!=NULL)||
        (strstr(s,"JMP")!=NULL)||
        (strstr(s,"JMP")!=NULL)||
        (strstr(s,"JC")!=NULL)||
        (strstr(s,"JNC")!=NULL)||
        (strstr(s,"JZ")!=NULL)||
        (strstr(s,"JNZ")!=NULL)||
        (strstr(s,"JPO")!=NULL)||
        (strstr(s,"JPE")!=NULL)||
        (strstr(s,"JP")!=NULL)||
        (strstr(s,"JM")!=NULL)||
        (strstr(s,"CALL")!=NULL)||
        (strstr(s,"CC")!=NULL)||
        (strstr(s,"CNC")!=NULL)||
        (strstr(s,"CZ")!=NULL)||
        (strstr(s,"CNZ")!=NULL)||
        (strstr(s,"CPO")!=NULL)||
        (strstr(s,"CPE")!=NULL)||
        (strstr(s,"CP")!=NULL)||
        (strstr(s,"CM")!=NULL)) return TRUE;
     else return FALSE;
}

boolean EsConPORT (char mnemo[MAXSIMLEN])
{
   char *s;

   s = strupr (mnemo);
   if ((strstr(s,"IN")!=NULL)||(strstr(s,"OUT")!=NULL))
      return TRUE;
   else return FALSE;
}

void ObtenerCodigo_Y_Longitud (char *mnemo,unsigned int *Codigo,unsigned int *Long,int *NumOps)
{
     int i,j;
     boolean Encontrado = FALSE;
     j = strlen (mnemo);
     for (i = 0; ((strcmpi(TablaInstr[i].Nemotecnico,"") != 0) && (Encontrado == FALSE)); i++)
         if (strncmpi(TablaInstr[i].Nemotecnico,mnemo,j) == 0) {
            *Codigo = TablaInstr[i].Codigo;
            *Long = TablaInstr[i].LongitudOBJ;
            *NumOps = TablaInstr[i].NOperandos;
            Encontrado = TRUE;
         }
}
