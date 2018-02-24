/***************************************************************************/
//              "��������"-"������" - open source ����������
//          ���������������� � ����������� � ��������� GNU v 2.0
//
//              �����, ����������� ��������� ���� �� �����
//       �������� �.�.                                    �������
/***************************************************************************/
# ifndef LEXER_H
# define LEXER_H

class t_Lexer
{
   char *Mass,*Mass1 ;
   char  Type[256] ;   // ��� �������� 0-������, 1-����������� 2,3-����� ������

public:
   char **Word ;    // ����� (������ ��������� �� Mass1)
   long  *Str ;     // ������ �����, ��������������� ������
   long   l_Mass,   // ������ ������� Mass
          n_Word ;  // ����� ����
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
