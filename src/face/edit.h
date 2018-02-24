/***************************************************************************/
//              "��������"-"������" - open source ����������
//          ���������������� � ����������� � ��������� GNU v 2.0
//
//                  ����������� ����� ��������� ��������
//     �������� �.�.                                         �������
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

/* --------------------------------- ����� ������ � ������ --------------- */
class t_Text
{
public: // �� ������ ������ ��� ����������
   char     *Text ;         // ������� ������ ���������� � ������
   long      Length ;       // ������ ������, ����������� � ������
   long      i_str,i_pos ;  // ������� ������ � �� �������
   long      str_Txt ;      // ����� ����� � ������� Text
   char      fformat ;      // ������� "�����������"
public:
	  t_Text( void );
	 ~t_Text( void );
   void   init( char *Text );         // ������������ ����� �������
   void   clear( void );
   void   write_file( char *name, char f_cp1251=0 ); // �������� ����� � ����
   char   read_file(  char *name, char f_cp1251=0 ); // ��������� ���� �� �����
   char   operator[]( long i );
   long   length( );
   long   str_txt( );
   long   pos_str( long i_Str );              // ������ ������ i_Str
   char  *get_str( long i_Str );                 // �������� �� ������ ������ ����� i_Str
   char  *get_str( long i_Str, char *Buf );      // �������� ������ ����� i_Str � �����
   void   put_str_ipos( long i_pos, char *str ); // �������� � ����� ������ str
   void   put_str_istr( long i_str, char *str ); // �������� � ����� ������ str
   long   get_text1( long Str1, long Str2, char *List ); // �������� �� ������ �����
   long   get_text2( long Str1, long Str2, char *List ); // �������� �� ������ ����� ��� ��������
   void   put_text1( long Str1, char *List, long l_List );
   void   put_text1p( long Pos1, long Pos2, char *List ); // �������� ����� ������
   void   insert_str( long PY );      // �������� ������ ������
   void   delete_str( long PY );      // ������� ������ PY
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
   
   char   (*is_const)( char *Str ); // ������� ����������� "����� � ������"
   long   get_pos_xy( long py, long px );
   void   get_xy_pos( long pos1, long *_py, long *_px );
} ;

/* --------- ���������� �������� ---------------------------------------- */
class t_Edit : public t_Win
{
public :
   char    Bold ;         // ������� ��������� ����
protected:
   t_Text *Text ;
   long    Begin_Y ;       /* ������ ������, ������� � ���� */
   short   Begin_X ;       /* ������ �������, ������� � ���� */
   short   PX ;            /* ��������� ������� */
   long    PY ;            /* ����� ������������� ������ */
   long    old_PY ;
   short   otm1,otm2,      /* ������ ��������� �������� */
           Otm1,Otm2 ;     /* ������ ��������� �������� (�������������) */
   long    str1,str2,      /* ������ ��������� ����� */
           Str1,Str2 ;     /* ������ ��������� ����� (�������������) */
   char    Str[256],       /* ������� ������ */
           Buf[256] ;
   char    fview ;         // ���� fview==1 - �� ������ ��������������
   char    REG ;           /* �������-������ */
   char    Flag_b ;        /* ���� ������ ������ � ������� */   
   t_KeyWord *KeyWord ;     // �����, ������� ���������� ������ ������
   long      n_KeyWord ;
public :
   void     begin( void );
   e_WinMsg main( void );
   e_WinMsg loop( short ZZ, short key1, short key2 );
   void     paint( void );                         // ���������� �����
   void     paint1( long Otm1, long Otm2 );        // ���������� �����
   void     paint_current( short Otm1, short Otm2 );// ���������� ������
   void     set_string_color( char *Str, short x1, short y );
   short    hot_key( short Taste );               // ��������� ������� ������
   e_WinMsg int_to_msg( short i );
   void   raden_y3( );
   short  raden_Y( long PY );                   // �������� ��������� �� ������ ������� ������
   short  raden_X( short px );                  // �������� ��������� �� ������ ������� �������
   void   delet_str1( short px );               // ������� �� ������� ������ ������
   void   insert_str1( short px, short key );   // �������� � ������� ������ ������
   void   prepare_text( void );                 // ���������� ������ ����� ������� ����
   short  beg_str( void );                      // ��������� ������ ������
   short  end_str( void );                      // ��������� ����� ������
   short  char_exist( short px );               // ���� �� � ������ �������
   short  key_WK( void );                       // ������� �� ������� Enter
   void   key_Delet( void );                    // ������� �� ������� Delete
   void   delet_str( short Otm1,short Otm2,char f=0 );   // ������� ����� � �����
   void   insert_str( short px );               // ������� ����� � ������� ������
   void   insert_Buf( long PY );                // ������� �����
   void   delet_Buf( long Pos, long Length );
   void   find_forward( void );
   void   find_backward( void );
   void   goto_str( void );                     // ����� � �������� ������
   void   str_shift( char R );                  // ����� ��������� �������
   char   find1( void );
   char   find( char Richt );
   void   replace( void );
   void   replace1( void );
   void   get_str( void );                      // ���������������� Str � �������
   void   set_text( t_Text *Text );
   void   init( char *_Text );
   void   set_view( char f );                   // ���������� ���� "������ ������"
   void   set_keyword( t_KeyWord *KeyWord, long n_KeyWord );
   t_Text *text();
} ;

# endif
