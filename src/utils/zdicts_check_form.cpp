/**************************************************************************/
//   Проверить грамматические словари на полноту
//   И напечатать все слова, которые отсутсвуют в грамматических словарях
/**************************************************************************/
# include <string.h>
# include <stdlib.h>
# include <unistd.h>
# include <mylib.h>
# include <grammar.h>
# include <slowo2.h>
# include <slowo3.h>
# include <core.h>

t_Slowo3     Macro ;     // словарь английских сокращений
t_Slowo3     DicConst ;  // словарь английских constant
t_Slowo3     Perevod ;
t_Slowo2    *SlowoF,*SlowoT ;
short       *i_FormatT ; // соответствие часть речи -> SlowoT
short       *i_FormatF ; // соответствие часть речи -> SlowoF
short        n_SlowoF=0,n_SlowoT=0 ; // размер массивов
t_Core       Core ;
static char  f_data_read=0 ; // данные уже были прочитаны
FILE        *fw ;

char  *ru_part[]={ "сущ","прил","глагол",NULL };
char  *en_part[]={ "сущ",       "глагол",NULL };
char  *es_part[]={ "сущ","прил","глагол",NULL };

int   main1( char *Prefix1, char *Prefix2, char **Part1, char **Part2 );
void  my_grep( char *File, char *Str );
void  Grep( char *File1, char *File2, char *Str1 );
long  FileToString( char *File, char ***_Str );
long  FileToWord( char *File, char ***_Str );
void  WordsSort1( char *File1, char *File2 );

/************************************************************************/

/************************************************************************/
int main( void )
{  t_DirTrans DT ;

   unlink( "../../dicts/z_knowlege/tmp/absend" );
   fw=Fopen( "../../dicts/z_knowlege/tmp/absend","w" );
   
   read_grammar( "../../grammar/english_russian/lang.txt",&DT );
   main1( "en_","ru_",en_part,ru_part );

   read_grammar( "../../grammar/esperanto_russkij/lang.txt",&DT );
   main1( "es_","ru_",es_part,ru_part );
   
   Fclose( fw );

   my_grep( "../../dicts/z_knowlege/absend/ru_sub", "ru_сущ" );
   my_grep( "../../dicts/z_knowlege/absend/ru_adj", "ru_прил" );
   my_grep( "../../dicts/z_knowlege/absend/ru_verb","ru_глагол" );
   my_grep( "../../dicts/z_knowlege/absend/en_sub", "en_сущ" );
   my_grep( "../../dicts/z_knowlege/absend/en_verb","en_глагол" );
   my_grep( "../../dicts/z_knowlege/absend/es_sub", "es_сущ" );
   my_grep( "../../dicts/z_knowlege/absend/es_adj", "es_прил" );
   my_grep( "../../dicts/z_knowlege/absend/es_verb","es_глагол" );
}
/***************************************************************************/
//                        прочесть все данные 
/***************************************************************************/
char read_grammar( char *FileGrammar, t_DirTrans *DT )
{
   t_File *F ;
   long   i,N ;
   char   Filename[200] ;
   char   Path[]="../../" ;
   char   r ;
try
{
   r=Grammar.make_all( FileGrammar );
   if( r<0 ) return r ;
   File_Error=Fopen("mercury.err","w+"); 

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
   Fclose( File_Error );
   return -1 ;
}
}
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
//
/***************************************************************************/
int main1( char *Prefix1, char *Prefix2, char **Part1, char **Part2 )
{  long i,i1,i2,j,i_struct,i_dict ;
   t_sStruct *S ;
   long   Ant[20] ;
   t_Form Form[20] ;
   
   for( i=0 ; i<Perevod.n_record() ; i++ )
   {  S=Perevod.sstruct( Perevod.record(i)->sy_struct ) ;
      for( i1=0 ; i1<S->n_Word ; i1++ )
      {  i_struct=S->Word[i1].i_struct ;
         if( i_struct<0 ) continue ;
	 if( Grammar[i_struct].From.type!=TWORD ) continue ;
	 i_dict=i_FormatF[i_struct] ;
	 if( i_dict<0 ) continue ;
	 for( i2=0 ; Part1[i2]!=NULL ; i2++ )
	    if( 0==strcmp(Grammar[i_struct].From.Name,Part1[i2]) )
	        goto M_OK1 ;
	 continue ;
  M_OK1: if( 0>=SlowoF[i_dict].quest( S->Word[i1].str,Ant,Form ) )
	     fprintf( fw, "\n%s%s %s",Prefix1,Part1[i2],S->Word[i1].str );
      }   
   }
   
   for( i=0 ; i<Perevod.n_record() ; i++ )
   {
      for( j=1 ; j<Perevod.record(i)->n_struct ; j++ )
      {  S=Perevod.sstruct(Perevod.record(i)->sy_struct+j) ;
	 for( i1=0 ; i1<S->n_Word ; i1++ )
	 {  i_struct=S->Word[i1].i_struct ;
            if( i_struct<0 ) continue ;
	    if( Grammar[i_struct].From.type!=TWORD ) continue ;
	    i_dict=i_FormatT[i_struct] ;
	    if( i_dict<0 ) continue ;
	    for( i2=0 ; Part2[i2]!=NULL ; i2++ )
	       if( 0==strcmp(Grammar[i_struct].To.Name,Part2[i2]) )
	           goto M_OK2 ;
	    continue ;
    M_OK2 : if( 0>=SlowoT[i_dict].quest( S->Word[i1].str,Ant,Form ) )
		fprintf( fw,"\n%s%s %s",Prefix2,Part2[i2],S->Word[i1].str );
	 }   
      }
   }

   return 0 ;
}
/****************************************************************************/
void my_grep( char *File, char *Str1 )
{  long   i,i1,j,z,N ;
   char **Str ;

   Grep( "../../dicts/z_knowlege/tmp/absend","../../dicts/z_knowlege/tmp/tmp.txt",Str1 );
   N=FileToString( "../../dicts/z_knowlege/tmp/tmp.txt", &Str );

   FILE *fw=Fopen( "../../dicts/z_knowlege/tmp/tmp1.txt","w" );
   for( i=0 ; i<N ; i++ )
   {  for( i1=j=z=0 ; Str[i][i1]!=0 ; i1++)
         if( Str[i][i1]==' ' ) 
	 {  j++ ; z=i1 ;  }
      if( 1<j ) continue ;
      fprintf( fw,"\n%s",Str[i]+z );
   }
   Fclose( fw );
   WordsSort1( "../../dicts/z_knowlege/tmp/tmp1.txt", File );
   
   unlink( "../../dicts/z_knowlege/tmp/tmp.txt" );
   unlink( "../../dicts/z_knowlege/tmp/tmp1.txt" );
}
/***************************************************************************/
void Grep( char *File1, char *File2, char *Str1 )
{  long i,i1,N,L ;
   char **Str ;
   FILE *fw ;

   N=FileToString( File1, &Str );
   L=strlen( Str1 );
   fw=fopen( File2,"w" );
   for( i=0 ; i<N ; i++ )
   {  for( i1=0 ; Str[i][i1]!=0 ; i1++ )
         if( 0==strncmp( Str[i]+i1,Str1,L ) )
	 {  fprintf( fw,"%s\n",Str[i]); break ;  }
   }
   fclose( fw );
   free( Str[0] );
   free( Str );
}
/***************************************************************************/
long FileToString( char *File, char ***_Str )
{  long  i,j,L ;
   char *Mass,**ss ;

   L=file_to_mass( File, &Mass );
   for( i=j=0 ; i<L ; i++ )
      if( Mass[i]=='\n' ) j++ ;
   ss=(char **)Calloc( j+1,sizeof(char *) );
   ss[0]=Mass ;
   for( i=0,j=1 ; i<L ; i++ )
      if( Mass[i]=='\n' )
      {   ss[j++]=Mass+i+1 ; Mass[i]=0 ;  }
   *_Str=ss ;
   if( L==0 ) return 0 ;
   return j ;
}
/***************************************************************************/
long FileToWord( char *File, char ***_Str )
{  long  i,j,L ;
   char *m,**ss ;

   L=file_to_mass( File, &m );
   if( L==0 ) return 0 ;

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
/***************************************************************************/
int strcmp1( const void *s1, const void *s2 )
{
   return Strcmp(*((char **)s1),*((char **)s2) );
}
/***************************************************************************/
//    отсортировать массив слов, лежащих в файле с исключением лишних
/***************************************************************************/
void WordsSort1( char *File1, char *File2 )
{  long i,N ;
   char **Str,*Str1="" ;
   FILE *fw ;

   N=FileToWord( File1, &Str );
   qsort( Str,N,sizeof(char *),strcmp1 );
   fw=Fopen( File2,"w" );
   for( i=0 ; i<N ; i++ )
   {  if( 0!=strcmp( Str1,Str[i] ) )
          fprintf( fw,"\n%s",Str[i] );
      Str1=Str[i] ;
   }
   Fclose( fw );
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
char * t_Core :: get_meaning( t_rStruct *V )
{  return NULL ;  }
void t_Core :: real_param_up( t_rStruct *V )
{ ; }
/****************************************************************************/
t_Antwort :: t_Antwort( void )
{
   Str[0]=0 ;
}
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
/***************************************************************************/
DEFINE_LIST_BTYPE( t_aWord,t_aWordList )
DEFINE_LIST_BTYPE( t_rStruct,t_rStructList )
DEFINE_LIST_BTYPE( t_rWord,t_rWordList )
DEFINE_LIST_BTYPE( t_Item, t_ItemList )
DEFINE_LIST_TYPE ( t_Tree, t_TreeList )
