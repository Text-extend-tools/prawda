/***************************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                      Редактор, заточенный под перевод 
//     Анисимов Д.В.                                         сегодня
/***************************************************************************/

# include <stdio.h>
# include <malloc.h>
# include <string.h>

# include "mylib.h"
# include "video.h"
# include "resource.h"
# include "menu.h"
# include "face.h"
# include "edit1.h"

extern t_Resource Resource ;

/***************************************************************************/
t_EditTrans :: t_EditTrans( void ) : t_Edit()
{
   Name[0]=0 ;
   Text=&Text1 ;
   begin();
}
/***************************************************************************/
t_EditTrans :: ~t_EditTrans( void )
{
   ;
}
/***************************************************************************/
void t_EditTrans :: set_name( char *S )
{  strcpy(Name,S) ;
}
/***************************************************************************/
void t_EditTrans :: write( char *_Name, char f_cp1251 )
{
   Text->write_file( _Name, f_cp1251 );
}
/***************************************************************************/
void t_EditTrans :: read( char *_Name, char f_cp1251 )
{
   Text->read_file( _Name, f_cp1251 );
   PX=0 ; PY=old_PY=0 ;
   Text->get_str( PY,Str );
}
/***************************************************************************/
e_WinMsg t_EditTrans :: main_loop( void )
{  
   short  ZZ,key1,key2,Funk ;    /* нажатая клавиша */
   e_WinMsg r ;

   null_menu( );
   while( 1 )
   {
      paint( );

      s_getch( &key1,&key2 ) ;
      ZZ=s_shiftstatus();

      if( key1==S_key_TabR ) return WM_NEXT ;

      if( (ZZ&(S_Shift_L|S_Shift_R))!=0 )
      {	  // -------- Shift включен ----------------
          if( 1==Text1.fformat )
	  {  // ------- режим препроцессора включен --------
	     if( key2==S_key_F2 ) return WM_SLOWO1 ;
             if( key2==S_key_F3 ) {   set_enum (  );  continue ;  }
	     if( key2==S_key_F4 ) {   set_enum1( "множ"    );  continue ;  }
	     if( key2==S_key_F5 ) {   set_enum1( "сущ"     );  continue ;  }
	     if( key2==S_key_F6 ) {   set_enum1( "прил"    );  continue ;  }
	     if( key2==S_key_F7 ) {   set_enum1( "глагол"  );  continue ;  }
	     if( key2==S_key_F8 ) {   set_enum1( "наречие" );  continue ;  }
	  }
	  else
	  {  switch( key2 )
	     {
		case S_key_F2 : return WM_FIRST ;
		case S_key_F3 : return WM_SRC ;
		case S_key_F4 : return WM_DST ;
		case S_key_F5 : return WM_DEBUG ;
	     }
	  }
      }
      else
      {  // -------- Shift отпущен ----------------
	 if( key1==0 )
	 {  switch( key2 )
	    {  case S_key_F1 : Window_help( (void *)"edit.html") ; continue ;
	       case S_key_F2 : return WM_SLOWO1 ;
	       case S_key_F5 : return WM_ADD_WORD ;
               case S_key_F6 : return WM_ADD_WORD1 ;
	       case S_key_F7 : Windows.trans( ); continue ;
	       case S_key_F8 : Windows.trans_frasa(); continue ;		  
	       case S_key_F9 : return WM_PATH2 ;
	    }
	 }	 
      }

      r=loop( ZZ,key1,key2 );
      if( r!=WM_NULL1 ) return r ;
      
      Funk=Resource.translate( ZZ,key1,key2 );
      switch( Funk )
      {   /*------------ обработка СТРЕЛОК -------------*/
	  case 0           : 
	  case F_Paste     :
	  case F_Cat       :
	  case F_IncCopy   :
	  case F_ShiftLeft :
	  case F_ShiftRight:
	  case F_Delete    : prepare_text( ); break ;
	  case F_PrevFrasa : Windows.prev_frasa() ; break ;
	  case F_NextFrasa : Windows.next_frasa() ; break ;
      }
   }
}
/***************************************************************************/
long  t_EditTrans :: current_pos( void )
{  return Text->pos_str(PY)+PX ;  }
/***************************************************************************/
char* t_EditTrans :: get_frasa( long i )
{  return Text->get_frasa( i );  }
/***************************************************************************/
char* t_EditTrans :: get_word( long i )
{  return Text->get_word_punkt( i );  }
/***************************************************************************/
void  t_EditTrans :: put_frasa( long p, char *Frasa )
{  long i,z ;
   char c ;
   char Strtmp[1000] ;

   for( i=z=0 ; Frasa[i]!=0 ; i++ )
   {  c=Frasa[i] ;
      if( 80<=(i-z) && (c==' ' || c=='\t') )
          Frasa[i]='\n' ;
      if( Frasa[i]=='\n' ) z=i ;
   }
   if( 990 <strlen(Frasa) )
       Frasa[990]=0 ;
   if( Text1.fformat==1 ) 
            sprintf( Strtmp,"\\@@ { %s }",Frasa );
       else strcpy( Strtmp,Frasa );
   
   Text->put_frasa( p,Strtmp );
   get_str();
}
/***************************************************************************/
//      обертка для "шагающих функций" - след/пред слово/фраза
/***************************************************************************/
void  t_EditTrans :: step( short flag )
{
   long p,p1 ;
   long py,px ;

   old_PY = PY ;

   p=current_pos();
   
   if( flag==0 ) p1=Text->next_frasa( p );
   if( flag==1 ) p1=Text->prev_frasa( p );
   if( flag==2 ) p1=Text->next_word( p );
   if( flag==3 ) p1=Text->prev_word( p );
   
   Text->get_xy_pos( p1,&py,&px );
   PY=py ; PX=px ;
   raden_Y( PY ) ;
   raden_X( PX ) ;
   raden_y3( );
}
/***************************************************************************/
void  t_EditTrans :: next_frasa( void ){  step(0) ;  }
void  t_EditTrans :: prev_frasa( void ){  step(1) ;  }
void  t_EditTrans :: next_word( void ) {  step(2) ;  }
void  t_EditTrans :: prev_word( void ) {  step(3) ;  }
/***************************************************************************/
void  t_EditTrans :: set_funk_const( char (*is_const)( char *Str ) )
{
   Text1.is_const=is_const ;
}
/***************************************************************************/
//         установить для перпроцессора "знаковую часть речи" \@= множ{}
/***************************************************************************/
void t_EditTrans :: set_enum( void )
{  short i ;

   if( Otm2<=Otm1 ) return ;
   for( i=255 ; Otm2<i ; i-- )
      Str[i]=Str[i-1] ;
   Str[Otm2]='}' ;
   for( i=255 ; Otm1<i ; i-- )
      Str[i]=Str[i-10] ;
   strncpy( Str+Otm1,"\\@= множ {",10 );
   Otm2=Otm1-1 ;
}
/***************************************************************************/
//         установить часть речи для перпроцессора например \@= прил{}
/***************************************************************************/
void t_EditTrans :: set_enum1( char *pStr )
{  short i,d,f,z1,z2,L ;
   char Str3[100] ;

   L=strlen(pStr);
   if( Otm2<=Otm1 )
   {  // ----Shift не нажат --------------------
      // ---- находимся ли мы внутри группы ----
      for( f=0,i=PX ; 0<=i ; i-- )
      {  if( Str[i]=='}' ) break ;
         if( Str[i]=='{' ) {  f=1 ; break ; }
	 if( Str[i]=='\\' && Str[i+1]=='@' && Str[i+2]=='=' )
	     {  f=1 ; break ; }
      }
      if( f==1 )
      {   // ------ мы внутри группы ----------
          for( i=PX ; 0<=i ; i-- )
             if( Str[i]=='\\' && Str[i+1]=='@' && Str[i+2]=='=' )
	     {   z1=i ; break ;  }
          for( i=z1 ; i<256 ; i++ )
	     if( Str[i]=='{' )
	     {   z2=i ; break ;  }          
	  d=L+5-(z2-z1) ;
	  if( 0<d )
	  {  for( i=255 ; z1+4+d<i ; i-- )
		Str[i]=Str[i-d] ;	  
	  }
	  else
	  {  for( i=z1+4 ; i-d<256 ; i++ )
		Str[i]=Str[i-d] ;	  
	  }
	  strncpy( Str+z1+4,pStr,L );
	  prepare_text( );
	  return ;
      }
      else
      {  // ------ мы вне группы ----------
         for( i=PX ; 0<=i ; i-- )
            if( Str[i]==' ' )
	    {   z1=i ; break ;  }
         for( i=z1+1 ; i<256 ; i++ )
	    if( Str[i]==' ' )
	    {   z2=i ; break ;  }

         Otm1=z1+1 ; Otm2=z2 ;
      }
   }
   // ----Shift нажат -----------------------
   for( i=255 ; Otm2<i ; i-- )
      Str[i]=Str[i-1] ;
   Str[Otm2]='}' ;
   for( i=255 ; Otm1<i ; i-- )
      Str[i]=Str[i-L-6] ;
   sprintf( Str3,"\\@= %s {",pStr );
   strncpy( Str+Otm1,Str3,strlen(Str3) );
   Otm2=Otm1-1 ;
   prepare_text( );
}

