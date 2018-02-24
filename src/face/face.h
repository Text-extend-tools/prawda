/***************************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                    t_Windows - главный класс интерфейса
//     Анисимов Д.В.                                         сегодня
/***************************************************************************/

# ifndef FACE_H
# define FACE_H

# include <dir1.h>
# include <help.h>
# include <resource1.h>
# include <hotkey.h>
# include <window.h>

struct t_Windows
{
   char   Reg  ;   // текущее окно
   char   Reg1 ;   // предыдущее окно
   
          t_Windows();
   void   init( void );
   void   set_pos( void );
   void   main_loop( void );
   void   create( char *Name, char f_cp1251 ); // создать окно с файлом Name
   void   save_src( char f_cp1251 );           // запомнить оригинал
   void   save( char f_cp1251 );               // запомнить перевод
   void   save_as( char f_cp1251 );            // запомнить перевод под другим именем
   void   standen( void );
   void   exit( void );            // выйти из редактора
   void   find_forward( void );    // в редакторе поиск вперед
   void   find_backward( void );   // в редакторе поиск назад
   void   replace( void );         // в редакторе поиск-замена
   void   goto_str( void );        // в редакторе перейти в заданную строку
   void   help( char *Str );       // подказка на ключевое слово Str
   void   help_exit( void );       // выход из подсказки
   void   paint( void );
   void   trans( void );
   void   trans_frasa( void );
   void   trans_bilingvo( void );
   void   win_firstlook( void );
   e_WinMsg  win_path( void );
   e_WinMsg  win_path2( void );
   e_WinMsg  win_slowo( void );
   void   prev_frasa( void );
   void   next_frasa( void );

   char   schnell_taste( short Shift, short t1, short t2 );
};

extern t_Windows     Windows ;
extern t_HelpData    HelpData ;
extern t_Option      Option ;

void  figna( char *str );
void  message( char *str );
short fragen( char *str );
void  Window_help( void *Void );

# ifndef __WIN__
e_WinMsg kommand_yxf( short y, short x, char f, char *str, short lenght,
		   char EIN, char **str1=NULL, short n_str1=0 );
# endif

# endif
