/***************************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                     "Человеческий" словарь
//     Анисимов Д.В.                                         сегодня
/***************************************************************************/
# ifndef SLOWO_H
# define SLOWO_H

struct t_Paragraf
{  char word[8] ;           // первое слово этого параграфа
} ;

struct t_Slowo
{
   long       *sy_Word  ;   // ссылки на начала слов
   char        f_disk ;     // признак того, что sy_Word лежит на диске
   t_Paragraf *Paragraf ;   //
   long        n_sy_Word,   // число слов
	       n_Paragraf ; // параграфов
   char        NameFile[200] ; // имя файла в котором лежит словарь
   long        i_Paragraf ; // номер прочитанного параграфа
   char       *Buf ;        // массов в котором лежат прочитанные слова
   short       l_Buf ;      // длинна массива Buf

   char        Divider[10] ;
   short       l_Divider ;

   t_Slowo( void );
   char  make( char *File );
   void  write( char *File );
   long  find( char *Str );
   void  read_paragraf( long i_wort );
   char *wort( long i );
   char *trans( long i_wort );
   char *answer( long i_wort );
   void  set_divider( char * _Divider );
   void  set_sort( char f );
};

# endif





