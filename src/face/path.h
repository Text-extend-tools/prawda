/*******************************************************************/
//              "��������"-"������" - open source ����������
//          ���������������� � ����������� � ��������� GNU v 2.0
//
//             ���������������� ����� ��������� ��������
//     �������� �.�.                               �������
/*******************************************************************/

# ifndef PATH_H
# define PATH_H

# include <core.h>
# include <window.h>
# include <edit2.h>

struct t_Meaning
{
   char *Meaning ;      // ������ t_aWord->meaning
   long  j_Meaning,
         l_Meaning ;

   t_Meaning();
  ~t_Meaning();
   long  set( char *str );
   char *get( long z );
};

struct t_pVariant
{   short  index ;        // ������������� ������
    short  i_down ;       // �� ������ ������� ��������
    t_aWordList aWord ;

    long word_to_aword( long i_word );
};

struct t_Path ;

// --------- ������� ������ ������ ------------------------
struct t_pTree
{  long   up,down,n_down ;
   long   first,last ;   // ������ ������ � ��������� (������������) Record, 
                         // �������� � ��� ���������
   short  i_struct ;     // (���� �� �����������!!!)
   short  i_aword ;      // ����� aWord � ������� ���������
   short  i_last_aword ;
   short  i_slowo1 ;
   short  n_slowo ;
   char   ende ;
   char   fselect2 ;
   t_Path *Down ;       // ������ (��� ����������)
} ;

struct t_Path : public t_Win
{
   t_Path     *Up ;            // ������ �� ����������� ������
   t_pVariant *Variant ;       // ������ ��������� ��������
   long        n_Variant ; 
   t_pTree    *Tree ;          // ������ ��������� ��������
   long        n_Tree ;   
   long        z_Tree,i_down ; // ������� ������� � "������ �������"
   t_aWord    *aWord ;
   t_Meaning   Meaning ;        // ������ t_aWord->meaning

   char        StrUp  [LFRASA] ;
   char        Antwort[LFRASA] ;

public:
         t_Path( void );
	 t_Path( t_Path *Up, long z_Tree );
	 ~t_Path();
   void  del();
   void  make_tree( void );
   long  n_variant_calc( t_Path *Up, long z_Tree );
   void  make_variant1( void );
   void  make_variant2( t_Path *Up, long z_Tree );
   void  sort_variant();
   void  paint( );
   void  print1( char *Str1, long i_Node );
   void  print2( char *Str1, long i_Node, long i_down, long i_slowo1 );
   long  print3( char *Str1, long i_Node, long i_down, long i_slowo1 );
   char *print_serv( t_aWord *W, short i_slowo1 );
   e_WinMsg universe(  );
   e_WinMsg loop( void );
   void  split_down( long z_Tree, long i_down, long *i1, long *i2 );
   void  split_final( long z_Tree, long *i1, long *i2 );
   long  merge_down( long z_Tree, long i_down );
   void  print_variant( char *File );
   void  print_tree( char *Name );
   void  print_all(  );
   long  sou_length1( );
   long  sou_length( long i_word );
};

# endif
