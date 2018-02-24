/*******************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//            библиотека интерфейса с текстовым терминалом
//     Анисимов Д.В.                               сегодня
/*******************************************************************/
# ifndef SCHONE_H
# define SCHONE_H

# include "s_defkey.h"

# define uchar unsigned char
extern short n_Line,n_Col ;

/* ----------------------------------------------------------------------- */
   void   s_begin_schone( void );
   void   s_begin_schone( int argc, char *argv[] );
   void   s_end_schone( void );
   void   s_nacht( void );
   void   s_set_size( short sx, short sy );
   void   s_get_size( short &sx, short &sy );
   short  s_get_sx( void );
   short  s_get_sy( void );

   void   s_text_yxf(  short y, short x, uchar f, char *str );
   void   s_text_yxf1( short y, short x, uchar f, char *str );
   void   s_text_yxfl( short y, short x, uchar f, short L, char *str );
   void   s_text_yx( short y, short x, char *str );
   void   s_color_yxt( short y, short x, uchar *t, char *str );
   void   s_foreground_yxt( short y, short x, uchar *t, char *str );

   void   s_rame1_f( short y1, short x1, short y2, short x2, uchar f );
   void   s_rame1_F( short y1, short x1, short y2, short x2, uchar f );
   void   s_rame2_f( short y1, short x1, short y2, short x2, uchar f );
   void   s_rame2_F( short y1, short x1, short y2, short x2, uchar f );

   void   s_save     ( short y1, short x1, short y2, short x2 );
   void   s_save_rame( short y1, short x1, short y2, short x2 );
   void   s_restore( void );

   void   s_redraw( void );
   void   s_redraw_str( short y );

   short  s_clear_cursor( void );
   short  s_set_cursor( short S );
   void   s_goto_xy( short y,short x );
   void   s_getch( short *,short *) ;

   int    s_shiftstatus( void );
   void   s_set_font( char *Name );
   char  *s_get_font( void );
   void   s_refresh( void );
   void   s_set_ruskomb( short Komb );
   char   s_run();
   char   s_x_version( );

   void   s_debug( );

# endif
