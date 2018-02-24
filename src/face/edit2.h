/***************************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                Всяко-разные отображалки
//     Анисимов Д.В.                                         сегодня
/***************************************************************************/

# ifndef EDIT2_H
# define EDIT2_H

# include "core.h"
# include "window.h"

// ----- абстрактный класс списка ----------
class t_SelectWin : public t_Win
{

public:
   short Begin,Begin_X,PY,MaxY ;
   char  f_slave ;                // флаг подчиненного окна

	       t_SelectWin( void );
virtual	      ~t_SelectWin( void );
	void   restart( void );
	void   paint( void );
virtual void   begin( void );
virtual void   end( void );
virtual void   paint_begin( void );
virtual void   paint1( short i, char c );
virtual char   hot_key( short SS, short key1, short key2 );
virtual e_WinMsg loop( void );
};

// ----------- отображение первичного массива слов -------------
class t_Edit2 : public t_Win // public t_SelectWin
{
   short Begin,Begin_X,PY,MaxY ;
   short pos1 ;       // где начался буфер
   char  Reg ;        // буфер - небуфер
public:
	  t_Edit2( void );
   void   restart( void );
   void   paint( void );
   void   paint1( short i, char c );
   char   hot_key( short SS, short key1, short key2 );
e_WinMsg  loop( void );
};
// ---------- структура для визуализации дерева -------------
struct t_Vision
{  long  i_tree ;
   short level ;
   short unused ;
};
DECLARE_LIST_TYPE( t_Vision, t_VisionList );

// ----------- отображение структуры фразы ---------------------
class t_Edit3 : public t_SelectWin
{
   char       Halb ;
   char       Reg ;
   short      max_level ;
   t_Antwort *pAnt ;
   t_VisionList Vision1 ;   
public:
	  t_Edit3( char Halb );
   void   begin( void );
   void   end( void );
   void   paint_begin( void );
   void   make_vision( t_VisionList *Vision, t_TreeList *Tree, long i_tree );
   void   paint1( short i, char c );
   char   hot_key( short SS, short key1, short key2 );
};
// ----------- редактор частей речи -----------------------------
struct t_Tag
{  short  i_struct ;
   long   i_slowo, i_slowo1 ;
   t_Form Form ;
};

class t_EditPart : public t_SelectWin
{
   char  *Word ;   // переводимое слово
   t_Tag  Tag[100] ;// части речи, которыми оно может быть
   short  j_Tag ;
   char       present[100] ;
public:
	  t_EditPart( short i_word );
   void   paint1( short i, char c );
   char   hot_key( short SS, short key1, short key2 );
};
// ------------ редактор грамматических форм ---------------------
class t_EditForm : public t_SelectWin
{
   t_ParamList  *LParam ; // параметры языка
   t_Param1List *SParam ; // описание параметров структуры
   char         *value ;  // значения параметров структуры
public:
	  t_EditForm( short i_tree, char Halb );
   void   paint1( short i, char c );
   char   hot_key( short SS, short key1, short key2 );
};
// ----------- редактор грамматических форм (вспомогательный) ---
class t_EditForm1 : public t_SelectWin
{
   t_Param  *Param ; // параметр языка
   char     *Value ; // значение параметра (его надо задать)
public:
	  t_EditForm1( t_Param *Param, char *Value );
   void   paint1( short i, char c );
   char   hot_key( short SS, short key1, short key2 );
};

// ----------- отображение массива словарей ---------------------
class t_SlowoView : public t_SelectWin
{
   short  n_From ;
public:
	  t_SlowoView( void );
   void   paint_begin( void );
   void   paint1( short i, char c );
   char   hot_key( short SS, short key1, short key2 );
};
// ---- отображение грамматических форм, присутствующих в словаре ----
class t_SlowoView1 : public t_SelectWin
{
   t_Format    *Format ;
   t_Struct    *Part ;
   t_ParamList *Param ;

public:
	  t_SlowoView1( short i_Format );
   void   paint_begin( void );
   void   paint1( short i, char c );
};

# endif


