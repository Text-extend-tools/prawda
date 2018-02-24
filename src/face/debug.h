/***************************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                         главный отладчик
//     Анисимов Д.В.                                         сегодня
/***************************************************************************/
# ifndef  DEBUG_H
# define  DEBUG_H

# include <core.h>
# include <window.h>
# include <edit2.h>

enum e_DebugRegim
{
   DSTRUCT,
   DVARIANT,
   DREAL
};
enum e_DebugRegim1
{  D1STRUCT,
   D1SELECT
};

/********************************************************************/
//    смотрелка для t_Core->Variants (основной таблицы разбора)     //
/********************************************************************/
class t_Debug : public t_Win
{
   short   i_start_struct ;
   long    i_Select ;
   short   Begin_S,Begin_V,Begin_R ;
   short   n_struct ;
   t_shortList SelectVision ;
   short   z_Vision ;

public:
   t_rWord Current ;
   t_rWord stack[30] ;
   short   j_stack ;
   
   e_DebugRegim  Reg ;
   e_DebugRegim1 Reg1 ;

       t_Debug( void );
void   init( void );
void   restart( void );
void   paint( void );
void   paint_frasa( void );
void   paint_stack( short xx );
void   paint_struct( short xx );
void   paint_variant( short xx );
void   sprint_param( char *Str, t_Word *W );

char   hot_key( short SS, short key1, short key2 );
e_WinMsg loop( void );
};

/********************************************************************/
//       смотрелка для t_Grammar (таблицы правил перевода)          //
/********************************************************************/
class t_GrammarView : public t_Win
{
   short  n_stack,i_Select,Begin_Y ;
   short  stack[40] ;
   e_Half Half ;

public:
       t_GrammarView( void );
void   set_pos( short x1, short y1, short x2, short y2 );
void   restart( void );
void   paint( void );
e_WinMsg loop( void );
};

class t_ViewTransParam : public t_SelectWin
{
   short  i_struct ;
   e_Half Half ;
public:
	  t_ViewTransParam( short _i_struct, e_Half _Half );
   void   paint1( short i, char c );
   char   hot_key( short SS, short key1, short key2 );
};

/********************************************************************/
//       смотрелка для t_Slowo3 словарь структур перевода           //
/********************************************************************/
class t_Slowo3View : public t_Win
{
   long   n_stack,i_Select,Begin_Y ;
   long   stack[40] ;

public:
       t_Slowo3View( void );
void   set_pos( short x1, short y1, short x2, short y2 );
void   restart( void );
void   paint( void );
e_WinMsg loop( void );
};

# endif
