/***************************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//     t_Debug       смотрелка для t_Core->Variants
//     t_GrammarView смотрелка для t_All_Struct таблицы правил перевода
//     t_Slowo3View  смотрелка для t_Slowo3
//     Анисимов Д.В.                                         сегодня
/***************************************************************************/
# include <string.h>
# include <mylib.h>
# include <video.h>
# include <s_defkey.h>
# include <color.h>
# include <hotkey.h>
# include <debug.h>

void  Window_help( void *Void );
void  remove_wk( char *Str );

/***************************************************************************/
//       смотрелка для t_Core->Variants (основной таблицы разбора)         //
/***************************************************************************/
t_Debug :: t_Debug( void ) : t_Win()
{
   strcpy( Name, "Отладчик" );
   i_start_struct   =0 ;

   Current.i_struct   =0 ;
   Current.i_word     =0 ;
   Current.i_last_word=0 ;
   Current.i_variant  =0 ;

   i_Select =0 ;
   Begin_S  =0 ;
   Begin_V  =0 ;
   Begin_R  =0 ;
   SelectVision.init();
   z_Vision =0 ;
   Reg      =DSTRUCT ;
   Reg1     =D1STRUCT ;
}
/***************************************************************************/
void t_Debug :: init( void )
{  short  i ;
   e_Type t ;

   SelectVision.j=0 ;
   for( i=0 ; i<Grammar.n_trans() ; i++ )
   {  t=Grammar[i].From.type ;
      if( t==TWORD || t==TSTRUCT2 || t==TSELECT || t==TSELECT1 )
          SelectVision.add( i );
   }
}
/***************************************************************************/
void t_Debug :: paint( void )
{  short xx ;
   char  *s1="",*s2="" ;

   t_Win::paint();

   xx=x1+(x2-x1)/3 ;
   // ----------------- нарисовать рамки ------------------------
   s_rame2_F( y1,   x1, y1+4, x2  , 0x07 );
   s_rame2_F( y1,   x1, y1+2, x2  , 0x07 );
   s_rame2_F( y1+5, x1, y2,   xx-1, 0x07 );
   s_rame2_F( y1+5, xx, y2,   x2  , 0x07 );
   s_text_yxf( y1,   x1+2, 0x0a, "Переводимая фраза" );
   s_text_yxf( y1+2, x1+2, 0x0a, "Вершина стека" );
   switch( Reg )
   {   case DSTRUCT  : s1 = "Конструкции" ; 
                       if( Reg1==D1SELECT ) s1 = "Выборы     " ;
		       s2 = "Варианты" ;
		       break ;
       case DVARIANT : s1 = "Варианты   " ;
                       s2 = "Потроха " ;
                       break ;
       case DREAL    : s1 = "Стек       " ;
                       s2 = "Потроха " ;
                       break ;
   }
   s_text_yxf( y1+5, x1+2, 0x0a, s1 );
   s_text_yxf( y1+5, xx+2, 0x0a, s2 );

   if( 0==Core.fn_word() ) return ;

   paint_frasa( );
   paint_stack( xx );
   if( Reg==DSTRUCT )
       paint_variant( xx );
   if( Reg==DVARIANT || Reg==DREAL )
       paint_struct( xx );
}
/***************************************************************************/
//                    нарисовать переводимую фразу
/***************************************************************************/
void t_Debug :: paint_frasa( void )
{
   short  i,l,L,z,z1,z2,Z1,Z2,begin=0 ;
   short  first_word,last_word ;
   char   *S,Str[LFRASA] ;
   t_Variants *VV ;
   t_rStruct  *V ;
   t_rWord     R ;

   switch( Reg )
   {   case DSTRUCT  :
	    first_word=0 ;
	    last_word=-1 ;
	    break ;
       case DVARIANT :
	    stack[0]=Current ;
	    VV=Core.variants( Current.i_word,Current.i_struct );
	    if( VV->Variant.j<=Current.i_variant )
	    {   first_word=0 ;
	        last_word=-1 ;
	    }else
	    {   V=&VV->Variant[Current.i_variant];
		first_word=V->i_word ;
		last_word =V->i_last_word ;
	    }
	    break ;
       case DREAL :
	    V=Core.variant( Current ) ;
	    R=Core.rword(V->r_word+Begin_R+i_Select) ;
	    first_word=R.i_word ;
	    last_word =R.i_last_word ;
	    break ;
   }
   strcpy( Str,emptystr(LFRASA-1) );

   begin=0 ; L=x2-x1-4 ; Z1=Z2=0 ;
   for( i=z=0 ; i<Core.from().j ; i++ )
   {  S=Core.from()[i].Str ;
      l=strlen( S );
      if( LFRASA<z+l ) break ;
      strncpy( Str+z,S,l );
      if( i==Current.i_word ) {  z1=z ; z2=z+l ;  }
      if( i==first_word ) Z1=z ;
      if( i==last_word-1 ) Z2=z+l ;
      z+=l+1 ;
   }
   Str[z]=0 ;
   remove_wk( Str );
   //if( L<z2 ) begin=z2-L ;
   if( L/2<z2 ) begin=z2-L/2 ;
   if( L<z-begin ) Str[L+begin]=0 ;
   if( Z1<begin ) Z1=begin ;
   if( begin+L<Z2 ) Z2=begin+L ;
   s_text_yxf( y1+1, x1+2, 0x07, emptystr(L) ); // вот тут строка выходит за размеры экрана
   s_text_yxf( y1+1, x1+2, 0x07, Str+begin );
   Str[Z2]=0 ;
   if( Z1<Z2 )
       s_text_yxf( y1+1, x1+2+Z1-begin, 0x70, Str+Z1 );
   S=Core.from()[Current.i_word].Str ;
   if( first_word<=Current.i_word && Current.i_word<last_word )
	    s_text_yxf( y1+1, x1+2+z1-begin, 0x7b, S );
       else s_text_yxf( y1+1, x1+2+z1-begin, 0x0b, S );
}
/***************************************************************************/
//                           нарисовать стек
/***************************************************************************/
void t_Debug :: paint_stack( short xx )
{  long i,z ;
   char Str[80] ;
   t_Variants *VV ;
   char f ;

   // ---------------- нарисовать вершину стека ------------------
   if( Reg==DVARIANT )
   {   if( 0<Core.n_antwort() && stack[0].i_variant < Core.n_antwort() )
       {   t_Form  Form0 ;
	   Core.antwort().make( 0,i_main_struct,stack[0].i_variant,Form0 );
	   s_text_yxf( y1+3, x1+2, 0x07, Core.antwort().str() );
       }
       char *Str1 ;
       VV=Core.variants( Current.i_word,Current.i_struct );
       if( VV->Variant.j<=Current.i_variant ) Current.i_variant=VV->Variant.j-1 ;
       Str1=Core.get_meaning( &VV->Variant[Current.i_variant] );
       s_text_yxf( y1+3, x1+2, 0x07, Str1 );
       paint_hotkey( n_Line-1, 0, hk_Debug2 );
   }

   if( Reg==DSTRUCT )
   {  for( i=0 ; i<size_y-6 ; i++ )
      {  strcpy( Str,emptystr(79) );
	 z=Begin_S+i ;
	 if( (Reg1==D1STRUCT && z<Grammar.n_trans()) ||
	     (Reg1==D1SELECT && z<SelectVision.j ) )
	 {   
	     if( Reg1==D1SELECT ) z=SelectVision[z] ;
	     char try1='*' ;	     
	     VV=Core.variants( Current.i_word,z );
             if( VV->absend==0 && VV->Variant.j==0 ) try1=' ' ;
	     sprintf( Str,"%c %2ld %s",try1,VV->Variant.j,Grammar[z].From.Name );
	 }
         if( Begin_S+i==z_Vision ) f=0x70 ; else f=0x07 ;
	 Str[xx-x1-3]=0 ;	 
	 s_text_yxf( y1+6+i, x1+2, f, Str );
      }
      paint_hotkey( n_Line-1, 0, hk_Debug1 );
   }
   if( Reg==DVARIANT )
   {  VV=Core.variants( Current.i_word,Current.i_struct );
      for( i=0 ; i<size_y-7 ; i++ )
      {  strcpy( Str,emptystr(79) );
	 if( Begin_V+i<VV->Variant.j )
	 {   sprintf( Str,"%2ld %d-%d",
		      Begin_V+i,
		      VV->Variant[Begin_V+i].i_word,
		      VV->Variant[Begin_V+i].i_last_word
		      );
	 }
	 Str[xx-x1-5]=0 ;
	 if( Begin_V+i==Current.i_variant ) f=0x70 ; else f=0x07 ;
	 s_text_yxf( y1+7+i, x1+4, f, Str );
      }
   }
   if( Reg==DREAL )
   {  stack[j_stack]=Current ;
      for( i=0 ; i<size_y-6 ; i++ )
      {  sprintf( Str,"%s",Grammar[stack[i].i_struct].From.Name );
	 Str[xx-x1-3]=0 ;
	 s_text_yxf( y1+6+i, x1+2, 0x07, Str );
	 if( j_stack<=i ) break ;
      }
      paint_hotkey( n_Line-1, 0, hk_Debug3 );
   }
}
/***************************************************************************/
//           нарисовать варианты "0 1-13" в правом окошке
/***************************************************************************/
void t_Debug :: paint_variant( short xx )
{  long i ;
   t_Variants *VV ;
   char Str[80] ;

   VV=Core.variants( Current.i_word,Current.i_struct );
   for( i=0 ; i<size_y-7 ; i++ )
   {  strcpy( Str,emptystr(79) );
      if( i<VV->Variant.j )
      {   sprintf( Str,"%2ld %d-%d",
		   i,VV->Variant[i].i_word,VV->Variant[i].i_last_word );
      }
      Str[x2-xx-5]=0 ;
      s_text_yxf( y1+7+i, xx+2, 0x07, Str );
   }
}
/***************************************************************************/
//                      нарисовать стуктуры
/***************************************************************************/
void t_Debug :: paint_struct( short xx )
{  long        i ;
   t_Variants *VV ;
   t_rStruct  *V,*V1 ;
   t_rWord     R ;
   t_Struct   *SS ;
   t_Word     *W ;
   char        c,Str1[100],Str2[100] ;

   VV=Core.variants( Current.i_word,Current.i_struct ) ;
   if( VV->Variant.j<=Current.i_variant ) return ;
   V =Core.variant( Current ) ;
   SS=&Grammar[VV->i_struct].From ;

   s_text_yxf( y1+6, xx+4, 0x0f, "          " );
   if( SS->type==TSTRUCT || SS->type==TSTRUCT1 )
   {   if( SS->type==TSTRUCT  ) s_text_yxf( y1+6, xx+4 , 0x0f, "Структура" );
       if( SS->type==TSTRUCT1 ) s_text_yxf( y1+6, xx+4 , 0x0f, "Структура1" );
       s_text_yxf( y1+7, x2-18, 0x0f, "q3 n_word" );

       for( i=0 ; i<size_y-7 ; i++ )
       {
	  if( Begin_R+i==i_Select && Reg==DREAL ) c=0x70 ; else c=0x07 ;
	  s_text_yxf( y1+7+i, xx+4, c, emptystr(x2-xx-6) );

	  if( SS->Word.j<=Begin_R+i ) continue ;
	  W  =&SS->Word[Begin_R+i] ;
	  R  =Core.rword( V->r_word+Begin_R+i ) ;

	  sprintf( Str1,"%s",W->Name );
	  if( W->type==TCONST ) sprintf( Str1,"@0[%s]",W->literal );
	  if( W->type==TWORD0 ) strcpy( Str1,"@00" );

	  // ----------- нарисовать параметры структуры -----------
	  if( W->type!=TCONST && W->type!=TCONST1 && W->type!=TWORD0 )
	  {
	      V1=Core.variant( R ) ;
	      sprint_param( Str1+strlen(Str1), W );
	      s_text_yxf( y1+7+i, xx+4, c, Str1 );
	      sprintf( Str2,"%d-%d", R.i_word,R.i_last_word );
	      s_text_yxf( y1+8+i, x2-18, c, Str2 );
	  }
	  s_text_yxf( y1+8+i, xx+4, c, Str1 );
       }
   }
   if( SS->type==TSTRUCT2 )
   {   
       t_sStruct *sS ;
       t_sWord   *sW ;
       e_Type    t ;

       sS=Perevod.get_from( V->i_slowo );  
       s_text_yxf( y1+6, xx+4 , 0x0f, "Структура2" );
       s_text_yxf( y1+6, x2-18, 0x0f, "q3 n_word" );

       for( i=0 ; i<size_y-7 ; i++ )
       {
	  if( Begin_R+i==i_Select && Reg==DREAL ) c=0x70 ; else c=0x07 ;
	  s_text_yxf( y1+7+i, xx+4, c, emptystr(x2-xx-6) );

	  if( sS->n_Word<=Begin_R+i ) continue ;
	  sW =&sS->Word[Begin_R+i] ;
	  R  =Core.rword( V->r_word+Begin_R+i ) ;
	  t  =R.type ;

          Str1[0]=0 ;
	  if( sW->i_struct>=0  ) 
	      sprintf( Str1,"%s",Grammar[sW->i_struct].From.Name );	     
	  if( t==TCONST ) sprintf( Str1,"@0[%s]",sW->str );
	  if( t==TCONST1) sprintf( Str1,"@1[%s]",sW->str );
	  if( t==TWORD0 ) strcpy( Str1,"@00" );

	  if( t!=TCONST && t!=TCONST1 && t!=TWORD0 )
	  {
	      V1=Core.variant( R ) ;	      
	      s_text_yxf( y1+8+i, xx+4, c, Str1 );
	      sprintf( Str2,"%d-%d",R.i_word,R.i_last_word );
	      s_text_yxf( y1+7+i, x2-18, c, Str2 ); 
	  }
	  s_text_yxf( y1+7+i, xx+4, c, Str1 );
       }
   }
   if( SS->type==TSELECT || SS->type==TSELECT1 )
   {   if( SS->type==TSELECT  ) s_text_yxf( y1+6, xx+4, 0x0f, "Выбор    " );
       if( SS->type==TSELECT1 ) s_text_yxf( y1+6, xx+4, 0x0f, "Множество" );
       s_text_yxf( y1+6, x2-18, 0x0f, "q3 n_word" );

       if( Reg==DREAL ) c=0x70 ; else c=0x07 ;
       s_text_yxf( y1+7, xx+4, c, emptystr(x2-xx-6) );

       V1 =Core.variant(Current) ;
       R  =Core.rword( V1->r_word ) ;

       if( 0<R.i_struct )
               sprintf( Str1,"%s",Grammar[R.i_struct].From.Name );
	  else strcpy ( Str1,"@0" );
       W=&SS->Word[R.index] ;
       if( W->type==TCONST ) sprintf( Str1,"@0[%s]",W->literal );
       if( W->type==TWORD0 ) strcpy( Str1,"@00" );

       sprint_param( Str1+strlen(Str1), W );
       s_text_yxf( y1+7, xx+4, c, Str1 );

       sprintf( Str1,"%d-%d",R.i_word,R.i_last_word );
       s_text_yxf( y1+7, x2-18, c, Str1 );
   }
}
/***************************************************************************/
//                   нарисовать параметры структуры
/***************************************************************************/
void t_Debug :: sprint_param( char *Str, t_Word *W )
{
   long  i1,i2,i3 ;   
   char *s1 ;

   Str[0]=0 ;
   if( 0<W->Param.j ) strcat( Str,"(" );
   for( i1=0 ; i1<W->Param.j ; i1++ )
   {  i2 = W->Param[i1].param ;
      i3 = W->Param[i1].value ;
      if( i3<0 ) s1 = "@0" ;
	  else   s1 = Grammar.from().Param[i2].Value[i3].Name ;
      strcat( Str," " );
      strcat( Str,s1 );
   }
   if( 0<W->Param.j ) strcat( Str," )" );
}
/***************************************************************************/
e_WinMsg t_Debug :: loop( void )
{
   short  ZZ,key1,key2 ;
   long   i ;
   t_Struct   *SS ;
   e_Type      t ;
   t_Variants *VV ;
   t_rWord     R ;
   t_Form      Form0 ;

   init();
   while( 1 )
   {  paint( );

      s_getch( &key1,&key2 ) ;
      ZZ=s_shiftstatus();
      
      if( key1==0 )
      {
         if( 0!=(ZZ&(S_Shift_L|S_Shift_R)) )
	 {   switch( key2 )
	     {	// -------------- переход в другие окна ------------
		case S_key_F2 : return WM_FIRST ;
		case S_key_F3 : return WM_SRC ;
		case S_key_F4 : return WM_DST ;
		case S_key_F5 : return WM_DEBUG ;
	     }	 
	 }
	 else
	 {   switch( key2 )
	     {  case S_key_F1 : Window_help( (void *)"debug.html") ; continue ;
		case S_key_F2 : return WM_FIRST ;
		case S_key_F3 : return WM_SRC ;
		case S_key_F4 : return WM_DST ;
		case S_key_F5 : return WM_DEBUG ;
        //	case S_key_F6 : return WM_PATH ;
        	case S_key_F10 :return WM_ESC ;
	     }	 
	 }
      }

      if( 0==Core.fn_word() ) continue ;
      switch( Reg )
      {  case DSTRUCT:
	      switch( key1 )
	      {  case S_key_Esc : return WM_ESC ;
	         case S_key_TabR :
		      if( Reg1==D1SELECT ) 
		      {   Reg1=D1STRUCT ; 
		          z_Vision=SelectVision[z_Vision] ;
		      }
		      else
		      {   Reg1=D1SELECT ;		          
		          for( i=0 ; i<SelectVision.j ; i++ )
			     if( z_Vision<=SelectVision[i] )
			     {   z_Vision=i ; break ;  }
		      }
                      break ;
		 case S_key_Enter :
		      VV=Core.variants( Current.i_word,Current.i_struct ) ;
		      if( VV->Variant.j<=0 ) break ;
		      Reg=DVARIANT ; 
		      break ;
		 case 0:
		     switch( key2 )
		     {
			case S_key_Left :
			     if( Current.i_word>0 ) Current.i_word-- ;
			     break ;
			case S_key_Right :
			     if( Current.i_word<Core.from().j-1 ) Current.i_word++ ;
			     break ;
		        case S_key_Up   : z_Vision-- ;         break ;
			case S_key_Down : z_Vision++ ;         break ;
			case S_key_PgUp : z_Vision-=size_y-7 ; break ;
			case S_key_PgDn : z_Vision+=size_y-7 ; break ;
			case S_key_Home : z_Vision=0     ;     break ;
			case S_key_End  : z_Vision=32000 ;     break ;
		     }
	      }
	      // ------- поставить курсоры на место --------------------------
	      if( z_Vision<0 )       z_Vision=0 ;
	      if( z_Vision<Begin_S ) Begin_S=z_Vision ;
	      if( Reg1==D1SELECT )
	      {   if( SelectVision.j<=z_Vision )   z_Vision=SelectVision.j-1 ;
		  if( size_y-7<=z_Vision-Begin_S ) Begin_S=z_Vision-(size_y-7) ;
		  Current.i_struct=SelectVision[z_Vision] ;
	      }
	      else
	      {
		  if( Grammar.n_trans()<=z_Vision )  z_Vision=Grammar.n_trans()-1 ;
		  if( size_y-7<=z_Vision-Begin_S ) Begin_S=z_Vision-(size_y-7) ;
		  Current.i_struct=z_Vision ;
	      }
	      break;
	 case DVARIANT:
	      switch( key1 )
	      {  case S_key_Esc   : return WM_ESC ;
		 case S_key_Enter : 
		      Reg=DREAL ;
		      if( Current.i_struct==Grammar.i_all() )
			  Core.antwort().make( 0, Grammar.i_all(), Current.i_variant, Form0 );
		      break ;
		 case S_key_Back  : Reg=DSTRUCT ; break ;
		 case 0:
		     switch( key2 )
		     {  case S_key_Up :
			     if( 0<Current.i_variant ) Current.i_variant-- ;
			     if( Current.i_variant<Begin_V ) Begin_V=Current.i_variant ;
			     break ;
			case S_key_Down :
		             VV=Core.variants( Current.i_word,Current.i_struct ) ;
			     if( Current.i_variant<VV->Variant.j-1 )
				 Current.i_variant++ ;
			     if( size_y-8<=Current.i_variant-Begin_V ) 
				 Begin_V=Current.i_variant-(size_y-8) ;
			     break ;
		     }
	      }
	      break;
	 case DREAL:
	      switch( key1 )
	      {  case S_key_Esc : return WM_ESC ;
		 case S_key_Back :
		      if( j_stack<=0 ) {  Reg=DVARIANT ; break ;  }
		      Current =stack[--j_stack] ;
		      Begin_R=i_Select=0 ;
		      break ;		      
		 case S_key_Enter :
		      VV=Core.variants( Current.i_word, Current.i_struct );
		      SS=&Grammar[Current.i_struct].From ;
		      if( IF_STRUCT( SS->type ) )
		      {	  t=SS->Word[i_Select].type ;
			  if( t==TCONST ||t==TCONST1 || t==TWORD0 || t==TWORD ) break ;
			  R=Core.rword( VV->Variant[Current.i_variant].r_word+i_Select ) ;
			  stack[j_stack++] = Current ;
			  Current  =R ;
			  i_Select =0 ;
		      }
		      if( IF_SELECT( SS->type ) )
		      {   t=SS->Word[i_Select].type ;
			  if( t==TCONST || t==TWORD0 || t==TWORD ) break ;
			  R=Core.rword( VV->Variant[Current.i_variant].r_word ) ;
			  if( R.i_struct<0 ) break ;
			  stack[j_stack++] = Current ;
			  Current  =R ;
			  i_Select =0 ;
		      }
		      break ;
		 case 0:
		      switch( key2 )
		      {
			 case S_key_Up :
			      if( i_Select>0 ) i_Select-- ;
			      if( i_Select<Begin_R ) Begin_R=i_Select ;
			      break ;
			 case S_key_Down :
			      SS=&Grammar[Current.i_struct].From ;
			      //if( IF_CONSTR( SS->type ) )
			      if( SS->type==TSTRUCT || SS->type==TSTRUCT1 )
			      {   if( i_Select<SS->Word.j-1 )
				      i_Select++ ;
			      }
			      if( size_y-7<=i_Select-Begin_R )
				  Begin_R=i_Select-(size_y-8) ;
			      break ;
		      }
		      break;
	      }
      }
   }
   return WM_ESC ;
}
/***************************************************************************/
//          смотрелка для t_Grammar (таблицы правил перевода)              //
/***************************************************************************/
t_GrammarView :: t_GrammarView( void ) : t_Win()
{   strcpy( Name,"База грамматики" );
}
/***************************************************************************/
void t_GrammarView :: set_pos( short _x1, short _y1, short _x2, short _y2 )
{
   t_Win::set_pos( _x1,_y1,_x2,_y2 );
   n_stack = 0 ;
   i_Select= 0 ;
   Begin_Y = 0 ;
   Half    = FROM ;
}
/***************************************************************************/
void t_GrammarView :: restart( void )
{
   stack[0]=Grammar.i_all(); //i_main_struct ;
   n_stack=1 ;
}
/********************************************************************/
//                 напечатать имена параметров
/********************************************************************/
void sprintf_param( char *Str, t_Param1List &Param )
{
   if( 0<Param.j ) strcat( Str,"(" );
   for( short i=0 ; i<Param.j ; i++ )
   {  strcat( Str," " );
      if( Param[i].Dir!=0 ) strcat( Str,"&" );
      strcat( Str,Param[i].Name );
   }
   if( 0<Param.j ) strcat( Str," )" );

}
/***************************************************************************/
void t_GrammarView :: paint( void )
{  short xx ;

   t_Win::paint();

   xx=x1+(x2-x1)/2 ;

   // ----------------- нарисовать рамки ------------------------
   s_rame2_F( y1, x1  , y2, xx, 0x07 );
   s_rame2_F( y1, xx+1, y2, x2, 0x07 );
   s_text_yxf( y1, x1+3, 0x0a, "Стек" );
   s_text_yxf( y1, xx+3, 0x0a, "Структура" );

   if( Half==FROM )  s_text_yxf( y1, x1+20, 0x0f, "Источник" );
       else          s_text_yxf( y1, x1+20, 0x0f, "Приемник" );

   // ---------------- нарисовать стек --------------------------
   {  short i,i1,b=0,z ;
      char  Str[100] ;
      t_Struct *St ;

      if( (size_y-4)<b+n_stack ) b=n_stack-(size_y-4) ;
      for( i=0 ; i<size_y ; i++ )
      {  if( n_stack<=b+i ) break ;
	 z=stack[b+i] ;
	 if( Half==FROM ) St=&Grammar[z].From ;
	     else         St=&Grammar[z].To ;
	 sprintf( Str,"%3d %s",z,St->Name );
	 // ----------- нарисовать параметры структуры -----------
	 sprintf_param( Str, St->Param );
	 Str[xx-x1-3]=0 ;
	 s_text_yxf( y1+i+1, x1+2, 0x07, Str );
      }
   }
   // ---------------- нарисовать составляющие ------------------
   {  short i,i1 ;
      char  c,Str[100] ;
      t_Struct *St ;
      t_Word   *W ;
      
      if( Half==FROM ) St=&Grammar[stack[n_stack-1]].From ;
	  else         St=&Grammar[stack[n_stack-1]].To ;

      for( i=0 ; i<size_y-3 ; i++ )
      {  if( St->Word.j<=Begin_Y+i ) break ;
         W=&St->Word[Begin_Y+i] ;
	 strcpy( Str,W->Name );
	 if( W->literal[0]!=0 )
	 {   strcat( Str,"[" );
	     strcat( Str,W->literal );
	     strcat( Str,"]" );
	 }
	 
	 // ----------- нарисовать параметры структуры -----------
	 sprintf_param( Str, W->Param );
	 Str[xx-x1-3]=0 ;
	 if( Begin_Y+i==i_Select ) c=0x70 ; else c=0x07 ;
	 s_text_yxf( y1+i+1, xx+3, c, emptystr(xx-x1-3) );
	 s_text_yxf( y1+i+1, xx+3, c, Str );
      }
   }
   paint_hotkey( n_Line-1, 0, hk_Gramm );
}
/********************************************************************/
e_WinMsg t_GrammarView :: loop( void )
{
   short ZZ,key1,key2 ;
   restart();

   while( 1 )
   {
      paint( );

      s_getch( &key1,&key2 ) ;
      ZZ=s_shiftstatus();

      switch( key1 )
      {  case S_key_Esc : return WM_ESC ;
	 case S_key_TabR :
	      if( Half==FROM ) Half=TO ; else Half=FROM ;
	      break ;
	 case S_key_Back :
	      {  if( 1<n_stack ) n_stack-- ;
		 i_Select=0 ;
	      } break ;
	 case S_key_Enter :
	      {  t_Struct *St ;
		 if( Half==FROM ) St=&Grammar[stack[n_stack-1]].From ;
		     else         St=&Grammar[stack[n_stack-1]].To ;
		 t_Word   *W =&St->Word[i_Select] ;
		 if( W->type==TWORD || W->type==TCONST || W->type==TWORD0 )
		     break ;
		 stack[n_stack++]=W->i_struct ;
		 i_Select=Begin_Y=0 ;
	      } break ;
	 case 0:
	 {   switch( key2 )
	     {
		case S_key_Up :
		     if( i_Select>0 ) i_Select-- ;
		     if( i_Select<Begin_Y ) Begin_Y=i_Select ;
		     break ;
		case S_key_Down :
		     {
			t_Struct *St ;
			if( Half==FROM ) St=&Grammar[stack[n_stack-1]].From ;
			    else         St=&Grammar[stack[n_stack-1]].To ;
			if( i_Select<St->Word.j-1 ) i_Select++ ;
			if( size_y-3<=i_Select-Begin_Y ) Begin_Y=i_Select-(size_y-4) ;
		     }
		     break ;
		case S_key_F8 :
		    {  short yy1,yy2,xx1,xx2 ;
		       t_ViewTransParam TransParam( stack[n_stack-1],Half );
		       xx1=(x1+x2-size_x/2)/2 ;
		       xx2=(x1+x2+size_x/2)/2 ;
		       yy1=(y1+y2-size_y/2)/2 ;
		       yy2=(y1+y2+size_y/2)/2 ;

		       s_save( yy1, xx1, yy2, xx2 );
		       s_rame2_F( yy1, xx1, yy2, xx2, 0x0f );
		       s_text_yxf( yy1, xx1+3, name_Color, "Внутренняя передача параметров" );
		       TransParam.set_pos( xx1+1, yy1+1, xx2-1, yy2-1 );
		       TransParam.loop( );
		       s_restore( );
		    }
		    break ;
		case S_key_F10 : return WM_ESC ;
                case S_key_F1 :  Window_help( (void *)"grammar.html") ; continue ;
	     }
	 }
      }
   }
}
/********************************************************************/

/********************************************************************/
t_ViewTransParam :: t_ViewTransParam( short _i_struct, e_Half _Half )
{
   i_struct=_i_struct ; Half=_Half;
}
/********************************************************************/
void t_ViewTransParam :: paint1( short i, char current )
{
   t_Struct   *St ;
   t_Relation  R;
   char        c,*s1,Str[80] ;

   s1=emptystr( size_x );
   s_text_yxf( y1+i,x1+1 ,0x07, s1 );
   if( Half==FROM ) St=&Grammar[i_struct].From ;
       else         St=&Grammar[i_struct].To ;

   MaxY=St->Relation.j ;
   if( MaxY<=Begin+i ) return ;

   if( current ) c=0x70 ; else c=0x07 ;
   R=St->Relation[i] ;
   sprintf( Str," %2d %2d -> %2d %2d",R.s1,R.p1,R.s2,R.p2 );
   s_text_yxf( y1+i,x1+1 ,c, Str );
}
/********************************************************************/
char t_ViewTransParam :: hot_key( short SS, short key1, short key2 )
{
   return 0 ;
}
/********************************************************************/
//       смотрелка для t_Slowo3 словарь структур перевода           //
/********************************************************************/
t_Slowo3View :: t_Slowo3View( void )
{  strcpy( Name,"Словарь переводов" );
}
/********************************************************************/
void t_Slowo3View :: set_pos( short _x1, short _y1, short _x2, short _y2 )
{
   t_Win::set_pos( _x1,_y1,_x2,_y2 );
   n_stack = 0 ;
   i_Select= 0 ;
   Begin_Y = 0 ;
}
/********************************************************************/
void t_Slowo3View :: restart( void )
{
   stack[0]=0 ;
   n_stack=1 ;
}
/********************************************************************/
void t_Slowo3View :: paint( void )
{  short xx ;

   t_Win::paint();
   
   xx=x1+(x2-x1)/2 ;
   // ----------------- нарисовать рамки ------------------------
   s_rame2_F(  y1, x1  , y2, xx, 0x07 );
   s_rame2_F(  y1, xx+1, y2, x2, 0x07 );
   s_text_yxf( y1, x1+3, 0x0a, "Стек" );
   s_text_yxf( y1, xx+3, 0x0a, "Дальше" );

   // ---------------- нарисовать стек --------------------------
   {  long i,b=0,z ;
      char  Str[100] ;
      t_sTree   *T ;

      if( (size_y-4)<b+n_stack ) b=n_stack-(size_y-4) ;
      for( i=0 ; i<size_y ; i++ )
      {  if( n_stack<=b+i ) break ;
	 z=stack[b+i] ;
	 T=Perevod.tree(z) ;
	 if( T->rang<0 || T->empty==2 )
	      sprintf( Str,"%5ld %5ld %2d",z,T->up,T->rang );
	 else sprintf( Str,"%5ld %5ld %2d %s",z,T->up,T->rang,Perevod.word_src(z) );

	 Str[xx-x1-3]=0 ;
	 s_text_yxf( y1+i+1, x1+2, 0x07, Str );
      }
   }
   // ---------------- нарисовать составляющие ------------------
   {  long  i,z ;
      char  c,Str[100],*Str1,*Str2 ;
      t_sTree   *T,*T1 ;
      t_sStruct *S ;
      t_sWord   *W ;
      t_Format1 *Format = Perevod.format() ;
      
      T=Perevod.tree(stack[n_stack-1]) ;

      if( 0<T->n_down)
      {  for( i=0 ; i<size_y-3 ; i++ )
	 {  if( T->n_down<=Begin_Y+i ) break ;

	    if( Begin_Y+i==i_Select ) c=0x70 ; else c=0x07 ;
	    s_text_yxf( y1+i+1, xx+2, c, emptystr(xx-x1-1) );
	    
            z=T->down+Begin_Y+i ;
            T1=Perevod.tree(z) ;
	    S=Perevod.get_from( T1->first );
	    if( S==NULL ) continue ; // вообще-то надо разобраться

	    if( n_stack==1 )
	    {  Str1=Format->get_tag(  0,S->i_struct );
	       Str2=Format->get_name( 0,S->i_struct );
	       sprintf( Str,"%s (%s)",Str1,Str2 );
            }
	    else
	    {  if( T->empty==2 )
	       {   W=&S->Word[T1->rang+1] ;
	           sprintf( Str,"%s",Format->get_tag( 0,W->i_struct ) );
	       }
	       else
	       {   W=&S->Word[T1->rang] ;
		   Str1=Format->get_tag( 0,W->i_struct );
		   sprintf( Str,"%s[%s]",Str1,Perevod.word_src(z) );
	       }
	    }
	    // ----------- нарисовать параметры структуры -----------
	    // ------------------------------------------------------ 
	    Str[xx-x1-4]=0 ;	    
	    s_text_yxf( y1+i+1, xx+2, c, Str );
	 }
      }
      else
      {  for( i=0 ; i<size_y-3 ; i++ )
         {  z=T->first+Begin_Y+i ;
	    if( T->last<=z ) break ;
	    S=Perevod.get_from( z );
	    Str1=Format->get_tag(  0,S->i_struct );
	    //sprintf( Str,"%s[%s]",Str1,Perevod.Mass+S->Word[T->rang+1].sy );
	    sprintf( Str,"%s[%s]",Str1,Perevod.word_src(z) );
	    Str[xx-x1-4]=0 ;
	    if( Begin_Y+i==i_Select ) c=0x70 ; else c=0x07 ;
	    s_text_yxf( y1+i+1, xx+2, c, emptystr(xx-x1-1) );
	    s_text_yxf( y1+i+1, xx+2, c, Str );
	 }  
      }
   }
   paint_hotkey( n_Line-1, 0, hk_Perev );
}
/********************************************************************/
e_WinMsg t_Slowo3View :: loop( void )
{
   short ZZ,key1,key2 ;
   restart();

   while( 1 )
   {
      paint( );

      s_getch( &key1,&key2 ) ;
      ZZ=s_shiftstatus();

      switch( key1 )
      {  case S_key_Esc : return WM_ESC ;
	 case S_key_Back :
	      {  if( 1<n_stack ) n_stack-- ;
	         i_Select=stack[n_stack]-Perevod.tree(stack[n_stack-1])->down ;
		 Begin_Y=i_Select ; // поправить на прежний индекс
	      } break ;
	 case S_key_Enter :
	      {  t_sTree *T=Perevod.tree(stack[n_stack-1]) ;
	         if( T->n_down<=0 ) break ;
		 stack[n_stack++]=T->down+i_Select ;
		 i_Select=Begin_Y=0 ;
	      } break ;
	 case 0:
	 {   switch( key2 )
	     {
		case S_key_Up :
		     if( i_Select>0 ) i_Select-- ;
		     if( i_Select<Begin_Y ) Begin_Y=i_Select ;
		     break ;
                case S_key_PgUp :
		     if( i_Select>size_y-3 ) i_Select-=size_y-3 ;
		     if( i_Select<Begin_Y ) Begin_Y=i_Select ;
		     break ;
		case S_key_Down :
		     if( i_Select<Perevod.tree(stack[n_stack-1])->n_down-1 ) i_Select++ ;
		     if( size_y-3<=i_Select-Begin_Y ) Begin_Y=i_Select-(size_y-4) ;
		     break ;
		case S_key_PgDn :
		     if( i_Select<Perevod.tree(stack[n_stack-1])->n_down-(size_y-3) )
		         i_Select+=(size_y-3) ;
		     if( size_y-3<=i_Select-Begin_Y ) Begin_Y=i_Select-(size_y-4) ;
		     break ;
		     
		case S_key_F10 : return WM_ESC ;
		case S_key_F1 :  Window_help( (void *)"base_enru.html") ; continue ;
	     }
	 }
      }
   }
}
