/*******************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//             Последовательный выбор вариантов перевода
//     Анисимов Д.В.                               сегодня
/*******************************************************************/

# include <string.h>
# include <mylib.h>
# include <video.h>
# include <s_defkey.h>
# include <color.h>
# include <hotkey.h>
# include <core.h>
# include <slowo2.h>
# include <window.h>
# include <edit2.h>
# include <path.h>

extern t_Slowo2    *SlowoT ;
extern short       *i_FormatT ;
void  word_source( short i_struct, char *Word, char *Source, e_Half H );

void  Window_help( void *Void );
void  figna( char *str );

# define no_trans(r) (E_NULL==(r) || E_NO==(r) || E_FIGNA==(r) || E_ERROR==(r) )

/************************************************************************/
//      заменить в строке '\n' и '\t' на ' ' (для форматирования)
/************************************************************************/
void remove_wk( char *Str )
{
   for( long i=0 ; Str[i]!=0 ; i++ )
      if( Str[i]=='\n' || Str[i]=='\t' )
          Str[i]=' ' ;
}
/************************************************************************/
long t_pVariant :: word_to_aword( long i_word )
{  long i ;
   for( i=0 ; i<aWord.j ; i++ )
      if( i_word<=aWord[i].i_word ) break ;
   return i ;
}
/************************************************************************/
t_Path :: t_Path( void ) : t_Win()
{
   strcpy( Name,"Ручной перевод" );
   Up     =NULL ;
   Variant=NULL ;
   Tree   =NULL ;
   aWord  =NULL ;
   n_Tree =0 ;
   n_Variant=0 ;
   Antwort[0]=StrUp[0]=0 ;
   z_Tree=i_down=0 ;
}
/************************************************************************/
t_Path :: t_Path( t_Path *Up1, long z_Tree1 )
{
   strcpy( Name,Up1->Name );
   x1=Up1->x1 ; x2=Up1->x2 ; size_x=Up1->size_x ;
   y1=Up1->y1 ; y2=Up1->y2 ; size_y=Up1->size_y ;
   Up=Up1 ;
   Variant=NULL ;
   Tree   =NULL ;
   aWord  =NULL ;
   n_Tree =0 ;
   n_Variant=0 ;
   Antwort[0]=StrUp[0]=0 ;
   Up1->print1( StrUp,z_Tree1 );
   z_Tree=i_down=0 ;
}
/************************************************************************/
t_Path :: ~t_Path()
{
   del();
}
/************************************************************************/
void t_Path :: del()
{
   for( long i=0 ; i<n_Tree ; i++ )
      if( Tree[i].Down!=NULL )
      {   delete(Tree[i].Down); Tree[i].Down=NULL ;  }
   Free(aWord);
   Free(Variant);
   Free(Tree);
   n_Tree   =0 ;
   n_Variant=0 ;
}
/***************************************************************************/
//                             рисовалка
/***************************************************************************/
void t_Path :: paint( )
{   
   long   i,j,j1,n,z,z1,Begin,n_Down ;
   short  yy ;
   char   f,f1 ;
   e_Result r ;
   t_pTree *T ;
   char     Str[LFRASA] ;

try
{
   // ------ нарисовать рамки --------------------------
   t_Win :: paint();
   s_rame2_F( y1, x1, y2, x2, 0x07 );
   s_rame2_F( y1, x1, y1+4, x2, 0x07 );
   s_rame2_F( y1, x1, y1+2, x2, 0x07 );
   s_text_yxf( y1  , x1+3, 0x0a, "Оригинал" );
   s_text_yxf( y1+2, x1+3, 0x0a, "Перевод" );
   s_text_yxf( y1+4, x1+3, 0x0a, "Варианты продолжения" );
   s_goto_xy( y1+1,x1+1 );

   T=&Tree[z_Tree] ;
   // ------ нарисовать оригинал -----------------------   
   {
   short z1,z2,halb=size_x/2 ;

   z1=sou_length1( );
   
   z2=z1+x1+2 ;
   r=Core.result(); 
   if( r==E_NULL || r==E_ERROR )
            Strcpy( Str,"",LFRASA ) ;
       else Strcpy( Str,Core.sou(),LFRASA ) ;
   if( size_x-5<z1+halb )
   {   strcpy( Str,Str+z1-(size_x-5-halb) );
       z2=x1+2+size_x-5-halb ;
   }
   Str[size_x-5]=0 ;
   remove_wk( Str );
   s_text_yxf( y1+1, x1+2, 0x07, Str );
   uchar t[2]={ 0x3d ,0 };
   s_color_yxt( y1+1, z2+6, t,"0" );
   }

   // ------ нарисовать перевод -----------------------
   if( no_trans(r) )
            Strcpy( Str,Core.antwort().str(),LFRASA );
       else print1( Str,z_Tree );
   n=strlen( Str );
   if( size_x-4<n ) Begin=n-size_x+4 ;
       else         Begin=0 ;
   remove_wk( Str );
   s_text_yxf( y1+3, x1+2, 0x07, Str+Begin );
   if( no_trans(r) ) return ;
   
   // ------ нарисовать варианты перевода --------------   
   n_Down=0 ;
   for( i=0 ; i<T->n_down ; i++ )
      if( 0<Tree[T->down+i].n_slowo )
      n_Down+=Tree[T->down+i].n_slowo ; 
      
   z=size_y-6 ;
   Begin=0 ;
   if( i_down<Begin )   Begin=i_down ;
   if( Begin+z<i_down ) Begin=i_down-z ;
   
   long L ;
   
   for( i=j=j1=n=0 ; i<n_Down ; i++ )
   {  j++ ;
      if( Tree[T->down+j1].n_slowo<j )
      {  j1++ ; j=1 ;  }
      if( Begin+z<i ) break ;
      if( i<Begin ) continue ;
      if( i==i_down ) {  f=0x60 ; f1=0x70 ;  }
          else        {  f=0x0e ; f1=0x07 ;  }
      Strcpy( Str,emptystr(size_x-3),LFRASA );
      yy=y1+5+i-Begin ;
      s_text_yxf( yy, x1+2, f1, Str );

      print2( Str, z_Tree, j1, j-1 );
      Str[size_x-3]=0 ;
      remove_wk( Str );
      s_text_yxf( yy, x1+2, f, Str );
      z1=strlen( Str );
      L=print3( Str, z_Tree, j1, j-1 );
      Str[size_x-4-z1]=0 ;
      remove_wk( Str );
      s_text_yxf( yy, x1+3+z1, f1, Str );
      if( L<size_x-4-z1 ) Str[L]=0 ;
      s_text_yxf( yy, x1+3+z1, 0x0a, Str );
   }
   paint_hotkey( n_Line-1, 0, hk_Path );
}
catch( int E )
{
   error_add( "t_Path :: paint( )\n" );
   throw(E);
}
}
/************************************************************************/
//        переведенный кусок источника
//        (надо же как сложно сделать такое простое действие)
/************************************************************************/
long t_Path :: sou_length1(  )
{  long i,z2,z3 ;

   if( n_Variant<=0 ) return 0 ;

   t_pTree    *T=&Tree[z_Tree] ;
   t_pVariant *V=&Variant[T->first] ;
   
   if( V->aWord.j<=0 )
   {   if( Up!=NULL ) return Up->sou_length1( );
       return 0 ;
   }   
   t_aWord  *W=&V->aWord[0] ;
   
   if( T->i_last_aword< V->aWord.j )
   {
       if( 0<T->i_last_aword )
       {  z2=T->i_last_aword-1 ;
          z3=W->i_last_word ;
       }
       else
       {  z2=T->i_last_aword ;
          z3=W->i_word ;
       }
   }
   else
   {  z2=V->aWord.j-1 ;
      z3=W->i_last_word ;
   }   
   for( i=z2 ; 0<=i ; i-- )
   {  W=&V->aWord[i] ;
      if( 0<=z3 ) break ;
   }
   return sou_length( W->i_last_word );
}
/***************************************************************************/
long t_Path :: sou_length( long i_word )
{  short i,L ;

   if( i_word<0 ) return 0 ;
   if( Core.fn_word()<=i_word ) return strlen( Core.sou() );
   return Core.from()[i_word].z_str ;
}
/***************************************************************************/
//           нарисовать фразу (уже переведенный кусок)
/***************************************************************************/
# define add_ws(Str) if( ' '!=Str[strlen(Str)-1] ) Strcat( Str," ",LFRASA );
void t_Path :: print1( char *Str1, long i_Node )
{  long i,i1,j,z,i_slowo1 ;
   t_pTree   *T ;
   long index[100] ;

try
{  z=i_Node ;
   j=0 ;
   while( 1 )
   {  index[j++]=z ;
      z=Tree[z].up ;
      if( z<0 ) break ;
   }
   
   Str1[0]=0 ;
   if( StrUp[0]!=0 )
   {   strcat( Str1,StrUp ); add_ws( Str1 );
   }
   
   for( i=j-1 ; 0<=i ; i-- )
   {
      T=&Tree[index[i]] ;
      for( i1=T->i_aword ; i1<T->i_last_aword ; i1++ )
      {  if( Variant[T->first].aWord.j<=i1 ) return ;
         if( T->i_aword<i1 ) i_slowo1=0 ; 
             else            i_slowo1=T->i_slowo1 ;
      	 Strcat( Str1, print_serv( &Variant[T->first].aWord[i1], i_slowo1 ),LFRASA );
         add_ws( Str1 );	 
      }
   }
}
catch( int E )
{
   char EStr[200] ;
   sprintf( EStr,"t_Path :: print1( Str1,%d )\n",i_Node );
   error_add( EStr );
   throw(E);
}
}
/***************************************************************************/
//       нарисовать ближайший шаг
/***************************************************************************/
void t_Path :: print2( char *Str1, long i_Node, long i_down1, long i_slowo1 )
{  long     i ;
   t_pTree *T ;

try
{  T=&Tree[Tree[i_Node].down+i_down1] ;

   Str1[0]=0 ;
   for( i=T->i_aword ; i<T->i_last_aword ; i++ )
   {  if( Variant[T->first].aWord.j<=i ) return ;
      if( T->i_aword<i ) i_slowo1=0 ;
      Strcat( Str1, print_serv( &Variant[T->first].aWord[i], i_slowo1 ),LFRASA );
      Strcat( Str1," ",LFRASA );
   }
}
catch( int E )
{
   char EStr[200] ;
   sprintf( EStr,"t_Path :: print2( Str1,%d,%d,%d )\n",i_Node, i_down1, i_slowo1 );
   error_add( EStr );
   throw(E);
}
}
/***************************************************************************/
//       нарисовать дальнейшие шаги
/***************************************************************************/
long t_Path :: print3( char *Str1, long i_Node, long i_down1, long i_slowo1 )
{
   long   i,i1 ;
   t_pVariant *V ;
   t_pTree    *T ;

try
{  T=&Tree[Tree[i_Node].down+i_down1] ;
   V=&Variant[T->first] ;
   Str1[0]=0 ;
   for( i=T->i_last_aword ; i<V->aWord.j ; i++ )
   {  Strcat( Str1, print_serv( &V->aWord[i], 0 ),LFRASA );
      Strcat( Str1," ",LFRASA );
   }
   if( Up!=NULL )
   {   char Str2[LFRASA] ;
       split_final( Tree[i_Node].down+i_down1,&i,&i1 );
       // zz_Tree индекс этого дерева в вышестоящем дереве
       Up->print3( Str2, Up->z_Tree ,i, 0 );
       Strcat( Str1,Str2,LFRASA );
   }
   return 0 ;
}
catch( int E )
{
   char EStr[200] ;
   sprintf( EStr,"t_Path :: print2( Str1,%d,%d,%d )\n",i_Node, i_down1, i_slowo1 );
   error_add( EStr );
   throw(E);
}
}
/***************************************************************************/
char * t_Path :: print_serv( t_aWord *W, short i_slowo1 )
{
   t_Slowo2   *Slowo2 ;
   t_sStruct  *S ;
   short       i,is ;
   char       *Str ;
   static char Str2[LFRASA] ;
   static char Str3[LFRASA] ;

try
{      
   if( W->i_struct<0 )
       // ---- если это константа --------
       if( W->meaning<0 ) return W->Str ;
           else           return Meaning.get( W->meaning ) ;
   else
   {   if( W->n_slowo<=1 ) i_slowo1=0 ;       
       if( W->i_slowo<0 || W->n_slowo<=0 )
       {   // ----- какой-то загадочный случай (убрать смелости не хватило) -----
           if( W->meaning<0 ) Str=W->Str ;
	       else           Str=Meaning.get( W->meaning ) ;
	   if( Grammar[W->i_struct].To.type==TWORD )
	            is=i_FormatT[W->i_struct] ;
	       else is=-1 ;
	   if( 0<=is )
	   {  word_source( W->i_struct, Str, Str2, TO );       
              Slowo2=&SlowoT[ is ];
              return Slowo2->form( Str2, &W->Form );
	   }
	   else
              return Str ;
       }
       else
       {   // ----- слово или структура -------------------
           t_Form Form[10] ;
	   t_Struct *SS ;
	   t_sWord  *W1 ;
	   t_RelationList1 RR ;
	   t_Relation R ;

           S=Perevod.get_to( W->i_slowo, i_slowo1 );
           // ----- вычисление параметров -----------------
	   SS=&Grammar[S->i_struct].To ;
	   if( SS->type==TSTRUCT2 )
        	    RR=Perevod.get_relation( W->i_slowo, i_slowo1 );
	       else RR=SS->Relation ;
           Form[0]=W->Form ;
	   for( i=0 ; i<S->n_Word ;i++ )
	      Form[i+1]=S->Word[i].Param.form() ;
	   if( 0<=W->i_struct && Grammar[W->i_struct].To.type==TWORD )
	       Form[1]=Form[0] ;
	   for( i=0 ; i<RR.j ; i++ )
	   {  R=RR.list[i] ;
	      Form[R.s2].value[R.p2]=Form[R.s1].value[R.p1] ;
           }
	   // ----- сложение составляющих -----------------
	   Str3[0]=0 ;
           for( i=0 ; i<S->n_Word ;i++ )
	   {  W1=&S->Word[i] ;
	      Str=W1->str ;
	      if( 0<=W1->i_struct ) 
	               is=i_FormatT[W1->i_struct] ;
	          else is=-1 ;
	      if( 0<i ) strcat( Str3," " ) ;
	      if( 0<=is )
	      {  word_source( W1->i_struct, Str, Str2, TO );
        	 Slowo2=&SlowoT[ is ];
        	 Strcat( Str3,Slowo2->form( Str2, &Form[i+1] ),LFRASA );
	      }
	      else
        	 Strcat( Str3,Str,LFRASA ) ;
          }
	  return Str3 ;
       }
   }
}
catch( int E )
{
   char EStr[200] ;
   sprintf( EStr,"t_Path :: print_serv( Str1,%d )\n", i_slowo1 );
   error_add( EStr );
   throw(E);
}
}
/***************************************************************************/
//                 вызов окна ручного выбора перевода
/***************************************************************************/
e_WinMsg t_Path :: loop( void )
{
   e_WinMsg r1 ;
try
{
   e_Result r=Core.result( );
   del();   
   
   if( no_trans(r) )
   {   Strcpy( Antwort,Core.antwort().str(),LFRASA );
       r1=universe() ;
       provokator( );
       return r1 ;
   }
   else
   {   Antwort[0]=0 ;
       make_variant1( );
       make_tree( );
       z_Tree=i_down=0 ;
       r1=universe( );
       provokator( );
       return r1 ;
   }
}
catch( int E )
{
   error_add( "t_Path :: ВНУТРЕННЯЯ ОШИБКА ПРОГРАММЫ" );
   figna( error_get() );
   return WM_ESC ;
}
}
/***************************************************************************/
enum e_Step  // что делает universe в данный момент
{   Step_Up,
    Step_Down,
    Step_Nothing
};
/***************************************************************************/
//                     формирование ответа
//       (ужасная повторяемость текста надо в if переделать)
/***************************************************************************/
e_WinMsg t_Path ::  universe(  )
{
   short     ZZ,key1,key2,Level ;
   long      z1_Tree,i,i1,i2,n_Down ;
   char     *s1,*s2 ;
   t_pTree  *T,*T1 ;
   e_WinMsg  r ;
   e_Result  r1 ;
   e_Step    Step=Step_Nothing ;
   char Sign[2]=".";
   char StrTree[60] ;

   
try
{
   Sign[0]=Core.sign();

   // -------- определение уровня дерева (для отладки) ----------
   t_Path *P=this ;
   for( Level=0 ; Level<100 ; Level++ )
   {  if( P->Up==NULL ) break ;
      P=P->Up ;
   }

   r1=Core.result();
   if( !no_trans(r1) )
   {   if( n_Tree<=0 || Tree[z_Tree].ende==1 )
       {   // ---- дерево состоит из единственной вершины (вырожденный собиратель)
           if( z_Tree==0 )
	   {   print1( Antwort, z_Tree );
	       if( Up==NULL ) strcat( Antwort, Sign );
	       return WM_SELECT2 ;
	   }   
	   z_Tree=Tree[z_Tree].up ;
       }
   }

   while( 1 )
   {
      if( !no_trans(r1) )
      {	  T=&Tree[z_Tree] ;
	  if( T->fselect2 )
	  {   // ----- если попали в собиратель ----------
              if( T->Down==NULL )
	      {   T->Down=new t_Path( this, z_Tree ) ;
		  T->Down->make_variant2( this,z_Tree );
		  T->Down->make_tree( );
              }
	      r=T->Down->universe( );
	      switch( r )
	      {  case WM_OK:
		      T->Down->split_final( T->Down->z_Tree,&i1, &i2 );
		      // в этом месте ошибка
		      goto M_OK;
  	         case WM_SELECT2:
		      switch( Step )
		      {  case Step_Up   : goto M_PREV; 
		         case Step_Down :
			      T->Down->split_final( T->Down->z_Tree,&i1, &i2 );
			      goto M_OK;
		         case Step_Nothing :
			      goto M_Nothing ;    
		      }
		      break ;
		 case WM_PREV : goto M_PREV;
        	 case WM_ESC : return WM_ESC ;	  
	      }
	      continue ;
	  }
      }
 M_Nothing:
      paint( );
      // sprintf( StrTree," Level=%2d z_Tree =%4d i_down=%2d",Level,z_Tree,i_down );
      // s_text_yxf( y2,10,0xf0,StrTree );

      s_getch( &key1,&key2 ) ;
      ZZ=s_shiftstatus();

      Step=Step_Nothing ;
      if( (ZZ&(S_Ctrl_L|S_Ctrl_R))!=0 )
      {
          if( key1==0 && key2==S_key_Left  ) return WM_P_FRASA ;
	  if( key1==0 && key2==S_key_Right ) return WM_N_FRASA ;
	  continue ;
      }      
      
      switch( key1 )
      {
         case S_key_TabR : return WM_NEXT ;
	 case 0:
	     switch( key2 )
             {  case S_key_F1 : 
		     Window_help( (void *)"path.html") ;
		     break ;
		// ------------ переход в другие окна ------------
		case S_key_F2 : return WM_FIRST ;
		case S_key_F3 : return WM_SRC ;
		case S_key_F4 : return WM_DST ;
		case S_key_F5 : return WM_DEBUG ;
		case S_key_F6 : print_all(  );
				figna("отладочные деревья записаны");
		                break ;
        	case S_key_F10: return WM_ESC ;

		case S_key_Left : if( no_trans(r1) ) return WM_P_FRASA ;
		     break ;
		case S_key_Right: if( no_trans(r1) ) return WM_OK ;
		     break ;
             }
      }

      if( no_trans(r1) ) continue ;

      T=&Tree[z_Tree] ;
      for( n_Down=i=0 ; i<T->n_down ; i++ )
         if( 0<Tree[T->down+i].n_slowo )
	     n_Down+=Tree[T->down+i].n_slowo ;

      switch( key1 )
      {  case S_key_Esc : case S_key_Back : return WM_ESC ;
	 case 0:
	     switch( key2 )
	     {	case S_key_Left :
	             Step=Step_Up ;
	             goto M_PREV ;
		case S_key_Right :
		     split_down( z_Tree, i_down, &i1, &i2 );
		     Step=Step_Down ;
		     goto M_OK;
		case S_key_Up :   if( 0<i_down )        i_down-- ; break ;
		case S_key_Down : if( i_down<n_Down-1 ) i_down++ ; break ;
	     }
      }
      continue ;
 M_OK: // ---- шаг вперед -------
      z1_Tree=T->down+i1 ;
      T1=&Tree[z1_Tree] ;
      z_Tree=z1_Tree ;
      T1->i_slowo1=i2 ;
      if( T1->i_struct!=-2 && T->Down!=NULL )
      {	 t_aWord *W=&Variant[T1->first].aWord[T1->i_aword] ;
         s1=W->Str ;
	 s2=T->Down->Antwort+strlen( T->Down->StrUp ) ;
	 if( LWORD<=strlen(s2) ) W->meaning=Meaning.set( s2 ) ;
	     else                Strcpy( s1,s2,LWORD );
      }
      if( T1->ende==1 )
      {	  print1( Antwort, z1_Tree );
	  if( Up==NULL )
    	  {  
	     t_pVariant *V=&Variant[T1->first] ;
	     short       n_word=0,z ;
	     for( z=V->aWord.j-1 ; 0<=z ; z-- )
	        if( 0<=V->aWord[z].i_last_word )
		{   n_word=V->aWord[z].i_last_word ; break ;  }
	     
	     if( n_word<Core.fn_word() )
	     {   t_Antwort A ;
	         Strcat( Antwort, A.translate_simple( n_word ), LFRASA );
	     }
	     short L=strlen(Antwort) ;
	     if( ' '==Antwort[L-1] ) Antwort[L-1]=0 ;
	     Strcat( Antwort, Sign, LFRASA );
          }	  
	  return WM_OK ;
      }      
      i_down=0 ;
      continue ;
 M_PREV: // ---- шаг назад -------
      if( 0<=T->up )
      {   i_down=merge_down( z_Tree,z_Tree-Tree[T->up].down )+T->i_slowo1 ;
	  z_Tree=T->up ;
      M1: T1=&Tree[z_Tree] ;
	  if( T1->fselect2!=0 && T1->Down->n_Tree<=1 && 0<=T1->up )
	  // ------ вырожденный собиратель -------
	  {   z_Tree=T1->up ; goto M1 ; }	  
      }
      else
      {   if( Up!=NULL ) return WM_PREV ;
      }
      continue ;
   }
}
catch( int E )
{
   error_add( "t_Path :: universe\n" );
   throw(E);
}
}
/************************************************************************/
//       сосчитать число вариантов i_down сыновей
/************************************************************************/
long t_Path :: merge_down( long i_Tree, long i_down1 )
{  long  i,n ;
   t_pTree *T ;
try
{
   T=&Tree[i_Tree] ;
   for( n=i=0 ; i<i_down1 ; i++ )
      n=Tree[T->down+i].n_slowo ;
   return n ;
}
catch( int E )
{
   error_add( "t_Path :: merge_down\n" );
   throw(E);
}
}
/************************************************************************/
//    расщепить индекс потомка на "старший" (индекс потомка) 
//    и "младший" (индекс варианта потомка)
/************************************************************************/
void t_Path :: split_down( long i_Tree, long i_down1, long *i1, long *i2 )
{  long  i,n,n_Down ;
   t_pTree *T ;

try
{
   T=&Tree[i_Tree] ;
   n_Down=0 ;
   for( i=0 ; i<T->n_down ; i++ )
   {  n=Tree[T->down+i].n_slowo ;
      if( i_down1<n_Down+n )
      {   *i1=i ; *i2=i_down1-n_Down ; return ;  }
      n_Down+=n ;
   }
   *i1=-1 ; *i2=-1 ;
}
catch( int E )
{
   error_add( "t_Path :: split_down\n" );
   throw(E);
}
}
/************************************************************************/
void t_Path :: split_final( long i_Tree, long *i1, long *i2 )
{  long  z,up ;

try
{  z=i_Tree ;
   while( 0<z )
   {  up=Tree[z].up ;
      if( up==0 ) break ;
      z=up ;
   }
   //*i1=Variant[Tree[z].first].i_down ;
   *i1=Variant[Tree[i_Tree].first].i_down ;
   *i2=Tree[z].i_slowo1 ;
}
catch( int E )
{
   error_add( "t_Path :: split_final\n" );
   throw(E);
}
}
/************************************************************************/
//         сравнить два варианта перевода (для упорядочивания)
/************************************************************************/
int varcmp( const void *_a , const void *_b )
{   t_pVariant *a,*b ;
    long i,i1 ;
    char f=0 ;

try
{   a=(t_pVariant *)_a ; b=(t_pVariant *)_b ;
    if( a->aWord.j != b->aWord.j ) {  f=1 ; goto M_End ;  }
    for( i=0 ; i<a->aWord.j ; i++ )
    {  if( a->aWord[i].i_struct != b->aWord[i].i_struct ) { f=1 ; break ; }
       if( a->aWord[i].i_slowo  != b->aWord[i].i_slowo  ) { f=1 ; break ; }
       if( a->aWord[i].i_struct<0 && 0!=Strcmp( a->aWord[i].Str, b->aWord[i].Str ) ) 
       { f=1 ; break ; }
       
       t_Form &F1 = a->aWord[i].Form ;
       t_Form &F2 = b->aWord[i].Form ;
       for( i1=0 ; i1<10 ; i1++ )
          if( F1.value[i1]!=F2.value[i1] ) {  f=1 ; break ; }
    }
M_End :
    if( f==1 ) return a->index - b->index ;
        else   return 0 ;
}
catch( int E )
{
   error_add( "t_Path :: varcmp\n" );
   throw(E);
}
}
/************************************************************************/
//         сравнить два варианта слова (для упорядочивания)
/************************************************************************/
int awordcmp( const void *_a , const void *_b )
{  t_aWord *a,*b ;

   a=(t_aWord *)_a ; b=(t_aWord *)_b ;
   if( a->i_struct != b->i_struct )
       return a->i_struct - b->i_struct ;
   if( a->i_slowo  != b->i_slowo )
       return a->i_slowo - b->i_slowo ;

   if( a->i_word != b->i_word )       // только что вставил
       return a->i_word - b->i_word ; 
   if( a->i_last_word != b->i_last_word )       // только что вставил
       return a->i_last_word - b->i_last_word ;

   if( a->i_struct<0 )
       return Strcmp( a->Str, b->Str );
   t_Form &F1 = a->Form ;
   t_Form &F2 = b->Form ;
   for( short i1=0 ; i1<10 ; i1++ )
      if( F1.value[i1]!=F2.value[i1] ) 
      {   return F1.value[i1]-F2.value[i1] ;  }

   return 0 ;    
}
/************************************************************************/
t_aWord get_word( t_pVariant &V, short i_aword )
{
   if( i_aword<V.aWord.j )
      return V.aWord[i_aword] ;
   else
   {  t_aWord W ;
      W.i_word   = 0 ;
      W.i_last_word= 0 ;
      W.i_struct =-2 ;
      W.i_slowo  = 0 ;
      W.n_slowo  = 0 ;
      W.Str[0]   = 0 ;
      W.meaning  =-1 ;
      return W ;
   }
}
/************************************************************************/
//              сделать дерево вариантов перевода
/************************************************************************/
void t_Path :: make_tree( void )
{  long    i,i1,j ;
   t_pTree     T1,*T ;
   t_aWord     W,W1 ;

try
{
   if( 0==Core.fn_word() )
   {   n_Variant=0 ;
       n_Tree   =0 ;
       return ;
   }

   Free(Tree);
   if( n_Variant==0 ) {  n_Tree=0 ; return ; }
   n_Tree=1+Core.from().j*n_Variant ;
   Tree=(t_pTree *)Calloc( n_Tree,sizeof(t_pTree) );
      
   // -------- внести первую вершину ------------------
   T1.up   =-1 ;
   T1.down = 1 ;
   T1.n_down = 0 ;
   T1.first= 0 ;
   T1.last = n_Variant ;
   T1.ende = 0 ;
   T1.i_struct = i_main_struct ;
   T1.i_aword =-1 ;
   T1.i_last_aword =0 ;
   T1.i_slowo1 =0 ;
   T1.n_slowo =0 ;
   T1.Down    =NULL ;
   Tree[0]=T1 ;
   // -------- внести последующие вершины -------------
   char  f_split,f_exist ;
   short i_aword ;
   
   for( i=0,j=1 ; i<j ; i++ )
   {  T=&Tree[i] ;
      // ----- выяснить есть ли "разветвления" на этой ветке ------
      if( T->last-T->first<=1 ) 
      {  for( i1=T->i_aword+1 ; i1<Variant[T->first].aWord.j ; i1++ )
         {  W = get_word( Variant[T->first], i1 );
            if( 1<W.n_slowo ) goto M_Need ;
	    if( 0<=W.i_struct && Grammar[W.i_struct].From.type==TSELECT2 )	    
	        goto M_Need ;
         }
	 T->ende=1 ; T->i_last_aword=100 ; continue ;  
         M_Need : ;
      }
      T->down=j ;
      // ---- пройти вдоль ветки, и сделать в нужном слове разветвление ----
      for( i_aword=T->i_aword+1 ; i_aword<100 ; i_aword++ )
      {  f_split=f_exist=0 ;
         // ---- делать ли разветвление в слове i_word ----
         if( 0<=T->i_struct && Grammar[T->i_struct].From.type==TSELECT2 )
	     f_split=1 ; 
         W = get_word( Variant[T->first], i_aword );
         for( i1=T->first ; i1<T->last ; i1++ )
	 {  W1 = get_word( Variant[i1], i_aword );
	    if( i_aword<Variant[i1].aWord.j ) f_exist=1 ;
	    if( 0!=awordcmp( &W,&W1 ) ) {  f_split=1 ; break ; }
	    if( 1<W1.n_slowo )          {  f_split=1 ; break ; }
	    if( 0<=W1.i_struct && Grammar[W1.i_struct].From.type==TSELECT2 )
	    {   f_split=1 ; break ; }
	 }
	 if( f_exist==0 ) break ;
	 if( f_split==0 ) continue ;

         // ---- если делать, то какие варианты в какие ветки попадают ----
         W.i_struct=-2 ;
         for( i1=T->first ; i1<T->last ; i1++ )
         {        
            W1 = get_word( Variant[i1], i_aword );
	    
            if( 0!=awordcmp( &W,&W1 ) || W.i_struct==-2 )
	    {
		if( T->first<i1 ) Tree[j-1].last=i1 ;
		T1.up      = i ;
		T1.n_down  = 0 ;
		T1.first   = i1 ;
		T1.last    = n_Variant ;
		T1.ende    = 0 ;
		T1.i_struct= W1.i_struct ;
		T1.i_aword = i_aword ;
		T1.i_slowo1= 0 ;
		T1.n_slowo = W1.n_slowo ;
		if( n_Tree<=j )
		{   Tree=(t_pTree *)Realloc( Tree,2*n_Tree*sizeof(t_pTree) );
		    n_Tree=2*n_Tree ;
		}
		Tree[j++]=T1 ;	 
		W=W1 ;
	    }
	 }
	 if( f_split==1 )
	 {  T->i_last_aword=i_aword ;
	    Tree[j-1].last=T->last ;
         }
	 if( f_split==1 ) break ;
      }
      T->n_down=j-T->down ;
   }
   if( n_Tree<j )
   {  error_set("t_Path :: make_tree Ошибка памяти при заполнении Tree\n");
      throw(-1);
   }
   n_Tree=j ;
   Tree=(t_pTree *)Realloc( Tree,n_Tree*sizeof(t_pTree) ); // Memory Leak
   Tree[0].i_aword=0 ;
   // ---- проставить n_slowo и fselect2 -------
   for( i=0 ; i<n_Tree ; i++ )
   {
      if( Tree[i].n_slowo<=0 )
          Tree[i].n_slowo=1 ;
      Tree[i].fselect2=0 ;
      if( 0<=Tree[i].i_struct && Grammar[Tree[i].i_struct].From.type==TSELECT2 &&
          0<=Tree[i].up )
          Tree[Tree[i].up].fselect2=1 ;
   }
// print_tree( "Tree0" );
}
catch( int E )
{
   error_add( "t_Path :: make_tree\n" );
   throw(E);
}
}
/************************************************************************/
//           подсчет сыновей для "пред-собирателя"
/************************************************************************/
long t_Path :: n_variant_calc( t_Path *Up1, long z_Tree1 )
{  long   i,i1,n ;
   short  i_struct ;
   t_Struct *SS ;
   t_pTree  *T,*T1 ;
   t_Variants *VV ;

try
{
   T=&Up1->Tree[z_Tree1] ;
   for( i=n=0 ; i<T->n_down ; i++ )
   {  T1=&Up1->Tree[T->down+i] ;
      i_struct=T1->i_struct ;
      if( 0<=i_struct )
          SS=&Grammar[i_struct].From ;

      if( i_struct<0 || SS->type!=TSELECT2 )
          n++ ;
      else
      {  
	 for( i1=0 ; i1<SS->Word.j ; i1++ )
	 {  //t_aWord *W=&Up1->Variant[T->first].aWord[T->i_last_aword] ;
	    t_aWord *W=&Up1->Variant[T1->first].aWord[T1->i_aword] ;
	    if( W->i_word<0 || SS->Word[i1].i_struct<0 ) 
	        n++ ;
            else
	    {   VV=Core.variants( W->i_word,SS->Word[i1].i_struct );
		n+=VV->Variant.j ;
            }
	 }
      }
   }
   return n ;
}
catch( int E )
{
   char EStr[200] ;
   sprintf( EStr,"t_Path :: n_variant_calc( Up1,%d )\n", z_Tree );
   error_add( EStr );
   throw(E);
}
}
/************************************************************************/
//     сделать массив вариантов для последующего построения дерева
/************************************************************************/
void t_Path :: make_variant1( void )
{  long i,j ;
   t_Variants *VV ;
   t_Form      Form0 ;
try
{
   VV=Core.variants( 0, i_main_struct );
   Free( Variant );
   Variant=(t_pVariant *)Calloc( VV->Variant.j,sizeof(t_pVariant) );
   
   for( i=j=0 ; i<VV->Variant.j ; i++ )
   {   
      if( Core.f_full() && VV->Variant[i].i_last_word<Core.fn_word())
          continue ;
      Core.antwort().make( 0,i_main_struct,i,Form0 );
      Variant[j].index =i ;
      Variant[j].i_down=i ;
      Variant[j].aWord  =*Core.antwort().aword() ;
      Core.antwort().del();
      if( VV->Variant.j<=j )
      {  error_set("t_Path :: make_variant Ошибка памяти при заполнении Variant\n");
	 throw(-1);
      }
      j++ ;
   }
   n_Variant=j ;
   sort_variant();
}
catch( int E )
{
   error_add( "t_Path :: make_variant1\n" );
   throw(E);
}
}
/************************************************************************/
//     сделать массив вариантов для последующего построения дерева
/************************************************************************/
void t_Path :: make_variant2( t_Path *Up1, long z_Tree1 )
{  long   i,i1,i2,i3,j,n,i_struct,i_last_word ;
   t_Variants *VV ;
   t_Struct   *SS ;
   t_pTree    *T,*T1 ;
   t_aWord    *W ;
   t_RelationList1 RR ;
   t_Relation R ;

try
{   n_Variant=n_variant_calc( Up1,z_Tree1 );

   Free( Variant );
   Variant=(t_pVariant *)Calloc( n_Variant,sizeof(t_pVariant) ); // Memory Leak
   
   T=&Up1->Tree[z_Tree1] ;
   for( j=i=n=0 ; i<T->n_down ; i++ )
   {  T1=&Up1->Tree[T->down+i] ;
      i_struct=T1->i_struct ;
      if( 0<=i_struct )
          SS=&Grammar[i_struct].From ;
      if( 0<=i_struct && SS->type==TSELECT2 )
      {   RR=Grammar[i_struct].To.Relation ;
          W=&Up1->Variant[T1->first].aWord[T1->i_aword] ;
          i_last_word=W->i_last_word ;
          for( i1=0 ; i1<SS->Word.j ; i1++ )
	  {
	     if( SS->Word[i1].type==TWORD0 )
	     {  
		Variant[j].index=j ; // i
		Variant[j].i_down=i ;
		j++ ;
	     }
	     else
	     {
		VV=Core.variants( W->i_word,SS->Word[i1].i_struct );
		for( i2=0 ; i2<VV->Variant.j ; i2++ )
		{  if( VV->Variant[i2].i_last_word!=i_last_word ) continue ;
	           t_Antwort A ;

		   // ---- госссподи, что я делаю, какой позор! -------
		   // ---- вычисляю параметры потомка, подменяя функции ядра ----
		   t_Form    Form ;
		   Form.init();
		   for( i3=0 ; i3<RR.j ; i3++ )
		   {  R=RR.list[i3] ;
		      if( R.s1!=0 ) continue ;
		      if((R.s2-1)!=i1 ) continue ;
		      Form.value[R.p2] = W->Form.value[R.p1] ;
                   }
		   A.make( W->i_word, SS->Word[i1].i_struct, i2, Form );
		   if( n_Variant<=j ) 
		       goto M_Err_Mem ;
		   Variant[j].index=j ; // i
		   Variant[j].i_down=i ;
		   Variant[j].aWord  =*A.aword() ;
		   A.del();
		   j++ ;
        	}	     
	     }
	  }
      }
      else
      {
	  if( n_Variant<=j ) 
	      goto M_Err_Mem ;
          Variant[j].index =j ;
          Variant[j].i_down=i ;
	  // ну а вообще-то как отображать такой "вариант с пустым словом"???
	  if( T1->i_struct!=-2 )
	      Variant[j].aWord.add( Up1->Variant[T1->first].aWord[T1->i_aword] );
          j++ ;
      }
   }
   n_Variant=j ;
   sort_variant();
   return ;

M_Err_Mem:
   error_set("t_Path :: make_variant2 Ошибка памяти при заполнении Variant\n");
   throw(-1);
}
catch( int E )
{
   char EStr[200] ;
   sprintf( EStr,"t_Path :: make_variant2( Up1,%d )\n", z_Tree1 );
   error_add( EStr );
   throw(E);
}
}
/************************************************************************/
//          сортировка вариантов в удобном для человека порядке
/************************************************************************/
void t_Path :: sort_variant()
{
   long i,i1,j,i_word,max_Word,End,m_Variant ;
   t_pVariant **Var1,**Var2,**Var3,*zVar ;
   t_aWord    W1,W2 ;

   // ------ Сделать массив ссылок на варианты -----------------
   m_Variant=n_Variant ;
   Var1=(t_pVariant **)Calloc( n_Variant,sizeof(t_pVariant *) );
   Var2=(t_pVariant **)Calloc( n_Variant,sizeof(t_pVariant *) );
   for( i=0 ; i<n_Variant ; i++ )
      Var1[i]=&Variant[i] ;

   // ------ Уничтожить одинаковые стоящие рядом варианты ------
   for( i=j=1 ; i<n_Variant ; i++ )
      if( 0!=varcmp( Var1[j-1],Var1[i] ) )
      {   Var1[j++]=Var1[i] ;  }
   if( 0<n_Variant ) n_Variant=j ;

   for( i=max_Word=0 ; i<n_Variant ; i++ )
      if( max_Word<Var1[i]->aWord.j ) max_Word=Var1[i]->aWord.j ;

   // ------ Цикл по номеру слова в варианте -------------------
   for( i_word=0 ; i_word<max_Word ; i_word++ )
   {
       // ---- Цикл по номеру варианта ----------------------
       for( i=j=0 ; i<n_Variant ; i++ )
       {
	  // ----- Если вариант выбран - continue -----
	  if( Var1[i]==NULL ) continue ;
	  // ----- Переписать вариант во второй массив вариантов и
	  // ----- Установить на него стрелку
	  zVar=Var1[i] ; Var2[j++]=Var1[i] ; Var1[i]=NULL ;
	  // ----- Установить предел индекса End по неравенству предыдущего слова 
	  if( i_word==0 ) 
	     End=n_Variant ;
	  else
	  {  W1=get_word( *zVar,i_word-1 );
	     for( End=i+1 ; End<n_Variant ; End++ )
	     {  if( Var1[End]==NULL ) continue ;
	        W2=get_word( *(Var1[End]),i_word-1 );
		if( 0!=awordcmp( &W1,&W2 ) ) break ;
	     }
	  }
	  // ----- Еще цикл по номеру варианта i1=i+1 ; i1<End
	  for( i1=i+1 ; i1<End ; i1++ )
	  {  if( Var1[i1]==NULL ) continue ;
	     // -- Если вариант тождественен варианту на стрелке - переписать в другой массив
	     W1=get_word( *zVar,i_word );
	     W2=get_word( *(Var1[i1]),i_word );
	     if( 0==awordcmp( &W1,&W2 ) )
	     {   Var2[j++]=Var1[i1] ; Var1[i1]=NULL ;  } 
	  }
       }
       if( n_Variant<j )
           printf("Аааа!!! Ошибка!!!!\n");
       // ----- Поменять первый и второй массивы ссылок ------
       Var3=Var1 ; Var1=Var2 ; Var2=Var3 ;
   }

   // ------ Уничтожить одинаковые стоящие рядом варианты ------
   for( i=j=1 ; i<n_Variant ; i++ )
      if( 0!=varcmp( Var1[j-1],Var1[i] ) )
      {   Var1[j++]=Var1[i] ;  }
   if( 0<n_Variant ) n_Variant=j ;

   // ------ Переписать массив вариантов ------------------------
   t_pVariant *Variant1 ;
   long        n_aWord ;

   for( i=n_aWord=0 ; i<n_Variant ; i++ )
      n_aWord+=Var1[i]->aWord.j ;

   Variant1=(t_pVariant *)Calloc( n_Variant,sizeof(t_pVariant) );
   aWord   =(t_aWord    *)Calloc( n_aWord  ,sizeof(t_aWord) );

   for( i=j=0 ; i<n_Variant ; i++ )
   {  Variant1[i].index =Var1[i]->index ;
      Variant1[i].i_down=Var1[i]->i_down ;
      Variant1[i].aWord.list=aWord+j ;
      Variant1[i].aWord.j=Var1[i]->aWord.j ;
      for( i1=0 ; i1<Var1[i]->aWord.j ; i1++ )
         aWord[j++]=Var1[i]->aWord[i1] ;
   }
   for( i=0 ; i<m_Variant ; i++ )
      Variant[i].aWord.del();

   Free( Var1 ); Free( Var2 ); Free(Variant) ;
   Variant=Variant1 ;
}
/************************************************************************/
//               печать вариантов (для отладки)
/************************************************************************/
void t_Path :: print_variant( char *File )
{  long     i,i1,j ;
   FILE    *fw ;
   t_aWord  W ;
   
   fw=Fopen( File,"w");
   
   for( i=0 ; i<n_Variant ; i++ )
   {  fprintf( fw,"\n %3d i_down=%4d",i,Variant[i].i_down );
      for( i1=0 ; i1<Variant[i].aWord.j ; i1++ )
      {  W=Variant[i].aWord[i1] ;
         fprintf( fw,", %d %s ",W.n_slowo, W.Str );
	 for( j=0 ; j<10 ; j++ )
	    fprintf( fw,"%c",'0'+W.Form.value[j] );
      }
   }
   Fclose(fw);
}
/************************************************************************/
//               печать дерева (для отладки)
/************************************************************************/
void t_Path :: print_tree( char *Name1 )
{  long i ;
   t_pTree *T ;
   FILE *fw=Fopen( Name1,"w");
   char *s1 ;

   fprintf( fw,"\n    i    up  down n_down | first  last | i_word i_word n_slowo ende select |" );
   for( i=0 ; i<n_Tree ; i++ )
   {  T=&Tree[i] ;
      s1="" ;
      if( 0<=T->i_aword && T->i_aword<Variant[T->first].aWord.j )
          s1=Variant[T->first].aWord[T->i_aword].Str ;
      fprintf( fw,"\n %5ld %5ld %5ld %5ld | %5ld %5ld | %5d %5d %5d %5d    %d | %s",i,
               T->up, T->down, T->n_down, T->first, T->last, 
	       T->i_aword, T->i_last_aword, T->n_slowo, T->ende, T->fselect2, s1 );
   }
   Fclose(fw);
}
/************************************************************************/
void t_Path :: print_all(  )
{  t_Path *P=this ;
   short i,n ;
   char Str[100] ;

   for( i=0 ; i<100 ; i++ )
   {  if( P->Up==NULL ) break ;
      P=P->Up ;
   }
   n=i ;
   P=this ;
   for( i=0 ; i<100 ; i++ )
   {  sprintf( Str,"Var%d",n-i );
      P->print_variant( Str );
      sprintf( Str,"Tree%d",n-i );
      P->print_tree( Str );
      if( P->Up==NULL ) break ;
      P=P->Up ;
   }
   
}
/************************************************************************/
t_Meaning ::  t_Meaning()
{
   Meaning  =(char *)Calloc( l_Meaning=5000,1 );
   j_Meaning=0 ;
}
/************************************************************************/
t_Meaning :: ~t_Meaning()
{
   Free(Meaning);
}
/************************************************************************/
long t_Meaning :: set( char *Str )
{  long z,L,LL ;

   L=strlen(Str);
   if( L==0 ) return -1 ;

   if( l_Meaning<=j_Meaning+L+1 )
   {   LL=l_Meaning+max(l_Meaning>>2,L+1) ;
       Meaning=(char *)Realloc( Meaning,LL );
       l_Meaning=LL ;
   }
   z=j_Meaning ;
   strcpy( Meaning+j_Meaning, Str );
   j_Meaning+=L+1 ;
   return z ;
}
/************************************************************************/
char * t_Meaning :: get( long z )
{
   if( z<0 ) return "" ;
   return Meaning+z ;
}
