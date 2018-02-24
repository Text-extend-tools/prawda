/****************************************************************************/
//              "��������"-"������" - open source ����������
//          ���������������� � ����������� � ��������� GNU v 2.0
//
//                         �������������� �������
//       �������� �.�.                                    �������
/****************************************************************************/
# ifndef SLOWO2_H
# define SLOWO2_H
# include <grammar.h>

// -------- ����� ��������� -------------
struct t_Shabl
{  char  **end ;  // ��������� �������
   long  *index ; // ������� ��������� � ���������� �������
   short n_end ;  // ����� ��� ���������
};

struct t_compWord
{   char *str ;     // "��������� ����� �����"
    short i_shabl ; // ������ �������
};

struct t_Slowo2
{
   char        f_compress ; // ����, ��� ������� ������
   t_Lang     *Lang ;   // ����, �������� ����������� �������
   t_Format   *Format ; // ������ �������

   long        n_Word ; // ����� ����
   short       n_Form ; // ����� ����
   char       *Mass ;   // ������, � ������� ����� �����
   long       *Word ;   // ������ �� ������ ���� [n_Word*n_Form]
   long       *reverce ;// ���������� ������ ����
// -------- ����� ������ -----------------
   t_compWord *Word1 ;   // ��������� ���� ����� [n_Word1]
   long       *sy_Word1 ;// ������ ������ ���� [n_Word]
   t_Shabl    *Shabl ;   // ������ �������� [n_Shabl]
   char       **EndSh ;  // ����� ������ Shabl->end 
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
���������� �����
1) ������� compress � ������� � �������
2) �������
   1) ������
   2) ����������
   3) ������� ������
   4) ����������� ������� �� ������� �����
*/

# endif
