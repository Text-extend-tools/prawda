/****************************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                         Грамматический словарь
//       Анисимов Д.В.                                    сегодня
/****************************************************************************/
# include <stdio.h>
# include <string.h>
# include <stdlib.h>
# include <mylib.h>
# include <slowo2.h>

# define Strcmp  strcmp
# define Strcmp1 strcmp

extern uchar T_to_lower[] ;

struct t_Sort
{   long  i ;
    char *s ;
};
struct t_Decompress
{   char str[80] ;
    long i ;
};

t_Form Form0 ;

/****************************************************************************/
char * to_lower( char *Str )
{  long i ;
   static char Str1[200] ;

   // ------ перевести строку в маленькие буквы -------
   for( i=0 ; i<200 && Str[i]!=0 ; i++ )
      Str1[i]=T_to_lower[(uchar)Str[i]] ;
   Str1[i]=0 ;
   return Str1 ;
}
/****************************************************************************/
//                     инициировать нулями
/****************************************************************************/
t_Slowo2 :: t_Slowo2( void )
{
   f_compress=0 ;
   Lang   =NULL ;
   Format =NULL ;

   n_Word=0 ;
   n_Form=0 ;
   Mass   =NULL ;
   Word   =NULL ;
   reverce=NULL ;
   
   Word1   =NULL ;
   sy_Word1=NULL ;
   Shabl   =NULL ;
   n_Word1=0 ;
   n_Shabl=0 ;
   Form0.init();
}
/***************************************************************************/
//                     освободить данные от словаря
/***************************************************************************/
void t_Slowo2 :: clr( )
{
   f_compress=0 ;
   Lang   =NULL ;
   Format =NULL ;

   n_Word=0 ;
   n_Form=0 ;
   Free( Mass );
   Free( Word );
   Free( reverce ) ;
   
   Free( Word1 ) ;
   Free( sy_Word1 ) ;
   Free( Shabl );
   n_Word1=0 ;
   n_Shabl=0 ;
}
/***************************************************************************/
//                     установить Lang и формат
/***************************************************************************/
void t_Slowo2 :: set_lf( t_Lang *_Lang, t_Format *_Format )
{
   Lang  = _Lang ;
   Format= _Format ;
}
/***************************************************************************/
//              сравнить два слова из словаря (для сортировки)
/***************************************************************************/
int wordcmp( const void *s1, const void *s2 )
{
   return Strcmp( ((t_Sort *)s1)->s,((t_Sort *)s2)->s );
}
/***************************************************************************/
//             прочесть словарь из файла в соответствии с форматом
//   File    - имя файла
//   Lang    - язык (источник или приемник), которому принадлежит словарь
//   _Format - формат, которому соответствует этот язык
/***************************************************************************/
void t_Slowo2 :: read( char *File, t_Lang *_Lang, t_Format *_Format )
{
   long  i,L ;
   FILE *fr ;
   t_Lexer Lexer ;
try
{
   Lang  = _Lang ;
   Format= _Format ;

   n_Form=Format->Record.j ;

   fr=Fopen( File,"r" );
   L=FileLength( fr );

   Mass=(char   *)Calloc( L,sizeof(char) );
   Fread( Mass,L,sizeof(char),fr );
   for( i=0 ; i<L ; i++ )
      Mass[i]=T_to_lower[(uchar)Mass[i]] ;

   Lexer.clr_breaker(  );
   Lexer.init( Mass,L );
   Lexer.remark();
   Lexer.make_words();
   
   if( 0==strcmp(Lexer.Word[0],"@шаблоны") )           
           read_compress( Lexer );
      else read1( Lexer, File );
}
catch( int E )
{  
   fprintf( File_Error,"\n Ребята, че-то я словарь %s не могу прочесть.",File );
   if( E==Err_Fopen ) fprintf( File_Error,"\n А он вообще-то есть?" );
   throw( E );
}
}
/***************************************************************************/
//                   прочесть несжатый словарь
/***************************************************************************/
void t_Slowo2 :: read1( t_Lexer &Lexer, char *File )
{
   long  i,i1,j,z,L ;
   char *Str ;
   t_Sort *ss ;
   
   for( i=0,n_Word=0 ; i<Lexer.n_Word ; i++ )
   {
      if( ';'==Lexer.Word[i][0] ) 
      {  n_Word++ ;
         if( (i+1)%(n_Form+1)!=0 )
	     printf("");
      }
   }

   Word=(long   *)Calloc( n_Word*n_Form,sizeof(long) );

   // ---- отсортировать строки по алфавиту -----
   ss=(t_Sort *)Malloc( n_Word,sizeof(t_Sort) );
   for( i=0 ; i<n_Word ; i++ )
   {  ss[i].i=i*(n_Form+1) ;
      ss[i].s=Lexer.Word[i*(n_Form+1)] ;
   }
   qsort( ss,n_Word,sizeof(t_Sort),wordcmp );

   for( i=z=0 ; i<n_Word ; i++ )
   {  for( i1=j=0 ; i1<n_Form+2 ; i1++ )
      {
         Str=Lexer.Word[ ss[i].i+i1 ] ;
	 if( 0==strcmp(Str,";") )
	 {   if( n_Form!=j )
	     {   fprintf( File_Error,"\n Че-то странное с форматом %s",File );
	         fprintf( File_Error,"\n Строка \"%s\" дурит",Mass+Word[i*n_Form] ); 
		 throw(-1);
             }
	     break ;
	 }
	 // --------------- чтение слова ---------------
	 Word[i*n_Form+j] = z ;
	 strcpy( Mass+z,Str );
	 z+=strlen( Str )+1 ;
	 j++ ;
      }
   }
   Free( ss );
   freverce( );
}
/***************************************************************************/
//                   прочесть сжатый словарь
/***************************************************************************/
void t_Slowo2 :: read_compress( t_Lexer &Lexer )
{  long i,i1,j,j1,z,zz ;
   long j_Shabl,j_EndSh,lBraket,rBraket ;
   char *Str ;

   // --------- сформировать массив шаблонов -------------------
   f_compress=1 ;
   for( i=0 ; i<Lexer.n_Word ; i++ )
   {  if( Lexer.Word[i][0]=='{' )
          lBraket=i ;
      if( Lexer.Word[i][0]=='}' )
      {   rBraket=i ; break ;  }
   }
   for( n_Shabl=0,i=2 ; i<rBraket ; i++ )
      if( Lexer.Word[i][0]==';' )
	  n_Shabl++ ;
   Shabl=(t_Shabl *)Calloc( n_Shabl,sizeof(t_Shabl) );
   EndSh=(char   **)Calloc( rBraket,sizeof(char *) );

   zz=0 ; z=0 ; j_EndSh=0 ;
   Shabl[0].end=EndSh ;
   for( j_Shabl=0,i=lBraket+3 ; i<rBraket ; i++ )
   {  Str=Lexer.Word[i] ;
      if( Str[0]==';' )
      {  Shabl[j_Shabl].n_end=j_EndSh-zz ;
	 Shabl[j_Shabl].end  =EndSh+zz ;
	 j_Shabl++ ;
	 zz=j_EndSh ;
	 i+=2 ; 
	 continue ;  
      }	 
      strcpy( Mass+z,Str+1 );
      EndSh[j_EndSh++]=Mass+z ;
      z+=strlen( Str+1 )+1 ;
   }
   // ----- сосчитать число сжатых слов ---------
   for( i=rBraket+1,n_Word=0 ; i<Lexer.n_Word ; i++ )
      if( ';'==Lexer.Word[i][0] ) n_Word++ ;

   n_Word1 =(Lexer.n_Word-rBraket-n_Word)/2 ;
   Word1   =(t_compWord *)Calloc( n_Word1     ,sizeof(t_compWord) );
   sy_Word1=(long       *)Calloc( n_Word+1,sizeof(long) );

   // ----- чтение массива сжатых слов ----------
   long n_form=0 ;
   long i_shabl ;
   for( i=rBraket+1,j=j1=0 ; i<Lexer.n_Word ; i++ )
   {  Str=Lexer.Word[i] ;
      if( Str[0]==';' )
      {  j1++ ;
	 sy_Word1[j1]=j ;
	 if( n_form!=n_Form )
	 {  printf("Error n_form in t_Slowo2::read"); throw(-1) ;  }
	 n_form=0 ;
	 continue ;
      }
      Word1[j].str=Mass+z ;
      Word1[j].i_shabl=i_shabl=atoi( Lexer.Word[i+1] );
      j++ ;
      strcpy( Mass+z,Str );
      z+=strlen( Str )+1 ;
      i++ ;
      if( i_shabl<0 || n_Shabl<=i_shabl )
      {  printf("Error i_shabl in t_Slowo2::read"); throw(-1) ;  }
      n_form+=Shabl[i_shabl].n_end ;
   }
   sy_Word1[j1]=j ;
   // --------------- сортировка sy_Word1 и Word1 -------------
   
   t_Sort *ss=(t_Sort *)Malloc( n_Word,sizeof(t_Sort) );
   
   for( i=0 ; i<n_Word ; i++ )
   {  ss[i].i=i ;
      ss[i].s=Word1[sy_Word1[i]].str ;
   }
   qsort( ss,n_Word,sizeof(t_Sort),wordcmp );
   long       *sy_Word2 =(long       *)Malloc( n_Word+1 ,sizeof(long) );
   t_compWord *Word2    =(t_compWord *)Malloc( n_Word1  ,sizeof(t_compWord) );
   
   for( i=j=0 ; i<n_Word ; i++ )
   {  z=sy_Word1[ss[i].i+1] ;
      sy_Word2[i]=j ;
      for( i1=sy_Word1[ss[i].i] ; i1<z ; i1++ )
         Word2[j++]=Word1[i1] ;
   }
   sy_Word2[n_Word]=sy_Word1[n_Word] ;
   Free( ss );
   Free( sy_Word1 ); sy_Word1=sy_Word2 ;
   Free(    Word1 );    Word1=   Word2 ;

   freverce2();

   char *s,Str1[100] ;
   strcpy( Str1,word_internal( reverce[0] ) );
   for( i=1 ; i<n_Word ; i++ )
   {  s=word_internal( reverce[i] );
      if( 0<strcmp(Str1,s) )
          printf("\n Error sort %s %s",Str1,s );
      strcpy( Str1,s );
   }
}
/***************************************************************************/
//    построить таблицы для поиска слов, заданных не в начальной форме
/***************************************************************************/
void t_Slowo2 :: freverce( void )
{  long i,n_All ;
   t_Sort *ss ;

   // ---- отсортировать строки по алфавиту -----
   n_All=n_Word*n_Form ;
   ss=(t_Sort *)Malloc( n_All,sizeof(t_Sort) );
   for( i=0 ; i<n_All ; i++ )
   {  ss[i].i=i ;
      ss[i].s=Mass+Word[i] ;
   }
   qsort( ss,n_All,sizeof(t_Sort),wordcmp );
   reverce=(long *)Malloc( n_All,sizeof(long) );
   for( i=0 ; i<n_All ; i++ )
      reverce[i]=ss[i].i ;
   Free( ss );
}
/***************************************************************************/
//
/***************************************************************************/
void t_Slowo2 :: freverce2( void )
{
   long  i,i1,j,n_Ende ;
   long *indEnd ;
   t_Sort *ss ;

   // -------- сортировка "внутри шаблонов" --------------------
   n_Ende=0 ;
   for( i=0 ; i<n_Shabl ; i++ )
      n_Ende+=Shabl[i].n_end ;
   indEnd=(long   *)Calloc( n_Ende,sizeof(long) );
   ss    =(t_Sort *)Calloc( n_Ende,sizeof(t_Sort) );
   for( i=j=0 ; i<n_Shabl ; i++ )
   {  for( i1=0 ; i1<Shabl[i].n_end ; i1++ )
      {  ss[i1].s=Shabl[i].end[i1] ;
         ss[i1].i=i1 ;
      }
      qsort( ss,Shabl[i].n_end,sizeof(t_Sort),wordcmp );
      Shabl[i].index=&indEnd[j] ;
      for( i1=0 ; i1<Shabl[i].n_end ; i1++ )
         indEnd[j++]=ss[i1].i ;
   }
   Free( ss );
   // --------- захват памяти ----------------------------------------
   t_Decompress zWord1,*TT,*T ;
   t_Decompress *Tmp0=(t_Decompress *)Calloc(    n_Form,sizeof(t_Decompress) );
   t_Decompress *Tmp1=(t_Decompress *)Calloc( 30*n_Form,sizeof(t_Decompress) );
   t_Decompress *Tmp2=(t_Decompress *)Calloc( 30*n_Form,sizeof(t_Decompress) );
   ss=(t_Sort *)Malloc( n_Word1,sizeof(t_Sort));
   long *i_rever=(long *)Malloc( n_Word1,sizeof(long));
   reverce=(long *)Malloc( n_Word*n_Form,sizeof(long) );
   
   // ---------- сортировка "сжатых слов" ----------------------------
   long j0,j1,j2,n,n0,n1 ;
   
   n=0 ;
   char *STmp=(char *)Calloc( 20*n_Word1,sizeof(char) );

   for( i=j=j1=0 ; i<n_Word ; i++ )
   {  
      for( i1=sy_Word1[i] ; i1<sy_Word1[i+1] ; i1++ )
      {  if( Shabl[Word1[i1].i_shabl].index[0]==0 )
             ss[j].s=Word1[i1].str ;
         else
	 {  // ----- борьба с явлением, когда исходная форма не первая по алфавиту
	    char *s ;
	    s=decompress( i1,Shabl[Word1[i1].i_shabl].index[0] );
	    ss[j].s=STmp+j1 ;
	    strcpy( STmp+j1,s );
	    j1+=strlen( s )+1 ;
	    if( 20*n_Word1<j1 )
	        printf( "\n Error_sort2" );
	 }
	 ss[j].i=i1 ;
	 i_rever[i1]=n ;
	 n+=Shabl[Word1[i1].i_shabl].n_end ;
	 j++ ;
      }
   }
   qsort( ss,n_Word1,sizeof(t_Sort),wordcmp );
   Free( STmp );

   // -------- собственно формирование отсортированного массива ------
   n1=0 ; j=0 ; 
   n=0 ;  // число уже отсортированных форм слов
   for( i=0 ; i<n_Word1 ; i++ )
   {
      t_Shabl *S=&Shabl[Word1[ss[i].i].i_shabl] ;
      for( i1=0 ; i1<S->n_end ; i1++)
      {  strcpy( Tmp0[i1].str,decompress( ss[i].i,S->index[i1] ) );
         Tmp0[i1].i=i_rever[ss[i].i]+S->index[i1] ;
      }
      n+=S->n_end ;
      n0=i1 ;
      if( i<n_Word1-1 )
      {  S=&Shabl[Word1[ss[i+1].i].i_shabl] ;
         strcpy( zWord1.str,decompress( ss[i+1].i,S->index[0] ) );
      }
      // ---------- а теперь собственно сортировка слиянием -------
      char flag1=0 ;
      
      j0=j1=j2=0 ;
      while( 1 )
      {  if( n0<=j0 && n1<=j1 ) break ;         
         if( j1<n1 && j0==n0 )
	 {   TT=&Tmp1[j1++] ; goto M1 ;  }
         if( j0<n0 && j1==n1 )
	 {   TT=&Tmp0[j0++] ; goto M1 ;  }
	 if( 0>Strcmp( Tmp0[j0].str,Tmp1[j1].str ) )
                  TT=&Tmp0[j0++] ;
             else TT=&Tmp1[j1++] ;
   M1:   if( flag1==0 && (0>=Strcmp( TT->str,zWord1.str ) || i==n_Word1-1) )
	          reverce[j++]=TT->i ;	     
	     else {  Tmp2[j2++]  =*TT ; flag1=1 ;  }
      }
      T=Tmp1 ; Tmp1=Tmp2 ; Tmp2=T ;
      n1=j2 ;
   }
   Free( ss );
   Free( i_rever );
   Free( Tmp0 ); Free( Tmp1 ); Free( Tmp2 );
}
/***************************************************************************/
//                  вернуть слово в начальной форме
//    index - индекс слова (строки)
/***************************************************************************/
char * t_Slowo2 :: normal( long index )
{
   if( f_compress==1 )
        return Word1[sy_Word1[index]].str ;
   else return Mass + Word[index*n_Form] ;
}
/***************************************************************************/
//                  вернуть слово в заданной форме
//    index - индекс слова (строки)
//    Form  - форма слова
/***************************************************************************/
char * t_Slowo2 :: form( long index, t_Form *Form )
{  long i,i1,i_Form ;
   t_Form       *R ;
   t_Param1List *Param ;

   Param=&Lang->Part[Format->i_part].Param ; // параметры этой части речи

   i_Form=0 ;
   for( i=0 ; i<Format->Record.j ; i++ )
   {  R=&Format->Record[i] ;
      for( i1=0 ; i1<Param->j ; i1++ )
         if( Param->list[i1].Dir==0 &&  // переменный параметр слова
	     Form->value[i1]>=0 &&      // параметр задан в конструкции
	     R->value[i1]>=0 &&         // параметр задан в словаре 
	     Form->value[i1]!=R->value[i1] ) // параметр не соответствует 
	     goto M_No;
      i_Form=i ; break ;
      M_No:;
   }
   if( f_compress==1 )
   {  long n_end,z ;
   
      for( z=0,i=sy_Word1[index] ; i<sy_Word1[index+1] ; i++ )
      {  n_end=Shabl[Word1[i].i_shabl].n_end ;
         if( i_Form<z+n_end )
	     return decompress( i, i_Form-z );
         z+=n_end ;
      }
   }
   return Mass + Word[index*n_Form+i_Form] ;
}
/***************************************************************************/
//            вернуть слово в заданной форме (другой интерфейс)
//    Str   - слово в начальной форме
//    Form1 - форма слова
/***************************************************************************/
char * t_Slowo2 :: form( char *Str, t_Form *Form1 )
{  long i,S1,S2,SS ;
   char *Str1=to_lower(Str) ;

   S1=0 ; S2=n_Word-1 ;

   while( 1 )
   {  if( S2-S1<=4 ) break ;
      SS=(S1+S2)/2 ;
      if( 0<Strcmp( Str1,normal( SS ) ) ) S1=SS ; else S2=SS ;
   }
   for( i=S1 ; i<=S2 ; i++ )
      if( 0==Strcmp( Str1,normal( i ) ) ) goto M_Found ;

   return Str1 ;
M_Found:
   return form( i, Form1 );
}
/**************************************************************************/
//             найти слово и определить в какой оно форме
//             Str  - искомое слово
//             Ant  - индексы подходящих слов
//            _Form - их формы
/**************************************************************************/
short t_Slowo2 :: quest( char *Str, long *Ant, t_Form *_Form )
{  long  i,i1,j,S1,S2,S21,SS ;
   char  f ;
   char *s ;
   char *Str1=to_lower(Str) ;

   S1=0 ; S2=S21=n_Word*n_Form ;

   // ---- установить i на первое слово в массиве reverce, равное Str ----
   while( 1 )
   {  if( S2-S1<=4 )
      {   for( i=S1 ; i<=S2 && i<S21 ; i++ )
             if( 0==Strcmp( Str1,word_internal(reverce[i]) )) break ;
	  S1=i ;
          break ;
      }
      SS=(S1+S2)/2 ;
      f=Strcmp( Str1,s=word_internal( reverce[SS] ) );
      if( 0<f ) S1=SS ; 
      if( 0>f ) S2=SS ;
      if( f==0 )
      {   for( i=SS ; S1<=i ; i--)
             if( 0!=Strcmp( Str1,word_internal(reverce[i]) ) ) 
	     {  S1=i+1 ; break ; }
          break ;
      }
   }

   // -------------- заполнить масивы ответов ---------------------------
   for( j=0,i=S1 ; i<n_Word*n_Form ; i++ )
   {  i1=reverce[i] ;      
      if( 0!=Strcmp( Str1,word_internal( i1 ) ) ) break ;
      Ant[j] = i1/n_Form ; 
      // -------- прописать переменные параметры слова ----------
      _Form[j] = Format->Record[ i1%n_Form ] ;
      j++ ;
      if( 20<=j ) break ;
   }
   return j ;
}
/***************************************************************************/
//              найти номер слова в словаре
//              Str - слово в начальной форме
/***************************************************************************/
long t_Slowo2 :: find( char *str )
{  long  a,b,c,i ;
   char *s1 ;

   a=0 ; b=n_Word-1 ;
   while( 1 )
   {  if( b-a<10 )
      {  for( i=a ; i<=b ; i++ )
	 {  
	    if( 0==Strncmp( str,normal( i ),40 ) )
		return i ;
	 }
	 return -1 ;
      }
      c=(a+b)>>1 ;
      if( 0<Strncmp( str,normal( c ),40 ) ) a=c ; else b=c ;
   }
}
/***************************************************************************/
//             выдать форму слова по номеру reverse
/***************************************************************************/
char * t_Slowo2 :: word_internal( long i_reverse )
{
   static char Str[100] ;
   if( f_compress==0 )
       return Mass+Word[i_reverse] ;
   else
   {  long i,z,n_end ;
      long i_word,i_Form ;
      
      i_word=i_reverse/n_Form ;
      i_Form=i_reverse%n_Form ;
      
      for( z=0,i=sy_Word1[i_word] ; i<sy_Word1[i_word+1] ; i++ )
      {  n_end=Shabl[Word1[i].i_shabl].n_end ;
         if( i_Form<z+n_end )
	     return decompress( i, i_Form-z );
         z+=n_end ;
      }
      return "" ; // безобразие
   }
}
/***************************************************************************/
//          выдать форму СЖАТОГО слова по номеру слова и форме
/***************************************************************************/
char * t_Slowo2 :: decompress( long i_word1, long i_ende )
{  static char Str[100] ;
   static long i_word2=-1 ;
   static long i_shabl,L,L1 ;

   if( i_word2!=i_word1 )
   {  i_shabl=Word1[i_word1].i_shabl ; 
      L1 =strlen( Shabl[i_shabl].end[0] );
      strcpy( Str,Word1[i_word1].str );
      L  =strlen( Str );
      i_word2=i_word1 ;
   }
   strcpy( Str+L-L1,Shabl[i_shabl].end[i_ende] );
   return Str ;
}
/***************************************************************************/
//            есть ли в словаре такое слово
//            Str - слово в начальной форме
/***************************************************************************/
char t_Slowo2 :: word_exist( char *Str )
{  char *Str1=to_lower( Str ) ;

   if( 0>find( Str1 ) ) return -1 ;
       else return 0 ;
}
/***************************************************************************/
//                напечатать все формы слова в заданный файл 
/***************************************************************************/
void t_Slowo2 :: print_word( FILE *fw, long index )
{  long i,N ;

   if( f_compress==0 )
   {  N=Format->Record.j ;
      for( i=0 ; i<N ; i++ )
	 fprintf( fw,"%s ",Mass + Word[index*n_Form+i] );

      fprintf( fw,";\n");
   }
   else
   {  long i1,n_end ;
      for( i=sy_Word1[index] ; i<sy_Word1[index+1] ; i++ )
      {  n_end=Shabl[Word1[i].i_shabl].n_end ;
         for( i1=0 ; i1<n_end ; i1++ )         
	    fprintf( fw,"%s ", decompress( i, i1 ) );
      }
      fprintf( fw,";\n");
   }
}
