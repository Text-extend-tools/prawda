/***************************************************************************/
//              "��������"-"������" - open source ����������
//          ���������������� � ����������� � ��������� GNU v 2.0
//
//                       ������� �� �����
//     �������� �.�.                                         �������
/***************************************************************************/

# ifndef DIR_H
# define DIR_H

/* ----------------------- ���, ��� ��������� � ������ �������� ������ --- */
struct t_File1
{  char name[32] ;    /* ��� ����� � ����������� */
   long size ;        // ������ �����
   long date ;
   long time ;
   char type ;        /* 0-���� 1-������� */
} ;

class t_Dir
{  t_File1 *File ;
   short    Begin,        // ������ ����, ������� �� ������
	    i_File,       // ������� ����
	    n_File ;      // ����� ������ � ��������
   char     shabl[32] ;   // ������ ������
   char     Name[32] ;
   char     Path[100] ;   // ������� �������
public:
   short  direktor( char *Antwort, char f=0 );
   void   set_schabl( char *str );
   void   get_schabl( char *str );
   short  in_file( void );           // ��������� ������ � ���� ������
   void   in_schabl( void );         // ��������� ������ � ���� �������
   short  in_name( void );           // ��������� ������ � ���� ������ �����
   void   paint_file( char f );      // ���������� ������ ������
   void   paint_schabl( void );      // ���������� ������
   char   read_file( char *Path );   // �������� ������ ������ � �������� ��������
   void   karkas( char f );            // ���������� ��� �������
   void   help( void );
   void   file_date( char *antwort, struct t_File1 &file );
   void   sort_file( void );
} ;

# endif
