/*******************************************************************/
//              "��������"-"������" - open source ����������
//          ���������������� � ����������� � ��������� GNU v 2.0
//
//                      ����������� ����� ����
//     �������� �.�.                               �������
/*******************************************************************/

# ifndef WINDOW_H
# define WINDOW_H

enum e_WinMsg
{  WM_NULL1,
   // -------- ��������� ������ ���� ------
   WM_NEXT,
   WM_PREV,
   WM_ESC,
   WM_OK,
   WM_HELP,
   // -------- ������� � ���������� ���� --
   WM_EDIT1,
   WM_EDIT2,
   WM_FIRST,
   WM_SRC,
   WM_DST,
   WM_DEBUG,
   WM_PATH,
   WM_PATH2,
   WM_GRAMMAR,
   WM_SLOWO,
   WM_BASE,
   WM_ADD_WORD,
   WM_ADD_WORD1,
   WM_SLOWO1,
   WM_ERROR,
   // -------- ���������� �������� --------
   WM_RESIZE,
   WM_N_FRASA,
   WM_P_FRASA,
   WM_N_WORD,
   WM_P_WORD,
   WM_SELECT2
};
// ------- ������������� (��� ��������� ����) ---------
struct t_Rect
{
   short x1,y1,x2,y2 ;
};

class t_Win  // ----- ����������� ����� ���� ----------
{
protected:
   short x1,y1,x2,y2 ;
   short size_x,size_y ;
public:
   char  Name[100] ;
	  t_Win( void );
virtual	 ~t_Win( void );
virtual   void   set_pos( short x1, short y1, short x2, short y2 );
virtual   void   set_name( char *Name );
virtual   void   paint( char Select=0 );
};

# endif
