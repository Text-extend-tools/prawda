# ifndef WORD1_H
# define WORD1_H

struct t_fWord
{  char *str ;         // ������ (���������� �����)
   long  index ;       // ������ ����� � ������ (����� ��� ����������� ������� ���������)
   long  i_s_word ;    // ������ �� sortWord
   long  i_text ;      // �� ������ ����� ������ ����� ��� �����
} ;
DECLARE_LIST_TYPE( t_fWord,t_fWordList );

struct t_sortWord
{  char *str ;
   long  index ;       // ����� ����
   long  sy_word ;     // ������ � sy_word
} ;

struct t_WordTree
{  long up,down,n_down ;
   long i_s_word ;      // ������ � sortWord
   long counter,        // �������
        counter1 ;      // 
};
DECLARE_LIST_TYPE( t_WordTree,t_WordTreeList );

struct t_sortStruct
{  short  counter,
          counter1 ;    // 
   short  i_tree ;
   char  *str ;
};

class t_NewWord1 : public t_Win
{
   t_Text         *pText ;    // �� ������ ������ ��� �������
   t_fWordList     Word ;     // ������ �������������� ����
   t_sortWord     *sortWord ; // ������ ����, ��������������� �� ��������
   long            n_sortWord ;
   long           *sy_word ;  // ������ �� sortWord->Word (����������� Word.j)
   t_WordTreeList  WordTree ; // ������ (���) ���������
   t_sortStruct   *sortStruct ;// ������������� ������ �������� (����������� WordTree.j)
   char           *Mass ;
   char           *Mass1 ;
// ------------------------------
   t_Rect          wFrasa,    // ���� �����
                   wWordList, // ���� ������ ����
		   wNewWord ; // ���� ����� ����� ���������

   long            i_word,    // ������ �������� �����
                   i_begin,   // ������ �����, ������� � ������ ����
		   i_frasa ;  // ������ ������� �����
public:
          t_NewWord1();
	  ~t_NewWord1();
   void   make_list_words( t_Text &Text );
   long   find_down( long z_tree, long i_s_word );
   void   add_down( long z_tree, long i_s_word );
   void   make_struct( long z_tree, char *Str );

   void   print_Word( char *File );
   void   print_sortWord( char *File );
   void   print_Tree( char *File );

   void     set_pos( short x1, short y1, short x2, short y2 );
   void     paint( );
   void     paint_frasa( char *Str );
   void     paint_wordlist(  );
   e_WinMsg main_loop( t_Text &Text );
   long     sort_to_word( long i );
};

# endif
