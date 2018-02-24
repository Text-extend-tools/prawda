/***************************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                      Редактор, заточенный под перевод 
//     Анисимов Д.В.                                         сегодня
/***************************************************************************/

# ifndef EDIT1_H
# define EDIT1_H

# include <edit.h>

class t_EditTrans : public t_Edit
{
public :
   t_Text Text1 ;

	 t_EditTrans( void );
        ~t_EditTrans( void );
   void  set_name( char *Name );
   void  write( char *Name, char f_cp1251=0 );
   void  read(  char *Name, char f_cp1251=0 );
   char* get_frasa( long i );
   void  put_frasa( long i,char *Frasa );
   void  step( short f );
   void  next_frasa( void );
   void  prev_frasa( void );
   char* get_word( long i );
   void  next_word( void );
   void  prev_word( void );   
   void  set_funk_const( char (*is_const)( char *Str ) );

   long  current_pos( void );
   e_WinMsg main_loop( void );
   void  set_enum( void );
   void  set_enum1( char *pStr );
};

# endif
