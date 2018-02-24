/***************************************************************************/
//              "��������"-"������" - open source ����������
//          ���������������� � ����������� � ��������� GNU v 2.0
//
//                     "������������" �������
//     �������� �.�.                                         �������
/***************************************************************************/
# ifndef SLOWO_H
# define SLOWO_H

struct t_Paragraf
{  char word[8] ;           // ������ ����� ����� ���������
} ;

struct t_Slowo
{
   long       *sy_Word  ;   // ������ �� ������ ����
   char        f_disk ;     // ������� ����, ��� sy_Word ����� �� �����
   t_Paragraf *Paragraf ;   //
   long        n_sy_Word,   // ����� ����
	       n_Paragraf ; // ����������
   char        NameFile[200] ; // ��� ����� � ������� ����� �������
   long        i_Paragraf ; // ����� ������������ ���������
   char       *Buf ;        // ������ � ������� ����� ����������� �����
   short       l_Buf ;      // ������ ������� Buf

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





