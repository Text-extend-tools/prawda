/***************************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                       удобные штучки для интерфейса
//     Анисимов Д.В.                                         сегодня
/***************************************************************************/

# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <malloc.h>
# include "mylib.h"
# include "video.h"
# include "color.h"
# include "window.h"

short kommand_yxf1( short y, short x, char **str, short n_str );
e_WinMsg kommand_yxf( short y, short x, char f, char *str, short lenght,
		   char EIN, char **str1=NULL, short n_str1=0 );

/***************************************************************************/
//                      Выдать сообщение или ошибку
//   flag 0-ошика 1-сообщение
/***************************************************************************/
short my_figna( char *str, char flag )
{  short i,j,x,y,y_i,n_str,l_str,r1,sx,sy ;
   short key1,key2 ;
   char  str1[80] ;

   j=0 ; l_str=0 ;
   for( n_str=1,i=0 ; i<512 ; i++ )
   {   if( str[i]==0 )
       {  if( i-j>l_str ) l_str=i-j ;
	  break ;
       }
       if( str[i]=='\n' )
       {  if( i-j>l_str ) l_str=i-j ;
	  n_str++ ; j=i ;
       }
   }
   s_get_size( sx, sy );
   x=sx/2-l_str/2-1 ; y=sy/2-n_str/2-2 ;

   if( flag!=1 )
       s_save( y,x,y+n_str+1,x+l_str+2 ) ;

   s_rame1_F( y,x,y+n_str+1,x+l_str+2,0x70 ) ;
   for( y_i=0,r1=0 ; y_i<n_str ; y_i++ )
   {   for( i=r1,j=0 ; i<512 ; i++ )
       {   if( str[i]==0 || str[i]=='\n' ){  str1[j]=0 ; r1=i+1 ; break ;  }
	   str1[j++]=str[i] ;
       }
       s_text_yxf( y+y_i+1,x+1,0x70,str1 ) ;
   }
   if( flag==0 )
   {
      s_getch( &key1,&key2 );
      s_restore( ) ;
   }
   if( flag==1 )
   {  s_redraw();
      s_refresh();
   }
   if( flag==2 )
   {   short ret ;
    M1:s_getch( &key1,&key2 );
       switch( key1 )
       {  case '1': ret = 1 ; break ;
	  case '0': ret = 0 ; break ;
	  case S_key_Esc: ret = S_key_Esc ; break ;
	  default : goto M1 ;
       }
       s_restore( ) ;
       return ret ;
   }
   return 0 ;
}
/***************************************************************************/
//                          Выдать сообщение
/***************************************************************************/
void message( char *str )
{  my_figna( str, 1 );  }
/***************************************************************************/
//                          Выдать сообщение
/***************************************************************************/
void figna( char *str )
{  my_figna( str, 0 );  }
/***************************************************************************/
/*                   Задать вопрос и получить ответ                        */
/***************************************************************************/
short fragen( char *str )
{  return  my_figna( str, 2 );  }
/***************************************************************************/
//                            Ввести строку
//   x y - координаты на экране
//   f   - цвет
//   str - заполняемая строка
//   length - максимальная длинна строки
//   EIN    - можно ли ходить вверх-вниз (в соседние виджеты)
//   str1   - предыдущие введенные строки
//   n_str1 - число предыдущих строк
/***************************************************************************/
e_WinMsg kommand_yxf( short y, short x, char f, char *str, short lenght, char EIN,
		   char **str1, short n_str1 )
{  short i,p=0,key1,key2,ZZ,REG=1,L ;
   char alt_str[80] ;

strcpy( alt_str,str ) ;
L=strlen( str ) ;

while( 1==1 )
{
   s_text_yxf1( y,x,f,str ) ;
   s_text_yxf( y,x+L+2,f,emptystr(lenght-L-2) ) ;
   s_goto_xy( y,x+p+1 );

   s_getch( &key1,&key2 );
   ZZ=s_shiftstatus();
   if( key1==0 )
   {   switch(key2)
       {  case S_key_Left : if( ZZ&(S_Ctrl_L|S_Ctrl_R) ) return WM_PREV ;
                            if( p>0 ) p-- ; break ;
	  case S_key_Right: if( ZZ&(S_Ctrl_L|S_Ctrl_R) ) return WM_NEXT ;
	                    if( p<L && str[p+1]!=0 ) p++ ; break ;
	  case S_key_Up   : if( (EIN&0x01)==0 ) break ;
			    return WM_PREV ;
	  case S_key_Down : if( (EIN&0x01)==0 ) break ;
			    return WM_NEXT ;
	  case S_key_Del  :
		    if( str[p]==0 || p>=L ) break ;
		    for( i=p ; i<L && str[i]!=0 ; i++ )
		       str[i]=str[i+1] ;
		    str[L]=0 ; L-- ; break ;
	  case S_key_Ins :
		    if(REG==0) {  REG=1 ;  }
		       else    {  REG=0 ;  }
		    break ;
	  case S_key_Home: p=0 ; break ;
	  case S_key_End:
		   for( p=0 ; str[p]!=0 && p<lenght ; p++ ) ;
		   for( p=p-1; str[p-1]==' ' ; p-- ) ;
		   /*if( str[p]==0 ){ str[p]=' '; str[p+1]=0 ; }*/
		   break ;
	  case S_key_F1:
		   if( (EIN&0x02)==0 )
		       return WM_HELP ;
		   else
		   {   short i ;
		       i=kommand_yxf1( y,x,str1,n_str1 );
		       if( 0<=i )
		       {  strcpy( str,str1[i] );
			  L=strlen( str );
		       }
		       break ;
		   }
	  case S_key_F10:
		  strcpy( str,alt_str ) ;
		  return WM_ESC ;
	 default: ;
       }
   }
   else
   {  switch( key1 )
      {  case  8: if(p>0)
    /* Zuruck  */ {  for( i=(--p) ; i<L && str[i]!=0 ; i++ )
		       str[i]=str[i+1];
		     L-- ;
		  }  break ;
	 case '\r': goto AUSGANG ;
	 case S_key_Esc:
		  strcpy( str,alt_str ) ;
		  return WM_ESC ;
	 default: if( REG==1 )
		  {  if( L<lenght )
		     {   for( i=L ; i>=p ; i-- ) str[i+1]=str[i] ;
			 str[p]=(char )key1 ; p++ ; L++ ;
		     }else
		     if( p<lenght )
		     {   for( i=lenght-2 ; i>=p ; i-- ) str[i+1]=str[i] ;
			 str[p]=(char )key1 ; p++ ;
		     }
		  }
		  if( REG==0 && p<lenght )
		  {   str[p]=(char )key1 ;
		      if( p>=L ){ str[p+1]=0 ; L=p+1 ; }
		      p++ ;
		  }
       }
   }
}
AUSGANG:
return WM_OK ;
}
/***************************************************************************/
//             выбор строки из массива предыдущих строк
/***************************************************************************/
short kommand_yxf1( short y, short x, char **str, short n_str )
{  short i,PY=0 ;
   short key1,key2 ;
   char  c ;

   s_save(    y,x,y+n_str+1,x+40 ) ;
   s_rame1_F( y,x,y+n_str+1,x+40,0x70 ) ;

   while( 1 )
   {
      for( i=0 ; i<n_str ; i++ )
      {   if( i==PY ) c=v_curs_Color ;
	      else    c=v_menu_Color ;
	  s_text_yxf( y+i+1,x+1,c,emptystr(38) ) ;
	  s_text_yxf( y+i+1,x+1,c,str[i] ) ;
      }
      s_getch( &key1,&key2 );
      if( key1==0 )
      {   switch( key2 )
	  {   case S_key_Up   :
		   PY-- ; if( PY<0  ) PY=n_str-1 ;
		   break ;
	      case S_key_Down :
		   PY++ ; if( n_str<PY ) PY=0 ;
		   break ;
	  }
      }
      else
      {   switch( key1 )
	  {
	     case '\r':      goto Ende ;
	     case S_key_Esc: PY=-1 ; goto Ende ;
	  }
      }
   }
   Ende:
   s_restore( );
   return PY ;
}
/************************************************************************/
//       напечатать список горячих клавиш (в формате "[F1]Help" )
/************************************************************************/
void paint_hotkey( short y, short x, char *Str )
{  short i,j,f,sx,sy ;
   uchar t[2] ={ 0xb0, 0x3f };
   static char Str1[200] ;
   static char Color[200] ;

   for( i=j=0,f='0' ; Str[i]!=0 && i<200 ; i++ )
   {  if( Str[i]=='[' ) {  f='1' ; continue ;  }
      if( Str[i]==']' ) {  f='0' ; continue ;  }
      Str1[j] =Str[i] ;
      Color[j]=f ;
      j++ ;
   }
   s_get_size( sx,sy );
   for( i=j ; i<sx && i<200 ; i++ )
   {  Str1[i]=' ' ; Color[i]='0' ; }
   Str1[i]=0 ; Color[i]=0 ;
   s_text_yx( y,x,Str1 );
   s_color_yxt( y,x,t,Color );
   
}


