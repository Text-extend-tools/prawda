/*******************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                             менюшка
//     Анисимов Д.В.                               сегодня
/*******************************************************************/

# include <stdio.h>
# include <malloc.h>
# include <string.h>

# include "mylib.h"
# include "video.h"
# include "color.h"
# include "menu.h1"

# define stop() {  short key1,key2 ; s_getch(&key1,&key2); }
 
/* ----------------------------------------------------------------------- */
struct t_menuForm
{  short   size_x,size_y,
	   n_punkt,
	  *igrek ;
   t_Menu *menu ;
} ;

static   char   *stek ;
static   short   SP ;
static   short  *mass_iks, *mass_igrek, Ret ;
static   char    Exist=0 ;

/* ----------------------------------------------------------------------- */

void   init_menu( void ) ;
short  hor_menu( short y, t_Menu *menu ) ;
void   s_hor_menu( short y, short *x, t_Menu *menu, short pos=-1 ) ;
void   s_ver_menu( short y, short x, t_menuForm F, short pos=-1 ) ;
short  ver_menu( short y, short x, t_Menu *menu ) ;
void   ver_menu_creat( t_menuForm *f, t_Menu *menu ) ;
short  sehen_hor_menu( short **x, t_Menu *menu ) ;
static short  schnell_taste( t_Menu *menu, uchar taste ) ;
void   sorry( short y, short x );

/* *********************************************************************** */
short menu( void )
{
    Ret=0 ;
    if( Exist==0 ) init_menu( );
    hor_menu( 0, wort_menu ) ;
    null_menu( );
    SP=0 ;
    return Ret ;
}
/* ----------------------------------------------------------------------- */
void init_menu( void )
{
    stek       = (char  *)Calloc( 4000,1 ) ;
    mass_iks   = (short *)Calloc( 25,sizeof(short) ) ;
    mass_igrek = (short *)Calloc( 25,sizeof(short) ) ;
    SP=0 ;
    Exist=1 ;
}
/* ----------------------------------------------------------------------- */
void null_menu( void )
{  short *x ;

   if( Exist==0 ) init_menu( );
   sehen_hor_menu( &x,wort_menu ) ;
   s_hor_menu( 0,x,wort_menu ) ;
}
/* ----------------------------------------------------------------------- */
/*             реакция на кнопки внутри горизонтального меню               */
/* ----------------------------------------------------------------------- */
short hor_menu( short y, t_Menu *menu )
{  short p=0,        // текущий пункт меню
	 flag=0 ;
   static short key1,key2,key3 ;
   short size_Menu ;
   short *x ;

   size_Menu=sehen_hor_menu( &x,menu ) ;
   while( 1 )
   {
       s_hor_menu( y,x,menu,p ) ;

       s_getch( &key1,&key2) ;

       switch( key2 )
       {   case S_key_Left: M_LINKS :
		if( p<=0 ) p=size_Menu-1 ; else p-- ;
		if( flag==1 && menu[p].flag==F_MENU )
		{   s_hor_menu( y,x,menu,p );
		    key3=ver_menu( 1,x[p], (t_Menu *)menu[p].data ) ;
		}
		else
		{  key3=0 ; }
		break ;
	   case S_key_Right: M_RECHTS :
		if( p>=size_Menu-1 ) p=0 ; else p++ ;
		if( flag==1 && menu[p].flag==F_MENU )
		{   s_hor_menu( y,x,menu,p );
		    key3=ver_menu( 1,x[p], (t_Menu *)menu[p].data ) ;
		}
		else
		{  key3=0 ; }
		break ;
	   case S_key_Down: goto M_ENTER ;
	   case S_key_F10 : return S_key_Esc ;
	   case 0 :
	   switch( key1 )
	   {
		schnell :
		    s_hor_menu( y,x,menu,p );
		case '\r' : M_ENTER :
		   if( menu[p].flag==4 )
		   {   Ret=((char *)menu[p].data)[0] ;
		       return S_key_Esc ;
		   }
		   if( menu[p].flag==3 )
		   {   menu[p].funk( menu[p].data ) ;
		       return S_key_Esc ;
		   }
		   if( menu[p].flag==2 )
		   {   flag=1 ;
		       key3=ver_menu( 1,x[p], (t_Menu *)menu[p].data ) ;
		   }
		   break ;
	       case S_key_Esc : return S_key_Esc ;
	       default :
		   {   short p1 ;
		       p1=schnell_taste( menu,key1 ) ;
		       if( 0<=p1 ) {  p=p1 ; goto schnell ;  }
		   }
		   break ;
	   }
       }
       switch( key3 )
       {   case S_key_Left : goto M_LINKS ;
	   case S_key_Right: goto M_RECHTS ;
	   case S_key_Esc  : return S_key_Esc ;
       }
   }
}
/* ----------------------------------------------------------------------- */
/*              реакция на кнопки внутри вертикального меню                */
/* ----------------------------------------------------------------------- */
short ver_menu( short y, short x, t_Menu *menu )
{  short p=0,ret ;
   static short key1,key2 ;
   t_menuForm  f ;
   short i ;

   ver_menu_creat( &f,menu ) ;
   s_save( y,x,y+f.size_y+1,x+f.size_x+1 );
   while( 1 )
   {
       s_ver_menu( y,x,f,p ) ;

       s_getch( &key1,&key2 ) ;

       switch( key2 )
       {   case S_key_Up:
		if( p<=0 ) p=f.n_punkt-1 ; else p-- ; break ;
	   case S_key_Down:
		if( p>=f.n_punkt-1 ) p=0 ; else p++ ; break ;
	   case S_key_Left : ret = S_key_Left ;  goto ENDE ;
	   case S_key_Right: ret = S_key_Right ; goto ENDE ;
	   case S_key_F10  : ret = S_key_Esc ;   goto ENDE ;
	   case 0 :
	       switch( key1 )
	       {   case '\r' :
	   schnell :   i=f.igrek[p] ;
		       if( menu[i].flag==4 )
		       {//   Ret=((char *)menu[p].data)[0] ;
		           Ret=((char *)menu[i].data)[0] ;
			   ret = S_key_Esc ;
			   goto ENDE ;
		       }
		       if( menu[i].flag==3 )
		       {   s_restore( ) ;
			   menu[i].funk( menu[i].data ) ;
			   ret = S_key_Esc ;  goto ENDE1 ;
		       }
		       if( menu[i].flag==2 )
		       {   ver_menu( y+i+1,x+f.size_x, (t_Menu *)menu[i].data ) ;
			   break ;
		       }
		   default :
		       {   short p1 ;
			   p1=schnell_taste( menu,key1 ) ;
			   if( 0<=p1 ) {  p=p1 ; goto schnell ;  }
		       }   break ;
	       }
	   case S_key_Esc : ret = S_key_Esc ; goto ENDE ;
       }
   }
ENDE :  s_restore( ) ;
ENDE1 :
   return ret ;
}
/* ----------------------------------------------------------------------- */
short schnell_taste( t_Menu *menu, uchar taste )
{  short i,j ;
   uchar c ;

   for( i=0 ; menu[i].flag!=0 ; i++ )
   {   for( j=0 ; ; j++ )
       {   c=menu[i].name[j] ;
	   if( c==0 ) break ;
	   if( (uchar)'А'<=c && c<=(uchar)'Я' )
	   {   if( c==taste ) return i ;
	       //if( XLAT[c]==XLAT[taste] ) return i ;
	       break ;
	   }
       }
   }
   return -1 ;
}
/* ----------------------------------------------------------------------- */
short sehen_hor_menu( short **iks, t_Menu *menu )
{  short i,j, *a ;

   *iks=a = mass_iks ;
   for( j=2,i=0 ; ; i++ )
   {  if( menu[i].flag==0 ) break ;
      a[i]=j ;
      j+=strlen( menu[i].name )+2 ;
   }
   return i ;
}
/* ----------------------------------------------------------------------- */
/*                     изобразить горизонтальное меню                      */
/* ----------------------------------------------------------------------- */
void s_hor_menu( short y, short *x, t_Menu *menu, short pos )
{  short i ;

   s_text_yxf( y,0,h_menu_Color,emptystr(80) ) ;
   for( i=0 ; i<25 ; i++ )
   {  if( menu[i].flag==0 ) break ;
      s_text_yxf( y,x[i],h_menu_Color,menu[i].name ) ;
   }
   // ------- изобразить текущую позицию горизонтального меню -------
   if( 0<=pos )
       s_text_yxf( y,x[pos],h_curs_Color,menu[pos].name ) ;
}
/* ----------------------------------------------------------------------- */
/*                     изобразить вертикальное меню                        */
/* ----------------------------------------------------------------------- */
void s_ver_menu( short y, short x, t_menuForm F, short pos )
{  short i,c ;

   s_rame1_F( y,x,y+F.size_y+1,x+F.size_x+1,v_menu_Color ) ;
   for( i=0 ; i<F.size_y ; i++ )
   {  if( i==mass_igrek[pos] )
      {   s_text_yxf( y+1+i,x+1,v_curs_Color,emptystr(F.size_x) ) ;
	  c=v_curs_Color ;
      }
      else c=v_menu_Color ;
      s_text_yxf( y+1+i,x+1,c,F.menu[i].name ) ;
   }
}
/* ----------------------------------------------------------------------- */
/*                       конструктор формы меню                            */
/* ----------------------------------------------------------------------- */
void ver_menu_creat( t_menuForm *f, t_Menu *menu )
{  short i,j,s,*igrek ;

   for( i=0 ; i<25 ; i++ ) if( menu[i].flag==0 ) break ;
   f->size_y=i ;

   igrek = mass_igrek ;
   for( s=j=0,i=0 ; i<f->size_y ; i++ )
   {  if( menu[i].flag==0 ) break ;
      s=max( (unsigned)s, strlen( menu[i].name ) );
      if( menu[i].flag==2 || menu[i].flag==3 || menu[i].flag==4 )
	  igrek[j++]=i ;
   }
   f->size_x=s ;     f->n_punkt=j ;
   f->igrek=igrek ;  f->menu=menu ;
}
/* ----------------------------------------------------------------------- */
//            переобозвать пункт меню (поставить в него галку)
/* ----------------------------------------------------------------------- */
char menu_set( t_Menu *Menu, char *sFind, char *sReplace )
{  short i ;

   for( i=0 ; i<100 ; i++ )
   {  if( Menu[i].flag==F_ENDE ) break ;
      if( 0==strcmp(Menu[i].name,sFind) )
      {   strcpy( Menu[i].name,sReplace );
          return 1 ;
      }	  
      if( Menu[i].flag==F_MENU )
          if( 1==menu_set( (t_Menu *)Menu[i].data,sFind, sReplace ) ) return 1;
   }
   return 0 ;
}
/* ----------------------------------------------------------------------- */
void menu_set( char *sFind, char *sReplace )
{
   menu_set( wort_menu, sFind, sReplace );
}
/* ----------------------------------------------------------------------- */
//                    найти пункт меню по его имени
/* ----------------------------------------------------------------------- */
t_Menu * menu_find( t_Menu *Menu, char *sFind )
{  short i ;
   t_Menu *R ;

   for( i=0 ; i<100 ; i++ )
   {  if( Menu[i].flag==F_ENDE ) break ;
      if( 0==strcmp(Menu[i].name,sFind) )
          return &Menu[i] ;
      if( Menu[i].flag==F_MENU )
      {   R=menu_find( (t_Menu *)Menu[i].data,sFind );
          if( R!=NULL ) return R ;
      }
   }
   return NULL ;
}
/* ----------------------------------------------------------------------- */
t_Menu * menu_find( char *sFind )
{
   return menu_find( wort_menu, sFind );
}
/* ----------------------------------------------------------------------- */
void Sorry( void *d ){   sorry( 10,30 ) ;  }
/* ----------------------------------------------------------------------- */
void sorry( short y, short x )
{
   s_save( y,x,y+4,x+20 ) ;
   s_rame1_f( y,x,y+4,x+20,0x70 ) ;
   s_text_yxf( y+1,x+1,0x70,"      Извините     ") ;
   s_text_yxf( y+2,x+1,0x70,"  эта функция еще  ") ;
   s_text_yxf( y+3,x+1,0x70,"  не  реализована  ") ;

   stop( ) ;
   s_restore( ) ;
}



