/***************************************************************************/
//              "��������"-"������" - open source ����������
//          ���������������� � ����������� � ��������� GNU v 2.0
//
//                  ���������, ����������� ����������
//       �������� �.�.                                    �������
/***************************************************************************/
# ifndef GRAMMAR_H
# define GRAMMAR_H

# include <stdio.h>
# include <list.h>
# include <lexer.h>

const int LNAME =  40 ; // ������ ����� ����� lang.txt

DECLARE_LIST_TYPE( short,t_shortList );
DECLARE_LIST_TYPE( long ,t_longList );

struct t_Grammar ;
struct t_Lang ;

// -------------- ��������� �������� ���������� ------------------
struct t_Value
{  char *Name ;      // �������� ��������� �������� "������������"
} ;
DECLARE_LIST_TYPE( t_Value,t_ValueList );

// -------------- �������� ����� - ����� � �� --------------------
struct t_Param
{
   char     *Name ;  // �������� ��������� �������� "�����"
   t_ValueList Value ;  // ��������� �������� ����������

    t_Param( void );
   ~t_Param( void );
   void  init( void );
   void  del( void );
} ;
DECLARE_LIST_TYPE( t_Param,t_ParamList );

// -------------- �������� ����� - ����� � �� --------------------
struct t_Param1
{  short param ;    // ��� ��������� � ������������ � Lang->Param
   char  *Name ; // �������� ��������� �������� "�����"
   char  Dir ;      // 1-�������� 0-��������
   char  value ;    // �������� ���������, ���� -1 ������ �� �����
};
DECLARE_LIST_TYPE( t_Param1,t_Param1List );

// ------------- ����� ����� -------------------------------------
struct t_Form
{
   char value[10];  // ������� ���������� - � ������������ � Part->Param
   void init( );
   t_Form();
};

// =========== ���, ��� ��������� � ��������������� ������� ======

DECLARE_LIST_TYPE( t_Form,t_FormList );

// ------------ �������������� �������������� ���������� ---------
struct t_Rename
{  char *Reduce ;  // ����������� ����������� ��������� (�������� "f")
   char *Full ;    // ������ ��� �������� ��������� (�������� "�������")
};
DECLARE_LIST_TYPE( t_Rename,t_RenameList );

// ------------ ������, ����������� �������������� ������� -------
struct t_Format
{
   char     *Name ;     // ��� �������
   short     i_part ;   // ������ ����� ���� � ������������ �  
                        // t_Lang->Part[this->i_part]
   t_FormList Record ;  // ������ ��������� ���� �����

    t_Format( void );
   ~t_Format( void );
   void  init( void );
   void  del( void );
};
DECLARE_LIST_TYPE( t_Format,t_FormatList );

DECLARE_LIST_TYPE( char *,t_strList );
// ----------- �����, � ������� ����� �������������� ������� ---
struct t_File
{
   short  i_part,       // ����� ���� � ������������ � t_Lang->Part[this->i_part]
	  i_format ;    // ������ �������
   t_strList  FileName ;// ����� ������, � ������� ����� ��� ����� ����

    t_File( void );
   ~t_File( void );
   void  init( void );
   void  del( void );
};
DECLARE_LIST_TYPE( t_File,t_FileList );

// =========== ���, ��� ��������� � ������� ��������� ==========
// -------------- �������� ����� ���� � ������� ��������� ------
struct t_PartDsk
{  char  *Tag ;        // ��� ����� ���� � ������� ���������
   char  *Name ;       // ��� ����� ����
   short  i_part ;     // ����� ����� ����
};

DECLARE_LIST_TYPE( t_PartDsk, t_PartDskList )

// -------------- ������, ����������� ������� ��������� -------
struct t_Format1
{  char *Name ;             // ��� �������
   t_PartDskList SouPart,   // ������ �������������� ��������
                 DstPart ;
   t_RenameList  Rename ;   // ������ �������������� ����������

   void   init( void );
   void   del( void );
   void   make_part( t_Grammar *Grammar );
   short  find_struct( char Dst, char *Str );
   char  *get_tag( char Dst, short i_struct );
   char  *get_name( char Dst, short i_struct );
};
DECLARE_LIST_TYPE( t_Format1, t_Format1List )

// =========== ���, ��� ��������� � ��������� =================
// ----------- ��� ��������� ----------------------------------
enum e_Type
{  TNULL,     // ��� � ���� ����� ��������� - ������ ������
   TCONST,    // ������������ �������
   TCONST1,   // ������������ ������� �� �������
   TWORD,     // �����
   TWORD0,    // ������ ����� (@00)
   TSTRUCT,   // ���������
   TSTRUCT1,  // ������������� ��������� �� �������
   TSTRUCT2,  // ������������ ��������� �� �������
   TSELECT,   // �����
   TSELECT1,  // �������������� �����
   TSELECT2,  // "�������������" �����
   TENUM,     // "���������������" �����
   TUNORDER,  // ���������, ����� ������� ����� ������ �������
   TUNORDER1,
   TUNORDER2
} ;
// ----------- ������� ���� ��������� --------------------------
# define IF_WORD( T )    ((T)==TCONST   || (T)==TCONST1   || (T)==TWORD)
# define IF_STRUCT( T )  ((T)==TSTRUCT  || (T)==TSTRUCT1  || (T)==TSTRUCT2)
# define IF_SELECT( T )  ((T)==TSELECT  || (T)==TSELECT1  || (T)==TSELECT2)
# define IF_UNORDER( T ) ((T)==TUNORDER || (T)==TUNORDER1 || (T)==TUNORDER2)
# define IF_CONSTR( T )  ((T)==TSTRUCT  || (T)==TSTRUCT1  || (T)==TSTRUCT2\
		      || (T)==TSELECT || (T)==TSELECT1 || (T)==TSELECT2\
		      || (T)==TUNORDER || (T)==TUNORDER1 || (T)==TUNORDER2)

// -------------- ��������� �������� ---------------------------
enum e_Half
{  FROM,
   TO
} ;

// ---------- ������� ��������� ������� ���������� ���������� -----
struct t_2Index
{  short i1,       // ������ ����� 
	 i2,       // ������ ��������� � ���������
	 i_param ; // ������ ��������� � �����
   char *Name ;
};
DECLARE_LIST_TYPE( t_2Index, t_2IndexList );

// ---------- ����������� �������� ���������� ������ ��������� -----
struct t_Relation
{  char s1,p1 ; // ������ ��������� � ��������� ���������
   char s2,p2 ; // ������ ��������� � ��������� ���������
};
DECLARE_LIST_TYPE( t_Relation, t_RelationList );

// ---------- ������ ������ ���������� - ������� �� t_RelationList --
struct t_RelationList1
{  
   t_Relation *list ;
   long        j ;
   void  operator = ( t_RelationList &a ){ list=a.list ; j=a.j ;  }
};

// ------------------------ �������� ����� -------------------------
struct t_Word
{  e_Type type ;       // �����,��������� ��� �����
   char  *Name ;   // ��� ���������
   char  *literal ;// ��������� ���������
   t_Param1List Param; // ��������� ���������
   short  order ;      // ������ ��������������
   short  i_struct ;   // ������ ��������� � ������� t_All_Data->Trans
   char   use ;
   char   meaning_use ; // ������� ����, ��� �������� ���������� ������

	  t_Word( void );
   void   init( void );
   void   del( void );
   void   operator = ( t_Word &W );
};
DECLARE_LIST_TYPE( t_Word,t_WordList );

// ------------------- �������� �������������� ------------------
struct t_Struct
{  e_Type         type ;     // �����,��������� ��� �����
   char          *Name ;     // ��� ���������
   long           i_str ;    // ������, � ������� ���������� �� ��������
   t_Param1List   Param ;    // ��������� ���������
   t_WordList     Word ;     // ������������
   t_RelationList Relation ; // ������������ ����������
			     // ���� ��������� s1 - ������ �����
			     // ���� �����     s1 - ������ ��������

   t_Struct( void );
   void operator = ( t_Struct &S );
   void init( void );
   void del( void );
   long read( long Begin, t_Lang *Lang );
   long trans_word1( long Begin );
   long trans_word ( long Begin );
   long trans_param( long Begin, t_Lang *Lang );
   long trans_order( long Begin, t_Word *Word );
   void trans_relation( void );
   void tst_relation( void );
   t_2Index  find_param2( char *Str );
};
DECLARE_LIST_TYPE( t_Struct,t_StructList );

enum e_Table
{   T_SOU, T_TRANS, T_DST
};

// --------------------- ������� ���������� ���������� ----------
struct t_Table
{  e_Table      type ;   // ����� ���������, ����� ���������, ��� ��������
   short        Size ;   // ������ ������ �������
   t_2IndexList In,Out ; // ������� � �������� ���������
   t_shortList  Value ;  // �������� ����������

   void   init( void );
   void   del( void );
   void   operator = ( t_Table &T );
};
DECLARE_LIST_TYPE( t_Table, t_TableList );

// =========== ���������� � ��������������� ��������� ===========
// ----------- �������������� ���� ------------------------------
struct t_Trans
{  t_Struct    From,To ;      // �������� ��������� � ��������������� �����
   t_shortList Relation1,     // ������������ ������ From->To
	       Relation2 ;    // ������������ ������ To->From
   t_strList   Param1,Param2 ;// ����� ���������� ��� � ������ ���������
   t_TableList Table ;        // ������� ���������� ����������

	 t_Trans( void );
   void  operator = ( t_Trans &SS );
   void  init( void );
   void  del( void );
   void  make_order( void );
   void  make_index_table( void );
};
DECLARE_LIST_TYPE( t_Trans, t_TransList );

// -------------- �������� ����� (��������� ��� ���������) --------
struct t_Lang     
{
   char          Name[LNAME] ;
   t_ParamList   Param ;  // ��������� 
   t_StructList  Part ;   // ����� ����
   t_FormatList  Format ; // ������� �������������� ��������
   t_FileList    File ;   // ����� �������������� ��������
   t_Lexer      *Lexer ;
   char          To ;     // ������� ����, ��� ��� ����-��������

   void del( void );
   long read( t_Lexer *Lexer, long B );
   void read_format( long Begin, t_Format *Format );
};

// ------------- ������ ������� ��� �������� ----------------------
struct t_Format2
{  char *Name ;        // ��� �������
   char  Divider[10] ; // ����������� ����� � ��������
   char  fsort ;       // ��� ����������� �����

   void   init( void );
   void   del( void );
};
DECLARE_LIST_TYPE( t_Format2, t_Format2List );

// ---------------------- ��� ���������, ����������� ���� ---------
class t_Grammar
{
   t_Lang         From ,To ; // ���� ��������� � ��������
   t_Format1List  Format1 ;  // ������� ���������
   t_Format2List  Format2 ;  // ������� �������� ��� ��������
   t_FileList     File1 ;    // ����� ������ ���������
   t_FileList     FilePre ;  // ����� ������ ��������������
   t_FileList     FilePost ; // ����� ������ ���������������
public:
   t_FileList     File2 ;    // ����� ������ �������� ��� ��������
private:
   t_TransList    Trans ;    // ������ ���� �������������� ��� 
   short          i_Number ; // ����� ����, ������� ������������
   short          i_All ;    // ������� ����������� "�����������" ��������
   short          i_Surmise ;// ����������� "�������"
   short          i_Irregular ; // ����������� "����������"
   short          i_Unknown ; // "@��������_�����_����"
public:
   t_Lexer        Lexer ;    // ������, ������� �������� ��� ����� � �.�. 

public:
   char           make_all( char *File );
   short          find_struct( e_Half Half, char *Str );
   t_Trans&       operator []( long i );
   short          n_trans( );
   short          main_struct( );
   short          i_number( );
   short          i_all( );
   short          i_surmise( );
   short          i_irregular( );
   short          i_unknown( );
   t_Lang&        from();
   t_Lang&        to();
   t_Format1List& format1();
   t_FileList&    file1();
   t_FileList&    file_pre();
   t_FileList&    file_post();
private:   
   void    copy_words( void );
   void    read( char *File );
   long    read_slowo1( long Begin );
   long    read_slowo2( long Begin );
   long    read_format1( long Begin, t_Format1 *Format );
   long    read_format2( long Begin, t_Format2 *Format );
   long    read_trans( long Begin );
   long    read_trans1( long Begin );
   long    read_table( long Begin, t_Trans *TT );
   void    make_index_struct( void );
   void    make_index_param0( long i_trans );
   void    make_index_param( e_Half Half, long i_trans, long i_word );
   void    make_index( void );
   void    index_word( t_Word *W, e_Half Half );
   short   find_format1( char *Word );
   short   find_format2( char *Word );
   void    del( void );
};

// -------- ������ � ��������� ������������ �������� -----------
struct t_DirTrans
{  char *Dir,*Name ;
};

extern t_Grammar   Grammar ;
extern t_DirTrans *DirTrans ;
extern t_DirTrans *z_DirTrans ;
extern FILE       *File_Error ;
extern char        Read_Error ;
extern FILE *File_Error ;
extern char  Read_Error ;

short  find_param( t_ParamList  *List, char *Str );
short  find_format(t_FormatList *List, char *Str );
short  find_value( t_ValueList  *List, char *Str );
t_DirTrans *current_dir_trans( void );

# define i_main_struct Grammar.main_struct( )

# endif
