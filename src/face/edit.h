/***************************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                  Простенький такой текстовый редактор
//     Анисимов Д.В.                                         сегодня
/***************************************************************************/

# ifndef EDIT_H
# define EDIT_H

# include <stdio.h>
# include <stdlib.h>
# include <window.h>

struct t_KeyWord
{  char Begin[32], l_Begin ;
   char End[32],   l_End ;
   char f ;
};

/* --------------------------------- образ текста в памяти --------------- */
class t_Text
{
public: // не забыть убрать это безобразие
   char     *Text ;         // участок текста хранящийся в памяти
   long      Length ;       // длинна текста, хранящегося в памяти
   long      i_str,i_pos ;  // текущая строка и ее позиция
   long      str_Txt ;      // число строк в массиве Text
   char      fformat ;      // признак "спецформата"
public:
	  t_Text( void );
	 ~t_Text( void );
   void   init( char *Text );         // инициировать текст строкой
   void   clear( void );
   void   write_file( char *name, char f_cp1251=0 ); // записать текст в файл
   char   read_file(  char *name, char f_cp1251=0 ); // прочитать тект из файла
   char   operator[]( long i );
   long   length( );
   long   str_txt( );
   long   pos_str( long i_Str );              // начало строки i_Str
   char  *get_str( long i_Str );                 // прочесть из текста строку номер i_Str
   char  *get_str( long i_Str, char *Buf );      // прочесть строку номер i_Str в буфер
   void   put_str_ipos( long i_pos, char *str ); // вставить в текст строку str
   void   put_str_istr( long i_str, char *str ); // вставить в текст строку str
   long   get_text1( long Str1, long Str2, char *List ); // вытащить из текста кусок
   long   get_text2( long Str1, long Str2, char *List ); // вытащить из текста кусок без стирания
   void   put_text1( long Str1, char *List, long l_List );
   void   put_text1p( long Pos1, long Pos2, char *List ); // вставить фразу взамен
   void   insert_str( long PY );      // вставить пустую строку
   void   delete_str( long PY );      // удалить строку PY
   short  max_length_str( void );
   char*  get_frasa( long pos );
   void   put_frasa( long pos, char *Frasa );
   long   next_frasa( long pos );
   long   prev_frasa( long pos );
   long   begin_frasa( long pos );
   long   end_frasa( long pos );
   char*  get_word_punkt( long pos );
   long   prev_word( long z );
   long   next_word( long z );
   
   char   (*is_const)( char *Str ); // функция определения "слова с точкой"
   long   get_pos_xy( long py, long px );
   void   get_xy_pos( long pos1, long *_py, long *_px );
} ;

/* --------- собственно редактор ---------------------------------------- */
class t_Edit : public t_Win
{
public :
   char    Bold ;         // признак активного окна
protected:
   t_Text *Text ;
   long    Begin_Y ;       /* первая строка, видимая в окне */
   short   Begin_X ;       /* первая позиция, видимая в окне */
   short   PX ;            /* положение курсора */
   long    PY ;            /* номер редактируемой строки */
   long    old_PY ;
   short   otm1,otm2,      /* номера выделеных символов */
           Otm1,Otm2 ;     /* номера выделеных символов (упорядоченные) */
   long    str1,str2,      /* номера выделеных строк */
           Str1,Str2 ;     /* номера выделеных строк (упорядоченные) */
   char    Str[256],       /* текущая строка */
           Buf[256] ;
   char    fview ;         // если fview==1 - то запрет редактирования
   char    REG ;           /* вставка-замена */
   char    Flag_b ;        /* флаг начала работы с буфером */   
   t_KeyWord *KeyWord ;     // слова, которые выделяются другим цветом
   long      n_KeyWord ;
public :
   void     begin( void );
   e_WinMsg main( void );
   e_WinMsg loop( short ZZ, short key1, short key2 );
   void     paint( void );                         // отобразить текст
   void     paint1( long Otm1, long Otm2 );        // отобразить текст
   void     paint_current( short Otm1, short Otm2 );// отобразить строку
   void     set_string_color( char *Str, short x1, short y );
   short    hot_key( short Taste );               // обработка быстрых клавиш
   e_WinMsg int_to_msg( short i );
   void   raden_y3( );
   short  raden_Y( long PY );                   // действия связанные со сменой текущей строки
   short  raden_X( short px );                  // действия связанные со сменой текущей колонки
   void   delet_str1( short px );               // удалить из текущей строки символ
   void   insert_str1( short px, short key );   // добавить в текущую строку символ
   void   prepare_text( void );                 // подготовка текста перед вызовом меню
   short  beg_str( void );                      // вычислить начало строки
   short  end_str( void );                      // вычислить конец строки
   short  char_exist( short px );               // есть ли в строке символы
   short  key_WK( void );                       // реакция на клавишу Enter
   void   key_Delet( void );                    // реакция на клавишу Delete
   void   delet_str( short Otm1,short Otm2,char f=0 );   // удалить текст в буфер
   void   insert_str( short px );               // вставть буфер в текущую строку
   void   insert_Buf( long PY );                // вставть буфер
   void   delet_Buf( long Pos, long Length );
   void   find_forward( void );
   void   find_backward( void );
   void   goto_str( void );                     // пойти в заданную строку
   void   str_shift( char R );                  // сдвиг несколких строчек
   char   find1( void );
   char   find( char Richt );
   void   replace( void );
   void   replace1( void );
   void   get_str( void );                      // синхронизировать Str с Текстом
   void   set_text( t_Text *Text );
   void   init( char *_Text );
   void   set_view( char f );                   // установить флаг "только чтение"
   void   set_keyword( t_KeyWord *KeyWord, long n_KeyWord );
   t_Text *text();
} ;

# endif
