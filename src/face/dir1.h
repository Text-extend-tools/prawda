/***************************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                       лазалка по диску
//     Анисимов Д.В.                                         сегодня
/***************************************************************************/

# ifndef DIR_H
# define DIR_H

/* ----------------------- все, что относится к режиму открытия файлов --- */
struct t_File1
{  char name[32] ;    /* имя файла с расширением */
   long size ;        // размер файла
   long date ;
   long time ;
   char type ;        /* 0-файл 1-каталог */
} ;

class t_Dir
{  t_File1 *File ;
   short    Begin,        // первый файл, видимый на экране
	    i_File,       // текущий файл
	    n_File ;      // число файлов в каталоге
   char     shabl[32] ;   // шаблон файлов
   char     Name[32] ;
   char     Path[100] ;   // текущий каталог
public:
   short  direktor( char *Antwort, char f=0 );
   void   set_schabl( char *str );
   void   get_schabl( char *str );
   short  in_file( void );           // обработка клавиш в окне файлов
   void   in_schabl( void );         // обработка клавиш в окне шаблона
   short  in_name( void );           // обработка клавиш в окне нового файла
   void   paint_file( char f );      // нарисовать список файлов
   void   paint_schabl( void );      // нарисовать шаблон
   char   read_file( char *Path );   // прочесть список файлов в заданном каталоге
   void   karkas( char f );            // нарисовать все рамочки
   void   help( void );
   void   file_date( char *antwort, struct t_File1 &file );
   void   sort_file( void );
} ;

# endif
