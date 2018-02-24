/****************************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                         Грамматический словарь
//       Анисимов Д.В.                                    сегодня
/****************************************************************************/
# ifndef SLOWO2_H
# define SLOWO2_H
# include <grammar.h>

// -------- новые структуры -------------
struct t_Shabl
{  char  **end ;  // окончания шаблона
   long  *index ; // индексы окончаний в алфавитном порядке
   short n_end ;  // число его окончаний
};

struct t_compWord
{   char *str ;     // "начальная форма слова"
    short i_shabl ; // индекс шаблона
};

struct t_Slowo2
{
   char        f_compress ; // флаг, что словарь сжатый
   t_Lang     *Lang ;   // язык, которому принадлежит словарь
   t_Format   *Format ; // формат словаря

   long        n_Word ; // число слов
   short       n_Form ; // число форм
   char       *Mass ;   // Массив, в котором лежат слова
   long       *Word ;   // ссылки на начала слов [n_Word*n_Form]
   long       *reverce ;// алфавитный список форм
// -------- новые данные -----------------
   t_compWord *Word1 ;   // несколько форм слова [n_Word1]
   long       *sy_Word1 ;// массив сжатых слов [n_Word]
   t_Shabl    *Shabl ;   // массив шаблонов [n_Shabl]
   char       **EndSh ;  // общий массив Shabl->end 
   long        n_Word1 ;
   long        n_Shabl ;

   t_Slowo2( void );
   void    clr();
   void    set_lf( t_Lang *_Lang, t_Format *_Format );
   void    read( char *File, t_Lang *Lang, t_Format *Format );
   char   *normal( long index );
   char   *form( char *Str, t_Form *Form );
   short   quest( char *Str, long *Ant, t_Form *Form );
   char    word_exist( char *str );
   void    print_word( FILE *fw, long index );
private:
   void    read1( t_Lexer &Lexer, char *File );
   void    read_compress( t_Lexer &Lexer );
   void    freverce( void );
   void    freverce2( void );
   long    find( char *str );
   t_Form  add_form( t_Form *Post,t_Form *Var );
   char   *form( long i, t_Form *Form );
   char   *word_internal( long i_reverse );
   char   *decompress( long i_word1, long i_ende );
};

/*
Дальнейшие планы
1) утилита compress с точками с запятой
2) отладка
   1) чтения
   2) сортировки
   3) функций поиска
   4) комплексная отладка на сложной фразе
*/

# endif
