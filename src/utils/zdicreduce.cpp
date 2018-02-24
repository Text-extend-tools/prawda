/****************************************************************************/
//   Сделать сокращенный словарь
// Оставить в словаре переводов только те слова, которые есть в заданном тексте
/****************************************************************************/
# include <string.h>
# include <stdlib.h>
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
t_Core       Core ;

/***************************************************************************/
//                        прочесть все данные 
/***************************************************************************/
char read_grammar( void )
{
   t_File *F ;
   long   i,N ;
   char   Filename[200] ;
   char   Dir[]="../dicts/" ;
   char   r ;
try
{
   r=Grammar.make_all( "../grammar/lang.txt" );
   if( r<0 ) return r ;
   File_Error=Fopen("mercury.err","w+"); 

   Macro.read( "../dicts/en_macro", &(Grammar.format1()[0]) );
//   make_dic_const( );
   // ---------------- прочитать словари источника --------------------
   t_Lang &From=Grammar.from();
   N=From.File.j ;
   SlowoF   =(t_Slowo2 *)Calloc( N,sizeof(t_Slowo2) );
   i_FormatF=(short    *)Calloc( From.Part.j,sizeof(t_Slowo2) );
   for( i=0 ; i<From.Part.j ; i++ )
      i_FormatF[i]=-1 ;

   for( i=0 ; i<N ; i++ )
   {  F=&From.File[i] ;
      strcpy( Filename,Dir ); strcat( Filename,F->FileName[0] );
      SlowoF[i].read( Filename, &From, &From.Format[F->i_format] );
      SlowoF[i].freverce();
      i_FormatF[F->i_part]=i ;
   }
   // ---------------- прочитать словари приемника --------------------
   t_Lang &To=Grammar.to();
   N=To.File.j ;
   SlowoT   =(t_Slowo2 *)Calloc( N,sizeof(t_Slowo2) );
   i_FormatT=(short    *)Calloc( To.Part.j,sizeof(t_Slowo2) );

   for( i=0 ; i<To.Part.j ; i++ )
      i_FormatT[i]=-1 ;

   for( i=0 ; i<N ; i++ )
   {  F=&To.File[i] ;
      strcpy( Filename,Dir ); strcat( Filename,F->FileName[0] );
      SlowoT[i].read( Filename, &To, &To.Format[F->i_format] );
      SlowoT[i].freverce();
      i_FormatT[F->i_part]=i ;
   }
   // -------------- прочитать словари перевода ---------------
   Perevod.read( &Grammar );

   Fclose( File_Error );
   
//   if( Read_Error==0 ) f_data_read=1 ;
   return Read_Error ; // если тут 0, то все в порядке
}
catch( int E )
{
   Fclose( File_Error );
   return -1 ;
}
}
/***************************************************************************/
//
/***************************************************************************/
int main( void )
{
   FILE *fr ;
   long  i,i1,i2,i3,L,N,N1 ;
   char *Present,**PresentF,**PresentT ;
   char *Mass,*Str,*Str1 ;
   long        Ant1[20] ;
   short       part[20] ;
   t_Form      Form[20] ;
   t_longList  Ant2 ;

   read_grammar( );

   // ---- прочесть файл ---------------------
   fr=Fopen( "../bin/tst/test.txt","r" );
   L=FileLength( fr );

   Mass=(char   *)Calloc( L,sizeof(char) );
   Fread( Mass,L,sizeof(char),fr );

   Lexer.clr_breaker(  );
   Lexer.set_breaker( (unsigned char *)".,!?;:#$%^=+-*/(){}<>[]\"\\" );
   Lexer.init( Mass,L );
   Lexer.remark();
   Lexer.make_words();

   // ---- захватить массивы присутсвия ---------------------
   t_Lang &From=Grammar.from();
   t_Lang &To  =Grammar.to();
   
   Present=(char *)Calloc( Perevod.n_record(),sizeof(char) );
   
   PresentF=(char **)Calloc( From.File.j,sizeof(char *) );
   for( i=0 ; i<From.File.j ; i++ )
      PresentF[i]=(char *)Calloc( SlowoF[i].n_Word,sizeof(char) );
   PresentT=(char **)Calloc( To.File.j,sizeof(char *) );
   for( i=0 ; i<To.File.j ; i++ )
      PresentT[i]=(char *)Calloc( SlowoT[i].n_Word,sizeof(char) );

   // ---- проставить присутствие --------------------------
   for( i=0 ; i<Lexer.n_Word ; i++ )
   {  Str=Lexer.Word[i] ;
      if( 0==strcmp(".",Str ) ) continue ;
      if( 0==strcmp("!",Str ) ) continue ;
      if( 0==strcmp("?",Str ) ) continue ;
      if( 0==strcmp(",",Str ) ) continue ; // временная мера
      if( 0==strcmp("(",Str ) ) continue ; // для облегчения
      if( 0==strcmp(")",Str ) ) continue ; // отладки

      // --------------- найти в словарях форм -------------
      for( i1=0 ; i1<From.File.j ; i1++ )
      {
         N=SlowoF[i1].quest( Str, Ant1, Form );
	 for( i2=0 ; i2<N ; i2++ )
         {  PresentF[i1][Ant1[i2]]=1 ;

	    Str1=SlowoF[i1].normal(Ant1[i2]);
            Perevod.find( Str1, SlowoF[i1].Format->i_part, &Ant2 );
            for( i3=0 ; i3<Ant2.j ; i3++ )
               Present[Ant2[i3]]=1 ;
	 }
      }
      // ---------- найти в главном словаре -----------------
      N=Perevod.part( Str, part );
      for( i1=0 ; i1<N ; i1++ )
      { 
         Perevod.find( Str, part[i1], &Ant2 );
         for( i2=0 ; i2<Ant2.j ; i2++ )
            Present[Ant2[i2]]=1 ;
         Str1=Perevod.translate_word_s( Str, part[i1] );
	 if( 0<=i_FormatT[part[i1]] )
	 {
             N1=SlowoT[i_FormatT[part[i1]]].quest( Str1, Ant1, Form );
	     for( i2=0 ; i2<N1 ; i2++ )
		 PresentT[i_FormatT[part[i1]]][Ant1[i2]]=1 ;
         }
      }   
   }
   // ---- распечатать --------------------------------------
   fr=fopen( "../dicts_reduce/base_enru","w" );
   for( i=0 ; i<Perevod.n_record() ; i++ )
      if( Present[i]==1 )
          Perevod.print_record( fr,i );
   fclose(fr);
    
   for( i=0 ; i<From.File.j ; i++ )
   {  char FileName[100]="../dicts_reduce/" ;
      strcat( FileName,From.File[i].FileName[0] );
      fr=fopen( FileName,"w" );
      for( i1=0 ; i1<SlowoF[i].n_Word ; i1++ )
	 if( PresentF[i][i1]==1 )
             SlowoF[i].print_word( fr,i1 );
      fclose(fr);
   }
   for( i=0 ; i<To.File.j ; i++ )
   {  char FileName[100]="../dicts_reduce/" ;
      strcat( FileName,To.File[i].FileName[0] );
      fr=fopen( FileName,"w" );
      for( i1=0 ; i1<SlowoT[i].n_Word ; i1++ )
	 if( PresentT[i][i1]==1 )
             SlowoT[i].print_word( fr,i1 );
      fclose(fr);
   }

   return 0 ;
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
   Q.init(0,0) ;
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
   Q.init(0,0);
   Str[0]=Str1[0]=0 ;
}
void t_Tree :: del( void )
{ ; }
/***************************************************************************/
DEFINE_LIST_BTYPE( t_rStruct,t_rStructList )
DEFINE_LIST_BTYPE( t_rWord,t_rWordList )
DEFINE_LIST_BTYPE( t_Item, t_ItemList )
DEFINE_LIST_TYPE ( t_Tree, t_TreeList )
