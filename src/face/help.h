/***************************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                     данные, для отображения подсказки 
//     Анисимов Д.В.                                         сегодня
/***************************************************************************/

# ifndef HELP_H
# define HELP_H

# include <list.h>

/* ----------------------- все, что относится к подсказке --------------- */
struct t_HelpLink
{   char Name[100] ;
    long i_file,i_pos ;
};
DECLARE_LIST_TYPE( t_HelpLink, t_HL_List );

struct t_HelpFile
{   char  Name[100] ;
    char *Text ;      //  1-закрывающая скобка  2-ссылка  3-жирный шрифт
    long  l_Text ;
};

class t_HelpData
{
   char        Path[200] ;
   t_HelpFile *File ;
   short       n_File ;
   t_HL_List   Link, Link1 ;
public :
   void  make( void );
   char *get_help( char *Str );
private:
   void  read_file( long i_File );
   void  mk_href( void );
   long  find_link( char *Str );
   void  hprint( void );
};
# endif
