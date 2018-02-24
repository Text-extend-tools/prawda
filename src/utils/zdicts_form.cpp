/************************************************************************/
//             Программа формирования форм
/************************************************************************/
# include <time.h>
# include <dirent.h>
# ifdef __WIN__
   # include <dir.h>
#else
   # include <unistd.h>
#endif

# include <sys/types.h>
# include <sys/stat.h>

# include <errno.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# include "mylib.h"
# include "grammar.h"
# include "slowo2.h"
# include "core.h"
# include "video.h"


struct t_File2
{  char  Src[200] ;     // имя исходного файла
   char  Dst[200] ;     // имя конечного файла
   char  Grammar[200] ; // файл грамматики этого языка
   char  Dict[200] ;    // файл опорного словаря
   short enter[7] ;     // перевод строки
   short tab[7] ;       // табуляция
} ;

t_File2  File[]=
{
   {  "../../dicts/z_knowlege/absend/ru_sub",
      "../../dicts/z_knowlege/absend/ru_sub_new",
      "../../grammar/esperanto_russkij/lang.txt",
      "dicts/zform/russkij/ru_sub_r",
      {  5,11,-1,-1,-1,-1,-1 },
      { -1,-1,-1,-1,-1,-1,-1 }
   },
   {  "../../dicts/z_knowlege/absend/ru_adj",
      "../../dicts/z_knowlege/absend/ru_adj_new",
      "../../grammar/esperanto_russkij/lang.txt",
      "dicts/zform/russkij/ru_adj_r",
      {  3, 7,11,15,19,23,27 },
      { -1,-1,-1,-1,-1,-1,-1 }
   }
};
const short n_File=2 ;
      short i_File ;

t_Slowo2 Slowo ;
t_Core   Core ;
char    **Word ;
long     n_Word ;
short    Shablon [300] ;
short    Shablon1[600] ;
short    j_Shablon ;
char    *MercuryDir="/home/wolf/work/mercury4_all/" ;

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
/************************************************************************/
void read_data( t_File2 &File )
{  int  r ;
   long i,i1,f_Ok=0 ;
   char Str[300] ;

   r=Grammar.make_all( File.Grammar );
   if( r<0 ) exit(-1);
   File_Error=Fopen("mercury.err","w+");

   t_Lang &From=Grammar.from();
   for( i=0 ; i<From.File.j ; i++ )
      for( i1=0 ; i1<From.File[i].FileName.j ; i1++ )
         if( 0==strcmp( File.Dict, From.File[i].FileName[i1] ) )
	 {   t_File *F=&From.File[i] ;
	     sprintf( Str,"%s%s",MercuryDir,F->FileName[i1] );
	     Slowo.read( Str, &From, &From.Format[F->i_format] );
	     f_Ok=1 ;
	 }
	     
   t_Lang &To=Grammar.to();
   for( i=0 ; i<To.File.j ; i++ )
      for( i1=0 ; i1<To.File[i].FileName.j ; i1++ )
         if( 0==strcmp( File.Dict, To.File[i].FileName[i1] ) )
	 {   t_File *F=&To.File[i] ;
	     sprintf( Str,"%s%s",MercuryDir,F->FileName[i1] );
	     Slowo.read( Str, &To, &To.Format[F->i_format] );
	     f_Ok=1 ;
	 }
   if( f_Ok!=1 )
   {   printf( "\n Не могу найти файл %s",File.Dict );
       exit(-1) ;
   }
   n_Word=FileToWord( File.Src, &Word );
}
/************************************************************************/
void paint1( short current )
{  short i ;
   char  f ;

   s_text_yxf( 1,1,0x0f,"Выберите файл, с которым будете работать" );
   for( i=0 ; i<n_File ; i++ )
   {  if( i==current ) f=0xb0 ;
          else         f=0x07 ;
      s_text_yxf( 2+i,1,f,File[i].Src );
   }
}
/************************************************************************/
//                 выбрать словарь для работы
/************************************************************************/
int loop1( )
{
   short i_File=0 ;
   static short taste1,taste2 ;

   while( 1 )
   {   paint1( i_File ) ;

       s_getch( &taste1,&taste2 );
       switch( taste1 )
       {   case 0 :
	       switch( taste2 )
	       {   case S_key_Down : i_File++ ; break ;
		   case S_key_Up   : i_File-- ; break ;
		   case S_key_Home : i_File=0 ; break ;
		   case S_key_End  : i_File=n_File-1 ; break ;
		   case S_key_F10 : return -1 ;
	       }
	       break ;
	  case '\r' : return i_File ;
       }
       if( i_File<0 ) i_File=0 ;
       if( n_File<=i_File ) i_File=n_File-1 ;
   }
}
/************************************************************************/
char *decompress( char *Word1, t_Shabl &S, short i_ende )
{  long L,L1 ;
   static char Str[100] ;

   L1 =strlen( S.end[0] );
   strcpy( Str,Word1 );
   L  =strlen( Str );

   strcpy( Str+L-L1,S.end[i_ende] );
   return Str ;
}
/************************************************************************/
//
/************************************************************************/
void write( char *File1, char *Word, short i_Shablon )
{
   long   i ;   
   short *e=File[i_File].enter ;
   short *t=File[i_File].tab ;   

   FILE *fw=Fopen( File1,"a+" );
   t_Shabl &S=Slowo.Shabl[i_Shablon] ;

   for( i=0 ; i<S.n_end ; i++ )
   {
      fprintf( fw,"%s ",decompress( Word, S, i ) );
      if( i==e[0] || i==e[1] || i==e[2] || i==e[3] || i==e[4] || i==e[5] || i==e[6] )
          fprintf( fw,"\n" );
      if( i==t[0] || i==t[1] || i==t[2] || i==t[3] || i==t[4] || i==t[5] || i==t[6] )
          fprintf( fw,"\t" );
   }
   fprintf( fw,";\n" );
   Fclose( fw );
}
/************************************************************************/
//               нарисовать шаблон словоизменения
/************************************************************************/
void paint( short i_Shablon, long i_first_Word, long i_Word )
{  short  i,x,y,sx,sy,f ;
   char  *s ;
   short *e=File[i_File].enter ;
   short *t=File[i_File].tab ;
   char   Str[100] ; 

   t_Shabl &S=Slowo.Shabl[Shablon[i_Shablon]] ;

   s_get_size( sx,sy );
   s_nacht();
   s_rame1_F( 0,0,   7,sx-1,0x0f );
   s_rame1_F( 8,0,sy-1,sx-1,0x0f );
   // -------- номера шаблонов ------------------------------------
   sprintf( Str,"%3d/%3d",i_Shablon,j_Shablon );
   s_text_yxf( 0,sx-20,0x0f,Str );
   
   // -------- способ склонения -----------------------------------
   x=2 ; y=1 ;
   for( i=0 ; i<S.n_end ; i++ )
   {
      s=decompress( Word[i_Word], S, i );
      s_text_yxf( y,x,0x0f,s );
      
      x+=strlen( s )+1 ;
      if( i==e[0] || i==e[1] || i==e[2] || i==e[3] || i==e[4] || i==e[5] || i==e[6] )
      {   x=2 ; y++ ;  }
      if( i==t[0] || i==t[1] || i==t[2] || i==t[3] || i==t[4] || i==t[5] || i==t[6] )
      {   x+=8 ; }
   }
   // ------- список слов ----------------------------------------
   sprintf( Str,"%3d/%3d",i_Word,n_Word );
   s_text_yxf( 8,sx-20,0x0f,Str );
   for( i=i_first_Word ; i<n_Word ; i++ )
   {  if( sy-10<=i-i_first_Word ) break ;
      if( i==i_Word ) f=0x70 ;
          else        f=0x07 ;
      s_text_yxf( 9+i-i_first_Word,2,f,Word[i] );
   }
}
/************************************************************************/
long make_shablon( char *Word )
{  long i,j,L,L1 ;

   // -------- найти множество возможных шаблонов ---------
   L=strlen( Word );
   for( i=0 ; i<600 ; i++ )
      Shablon1[i]=0 ;
   for( i=j=0 ; i<Slowo.n_Shabl ; i++ )
   {  L1=strlen( Slowo.Shabl[i].end[0] ) ;
      if( 0==strcmp( Word+L-L1,Slowo.Shabl[i].end[0] ) )
      {   Shablon[j++]=i ;
          if( i<600 ) Shablon1[i]=j ;
      }
   }
   j_Shablon=j ;
   // ------ найти лучший шаблон --------------------------
   long i1,is,Lmin,i_best=0,i_best_word=0 ;

   for( i=0 ; i<Slowo.n_Word ; i++ )
   {  is=Slowo.Word1[i].i_shabl ;
      if( 600<=is ) continue ;
      if( Shablon1[is]==0 ) continue ;
      L1=strlen( Slowo.Word1[i].str );
      Lmin=min(L,L1);
      for( i1=0 ; i1<Lmin ; i1++ )
         if( Word[L-i1]!=Slowo.Word1[i].str[L1-i1] ) break ;
      if( i_best<i1 )
      {   i_best=i1 ; i_best_word=i ;  }
   }
   is=Slowo.Word1[i_best_word].i_shabl ;
   return Shablon1[is]-1 ;
}
/************************************************************************/
void loop( )
{  short taste1,taste2 ;
   short sx,sy ;
   long  i_first_Word,i_Word,i_Shablon ;

   s_get_size( sx,sy );
   i_first_Word=i_Word=i_Shablon=0 ;
   make_shablon( Word[i_Word] );

   while( 1 )
   {
       paint( i_Shablon, i_first_Word, i_Word );
       
       s_getch( &taste1,&taste2 );
       switch( taste1 )
       {   case 0 :
	       switch( taste2 )
	       {   case S_key_Down : 
	                  i_Word++ ;
			  if( n_Word<=i_Word ) i_Word=n_Word-1 ;
			  i_Shablon=make_shablon( Word[i_Word] );			  
			  break ;
		   case S_key_Up   : 
		          i_Word-- ;
			  if( i_Word<0 ) i_Word=0 ;
			  i_Shablon=make_shablon( Word[i_Word] );
			  break ;
		   case S_key_Left : i_Shablon-- ; break ;
		   case S_key_Right: i_Shablon++ ; break ;
		   case S_key_F10  : exit(0) ;
	       }
	       break ;
	   case '\r' : 
	        write( File[i_File].Dst, Word[i_Word], Shablon[i_Shablon] );
		Word[i_Word][0]='*' ;
       }
       if( i_Word<i_first_Word )        i_first_Word=i_Word ;
       if( i_first_Word+sy-10<=i_Word ) i_first_Word=i_Word-(sy-11) ;
       if( i_Shablon<0 )          i_Shablon=j_Shablon-1 ;
       if( j_Shablon<=i_Shablon ) i_Shablon=0 ;
   }
}
/************************************************************************/
int main( int argc, char *argv[] )
{
   s_begin_schone( );
   i_File=loop1();
   read_data( File[i_File] );
   loop();
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
/************************************************************************/
DEFINE_LIST_BTYPE( t_aWord,t_aWordList )
DEFINE_LIST_BTYPE( t_rStruct,t_rStructList )
DEFINE_LIST_BTYPE( t_rWord,t_rWordList )
DEFINE_LIST_BTYPE( t_Item, t_ItemList )
DEFINE_LIST_TYPE ( t_Tree, t_TreeList )
