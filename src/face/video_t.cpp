/*******************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//          библиотека интерфейса с текстовым терминалом (xterm)
//          Анисимов Д.В.                               сегодня
/*******************************************************************/

# include <malloc.h>
# include <string.h>
# include <curses.h>
# include <mylib.h>
# include <s_defkey.h>
# include <video.h>

//# define _SUN_

# define  DEFOULT 0
# define  XTERM   1
# define  NXTERM  2
# define  RXVT    3
# define  CONSOLE 4
# define  SUN     5

   long  *Symbol ;
   uchar *Color ;
   short  max_x,max_y ;
   short  size_x,size_y ;
   char   TermName[40] ;
   char   TermType ;
   char   f_Color ;
   int    shift2=0 ;

class t_Save_Screen
{  long   *stek ;   // запоминание областей экрана
   long   l_stek ; // длинна массива Stek
   long   j_stek ; // указатель последнего свободного байта
public:
   t_Save_Screen( long l );
   ~t_Save_Screen( void );
   void   save   ( short y1, short x1, short y2, short x2 );
   void   save_rame( short y1, short x1, short y2, short x2 );
   void   restore( void );
} ;

# ifndef _SUN_
mmask_t     mm ;
#endif 
WINDOW     *Win ;
short       Cur_X=0,Cur_Y=0 ;
short       n_Line,n_Col ;
char        Run=0 ; // признак того, что графика запущена
static char    TextFont[100] ;
t_Save_Screen  Save( 51200 ) ;

void s_rame( short y1, short x1, short y2, short x2, uchar f );
void s_quadro( short y1, short x1, short y2, short x2, uchar f );

/***************************************************************************/
void   s_begin_schone( void )
{  short i,i1 ;

   max_x=256 ; max_y=100 ;
   Symbol=(long  *)Calloc( max_x*max_y,sizeof(long) );
   Color =(uchar *)Calloc( max_x*max_y,1 );
   for( i=0 ; i<max_x*max_y ; i++ )
      Symbol[i]=' ';

   Win=initscr();
   noecho( );
   keypad( Win, TRUE );
   strcpy( TermName,termname() );

   TermType=DEFOULT ;
   if( 0==strncmp( TermName,"xterm"  ,5) )   TermType=XTERM ; 
   if( 0==strncmp( TermName,"nxterm" ,6) )   TermType=NXTERM ;
   if( 0==strncmp( TermName,"rxvt"   ,4) )   TermType=RXVT ;
   if( 0==strncmp( TermName,"linux"  ,5) )   TermType=CONSOLE ; 
   if( 0==strncmp( TermName,"cmdtool",7) )   TermType=SUN ;

   getmaxyx( Win, size_y, size_x );
   n_Line=size_y ;
   n_Col =size_x ;

# ifndef _SUN_
//   mousemask( ALL_MOUSE_EVENTS,&mm );
   f_Color=has_colors();

   if( f_Color!=0 )
   {
      start_color();
      long Color[8] ;
      Color[0]=COLOR_BLACK ;
      Color[1]=COLOR_BLUE ;
      Color[2]=COLOR_GREEN ;
      Color[3]=COLOR_CYAN ;
      Color[4]=COLOR_RED ;
      Color[5]=COLOR_MAGENTA ;
      Color[6]=COLOR_YELLOW ;
      Color[7]=COLOR_WHITE ;

      for( i=0 ; i<8 ; i++ )
	 for( i1=0 ; i1<8 ; i1++ )
	    init_pair( 1+8*i1+i,Color[i],Color[i1] );
      color_set( 1,NULL );	    
   }
# endif
   Run=1 ;
}
/* ----------------------------------------------------------------------- */
void   s_begin_schone( int argc, char *argv[] )
{  s_begin_schone( );  }
/* ----------------------------------------------------------------------- */
void   s_end_schone( void )
{  endwin();  
   Run=0 ;
}
/* ----------------------------------------------------------------------- */
void   s_nacht( void )
{  long i ;

   for( i=0 ; i<max_x*max_y ; i++ )
   {  Symbol[i]=' ' ; Color[i]=0 ;  }
}
/* ----------------------------------------------------------------------- */
void   s_get_size( short *sx, short *sy )
{  short size_y1, size_x1 ;

   getmaxyx( Win, size_y1, size_x1 );
   if( max_x<size_x1 ) size_x1=max_x ;
   if( max_y<size_y1 ) size_y1=max_y ;
   *sx=size_x1 ; *sy=size_y1 ;
   size_x=size_x1 ; size_y=size_y1 ;
}
/* ----------------------------------------------------------------------- */
void   s_text_yxf(  short y, short x, uchar f, char *str )
{  short i,r1 ;

   r1=max_x*y+x ;
   for( i=0 ; str[i]!=0 && str[i]!='\r' && str[i]!='\n' ; i++ )
   {  Symbol[ r1+i ]=str[i] ;
      Color [ r1+i ]=f ;
   }
}
/* ----------------------------------------------------------------------- */
void   s_text_yxf1( short y, short x, uchar f, char *str )
{  short i,r ;

   r = max_x*y+x ;
   Symbol[ r ] = '\"' ; Color[ r ] = f ;
   for( i=0 ; str[i]!=0 ; i++ )
   {  r=max_x*y+x+i+1 ;
      Symbol[ r ] = str[i] ; Color[ r ] = f ;
   }
   r=r+1 ;
   Symbol[ r ] = '\"' ; Color[ r ] = f ;
}
/* ----------------------------------------------------------------------- */
void   s_text_yxfl( short y, short x, uchar f, short L, char *str )
{  short i,r1 ;

   r1=max_x*y+x ;
   for( i=0 ; str[i]!=0 && i<L ; i++ )
   {  Symbol[ r1+i ]=str[i] ;
      Color [ r1+i ]=f ;
   }
}
/* ----------------------------------------------------------------------- */
void   s_text_yx( short y, short x, char *str )
{  short i,r1 ;

   r1=max_x*y+x ;
   for( i=0 ; str[i]!=0 ; i++ )
      Symbol[ r1+i ]=str[i] ;
}
/* ----------------------------------------------------------------------- */
void   s_color_yxt( short y, short x, uchar *t, char *str )
{  short i,r1,r2,f ;

   r1=max_x*y+x ;
   for( i=0 ; str[i]!=0 ; i++ )
   {  f=0x07 ; r2=str[i] ;
      if( '0'<=r2 && r2<= '9' ) f=t[r2-'0'] ;
      if( 'a'<=r2 && r2<= 'z' ) f=t[r2-'a'+10] ;
      Color[ r1+i ]=f ;
   }
}
/* ----------------------------------------------------------------------- */
void   s_foreground_yxt( short y, short x, uchar *t, char *str )
{  short i,r1,r2,f ;

   r1=max_x*y+x ;
   for( i=0 ; str[i]!=0 ; i++ )
   {  f=0x07 ; r2=str[i] ;
      if( '0'<=r2 && r2<= '9' ) f=t[r2-'0'] ;
      if( 'a'<=r2 && r2<= 'z' ) f=t[r2-'a'+10] ;
      Color[ r1+i ] = (0xf0 & Color[ r1+i ]) + (0x0f & f) ;
   }
}
/* ----------------------------------------------------------------------- */
void   s_rame1_f( short y1, short x1, short y2, short x2, uchar f )
{
   s_rame( y1, x1, y2, x2, f );
}
/* ----------------------------------------------------------------------- */
void   s_rame1_F( short y1, short x1, short y2, short x2, uchar f )
{
   s_quadro( y1, x1, y2, x2, f );
   s_rame  ( y1, x1, y2, x2, f );
}
/* ----------------------------------------------------------------------- */
void   s_rame2_f( short y1, short x1, short y2, short x2, uchar f )
{
   s_rame  ( y1, x1, y2, x2, f );
}
/* ----------------------------------------------------------------------- */
void   s_rame2_F( short y1, short x1, short y2, short x2, uchar f )
{
   s_quadro( y1, x1, y2, x2, f );
   s_rame  ( y1, x1, y2, x2, f );
}
/* ----------------------------------------------------------------------- */
void s_rame( short y1, short x1, short y2, short x2, uchar f )
{  short i,r ;

   for( i=x1+1 ; i<x2 ; i++ )
   {  r=max_x*y1+i ; Symbol[ r ]=ACS_HLINE ; Color[ r ]=f ;
      r=max_x*y2+i ; Symbol[ r ]=ACS_HLINE ; Color[ r ]=f ;
   }
   for( i=y1+1 ; i<y2 ; i++ )
   {  r=max_x*i+x1 ; Symbol[ r ]=ACS_VLINE ; Color[ r ]=f ;
      r=max_x*i+x2 ; Symbol[ r ]=ACS_VLINE ; Color[ r ]=f ;
   }
   r=max_x*y1+x1 ; Symbol[ r ]=ACS_ULCORNER ; Color[ r ]=f ;
   r=max_x*y2+x1 ; Symbol[ r ]=ACS_LLCORNER ; Color[ r ]=f ;
   r=max_x*y1+x2 ; Symbol[ r ]=ACS_URCORNER ; Color[ r ]=f ;
   r=max_x*y2+x2 ; Symbol[ r ]=ACS_LRCORNER ; Color[ r ]=f ;
}
/* ----------------------------------------------------------------------- */
void s_quadro( short y1, short x1, short y2, short x2, uchar f )
{  short i,j,r1 ;

   for( j=y1+1 ; j<y2 ; j++ )
   {  for( i=x1+1 ; i<x2 ; i++ )
      {   r1=max_x*j+i ;
	  Symbol[ r1 ]=' ' ;
	  Color [ r1 ]=f ;
      }
   }
}
/* ----------------------------------------------------------------------- */
void s_save_rame( short s1, short p1, short s2, short p2 )
{  Save.save_rame( s1, p1, s2, p2 );  }

void s_save( short s1, short p1, short s2, short p2 )
{  Save.save( s1, p1, s2, p2 );  }

void s_restore( void )
{  Save.restore( );  }
/* ----------------------------------------------------------------------- */
t_Save_Screen :: t_Save_Screen( long L )
{
   stek=(long *)Calloc( L,sizeof(long) );
   j_stek=0 ;
   l_stek=L ;
}
/* ----------------------------------------------------------------------- */
t_Save_Screen :: ~t_Save_Screen( void )
{
   if( stek!=NULL ){  free( stek ); stek=NULL ;  }
   j_stek=0 ;
   l_stek=0 ;
}
/* ----------------------------------------------------------------------- */
void t_Save_Screen :: save( short y1, short x1, short y2, short x2 )
{  short i,j,r ;

   for( i=y1 ; i<=y2 ; i++ )
   {  for( j=x1 ; j<=x2 ; j++ )
      {   r=max_x*i+j ;
	  stek[j_stek++] = Color [ r ] ; /* атрибут */
	  stek[j_stek++] = Symbol[ r ] ;   /* символ  */
      }
   }
   stek[j_stek++]=(uchar )x1 ; stek[j_stek++]=(uchar )y1 ;
   stek[j_stek++]=(uchar )x2 ; stek[j_stek++]=(uchar )y2 ;
   stek[j_stek++]=0 ;
   if( l_stek<=j_stek )
   {   printf("\n video_t.cpp : переполнение stek"); throw(-1);  }
}
/* ----------------------------------------------------------------------- */
void t_Save_Screen :: save_rame( short s1, short p1, short s2, short p2 )
{  short i,r,r1,r2 ;

   r1=max_x*s1 ; r2=max_x*s2 ;
   for( i=p1 ; i<=p2 ; i++ )
   {  r=r1+i ;
      stek[j_stek++]=Symbol[ r ] ;
      stek[j_stek++]=Color [ r ] ;
      r=r2+i ;
      stek[j_stek++]=Symbol[ r ] ;
      stek[j_stek++]=Color [ r ] ;
   }
   for( i=s1 ; i<=s2 ; i++ )
   {  r1=max_x*i+p1 ;
      stek[j_stek++]=Symbol[ r1 ] ;
      stek[j_stek++]=Color [ r1 ] ;
      r2=max_x*i+p2 ;
      stek[j_stek++]=Symbol[ r2 ] ;
      stek[j_stek++]=Color [ r2 ] ;
   }
   stek[j_stek++]=(uchar )p1 ; stek[j_stek++]=(uchar )s1 ;
   stek[j_stek++]=(uchar )p2 ; stek[j_stek++]=(uchar )s2 ;
   stek[j_stek++]=1 ;
   if( l_stek<=j_stek )
   {   printf("\n video_t.cpp : переполнение stek"); throw(-1);  }

}
/* ----------------------------------------------------------------------- */
void t_Save_Screen :: restore( void )
{  short i,j,p1,p2,s1,s2,
	 r,r1,r2 ;
   uchar c,f ;

   f =stek[--j_stek] ;
   s2=stek[--j_stek] ; p2=stek[--j_stek] ;
   s1=stek[--j_stek] ; p1=stek[--j_stek] ;
   if( f==1 )
   {  for( i=s2 ; s1<=i ; i-- )
      {  r=max_x*i+p2 ;
	 f=Color [ r ] = stek[--j_stek] ;
	 c=Symbol[ r ] = stek[--j_stek] ;
	 r=max_x*i+p1 ;
	 f=Color [ r ] = stek[--j_stek] ;
	 c=Symbol[ r ] = stek[--j_stek] ;
      }
      r1=max_x*s1 ; r2=max_x*s2 ;
      for( i=p2 ; p1<=i ; i-- )
      {  r=r2+i ;
	 f=Color [ r ] = stek[--j_stek] ;
	 c=Symbol[ r ] = stek[--j_stek] ;
	 r=r1+i ;
	 f=Color [ r ] = stek[--j_stek] ;
	 c=Symbol[ r ] = stek[--j_stek] ;
      }
   }
   else
   {  for( i=s2 ; s1<=i ; i-- )
	 for( j=p2 ; p1<=j ; j-- )
	 {   r=max_x*i+j ;
	     c=Symbol[ r ] = stek[--j_stek] ; /* символ  */
	     f=Color [ r ] = stek[--j_stek] ; /* атрибут */
	 }
   }
}
/* ----------------------------------------------------------------------- */
void   s_redraw( void )
{  short x,y,r ;
   uchar f ;

   for( y=0 ; y<size_y ; y++ )
   {  r=max_x*y ;
      move( y,0 );
      for( x=0 ; x<size_x ; x++ )
      {  f=Color[r+x] ;
# ifndef _SUN_
	 color_set( 1+(f&0x07)+((f&0x70)>>1) ,NULL );
//	 if( (f&0x80)!=0 ) attr_on ( A_BLINK,NULL );
//	     else          attr_off( A_BLINK,NULL );
//	 if( (f&0x08)!=0 ) attr_on ( A_BOLD ,NULL );
//	     else          attr_off( A_BOLD ,NULL );
# endif
	 if( 256<=Symbol[r+x] )
	 {   addch( Symbol[r+x] );  }
	 else
	 {   addch( Symbol[r+x]&0x00ff ); }
      }
   }
   move( Cur_Y,Cur_X );
}
/* ----------------------------------------------------------------------- */
void   s_goto_xy( short y, short x )
{
   Cur_X=x ; Cur_Y=y ;
}
/* ----------------------------------------------------------------------- */
void   s_getch( short *key1, short *key2 )
{  short key ;

   getmaxyx( Win, size_y, size_x );
   shift2=0 ;
   n_Line=size_y ;
   n_Col =size_x ;

   s_redraw( );
   key=wgetch( Win );

   if( key== 10 ) {  *key1=S_key_Enter ; *key2=0 ; return ;  }
   if( key==127 ) {  *key1=0 ; *key2=S_key_Del ; return ;  }
   if( key <256 ) {  *key1=key ; *key2=0 ; return ;  }

   *key1=0 ;

   if( TermType==XTERM && 277<=key && key<=286 )
       shift2=S_Ctrl_L ;

   switch( key )
   {  case 259 : *key2=S_key_Up   ; break ;
      case 258 : *key2=S_key_Down ; break ;
      case 260 : *key2=S_key_Left ; break ;
      case 261 : *key2=S_key_Right; break ;
      case 265 : *key2=S_key_F1   ; break ;
      case 266 : *key2=S_key_F2   ; break ;
      case 267 : *key2=S_key_F3   ; break ;
      case 268 : *key2=S_key_F4   ; break ;
      case 269 : *key2=S_key_F5   ; break ;
      case 270 : *key2=S_key_F6   ; break ;
      case 271 : *key2=S_key_F7   ; break ;
      case 272 : *key2=S_key_F8   ; break ;
      case 273 : *key2=S_key_F9   ; break ;
      case 274 : *key2=S_key_F10  ; break ;
      case 331 : *key2=S_key_Ins  ; break ;
      case 339 : *key2=S_key_PgUp ; break ;
      case 338 : *key2=S_key_PgDn ; break ;
      case 263 : *key1=S_key_Back ; *key2=0 ; break ;
      // rxvt
      case 362 : *key2=S_key_Home ; break ;
      case 385 : *key2=S_key_End  ; break ;
      case 330 : *key2=S_key_Del  ; break ;
      // linux
      case 262 : *key2=S_key_Home ; break ;
      case 360 : *key2=S_key_End  ; break ;
      default: *key2=0 ;
   }
   if( TermType==CONSOLE )
   {   switch( key )
       {  case 275 : *key2=S_key_F1   ; break ;
	  case 276 : *key2=S_key_F2   ; break ;
	  case 277 : *key2=S_key_F3   ; break ;
	  case 278 : *key2=S_key_F4   ; break ;
	  case 279 : *key2=S_key_F5   ; break ;
	  case 280 : *key2=S_key_F6   ; break ;
	  case 281 : *key2=S_key_F7   ; break ;
	  case 282 : *key2=S_key_F8   ; break ;
	  case 283 : *key2=S_key_F9   ; break ;
	  case 284 : *key2=S_key_F10  ; break ;
       }
   }
   if( TermType==XTERM )
   {   switch( key )
       {  case 277 : *key2=S_key_F1   ; break ;
	  case 278 : *key2=S_key_F2   ; break ;
	  case 279 : *key2=S_key_F3   ; break ;
	  case 280 : *key2=S_key_F4   ; break ;
	  case 281 : *key2=S_key_F5   ; break ;
	  case 282 : *key2=S_key_F6   ; break ;
	  case 283 : *key2=S_key_F7   ; break ;
	  case 284 : *key2=S_key_F8   ; break ;
	  case 285 : *key2=S_key_F9   ; break ;
	  case 286 : *key2=S_key_F10  ; break ;
	  case 275 : *key2=S_key_F11  ; break ;
	  case 276 : *key2=S_key_F12  ; break ;
       }
   }
}
/***************************************************************************/
# include <sys/ioctl.h>

int    s_shiftstatus( void )
{  char shift=6 ;
   int  shift1=0 ;

# ifndef _SUN_
   if( TermType==CONSOLE )
   {
      ioctl( 0,TIOCLINUX,&shift );

      if( shift&0x01 ) shift1|=S_Shift_L ;
      if( shift&0x04 ) shift1|=S_Ctrl_L ;
      if( shift&0x08 ) shift1|=S_Alt_L ;
   }
# endif
   
   if( TermType==XTERM )
       return shift2 ;
       
   return shift1 ;
}
/***************************************************************************/

void   s_set_size( short sx, short sy ){ ; }
void   s_get_size( short &sx, short &sy ){  sx=n_Col ; sy=n_Line ;  }
short  s_get_sx( void ) {  return n_Col ; }
short  s_get_sy( void ) {  return n_Line ; }

void   x_set_fonts( void ){;}
void   s_set_font( char *F ){ strcpy( TextFont,F ); }
char  *s_get_font( void ){ return TextFont ; }
short  s_clear_cursor( void ){ return 0; }
void   s_refresh( void ){ refresh(); }
void   s_set_ruskomb( short Komb ){  ;  }
char   s_run( ){  return Run ;  }
char   s_x_version( ){  return 0 ;  }
