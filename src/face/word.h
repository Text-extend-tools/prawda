/*******************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                   Ввод новых слов и выражений
//      Анисимов Д.В.                               сегодня
/*******************************************************************/

# include <core.h>
# include <window.h>

struct t_NewWord
{
   char *Mass ;
   long j_Mass,n_Mass,z_Mass ;
   
    t_NewWord( );
   ~t_NewWord( );
   void  add( char *str );
   char *get_all( void );
   char *get_new( void );
   void  remember( );
};

struct t_AddWord : public t_Win
{
   t_Format1   *FF ;
   t_shortList  sWord, sStruct ;
   t_shortList  dWord, dStruct ;
   t_shortList  sStruct1 ;
   t_shortList  dStruct1 ;

   char     sStr [200],dStr [200] ; // исходная строка
   char     sStr1[200],dStr1[200] ; // строка с нулями
   char     sStr2[200],dStr2[200] ; // строка с тегами
   t_sWord  Sou[20],  // слова источника
            Dst[20] ; // слова приемника
   char     Antwort[200] ;
   short    j_Sou,j_Dst ; 
   short    s_i_struct,d_i_struct ; // номер структуры источника и приемника
   short    Reg ;

            t_AddWord();
   void     init( void );
   void     init1( void );
   void     paint( void );
   void     paint_from( long i_Word );
   void     paint_to  ( long i_Word );
   e_WinMsg loop( void );
   void     make_struct( void );
   void     make_str(  char *S );
   void     make_str1( char to );
   void     make_str2( void );
   void     paint_struct( t_shortList &List, char to, short current );
   e_WinMsg tag_edit( t_shortList &List, char to, short i_Word, short &i_struct );
   e_WinMsg tag_edit1( char to );
   short    find( t_shortList &List, short z );
   char    *antwort();
};
