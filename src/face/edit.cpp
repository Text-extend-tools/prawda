/***************************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                  Простенький такой текстовый редактор
//     Анисимов Д.В.                                         сегодня
/***************************************************************************/
# include <string.h>

# include "mylib.h"
# include "video.h"
# include "resource.h"
# include "color.h"
# include "face.h"
# include "edit.h"
# include "menu.h"

# define  size_Tab 8
# define  max_Length   1000000L
# define  max_Clip     100000L

t_KeyWord KeyWord1[]=
{ { "\\@=",3, "}" ,1, 0x0e },
  { "\\@@",3, ""  ,0, 0x0a },
  { "\\##",3, ""  ,0, 0x0a },
  { "{"   ,1, ""  ,0, 0x0a },
  { "}"   ,1, ""  ,0, 0x0a }
} ;

/* ------------------------ все, что относится к режиму поиска и замены -- */
class t_Find
{  public :
   char  sfind[256] ;   // строка, которую надо найти
   char  sreplace[256] ;// строка, на которую надо заменить
   char *xlat ;         // соответствие больших и маленьких букв
   char  Mass[2560] ;   // предыдущие поиски
   char *prev_find[10] ;// предыдущие поиски
   char  fxlat,
	 fcontinue,
	 fdirect,       // направление поиска
	 falles ;

	 t_Find( void );
   void  opt( void );
   void  flag( short punkt );
} ;

void  set_cursor( int Reg );
char *emptystr( short n );
char  str_recht( char *str );

static t_Find     Find ;
       t_Resource Resource ;

static char    *Clip=NULL ;  // клипборд, радактор все-таки
static long   l_Clip=0 ;     // текущая длинна клипборда
static char   f_Clip=0 ;     // признак, что в буфере больше одной строки

/***************************************************************************/
/*                                                                         */
/***************************************************************************/
void  t_Edit :: begin( void )
{
   Bold=0 ;
   x1=1 ; x2=78 ;
   y1=2 ; y2=n_Line-2 ;
   size_y=y2-y1 ;
   size_x=x2-x1 ;
   Begin_X=0 ;
   PX=0 ;
   PY=0 ;
   otm1=0 ; otm2=-1 ;
   Otm1=0 ; Otm2=-1 ;
   str1=0 ; str2=-1 ;
   Str1=0 ; Str2=-1 ;
   REG =0 ;
   Flag_b=0 ;

   if( Clip==NULL )
       Clip=(char *)Calloc( max_Clip,1 );
   strcpy( Str,emptystr(255) );
   KeyWord=NULL ;
   n_KeyWord=0 ;
   set_keyword( KeyWord1, 5 );
}
/***************************************************************************/
e_WinMsg t_Edit :: main( void )
{
   short  ZZ,key1,key2 ;    /* нажатая клавиша */
   e_WinMsg r ;

   null_menu( ) ;

   while( 1 )
   {
      paint( );

      s_getch( &key1,&key2 ) ;
      ZZ=s_shiftstatus();

      r=loop( ZZ,key1,key2 );
      if( r!=WM_NULL1 ) return r ;
   }  /* while */
}
/***************************************************************************/
e_WinMsg t_Edit :: loop( short ZZ, short key1, short key2 )
{
   short Funk,r ;
   old_PY = PY ;

   Funk=Resource.translate( ZZ,key1,key2 );

   {  short f ;
      f=hot_key( Funk ) ;
      if( 0==f || f==1 ) goto M1 ;
   }

   if( (ZZ & (S_Shift_L|S_Shift_R) ) != 0 )
   {  /* ---- если нажат Shift ---- */
      if( Flag_b==0 )
      {   Flag_b=1 ; str2=str1=PY ; otm2=otm1=PX ;	  
      }
   }

   switch( Funk )
   {
   case 0 :
	     switch( key1 )
	     {   case '\r': if( fview==1 ) 
	                    {   short i,j=0 ;
			        for( i=PX ; 0<=i && Str[i]!=2 ; i-- )
				   if( Str[i]==1 ) goto M_No_Link ;
				if( i<0 ) goto M_No_Link ;
				for( i++ ; i<256 && Str[i]!=1 ; i++ )
				   Buf[j++]=Str[i] ;
				Buf[j]=0 ;
	                        init( HelpData.get_help( Buf ) );
				Begin_Y=Begin_X=PY=old_PY=PX=0 ;
				return WM_NULL1 ;
                 M_No_Link :	break ;
	                    }
			    PX=key_WK( ) ; break ;
		 case S_key_Esc:
			    if( fview==1 ) return WM_ESC ;
			    prepare_text( ) ;
			    r=menu( ) ;
			    null_menu( ) ;
			    if( r!=0 ) return int_to_msg( r );
			    break ;
                 case  0 :  break ;
		 default :
			    if( fview==1 ) break ;
			    if( REG==0 ) insert_str1( PX,key1 ) ;
				else Str[PX]=(char)key1 ;
			    if(PX<255) PX++ ;
			    break ;
		 case S_key_Back:
			    if( fview==1 ) break ;
			    if( PX>0 ) delet_str1( --PX ) ;
			    break ;
	     } break ;
   case F_Menu:      if( fview==1 ) return WM_ESC ;
		     prepare_text( ) ;
		     r=menu( ) ;
		     null_menu( ) ;
		     if( r!=0 ) return int_to_msg( r );
		     break ;

   /*------------ обработка СТРЕЛОК -------------*/
   case F_MLeft  :   if(PX>0  ) {  PX--; otm2=PX;  } break ;
   case F_MRight :   if(PX<255) {  PX++; otm2=PX;  } break ;
   case F_MUp    :   if( PY>0 ) PY-- ; goto M_key1 ;
   case F_MDown  :   if( PY<Text->str_Txt-1 ) PY++ ;
          M_key1 :   str2=PY ; otm1=otm2=0 ;
		     break ;
   case F_MHome  :   PX=beg_str(); goto M_key2 ;
   case F_MEnd   :   PX=end_str() ;
	  M_key2 :   otm2=PX ;
		     break ;
   case F_MPageUp:   PY=max( PY-size_y,0 ) ; goto M_key3 ;
   case F_MPageDown: PY=min( PY+size_y,Text->str_Txt-1 ) ;
	    M_key3 : str2=PY ; otm1=otm2=0 ;
		     break ;
   case F_Paste    : if( fview==1 ) break ;
		     if( f_Clip )
		     {   Text->put_text1( PY, Clip,l_Clip );
		         Text->get_str( PY,Str );			 
		     }
		     else
		     {  insert_str( PX ) ; }
		     break ;
   case F_Cat      :
   case F_Copy     : if( fview==1 ) break ;
                     if( Str1<Str2 )
		     {
			if( Funk==F_Cat )
			     l_Clip=Text->get_text1( Str1,Str2, Clip );
			else l_Clip=Text->get_text2( Str1,Str2, Clip );
			f_Clip=1 ;
			str2=str1 ;
			PY=min( Str1,Text->str_Txt-1 );
			Text->get_str( PY,Str );
			old_PY = PY ;			
		     } 
		     else
		     {  if( Funk==F_Cat )
				 delet_str( Otm1,Otm2 ) ;
			    else delet_str( Otm1,Otm2,1 ) ;
			otm2=otm1 ; PX=Otm1 ;
			f_Clip=0 ;
		     } break ;
   case F_IncCopy :  if( fview==1 ) break ;
                     if( Str1<Str2 )
		     {  
			l_Clip=Text->get_text2( Str1,Str2, Clip );
			str2=str1 ;			
		     }
		     else
		     {  
			for( short i=Otm1 ; i<=Otm2 ; i++ )
			   Clip[l_Clip++]=Str[i] ;
			Clip[l_Clip++]='\n' ;
			otm2=otm1 ;
		     } break ;

   case F_BeginText : PY=0 ; break ;
   case F_EndText   : PY=Text->str_Txt-1 ; break ;
   case F_ShiftLeft : if( fview==1 ) break ;
                      str_shift(-1) ; break ;
   case F_ShiftRight: if( fview==1 ) break ;
                      str_shift( 1) ; break ;
   case F_Left      : if( PX>0   ) PX-- ; break ;
   case F_Right     : if( PX<255 ) PX++ ; break ;
   case F_Up        : if( PY>0   ) PY-- ; break ;
   case F_Down      : if( PY<Text->str_Txt-1 ) PY++ ; break ;
   case F_Delete    : if( fview==1 ) break ;
		      if( 0==char_exist( PX ) )
		          key_Delet( ) ;
		      else
			  delet_str1( PX ) ;
		      break ;
   case F_Insert    : if(REG==0){  REG=1 ;  }
			    else{  REG=0 ;  }
		      break ;
   case F_Home      : PX=beg_str() ; break ;
   case F_End       : PX=end_str(); break ;
   case F_PageUp    : PY=max( PY-size_y,0 ) ; break ;
   case F_PageDown  : PY=min( PY+size_y,Text->str_Txt-1 ) ; break ;

   }  /* switch(key1) */

   if( ZZ==0 )
   {  if( Flag_b==1 )
      {  Flag_b=0 ;
	 str2=str1 ; Str2=Str1-1 ;
	 otm2=otm1 ; Otm2=Otm1-1 ;
      }
   }

   if( (ZZ & (S_Shift_L|S_Shift_R) ) != 0 || Funk==F_Copy )
   {  Otm1=min(otm1,otm2) ; Otm2=max(otm1,otm2) ;
      Str1=min(str1,str2) ; Str2=max(str1,str2) ;
      goto M1 ;
   }

M1 :
   raden_Y( PY ) ;
   raden_X( PX ) ;
   return WM_NULL1 ;
}
/***************************************************************************/
//           нарисовать рамки и прочие причиндалы окна
/***************************************************************************/
void t_Edit :: paint( void )
{  char s1[10] ;

   if( Bold==1 ) s_rame2_F( y1-1,x1-1,y2+1,x2+1,0x0f );
       else      s_rame1_F( y1-1,x1-1,y2+1,x2+1,red_rame_Color );
   s_text_yxf( y1-1,x1+10,0x07,Name );

   if( Str1<Str2 )
   {   paint1( Str1,Str2 ) ; Otm1=0 ; Otm2=255 ;  }
   else
   {   paint1( 1,0 ) ;  }

   paint_current( Otm1,Otm2 ) ;
   sprintf( s1,"%4ld:%3d",PY,PX );
   s_text_yxf( n_Line-2,n_Col-10,0x70,s1 ) ;

   if( REG==0 ) s_text_yxf( n_Line-2,1,0x70," <Вставка> " ) ;
       else     s_text_yxf( n_Line-2,1,0x70," <Замена > " ) ;

   // ------- вот тут этот курсор ---------
   uchar t[2]={ 0x1f,0x1f };
   s_color_yxt( y1+PY-Begin_Y,x1+PX-Begin_X,t,"0" );
   s_goto_xy( y1+PY-Begin_Y,x1+PX-Begin_X );

   if( fview==1 )
            paint_hotkey( n_Line-1, 0, hk_Help );
       else paint_hotkey( n_Line-1, 0, hk_Edit ); 
}
/***************************************************************************/
//               нарисовать основную внутреннюю область
/***************************************************************************/
void t_Edit :: paint1( long _Otm1, long _Otm2 )
{   short i ;
    char  f,*Str3 ;

    if( fview==0 )
    {  uchar t[2]={0x0a,0xa0 };
       short i1 ;
       for( i=0 ; i<=size_y ; i++ )
       {   if( _Otm1<=Begin_Y+i && Begin_Y+i<=_Otm2 )
		    f=buf_Color ;
	       else f=text_Color ;
	   if( Begin_Y+i==PY )
		    Str3=Str ;
	       else Str3=Text->get_str( Begin_Y+i,Buf );
	   s_text_yxfl( y1+i,x1,f,x2-x1+1,Str3+Begin_X );
	   if( Text->fformat )
	       set_string_color( Str3, x1, y1+i );
       }
    }
    else
    {  short i1,xx1,z,begin,end ;

       for( i=0 ; i<=size_y ; i++ )
       {  Str3=Text->get_str( Begin_Y+i,Buf );
	  s_text_yxfl( y1+i,x1,help_Color,x2-x1+1,Str3+Begin_X );
          for( i1=0 ; i1<256 ; i1++ )
	  {  if( Str3[i1]==2 ) {  z=i1+1 ; f=link_Color ; Str3[i1]=' '; }
	     if( Str3[i1]==3 ) {  z=i1+1 ; f=bold_Color ; Str3[i1]=' '; }
	     if( Str3[i1]==1 )
	     {   Str3[i1]=' ';
	         if( z<Begin_X+x2-x1 && Begin_X<i1 )
	         {   begin=max( z-1 , Begin_X );
		     end  =min( i1+1, Begin_X+x2-x1 );
		     xx1=begin-Begin_X ;
		     s_text_yxfl( y1+i,x1+xx1,f,end-begin,Str3+begin );  
		 }	      
	     }	      
	  }
       }
    }
}
/***************************************************************************/
//                 нарисовать одну (текущую) строчку
/***************************************************************************/
void t_Edit :: paint_current( short _Otm1, short _Otm2 )
{  short  x11,L,i_Str ;
   uchar f ;

   if( fview==1 ) return ;

   i_Str=PY-Begin_Y ;
   if( i_Str<0 || y2-y1<i_Str )
       return ; // Возмутительная ошибка
   s_text_yxfl( y1+i_Str,x1,text_Color,x2-x1+1,Str+Begin_X );
   x11=x1+_Otm1-Begin_X ;
   L=min( _Otm2-_Otm1,x2-x11+1 );
   if( fview==1 ) f=help_Color ; 
       else       f=buf_Color ;
   s_text_yxfl( y1+i_Str,x11,f,L,Str+_Otm1 );
   if( Text->fformat )
       set_string_color( Str, x1, y1+i_Str );
}
/***************************************************************************/
//            выделить цветом директивы lang-препроцессора
/***************************************************************************/
void t_Edit :: set_string_color( char *_Str, short _x1, short y )
{  
   short i,i1,i2,L ;
   char StrC[256] ;
   uchar t[2] ;

   for( i=0 ; i<256 ; i++ )
      StrC[i]='0' ;
   for( i=Begin_X ; i<256 ; i++ )
   {  for( i1=0 ; i1<n_KeyWord ; i1++ )
      {  if( 0==strncmp( _Str+i,KeyWord[i1].Begin,KeyWord[i1].l_Begin ))
         {   t[0]=KeyWord[i1].f ;	     
	     if( KeyWord[i1].End[0]==0 ) 
	        L=KeyWord[i1].l_Begin ;
	     else
	     {  L=256-i ;
	        for( i2=i+1 ; i2<256 ; i2++ )
	           if( 0==strncmp( _Str+i2,KeyWord[i1].End,KeyWord[i1].l_End ))
		   {   L=i2-i ; break ;  }
	     }
	     StrC[L]=0 ;
	     s_foreground_yxt( y,_x1+i-Begin_X,t,StrC );
	     StrC[L]='0' ;
	 }      
      }   
   }
}
/***************************************************************************/
void t_Edit :: str_shift( char R )
{  long i,py ;
   char *S ;

   if( Str1<Begin_Y || Begin_Y+size_y<=Str2 ) return ;
   for( py=Str1 ; py<=Str2 ; py++ )
   {
      S=Text->get_str( py-Begin_Y,Buf );
      if( 0<R )
      {   for( i=255 ; 0<i ; i-- )
	     S[i]=S[i-1] ;
	  S[0]=' ';
      }
      else
      {   for( i=0 ; i<255 ; i++ )
	     S[i]=S[i+1] ;
	  S[255]=' ';
      }
      Text->put_str_istr( py,Buf );
   }
   Text->get_str( PY,Str );
}
/***************************************************************************/
void t_Edit :: delet_str1( short px )
{  short i ;
   for( i=px ; i<255 ; i++ )
       Str[i]=Str[i+1] ;
   Str[255]=' ';
}
/***************************************************************************/
void t_Edit :: insert_str1( short px, short key )
{  short i ;

   for( i=255 ; px<i ; i-- )
      Str[i]=Str[i-1] ;
   Str[px]=(char )key ;
}
/***************************************************************************/
short t_Edit :: beg_str( void )
{  short i ;
   for( i=0 ; i<255 ; i++ )
      if( Str[i]!=' ') return i ;
   return 0 ;
}
/***************************************************************************/
short t_Edit :: end_str( void )
{  short i ;
   for( i=255 ; 0<=i ; i-- )
      if( Str[i]!=' ') return i+1 ;
   return 0 ;
}
/***************************************************************************/
short t_Edit :: char_exist( short px )
{  short i ;
   for( i=px ; i<256 ; i++ )
      if( Str[i]!=' ' ) return 1 ;
   return 0 ;
}
/***************************************************************************/
/*                                                                         */
/***************************************************************************/
short t_Edit :: hot_key( short Taste )
{
    switch( Taste )
    {
       case F_FindForward  : find( 0 );  return 1 ;
       case F_FindBackward : find( 1 );  return 1 ;
       case F_Find         : find1( );   return 1 ;
       case F_GoToLine     : goto_str(); return 1 ;
       case F_Replace      : replace( );      return 1 ;
    }
    return -1 ;
}
/***************************************************************************/
e_WinMsg t_Edit :: int_to_msg( short i )
{
   switch( i )
   {  case '1' : return WM_EDIT1 ;
      case '2' : return WM_EDIT2 ;
      case '3' : return WM_FIRST ;
      case '4' : return WM_SRC ;
      case '5' : return WM_DST ;
      case '6' : return WM_DEBUG ;
      case 'b' : return WM_PATH ;
      case 'c' : return WM_PATH2 ;
      case 'd' : return WM_ADD_WORD ;
      case 'D' : return WM_ADD_WORD1 ;
      case '7' : return WM_GRAMMAR ;
      case '8' : return WM_SLOWO ;
      case '9' : return WM_BASE ;
      case 's' : return WM_SLOWO1 ;
      case 'e' : return WM_ERROR ;
   }
   return WM_NULL1 ;
}
/***************************************************************************/
void t_Edit :: prepare_text( void )
{
   Text->put_str_istr( PY,Str );
}
/***************************************************************************/
void  t_Edit :: get_str( void )
{
   Text->get_str( PY,Str );
}
/***************************************************************************/
//       между прочим, функция уничтожающая возмутительную ошибку
/***************************************************************************/
void t_Edit :: raden_y3(  )
{
   if( PY<Begin_Y ) Begin_Y=PY ;
   if( Begin_Y+(y2-y1)<PY+3 ) Begin_Y=PY+3-(y2-y1) ;
}
/***************************************************************************/
short t_Edit :: raden_Y( long _PY )
{
   if( _PY==old_PY ) return 0 ;
   Text->put_str_istr( old_PY,Str );
   Text->get_str( _PY,Str );
   if( _PY<Begin_Y ) Begin_Y=_PY ;
   if( Begin_Y+(y2-y1)<_PY ) Begin_Y=_PY-(y2-y1) ;
   
   return 1 ;
}
/***************************************************************************/

/***************************************************************************/
short t_Edit :: raden_X( short px )
{
   if( px<Begin_X )
   {   Begin_X=px ;
       return 1 ;
   }
   if( px>=Begin_X+size_x )
   {   Begin_X=max(0,px-size_x) ;
       return 1 ;
   }
   return 0 ;
}
/***************************************************************************/
short t_Edit :: key_WK( void )
{  short ret_x ;

   ret_x=beg_str() ;
   if( PX<ret_x ) ret_x=PX ;
   if( size_y<=PY-Begin_Y )
       Begin_Y++ ;

   Str[255]=0 ;
   Text->insert_str( PY );
   strcpy( Buf,emptystr(ret_x) );
   strncpy( Buf+ret_x,Str+PX,256-PX );
   Text->put_str_istr( PY+1, Buf );
   Str[PX]=0 ;
   Text->put_str_istr( PY, Str );
   PY++ ;
   Text->get_str( PY,Str );
   old_PY=PY ;
   return ret_x ;
}
/***************************************************************************/
void t_Edit :: key_Delet( void )
{  short i,px1 ;

   Text->get_str( PY+1,Buf );
   for( px1=0 ; px1<256 ; px1++  )
      if( Buf[px1]!=' ' ) break ;
   if( px1==256 ) px1=0 ;
   for( i=0 ; PX+i<256 ; i++  )
      Str[PX+i]=Buf[px1+i] ;
   Text->delete_str( PY+1 );
}
/***************************************************************************/
void t_Edit :: delet_str( short _Otm1, short _Otm2, char f )
{  short  i,j ;

   l_Clip=_Otm2-_Otm1 ;

   for( i=0 ; i<l_Clip ; i++ )
      Clip[i]=Str[_Otm1+i] ;

   if( f==0 )
   {  for( j=_Otm1,i=_Otm2 ; i<256 ; i++ )
	  Str[j++]=Str[i] ;
      for( ; j<256 ; j++ )
	  Str[j]=' ' ;
   }
   f_Clip=0 ;
}
/***************************************************************************/
void t_Edit :: insert_str( short px )
{  short i,L ;

   L=(short)l_Clip ;
   for( i=255 ; px+L<=i ; i-- )
      Str[i]=Str[i-L] ;
   for( i=0 ; i<L ; i++ )
      Str[px+i]=Clip[i] ;
}
/***************************************************************************/
void t_Edit :: goto_str( void )
{  short i ;
   char  Name1[40]="" ;

   s_save( 10,15,14,65 ) ;
   s_rame1_F( 10,15,14,65,h_menu_Color ) ;
   s_text_yxf( 11,20,h_menu_Color,"Введите номер строки" ) ;
   s_text_yxf( 12,20,h_curs_Color,emptystr( 40 ) ) ;
   if( WM_OK!=kommand_yxf( 12,20,h_curs_Color,Name1,40,0 ) ) return ;

   for( i=0 ; i<8 && Name1[i]!=0 ; i++ )
      if( Name1[i]<'0' || '9'<Name1[i] ) goto M_Err ;
   if( i==0 || 6<i ) goto M_Err ;
   PY=atoi( Name1 );
M_Err:
   s_restore( ) ;
}
/***************************************************************************/
char t_Edit :: find1( void )
{  char  *S1 ;

   S1=Find.sfind ;
   if( Otm1<Otm2 )
   {
       strncpy( S1,Str+Otm1,Otm2-Otm1 );
       S1[Otm2-Otm1]=0 ;
       Otm2=Otm1-1 ;
   }
   else
       S1[0]=0 ;
   Find.opt( );
   return find(0);
}
/***************************************************************************/
/*                             повторный поиск                             */
/***************************************************************************/
char t_Edit :: find( char Richt )
{  short   i,y,px,px1,py,size,L,n_Str ;
   char    *S ;
   t_Find  *f ;
   char     strtmp[256] ;

   f=&Find ;
   if( f->sfind[0]==0 ) Find.opt( );
   size = strlen( f->sfind ) ;
   if( size==0 ) return 0 ;
   n_Str=Text->str_Txt ;

   if( f->fxlat==1 )
   {  for( i=0 ; i<size ; i++ )
	 strtmp[i] = f->xlat[(uchar)(f->sfind[i])] ;
   }
   else strcpy( strtmp,f->sfind );

   for( y=0 ; y<n_Str ; y++ )
   {  if( Richt==0 ) py=PY+y ;
	 else        py=PY-y ;
      if( py>=n_Str )
	 if( f->fcontinue!=0 ) py-=n_Str ; else break ;
      if( py<0 )
	 if( f->fcontinue!=0 ) py+=n_Str ; else break ;

      S = Text->get_str( py ) ;
      for( i=0 ; i<256 ; i++ )
	 if( S[i]==0 || S[i]=='\n' ) break ;
      L=i ;
      if( y==0 ) px=PX+1 ; else px=0 ;
      for( ; px+size<L ; px++ )
      {
	 if( f->fxlat==1 )
	 {  for( i=0 ; i<size ; i++ )
	       if( f->xlat[(uchar)S[px+i]] != strtmp[i] ) goto M1 ;
	 }
	 else
	 {  for( i=0 ; i<size ; i++ )
	       if( S[px+i] != strtmp[i] ) goto M1 ;
	 }
	 for( i=0,px1=0 ; i<px ; i++ )
	    if( S[i]=='\t' ) px1+=size_Tab-1 ;
	 PY=py ; PX=px+px1  ;
	 return 1 ;
      M1: ;
      }
   }
   figna("Строка не найдена");
   return 0 ;
}
/***************************************************************************/
void t_Edit :: replace( void )
{  t_Find *f ;
   char    r ;
   short   taste,L ;

   f=&Find ;
   Find.opt( );
   if( f->sfind[0]==0 || f->sreplace[0]==0 ) return ;
   L=strlen( f->sfind );

   while( 1 )
   {  old_PY=PY ;
      r=find( f->fdirect );
      if( r==0 ) break ;
      raden_Y( PY );
      raden_X( PX );
      if( f->falles==0 )
      {
	 Otm1=PX ; Otm2=PX+L ;
	 paint( );
     M3: taste=fragen( "Менять ? (1-Да 0-Нет Esc-Выход)");
	 switch( taste )
	 {  case S_key_Esc: goto M1 ;
	    case   1: break ;
	    case   0: goto M2 ;
	    default : goto M3 ;
	 }
      }
      replace1( );
   M2: ;
   }
M1: old_PY=PY ;
    Otm1=0 ; Otm2=-1 ;
}
/***************************************************************************/
void t_Edit :: replace1( void )
{  char   *s,Buf1[256] ;
   short   i,l1,l2,d ;
   t_Find *f ;

   f=&Find ;
   s=Text->get_str( PY );
   for( i=0 ; i<256 ; i++ )
   {  if( s[i]==0 || s[i]=='\n' ) {  Buf1[i]=0 ; break ;  }
      Buf1[i]=s[i] ;
   }
   l1 = strlen( f->sfind );
   l2 = strlen( f->sreplace );
   d = l2-l1 ;
   if( d>0 )
      for( i=256-d-1 ; PX<=i ; i-- )
	 Buf1[i+d] = Buf1[i] ;
   for( i=0 ; i<l2 ; i++ )
      Buf1[PX+i] = f->sreplace[i] ;
   if( d<0 )
   {  for( i=PX+l2 ; i<256-l2 ; i++ )
	 Buf1[i] = Buf1[i-d] ;
      for( i=256-d-1 ; i<256 ; i++ )
	 Buf1[i] = ' ' ;
   }
   Text->put_str_istr( PY,Buf1 );
   Text->get_str( PY,Str );
}
/***************************************************************************/
void t_Edit :: set_text( t_Text *_Text )
{
   PX=0 ; PY=0 ;
   Text=_Text ;
   Text->get_str( PY,Str );
}
/***************************************************************************/
void t_Edit :: init( char *_Text )
{
   Text->init( _Text );
   PX=0 ; PY=0 ;
   Text->get_str( PY,Str );
}
/***************************************************************************/
void t_Edit :: set_view( char f )
{  fview=f ;  }
/***************************************************************************/
char t_Resource :: translate( short shift, short key1, short key2 )
{
   if( shift==0 )
   {
       switch( key1 )
       {  case 0:
	       switch( key2 )
	       {
		  case S_key_Ins  : return F_Insert ;
		  case S_key_Del  : return F_Delete ;
		  case S_key_Home : return F_Home ;
		  case S_key_End  : return F_End ;
		  case S_key_PgUp : return F_PageUp ;
		  case S_key_PgDn : return F_PageDown ;
		  case S_key_Left : return F_Left ;
		  case S_key_Right: return F_Right ;
		  case S_key_Up   : return F_Up ;
		  case S_key_Down : return F_Down ;
		  
		  case S_key_F3   : return F_FindForward ;
		  case S_key_F4   : return F_FindBackward ;
		  
		  case S_key_F10  : return F_Menu ;
	       }
	  break ;
       }
   }
   if( (shift&(S_Shift_L|S_Shift_R))!=0 )
   {
       switch( key1 )
       {  case 0:
	       switch( key2 )
	       {
		  case S_key_Ins  : return F_Paste ;
		  case S_key_Del  : return F_Cat ;
		  case S_key_Home : return F_MHome ;
		  case S_key_End  : return F_MEnd ;
		  case S_key_PgUp : return F_MPageUp ;
		  case S_key_PgDn : return F_MPageDown ;
		  case S_key_Left : return F_MLeft ;
		  case S_key_Right: return F_MRight ;
		  case S_key_Up   : return F_MUp ;
		  case S_key_Down : return F_MDown ;
	       }
	  break ;
       }
   }
   if( (shift&(S_Ctrl_L|S_Ctrl_R))!=0 )
   {
       switch( key1 )
       {  case 0:
	       switch( key2 )
	       {
		  case S_key_Left : return F_PrevFrasa ;
		  case S_key_Right: return F_NextFrasa ;
               }       
          break ;
       }
   }
   return F_Nop ;
}
/***************************************************************************/
t_Find :: t_Find( void )
{  short i ;
   char *x=xlat ;

   for( i=0 ; i<256 ; i++ )
   {  sfind[i]=0 ; sreplace[i]=0 ;  }
   for( i=0 ; i<2560 ; i++ )
      Mass[i]=0 ;
   for( i=0 ; i<10 ; i++ )
      prev_find[i]=Mass+i*256 ;

   xlat=(char *)SortChar() ;

   fxlat     = 0 ;
   fcontinue = 0 ;
   fdirect   = 0 ;
   falles    = 0 ;
}
/***************************************************************************/
void t_Find :: opt( void )
{  t_Find Alt ;
   short  x0,punkt=0,key1,key2 ;

   x0=(s_get_sx( )-56)/2 ;
   s_save( 4,x0,10,x0+56 ) ;
   s_rame1_F( 4,x0,10,x0+56,v_menu_Color ) ;

   s_text_yxf( 5,x0+1,v_menu_Color,"Найти      :" ) ;
   s_text_yxf( 6,x0+1,v_menu_Color,"Заменить на:" ) ;

   Alt=*this ;

while( 1 )
{
   x0=(s_get_sx( )-56)/2 ;
   {  short l1,l2 ;
      s_text_yxf1( 5,x0+12,v_menu_Color,sfind ) ;
      s_text_yxf1( 6,x0+12,v_menu_Color,sreplace ) ;
      l1=strlen(sfind); l2=strlen(sreplace);
      s_text_yxf( 5,x0+14+l1,v_menu_Color,emptystr(37-l1) ) ;
      s_text_yxf( 6,x0+14+l2,v_menu_Color,emptystr(37-l2) ) ;

   }
   flag( punkt ) ;

   key1=WM_NULL1 ;
   if( punkt==0 ) key1=kommand_yxf( 5,x0+12,v_curs_Color,sfind  ,39,3,prev_find,10 ) ;
   if( punkt==1 ) key1=kommand_yxf( 6,x0+12,v_curs_Color,sreplace,39,1 ) ;

   switch( key1 )
   {  case WM_OK     : {  short i ;
			  for( i=9 ; 0<i ; i-- )
			     strcpy( prev_find[i],prev_find[i-1] );
			  strcpy( prev_find[0],sfind );
			  s_restore( ) ;
			  if( !str_recht(sfind) ) *this=Alt ;
			  return ;
		       }
      case WM_ESC  :  key2=0 ; break ;
      case WM_NEXT :  key2=S_key_Down ; key1=0 ; break ;
      case WM_PREV :  key2=S_key_Up   ; key1=0 ; break ;
      case WM_NULL1 : s_getch( &key1,&key2 ) ;  break ;
   }

   switch( key1 )
   {   case 0 :
	   switch( key2 )
	   {   case S_key_Up   : if(punkt<=0) punkt=4 ; else punkt-- ; break ;
	       case S_key_Down : if(punkt>=4) punkt=0 ; else punkt++ ; break ;
	   } break ;
       case '\r' :
	   s_restore( ) ;
	   if( !str_recht(sfind) ) *this=Alt ;
	   return ;
       case S_key_Esc :
	   s_restore( ) ;
	   *this=Alt ;
	   return ;
       case ' ' :
	   switch( punkt )
	   {   case 2 : fxlat   = !fxlat ;   break ;
	       case 3 : falles  = !falles ;  break ;
	       case 4 : fdirect = !fdirect ; break ;
	   }
	   break ;
   }
}
}
/***************************************************************************/
void t_Find :: flag( short punkt )
{  char f ;
   short x0 ;

   x0=(s_get_sx( )-56)/2 ;
   if( punkt==2 ) f=v_curs_Color ; else f=v_menu_Color ;
   s_text_yxf( 7,x0+1,f,"[ ] Различать большие и малые" ) ;
   if( fxlat==0 )  s_text_yxf( 7,x0+2,f,"X" ) ;

   if( punkt==3 ) f=v_curs_Color ; else f=v_menu_Color ;
   if( falles!=0 )  s_text_yxf( 8,x0+1,f,"[        Заменить все        ]" ) ;
	   else     s_text_yxf( 8,x0+1,f,"[       Найти и спросить     ]" ) ;
   if( punkt==4 ) f=v_curs_Color ; else f=v_menu_Color ;
   if( fdirect==0 ) s_text_yxf( 9,x0+1,f,"[        Поиск вперед        ]" ) ;
	   else     s_text_yxf( 9,x0+1,f,"[        Поиск назад         ]" ) ;
}
/***************************************************************************/
char str_recht( char *str )
{  short i ;
   for( i=0 ; str[i]!=0 && i<80 ; i++ )
      if( str[i]!=' ' && str[i]!='\t' ) return 1 ;
   return 0 ;
}
/***************************************************************************/
void t_Edit :: find_forward( void )
{  Find.opt( );
   find( 0 );
}
/***************************************************************************/
void t_Edit :: find_backward( void )
{  Find.opt( );
   find( 1 );
}
/***************************************************************************/
t_Text * t_Edit :: text( void )
{
   return Text ;
}
/***************************************************************************/
void  t_Edit :: set_keyword( t_KeyWord *_KeyWord, long _n_KeyWord )
{  short i ;

   Free( KeyWord );
   KeyWord=(t_KeyWord *)Calloc( _n_KeyWord,sizeof(t_KeyWord) );
   for( i=0 ; i<_n_KeyWord ; i++ )
      KeyWord[i]=_KeyWord[i] ;
   n_KeyWord=_n_KeyWord ;
}
