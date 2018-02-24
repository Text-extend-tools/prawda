/***************************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                 словарь переводов слов и словосочетаний
//       Анисимов Д.В.                                    сегодня
/***************************************************************************/
# ifndef SLOWO3_H
# define SLOWO3_H

# include <mylib.h>
# include <grammar.h>

struct t_sForm
{
   unsigned int v0 : 4 ;
   unsigned int v1 : 4 ;
   unsigned int v2 : 4 ;
   unsigned int v3 : 4 ;
   unsigned int v4 : 4 ;
   unsigned int v5 : 4 ;
   unsigned int v6 : 4 ;
   unsigned int v7 : 4 ;

   void init( );
   t_sForm();
   void operator = ( t_Form &F );
   t_Form form( );
};

struct t_sWord
{  long  sy ;        // ссылка на слово       t_Slowo3->Mass
   char *str ;       // ссылка на слово       t_Slowo3->Mass
   short i_struct ;  // индекс структуры в    t_Grammar->Trans
   short order  ;    // смысловой индекс типа<1>
   t_sForm Param ;    // константные параметры
} ;

// -------- структура оригинала или перевода ----------
struct t_sStruct
{  long     sy_word ;  // индекс первого слова структуры в t_Slowo3->Word
		       // сюда же можно запихнуть число n_From при трансляции
   t_sWord *Word ;     // ссылка на то же самое
   short    n_Word ;   // число слов в этой структуре
   short    i_struct ; // индекс структуры в    t_Grammar->Trans
   t_sForm  Param ;    // константные параметры
   long     i_relation ;// индекс таблицы передачи параметров t_Slowo3->Relation
} ;

// -------- одна строка перевода -------------------------
struct t_sRecord 
{  long  sy_struct ;    // индекс первой структуры в t_Slowo3->Struct
   short  n_struct ;     // число таких структур (минимум две)
   short  i_dict ;       // индекс словаря
};

// --------- элемент дерева поиска ------------------------
struct t_sTree
{  long  up,down,n_down ;
   long  first,last ;   // индекс первой и последней (включительно) Record, 
                        // входящих в это поддерево
   short rang ;         // номер слова в искомой структуре
   char  empty ;        // признак, в поддереве нельзя использовать
                        // алфавитный поиск
   char  unused ;
} ;

// ---------- словарь переводов ---------------------------------------
class t_Slowo3
{
private:
   char        FileName[40] ;
   t_Format1  *Format ;  // формат словаря
   uchar      *Mass ;    // массив слов, запись типа "S[word]=S[слово]"
   t_sWord    *Word ;    // ссылка на одну запись типа "S<1>(Р)[собака]"
   t_sStruct  *Struct ;  // ссылка на одну запись типа "GS:@0[a hot]S<1>[dog];"
   t_sRecord  *Record ;  // индекс Struct, с которой начинается очередная
			 // запись типа S[word]=S[слово]
   t_sTree    *Tree ;    // дерево для поиска структур
   t_Relation *Relation ;// таблицы передачи параметров
   
   long       n_Mass    ,l_Mass ;    // длинна массива Mass
   long       n_Word    ,l_Word ;
   long       n_Struct  ,l_Struct ;  // число и длинна массива структур
   long       n_Record  ,l_Record ;  // число -----// -------  записей
   long       n_Tree    ,l_Tree ;    // число -----// -------  Tree
   long       n_Relation,l_Relation; // число -----// -------  Relation

private:
   t_longList Antwort ;  // массив ответов
   char Type[256] ;

public:
	      t_Slowo3( void );
   void       del( void );
   void       read( char *Dir, t_Grammar *Gr );
   void       read( char *File, t_Format1 *Format );
   char       read_bin( char *File );
   void       write_bin( char *File );
   void       make( char *_Mass, t_Format1 *Format );
   void       s_universe( short i_word, short i_struct );
   void       find( char *From, short i_struct, t_longList *Antwort );
   char      *translate_word_i( char *From, long  i_slowo,  short i_slowo1=0 );
   char      *translate_word_s( char *From, short i_struct, short i_slowo1=0 );
   short      part( char *From, short *i_Part );
   t_Form     struct_param( long i_slowo, long i_variant=0 );
   t_Form     word_param( long i_slowo, long i_variant, long i_word );
   t_Format1       *format( void );
   t_sRecord       *record( long i_record );   
   long             n_record( void );
   char             is_atom( long i_record );
   t_sTree         *tree( long i_tree );
  inline t_sStruct *sstruct( long i_struct );
   t_sStruct       *get_from( long i_slowo );
   t_sStruct       *get_to( long i_slowo, long i_slowo1=0 );
   t_RelationList1  get_relation( long i_slowo, long i_variant );
   short            get_n_perevod( long i_slowo );
   char            *word_src(  long i_tree );
   char            *word_src1( long i_slowo );
   void             print( char *File );
   void             print_record( FILE *fw, long i_record );
   void             print_param( FILE *fw, char to, t_sStruct *S, short i_word );
   void             add_new( char *Dir, t_DirTrans *DT, char *str );
   void             add_new1( char *Str );
private:
   void      read0( char *File );
   void      make0( char *Mass );
   void      parse( void );
   void      parse1( long i, long &j_Word, long &j_Struct, long &j_Relation );
   void      make_relation1( char fdst, t_sStruct *S, char *Str );
   void      make_relation2( char fdst, t_sStruct *S, long i_word, char *Str, long &j_Relation );
   short     find_value1( short i_struct, short i_param, char fdst, char *Str1 );
   short     find_rename( char *Str );
   void      sort0( void );
   void      split_unorder( void );
   void      make_tree( void );
   void      s_universe1( short i_word, long i_tree );
   void      s_universe2( char *From, short i_word, long i_tree );
   char      find_bin(  char *From1, long i_tree );
   char      find_bin1(  char *From1, long i_tree );
   long      find_mutt( long i_tree );
   long      root_of_struct( short i_struct );
   void      calc_delta( char *Str, long &d_Word, long &d_Struct );
   long      find_insert( t_sRecord &R );
friend class t_Core ;
};

/*
(1) Синтаксический разбор вставляемой строки
   Вытащить из parse функцию parse_record()
   j_Struct j_Word j_Relation передать в качестве параметра
   
2) Раздвижка массивов Mass Word Struct Record Relation 
3) Вставка в Record
   1) Найти нужное место
   2) Собственно вставить
      Word Struct Record
      дописать Relation
*/

# endif


