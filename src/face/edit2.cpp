/***************************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                Всяко-разные отображалки
//
//  t_Edit2       отображение первичного массива слов
//  t_ViewAntwort отображение вариантов перевода
//  t_Edit3       отображение структуры фразы
//  t_EditPart    редактор частей речи
//  t_EditForm    редактор грамматических форм
//  t_EditForm1   редактор грамматических форм (вспомогательный)
//  t_SlowoView   отображение массива словарей
//  t_SlowoView1  отображение грамматических форм, присутствующих в словаре
//                                              
//     Анисимов Д.В.                                         сегодня
/***************************************************************************/

# include <string.h>

# include <mylib.h>
# include <slowo2.h>
# include <video.h>
# include <color.h>
# include <face.h>
# include "core.h"
# include "edit2.h"
# include "debug.h"
# include "path.h"
# include "word.h"

extern t_Debug     Debug ;
extern t_Path      Path ;
extern t_AddWord   AddWord ;
extern t_Slowo2   *SlowoT ;
extern short      *i_FormatT ;

char *name_struct( e_Type Type, char Half, short i_struct );
void sprint_param( char *Str, short Length, 
                   short i_struct, t_Form &Form, char Halb);

/***************************************************************************/
//                       абстрактный класс списка                          //
/***************************************************************************/
t_SelectWin :: t_SelectWin( void ) : t_Win( )
{
   restart();
   f_slave=0 ;
}
/***************************************************************************/
t_SelectWin :: ~t_SelectWin( void )
{ ;  }
/***************************************************************************/
void t_SelectWin :: restart( void )
{
   Begin   = 0 ;
   Begin_X = 0 ;
   PY      = 0 ;
}
/***************************************************************************/
void t_SelectWin :: begin( void ){ ; }
void t_SelectWin :: end  ( void ){ ; }
/***************************************************************************/
e_WinMsg t_SelectWin :: loop( void )
{
   short sx,sy ;
   short ZZ,key1,key2 ;
   static short sx1,sy1 ;

   s_get_size( sx1,sy1 );
   begin();
   while( 1 )
   {
      s_get_size( sx,sy );
      if( sx!=sx1 || sy!=sy1 )
      {  sx1=sx ; sy1=sy ;  return WM_RESIZE ;  }

      paint( );

      s_getch( &key1,&key2 ) ;
      ZZ=s_shiftstatus();
      if( 1==hot_key( ZZ,key1,key2 ) ) continue ;

      if( 0!=(ZZ&(S_Shift_L|S_Shift_R)) )
      {  switch( key2 )
	 {  // -------------- переход в другие окна ------------
	    case S_key_F2 : end(); return WM_FIRST ;
	    case S_key_F3 :        return WM_SRC ;
	    case S_key_F4 :        return WM_DST ;
	    case S_key_F5 : end(); return WM_DEBUG ;
      //    case S_key_F6 : end(); return WM_PATH ;
	 }
      }

      switch( key1 )
      {  case S_key_Back : 
              if( f_slave==0 ){ end(); return WM_ESC ;  } 
         case S_key_Esc : end(); return WM_ESC ;
	 case 0:
	 {   switch( key2 )
	     {
		case S_key_Up :
		     if( PY>0 ) PY-- ;
		     if( PY<Begin ) Begin=PY ;
		     break ;
		case S_key_Down :
		     if( PY<MaxY-1 ) PY++ ;
		     if( size_y<PY-Begin ) Begin=PY-size_y ;
		     break ;
		case S_key_Left : if( 0<Begin_X ) Begin_X-- ; break ;
		case S_key_Right : if( Begin_X<80 ) Begin_X++ ; break ;
		case S_key_Home : PY=0 ; break ;
		case S_key_End  : PY=MaxY-1 ; break ;
		case S_key_PgUp :
		     PY=max(0,PY-=size_y-1 );
		     if( size_y<PY-Begin ) Begin=PY-size_y ;
		     break ;
		case S_key_PgDn :
		     PY=min( MaxY,PY+=size_y-1 );
		     if( size_y<PY-Begin ) Begin=PY-size_y ;
		     break ;
		case S_key_F10 : end(); return WM_ESC ;
	     }
	 }
      }
   }
}
/***************************************************************************/
void t_SelectWin :: paint( void )
{  short i ;

   t_Win::paint();
   s_rame2_F( y1-1, x1-1, y2+1, x2+1, 0x0f );
   s_text_yxf( y1-1,x1+3 ,name_Color, Name );
   paint_begin( );
   for( i=0 ; i<=size_y ; i++ )
      paint1( i, Begin+i==PY );
}
/***************************************************************************/
void t_SelectWin :: paint_begin( void )
{ ; }
/***************************************************************************/
void t_SelectWin :: paint1( short i, char current )
{ ; }
/***************************************************************************/
char t_SelectWin :: hot_key( short SS, short key1, short key2 )
{  return 0 ;  }
/***************************************************************************/
//                   отображение первичного массива слов                   //
/***************************************************************************/
t_Edit2 :: t_Edit2( void ) : t_Win( )
{
   restart();
}
/***************************************************************************/
void t_Edit2 :: restart( void )
{
   Begin   = 0 ;
   Begin_X = 0 ;
   PY      = 0 ;
   Reg     = 0 ;
}
/***************************************************************************/
void t_Edit2 :: paint( void )
{  short i ;

   t_Win::paint();
   s_rame2_F( y1-1, x1-1, y2+1, x2+1, 0x0f );
   s_text_yxf( y1-1,x1+3 ,name_Color, Name );

   if( Reg==0 )
   {  for( i=0 ; i<=size_y ; i++ )
	 paint1( i, Begin+i==PY );
   }
   else
   {  short p1,p2 ;
      p1=min( pos1,PY );
      p2=max( pos1,PY );   
      for( i=0 ; i<=size_y ; i++ )
         paint1( i, p1<=Begin+i && Begin+i<=p2 );
   }
   paint_hotkey( n_Line-1, 0, hk_First ); 
}
/***************************************************************************/
void t_Edit2 :: paint1( short i, char current )
{  short p1,p2 ;
   short i_struct ;
   char  *Str,*Str1 ;
   char  c ;
   t_ItemList &From =Core.from( );

   MaxY = From.j ;
   if( MaxY<=Begin+i ) return ;
   p1=size_x/4 ;
   p2=size_x/2 ;
   if( current ) c=0x70 ; else c=0x07 ;

   Str=emptystr( size_x-2 );
   s_text_yxf( y1+i,x1+1 ,c, Str );
   Str=From[Begin+i].Str ;
   s_text_yxf( y1+i,x1+1 ,c, Str );
   if( From[Begin+i].exist==0 )
	    i_struct=-1 ;
       else i_struct=Core.struct_of_word( Begin+i );
   //if( 1<Core.From[Begin+i].Tag.j )
   //    s_text_yxf( y1+i,x1+p2-2,c,"*" );
   Str1=Perevod.translate_word_s( Str, i_struct );
   if( i_struct<0 )
   {   // ------- слово не перевелось ---------
       if( Begin+i==PY ) c=0x7c ; else c=0x0c ;
       s_text_yxf( y1+i,x1+p1,c,Str1 );
       s_text_yxf( y1+i,x1+p2,c,"?не знаю?" );
   }
   else
   {   s_text_yxf( y1+i,x1+p1,c,Str1 );
       s_text_yxf( y1+i,x1+p2,c,Grammar[i_struct].From.Name );
   }
}
/***************************************************************************/
e_WinMsg t_Edit2 :: loop( void )
{
   short sx,sy ;
   short ZZ,key1,key2 ;
   static short sx1=80,sy1=25 ;

   while( 1 )
   {
      s_get_size( sx,sy );
      if( sx!=sx1 || sy!=sy1 )
      {  sx1=sx ; sy1=sy ;  return WM_RESIZE ;  }

      paint( );

      s_getch( &key1,&key2 ) ;
      ZZ=s_shiftstatus();
      if( 1==hot_key( ZZ,key1,key2 ) ) continue ;

      //if( 0!=(ZZ&(S_Shift_L|S_Shift_R)) )
      {  switch( key2 )
	 {  // -------------- переход в другие окна ------------
	    case S_key_F2 : /* end(); */ return WM_FIRST ;
	    case S_key_F3 :        return WM_SRC ;
	    case S_key_F4 :        return WM_DST ;
	    case S_key_F5 : /* end(); */ return WM_DEBUG ;
            case S_key_F6 : /* end(); */ return WM_PATH ;
	 }
      }

      switch( key1 )
      {  case S_key_Esc : /* end(); */ return WM_ESC ;
         case ' ' : Reg=!Reg ; pos1=PY ; break ;
	 case 0:
	 {   switch( key2 )
	     {
	        case S_key_Ins :
                {   short i,p1,p2 ;   
		    char  Str[200]="" ;

                    if( Reg==0 ) pos1=PY ;
	            p1=min( pos1,PY );
		    p2=max( pos1,PY );
		    t_ItemList &From =Core.from( );
		    for( i=p1 ; i<=p2 ; i++ )
		    {  strcat( Str,From[Begin+i].Str );
		       if( i<p2 ) strcat( Str," " );
		    }
		    AddWord.make_str( Str );
		    AddWord.loop( );
		    break ;
	        }
		case S_key_Up :
		     if( PY>0 ) PY-- ;
		     if( PY<Begin ) Begin=PY ;
		     break ;
		case S_key_Down :
		     if( PY<MaxY-1 ) PY++ ;
		     if( size_y<PY-Begin ) Begin=PY-size_y ;
		     break ;
		case S_key_Left : if( 0<Begin_X ) Begin_X-- ; break ;
		case S_key_Right : if( Begin_X<80 ) Begin_X++ ; break ;
		case S_key_Home : PY=0 ; break ;
		case S_key_End  : PY=MaxY-1 ; break ;
		case S_key_PgUp :
		     PY=max(0,PY-=size_y-1 );
		     if( size_y<PY-Begin ) Begin=PY-size_y ;
		     break ;
		case S_key_PgDn :
		     PY=min( MaxY,PY+=size_y-1 );
		     if( size_y<PY-Begin ) Begin=PY-size_y ;
		     break ;
		case S_key_F10 : /* end(); */ return WM_ESC ;
	     }
	 }
      }
   }
}
/***************************************************************************/
char t_Edit2 :: hot_key( short SS, short key1, short key2 )
{
   switch( key1 )
   {  case S_key_Enter :
	  {  t_EditPart EditPart( PY );
	     s_save( y1, x1, y2, x2 );
	     EditPart.f_slave=1 ;
	     EditPart.set_name("Выбор части речи");
	     EditPart.set_pos( x1, y1, x2, y2 );
	     EditPart.loop( );
	     s_restore( );
	  }
	  break ;
      case 0 :
          if( key2==S_key_F1 ) 
	      Window_help( (void *)"first.html") ;
          return 0 ;
      default  : return 0 ;
   }
   return 1 ;
}
/***************************************************************************/
//                     Отображение структуры фразы                         //
/***************************************************************************/
t_Edit3 :: t_Edit3( char _Halb ) : t_SelectWin( )
{
   Halb=_Halb ;
   Reg=0 ;
   max_level=0 ;
}
/***************************************************************************/
void t_Edit3 :: begin( void )
{
   long        i ;  
   t_rWord     DC ;
   t_Variants *VV ;
   t_Form      Form0 ;

   if( 0==Core.fn_word() ) return ;

   Vision1.j=0 ;
   
   DC=Debug.Current ;
   if( Core.fn_word()<=DC.i_word ) return ;
   VV=Core.variants( DC.i_word,DC.i_struct );
   if( VV->Variant.j<=DC.i_variant ) return ;
   Form0=VV->Variant[DC.i_variant].Form ;

   Core.antwort().make( DC.i_word,DC.i_struct,DC.i_variant,Form0 ) ;
   pAnt=&Core.antwort() ;
   
   if( Halb==0 )  make_vision( &Vision1, &pAnt->tree1(),0 );
       else       make_vision( &Vision1, &pAnt->tree2(),0 );
   for( max_level=i=0 ; i<Vision1.j ; i++ )
      if( max_level<Vision1.list[i].level )
	  max_level=Vision1.list[i].level ;
}
/***************************************************************************/
void t_Edit3 :: end( void )
{  short i ;
   i=Debug.stack[0].i_variant ;
   if( i<0 || Core.n_antwort()<=i ) i=0 ;
   Core.antwort().del() ;
}
/***************************************************************************/
void t_Edit3 :: paint_begin( void )
{
   if( Halb==0 ) paint_hotkey( n_Line-1, 0, hk_Sou );
       else      paint_hotkey( n_Line-1, 0, hk_Dst );

}
/***************************************************************************/
void t_Edit3 :: make_vision( t_VisionList *Vision, t_TreeList *Tree, long i_tree )
{  long i ;
   t_Tree   *T ;
   t_Vision  V ;
   static short sp=0 ;

   if( i_tree<0 || Tree->j<=i_tree ) return ;

   if( i_tree==0 ) {  Vision->j=0 ; sp=0 ; }

   T=&Tree->list[i_tree] ;

   sp++ ;
   V.i_tree=i_tree ;
   V.level =sp ;
   Vision->add( V );
   
   if( IF_SELECT( T->type ) )
       make_vision( Vision, Tree, T->down );
   if( IF_STRUCT( T->type ) || IF_UNORDER( T->type ) )
       for( i=0 ; i<T->n_down ; i++ )
	  make_vision( Vision, Tree, T->down+i );
   sp-- ;
}
/***************************************************************************/
void t_Edit3 :: paint1( short i, char current )
{
   short  i1,i_tree,z,L ;
   char   c,*s1,*s2,Str[200],Str1[100] ;
   t_Tree       *T ;
   t_Struct     *S ;
   t_TreeList   *Tree ;
   t_VisionList *Vision=&Vision1 ;

   short p1,p2 ;
   p1=y1+size_y/2 ; p2=x1+size_x/2 ;

   if( 0==Core.fn_word() || Core.variants( 0,Grammar.i_all() )->Variant.j==0 )
   {  // ----------- Синтаксический разбор не произведен ----------
      s_rame2_F( p1-1,p2-19,p1+1,p2+19,0x70 );
      s_text_yxf( p1,p2-18 ,0x70, "Синтаксический разбор не произведен" );
      return ;
   }

   strcpy( Str,emptystr(199) );
   if( Halb==0 )
            Tree  =&pAnt->tree1() ;
       else Tree  =&pAnt->tree2() ;

   MaxY  =Vision->j ;
   if( current ) c=0x70 ; else c=0x07 ;

   // --------- начепятать имя структуры --------------
   if( MaxY<=Begin+i || Vision->j<=Begin+i ) return ;
   i_tree=Vision->list[Begin+i].i_tree ;
   T=Tree->list + i_tree ;

   s1=name_struct( T->type,Halb,T->i_struct );
   strncpy( Str+3*Vision->list[Begin+i].level,s1,strlen(s1) );

   // --------- напечатать само слово ----------------
   if( T->type==TWORD || T->type==TCONST || T->type==TCONST1 )
   {   // в случае с TCONST1 - неряшливость
       if( Halb==1 ) s2=pAnt->tree2()[i_tree].Str ;
	   else      s2=Core.from()[pAnt->tree1()[i_tree].i_word].Str ;
       strncpy( Str+3*max_level+20,s2,strlen(s2) );
   }
   // --------- начепятать параметры ------------------

   sprint_param( Str1, 100, T->i_struct, T->Form, Halb );
   z=3*max_level+40 ;
   L=strlen( Str1 );
   L=min( L,200-z-1 );
   strncpy( Str+z,Str1,L );

   Str[Begin_X+size_x-2]=0 ;
   s_text_yxf( y1+i,x1+1,c, emptystr(size_x-1) );
   s_text_yxf( y1+i,x1+1,c,Str+Begin_X );

   // -------- опознать ситуацию, когда слова нет в грамматическом словаре ----
   if( Halb==1 && 0<=T->i_struct )
   {   if( T->type==TWORD )
       {  short xx,l ;
       
          strcpy( Str,pAnt->tree2()[i_tree].Str1 );
	  s2=Str ;
          l=strlen(s2);
	  if( Str[l-1]==' ' ) Str[l-1]=0 ;	  
	  i1=i_FormatT[T->i_struct] ;
	  if( 0<=i1 )
	  {   if( 0>SlowoT[i1].word_exist( Str ) )
	      {   xx=3*max_level+20-Begin_X ;
		  if( xx<0 ) 
		  {   
	              if( l<=-xx ) Str[0]=0 ;
			  else     s2=Str-xx ;
	              xx=0 ;  
		  }
        	  s_text_yxf( y1+i,x1+1+xx,0x0e,s2 );
              }
	  }
       }
   }
}
/***************************************************************************/
char t_Edit3 :: hot_key( short SS, short key1, short key2 )
{
   switch( key1 )
   {  case S_key_Enter :
	  {  t_EditForm EditForm( PY,Halb );
	     s_save( y1, x1, y2, x2 );
	     EditForm.set_name( "Форма слова" );
	     EditForm.set_pos( x1, y1, x2, y2 );
	     EditForm.loop( );
	     s_restore( );
	  }
	  break ;
      case 0 :
          if( key2==S_key_F1 )
	      Window_help( (void *)"src.html") ; 
          return 0 ;
      default  : return 0 ;
   }
   return 1 ;
}
/***************************************************************************/
//                         редактор частей речи                            //
/***************************************************************************/
t_EditPart :: t_EditPart( short i_word )
{  short  i,i1,j ;
   e_Type t ;
   t_Variants *VV ;

   for( i=j=0 ; i<Grammar.n_trans() ; i++ )
   {  t=Grammar[i].From.type ;
      if( t!=TWORD && t!=TSTRUCT1 && t!=TSTRUCT2 ) continue ;
      VV=Core.variants( i_word, i );
      for( i1=0 ; i1<VV->Variant.j ; i1++ )
      {  if( 99<=j ) break ;
         Tag[j].i_struct=i ;
         Tag[j].i_slowo =VV->Variant[i1].i_slowo ;
	 Tag[j].i_slowo1=VV->Variant[i1].i_slowo1 ;
         Tag[j].Form    =VV->Variant[i1].Form ;
	 j++ ;
      }
   }

   Word= Core.from()[i_word].Str ;
   MaxY=j ;
   for( i=0 ; i<MaxY ; i++ )
      present[i]=0 ;
}
/* ----------------------------------------------------------------------- */
void t_EditPart :: paint1( short i, char current )
{  short  i1,z ;
   short  i_struct ;
   char   c,*s1,*Str1,Str[200] ;
   t_Struct  *S ;

   if( current ) c=0x70 ; else c=0x07 ;

   s1=emptystr( size_x );
   s_text_yxf( y1+i,x1+1 ,c, s1 );

   // --------- начепятать имя структуры --------------
   if( MaxY<=Begin+i ) return ;
   strcpy( Str,emptystr(size_x-1) );

   i_struct=Tag[Begin+i].i_struct ;
   S=&Grammar[i_struct].From ;
   s1=S->Name ;
   strncpy( Str+3,s1,strlen(s1) );
   if( present[Begin+i]==1 ) Str[1]='*' ; else Str[1]=' ' ;

   //Str1=Perevod.translate_word_s( Word, i_struct );
   Str1=Perevod.translate_word_i( Word, Tag[Begin+i].i_slowo,Tag[Begin+i].i_slowo1 );
   strncpy( Str+20,Str1,strlen(Str1) );

   // --------- начепятать параметры ------------------
   for( i1=0,z=40 ; i1<S->Param.j ; i1++ )
   {  if( Tag[Begin+i].Form.value[i1]<0 ) s1="@0" ; //НеЗадан
      else
      {   short i2,i3 ;
	  i2 = S->Param[i1].param ;
	  i3 = Tag[Begin+i].Form.value[i1] ;
	  s1 = Grammar.from().Param[i2].Value[i3].Name ;
      }
      strncpy( Str+z,s1,strlen(s1) );
      z+=strlen(s1)+1 ;
   }
   Str[size_x]=0 ;
   s_text_yxf( y1+i,x1+1,c,Str );
}
/***************************************************************************/
char t_EditPart :: hot_key( short SS, short key1, short key2 )
{  
   if( key1==0 && key2==S_key_F1 ) 
       Window_help( (void *)"first.html") ; 
   return 0 ;
}
/***************************************************************************/
/*                     редактор грамматических форм                        */
/***************************************************************************/
t_EditForm :: t_EditForm( short i_tree, char Halb )
{  t_Tree       *T ;
   t_TreeList   *Tree ;

   if( Halb==0 )
   {  Tree  =&Core.antwort().tree1() ;
      T     =&Tree->list[i_tree] ;
      SParam=&Grammar[T->i_struct].From.Param ;
      LParam=&Grammar.from().Param ;
   }
   else
   {  Tree  =&Core.antwort().tree2() ;
      T     =&Tree->list[i_tree] ;
      SParam=&Grammar[T->i_struct].To.Param ;
      LParam=&Grammar.to().Param ;
   }
   MaxY=SParam->j ;
   value=T->Form.value ;
}
/***************************************************************************/
void t_EditForm :: paint1( short i, char current )
{  char c,*s1,*s2,*s3,Str[200] ;

   if( current ) c=0x70 ; else c=0x07 ;

   s1=emptystr( size_x );
   s_text_yxf( y1+i,x1+1 ,c, s1 );
   if( MaxY<=Begin+i ) return ;

   t_Param *P1=&LParam->list[SParam->list[i].param] ;
   s2=P1->Name ;
   if( value[i]<0 ) s3="@0" ;
       else         s3=P1->Value[value[i]].Name ;
   sprintf( Str,"%10s %10s",s2,s3 );
   s_text_yxf( y1+i,x1+1 ,c, Str );
}
/***************************************************************************/
char t_EditForm :: hot_key( short SS, short key1, short key2 )
{
   switch( key1 )
   {
      case S_key_Enter :
	  {  t_Param *P1=&LParam->list[SParam->list[PY].param] ;
	     t_EditForm1 EditForm1( P1,&value[PY] );
			s_save( y1, x1, y2, x2 );
			s_rame2_F( y1, x1, y2, x2, 0x0f );
			s_text_yxf( y1, x1+3, name_Color, "Задание параметра" );
			EditForm1.set_pos( x1, y1, x2, y2 );
			EditForm1.loop( );
			s_restore( );
	  }

	  break ;
      default  : return 0 ;
   }
   return 1 ;
}
/***************************************************************************/
//              редактор грамматических форм (вспомогательный)             //
/***************************************************************************/
t_EditForm1 :: t_EditForm1( t_Param *_Param, char *_Value )
{  Param = _Param ;
   Value = _Value ;
   PY = *Value ;
   MaxY=Param->Value.j ;
}
/***************************************************************************/
void t_EditForm1 :: paint1( short i, char current )
{  char c,*s1 ;
   if( current ) c=0x70 ; else c=0x07 ;

   s1=emptystr( size_x );
   s_text_yxf( y1+i,x1+1 ,c, s1 );
   if( MaxY<=Begin+i ) return ;
   s_text_yxf( y1+i,x1+3 ,c, Param->Value[Begin+i].Name );
}
/***************************************************************************/
char t_EditForm1 :: hot_key( short SS, short key1, short key2 )
{
   switch( key1 )
   {
      case S_key_Enter :
	   *Value=PY ;
	   break ;
      default  : return 0 ;
   }
   return 1 ;
}
/***************************************************************************/
//                      отображение массива словарей                       //
/***************************************************************************/
t_SlowoView :: t_SlowoView( void )
{
   n_From=Grammar.from().Format.j ;
   MaxY  =n_From+Grammar.to().Format.j ;
}
/***************************************************************************/
void t_SlowoView :: paint_begin( void )
{
   n_From=Grammar.from().Format.j ;
   MaxY  =n_From+Grammar.to().Format.j ;
   s_text_yxf( y1-1,x1+3 ,name_Color, "Грамматические словари" );
   paint_hotkey( n_Line-1, 0, hk_Dicts );
}
/***************************************************************************/
void t_SlowoView :: paint1( short i, char current )
{  char c,*s1,*s2,Str[100] ;
   short i_part ;

   if( current ) c=0x70 ; else c=0x07 ;

   s1=emptystr( size_x );
   s_text_yxf( y1+i,x1+1 ,c, s1 );
   if( MaxY<=Begin+i ) return ;
   if( i<n_From )
   {  
      s1    =Grammar.from().Format[i].Name ;
      i_part=Grammar.from().Format[i].i_part ;
      s2    =Grammar.from().Part[i_part].Name ;
      sprintf( Str,"Источник : %-10s %-20s",s1,s2 );
   }
   else
   {  strcpy( Str,"Приемник : " );
      s1    =Grammar.to().Format[i-n_From].Name ;
      i_part=Grammar.to().Format[i-n_From].i_part ;
      s2    =Grammar.to().Part[i_part].Name ;
      sprintf( Str,"Приемник : %-10s %-20s",s1,s2 );
   }
   Str[Begin_X+size_x]=0 ;
   s_text_yxf( y1+i,x1+3 ,c, Str );

   paint_hotkey( n_Line-1, 0, hk_Dicts );
}
/***************************************************************************/
char t_SlowoView :: hot_key( short SS, short key1, short key2 )
{
   switch( key1 )
   {
      case S_key_Enter :
	  {  t_Format  *F ;

	     if( PY<n_From ) F=&Grammar.from().Format[PY] ;
		 else        F=&Grammar.to().Format[PY-n_From] ;
	     t_SlowoView1 SV( PY );
	     s_save( y1, x1, y2, x2 );
	     s_rame2_F( y1, x1, y2, x2, 0x0f );
	     //s_text_yxf( y1, x1+3, name_Color,"Присутствующие формы" );
	     SV.set_pos( x1, y1+1, x2, y2 );
	     SV.loop( );
	     s_restore( );
	  }
	  break ;
      default  : return 0 ;
   }
   return 1 ;
}
/***************************************************************************/
//        отображение грамматических форм, присутствующих в словаре        //
/***************************************************************************/
t_SlowoView1 :: t_SlowoView1( short SS )
{
   short n_From=Grammar.from().Format.j ;

   if( SS<n_From )
   {   Format=&Grammar.from().Format[SS] ;
       Part  =&Grammar.from().Part[Format->i_part] ;
       Param =&Grammar.from().Param ;
   }
   else
   {   Format=&Grammar.to().Format[SS-n_From] ;
       Part  =&Grammar.to().Part[Format->i_part] ;
       Param =&Grammar.to().Param ;
   }
   MaxY=Format->Record.j ;
}
/***************************************************************************/
void t_SlowoView1 :: paint_begin( void )
{  char *s,Str[200]="",Str1[50] ;
   short i ;

   for( i=0 ; i<Part->Param.j ; i++ )
   {  if( Part->Param[i].Dir==1 ) continue ;
      s=Param->list[Part->Param[i].param].Name ;
      sprintf( Str1," %-10s",s );
      strcat( Str,Str1 );
   }
   Str[Begin_X+size_x-7]=0 ;
   s_text_yxf( y1-1,x1+7 ,0x0f, Str+Begin_X );

   sprintf( Str,"%s( %s )",Format->Name,Part->Name );
   s_text_yxf( y1-2,x1+2, name_Color,Str );  
}
/***************************************************************************/
void t_SlowoView1 :: paint1( short _i, char current )
{  char c,*s,Str[200]="",Str1[20] ;
   short    i,i1,v ;
   t_Param *P  ;

   if( current ) c=0x70 ; else c=0x07 ;
   
   s=emptystr( size_x );
   s_text_yxf( y1+_i,x1+1 ,c, s );
   if( MaxY<=Begin+_i ) return ;

   i=Begin+_i ;
   sprintf( Str," %-3d",i );

   for( i1=0 ; i1<Part->Param.j ; i1++ )
   {  if( Part->Param[i1].Dir==1 ) continue ;
      P=&Param->list[Part->Param[i1].param] ;
      v=Format->Record[i].value[i1] ;
      if( v<0 ) s="@0" ;
          else  s=P->Value[ v ].Name ;
      sprintf( Str1," %-10s",s );
      strcat( Str,Str1 );
   }
   Str[Begin_X+size_x-2]=0 ;
   s_text_yxf( y1+_i,x1+3 ,c, Str+Begin_X );
}
/************************************************************************/
//                     дать имя конструкции
/************************************************************************/
char *name_struct( e_Type type, char Half, short i_struct )
{  char *s1 ;

   if( type==TCONST )
      s1="@0";
   else
   if( type==TCONST1 )
      s1="@1";
   else   
   if( type==TWORD0 )
      s1="@00";
   else
   {  if( Half==0 ) s1=Grammar[i_struct].From.Name ;
	  else      s1=Grammar[i_struct].To.Name ;
   }
   return s1 ;
}
/************************************************************************/
//                 напечатать значения параметров
/************************************************************************/
void sprint_param( char *Str, short Length, 
                   short i_struct, t_Form &Form, char Halb)
{  short i1,i2,i3 ;
   char *s1 ;
   t_Param1List *Param ;

   Str[0]=0 ;
   if( i_struct<0 ) 
       return ;
  
   if( Halb==0 ) Param=&Grammar[i_struct].From.Param ;
       else      Param=&Grammar[i_struct].To.Param ;
  
   for( i1=0 ; i1<Param->j ; i1++ )
   {  if( Form.value[i1]<0 ) s1="@0" ; // НеЗадан
      else
      {   short i2,i3 ;
	  i2 = Param->list[i1].param ;
	  i3 = Form.value[i1] ;
	  if( Halb==0 )
	  {   if( i3<0 || Grammar.from().Param[i2].Value.j<=i3 )
	               s1="ошибка" ;
		  else s1=Grammar.from().Param[i2].Value[i3].Name ;
          }
	  else
	  {   if( i3<0 || Grammar.to().Param[i2].Value.j<=i3 )
	               s1="ошибка" ;
		  else s1=Grammar.to().Param[i2].Value[i3].Name ;
	  }
      }
      strcat( Str,s1 );
      strcat( Str," ");
   }
}

DEFINE_LIST_BTYPE( t_Vision, t_VisionList )
