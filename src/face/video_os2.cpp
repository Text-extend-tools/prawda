/*******************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//     библиотека интерфейса с текстовым терминалом (OS/2 & M$-Windows)
//     Анисимов Д.В.                               сегодня
/*******************************************************************/

# include <malloc.h>
# include <string.h>
# include <conio.h>
# include <mylib.h>
# include <s_defkey.h>
# include <video.h>

# define max(a,b)    (((a) > (b)) ? (a) : (b))
# define min(a,b)    (((a) < (b)) ? (a) : (b))

char  *Symbol ;
uchar *Color ;

class t_Save_Screen
{  long   *stek ;   // ╖═╞╝╛╗╜═╜╗╔ ╝║╚═─│╔╘ ▄╙·═╜═
   short   l_stek ; // ╓╚╗╜╜═ ╛═──╗╒═ Stek
   short   j_stek ; // √╙═╖═│╔╚▀ ╞╝─╚╔╓╜╔ё╝ ─╒╝║╝╓╜╝ё╝ ║═╘│═
public:
   t_Save_Screen( short l );
   ~t_Save_Screen( void );
   void   save   ( short y1, short x1, short y2, short x2 );
   void   save_rame( short y1, short x1, short y2, short x2 );
   void   restore( void );
} ;

short       Cur_X=0,Cur_Y=0 ;
short       n_Line,n_Col ;
t_Save_Screen  Save( 10000 ) ;

void s_rame( short y1, short x1, short y2, short x2, uchar f );
void s_quadro( short y1, short x1, short y2, short x2, uchar f );

/***************************************************************************/
void   s_begin_schone( void )
{  short i ;

   n_Line=50 ;
   n_Col =78 ;

   Symbol=(char  *)Calloc( n_Line*n_Col,sizeof(char) );
   Color =(uchar *)Calloc( n_Line*n_Col,1 );
   for( i=0 ; i<n_Line*n_Col ; i++ )
      Symbol[i]=' ';

   textmode( C4350 );
}
/* ----------------------------------------------------------------------- */
void   s_begin_schone( int argc, char *argv[] )
{  s_begin_schone( );  }
/* ----------------------------------------------------------------------- */
void   s_end_schone( void )
{  ;  }
/* ----------------------------------------------------------------------- */
void   s_nacht( void )
{  long i ;

   for( i=0 ; i<n_Line*n_Col ; i++ )
   {  Symbol[i]=' ' ; Color[i]=0 ;  }
}
/* ----------------------------------------------------------------------- */
void   s_get_size( short *sx, short *sy )
{
   *sx=n_Col ; *sy=n_Line ;
}
/* ----------------------------------------------------------------------- */
void   s_text_yxf(  short y, short x, uchar f, char *str )
{  short i,r1 ;

   r1=n_Col*y+x ;
   for( i=0 ; str[i]!=0 && str[i]!='\r' && str[i]!='\n' ; i++ )
   {  Symbol[ r1+i ]=str[i] ;
      Color [ r1+i ]=f ;
   }
}
/* ----------------------------------------------------------------------- */
void   s_text_yxf1( short y, short x, uchar f, char *str )
{  short i,r ;

   r = n_Col*y+x ;
   Symbol[ r ] = '\"' ; Color[ r ] = f ;
   for( i=0 ; str[i]!=0 ; i++ )
   {  r=n_Col*y+x+i+1 ;
      Symbol[ r ] = str[i] ; Color[ r ] = f ;
   }
   r=r+1 ;
   Symbol[ r ] = '\"' ; Color[ r ] = f ;
}
/* ----------------------------------------------------------------------- */
void   s_text_yxfl( short y, short x, uchar f, short L, char *str )
{  short i,r1 ;

   r1=n_Col*y+x ;
   for( i=0 ; str[i]!=0 && i<L ; i++ )
   {  Symbol[ r1+i ]=str[i] ;
      Color [ r1+i ]=f ;
   }
}
/* ----------------------------------------------------------------------- */
void   s_text_yx( short y, short x, char *str )
{  short i,r1 ;

   r1=n_Col*y+x ;
   for( i=0 ; str[i]!=0 ; i++ )
      Symbol[ r1+i ]=str[i] ;
}
/* ----------------------------------------------------------------------- */
void   s_color_yxt( short y, short x, uchar *t, char *str )
{  short i,r1,r2,f ;

   r1=n_Col*y+x ;
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

// зд©
// Ё Ё
// юды

   for( i=x1+1 ; i<x2 ; i++ )
   {  r=n_Col*y1+i ; Symbol[ r ]='д' ; Color[ r ]=f ;
      r=n_Col*y2+i ; Symbol[ r ]='д' ; Color[ r ]=f ;
   }
   for( i=y1+1 ; i<y2 ; i++ )
   {  r=n_Col*i+x1 ; Symbol[ r ]='Ё' ; Color[ r ]=f ;
      r=n_Col*i+x2 ; Symbol[ r ]='Ё' ; Color[ r ]=f ;
   }
   r=n_Col*y1+x1 ; Symbol[ r ]='з' ; Color[ r ]=f ;
   r=n_Col*y2+x1 ; Symbol[ r ]='ю' ; Color[ r ]=f ;
   r=n_Col*y1+x2 ; Symbol[ r ]='©' ; Color[ r ]=f ;
   r=n_Col*y2+x2 ; Symbol[ r ]='ы' ; Color[ r ]=f ;
}
/* ----------------------------------------------------------------------- */
void s_quadro( short y1, short x1, short y2, short x2, uchar f )
{  short i,j,r1 ;

   for( j=y1+1 ; j<y2 ; j++ )
   {  for( i=x1+1 ; i<x2 ; i++ )
      {   r1=n_Col*j+i ;
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
t_Save_Screen :: t_Save_Screen( short L )
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
      {   r=n_Col*i+j ;
	  stek[j_stek++] = Color [ r ] ; /* ═│·╗║√│ */
	  stek[j_stek++] = Symbol[ r ] ;   /* ─╗╛╒╝╚  */
      }
   }
   stek[j_stek++]=(uchar )x1 ; stek[j_stek++]=(uchar )y1 ;
   stek[j_stek++]=(uchar )x2 ; stek[j_stek++]=(uchar )y2 ;
   stek[j_stek++]=0 ;
}
/* ----------------------------------------------------------------------- */
void t_Save_Screen :: save_rame( short s1, short p1, short s2, short p2 )
{  short i,r,r1,r2 ;

   r1=n_Col*s1 ; r2=n_Col*s2 ;
   for( i=p1 ; i<=p2 ; i++ )
   {  r=r1+i ;
      stek[j_stek++]=Symbol[ r ] ;
      stek[j_stek++]=Color [ r ] ;
      r=r2+i ;
      stek[j_stek++]=Symbol[ r ] ;
      stek[j_stek++]=Color [ r ] ;
   }
   for( i=s1 ; i<=s2 ; i++ )
   {  r1=n_Col*i+p1 ;
      stek[j_stek++]=Symbol[ r1 ] ;
      stek[j_stek++]=Color [ r1 ] ;
      r2=n_Col*i+p2 ;
      stek[j_stek++]=Symbol[ r2 ] ;
      stek[j_stek++]=Color [ r2 ] ;
   }
   stek[j_stek++]=(uchar )p1 ; stek[j_stek++]=(uchar )s1 ;
   stek[j_stek++]=(uchar )p2 ; stek[j_stek++]=(uchar )s2 ;
   stek[j_stek++]=1 ;
}
/* ----------------------------------------------------------------------- */
void t_Save_Screen :: restore( void )
{  short i,j,p1,p2,s1,s2,
	 r,r1,r2 ;
   uchar f ;

   f =stek[--j_stek] ;
   s2=stek[--j_stek] ; p2=stek[--j_stek] ;
   s1=stek[--j_stek] ; p1=stek[--j_stek] ;
   if( f==1 )
   {  for( i=s2 ; s1<=i ; i-- )
      {  r=n_Col*i+p2 ;
	 Color [ r ] = stek[--j_stek] ;
	 Symbol[ r ] = stek[--j_stek] ;
	 r=n_Col*i+p1 ;
	 Color [ r ] = stek[--j_stek] ;
	 Symbol[ r ] = stek[--j_stek] ;
      }
      r1=n_Col*s1 ; r2=n_Col*s2 ;
      for( i=p2 ; p1<=i ; i-- )
      {  r=r2+i ;
	 Color [ r ] = stek[--j_stek] ;
	 Symbol[ r ] = stek[--j_stek] ;
	 r=r1+i ;
	 Color [ r ] = stek[--j_stek] ;
	 Symbol[ r ] = stek[--j_stek] ;
      }
   }
   else
   {  for( i=s2 ; s1<=i ; i-- )
	 for( j=p2 ; p1<=j ; j-- )
	 {   r=n_Col*i+j ;
	     Symbol[ r ] = stek[--j_stek] ; /* ─╗╛╒╝╚  */
	     Color [ r ] = stek[--j_stek] ; /* ═│·╗║√│ */
	 }
   }
}
/* ----------------------------------------------------------------------- */
void   s_redraw1( void )
{  short x,y,r ;
   uchar f,f1=0 ;

   for( y=0 ; y<n_Line ; y++ )
   {  r=n_Col*y ;
      gotoxy( 1,1+y );
      for( x=0 ; x<n_Col ; x++ )
      {  f=Color[r+x] ;

         if( f!=f1 )
	 {  textcolor( f );
	    textbackground( f>>4 );
	 }
	 //if( (f&0x80)!=0 ) attr_on ( A_BLINK,NULL );
	 //    else          attr_off( A_BLINK,NULL );
	 //if( (f&0x08)!=0 ) attr_on ( A_BOLD ,NULL );
	 //    else          attr_off( A_BOLD ,NULL );

         putch( Symbol[r+x] );
         f1=f ;  
      }
   }
   gotoxy( 1+Cur_X,1+Cur_Y );
}
/* ----------------------------------------------------------------------- */
char Screen[ 16000 ];

void   s_redraw( void )
{  short x,y,r,r1 ;

   for( y=0 ; y<n_Line ; y++ )
   {  r=n_Col*y ;
      for( x=0 ; x<n_Col ; x++ )
      {  r1=(n_Col*y+x)*2 ;

	 Screen[r1+1]=Color [r+x] ;
	 Screen[r1  ]=Symbol[r+x] ;
      }
   }
//   puttext( 1,1,80,50,Screen );
   puttext( 1,1,78,50,Screen );
   gotoxy( 1+Cur_X,1+Cur_Y );
}
/* ----------------------------------------------------------------------- */
void   s_goto_xy( short y, short x )
{
   Cur_X=x ; Cur_Y=y ;
}
/* ----------------------------------------------------------------------- */
void   s_getch( short *key1, short *key2 )
{
   s_redraw( );
   *key1=getch();
   if( *key1==0 ) *key2=getch();
       else       *key2=0 ; 
}
/***************************************************************************/
int    s_shiftstatus( void )
{  
   return 0 ;
}
/***************************************************************************/

   void   s_set_size( short sx, short sy ){ ; }
   void   s_get_size( short &sx, short &sy ){  sx=n_Col ; sy=n_Line ;  }
   short  s_get_sx( void ) {  return n_Col ; }
   short  s_get_sy( void ) {  return n_Line ; }

   void   x_set_fonts( void ){;}
   short  s_clear_cursor( void ){ return 0;}
   void   s_refresh(){;}

