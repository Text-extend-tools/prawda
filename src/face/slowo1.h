/***************************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                    "Человеческий" словарь (морда)
//     Анисимов Д.В.                                         сегодня
/***************************************************************************/
# ifndef SLOWO1_H
# define SLOWO1_H

# include <stdio.h>
# include <malloc.h>
# include <string.h>
# include <stdlib.h>
# include <dirent.h>
# include <sys/types.h>
# include <sys/stat.h>

# include <mylib.h>
# include <window.h>
# include <slowo.h>

/****************************************************************************/
class t_Slowo1 : public t_Win
{
   char   Sempty[256] ;   // пустая строка
   char   fView ;         // признак, что словарь содержит европейские буквы
   char   fDirect ;       // признак горизонтального расположения окон
   char   Str[40] ;       // строка, в которой набирается слово
   short  px ;            // позиция курсора в строке
   long   i_word ;        // индекс текущего слова
   t_Rect rect1 ;
   t_Rect rect2 ;
   short  x, y ;          // положение текста в окне перевода
   t_Slowo Buch ;

   public:
         t_Slowo1( void );
   void     set_pos( short x1, short y1, short x2, short y2, char fDirect );
   e_WinMsg main_loop( void );
   void     paint( long i_wort, long first_word );
   void     paint1( long i_wort, short x, short y );
   void     antwort( long i_wort );
   char     read_buch( char *Str );
   void     set_str( char *_Str );
};

# endif
