/***************************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//              Класс, разбивающий текстовый файл на слова
//       Анисимов Д.В.                                    сегодня
/***************************************************************************/
# ifndef LEXER_H
# define LEXER_H

class t_Lexer
{
   char *Mass,*Mass1 ;
   char  Type[256] ;   // тип символов 0-просто, 1-разделитель 2,3-новая строка

public:
   char **Word ;    // слова (ссылки указывают на Mass1)
   long  *Str ;     // номера строк, соответствующие словам
   long   l_Mass,   // длинна массива Mass
          n_Word ;  // число слов
public:
         t_Lexer( void );
	~t_Lexer( void );
   void  init ( char *Mass, long L );
   void  init1( char *Mass, long L );
   void  remark ( void );
   void  remark1( void );
   void  clr_breaker( void );
   void  set_breaker( unsigned char *Breaker );
   void  make_words( void );
   long  str( long i );
   char *word( long i );
   long  n_word( void );
   long  find_paar( long Begin, char *Open, char *Close );
   long  find_word( long Begin, char *Str );
   void  error( long i );
};

# endif
