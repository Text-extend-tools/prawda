/*******************************************************************/
//              "��������"-"������" - open source ����������
//          ���������������� � ����������� � ��������� GNU v 2.0
//
//             ���������, ����������� ���� ��������
//     �������� �.�.                               �������
/*******************************************************************/

# ifndef CORE_H
# define CORE_H

# include <grammar.h>
# include <slowo3.h>

const int LWORD =  50 ; // ������ ������ �����
const int LFRASA=1000 ; // ������ ������ �����

struct t_Core ;

// ---------------- ���� ����� ������ ��������������� ������ -----------
enum e_TagType
{   P_FRASA,
    P_VERBATIM,
    P_STRUCT,
    P_CONST,
    P_TAG_LEFT,
    P_TAG_RIGHT,
    P_VERB
};
// ---------------- ��� ���������� ������� ����� -----------------------
enum e_Result
{  E_NULL,   // ��������� �� ���������� ������
   E_NO,     // �� ����� �������������
   E_OK,     // ����������
   E_FIG,    // �������� ����������
   E_FIGNA,  // �� ����������
   E_ERROR   // ���������� ������ ���������
};
// ------------------ ������������� ��� ����� ��������� ��� ��������� ---
struct t_Item
{  char     *Str1 ;  // ������ �� �� ����� �����, ��� ���������� ��� �����
   char     *Str  ;  // �������� ��� �����
   char      exist ; // ���������� �� ��� ����� � �������
   e_TagType type ;
   short     i_struct,i_word ;
   short     z_str ; // ��� ��� ����� ���������� � �������� ������
};
DECLARE_LIST_TYPE( t_Item,t_ItemList );

// ------------------ ���� ������ ������� ����� � -----------------------
// ------------------ ���� ������ ��������� �������� --------------------
struct t_Tree
{  e_Type type ;        // ��� ����
   long   up ;          // ������ �����
   long   down,n_down ; // ������ ����, � ����� ����� ������
                        // ���� down=-1 �� ��� ����� ������ �������
   short  i_word,       // � ������ ����� ����������
	  i_last_word,  // � ������ ����� ���������� ���������
	  i_struct ;    // ��� �� ���������
   long   i_variant ;   // ����� �������� ���������
   long   link ;        // ������ ��������� �� �������� ��� ��������
   t_Form Form ;        // ��������� ���� ��������� � ������������ � Struct->Param
   short  select ;      // ��������� ������� � TSELECT
   short  index ;       // ����� ����� � ��������� t_Struct->Word
   long   i_slowo ;     // ������ � ������� ���������
   short  i_slowo1 ;    // ������ ��������
   short  i_var_irreg ; // ������ �������� �������� ����� (new)
   short  n_slowo  ;    // ����� ��������� ��������
   short  left_tag,     // ������ ������ � ������� ����,
          right_tag ;   // ������������� � ����� ����
   char   Str [LWORD] ; // �������� � ������ �����
   char   Str1[LWORD] ; // �������� � ��������� �����

	 t_Tree( void );
	~t_Tree( void );
   void  init( void );
   void  del( void );
   void  set( char _type, long up, short iw, short i_struct );
} ;
DECLARE_LIST_TYPE( t_Tree, t_TreeList );

// ----------- ������� ���������� ����� ----------------------
struct t_rWord  
{
   e_Type  type ;
   short   i_word,      // ����� �����, � �������� ����������
	   i_last_word, // ����� �����, � �������� ���������� ���������
	   i_struct;    // ����� ��������� � t_Grammar->Trans[]
   long	   i_variant;   // ����� �������� �������� 
   short   index ;      // ������ � ������� t_Struct->Word[]
   long    i_slowo ;    // ������ � ������� ���������
   short   i_slowo1 ;   // ������ ��������
   short   n_slowo ;    // ����� ��������� �������� (� �������)

   t_rWord( );
};
DECLARE_LIST_TYPE( t_rWord,t_rWordList );

// ----------- ������� ���������� ��������� ------------------
struct t_rStruct
{
   e_Type  type ;
   short   i_word,       // � ������ ����� ����������
	   i_last_word,  // � ������ ����� ���������� ���������
	   i_struct ;    // ����� ��������� � t_Grammar->Trans[]
   long    r_word ;      // ������ �� t_Core->rWord
   short   nr_word ;     // ����� r_word (new)
   long    i_slowo ;     // ������ � ������� ���������
   short   i_slowo1 ;    // ������ ��������
   short   i_var_irreg ; // ������ �������� �������� ����� (new)
   t_Form  Form ;
   long    i_meaning ;   // ����� ���� ����������� �� ����� ���������
			 // (��������� �� t_Core->Meaning)
   t_rStruct( );
};
DECLARE_LIST_TYPE( t_rStruct,t_rStructList );

// ---------- ��� ��������� �������� ���������� ��������� ----
struct t_Variants
{  char  absend ;          // ������� ����, ��� ��� ����������� � 
                           // ���� ������� ������ �� �����
   short i_struct,i_word ;
   t_rStructList Variant ; // ���� �������� �������
};

// ---------- ����� ������������ ����� (t_Antwort) ------------
struct t_aWord
{  short  i_word,       // ����� ����� � �������� �����
          i_last_word ; // ����� ���������� ����� � �������� �����
   short  i_struct ;
   long   i_slowo ;     // ����� t_Record � ������� ������� ��������
   short  n_slowo ;     // ����� ��������� ��������
   long   meaning ;
   t_Form Form ;
   char   Str[LWORD] ;
};
DECLARE_LIST_TYPE( t_aWord, t_aWordList );

// ----------- ��������� ������� ����� ------------------------
class t_Antwort
{
   char         Str[LFRASA] ;  // ���������� �����
   t_TreeList   Tree1, Tree2 ; // ��������� �����-��������� � ����� ���������
   t_aWordList  aWord ;        // �����-��������
public:
                t_Antwort( );
   void         del( void );
   void         make( short i_word, short i_struct, long i_variant, t_Form Form );
   void         make_sou( short i_word, short i_struct, long i_Variant );
   char        *translate_simple( short Begin );
   char        *translate_simple1( short Begin );
   char        *str( void );
   t_aWordList *aword( void );
   t_TreeList  &tree1( void );
   t_TreeList  &tree2( void );
private:
   void  set_tree( t_Tree *T, t_rStruct  *pS, long i );
   void  make_dst( void );
   void  make_dst1( long up, long i2, t_Trans *Tr );
   void  make_dst2( long up, long i2, t_sStruct *S1, t_sStruct *S2 );
   void  make_dst3( long up, long i2 );
   void  make_to( long i_tree );
   void  param_from( void );
   void  param_transmit( void );
   void  param_transmit1( t_Table *Table, long i_Tree1, long i_Tree2 );
   void  param_to( void );
   void  make_vision( char Halb, long i_tree );
   void  insert_tag();
   void  print_antwort( long i_tree, char fword );
   void  add_word( t_Tree &T, char fword  );
   void  post_proc( void );
};

// ---------- ������� ����� �������� --------------------------
class t_Core
{
   short        n_struct,n_word ;
   t_ItemList   From,Tag ;     // ��������� From
   t_Variants  *Variants ;     // [n_struct*n_word] ��� �������������� 
                               //  �������� �������
   t_rWordList  rWord ;        // ��, �� ��� ��������� t_Variant->variant
   char        *Meaning ;      // ������ t_Variant->meaning
   long         j_Meaning,
                l_Meaning ;
   t_Antwort    Antwort;       // �������� ������������ �����
   char         f_Full ;       // �������, ��� ���������� ������ ��������
   char         Sign ;         // ���� � ����� ����������� '.' ��� '?'
   e_Result     Result ;       // ��� ����������
   char        *Sou ;          // �������� �����
   char         Sou0[LFRASA] ; // �������� ����� �������� �� �����
   char         Sou1[LFRASA] ; // �������� ����� �������� "�� ���� �� ����"
   long         j_VariantMem ; // ������� Variants[i].Variant (��� ����� ������� ������)
public:
   char         f_Figna ;      // �������� �� "#�����"
public:
   t_Core();
  ~t_Core();
   void        init( short n_struct, short n_word );
   void        clr( );
   void        translate( char *Sou, char *Dst );
   void        make_node( t_TreeList &Node, short i_word, long i_struct );
   char        make_from1( char *Sou );  // ������� public ��� t_NewWord1
   char        make_from2( void );
   t_Variants *variants( short i_word, long i_struct );
   t_rStruct  *variant(  short i_word, long i_struct, long i_variant );
   t_rStruct  *variant( t_rWord R );
   t_ItemList &from( void );
   t_ItemList &tag( void );
   t_rWord    &rword( long i );
   t_Antwort  &antwort( );   
   long        n_antwort( );
   long        n_reg_antwort( );
   char        f_full();
   char        sign( void );
   long        struct_of_word( short i_word );
   short       fn_word();
   e_Result    result();
   char       *sou();
   char       *get_meaning( t_rStruct *V );

private:
   void        universe( short i_word, long i_struct );
   void        next_chance( char *Dst );
   void        set_meaning( t_rStruct *V, char *Str );
   void        no_brakets( char *Str, char *Str1 );
   void        add_from( short i_word, long i_struct, t_Form Form,
                         long i_slowo, short i1_slowo );
   char        atom( short i_word, t_Word &Word );
   void        real_param_up( t_rStruct *V );
   char        Compare_Param( t_rStruct *Variant, t_Word *Word );
   char        select2_exist( short i_word, short i_struct, short i_last_word );
   void        mem_size_test( );
   void        double_variants( char *Name );

friend class t_Slowo3 ;
};

extern t_Core        Core ;
extern t_Slowo3      Perevod ;

char read_grammar  ( char *Dir, t_DirTrans *DT );
char reread_grammar( char *Dir, t_DirTrans *DT );
void set_formats( void );
char *filename( char *Path, char *File );
# endif
