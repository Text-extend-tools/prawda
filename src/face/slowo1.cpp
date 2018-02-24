/***************************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                    "Человеческий" словарь (морда)
//     Анисимов Д.В.                                         сегодня
/***************************************************************************/
# include <stdio.h>
# include <malloc.h>
# include <string.h>
# include <stdlib.h>
# include <dirent.h>
# include <sys/types.h>
# include <sys/stat.h>

# include <mylib.h>
# include <color.h>
# include <hotkey.h>
# include <video.h>
# include <slowo1.h>

void  Window_help( void *Void );

/***************************************************************************/

/***************************************************************************/
t_Slowo1 :: t_Slowo1( void )
{  short i ;

   fView=0 ;
   for( i=0 ; i<255 ; i++ )
      Sempty[i]=' ';
   Sempty[255]=0 ;
}
/***************************************************************************/
void t_Slowo1 :: set_pos( short _x1, short _y1, short _x2, short _y2, char flag )
{
   t_Win::set_pos( _x1,_y1,_x2,_y2 );
   fDirect=flag ;
   if( 0==flag )
   {  // -------- вертикальное расположение подокон ------
      rect1.x1=x1 ;
      rect1.x2=x2 ;
      rect1.y1=y1 ;
      rect1.y2=(y1+y2)/2 ;
      rect2.x1=x1 ;
      rect2.x2=x2 ;
      rect2.y1=rect1.y2+1 ;
      rect2.y2=y2 ;
   }
   else
   {  // -------- горизонтальное расположение подокон ------
      rect1.x1=x1 ;
      rect1.x2=x1+size_x/4 ;
      rect1.y1=y1 ;
      rect1.y2=y2+1 ;
      rect2.x1=rect1.x2+2 ;
      rect2.x2=x2 ;
      rect2.y1=y1 ;
      rect2.y2=y2 ;   
   }
}
/***************************************************************************/
//               реакция на кнопки когда выбирается слово
/***************************************************************************/
e_WinMsg t_Slowo1 :: main_loop( void )
{
   long  i ;
   short size1_y ;
   long  first_word ; // с какого слова начинается показ списка

   first_word=i_word ;
   Buch.read_paragraf( i_word );
   size1_y=rect1.y2-rect1.y1 ;

   while( 1 )
   {  short t1,t2,ZZ ;


      Buch.read_paragraf( first_word );      
      paint(  i_word, first_word );
      paint1( i_word, x, y );
      s_redraw( );
      s_goto_xy( rect1.y2-1,rect1.x1+px );
      s_getch( &t1,&t2 );
      ZZ=s_shiftstatus();

      switch( t1 )
      {
      case S_key_Back: case S_key_Del:
		       if( 0<px ) Str[--px]=0 ; break ;
      case '\r':       continue ;
//      case '\r':       paint( i_word, first_word );
//		       antwort( i_word ); Str[px=0]=0 ; break ;
      case S_key_Esc : Str[px=0]=0 ;
                       break ;
      default:         if( 40<=px ) break ;
		       Str[px++]=t1 ; Str[px]=0 ;
		       i=Buch.find( Str );
		       if( i<0 ) Str[--px]=0 ;
			   else  i_word=i ;
		       break ;
      case 0 :
         if( (ZZ&(S_Shift_L|S_Shift_R))==0 )
	 {  switch( t2 )
	    {
	       case S_key_Up:    if( 0<i_word ) i_word-- ; break ;
	       case S_key_Down:  if( i_word<Buch.n_sy_Word ) i_word++ ; break ;
	       case S_key_PgUp:  i_word-=size1_y-3 ; 
	                	 if( i_word<0 ) 
			             i_word=0 ; 
				 break ;
	       case S_key_PgDn:  i_word+=size1_y-3 ;
				 if( Buch.n_sy_Word<=i_word )
				     i_word=Buch.n_sy_Word-1 ;
				 break ;
               case S_key_F1 : 
		     Window_help( (void *)"slowo1.html") ;
		     break ;
	       case S_key_F10:   return WM_ESC ;
	    }
         }
	 if( (ZZ&(S_Shift_L|S_Shift_R))!=0 )
	 {
	    switch( t2 )
	    {  case S_key_PgUp : x=y=0 ; return WM_P_WORD ;
	       case S_key_PgDn : x=y=0 ; return WM_N_WORD ;
	       case S_key_Up:    y=max(     0,y-1); break ;
	       case S_key_Down:  y=min(size_y,y+1); break ;
	       case S_key_Left:  x=max(     0,x-1); break ;
	       case S_key_Right: x=min(size_x,x+1); break ;
	    }
	 }
	 break ;
      }
      if( i_word<first_word )          first_word=i_word ;
      if( first_word+size1_y-3<i_word) first_word=i_word-size1_y+3 ;      
   }
   return WM_ESC ;
}
# ifdef NIE
/***************************************************************************/
//        реакция на кнопки когда смотрится перевод выбранного слова       //
/***************************************************************************/
void t_Slowo1 :: antwort( long i_word1 )
{
   short x=0,y=0 ;
   short t1,t2 ;

   while( 1 )
   {
      paint1( i_word1,x,y );
      s_redraw( );
      s_goto_xy( rect1.y2-1,rect1.x1+px+1 );
      s_getch( &t1,&t2 );
      switch( t1 )
      {
      case S_key_Esc : case S_key_Enter :
	 paint1( -1,x,y ); return ;
      case 0 :
	 switch( t2 )
	 {
	 case S_key_Up:    y=max(     0,y-1); break ;
	 case S_key_Down:  y=min(size_y,y+1); break ;
	 case S_key_Left:  x=max(     0,x-1); break ;
	 case S_key_Right: x=min(size_x,x+1); break ;
	 case S_key_F10:   paint1( -1,x,y ); return ;
	 }
      }
   }
}
# endif
/***************************************************************************/
//                      отображение списка слов
/***************************************************************************/
void t_Slowo1 :: paint( long i_word1, long first_word1 )
{  long   i,i1,L ;
   long   first_word ;
   short  size1_y ;
   char  *Str1,f ;

   if( fDirect==0 )
   {
      t_Win::paint();
   }
   else
   {
      s_rame2_F ( y1-1,x1-1,y2+1,x2+1 ,0x0f );
      s_rame1_f ( y1-1,x1-1,y2+1,rect1.x2+1 ,0x0f );
      s_rame2_f ( y1-1,x1-1,y2+1,x2+1 ,0x0f );
      s_text_yxf( y1-1,x1+3 ,name_Color, Name );
   }
   if( 0<=first_word1 )
            first_word=first_word1 ;
       else first_word=Buch.i_Paragraf*16 ;

   size1_y=rect1.y2-rect1.y1 ;
   L=rect1.x2-rect1.x1 ;

   if( first_word<(i_word1-size1_y-3) ) 
       first_word=(i_word1-size1_y-3);

   for( i=0 ; i<size1_y ; i++ )
   {
       if( first_word+i==i_word1 )
                f=0xf0 ;
	   else f=0x0f ;

       Str1=Buch.wort( first_word+i );
       s_text_yxfl( rect1.y1+i,rect1.x1,f,L,Sempty );
       s_text_yxfl( rect1.y1+i,rect1.x1,f,L,Str1 );
   }

   s_rame2_F ( rect1.y2-2,rect1.x1-1,rect1.y2,rect1.x2+1,0x0f );
   s_text_yxf( rect1.y2-1,rect1.x1,0x0f,Str );
   paint_hotkey( n_Line-1, 0, hk_Slowo );
}
/***************************************************************************/
//                отобразить перевод выбранного слова                      //
/***************************************************************************/
void t_Slowo1 :: paint1( long i_word1, short x, short y )
{
   short i,j,j1,s,L ;
   short size2_x,size2_y ;
   char  *Ans,Str1[100] ;

   size2_x=rect2.x2-rect2.x1 ;
   size2_y=rect2.y2-rect2.y1 ;
   Sempty[size2_x]=0 ;
   for( i=0 ; i<size2_y ; i++ )
       s_text_yxf( rect2.y1+i+1,rect2.x1,0x0f,Sempty );
   Sempty[size2_x]=' ' ;
   if( i_word1<0 ) return ;

   Ans=Buch.answer( i_word1 );
   L=strlen( Ans );
   for( i=0,s=j=j1=0 ; i<L ; i++ )
   {  if( Ans[i]=='\n' || i==L-1 )
      {  Str1[j++]=0;
	 if( y<=s && s-y<size2_y )
	     s_text_yxf( rect2.y1+s-y,rect2.x1,0x0f,Str1 );
	 j=j1=0 ; s++ ;
      }
      else
      {  if( (j1++)<x || size_x-3<j ) continue ;
	 Str1[j++]=Ans[i] ;
      }
   }
}
/***************************************************************************/
//                 прочесть словарь из файла _Name
/***************************************************************************/
char t_Slowo1 :: read_buch( char *Name )
{  char r=0 ;
   if( 0!=strcmp( Buch.NameFile, Name ) )
   {   r=Buch.make( Name );
       Buch.set_divider( "=" );
       Buch.set_sort( 0 ) ;
       i_word=0 ;
   }
   return r ;
}
/***************************************************************************/
//                  установить переводимое слово
/***************************************************************************/
void t_Slowo1 :: set_str( char *_Str )
{  long i ;
   strncpy( Str,_Str,39 );
   Str[39]=0 ;
   px=strlen( Str );
   i=Buch.find( Str );
   if( 0<=i ) i_word=i ;
}
