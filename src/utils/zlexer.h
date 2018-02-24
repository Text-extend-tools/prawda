# ifndef LEXER_H
# define LEXER_H

struct t_Token
{  long  begin,end,length ;
   char  type ;            // 0-������ 1-��������� 2-������
   char  type1 ;           // 0-�����  1-�������
   long  i_class ;        // ������ ������
   long  i_funk ;         // ������ �������
};

class t_Lexer
{
   char  Type[256] ;   // ��� �������� 0-������, 1-����������� 2,3-����� ������
public:
   char  *Mass,*Mass1 ;
   long   l_Mass,l_Mass1 ;
   t_Token *Token ;
   long     n_Token ;

         t_Lexer( void );
	~t_Lexer( void );
   void  init( char *Mass, long L );
   void  clr_breaker( void );
   void  set_breaker( unsigned char *Breaker );
   void  make_token( void );
   char *token( long i );
   long  find_paar( long i_Token, char *Open, char *Close );
   long  find_word( long Begin, char *Str );
   void  error( long i );
};

# endif
