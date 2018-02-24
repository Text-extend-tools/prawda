/**************************************************************************/
// Внести в словарь переводов род и одушевленность русских существительных
// 
/**************************************************************************/
# include <string.h>

# include <grammar.h>
# include <slowo2.h>
# include <slowo3.h>
# include <core.h>

typedef int  (fSORTFUNC)(const void *val1,const void *val2);

struct t_Struct1
{  char *Word,*Gender ;
};

t_Slowo3     Perevod ;
t_Slowo2    *SlowoF,*SlowoT ;
short       *i_FormatT ; // соответствие часть речи -> SlowoT
short       *i_FormatF ; // соответствие часть речи -> SlowoF
t_Core       Core ;

char      **Str ;
t_Struct1  *Word ;
long        N,n_Word ;

/************************************************************************/
int wordcmp3( const void *A, const void *B )
{
   return strcmp( ((t_Struct1 *)A)->Word,((t_Struct1 *)B)->Word );
}
/***************************************************************************/
long FileToWord( char *File, char ***_Str )
{  long  i,j,L ;
   char *m,**ss ;

   L=file_to_mass( File, &m );

   for( i=0 ; i<L ; i++ )
      if( m[i]==' ' || m[i]=='\n' || m[i]=='\r' || m[i]=='\t' ) m[i]=0 ;
   for( i=j=0 ; i<L ; i++ )
      if( m[i]==0 && m[i+1]!=0 ) j++ ;
   ss=(char **)Calloc(j+1,sizeof(char *));
   j=0 ;
   if( m[0]!=0 ) ss[j++]=m ;
   for( i=0 ; i<L ; i++ )
      if( m[i]==0 && m[i+1]!=0 ) ss[j++]=m+i+1 ;

   *_Str=ss ;
   return j ;
}
/**************************************************************************/
int Qfind( void *Data, long Num, long Size, fSORTFUNC *cmpfunc, void *Object )
{
  int cur;
  int cmp_res;
  int top,bottom ;

  bottom=0 ;
  top   =Num ;

  while( bottom <= top )
  {
     cur=(top+bottom)>>1;

     // Compare to val.
     if((cmp_res=cmpfunc(Object,(char *)Data+cur*Size)) == 0) return cur;

     if( cmp_res > 0)
          bottom=cur+1 ;
     else top=cur-1;
  }
  return -1;        // Not found.
}
/**************************************************************************/
//                 вставить параметры в один словарь
/**************************************************************************/
int main1( char *FileGrammar, char *FileSrc, char *FileDst )
{
   char   r ;
   short  i_sub,i_gender,i_dusha ;
   long   i,i1,i_word,N1 ;

   // --------- начало ------------------------------
   r=Grammar.make_all( FileGrammar );
   if( r<0 ) return r ;
   Perevod.read( FileSrc,&Grammar.format1()[0] );

   // --------- найти нужные параметры -------------
   for( i=0 ; i<Grammar.n_trans() ; i++ )
      if( 0==strcmp(Grammar[i].To.Name,"сущ") )
      {   i_sub=i ; break ;  }
   for( i=0 ; i<Grammar[i_sub].To.Param.j ; i++ )
   {  if( 0==strcmp(Grammar[i_sub].To.Param[i].Name,"Род") )
          i_gender=i ;
      if( 0==strcmp(Grammar[i_sub].To.Param[i].Name,"Душа") )
          i_dusha=i ;
   }
   // --------- проставить параметры ---------------
   for( i=0 ; i<Perevod.n_record() ; i++ )
   {  N1=Perevod.get_n_perevod( i );
      for( i1=0 ; i1<N1 ; i1++ )
      {  
         t_sStruct *S=Perevod.get_to( i,i1 );
         if( S->i_struct!=i_sub ) continue ;

	 // -------- вычислить параметр рода и души ------
	 t_Form Param   =S->Word[0].Param.form() ;
	 char  a,b,a1,b1 ;

	 a=Param.value[i_gender] ;
	 b=Param.value[i_dusha] ;
	 if( 0<=a && 0<=b ) continue ;

	 // ------- найти в словаре Зализняка -------------
	 t_Struct1 W ;
	 W.Word=S->Word[0].str ;
	 i_word=Qfind( Word,n_Word,sizeof(t_Struct1),wordcmp3,&W );
	 if( i_word<0 ) continue ;

	 // ------- вычислить параметры -------------------
	 a1=b1=-1 ;
	 if( 0==strcmp( Word[i_word].Gender,"м"  ) ) {  a1=0 ; b1=0 ;  }
	 if( 0==strcmp( Word[i_word].Gender,"мо" ) ) {  a1=0 ; b1=1 ;  }
	 if( 0==strcmp( Word[i_word].Gender,"ж " ) ) {  a1=1 ; b1=0 ;  }
         if( 0==strcmp( Word[i_word].Gender,"жо" ) ) {  a1=1 ; b1=1 ;  }
	 if( 0==strcmp( Word[i_word].Gender,"с " ) ) {  a1=2 ; b1=0 ;  }
	 if( 0==strcmp( Word[i_word].Gender,"со" ) ) {  a1=2 ; b1=1 ;  }
	 if( 0==strcmp( Word[i_word].Gender,"мн" ) ) {  a1=3 ; b1=0 ;  }
	 if( 0==strcmp( Word[i_word].Gender,"мо-жо")){  a1=0 ; b1=1 ;  }

	 // ------- присвоить параметры --------------------
	 if( a<0 ) Param.value[i_gender]=a1 ;
	 if( b<0 ) Param.value[i_dusha] =b1 ;
	 S->Word[0].Param=Param ;
      }
   }

   // --------- конец ------------------------------
   Perevod.print( FileDst );
   return 0 ;
}
/****************************************************************************/
//
/****************************************************************************/
int main( void )
{
   char  r ;
   long  i ;
   char *FileGrammar, *FileSrc, *FileDst ;

   // --------- прочесть файл ----------------------
   N=FileToWord( "../../dicts/z_knowlege/russkij/ru_sub_gender",&Str );
   for( i=0 ; i<N ; i+=3 )
      if( Str[i+2][0]!=';' )
      {  printf("\n Error Gender Dict"); exit(-1);  }

   n_Word=N/3 ;
   Word=(t_Struct1 *)Calloc( n_Word,sizeof(t_Struct1) );
   for( i=0 ; i<n_Word ; i++ )
   {  Word[i].Word  =Str[3*i] ;
      Word[i].Gender=Str[3*i+1] ;
   }
   // --------- отсортировать его ------------------
   qsort( Word,n_Word,sizeof(t_Struct1),wordcmp3 );
   
   FileGrammar = "../../grammar/english_russian/lang.txt" ;
   FileSrc     = "../../dicts/z_knowlege/perevod/base_enru_s" ;
   FileDst     = "../../dicts/english_russian/base_enru" ;
   
   main1( FileGrammar, FileSrc, FileDst );

   FileGrammar = "../../grammar/esperanto_russkij/lang.txt" ;
   FileSrc     = "../../dicts/z_knowlege/perevod/base_esru_s" ;
   FileDst     = "../../dicts/esperanto_russkij/base_esru" ;

   main1( FileGrammar, FileSrc, FileDst );
}
/****************************************************************************/
t_Core :: t_Core( )
{ ; }
t_Core :: ~t_Core( )
{ ; }
t_Variants * t_Core :: variants( short i_word, long i_struct )
{  return NULL ;  }
void t_Core :: universe( short i_word, long i_struct )
{ ; }
void t_Core :: real_param_up( t_rStruct *V )
{ ; }
char * t_Core :: get_meaning( t_rStruct *V )
{  return NULL ;  }
/****************************************************************************/
t_Antwort :: t_Antwort( void )
{ ; }
/****************************************************************************/
t_rWord :: t_rWord( )
{
   type        = TNULL ;
   i_word      =-1 ;
   i_last_word =-1 ;
   i_struct    =-10;
   i_variant   =-1 ;
   index       =-1 ;
   i_slowo     =-1 ;
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

   Form.init() ;
   i_meaning   =-1 ;
}
/****************************************************************************/
t_Tree :: t_Tree( void )
{  init( );  }
t_Tree :: ~t_Tree( void )
{  del( );  }
/***************************************************************************/
void t_Tree :: init( void )
{
   type  = TSTRUCT ;
   up    =-1 ;
   down  =-1 ;
   n_down= 0 ;
   i_word= 0 ;
   i_last_word=-1 ;
   i_struct=0 ;
   select=-1 ;
   link  =-1 ;
   index =-1 ;
   Form.init();
   i_slowo=-1 ;
   Str[0]=Str1[0]=0 ;
}
void t_Tree :: del( void )
{ ; }
/****************************************************************************/
DEFINE_LIST_BTYPE( t_aWord,t_aWordList )
DEFINE_LIST_BTYPE( t_rStruct,t_rStructList )
DEFINE_LIST_BTYPE( t_rWord,t_rWordList )
DEFINE_LIST_BTYPE( t_Item, t_ItemList )
DEFINE_LIST_TYPE ( t_Tree, t_TreeList )
