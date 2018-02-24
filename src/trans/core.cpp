/***************************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                       главная программа перевода
//     Анисимов Д.В.                                         сегодня
/***************************************************************************/

# include <string.h>
# include <stdlib.h>
# include <unistd.h>
# include <mylib.h>
# include <slowo2.h>
# include <slowo3.h>
# include <grammar.h>
# include <core.h>

# define  Err_Node     -10
# define  Err_NextCh   -11
# define  Err_UniStack -12
# define  Err_MemSize  -13

t_Slowo3     Macro ;     // словарь английских сокращений (препроцессор)
t_Slowo3     Micro ;     // словарь русских сокращений (постпроцессор)
t_Slowo3     DicConst ;  // словарь английских constant
t_Slowo3     Perevod ;   // словарь переводов
t_Slowo2    *SlowoF=NULL ,    // грамматические словари языка-оригинала
            *SlowoT=NULL ;    // грамматические словари языка-перевода
short       *i_FormatT=NULL ; // соответствие часть речи -> SlowoT
short       *i_FormatF=NULL ; // соответствие часть речи -> SlowoF
short        n_SlowoF=0,n_SlowoT=0 ; // размер массивов
t_Core       Core ;
static char  f_data_read=0 ; // данные уже были прочитаны

void  make_dic_const( void );
short find_enum_select( e_Half H, short up_struct, long i_slowo );
char  compare_param( t_Param1List *Param, t_Form *Form );
void  word_source( short i_struct, char *Word, char *Source, e_Half H );
char  is_number( char *Str );
t_Tree *part_of_struct( t_TreeList &Tree, long i_tree, short i_word );
static void print_tree( t_TreeList &Node, char *FileName );
static void make_token( char *Sou, char *Dst );
void  error_main_table( void );

/***************************************************************************/
//                пристыковать к имени каталога имя файла
/***************************************************************************/
char *filename( char *Path, char *File )
{  static char Str[200] ;

   Strcpy( Str,Path,200 );
   if( Str[strlen(Str)-1]!='/' )
      Strcat( Str,"/",200 );
   Strcat( Str,File,200 );
   return Str ;
}
/***************************************************************************/
//                 прочесть все данные (может переименовать?)
//    Dir - корневой каталог проекта
//    DT  - направление перевода
/***************************************************************************/
char read_grammar( char *Dir, t_DirTrans *DT )
{
   t_File *F ;
   long    i ;
   char   *Path="../";
   char    Str[300] ;
   char    r ;
try
{
   if( Dir[0]!=0 ) Path=Dir ;
   z_DirTrans=DT ;
   sprintf( Str,"%s/%s/lang.txt",Path,DT->Dir );
   r=Grammar.make_all( Str );
   if( r<0 ) return r ;
   File_Error=Fopen("mercury.err","w+"); 

   // -------------- чтение словарей пре- и пост- процессора ---------
   {
      t_FileList &File1=Grammar.file_pre() ;
      Macro.del( );
      if( 0<File1.j )
	  if( 0<File1[0].FileName.j )
	      Macro.read( filename( Path,File1[0].FileName[0] ), &(Grammar.format1()[0]) );

      t_FileList &File2=Grammar.file_post() ;
      Micro.del( );
      if( 0<File2.j )
	  if( 0<File2[0].FileName.j )
	      Micro.read( filename( Path,File2[0].FileName[0] ), &(Grammar.format1()[0]) );
   }
   // -------------- сделать "словарь констант" ----------------------
   make_dic_const( );

   // ---------------- прочитать словари источника --------------------
   for( i=0 ; i<n_SlowoF ; i++ )
      SlowoF[i].clr();
   Free( SlowoF ); Free( i_FormatF );
   
   t_Lang &From=Grammar.from();
   n_SlowoF =From.File.j ;
   SlowoF   =(t_Slowo2 *)Calloc( n_SlowoF,sizeof(t_Slowo2) );
   i_FormatF=(short    *)Calloc( From.Part.j,sizeof(short) );
   for( i=0 ; i<From.Part.j ; i++ )
      i_FormatF[i]=-1 ;

   for( i=0 ; i<n_SlowoF ; i++ )
   {  F=&From.File[i] ;
      SlowoF[i].read( filename( Path,F->FileName[0] ), &From, &From.Format[F->i_format] );
      i_FormatF[F->i_part]=i ;
   }
   // ---------------- прочитать словари приемника --------------------
   for( i=0 ; i<n_SlowoT ; i++ )
      SlowoT[i].clr();
   Free( SlowoT ); Free( i_FormatT );

   t_Lang &To=Grammar.to();
   n_SlowoT =To.File.j ;
   SlowoT   =(t_Slowo2 *)Calloc( n_SlowoT,sizeof(t_Slowo2) );
   i_FormatT=(short    *)Calloc( To.Part.j,sizeof(short) );

   for( i=0 ; i<To.Part.j ; i++ )
      i_FormatT[i]=-1 ;

   for( i=0 ; i<n_SlowoT ; i++ )
   {  F=&To.File[i] ;
      SlowoT[i].read( filename( Path,F->FileName[0]), &To, &To.Format[F->i_format] );
      i_FormatT[F->i_part]=i ;
   }

   // -------------- прочитать словари перевода ---------------
   Perevod.read( Path, &Grammar );
   Fclose( File_Error );
   
   if( Read_Error==0 ) 
   {   unlink("mercury.err") ; f_data_read=1 ;  }
   return Read_Error ; // если тут 0, то все в порядке
}
catch( int E )
{
   Read_Error=-1 ;
   Fclose( File_Error );
   return -1 ;
}
}
/***************************************************************************/
//                        перепрочесть все данные
/***************************************************************************/
char reread_grammar( char *Dir, t_DirTrans *DT )
{  char  r ;
   char *Path="../";
   char  Str[1000] ;

   if( Dir[0]!=0 ) Path=Dir ;

# ifndef __WIN__
   sprintf( Str,"g++ -E -o %s/%s/lang.txt -I%s/%s %s/%s/lang.cpp",
             Path,DT->Dir,Path,DT->Dir,Path,DT->Dir );
   system( Str );
# endif
   if( 0==f_data_read )
       return read_grammar( Dir,DT );

   sprintf( Str,"%s/%s/lang.txt",Path,DT->Dir );
   r=Grammar.make_all( Str ) ;
   if( r<0 ) return r ;
   set_formats( );

   return 0 ;
}
/***************************************************************************/
//          сделать словарь для констант, встречаемых в lang.txt
/***************************************************************************/
void make_dic_const( void )
{  t_Struct *S ;
   long      i,i1,L ;
   char     *Mass,Str[100] ;

   // -------- сосчитать длинну потребного текстового массива -------
   for( i=L=0 ; i<Grammar.n_trans() ; i++ )
   {  S=&Grammar[i].From ;
      for( i1=0 ; i1<S->Word.j ; i1++ )
	 if( S->Word[i1].type==TCONST )
             L+=strlen( S->Word[i1].literal )+10 ;
   }
   // -------- заполнить массив словами -----------------------------
   Mass=(char *)Calloc( L+1,sizeof(char) );
   for( i=L=0 ; i<Grammar.n_trans() ; i++ )
   {  S=&Grammar[i].From ;
      for( i1=0 ; i1<S->Word.j ; i1++ )
	 if( S->Word[i1].type==TCONST )
         {   sprintf( Str,"\n@0[%s]=@0[]",S->Word[i1].literal );
	     strcpy( Mass+L,Str );
	     L+=strlen( Str );
         }
   }
   DicConst.make( Mass, &(Grammar.format1()[0]) );
   Free( Mass );
}
/***************************************************************************/
//    проставить словарям ссылки на форматы,
//    порушенные при повторном чтении lang.txt
/***************************************************************************/
void set_formats( void )
{  short i ;
   t_File *F ;
   t_Lang &From = Grammar.from();
   t_Lang &To   = Grammar.to();

   for( i=0 ; i<From.File.j ; i++ )
   {  F=&From.File[i] ;
      SlowoF[i].set_lf( &From, &From.Format[F->i_format] );
   }
   for( i=0 ; i<To.File.j ; i++ )
   {  F=&To.File[i] ;
      SlowoT[i].set_lf( &To, &To.Format[F->i_format] );
   }
}
/***************************************************************************/
t_Core :: t_Core( )
{
   n_struct =0 ;
   n_word   =0 ;
   Variants =NULL ;
   Meaning  =(char *)Calloc( l_Meaning=5000,1 );
   j_Meaning=0 ;
   rWord.init();
   Result=E_NULL ;
   f_Figna=0 ;
   j_VariantMem=0 ;
}
/***************************************************************************/
t_Core :: ~t_Core( )
{  long i,N ;

   N=(long)(n_struct)*(long)(n_word+1) ;
   for( i=0 ; i<N ; i++ )
      Variants[i].Variant.del();
   Free( Variants );
   Free( Meaning );
   rWord.del();
}
/***************************************************************************/
//    конструктор
//    _n_struct - число шаблонов в t_Grammar
//    _n_word   - число слов в переводимой фразе
/***************************************************************************/
void t_Core :: init( short _n_struct, short _n_word )
{  long i,i1,N ;
   t_Variants *VV ;

   N=(long)(n_struct)*(long)(n_word+1) ;
   for( i=0 ; i<N ; i++ )
      Variants[i].Variant.del();
   Free(Variants);

   n_struct=_n_struct ;
   n_word  =_n_word ;
   N=(long)(n_struct)*(long)(n_word+1) ;

   Variants=(t_Variants *)Calloc( N,sizeof(t_Variants) );
   for( i=0 ; i<n_struct ; i++ )
   {  for( i1=0 ; i1<n_word ; i1++ )
      {  VV=variants( i1,i );
         VV->i_struct=i ;
	 VV->i_word  =i1 ;
      }
   }
   rWord.j=0 ;
}
/***************************************************************************/
//                      главная программа перевода
//   Sou - строка источника
//   Dst - строка приемника (должна быть захвачена до вызова)
/***************************************************************************/
void t_Core :: translate( char *_Sou, char *Dst )
{
   long       i,r,r1 ;
   t_rStruct *pV ;
   t_Word     Word ;
   t_Form     Form0 ;

try
{
   Result=E_NULL ;
   Sou=_Sou ;
   if( 0==strncmp(Sou,"\\##",3) )
   {   // ----- фраза не нуждается в переводе --------
       no_brakets( Sou,Dst );
       Strcpy( Antwort.str(),Dst,LFRASA );
       Strcpy( Sou1,Dst,LFRASA );
       Result=E_NO ; 
       return ;
   }
   for( i=0 ; i<LFRASA ; i++ )
      Sou0[i]=Sou1[i]=0 ;

   Antwort.del();

   r =make_from1( Sou );
   r1=make_from2( );
   if( r!=0 || r1!=0 || From.j==0 )
   {  // ----- трансляция по подстрочнику ---------
      if( f_Figna==1 )
          strcpy( Antwort.str(),"#фигня# " );
      Antwort.del();
      Antwort.translate_simple( 0 );
      strcpy( Dst,Antwort.str() );
      Result=E_FIGNA ;   
   }
   else
   {  // ----- нормальная трансляция --------------
      j_VariantMem=0 ;
      universe( 0, Grammar.i_all() );
      next_chance( Dst );
      Result=E_OK ;
   }
   double_variants( "double.txt" );
}
catch( int E )
{
   switch( E )
   {  case Err_Node   :  if( f_Figna==1 ) strcpy( Dst," ### переполнение в make_node ###.");
			    else     goto M_Simple ;
                         break ;
      case Err_NextCh :  if( f_Figna==1 ) strcpy( Dst," ### переполнение в next_chance ###."); 
                            else     goto M_Simple ;
                         break ;
      case Err_UniStack: strcpy( Dst," ### кольцо описаний ###."); break ;
      case Err_MemSize:  strcpy( Dst," ### непотребное потребление памяти ###."); break ;
      case Err_List:     strcpy( Dst," ### ошибка индексации (руки и ноги оторвать такому программисту) ###."); break ;
      default :          strcpy( Dst," ### Какая-то ошибка ###.");
   }   
   Result=E_ERROR ;
   return ;

M_Simple: // трансляция по подстрочнику
   Antwort.del();
   Antwort.translate_simple( 0 );
   strcpy( Dst,Antwort.str() );
   Result=E_FIGNA ;
}
}
/************************************************************************/
//            вытянуть фразу из фигурных скобочек
//            ( надо использовать в make_from1 )
/************************************************************************/
void t_Core :: no_brakets( char *Str, char *Str1 )
{  long i,i1,L ;

   L=strlen( Str );
   for( i=0 ; i<L ; i++ )
      if( Str[i]=='{' ) break ;
   strcpy( Str1,Str+i+1 );
   for( i1=L-i-1 ; 0<=i1 ; i1-- )
      if( Str1[i1]=='}' )
      {   Str1[i1]=0 ; break ;  }
}
/***************************************************************************/
//        разбиение фразы на слова
//        Source - переводимая фраза
/***************************************************************************/
char t_Core :: make_from1( char *Source )
{
   long    i,j,f,L ;
   char    c,c1 ;
   t_Item      W ;
   e_TagType   t ;
   char    sPart[100],Str[100],stmp[2]=" " ;

   for( i=0 ; i<LFRASA ; i++ )
      Sou0[i]=Sou1[i]=0 ;

   From.j=0 ;
   L=strlen( Source );
   if( LFRASA<L ) return 1 ;

   if( 0==strncmp(Source,"\\@@",3) )
           no_brakets( Source,Sou1 );
      else strcpy( Sou1,Source );
   // ------- отделить точку от последнего слова ------------
   i=strlen( Sou1 ) ;
   Sign='.' ;
   for( ; 0<i ; i-- )
   {  c=Sou1[i] ;
      if( c=='}' || c=='.' || c=='!' || c=='?' ) break ;
   }
   if( c=='.' || c=='!' || c=='?' )
   {  Sign=Sou1[i] ;
      i++ ;
      Sou1[i+1]=0 ; Sou1[i]=Sou1[i-1] ; Sou1[i-1]=' ' ;
   }
   // ---- вот тут надо разворачивать сокращения ------
   for( i=f=j=0 ; i<L ; i++ )
   {
      c=Sou1[i] ;
      if( 0==strncmp( Sou1+i,"\\@~",3 ) ) f=1 ;
      if( 0==strncmp( Sou1+i,"\\@=",3 ) ) f=1 ;
      if( 0==strncmp( Sou1+i,"\\#<",3 ) ) f=1 ;
      if( 0==strncmp( Sou1+i,"\\#>",3 ) ) f=1 ;
      if( 0==strncmp( Sou1+i,"\\#" ,2 ) ) f=1 ;

      if( f==1 )
      {   stmp[0]=c ;
          strcat( Sou0,stmp );
      }
      else
      {	  if( c==' ' || c=='\t' || c=='\n' )
	  {   if( 0<j )
              {   Str[j]=0 ; j=0 ;
		  strcat( Sou0, Macro.translate_word_s( Str,-1 ) );
		  strcat( Sou0," " );
	      }
	  }
	  else
	  {  if( j<98 ) Str[j++]=c ;  }
      }
      if( c=='}' )
      {   f=0 ;
          stmp[0]=' ' ;
          strcat( Sou0,stmp );
      }
   }
   Str[j]=0 ;
   strcat( Sou0, Macro.translate_word_s( Str,-1 ) );

   strcpy( Sou1,Sou0 );

   // ------- проверить на корректность тегов ---------
   L=strlen( Sou1 );
   for( i=f=0 ; i<L ; i++ )
   {  c=Sou1[i] ;
      f+=(c=='{') ;
      f-=(c=='}') ;
      if( f==0 && (c=='\t' || c=='\n') )
          Sou1[i]=' ' ;
      if( f==0 && (c=='.' || c=='!' || c=='?' || c==',' || 
                  c=='(' || c==')' || c==';' || c==':' || c=='-' ) )
          Sou1[i]=' ' ;
   }
   if( f!=0 ) return 1 ;

   // ------ сделать массив From ---------------------
   c1=' ' ;
   t=P_FRASA ;
   for( i=0 ; i<L ; i++ )
   {
      f=0 ;
      if( 0==strncmp( Sou1+i,"\\@~",3 ) ){  f=1 ; t=P_STRUCT    ;  }
      if( 0==strncmp( Sou1+i,"\\@=",3 ) ){  f=1 ; t=P_CONST     ;  }
      if( 0==strncmp( Sou1+i,"\\#<",3 ) ){  f=1 ; t=P_TAG_LEFT  ;  }
      if( 0==strncmp( Sou1+i,"\\#>",3 ) ){  f=1 ; t=P_TAG_RIGHT ;  }
      if( f==1 )
      {   Sou1[i]=0 ;
	  i+=2 ;
	  if( t==P_STRUCT || t==P_CONST )
          {   make_token( Sou1+i+1,sPart );
	      W.i_struct=Grammar.find_struct( FROM,sPart );
          }
	  continue ;
      }

      c=Sou1[i] ;
      W.type=t ;
      W.exist=0 ;
      if( t==P_FRASA )
      {   if( c1==' ' && c!=' ' )
	  {   W.Str=Sou0+i ; W.Str1=Sou1+i ; W.z_str=i ; From.add( W );  }
      }
      if( t==P_CONST || t==P_STRUCT )
      {   if( c=='{' )
          {   W.Str=Sou0+i+1 ; W.Str1=Sou1+i+1 ; W.z_str=i+1 ; From.add( W );  }
      }
      if( t==P_VERB )
      {   if( c=='{' )
          {   W.Str=Sou0+i+1 ; W.Str1=Sou1+i+1 ; W.z_str=i+1 ; From.add( W );  }
      }
      if( t==P_TAG_LEFT || t==P_TAG_RIGHT )
      {   if( c=='{' )
          {   W.Str=Sou0+i+1 ; W.Str1=Sou1+i+1 ; W.z_str=i+1 ;
	      if( t==P_TAG_LEFT )
	               W.i_word = From.j ;
		  else W.i_word = From.j-1 ;
              Tag.add( W );
	  }
      }
      if( c==' ' && t==P_FRASA ) Sou0[i]=0 ;
      if( c=='}' )
      {   t=P_FRASA ; Sou0[i]=Sou1[i]=0 ; W.i_struct=-1 ;  }
      c1=c ;
   }
   return 0 ;
}
/************************************************************************/
//           заполнене главной таблицы атомарными элементами
/************************************************************************/
char t_Core :: make_from2( void )
{
   long        i,i1,i2,i3,i4,i_word,N,N1 ;
   short       i_part ;
   char       *Str,*Str1 ;
   t_Lexer     Lexer ;
   long        Ant1[20] ;
   short       part[20] ;
   t_Form      Form[20] ;
   t_longList  Ant2 ;
   t_Variants *VV=NULL ;

   init( Grammar.n_trans(), From.j );

   // ----- определить для каждого слова какой частью речи, оно может быть -----
   for( i_word=0 ; i_word<From.j ; i_word++ )
   {
      e_TagType t=From[i_word].type ;
      if( t==P_STRUCT || t==P_CONST || t==P_VERB )
      {   // ------- спец слова ----------------------------
          short     i_struct=From[i_word].i_struct ;
	  t_Struct *SS=&Grammar[i_struct].From ;
          if( SS->type==TENUM )
	  {   for( i=0 ; i<SS->Word.j ; i++ )
	         add_from( i_word, SS->Word[i].i_struct, t_Form(), -1, -1 );
          }
	  else
	     add_from( i_word, i_struct, t_Form(), -1, -1 );
          From[i_word].exist=1 ;
	  continue ;
      }
      
      Str=From[i_word].Str ;
      if( is_number(Str) )
      {  // -------- проверить, что это числительное -------
	 add_from( i_word, Grammar.i_number(), t_Form(), -1, -1 );
	 continue ;
      }

      // ----------- найти в словарях форм -----------------
      for( i1=0 ; i1<Grammar.from().File.j ; i1++ )
      {  i_part=SlowoF[i1].Format->i_part ;
         N=SlowoF[i1].quest( Str, Ant1, Form );
	 for( i2=0 ; i2<N ; i2++ )
	 {
	    Str1=SlowoF[i1].normal( Ant1[i2] );
	    Perevod.find( Str1,i_part,&Ant2 );
	    if( Ant2.j<=0 ) continue ;

            for( i3=0 ; i3<Ant2.j ; i3++ )
	    {  t_Form        FormPP=Perevod.word_param( Ant2[i3],-1,0 );
	       t_Param1List *Param =&Grammar[i_part].From.Param ;
	       for( i4=0 ; i4<Param->j ; i4++ )
		  if( Param->list[i4].Dir==0 )
	              FormPP.value[i4]=Form[i2].value[i4] ;

	       N1=Perevod.get_n_perevod( Ant2[i3] );
  	       add_from( i_word, i_part, FormPP, Ant2[i3], N1 );
            }
	    From[i_word].exist=1 ;
	 }
      }
      // ---------- найти в главном словаре -----------------
      N=Perevod.part( Str, part );
      for( i2=0 ; i2<N ; i2++ )
      {  t_Form  FormPP ;

         Perevod.find( Str, part[i2], &Ant2 );
	 if( Ant2.j<=0 ) continue ;
         for( i3=0 ; i3<Ant2.j ; i3++ )
	 {
	    VV=variants( i_word,part[i2] );
	    for( i4=0 ; i4<VV->Variant.j ; i4++ )
	       if( Ant2[i3]==VV->Variant[i4].i_slowo ) goto Now_Exist ;
            FormPP =Perevod.word_param( Ant2[i3],-1,0 ) ;

	    N1=Perevod.get_n_perevod( Ant2[i3] );
            add_from( i_word, part[i2], FormPP, Ant2[i3], N1 );
      Now_Exist	:
	    From[i_word].exist=1 ;
	 }
      }

      // ---------- найти в словаре constant -----------------
      DicConst.find( Str, -1, &Ant2 );
      if( 0<Ant2.j )
          From[i_word].exist=1 ;
   }
   // ------ проверить на дикое количество неизвестных слов -----
   for( i=N=0 ; i<From.j ; i++ )
      if( From[i].exist==0 ) N++ ;
   if( 6<N || From.j/2<N )
       return 1 ;

   return 0 ;
}
/**************************************************************************/
//               добавить вариант трансляции слова
//    i_word   - индекс слова в фразе
//    i_struct - часть речи
//    Form     - форма и грамматические параметры слова
//    i_slowo  - индекс слова в словаре переводов
//    i_slowo1 - номер перевода
/**************************************************************************/
void t_Core :: add_from( short i_word, long i_struct, t_Form Form, 
               long i_slowo, short n_slowo )
{  t_Variants *VV ;
   t_rStruct   S ;
   t_rWord     W ;

   VV=variants( i_word, i_struct ) ;
   S.type        = Grammar[i_struct].From.type ;
   S.i_word      = i_word ;
   S.i_last_word = i_word+1 ;
   S.i_struct    = i_struct ;
   S.r_word      = rWord.j ;
   S.Form        = Form ;
   S.i_slowo     = i_slowo ;
   S.i_slowo1    = 0 ;
   S.i_meaning   = -1 ;
   VV->Variant.add(S);
   j_VariantMem++ ;

   W.type        = Grammar[i_struct].From.type ;
   W.i_struct    = i_struct ;
   W.i_variant   = VV->Variant.j-1 ; 
   W.i_word      = i_word ;
   W.i_last_word = i_word+1 ;
   W.index       = 0 ;
   W.i_slowo     = i_slowo ;
   W.i_slowo1    = 0 ;
   W.n_slowo     = n_slowo ;
   rWord.add( W );
   if( 0<=i_slowo )
       set_meaning( &VV->Variant[VV->Variant.j-1], Perevod.word_src1(i_slowo) );
}
/**************************************************************************/
//           разбиение фразы на составные части (рекурсивная)             //
/**************************************************************************/
void t_Core :: universe( short i_word, long i_struct )
{
   t_Variants *VV0,*VV1 ;
   t_rStruct   V1,*pV ;
   t_Tree      N,*pN1 ;
   t_rWord     R,*pR ;
   t_Struct   *SS ;
   t_Word     *W ;
   e_Type      t ;
   static int  sp=0 ;

   long  i,i1,z,i1_struct ;
   char  q ;

   SS=&Grammar[i_struct].From ;

   if( i_struct ==269 && i_word==0 )
       printf("");

   if( n_word<i_word ) return ;
   VV0=variants( i_word,i_struct ) ;
   if( VV0->absend==1 ) return ; // если такую структуру нельзя построить
   if( 0<VV0->Variant.j ) return ; // если уже построена
   if( i_word==0 && i_struct==Grammar.i_all() ) sp=0 ;
   sp++ ;
   if( 150<sp )
   {   char Str[200] ;
       sprintf( Str,"Stack owerflow t_Core::universe( %d %s)",i_word,SS->Name );
       error_set( Str );       
       sp=0 ;
       throw(Err_UniStack);
   }

   if( SS->type==TSTRUCT1  || SS->type==TSTRUCT2 || 
       SS->type==TUNORDER1 || SS->type==TUNORDER2 )
   {   // ---- структура из словаря -----
       Perevod.s_universe( i_word, i_struct );
       sp-- ;
       return ;
   }
   if( SS->type==TSTRUCT || SS->type==TUNORDER )
   {  t_TreeList Node ;
      make_node( Node, i_word, i_struct );

      // ---- собирание вариантов ----------------
      for( i=0 ; i<Node.j ; i++ )
      {  if( Node[i].down!=(-10) ) continue ;
	 V1.i_word     =i_word ;
	 V1.i_last_word=Node[i].i_last_word ;
	 V1.i_struct   =i_struct ;
	 V1.r_word     =rWord.j ;
	 V1.i_slowo    =-1 ;

	 for( i1=0 ; i1<SS->Word.j ; i1++ )
	    rWord.add(R);
	 z=i ;
	 for( i1=SS->Word.j-1 ; 0<=i1 ; i1-- )
	 {  pN1=&Node[z] ;
	    if( pN1->i_struct==Grammar.i_irregular() )
	        continue ;
	    pR =&rWord[V1.r_word+pN1->index] ;
	    pR->type       =pN1->type ;
	    pR->i_word     =pN1->i_word ;
	    pR->i_last_word=pN1->i_last_word ;
	    pR->i_struct   =pN1->i_struct ;
	    pR->i_variant  =pN1->i_variant ;
	    pR->index      =pN1->index ;
	    pR->i_slowo    =pN1->i_slowo ;
	    pR->i_slowo1   =pN1->i_slowo1 ;
	    pR->n_slowo    =pN1->n_slowo  ;
	    z=Node[z].up ;
	 }
	 if( 0<z && Node[z].i_struct==Grammar.i_irregular() )
	     V1.i_var_irreg=Node[z].i_variant ;
	 else
	     V1.i_var_irreg=-1 ;

	 VV0->Variant.add(V1);
	 j_VariantMem++ ;
	 mem_size_test( );
	 // ------ сформировать meaning ----------
	 {
	    char Str[200]="" ;
	    for( i1=0 ; i1<SS->Word.j ; i1++ )
	    {  if( 0!=SS->Word[i1].meaning_use )
	       {  if( Str[0]!=0 ) strcat( Str," " );
	          strcat( Str,get_meaning( variant(rWord[V1.r_word+i1]) ) );
	       }
	    }
	    set_meaning( &VV0->Variant[VV0->Variant.j-1], Str );
	 }
	 real_param_up( &VV0->Variant[VV0->Variant.j-1] );
      }
      if( VV0->Variant.j==0 ) VV0->absend=1 ;
   }
   if( IF_SELECT( SS->type ) )
   {
      for( i=0 ; i<SS->Word.j ; i++ )
      {  // ------- цикл по составляющим выбора ---------
	 t        =SS->Word[i].type ;
	 i1_struct=SS->Word[i].i_struct ;

	 if( t==TCONST || t==TWORD0 )
	 {   if( -1==(q=atom( i_word,SS->Word[i] )) ) continue ;
	     if( SS->type==TSELECT2 && 
	         1==select2_exist( i_word, i_struct, i_word+q ) ) continue ;
	     V1.i_word     = i_word ;
	     V1.i_last_word= i_word+q ;
	     V1.i_struct   = i_struct ;
	     V1.r_word     = rWord.j ;
	     VV0->Variant.add(V1);
	     j_VariantMem++ ;
	     mem_size_test( );
	     R.i_struct    = i1_struct ;
	     R.i_variant   = VV0->Variant.j-1 ;
	     R.i_word      = i_word ;
	     R.i_last_word = i_word+q ;
	     R.index       = i ;
	     rWord.add( R );
	     continue ;
	 }
	 if( t==TWORD || t==TCONST1 )
	 {   if( -1==atom( i_word,SS->Word[i] ) ) continue ;  }
	 else
	 {   universe( i_word, i1_struct );  }

	 VV1=variants( i_word,i1_struct ) ;
	 for( i1=0 ; i1<VV1->Variant.j ; i1++ )
	 {  // ---- цикл по вариантам каждой составляющей -----
	    W =&SS->Word[i] ;
	    pV=&VV1->Variant[i1] ;

	    if( 0!=compare_param( &W->Param, &pV->Form ) ) continue ;	    
	    if( SS->type==TSELECT2 && 
	        1==select2_exist( i_word, i_struct, VV1->Variant[i1].i_last_word ) )
		   continue ;
	    if( W->literal[0]!=0 )          // сравнить литералы
	    {  char *Str=Core.get_meaning( pV ) ;
	       if( 0!=Strcmp( W->literal,Str) )
	           continue ;
	    }
	    V1           =VV1->Variant[i1] ;
	    V1.i_struct  =i_struct ;
	    V1.r_word    =rWord.j ;
	    V1.i_meaning =-1 ;
	    V1.i_var_irreg=-1 ; 
	    VV0->Variant.add(V1);
	    j_VariantMem++ ;
	    mem_size_test( );
	    R.type       =VV1->Variant[i1].type ;
	    R.i_struct   =SS->Word[i].i_struct ;
	    R.i_variant  =i1 ;
	    R.i_word     =V1.i_word ;
	    R.i_last_word=V1.i_last_word ;
	    R.index      =i ;
	    R.i_slowo    =V1.i_slowo ;
	    R.i_slowo1   =V1.i_slowo1 ;
	    rWord.add( R );

	    if( 0!=SS->Word[i].meaning_use )
	    {   char *Str ;
	        Str=get_meaning( variant(rWord[V1.r_word]) );
		// Free memory reading !!!
	        set_meaning( &VV0->Variant[VV0->Variant.j-1], Str );
	    }
	    real_param_up( &VV0->Variant[VV0->Variant.j-1] );
	 }
      }
      // ---- исключение "просто слов" если у нас есть "устойчивое выражение"
      /*  не всегда так можно безнаказанно делать,
          иногда теряются нужные варианты
      char f=0 ;
      long j,n_word ;
      for( i=0 ; i<VV0->Variant.j ; i++ )
      {  t=rWord[VV0->Variant[i].r_word].type ;
         n_word=VV0->Variant[i].i_last_word-VV0->Variant[i].i_word ;
         if( (t==TSTRUCT1 || t==TSTRUCT2 || t==TSELECT1) && 1<n_word )
	 {  f=1 ; break ;  }
      }
      if( f==1 )
      {  for( i=j=0 ; i<VV0->Variant.j ; i++ )
	 {  t=rWord[VV0->Variant[i].r_word].type ;
            n_word=VV0->Variant[i].i_last_word-VV0->Variant[i].i_word ;
            if( (t==TSTRUCT1 || t==TSTRUCT2 || t==TSELECT1) && 1<n_word )
	    {  VV0->Variant[j++]=VV0->Variant[i] ;  }
	 }
	 VV0->Variant.j=j ;
      }
      */
      if( VV0->Variant.j==0 ) VV0->absend=1 ;
   }
   sp-- ;
}
/***************************************************************************/
//                      построение дерева вариантов
//    Node - дерево вариантов перевода фразы
//    Node[i].select используется как счетчик уровня вершины
/***************************************************************************/
void t_Core :: make_node( t_TreeList &Node, short i_word, long i_struct )
{
   t_Variants *VV1 ;
   t_rStruct  *pV ;
   t_Tree      N,*pN ;
   t_Struct   *SS ;
   t_Word     *W ;
   e_Type      t ;
   char       *use=NULL ;

   long  i,i1,i2,z,z_begin,index,i1_word,i1_struct ;

   if( i_word==12 && i_struct==220 )
       printf("");

   SS=&Grammar[i_struct].From ;

   Node.j=0 ;
   // ----- внести нулевую вершину ----------------------
   N.i_struct   =i_struct ;
   N.i_last_word=i_word ;
   N.select     =0 ;
   Node.add(N);

   // ----- внести междометия ---------------------------
   z_begin=0 ;
   if( 0<=Grammar.i_irregular() && i_struct!=Grammar.i_irregular() )
   {
      universe( i_word,Grammar.i_irregular() );
      VV1=variants( i_word,Grammar.i_irregular() ) ;
      for( i=0 ; i<VV1->Variant.j ; i++ )
      {
	 pV=&VV1->Variant[i] ;
	 N.i_struct   =Grammar.i_irregular() ;
	 N.type       = Grammar[N.i_struct].From.type ;
	 N.up         = 0 ;
	 N.i_variant  = i ;
	 N.i_last_word= pV->i_last_word ;
	 N.i_slowo    = pV->i_slowo ;
	 N.i_slowo1   = pV->i_slowo1 ;
	 N.select     =0 ;
	 Node.add(N);
      }
      if( 0<VV1->Variant.j )
          z_begin=1 ;
   }

   if( SS->type==TUNORDER )
       use=(char *)Calloc( SS->Word.j, sizeof(char) );

   // ----- главный цикл построения дерева -------------
   for( i=z_begin ; i<Node.j ; i++ )
   {
      if( 64000<Node.j )
      {   print_tree( Node, "NodeError.txt" );
          throw(Err_Node);
      }
      pN=&Node[i] ;
      if( pN->select==SS->Word.j ) 
      {   pN->down=-10 ; continue ;  }

      i1_word=pN->i_last_word ;
      if( n_word<i1_word ) continue ;

      if( SS->type==TUNORDER )
      {   // ----------- заполнить used -----------------
          for( i2=0 ; i2<SS->Word.j ; i2++ )
             use[i2]=0 ;
          z=i ;
	  while( 0<z )
	  {  if( 0<=Node[z].index )
	         use[Node[z].index]=1 ;
	     z=Node[z].up ;
	  }
      }

      for( i2=0 ; i2<SS->Word.j ; i2++ )
      {
         pN=&Node[i] ; // введено чтобы не читать из перезахваченного массива
         if( SS->type==TSTRUCT  && i2!=pN->index+1 ) continue ;
         if( SS->type==TUNORDER && use[i2]!=0     ) continue ;
	 index=i2 ;
	 //  ----------- основной цикл -----------------
	 t=SS->Word[index].type ;
	 i1_struct=SS->Word[index].i_struct ;
	 N.type     = t ;
	 N.up       = i ;
	 N.i_word   = i1_word ;
	 N.index    = index ;
	 N.i_struct = i1_struct ;
	 N.select   = pN->select+1 ;

	 if( t==TWORD0 )
	 {   N.i_variant  = 0 ;
	     N.i_last_word= i1_word ;
	     Node.add(N);
	     break ;
	 }
	 if( t==TCONST || t==TCONST1 )
	 {   if( -1==atom( i1_word,SS->Word[index] ) ) continue ;
	     N.i_variant  = 0 ;
	     N.i_last_word= i1_word+1 ;
	     Node.add(N);
	     break ;
	 }
	 if( t==TWORD  )
	 {   if( -1==atom( i1_word,SS->Word[index] ) ) continue ;  }
	 else
	 {   universe( i1_word, i1_struct );  }

	 VV1=variants( i1_word,i1_struct ) ;
	 for( i1=0 ; i1<VV1->Variant.j ; i1++ )
	 {  W =&SS->Word[index] ;
	    pV=&VV1->Variant[i1] ;

	    if( 0!=compare_param( &W->Param, &pV->Form ) )
		continue ;
	    if( W->literal[0]!=0 )          // сравнить литералы
	    {  char *Str=Core.get_meaning( pV ) ;
	       if( 0!=Strcmp( W->literal,Str) )
	           continue ;
	    }
	    N.type       = Grammar[N.i_struct].From.type ;
	    N.i_variant  = i1 ;
	    N.i_last_word= pV->i_last_word ;
	    N.i_slowo    = pV->i_slowo ;
	    N.i_slowo1   = pV->i_slowo1 ;
	    Node.add(N);
	 }
      }
   }
   Free(use);
}
/***************************************************************************/
//   попробовать подходит ли атомарный элемент фразы
//   return -1 - не подходит, 0 - пустой выбор, 1 - подходит
/***************************************************************************/
char t_Core :: atom( short i_word, t_Word &Word )
{
   // ---------------- если это пустой выбор --------
   if( Word.type==TWORD0 )
      return 0 ;
   if( From.j<=i_word ) return -1 ;

   if( Word.type==TCONST )
   {  // ---------------- если это константное слово ----------
      if( 0==Strcmp( From[i_word].Str,Word.literal ) )
	  return 1 ;
      return -1 ;
   }

   if( Word.type==TWORD )
   {  // ---------------- если это слово ----------
      t_Variants *VV=variants( i_word,Word.i_struct ) ;

      if( VV->absend==1 ) return -1 ;

      if( From[i_word].exist==0 )
      {  // ----слово отсутствует в словаре - сделать вид, что оно подходит --
         if( 0<=Grammar.i_unknown() )
	 {   t_Struct *S=&Grammar[Grammar.i_unknown()].From ;
	     for( short i=0 ; i<S->Word.j ; i++ )
	        if( Word.i_struct==S->Word[i].i_struct )
		    goto M_Ok;
	     return -1 ;
	 }
   M_Ok: 
         if( VV->Variant.j<1 ) // защита от повторного внесения
             add_from( i_word, Word.i_struct, t_Form(), -1, -1 );
	 return 1 ;
      }

      if( Word.literal[0]!=0 )
      {  // ---- если это литерал, найти начальную форму слова и 
         // ---- проверить на совпадение----
	 char Str1[100] ;	 
	 word_source( Word.i_struct, From[i_word].Str, Str1, FROM );
	 if( 0==Strcmp(Str1,Word.literal ) )
	          return  1 ;
	     else return -1 ;
      }
      else return 1 ;
   }
   return -1 ;
}
/************************************************************************/
//           попытка продолжить оборванный перевод
//           (больно здоровая программа, надо сократить)
//    t_Tree->i_link используется как значения штрафа этой ветки
/************************************************************************/
void t_Core :: next_chance( char *Dst )
{
   long   i,i1,i2,j,z,n_Antwort ;
   long   z_word,z_best,straf,max_straf,n_Var ;
   t_Form     Form0 ;
   t_Tree     N,*pN ;
   t_TreeList Node ;
   t_rWord    R,*pR ;
   t_rStruct  V1,*VV ;    
   t_Variants *VV1 ;

   n_Antwort = Variants[0+Grammar.i_all()].Variant.j ;

   // ---- внести нулевую вершину -------------------------

   N.type    = Grammar[i_main_struct].From.type ;
   N.up      =-1 ;
   N.down    = 1 ;
   N.n_down  = n_Antwort ;
   N.i_word  = 0 ;
   N.i_last_word= 0 ;
   N.i_struct= Grammar.i_all() ;
   N.link    = 0 ;
   N.i_struct= i_main_struct ;
   Node.add( N );

   // -------- определить есть ли "полные" варианты ---------
   for( f_Full=0,i=0 ; i<n_Antwort ; i++ )
   {  VV=variant( 0,Grammar.i_all(),i );
      if( VV->i_last_word==From.j ) {  f_Full=1 ; break ;  }
   }
      
   // ---- занести в дерево "регулярные" варианты --------
   N.type    = Grammar[Grammar.i_all()].From.type ;
   N.up      = 0 ;
   N.down    =-1 ;
   N.n_down  = 0 ;
   N.i_word  = 0 ;
   N.i_struct= Grammar.i_all() ;
   N.link    = 0 ;

   for( i=0 ; i<n_Antwort ; i++ )
   {
      VV=variant( 0, Grammar.i_all(), i );
      if( f_Full==1 && VV->i_last_word<From.j ) continue ;
      N.i_last_word=VV->i_last_word ;
      N.i_variant =i ;
      if( VV->i_last_word==From.j )
          N.down=-10 ;
      Node.add( N );
   }

   // ---- достроить дерево с помощью догадок -------------
   max_straf=1000 ;

   N.down    =-1 ;
   N.n_down  = 0 ;
   N.i_struct= Grammar.i_surmise() ;
   N.link    = 0 ;

   for( i=0 ; i<Node.j ; i++ )
   {
      if( 64000<Node.j )
      {   print_tree( Node, "NodeError1.txt" );
          throw(Err_NextCh);
      }

      if( 0<Node[i].n_down ) continue ; // пропустить вершину, у которой уже есть сыновья
      if( max_straf<Node[i].link ) continue ; // штраф вершины больше "лучшего" варианта

      i1=Node[i].i_last_word ;
      if( From.j<=i1 ) continue ;

      if( max_straf<=Node[i].link+1 ) continue ; // штраф больше "лучшего" варианта
      N.up     = i ;
      N.i_word = i1 ;
      N.link   = Node[i].link+1 ;
      N.down   = 0 ;

      if( 0<=Grammar.i_surmise() )
      {   universe( i1, Grammar.i_surmise() );
	  VV1=variants( i1, Grammar.i_surmise() );
	  n_Var=VV1->Variant.j ;
      }
      else
	 n_Var=0 ;

      if( 0<n_Var )
      {  // -------- внести в дерево все варианты "догадок" ------------
         for( i2=0 ; i2<n_Var ; i2++ )
	 {  N.type        = Grammar[Grammar.i_surmise()].From.type ;
	    N.i_struct    = Grammar.i_surmise();
	    N.i_last_word = VV1->Variant[i2].i_last_word ;
	    N.i_variant   = i2 ;	       
            if( N.i_last_word==From.j )
	    {  N.down=-10 ;
	       if( N.link<max_straf ) max_straf=N.link ;
	    }
	    Node.add( N );
	 }
      }
      else
      {  // ------ если нет ни одной догадки, перевести очередное слово по словарю ------
	 N.i_struct   =Core.struct_of_word( N.i_word ) ;
	 if( 0<=N.i_struct ) 
	 {   N.type = Grammar[N.i_struct].From.type ;
	     universe( i1, N.i_struct );
	     VV1=variants( i1, N.i_struct );
	     N.i_slowo =VV1->Variant[0].i_slowo ;
	     N.i_slowo1=VV1->Variant[0].i_slowo1 ;
	 }
	 else N.type = TCONST ;
	 N.i_last_word=i1+1 ;
	 N.i_variant  =0 ;
         if( N.i_last_word==From.j )
	 {  N.down=-10 ;
	    if( N.link<max_straf ) max_straf=N.link ;
	 }
	 Node.add( N );
      }
   }

   // ---- записать в главную таблицу все полученные варианты ----
   VV1=variants( 0,i_main_struct ) ;
   for( i=0 ; i<Node.j ; i++ )
   {  if( Node[i].down!=(-10) ) continue ;

      long z1=rWord.j ;
      z=i ; j=0 ;
      while( z!=0 )
      {	 z=Node[z].up ;
	 rWord.add(R);
	 j++ ;
      }
      // ----- сам вариант ------------------
      V1.type       =TSTRUCT ;
      V1.i_word     =0 ;
      V1.i_last_word=Node[i].i_last_word ;
      V1.i_struct   =i_main_struct ;
      V1.r_word     =z1 ;
      V1.nr_word    =j ;
      V1.i_slowo    =-1 ;
      VV1->Variant.add(V1);      
      // ----- его составные части ----------
      z=i ;
      for( i1=j-1 ; 0<=i1 ; i1-- )
      {  pN =&Node[z] ;
	 pR =&rWord[V1.r_word+i1] ;	    
	 pR->type       =pN->type ;
	 pR->i_word     =pN->i_word ;
	 pR->i_last_word=pN->i_last_word ;
	 pR->i_struct   =pN->i_struct ;
	 pR->i_variant  =pN->i_variant ;
	 pR->index      =pN->index ;
	 pR->i_slowo    =pN->i_slowo ;
	 pR->i_slowo1   =pN->i_slowo1 ;
	 pR->n_slowo    =pN->n_slowo  ;
	 z=Node[z].up ;
      }
   }
   // ---- найти "лучший путь" --------------
/*
   straf=1000 ;
   for( i=j=0 ; i<Node.j ; i++ )
   {  if( Node[i].down==(-10) )
      {   if( Node[i].link<straf )
	  {  z_best=j ; straf=Node[i].link ;  }	 
	  j++ ;
      }
   }
*/
   //Antwort.make( 0, i_main_struct, z_best, Form0 ) ;
   Dst[0]=0 ;
   Antwort.make( 0, i_main_struct, 0, Form0 ) ;
   Strcat( Dst, Antwort.str(), LFRASA );
   // ---- поставить точку -----------
   z=strlen(Dst);
   if( 0<z ) Dst[z-1]=Sign ;
       else { Dst[0]=Sign ; Dst[1]=0 ;  }
}
/***************************************************************************/
//         сравнить требуемые параметры слова с фактическими
/***************************************************************************/
char t_Core :: Compare_Param( t_rStruct *V, t_Word *Word )
{
   if( Word->type==TWORD &&
       0!=compare_param( &Word->Param, &V->Form ) )
	  return 1 ;
   return 0 ;
}
/***************************************************************************/
char t_Core :: select2_exist( short i_word, short i_struct, short i_last_word )
{  long i ;
   t_Variants *VV=variants( i_word,i_struct ) ;
   for( i=0 ; i<VV->Variant.j ; i++ )
      if( VV->Variant[i].i_last_word==i_last_word ) return 1 ;
   return 0 ;
}
/***************************************************************************/
//  передать параметры вверх на один шаг при построении universe
//  V - конструкция, для которой определяются параметры на основе ее сыновей
/***************************************************************************/
void t_Core :: real_param_up( t_rStruct *V )
{
   long    i,ii ;

   t_rStruct *V1,*V2 ;
   t_Struct  *SS ;
   t_RelationList1  RR ;
   t_Relation R ;

   if( V->i_struct<0 ) return ;
   SS=&Grammar[V->i_struct].From ;
   if( SS->type==TWORD0 || SS->type==TCONST ) return ;

   if( SS->type==TSTRUCT2 )
        RR=Perevod.get_relation( V->i_slowo, -1 );
   else RR=Grammar[V->i_struct].From.Relation ;

   if( IF_STRUCT( SS->type ) ) // похоже, пропущен IF_UNORDER
   {   for( i=0 ; i<RR.j ; i++ )
       {  R=RR.list[i] ;
	  if( R.s1==0 ) V1= V ;
	      else      V1=variant( rWord[V->r_word+R.s1-1] ) ;
	  if( R.s2==0 ) V2= V ;
	      else      V2=variant( rWord[V->r_word+R.s2-1] ) ;
	  V2->Form.value[R.p2] = V1->Form.value[R.p1] ;
       }
   }
   if( IF_SELECT( SS->type ) )
   {
       ii=rWord[V->r_word].index+1 ;
       for( i=0 ; i<RR.j ; i++ )
       {  R=RR.list[i] ;
	  if( R.s1!=ii ) continue ;
	  if( R.s1==0 ) V1=V ;
	      else      V1=variant( rWord[V->r_word] ) ;
	  if( R.s2==0 ) V2=V ;
	      else      V2=variant( rWord[V->r_word] ) ;
	  V2->Form.value[R.p2] = V1->Form.value[R.p1] ;
       }
   }
}
/**************************************************************************/
//      первая попавшаяся часть речи, которой может быть это слово
/**************************************************************************/
long t_Core :: struct_of_word( short i_word )
{  long i ;
   e_Type t ;
   t_Variants *VV ;

   for( i=0 ; i<Core.n_struct ; i++ )
   {  t=Grammar[i].From.type ;
      if( t!=TWORD && t!=TSTRUCT1 && t!=TSTRUCT2 ) continue ;
      VV=Core.variants( i_word, i );
      if( 0<VV->Variant.j ) return i ;
   }
   return -1 ;
}
/***************************************************************************/
//      дать варианты перевода слова i_word как структуры i_struct
/***************************************************************************/
t_Variants * t_Core :: variants( short i_word, long i_struct )
{
   if( i_word<0 || n_word<i_word || i_struct<0 )
       error_main_table();
   return &Variants[ n_struct*i_word+i_struct ] ;
}
/***************************************************************************/
//                      дать один вариант перевода
//              (может быть сделать "культурный" throw() ?)
/***************************************************************************/
t_rStruct * t_Core :: variant(  short i_word, long i_struct, long i_variant )
{
   if( i_word<0 || n_word<i_word || i_struct<0 )
       error_main_table();
   if( Variants[ n_struct*i_word+i_struct ].Variant.j<=i_variant )
       error_main_table();
   return &Variants[ n_struct*i_word+i_struct ].Variant[i_variant] ;
}
/***************************************************************************/
//                      дать один вариант перевода
/***************************************************************************/
t_rStruct * t_Core :: variant( t_rWord R )
{
   if( R.i_word<0 || n_word<R.i_word || R.i_struct<0 )
       error_main_table();
   if( Variants[ n_struct*R.i_word+R.i_struct ].Variant.j<=R.i_variant )
       error_main_table();
   return &Variants[ n_struct*R.i_word+R.i_struct ].Variant[R.i_variant] ;
}
/***************************************************************************/
//                     установить смысл варианту перевода
/***************************************************************************/
void t_Core :: set_meaning( t_rStruct *V, char *Str )
{  long L,LL ;

   if( 0<=V->i_meaning )
   {   printf("\n t_Core :: meaning != NULL !!!"); exit(-1) ;  }
   L=strlen(Str);
   if( L==0 ) return ;

   if( l_Meaning<=j_Meaning+L+1 )
   {   LL=l_Meaning+max(l_Meaning>>2,L+1) ;
       Meaning=(char *)Realloc( Meaning,LL );
       l_Meaning=LL ;
   }
   V->i_meaning=j_Meaning ;
   strcpy( Meaning+j_Meaning, Str );
   j_Meaning+=L+1 ;
}
/***************************************************************************/
//                    прочесть смысл варианта перевода
/***************************************************************************/
char * t_Core :: get_meaning( t_rStruct *V )
{  static char *s="" ;
   static char  s1[1000] ;
   if( V->i_meaning<0 ) return s ;
   Strcpy( s1,Meaning+V->i_meaning,1000 );
   return s1 ;
}
/***************************************************************************/
void t_Core :: double_variants( char *Name )
{  short i,n,i_word,i_struct ;
   char  present[30] ;

   FILE *fw=Fopen( Name,"a+" );
   fprintf( fw,"\n%s",Sou );

   for( i_word=From.j-1 ; 0<=i_word ; i_word-- )
      if( From[i_word].exist==0 )
      {   fprintf( fw,"\n Неизвестные слова" );
          fclose( fw );
          return ;
      }
   for( i_word=From.j-1 ; 0<=i_word ; i_word-- )
   {  for( i_struct=0 ; i_struct<n_struct ; i_struct++ )
      {  t_Variants *VV=&Variants[ n_struct*i_word+i_struct ] ;
         for( i=0 ; i<30 ; i++ )
	    present[i]=0 ;
         for( i=0 ; i<VV->Variant.j ; i++ )
	 {  n=VV->Variant[i].i_last_word-VV->Variant[i].i_word ;
	    if( 30<=n ) continue ;
	    if( 1<present[n] )
	    {
	        fprintf( fw,"\n iw=%2d is=%s",i_word,Grammar[i_struct].From.Name );
	        break ;
	    }
	    present[n]++ ;
	 }
      }
   }
   fclose( fw );
}
/***************************************************************************/
t_Antwort & t_Core :: antwort( )
{
   return Antwort ;
}
/***************************************************************************/
long t_Core :: n_antwort( void )
{
   return Variants[0+i_main_struct].Variant.j ;
}
/***************************************************************************/
long t_Core :: n_reg_antwort( void )
{
   return Variants[0+Grammar.i_all()].Variant.j ;
}
/***************************************************************************/
char t_Core :: sign( void )
{
   return Sign ;
}
/***************************************************************************/
char t_Core :: f_full( void )
{
   return f_Full ;
}
/***************************************************************************/
t_ItemList & t_Core :: from( void )
{
   return From ;
}
/***************************************************************************/
t_ItemList & t_Core :: tag( void )
{
   return Tag ;
}
/***************************************************************************/
t_rWord & t_Core :: rword( long i )
{
   return rWord[i] ;
}
/***************************************************************************/
short t_Core :: fn_word( void )
{
   return n_word ;
}
/***************************************************************************/
e_Result t_Core :: result( void )
{
   return Result ;
}
/***************************************************************************/
char * t_Core :: sou( void )
{
   return Sou ;
}
/***************************************************************************/
void t_Core :: mem_size_test( )
{
   //j_VariantMem++ ;
   long L=//sizeof(t_Variants)*n_struct*n_word +
          sizeof(t_rWord)*rWord.j +
          sizeof(char)*l_Meaning + 
	  sizeof(t_rStruct)*j_VariantMem ;
   if( 100000000<L ) // захвачено больше 100 Мегабайт
   {  error_set( "захвачено больше 100 Мегабайт памяти" );
      throw(Err_MemSize);
   }
}
/***************************************************************************/
//                       сделать вариант ответа
//    i_word    - номер слова
//    i_struct  - номер структуры
//    i_variant - индекс варианта ответа 
/***************************************************************************/
void t_Antwort :: make( short i_word, short i_struct, long i_variant, t_Form Form )
{
   short L ;
   t_rStruct *pV ;

   Str[0]=0 ;
   Tree1.j=Tree2.j=aWord.j=0 ;

   pV=Core.variant( i_word ,i_struct, i_variant );
   make_sou( i_word, i_struct, i_variant );
   make_dst();
   make_to( 0 );
   if( 0<Tree2.j ) Tree2[0].Form=Form ;
   param_from();
   param_transmit();
   param_to( );

   insert_tag();
   print_antwort( 0,1 );
   post_proc(  );
}
/***************************************************************************/
//               построение дерева разбора (Tree1)
//    i_word    - номер слова
//    i_struct  - номер структуры
//    i_variant - индекс варианта ответа
/***************************************************************************/
void t_Antwort :: make_sou( short i_word, short i_struct, long i_variant )
{
   long        i,i1 ;
   e_Type      t ;
   t_Tree      T,*pT ;
   t_Struct   *SS ;
   t_rStruct  *pS ;
   t_sStruct  *sS ;
   t_rWord     W ;

   pS = Core.variant( i_word, i_struct, i_variant ) ;
   W  = Core.rword( pS->r_word ) ;

   // -------- внести в дерево первый элемент ---------------
   T.type      = Grammar[i_struct].From.type ;
   T.i_word    = i_word ;
   T.i_last_word = Core.from().j ;
   T.i_struct  = i_struct ;
   T.i_variant = i_variant ;
   T.select    = W.index ;
   T.i_slowo   = W.i_slowo ;
   T.i_var_irreg=pS->i_var_irreg ;

   Tree1.j=0 ;
   Tree1.add(T);

   // -------- внести все последующие элементы ---------------
   for( i=0 ; i<Tree1.j ; i++ )
   {
      pT=&Tree1[i] ;
      pT->down=Tree1.j ;
      t=pT->type ;

      if( t==TWORD || t==TCONST || t==TCONST1 || t==TWORD0 )
	  continue ;
      pS=Core.variant( pT->i_word, pT->i_struct, pT->i_variant ) ;
      SS=&Grammar[pT->i_struct].From ;
      pT->i_var_irreg=pS->i_var_irreg ; // возможно надо перенести до предыдущего if

      if( IF_STRUCT( t ) || t==TUNORDER || t==TUNORDER1 )
      {
          if( pT->i_struct==i_main_struct )
	  {   long n ;
	      n=Tree1[i].n_down=pS->nr_word ;
	      for( i1=0 ; i1<n ; i1++ )
	      {	 set_tree( &T, pS, i1 );
		 T.up = i ;
		 Tree1.add(T);
	      }
	  }
	  else
	  {
	     if( t==TSTRUCT || t==TSTRUCT1 || t==TUNORDER || t==TUNORDER1 )
	     {   // ------ нормальная структура ---------
		 pT->i_slowo     = pS->i_slowo ;
		 Tree1[i].n_down =SS->Word.j ;
		 for( i1=0 ; i1<SS->Word.j ; i1++ )
		 {  set_tree( &T, pS, i1 );
		    T.up = i ;
		    Tree1.add(T);
		 }
	     }
	  }
	  if( t==TSTRUCT2 )
	  {   // ------ структура из словаря ------
	      sS=Perevod.get_from( pS->i_slowo );
	      pT->i_slowo     = pS->i_slowo ;
	      pT->n_down      = sS->n_Word ;
	      for( i1=0 ; i1<sS->n_Word ; i1++ )
	      {   set_tree( &T, pS, i1 );
	          T.up = i ;
	          Tree1.add(T);
	      }
	  }
      }
      if( IF_SELECT( t ) )
      {
	  Tree1[i].n_down = 1 ;
	  set_tree( &T, pS, 0 );
	  T.up = i ;
	  Tree1[i].select   =Core.rword( pS->r_word ).index ;
	  Tree1.add(T);
      }
   }
}
/**************************************************************************/
//    присвоить должные значения полям t_Tree, нужные для make_sou()
//    T  - элемент дерева, который присваивается
//    pS - структура, которая соответствует элементу Tree1[T->up]
//    i  - номер каким сыном является T по отношению к Tree1[T->up]
/**************************************************************************/
void t_Antwort :: set_tree( t_Tree *T, t_rStruct *pS, long i )
{  t_rWord   W  = Core.rword( pS->r_word+i ) ;
   t_Struct *SS = &Grammar[pS->i_struct].From ;

   if( SS->type==TSTRUCT || SS->type==TSTRUCT1 )
   {   if( pS->i_struct!=i_main_struct )  // по-моему уже неверно
            T->type  = SS->Word[i].type ;
       else T->type  = W.type ;
   }
   if( SS->type==TUNORDER || SS->type==TUNORDER1 )
       T->type  = W.type ;
   if( SS->type==TSELECT || SS->type==TSELECT1 || SS->type==TSELECT2 )
       T->type  = SS->Word[W.index].type ;
   if( SS->type==TSTRUCT2 )
       T->type  = W.type ;
   if( SS->type==TWORD )
   {   printf("\n Error"); throw(-1);  }

   T->down        =-1 ;
   T->n_down      = 0 ;
   T->i_word      = W.i_word ;
   T->i_last_word = W.i_last_word ;
   T->i_struct    = W.i_struct ;
   T->i_variant   = W.i_variant ;
   T->Form        = pS->Form ;
   T->link        =-1 ;
   T->select      =-1 ;
   T->i_slowo     = W.i_slowo ;
   T->i_slowo1    = W.i_slowo1 ;
   T->n_slowo     = W.n_slowo ;
}
/**************************************************************************/
//         Построение дерева приемников по дереву источников              //
/**************************************************************************/
void t_Antwort :: make_dst( void )
{  long       i,i1,i2,is,n ;
   t_Trans   *Tr ;
   t_Tree     TT,*T2 ;
   t_sStruct *S1,*S2 ;
   e_Type     t ;

   Tree2.j=0 ;
   TT.i_struct=is=Tree1[0].i_struct ;   
   if( 0<=is && (Grammar[is].From.type==TSTRUCT1 || Grammar[is].From.type==TSTRUCT2 ) )
   {   S2=Perevod.get_to( Tree1[0].i_slowo,Tree1[0].i_slowo1 );
       TT.i_struct=S2->i_struct ;
   }
   TT.up=-1 ;
   TT.type=Tree1[0].type ;
   TT.down=1 ;
   TT.n_down=0 ;
   TT.select=0 ;
   TT.i_word=0 ;
   TT.link  =0 ;
   TT.Form.init();
   Tree2.add( TT );
   Tree1[0].link=0 ;

   for( i=0 ; i<Tree2.j ; i++ )
   {  T2=&Tree2[i] ;
      t=T2->type ;
      if( t==TWORD0 || t==TWORD || t==TCONST || t==TCONST1 )
	  continue ;
      Tr=&Grammar[ T2->i_struct ] ;
      T2->down  =Tree2.j ;

      if( 0<=T2->link )
      {	  T2->i_slowo    =Tree1[T2->link].i_slowo ;
	  T2->i_slowo1   =Tree1[T2->link].i_slowo1 ;
	  T2->i_variant  =Tree1[T2->link].i_variant ;
      }

      if( t==TSTRUCT2 || t==TSTRUCT1 )
      {
	  S2=Perevod.get_to( T2->i_slowo,T2->i_slowo1 );
	  T2->n_down   =S2->n_Word ;
	  for( i2=0 ; i2<S2->n_Word ; i2++ )
	     make_dst1( i, i2, NULL );
      }
      if( t==TSTRUCT || t==TUNORDER )
      {
          if( 0<=T2->link )
	      T2->i_var_irreg=Tree1[T2->link].i_var_irreg ;      

          if( T2->i_struct==i_main_struct )
	  {  T2->n_down   =n=Tree1[T2->link].n_down ;
	     for( i2=0 ; i2<n ; i2++ )
		make_dst3( i, i2 );
	  }
	  else
          {  T2->n_down   =Tr->To.Word.j ;
	     for( i2=0 ; i2<Tr->To.Word.j ; i2++ )
		make_dst1( i, i2, Tr );
          }
      }
      if( t==TSELECT || t==TSELECT2 )
      {   T2->n_down   =1 ;
	  make_dst1( i, Tree1[T2->link].select, Tr );
      }
      if( t==TSELECT1 )
      {   T2->n_down   =1 ;
	  T2->select=find_enum_select( TO, T2->i_struct, T2->i_slowo );
	  make_dst1( i, T2->select, Tr );
      }
   }
}
/**************************************************************************/
//            добавление одного элемента в дерево приемников
//            (почти точное надмножество make_dst2)
//    up - индекс родителя, к которому добавляется новый потомок
//    Tr - шаблон, соответствующий Tree2[up]
//    i2 - индекс слова в структуре-приемнике
/**************************************************************************/
void t_Antwort :: make_dst1( long up, long i2, t_Trans *Tr )
{
   t_Tree     TT,*pT ;
   t_Struct  *StTo ;
   t_sStruct *S1,*S2 ;
   long       i_link,i3,z ;
   e_Type     type,up_type ;

   TT.up=up ;
   up_type=Tree2[up].type ;
   i_link =Tree2[up].link ;

   if( up_type==TSTRUCT1 || up_type==TSTRUCT2 )
   {   // -------- родительская структура из словаря ---------------
       i3=Tree1[i_link].i_slowo ;
       S1=Perevod.get_from( i3 );
       S2=Perevod.get_to( i3,Tree1[i_link].i_slowo1 );

       TT.i_struct=S2->Word[i2].i_struct ;
       if( TT.i_struct<0 )
       {   TT.type=type=TCONST ;
	   TT.link=-1 ;
       }
       else
       {  // ------ проставить i_link --------------
          TT.type=type=Grammar[TT.i_struct].To.type ;
	  for( i3=0,z=-1 ; i3<S1->n_Word ; i3++ )
	     if( S1->Word[i3].i_struct==S2->Word[i2].i_struct &&
		 S1->Word[i3].order   ==S2->Word[i2].order ) 
		 {  z=i3 ; break ;  }
	  if( z<0 ) TT.link=-1 ;
	      else  TT.link= Tree1[i_link].down+z ;
       }
       TT.Form  =S2->Word[i2].Param.form() ;
   }
   else
   {   // -------- родительская структура из грамматики -----------
       StTo=&Tr->To ;
    
       TT.i_struct=StTo->Word[i2].i_struct ;
       type   =StTo->Word[i2].type ;

       // ------ проставить i_link --------------
       TT.link=-1 ;
       if( (up_type==TSTRUCT || up_type==TUNORDER ) && 0<=Tr->Relation2[i2] )
	   TT.link=Tree1[i_link].down+Tr->Relation2[i2] ;

       if( IF_SELECT( up_type ) )
	   TT.link=Tree1[i_link].down ;

      // ------ прописать параметры --------------
      for( i3=0 ; i3<StTo->Word[i2].Param.j ; i3++ )
	 TT.Form.value[i3]=StTo->Word[i2].Param[i3].value ;
   }
   if( 0<=TT.link )
       Tree1[TT.link].link=Tree2.j ;

   // ------ проставить все остальное ---------------------------
   if( type==TCONST || type==TCONST1 || type==TWORD0 )
   {   TT.type  =type ;
       TT.select=0 ;
   }
   else
   {   TT.type  =Grammar[TT.i_struct].To.type ;
       if( 0<=TT.link )
       {  
	  TT.select  =Tree1[TT.link].select ;
	  TT.i_slowo =Tree1[TT.link].i_slowo ;
	  TT.i_slowo1=Tree1[TT.link].i_slowo1 ;
       }
   }
   TT.down  = 0 ;
   TT.n_down= 0 ;
   TT.i_word=-1 ;
   TT.index = i2 ;

   Tree2.add( TT );
}
/***************************************************************************/
//    добавление одного элемента в дерево приемников
//    если родитель i_main_struct
//    up - индекс родителя, к которому добавляется новый потомок
//    i2 - индекс слова в структуре-приемнике
/***************************************************************************/
void t_Antwort :: make_dst3( long up, long i2 )
{
   t_Tree TT,*pT1 ;
   long z ;

   z=Tree1[Tree2[up].link].down+i2 ;
   pT1=&Tree1[z] ;
   TT.up      = up ;
   TT.type    = pT1->type ;
   TT.link    = z ;
   Tree1[TT.link].link=Tree2.j ;
   TT.i_struct= pT1->i_struct ;
   TT.select  = pT1->select ;
   TT.i_slowo = pT1->i_slowo ;
   TT.i_slowo1= pT1->i_slowo1 ;
   TT.down    = 0 ;
   TT.n_down  = 0 ;
   TT.i_word  =-1 ;
   TT.index   = i2 ;
   //TT.Form  =S2->Word[i2].Param.form() ;
   Tree2.add( TT );
}
/***************************************************************************/
//            задание строковых значений дереву приемников
/***************************************************************************/
void t_Antwort :: make_to( long i_tree )
{  long       i ;
   t_Tree    *T,*T1 ;
   t_Struct  *SS ;
   t_sStruct *S ;
   char      Str1[100],*Str2,Str3[100] ;

   T=&Tree2[i_tree] ;
   if( T->type==TWORD0 )
       return ;
   if( T->i_struct==i_main_struct )
   {   for( i=0 ; i<T->n_down ; i++ )
          make_to( T->down+i );
       return ;
   }
   if( 0<T->i_struct )
       SS=&Grammar[T->i_struct].To ;

   if( T->type==TWORD )
   {   if( T->link<0 ) return ;
       if( T->Str1[0]!=0 ) return ;
       T1=&Tree1[T->link] ;
       word_source( T1->i_struct, Core.from()[T1->i_word].Str, Str1, FROM );
       Str2=Perevod.translate_word_i( Str1, T1->i_slowo, T1->i_slowo1 );       
       if( Str2!=NULL )
       {   word_source( T1->i_struct, Str2, Str3, TO );
           Strcpy( T->Str1, Str3, LWORD );
       }
       else
           Strcpy( T->Str1, Str1, LWORD );
   }
   if( T->type==TCONST || T->type==TCONST1 )
   {   t_Struct *S1=&Grammar[Tree2[T->up].i_struct].To ;
       if( Tree2[T->up].i_struct==i_main_struct  )
               Strcpy( T->Str1, Core.from()[Tree1[T->link].i_word].Str, LWORD );
          else Strcpy( T->Str1, S1->Word[T->index].literal, LWORD );
   }
   if( IF_SELECT( T->type ) )
   {
       if( SS->Word[T->select].literal[0]!=0 )
	   Strcpy( Tree2[T->down].Str1, SS->Word[T->select].literal, LWORD );
       if( SS->Word[T->select].meaning_use!=0 && T->Str1[0]!=0 )
           Strcpy( Tree2[T->down].Str1, T->Str1, LWORD );
       make_to( T->down );
   }
   if( IF_STRUCT( T->type ) || IF_UNORDER( T->type ) )
   {   if( T->type==TSTRUCT || T->type==TUNORDER )
       {   for( i=0 ; i<T->n_down ; i++ )
	   {  T1=&Tree2[T->down+i] ;
	      if( SS->Word[i].literal[0]!=0 )
	          Strcpy( T1->Str1, SS->Word[i].literal, LWORD );
	      if( SS->Word[i].meaning_use!=0 && T->Str1[0]!=0 )
	          Strcpy( T1->Str1, T->Str1, LWORD );
	      make_to( T->down+i );
           }
       }
       else
       {   // ---- структура из словаря --------------------------
	   S=Perevod.get_to( T->i_slowo,T->i_slowo1 );
	   for( i=0 ; i<S->n_Word; i++ )
	   {  T1=&Tree2[T->down+i] ;
	      if( T1->type==TWORD )
	      {   if( S->Word[i].str[0]==0 )
		  {   make_to( T->down+i );  }
		  else
		  {   word_source( S->Word[i].i_struct, S->Word[i].str, Str1, TO );
	              Strcpy( T1->Str1, Str1, LWORD );
		  }
              }
	      if( T1->type==TCONST || T1->type==TCONST1 )
	          Strcpy( T1->Str1, S->Word[i].str, LWORD );
	      if( IF_CONSTR( T1->type ) )
	      {   if( S->Word[i].str[0]!=0 )
	              Strcpy( T1->Str1, S->Word[i].str, LWORD );
	          make_to( T->down+i );
	      }
	   }
       }
   }
}
/***************************************************************************/
//             Пословный перевод, если разбор не удался
//    Begin - на каком слове надорвался штатный алгоритм
/***************************************************************************/
char * t_Antwort :: translate_simple( short Begin )
{  short     i_word,z_begin ;
   t_ItemList &From=Core.from();

   z_begin=strlen( Str );
   if( Begin<0 ) Begin=0 ;
   for( i_word=Begin ; i_word<From.j ; i_word++ )
      translate_simple1( i_word );

   strcat( Str,"." );
   return Str+z_begin ;
}
/***************************************************************************/
//           перевод одного слова, если разбор не удался
/***************************************************************************/
char * t_Antwort :: translate_simple1( short i_word )
{  short     i_slowar,i_struct,z_begin,N ;
   t_Slowo2 *Slowo ;
   t_Form    Form1[20] ;
   char     *Str0,*Str1 ;
   long      Ant1[20] ;
   t_ItemList &From=Core.from();

   i_slowar=i_struct=-1 ;   
   z_begin=strlen( Str );
   if( From[i_word].exist )
   {  i_struct=Core.struct_of_word( i_word ) ;
      if( 0<=i_struct )
	  i_slowar=i_FormatF[i_struct] ;
   }

   if( 0<=i_slowar )
   {  Slowo=&SlowoF[ i_slowar ];
      N=Slowo->quest( From[i_word].Str, Ant1, Form1 );
   }
   else N=0 ;
   if( 0<N ) Str1=Slowo->normal(Ant1[0]) ;
       else  Str1=From[i_word].Str ;

   Str0=Perevod.translate_word_s( Str1, i_struct );
   strcat( Str,Str0 );
   strcat( Str," " );
   return Str+z_begin ;
}
/***************************************************************************/
/*     вычислить параметры структур на основании сыновей (в источнике)     */
/***************************************************************************/
void t_Antwort :: param_from( void )
{
   long    i,i1 ;
   t_Tree *F,*F1,*F2 ;
   t_rStruct *V ;
   t_RelationList1 RR ;
   t_Relation R ;

   for( i=Tree1.j-1 ; 0<=i ; i-- )
   {  F=&Tree1[i] ;
      if( F->type==TWORD0 || F->type==TCONST || F->type==TCONST1 ) continue ;
      if( F->type==TSTRUCT2 )
              RR=Perevod.get_relation( F->i_slowo, -1 );
	 else RR=Grammar[F->i_struct].From.Relation ;

      if( F->type==TWORD )
      {   // -------- по хорошему это надо через Grammar делать ----
	  V=Core.variant( F->i_word,F->i_struct,F->i_variant );
	  F->Form=V->Form ;
	  continue ;
      }
      for( i1=0 ; i1<RR.j ; i1++ )
      {  R=RR.list[i1] ;
	 if( IF_SELECT( F->type ) && (R.s1-1)!=F->select ) continue ;
	 F1=part_of_struct( Tree1, i, R.s1 );
	 F2=part_of_struct( Tree1, i, R.s2 );
	 F2->Form.value[R.p2] = F1->Form.value[R.p1] ;
      }
      // ------- вычисление параметров по таблице источника -----
      t_Trans  *Trans=&Grammar[ Tree1[i].i_struct ] ;
      for( i1=0 ; i1<Trans->Table.j ; i1++ )
      {  t_Table *Table=&Trans->Table[i1] ;
         if( T_SOU==Table->type )
	     param_transmit1( Table, i, -1 );
      }
   }
}
/**************************************************************************/
//               Передача параметров источник - приемник                  //
/**************************************************************************/
void t_Antwort :: param_transmit( void )
{  short     i,i1 ;

   for( i=0 ; i<Tree2.j ; i++ )
   {
      if( Tree2[i].link<0 ) continue ;

      e_Type t=Tree2[i].type ;
      if( t==TCONST || t==TCONST1 || t==TWORD0 || t==TWORD0 ) continue ;

      t_Trans  *Trans=&Grammar[ Tree2[i].i_struct ] ;

      for( i1=0 ; i1<Trans->Table.j ; i1++ )
      {  t_Table *Table=&Trans->Table[i1] ;
         if( T_TRANS==Table->type )
	     param_transmit1( Table, Tree2[i].link, i );
      }
   }
}
/**************************************************************************/
//    Передача параметров с помощью таблицы
//    Table - таблица
//    i_Tree1 - вершина в дереве-источнике 
//    i_Tree1 - вершина в дереве-приемнике (если -1 значит индекс не нужен)
/**************************************************************************/
void t_Antwort :: param_transmit1( t_Table *Table, long i_Tree1, long i_Tree2 )
{  short     i_str,i2,i_word ;
   char      f_sou,f_dst,f_trans ;
   t_Tree   *Dst,*Sou,*T1 ;

   if( 0<=i_Tree1 ) Sou=&Tree1[i_Tree1] ;
   if( 0<=i_Tree2 ) Dst=&Tree2[i_Tree2] ;

   f_sou  =T_SOU  ==Table->type ;
   f_dst  =T_DST  ==Table->type ;
   f_trans=T_TRANS==Table->type ;

   for( i_str=0 ; i_str<Table->Value.j ; i_str+=Table->Size )
   {  // --------- цикл по строчкам в таблице --------------
      for( i2=0 ; i2<Table->In.j ; i2++ )
      {  // ---------- найти строчку, соответствующую вычисленным
	 // ---------- значениям параметров ------------
	 i_word=Table->In[i2].i1 ;
	 if( Table->In[i2].i2<0 ) // ------- когда параметр - строка @Выбор'а
	 {  if( f_sou || f_trans ) {  if( (Sou->select+1)!=Table->Value[i_str+i2] ) goto M_Next ;  }
	    if( f_dst )            {  if( (Dst->select+1)!=Table->Value[i_str+i2] ) goto M_Next ;  }
	 }
	 else
	 {  if( f_sou || f_trans ) T1=part_of_struct( Tree1, i_Tree1, i_word );
      	    if( f_dst )            T1=part_of_struct( Tree2, i_Tree2, i_word );

	    if( T1->Form.value[Table->In[i2].i2]!=Table->Value[i_str+i2] && 
		0<=Table->Value[i_str+i2] ) goto M_Next ;
	 }
      }
      for( i2=0 ; i2<Table->Out.j ; i2++ )
      {  // ---------- записать параметры в приемник
	 i_word=Table->Out[i2].i1 ;
	 if( f_sou )             T1=part_of_struct( Tree1, i_Tree1, i_word );
	 if( f_dst || f_trans )  T1=part_of_struct( Tree2, i_Tree2, i_word );

	 T1->Form.value[Table->Out[i2].i2]=Table->Value[i_str+Table->In.j+i2] ;
      }
      M_Next: ;
   }
}
/***************************************************************************/
//                    Передача параметров в приемнике                      //
/***************************************************************************/
void t_Antwort :: param_to( void )
{
   long    i,i1,i2 ;
   short   v ;
   t_Tree    *T,*T1,*T2 ;
   t_Struct  *SS ;
   t_sStruct *sS ;
   t_RelationList1 RR ;
   t_Relation R ;
   t_Form     Form ;

   for( i=Tree2.j-1 ; 0<=i ; i-- )
   {  T=&Tree2[i] ;
      if( T->type==TSTRUCT1 || T->type==TSTRUCT2 )
          T->Form=Perevod.struct_param( T->i_slowo, T->i_slowo1 );
   }
   // ------------- проход снизу вверх ------------------
   for( i=Tree2.j-1 ; 0<=i ; i-- )
   {
      T=&Tree2[i] ;
      if( T->i_struct<0 ) continue ;
      
      SS=&Grammar[T->i_struct].To ;
      if( T->type==TSTRUCT2 )
               RR=Perevod.get_relation( T->i_slowo, T->i_slowo1 );
	  else RR=SS->Relation ;

      // ------------- вычисление постоянных параметров ----
      if( T->type==TWORD )
      {
	  if( 0<=T->link )
	  {   // ----- up - нормальная структура -------
	      Form=Perevod.word_param( T->i_slowo, T->i_slowo1 ,0 );
	  }
	  else
	  {   // ----- up - TSTRUCT1 или TSTRUCT2 ------
	      t_Tree *T_up=&Tree2[T->up] ;
	      Form=Perevod.word_param( T_up->i_slowo, T_up->i_slowo1 , T->index );
	  }
	  for( i1=0 ; i1<SS->Param.j ; i1++ )
	     if( SS->Param[i1].Dir==1 )
		 T->Form.value[i1]=Form.value[i1] ;
      }
      // ------------- вычисление по таблицам приемника -------
      t_Trans  *Trans=&Grammar[ T->i_struct ] ;

      for( i1=0 ; i1<Trans->Table.j ; i1++ )
      {  t_Table *Table=&Trans->Table[i1] ;
         if( T_DST==Table->type )
	     param_transmit1( Table, T->link, i );
      }
      // ------------- передача параметров наверх ----------
      for( i1=0 ; i1<RR.j ; i1++ )
      {  R=RR.list[i1] ;
	 if( R.s2!=0 ) continue ;
	 if( SS->Param[R.p2].Dir!=0 ) continue ; // ---- лишнее условие ?
	 if( IF_SELECT( T->type ) && (R.s1-1)!=T->select ) continue ;
	 T1=part_of_struct( Tree2, i, R.s1 );
	 T2=part_of_struct( Tree2, i, R.s2 );
	 T2->Form.value[R.p2] = T1->Form.value[R.p1] ;
      }
   }
   // ------------- проход сверху вниз ----------------
   for( i=0 ; i<Tree2.j ; i++ )
   {
      T=&Tree2[i] ;
      if( T->i_struct<0 ) continue ;

      t_Trans  *Trans=&Grammar[ T->i_struct ] ;

      for( i1=0 ; i1<Trans->Table.j ; i1++ )
      {  t_Table *Table=&Trans->Table[i1] ;
         if( T_DST==Table->type )
	     param_transmit1( Table, T->link, i );
      }

      SS=&Grammar[T->i_struct].To ;
      if( T->type==TSTRUCT2 )
               RR=Perevod.get_relation( T->i_slowo, T->i_slowo1 );
	  else RR=SS->Relation ;

      for( i1=0 ; i1<RR.j ; i1++ )
      {  R=RR.list[i1] ;
	 if( R.s2==0 ) continue ;
	 if( R.s1==0 )
	 {   // ------- передача параметров от вышестоящей структуры -------
	     if( SS->Param[R.p1].Dir==0 ) continue ; // вроде лишнее условие
	     if( IF_SELECT( T->type ) && (R.s2-1)!=T->select ) continue ;
	 }
	 T1=part_of_struct( Tree2, i, R.s1 );
	 T2=part_of_struct( Tree2, i, R.s2 );
	 T2->Form.value[R.p2] = T1->Form.value[R.p1] ;
      }
      // -------- задание константных параметров ----------
      if( IF_SELECT( T->type ) )
      {   // ---------- для выборов ------------
	  i1=T->select ;
	  for( i2=0 ; i2<SS->Word[i1].Param.j ; i2++ )
	  {  v=SS->Word[i1].Param[i2].value ;
	     if( 0<=v )
	         Tree2[T->down].Form.value[i2]=v ;
	  }
      }
      if( T->type==TSTRUCT || T->type==TSTRUCT1 )
      {   // --------- и для структур ----------
          t_Form F ;
          if( T->type==TSTRUCT1 ) 
	  {  sS=Perevod.get_to( T->i_slowo );
	     F=sS->Word[i1].Param.form();
	  }
	  for( i1=0 ; i1<SS->Word.j ; i1++ )
	  {
	     for( i2=0 ; i2<SS->Word[i1].Param.j ; i2++ )
	     {  v=-1 ;
	        if( T->type==TSTRUCT1 ) 
		{   if( SS->Word[i1].Param[i2].Dir==1 )		    
		       v=F.value[i2] ;
		}
		else  v=SS->Word[i1].Param[i2].value ;
		if( 0<=v )
		    Tree2[T->down+i1].Form.value[i2]=v ;
	     }
	  }
      }
   }
}
/***************************************************************************/
//     выдать адрес части(слова) структуры
//     Tree         - дерево
//     Tree[i_tree] - та самая структура
//     i_word       - индекс части (если i_word==0 возвращается 
//                    сама Tree[i_tree])
/***************************************************************************/
t_Tree *part_of_struct( t_TreeList &Tree, long i_tree, short i_word )
{
   t_Tree *T=&Tree[i_tree] ;
   if( i_word==0 ) return T ;
   else
   {  if( IF_SELECT( T->type ) )
	       return &Tree[T->down] ;
	  else return &Tree[T->down+i_word-1] ;
   }
}
/***************************************************************************/
//           развешать теги (как сопли) на дерево приемника
/***************************************************************************/
void t_Antwort :: insert_tag()
{  long   i,i1,i_word ;
   e_TagType  t ;
   t_Tree    *T ;

   for( i=0 ; i<Core.tag().j ; i++ )
   {  t=Core.tag()[i].type ;
      i_word=Core.tag()[i].i_word ;
      if( t==P_TAG_LEFT )
      {   for( i1=0 ; i<Tree2.j ; i1++ )
          {  T=&Tree2[i1] ;
	     if( IF_WORD(T->type) && 0<=T->link && i_word<=Tree1[T->link].i_word )
             {  T->left_tag =i ; break ;  }
	  }
      }
      if( t==P_TAG_RIGHT )
      {   for( i1=Tree2.j-1 ; 0<=i1 ; i1-- )
          {  T=&Tree2[i1] ;
	     if( IF_WORD(T->type) && 0<=T->link && Tree1[T->link].i_word<=i_word )
             {  T->right_tag =i ; break ;  }
	  }
      }
   }
}
/***************************************************************************/
//                     напечатать ответ (рекурсивная)
//    i_tree - индекс в массиве Tree2
//    fword  - если 0 - не вносить слово в ответ (нужно для STRUCT2)
/***************************************************************************/
# define F_LEFT_TAG  if( 0<=T->left_tag  ) strcat( Str,Core.tag()[T->left_tag].Str );
# define F_RIGHT_TAG if( 0<=T->right_tag ) strcat( Str,Core.tag()[T->right_tag].Str );

void t_Antwort :: print_antwort( long i_tree, char fword )
{  t_Tree    *T ;
   t_Slowo2  *Slowo ;
   short      i ;
   static short sp=0 ;

   sp++ ;
   if( 80<sp )
   {  printf("\n Стек исчерпался"); exit(-1);  }

   T=&Tree2[i_tree] ;
   // --------- внесение междометий ------------------------
   if( 0<=T->i_var_irreg )
   {
      t_Antwort    Antwort1 ;
      t_aWordList *Word ;
      t_Form       Form0 ;
      
      Antwort1.make( Tree1[T->link].i_word, Grammar.i_irregular(), T->i_var_irreg, Form0 );
      Word=Antwort1.aword();
      for( i=0 ; i<Word->j ; i++ )
         aWord.add( Word->list[i] );
      Strcat( Str,Antwort1.str(),LFRASA ); 
   }

   if( T->type==TCONST || T->type==TCONST1 )
   {
      F_LEFT_TAG
      strcpy( T->Str,T->Str1 );
      Strcat( Str,T->Str,LFRASA ); Strcat( Str," ",LFRASA );
      add_word( *T,fword );
      F_RIGHT_TAG
   }
   if( T->type==TWORD )
   {
      F_LEFT_TAG
      if( 0<=i_FormatT[T->i_struct] )
      {  Slowo=&SlowoT[ i_FormatT[T->i_struct] ];
	 Strcpy( T->Str, Slowo->form( T->Str1,&T->Form),LWORD );
      }
      else
      {  strcpy( T->Str, T->Str1 );
      }
      Strcat( Str,T->Str,LFRASA ); Strcat( Str," ",LFRASA );
      add_word( *T,fword );
      F_RIGHT_TAG
   }
   // ---------- заполнить и оттранслировать сыновей --------
   if( IF_STRUCT( T->type ) || IF_UNORDER( T->type ) )
   {   // ---- возможно, что тут еще надо теги вставлять
       if( Core.f_Figna==1 && T->i_struct==i_main_struct && 1<T->n_down )
           Strcat( Str,"#фигня# ",LFRASA );

       for( i=0 ; i<T->n_down ; i++ )
       {  if( Core.f_Figna==1 && T->i_struct==i_main_struct && 0<i )
              Strcat( Str,"## ",LFRASA );
	  print_antwort( T->down+i,fword );
       }
       if( T->type==TSTRUCT1 || T->type==TSTRUCT2 )
          if( 0==Perevod.is_atom(T->i_slowo) )
	  {   F_LEFT_TAG
              add_word( *T,fword );
	      F_RIGHT_TAG
          }
   }
   if( IF_SELECT( T->type ) )
   {  short z_str =strlen( Str );
      short z_word=aWord.j ;
      print_antwort( T->down,fword && (T->type!=TSELECT2 ) );
      if( T->type==TSELECT2 && fword )
      {   add_word( *T,1 );
          Strcpy( aWord[z_word].Str,Str+z_str,LWORD );
      }
   }
   sp-- ;
}
# undef F_LEFT_TAG
# undef F_RIGHT_TAG
/***************************************************************************/
//           добавть слово к массиву псевдослов ответа
//    fword  - если 0 - не вносить слово в ответ (нужно для STRUCT2)
/***************************************************************************/
void t_Antwort :: add_word( t_Tree &T, char fword )
{
   t_aWord W,W1 ;
   t_Tree  *T_up ;

   if( 0<=T.up ) T_up=&Tree2[T.up] ;
       else return ;

   if( (T_up->type==TSTRUCT1 || T_up->type==TSTRUCT2) &&
          0==Perevod.is_atom(T_up->i_slowo) ) return ;
   if( fword==0 ) return ;

   W.meaning=W1.meaning=-1 ;
   if( 0<T.link )
   {   W.i_word      = Tree1[T.link].i_word ;
       W.i_last_word = Tree1[T.link].i_last_word ;
   }
   else
   {   W.i_word      = -1 ;
       W.i_last_word = -1 ;
   }
   W.i_struct    = T.i_struct ;
   W.i_slowo     = T.i_slowo ;
   if( 0<=T.i_slowo ) W.n_slowo = Perevod.get_n_perevod( T.i_slowo );
       else           W.n_slowo = 1 ;
   W.Form        = T.Form ;
   W.Str[0]      = 0 ;
   if( IF_WORD( T.type ) )
       strcpy( W.Str,T.Str1 );

   W1.i_word     =W.i_word ;
   W1.i_last_word=W.i_last_word ;

   // ----- внесение слова, и прикрепленных к нему тегов ----
   if( 0<=T.left_tag  )
   {   Strcpy( W1.Str,Core.tag()[T.left_tag].Str,LWORD );
       aWord.add( W1 );
   }
   aWord.add( W );
   if( 0<=T.right_tag  )
   {   Strcpy( W1.Str,Core.tag()[T.right_tag].Str,LWORD );
       aWord.add( W1 );
   }

}
/***************************************************************************/
//                       постпроцессор ответа
/***************************************************************************/
void t_Antwort :: post_proc( void )
{
    long i,i1,i2,j ;
    t_sStruct *S ;
    t_longList Ant ;
    char c,Str1[LFRASA] ;

    c=' ' ;
    for( i=j=0 ; i<LFRASA && Str[i]!=0 ; i++ )
    {  if( !(Str[i]!=' ' && c==' ' ) ) goto M_Normal ;
       Micro.find( Str+i,-1,&Ant );
       if( Ant.j<=0 ) goto M_Normal ;
       // ------ внести новые слова ---------------
       S=Micro.get_to( Ant[0],0 );
       for( i1=0 ; i1<S->n_Word ; i1++ )
       {  for( i2=0 ; i2<LFRASA && S->Word[i1].str[i2]!=0 ; i2++ )
	     Str1[j++]=S->Word[i1].str[i2] ;
          Str1[j++]=' ' ;
       }
       // ------ убрать старые слова ---------------
       S=Micro.get_from( Ant[0] );
       i2=0 ;
       for( ; i<LFRASA && Str[i]!=0 ; i++ )
       {  if( Str[i]!=' ' && c==' ' ) i2++ ;
          if( S->n_Word<i2 ) break ;
	  c=Str[i] ;
       }
       // возможно здесь i увеличится лишний раз
       continue ;
M_Normal:
       Str1[j++]=c=Str[i] ;
    }
    Str1[j]=0 ;
    Strcpy( Str,Str1,LFRASA );
}
/***************************************************************************/
char * t_Antwort :: str( void )
{
   return Str ;
}
/***************************************************************************/
t_aWordList * t_Antwort :: aword( void )
{
   return &aWord ;
}
/***************************************************************************/
t_TreeList & t_Antwort :: tree1( void )
{
   return Tree1 ;
}
/***************************************************************************/
t_TreeList & t_Antwort :: tree2( void )
{
   return Tree2 ;
}
/***************************************************************************/
//                    найти начальную форму слова
//     i_struct - часть речи
//     Word     - слово
//     Source   - начальная форма слова
/***************************************************************************/
void word_source( short i_struct, char *Word, char *Source, e_Half H )
{
   t_Slowo2 *Slowo ;
   long     Ant1[20] ;
   t_Form   Form1[20] ;
   short    i_slowar,N ;

   i_slowar=-1 ;
   if( H==FROM )
   {  if( 0<=i_struct ) i_slowar=i_FormatF[i_struct] ;
      if( 0<=i_slowar )
      {  Slowo=&SlowoF[ i_slowar ];
         if( 0<=Slowo->word_exist( Word ) ) // если слово в начальной форме, то выход
	     goto M_OK ;
	 N=Slowo->quest( Word, Ant1, Form1 );
	 if( 0<N ) {  strcpy( Source,Slowo->normal(Ant1[0]) ); return ;  }
      }
   }
   else
   {  if( 0<=i_struct ) i_slowar=i_FormatT[i_struct] ;
      if( 0<=i_slowar )
      {  Slowo=&SlowoT[ i_slowar ];
         if( 0<=Slowo->word_exist( Word ) ) // если слово в начальной форме, то выход
	     goto M_OK ;
	 N=Slowo->quest( Word, Ant1, Form1 );
	 if( 0<N ) {  strcpy( Source,Slowo->normal(Ant1[0]) ); return ;  }
      }
   }
M_OK:
   strcpy( Source,Word );
}
/**************************************************************************/
//   найти в конструкции типа TSELECT1 выбор соответствующий строке словаря
//   up_struct - номер структуры
//   i_slowo   - номер строки словаря
/**************************************************************************/
short find_enum_select( e_Half H, short up_struct, long i_slowo )
{  t_Struct  *S0 ;
   t_sStruct *S1 ;
   short i ;

   if( H==FROM )
   {  S0=&Grammar[up_struct].From ;
      S1=Perevod.get_from( i_slowo );
   }
   else
   {  S0=&Grammar[up_struct].To ;
      S1=Perevod.get_to( i_slowo );
   }
   for( i=0 ; i<S0->Word.j ; i++ )
      if( S0->Word[i].i_struct==S1->i_struct )
	  return i ;
   return -1 ;
}
/**************************************************************************/
//            проверить, что данная строка является числом
/**************************************************************************/
char is_number( char *Str )
{
   for( short i=0 ; i<99 && Str[i]!=0 ; i++ )
      if( Str[i]<'0' || '9'<Str[i] ) return 0 ;
   return 1 ;
}
/**************************************************************************/
//            сравнить параметры 
//    Param - что должно быть согласно lang.txt
//    Form  - что есть фактически
/**************************************************************************/
char compare_param( t_Param1List *Param, t_Form *Form )
{  short i ;

   for( i=0 ; i<Param->j ; i++ )
   {  if( Param->list[i].value==-1 ) continue ;
      if( Form->value[i]==-1       ) continue ;
      if( Param->list[i].value!=Form->value[i] ) return -1 ;
   }
   return 0 ;
}
/**************************************************************************/
t_Antwort :: t_Antwort( void )
{
   Str[0]=0 ;
}
/**************************************************************************/
void t_Antwort :: del( void )
{  Tree1.del();
   Tree2.del();
   aWord.del();
   Str[0]=0 ;
}
/***************************************************************************/
t_rWord :: t_rWord( )
{
   type        = TNULL ;
   i_word      =-1 ;
   i_last_word =-1 ;
   i_struct    =-10;
   i_variant   =-1 ;
   index       =-1 ;
   i_slowo     =-1 ;
   i_slowo1    = 0 ;
   n_slowo     = 0 ;
}
/***************************************************************************/
t_rStruct :: t_rStruct( void )
{
   type        = TNULL ;
   i_word      =-1 ;
   i_last_word =-1 ;
   i_struct    =-10;
   r_word      =-1 ;
   i_slowo     =-1 ;
   i_var_irreg =-1 ;
   i_meaning   =-1 ;
   Form.init() ;
}
/***************************************************************************/
t_Tree :: t_Tree( void )
{  init( );  }
t_Tree :: ~t_Tree( void )
{  del( );  }
/***************************************************************************/
void t_Tree :: init( void )
{
   type    = TSTRUCT ;
   up      =-1 ;
   down    =-1 ;
   n_down  = 0 ;
   i_word  = 0 ;
   i_last_word=-1 ;
   i_struct=0 ;
   select  =-1 ;
   link    =-1 ;
   index   =-1 ;
   i_slowo =-1 ;
   i_slowo1= 0 ;
   i_var_irreg=-1 ;
   left_tag=-1 ;
   right_tag=-1 ;
   Form.init();
   Str[0]=Str1[0]=0 ;
}
void t_Tree :: del( void )
{ ; }
/***************************************************************************/
static void p_type( char *Str, e_Type type )
{  char *s1 ;

   switch( type )
   {
       case TNULL    : s1="TNULL" ;    break ;
       case TCONST   : s1="TCONST" ;   break ;
       case TCONST1  : s1="TCONST1" ;  break ;
       case TWORD    : s1="TWORD" ;    break ;
       case TWORD0   : s1="TWORD1" ;   break ;
       case TSTRUCT  : s1="TSTRUCT" ;  break ;
       case TSTRUCT1 : s1="TSTRUCT1" ; break ;
       case TSTRUCT2 : s1="TSTRUCT2" ; break ;
       case TSELECT  : s1="TSELECT" ;  break ;
       case TSELECT1 : s1="TSELECT1" ; break ;
       case TSELECT2 : s1="TSELECT2" ; break ;
   }
   strcpy( Str,s1 );
}
/***************************************************************************/
//           вывести дерево вариантов в Файл (для отладки)
/***************************************************************************/
void print_tree( t_TreeList &Node, char *FileName )
{  long  i ;
   char  s2[20] ;
   FILE *fw ;

   fw=fopen( FileName,"wb" );

   fprintf( fw, "\n  i   type   - up down n_down -  iw i_last Name i_var - select link index i_slowo");
   for( i=0 ; i<Node.j ; i++ )
   {
      p_type( s2,Node[i].type );
      fprintf( fw, "\n %2ld %8s - %2ld %4ld %4ld -  %2d %2d %12s %2ld -  %4d %4ld %4d %4ld",
              i,
	      s2,
	      Node[i].up,
	      Node[i].down,
	      Node[i].n_down,
	      
	      Node[i].i_word,
	      Node[i].i_last_word,
	      Grammar[Node[i].i_struct].From.Name,
	      Node[i].i_variant,
	      
	      Node[i].select,
	      Node[i].link,
	      Node[i].index,
	      Node[i].i_slowo );
   }
   fclose( fw );
}
/************************************************************************/
void make_token( char *Sou, char *Dst )
{  long i,j ;
   for( i=0 ; Sou[i]==' ' ; i++ ) ;
   for( j=0 ; Sou[i]!=0 && Sou[i]!=' ' ; i++ )
      Dst[j++]=Sou[i] ;
   Dst[j]=0 ;
}
/************************************************************************/
void error_main_table( void )
{   printf("\n MainTable :: Error index !!!\n"); exit(-1) ;  }
/************************************************************************/
DEFINE_LIST_BTYPE( t_aWord,t_aWordList )
DEFINE_LIST_BTYPE( t_rWord,t_rWordList )
DEFINE_LIST_BTYPE( t_rStruct,t_rStructList )
DEFINE_LIST_BTYPE( t_Item, t_ItemList )
DEFINE_LIST_TYPE ( t_Tree, t_TreeList )
