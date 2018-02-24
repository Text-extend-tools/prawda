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

void  check_dicts( char To, char *Part, char *NameFile );
void  my_grep( char *File, char *Str );
void  Grep( char *File1, char *File2, char *Str1 );
long  FileToString( char *File, char ***_Str );
long  FileToWord( char *File, char ***_Str );
void  WordsSort1( char *File1, char *File2 );

/************************************************************************/

/************************************************************************/
int main( void )
{  t_DirTrans DT ;

   read_grammar( "../../grammar/english_russian/lang.txt",&DT );
   check_dicts( 0, "сущ"   ,"../../dicts/z_knowlege/absend/en_sub_d" );
   check_dicts( 0, "глагол","../../dicts/z_knowlege/absend/en_verb_d" );

   check_dicts( 1, "сущ"   ,"../../dicts/z_knowlege/absend/ru_sub_d" );
   check_dicts( 1, "прил"  ,"../../dicts/z_knowlege/absend/ru_adj_d" );
   check_dicts( 1, "глагол","../../dicts/z_knowlege/absend/ru_verb_d" );

   read_grammar( "../../grammar/esperanto_russkij/lang.txt",&DT );
   check_dicts( 0, "сущ"   ,"../../dicts/z_knowlege/absend/es_sub_d" );
   check_dicts( 0, "прил"  ,"../../dicts/z_knowlege/absend/es_adj_d" );
   check_dicts( 0, "глагол","../../dicts/z_knowlege/absend/es_verb_d" );
   
   Fclose( fw );
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
void check_dicts( char To, char *Part, char *NameFile )
{  long      i,i_dict ;
   char     *s,*s1 ;
   t_Slowo2 *Slowo ;
   FILE     *fw ;
   
   i_dict=-1 ;
   if( To==0 )
   {  for( i=0 ; i<n_SlowoF ; i++ )
      {  if( 0==strcmp(Part,Grammar[SlowoF[i].Format->i_part].From.Name ) )
	 {   i_dict=i ; break ;  }
      }
      if( i_dict<0 ) return ;
      Slowo=&SlowoF[i_dict] ;
   }
   else
   {  for( i=0 ; i<n_SlowoT ; i++ )
      {  if( 0==strcmp(Part,Grammar[SlowoT[i].Format->i_part].From.Name ) )
	 {   i_dict=i ; break ;  }
      }
      if( i_dict<0 ) return ;
      Slowo=&SlowoT[i_dict] ;
   }
   
   fw=Fopen( NameFile,"w" );
   if( Slowo->f_compress==1 )
   {  
      s1="" ;
      for( i=0 ; i<Slowo->n_Word ; i++ )
      {  s=Slowo->Word1[Slowo->sy_Word1[i]].str ;
         if( 0==strcmp( s,s1 ) && s[0]!='-' )
	     fprintf( fw,"\n %s",s );
         s1=s ;
      }
   }
   fclose( fw );
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
