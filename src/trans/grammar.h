/***************************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                  Структуры, описывающие грамматику
//       Анисимов Д.В.                                    сегодня
/***************************************************************************/
# ifndef GRAMMAR_H
# define GRAMMAR_H

# include <stdio.h>
# include <list.h>
# include <lexer.h>

const int LNAME =  40 ; // длинна слова файла lang.txt

DECLARE_LIST_TYPE( short,t_shortList );
DECLARE_LIST_TYPE( long ,t_longList );

struct t_Grammar ;
struct t_Lang ;

// -------------- возможные значения параметров ------------------
struct t_Value
{  char *Name ;      // значение параметра например "Творительный"
} ;
DECLARE_LIST_TYPE( t_Value,t_ValueList );

// -------------- параметр слова - падеж и тд --------------------
struct t_Param
{
   char     *Name ;  // название параметра например "Падеж"
   t_ValueList Value ;  // возможные значения параметров

    t_Param( void );
   ~t_Param( void );
   void  init( void );
   void  del( void );
} ;
DECLARE_LIST_TYPE( t_Param,t_ParamList );

// -------------- параметр слова - падеж и тд --------------------
struct t_Param1
{  short param ;    // тип параметра в соответствии с Lang->Param
   char  *Name ; // название параметра например "Падеж"
   char  Dir ;      // 1-источник 0-приемник
   char  value ;    // значение параметра, если -1 значит не задан
};
DECLARE_LIST_TYPE( t_Param1,t_Param1List );

// ------------- форма слова -------------------------------------
struct t_Form
{
   char value[10];  // порядок параметров - в соответствии с Part->Param
   void init( );
   t_Form();
};

// =========== все, что относится к грамматическому словарю ======

DECLARE_LIST_TYPE( t_Form,t_FormList );

// ------------ переименование грамматических параметров ---------
struct t_Rename
{  char *Reduce ;  // сокращенное обозначение параметра (например "f")
   char *Full ;    // полное имя значения параметра (например "женский")
};
DECLARE_LIST_TYPE( t_Rename,t_RenameList );

// ------------ формат, описывающий грамматический словарь -------
struct t_Format
{
   char     *Name ;     // имя формата
   short     i_part ;   // индеск части речи в соответствии с  
                        // t_Lang->Part[this->i_part]
   t_FormList Record ;  // список возможных форм слова

    t_Format( void );
   ~t_Format( void );
   void  init( void );
   void  del( void );
};
DECLARE_LIST_TYPE( t_Format,t_FormatList );

DECLARE_LIST_TYPE( char *,t_strList );
// ----------- файлы, в которых лежат грамматические словари ---
struct t_File
{
   short  i_part,       // часть речи в соответствии с t_Lang->Part[this->i_part]
	  i_format ;    // индекс формата
   t_strList  FileName ;// имена файлов, в которых лежит эта часть речи

    t_File( void );
   ~t_File( void );
   void  init( void );
   void  del( void );
};
DECLARE_LIST_TYPE( t_File,t_FileList );

// =========== все, что относится к словарю переводов ==========
// -------------- описание части речи в словаре переводов ------
struct t_PartDsk
{  char  *Tag ;        // тег части речи в словаре переводов
   char  *Name ;       // имя части речи
   short  i_part ;     // номер части речи
};

DECLARE_LIST_TYPE( t_PartDsk, t_PartDskList )

// -------------- формат, описывающий словарь переводов -------
struct t_Format1
{  char *Name ;             // имя формата
   t_PartDskList SouPart,   // список переименования структур
                 DstPart ;
   t_RenameList  Rename ;   // список переименования параметров

   void   init( void );
   void   del( void );
   void   make_part( t_Grammar *Grammar );
   short  find_struct( char Dst, char *Str );
   char  *get_tag( char Dst, short i_struct );
   char  *get_name( char Dst, short i_struct );
};
DECLARE_LIST_TYPE( t_Format1, t_Format1List )

// =========== все, что относится к структуре =================
// ----------- тип структуры ----------------------------------
enum e_Type
{  TNULL,     // нет в этом месте структуры - пустая запись
   TCONST,    // неизменяемый литерал
   TCONST1,   // неизменяемый литерал из словаря
   TWORD,     // слово
   TWORD0,    // пустой выбор (@00)
   TSTRUCT,   // структура
   TSTRUCT1,  // фиксированная структура из словаря
   TSTRUCT2,  // произвольная структура из словаря
   TSELECT,   // выбор
   TSELECT1,  // несимметричный выбор
   TSELECT2,  // "собирательный" выбор
   TENUM,     // "препроцессорный" выбор
   TUNORDER,  // структура, члены которой могут менять порядок
   TUNORDER1,
   TUNORDER2
} ;
// ----------- макросы типа структуры --------------------------
# define IF_WORD( T )    ((T)==TCONST   || (T)==TCONST1   || (T)==TWORD)
# define IF_STRUCT( T )  ((T)==TSTRUCT  || (T)==TSTRUCT1  || (T)==TSTRUCT2)
# define IF_SELECT( T )  ((T)==TSELECT  || (T)==TSELECT1  || (T)==TSELECT2)
# define IF_UNORDER( T ) ((T)==TUNORDER || (T)==TUNORDER1 || (T)==TUNORDER2)
# define IF_CONSTR( T )  ((T)==TSTRUCT  || (T)==TSTRUCT1  || (T)==TSTRUCT2\
		      || (T)==TSELECT || (T)==TSELECT1 || (T)==TSELECT2\
		      || (T)==TUNORDER || (T)==TUNORDER1 || (T)==TUNORDER2)

// -------------- половинка перевода ---------------------------
enum e_Half
{  FROM,
   TO
} ;

// ---------- элемент заголовка таблицы трансляции параметров -----
struct t_2Index
{  short i1,       // индекс слова 
	 i2,       // индекс параметрв в структурк
	 i_param ; // индекс параметра в языке
   char *Name ;
};
DECLARE_LIST_TYPE( t_2Index, t_2IndexList );

// ---------- направление передачи параметров внутри структуры -----
struct t_Relation
{  char s1,p1 ; // индекс структуры и параметра источника
   char s2,p2 ; // индекс структуры и параметра приемника
};
DECLARE_LIST_TYPE( t_Relation, t_RelationList );

// ---------- способ обойти транслятор - близнец от t_RelationList --
struct t_RelationList1
{  
   t_Relation *list ;
   long        j ;
   void  operator = ( t_RelationList &a ){ list=a.list ; j=a.j ;  }
};

// ------------------------ описание слова -------------------------
struct t_Word
{  e_Type type ;       // слово,структура или выбор
   char  *Name ;   // имя структуры
   char  *literal ;// строковая константа
   t_Param1List Param; // параметры структуры
   short  order ;      // индекс упорядочивания
   short  i_struct ;   // индекс структуры в массиве t_All_Data->Trans
   char   use ;
   char   meaning_use ; // признак того, что значение передается наверх

	  t_Word( void );
   void   init( void );
   void   del( void );
   void   operator = ( t_Word &W );
};
DECLARE_LIST_TYPE( t_Word,t_WordList );

// ------------------- описание словосочетания ------------------
struct t_Struct
{  e_Type         type ;     // слово,структура или выбор
   char          *Name ;     // имя структуры
   long           i_str ;    // строка, в которой содержится ее описание
   t_Param1List   Param ;    // параметры структуры
   t_WordList     Word ;     // составляющие
   t_RelationList Relation ; // соответствие параметров
			     // если структура s1 - индекс слова
			     // если выбор     s1 - индекс варианта

   t_Struct( void );
   void operator = ( t_Struct &S );
   void init( void );
   void del( void );
   long read( long Begin, t_Lang *Lang );
   long trans_word1( long Begin );
   long trans_word ( long Begin );
   long trans_param( long Begin, t_Lang *Lang );
   long trans_order( long Begin, t_Word *Word );
   void trans_relation( void );
   void tst_relation( void );
   t_2Index  find_param2( char *Str );
};
DECLARE_LIST_TYPE( t_Struct,t_StructList );

enum e_Table
{   T_SOU, T_TRANS, T_DST
};

// --------------------- таблица трансляции параметров ----------
struct t_Table
{  e_Table      type ;   // языка источника, языка приемника, или передачи
   short        Size ;   // длинна строки таблицы
   t_2IndexList In,Out ; // входные и выходные параметры
   t_shortList  Value ;  // значения параметров

   void   init( void );
   void   del( void );
   void   operator = ( t_Table &T );
};
DECLARE_LIST_TYPE( t_Table, t_TableList );

// =========== глобальные и фундаментальные структуры ===========
// ----------- трансляционная пара ------------------------------
struct t_Trans
{  t_Struct    From,To ;      // описание структуры в соответствующем языке
   t_shortList Relation1,     // соответствие частей From->To
	       Relation2 ;    // соответствие частей To->From
   t_strList   Param1,Param2 ;// имена параметров той и другой структуры
   t_TableList Table ;        // таблицы трансляции параметров

	 t_Trans( void );
   void  operator = ( t_Trans &SS );
   void  init( void );
   void  del( void );
   void  make_order( void );
   void  make_index_table( void );
};
DECLARE_LIST_TYPE( t_Trans, t_TransList );

// -------------- описание языка (источника или приемника) --------
struct t_Lang     
{
   char          Name[LNAME] ;
   t_ParamList   Param ;  // параметры 
   t_StructList  Part ;   // части речи
   t_FormatList  Format ; // форматы грамматических словарей
   t_FileList    File ;   // файлы грамматических словарей
   t_Lexer      *Lexer ;
   char          To ;     // признак того, что это язых-приемник

   void del( void );
   long read( t_Lexer *Lexer, long B );
   void read_format( long Begin, t_Format *Format );
};

// ------------- формат словаря для человека ----------------------
struct t_Format2
{  char *Name ;        // имя формата
   char  Divider[10] ; // разделитель слова и перевода
   char  fsort ;       // как сортировать слова

   void   init( void );
   void   del( void );
};
DECLARE_LIST_TYPE( t_Format2, t_Format2List );

// ---------------------- все структуры, описывающие язык ---------
class t_Grammar
{
   t_Lang         From ,To ; // язык оригинала и перевода
   t_Format1List  Format1 ;  // форматы переводов
   t_Format2List  Format2 ;  // форматы словарей для человека
   t_FileList     File1 ;    // имена файлов переводов
   t_FileList     FilePre ;  // имена файлов препроцессоров
   t_FileList     FilePost ; // имена файлов постпроцессоров
public:
   t_FileList     File2 ;    // имена файлов словарей для человека
private:
   t_TransList    Trans ;    // список всех трансляционных пар 
   short          i_Number ; // часть речи, которая числительное
   short          i_All ;    // главная конструкция "правильного" перевода
   short          i_Surmise ;// конструкция "догадка"
   short          i_Irregular ; // конструкция "междометия"
   short          i_Unknown ; // "@знаковые_части_речи"
public:
   t_Lexer        Lexer ;    // лексер, который содержит все имена и т.п. 

public:
   char           make_all( char *File );
   short          find_struct( e_Half Half, char *Str );
   t_Trans&       operator []( long i );
   short          n_trans( );
   short          main_struct( );
   short          i_number( );
   short          i_all( );
   short          i_surmise( );
   short          i_irregular( );
   short          i_unknown( );
   t_Lang&        from();
   t_Lang&        to();
   t_Format1List& format1();
   t_FileList&    file1();
   t_FileList&    file_pre();
   t_FileList&    file_post();
private:   
   void    copy_words( void );
   void    read( char *File );
   long    read_slowo1( long Begin );
   long    read_slowo2( long Begin );
   long    read_format1( long Begin, t_Format1 *Format );
   long    read_format2( long Begin, t_Format2 *Format );
   long    read_trans( long Begin );
   long    read_trans1( long Begin );
   long    read_table( long Begin, t_Trans *TT );
   void    make_index_struct( void );
   void    make_index_param0( long i_trans );
   void    make_index_param( e_Half Half, long i_trans, long i_word );
   void    make_index( void );
   void    index_word( t_Word *W, e_Half Half );
   short   find_format1( char *Word );
   short   find_format2( char *Word );
   void    del( void );
};

// -------- данные о возможных направлениях перевода -----------
struct t_DirTrans
{  char *Dir,*Name ;
};

extern t_Grammar   Grammar ;
extern t_DirTrans *DirTrans ;
extern t_DirTrans *z_DirTrans ;
extern FILE       *File_Error ;
extern char        Read_Error ;
extern FILE *File_Error ;
extern char  Read_Error ;

short  find_param( t_ParamList  *List, char *Str );
short  find_format(t_FormatList *List, char *Str );
short  find_value( t_ValueList  *List, char *Str );
t_DirTrans *current_dir_trans( void );

# define i_main_struct Grammar.main_struct( )

# endif
