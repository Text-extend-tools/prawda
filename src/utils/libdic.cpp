# include <stdio.h>
# include <malloc.h>
# include <string.h>
# include <stdlib.h>

# define Err_Memory -2
# define Err_Fopen  -3
# define Err_Fread  -4
# define Err_Fwrite -5
# define Err_Data   -6
# define uchar       unsigned char

# define    max(a,b)    (((a) > (b)) ? (a) : (b))
# define    min(a,b)    (((a) < (b)) ? (a) : (b))
# define    Free( a )    if( a!=NULL ) {  free(a);   a=NULL ;  }
# define    Fclose( f )  if( f!=NULL ) {  fclose(f); f=NULL ;  }

/***************************************************************************/

struct t_SlowoLine     // структура для сортировки грамматических словарей
{  char *str ;
   long index ;  
};

static uchar  SC[256] ;

struct t_SC{  t_SC(void);  };

t_SC SC1 ;

/***************************************************************************/
void  *Calloc( long N, long S )
{  void *v ;
   if( N*S <0 ) return NULL ;
   v=calloc( N,S );
   if( v==NULL ) throw(Err_Memory);
   return v ;
}
/***************************************************************************/
void  *Malloc( long N, long S )
{  void *v ;
   if( N*S <0 ) return NULL ;
   v=malloc( N*S );
   if( v==NULL ) throw(Err_Memory);
   return v ;
}
/***************************************************************************/
void  *Realloc( void *M, long L )
{  void *v ;
   if( L <0 ) return NULL ;
   v=realloc( M,L );
   if( v==NULL ) throw(Err_Memory);
   return v ;
}
/***************************************************************************/
FILE  *Fopen( char *Name, char *a )
{  FILE *f ;
   f=fopen( Name, a );
   if( f==NULL ) throw(Err_Fopen);
   return f ;
}
/***************************************************************************/
void   Fread( void *M, long S, long N, FILE *File )
{  long n ;
   n=fread( M, S, N, File );
   if( n!=N ) throw(Err_Fread);
}
/***************************************************************************/
void   Fwrite( void *M, long S, long N, FILE *File )
{  long n ;
   n=fwrite( M, S, N, File );
   if( n!=N ) throw(Err_Fwrite);
}
/***************************************************************************/
void   Fseek( FILE *fw, long Offset, long Mode )
{
   if( 0!=fseek( fw, Offset, Mode ) )throw(-1);
}
/***************************************************************************/
long   FileLength( FILE *fw )
{
   fpos_t Cur ;
   long   L ;

   if( 0!=fgetpos( fw,&Cur ) ) throw( -1 );
   if( 0!=fseek( fw,0L,SEEK_END) ) throw( -1 );
   L = ftell( fw );
   if( 0!=fsetpos( fw,&Cur) ) throw( -1 );
   return (long )L ;
}
/***************************************************************************/
long file_to_mass( char *File, char **_Mass )
{
   char *Mass ; long L ; FILE *fr ;
   
   fr=Fopen( File,"r" );
   L=FileLength( fr );
   Mass=(char *)Calloc( L,1 );
   Fread( Mass,L,1,fr );
   Fclose(fr);

   *_Mass=Mass ;
   return L ;
}
/***************************************************************************/
t_SC :: t_SC( void )
{  short i ;

   for( i=0 ; i<256 ; i++ )
      SC[i]=i ;
   // ----- соответствие латинских больших и маленьких ----
   for( i=65 ; i<91 ; i++ )
      SC[i]=i+32 ;
   // ----- соответствие русских больших и маленьких ----
   SC[(uchar)'а']=SC[(uchar)'А']=128 ;
   SC[(uchar)'б']=SC[(uchar)'Б']=129 ;
   SC[(uchar)'в']=SC[(uchar)'В']=130 ;
   SC[(uchar)'г']=SC[(uchar)'Г']=131 ;
   SC[(uchar)'д']=SC[(uchar)'Д']=132 ;
   SC[(uchar)'е']=SC[(uchar)'Е']=133 ;
   SC[(uchar)'ё']=SC[(uchar)'Ё']=134 ;
   SC[(uchar)'ж']=SC[(uchar)'Ж']=135 ;
   SC[(uchar)'з']=SC[(uchar)'З']=136 ;
   SC[(uchar)'и']=SC[(uchar)'И']=137 ;
   SC[(uchar)'й']=SC[(uchar)'Й']=138 ;
   SC[(uchar)'к']=SC[(uchar)'К']=139 ;
   SC[(uchar)'л']=SC[(uchar)'Л']=140 ;
   SC[(uchar)'м']=SC[(uchar)'М']=141 ;
   SC[(uchar)'н']=SC[(uchar)'Н']=142 ;
   SC[(uchar)'о']=SC[(uchar)'О']=143 ;
   SC[(uchar)'п']=SC[(uchar)'П']=144 ;
   SC[(uchar)'р']=SC[(uchar)'Р']=145 ;
   SC[(uchar)'с']=SC[(uchar)'С']=146 ;
   SC[(uchar)'т']=SC[(uchar)'Т']=147 ;
   SC[(uchar)'у']=SC[(uchar)'У']=148 ;
   SC[(uchar)'ф']=SC[(uchar)'Ф']=149 ;
   SC[(uchar)'х']=SC[(uchar)'Х']=150 ;
   SC[(uchar)'ц']=SC[(uchar)'Ц']=151 ;
   SC[(uchar)'ч']=SC[(uchar)'Ч']=152 ;
   SC[(uchar)'ш']=SC[(uchar)'Ш']=153 ;
   SC[(uchar)'щ']=SC[(uchar)'Щ']=154 ;
   SC[(uchar)'ъ']=SC[(uchar)'Ъ']=155 ;
   SC[(uchar)'ы']=SC[(uchar)'Ы']=156 ;
   SC[(uchar)'ь']=SC[(uchar)'Ь']=157 ;
   SC[(uchar)'э']=SC[(uchar)'Э']=158 ;
   SC[(uchar)'ю']=SC[(uchar)'Ю']=160 ;
   SC[(uchar)'я']=SC[(uchar)'Я']=161 ;
}
/***************************************************************************/
uchar  *SortChar( void )
{
   return SC ;
}
/***************************************************************************/
//                         сравнить две строки                             //
/***************************************************************************/
short Strcmp( char *Str1, char *Str2 )
{  short i,c ;
   register uchar c1,c2 ;

   for( i=0 ;  ; i++ )
   {  c1=(uchar)Str1[i] ; c2=(uchar)Str2[i] ;
      //if( c1==0 && c2==0 ) return  0 ;
      if( c1 || c2 ) ; else return 0 ;
      if( c1==c2 ) continue ;
      if( c1==0  ) return -1 ;
      if( c2==0  ) return  1 ;
      c = SC[c1] - SC[c2] ;
      if( c!=0 ) return c ;
   }
}
/***************************************************************************/
//                           сравнить строки                               //
/***************************************************************************/
short Strncmp( char *Str1, char *Str2, short L )
{  short i,c ;

   for( i=0 ; i<L ; i++ )
   {  
      if( Str1[i]==0 && Str2[i]!=0 ) return -1 ;
      if( Str1[i]!=0 && Str2[i]==0 ) return  1 ;
      if( Str1[i]==0 && Str2[i]==0 ) return  0 ;
      c = SC[(uchar)Str1[i]] - SC[(uchar)Str2[i]] ;
      if( c!=0 ) return c ;
   }
   return 0 ;
}
/***************************************************************************/
/*                          удалить коментарии                             */
/***************************************************************************/
long   remark( char *Mass, long l_Mass )
{  long i,j ;
   char f=0 ;

   for( i=j=0 ; i<l_Mass-1 ; i++ )
   {  if( f==0 && Mass[i]=='/' && Mass[i+1]=='/' ) f=1 ;
//      if( f==0 && Mass[i]=='#' ) f=1 ;
      if( f==0 && Mass[i]=='/' && Mass[i+1]=='*' ) f=2 ;
      if( f==0 || Mass[i]=='\n' ) Mass[j++]=Mass[i] ;
      if( 0<i ) if( f==2 && Mass[i-1]=='*' && Mass[i]=='/' ) f=0 ;
      if( f==1 && Mass[i]=='\n') f=0 ;
   }
   if( f==0 ) Mass[j++]=Mass[i] ;
   return j ;
}
/***************************************************************************/
static char Emptystr[200] ;
char *emptystr( short n )
{  short i ;
   if( 199<n )
   {  printf("Какому дураку понадобилась такая длинная пустая строка?"); exit(-1); }
   for( i=0 ; i<n ; i++ )
      Emptystr[i]=' ' ;
   Emptystr[i]=0 ;
   return Emptystr ;
}
/***************************************************************************/
void list_error( void )
{
   throw(-1);
}
/***************************************************************************/
int strcmp1( const void *s1, const void *s2 )
{
   return Strcmp(*((char **)s1),*((char **)s2) );
}
int strcmp2( const void *s1, const void *s2 )
{
   return Strcmp( ((t_SlowoLine *)s1)->str, ((t_SlowoLine *)s2)->str );
}
/***************************************************************************/
long FileToMass( char *File, char **_Mass )
{
   char *Mass ; long L ; FILE *fr ;
   
   fr=Fopen( File,"r" );
   L=FileLength( fr );
   Mass=(char *)Calloc( L,1 );
   fread( Mass,L,1,fr );
   Fclose(fr);

   *_Mass=Mass ;
   return L ;
}
/***************************************************************************/
long FileToString( char *File, char ***_Str )
{  long  i,j,L ;
   char *Mass,**ss ;

   L=FileToMass( File, &Mass );
   for( i=j=0 ; i<L ; i++ )
      if( Mass[i]=='\n' ) j++ ;
   ss=(char **)Calloc( j+1,sizeof(char *) );
   ss[0]=Mass ;
   for( i=0,j=1 ; i<L ; i++ )
      if( Mass[i]=='\n' )
      {   ss[j++]=Mass+i+1 ; Mass[i]=0 ;  }
   *_Str=ss ;
   return j ;
}
/***************************************************************************/
long MassToWord( char *m, char ***_Str )
{  long  i,j,L ;
   char **ss ;

   L=strlen( m );

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
long FileToWord( char *File, char ***_Str )
{  long  i,j,L ;
   char *m,**ss ;

   L=FileToMass( File, &m );

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
long Replace( char *Mass, long L, char *Str1, char *Str2, 
              char **_Mass1, long *_L1 )
{  long  i,j,n1,n2,N,L1 ;
   char *Mass1 ;

   n1=strlen(Str1);
   n2=strlen(Str2);
   for( i=N=0 ; i<L ; i++ )
      if( 0==strncmp(Mass+i,Str1,n1) ) 
      {  N++ ; i+=n1 ;  }
   L1=L+N*(n2-n1);
   Mass1=(char *)calloc(L1,sizeof(char));
   for( j=i=N=0 ; i<L ; i++ )
   {  if( 0==strncmp(Mass+i,Str1,n1) ) 
      {   strncpy( Mass1+j,Str2,n2 );
          i+=n1-1 ; j+=n2 ;
      }
      else
          Mass1[j++]=Mass[i] ;
   }
   *_Mass1=Mass1 ;
   *_L1   =L1 ;
   return N ;
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
	 {  fprintf(fw,"%s\n",Str[i]); break ;  }
   }
   fclose( fw );
   free( Str[0] );
   free( Str );
}
/***************************************************************************/
void Grep1( char *File1, char *File2, char *Str1 )
{  long i,i1,N,L ;
   char **Str ;
   FILE *fw ;

   N=FileToString( File1, &Str );
   L=strlen( Str1 );
   fw=fopen( File2,"a" );
   for( i=0 ; i<N ; i++ )
   {  for( i1=0 ; Str[i][i1]!=0 ; i1++ )
	 if( 0==strncmp( Str[i]+i1,Str1,L ) )
	 {  fprintf(fw,"%s\n",Str[i]); break ;  }
   }
   fclose( fw );
   free( Str[0] );
   free( Str );
}
/***************************************************************************/
void UnGrep( char *File1, char *File2, char *Str1 )
{  long i,i1,N,L ;
   char **Str ;
   FILE *fw ;

   N=FileToString( File1, &Str );
   L=strlen( Str1 );
   fw=fopen( File2,"w" );
   for( i=0 ; i<N ; i++ )
   {  for( i1=0 ; Str[i][i1]!=0 ; i1++ )
      {  if( 0==strncmp( Str[i]+i1,Str1,L ) )
	     goto M_No ;
      }
      fprintf(fw,"\n%s",Str[i]);
      M_No :;
   }
   fclose( fw );
   free( Str[0] );
   free( Str );
}
/***************************************************************************/
//    вытащить из грамматического словаря все существующие там слова
/***************************************************************************/
void SlowoToWords( char *File1, char *File2, short n_Form )
{  long i,N ;
   char **Str ;
   FILE *fw ;

   N=FileToWord( File1, &Str );
   fw=Fopen( File2,"w" );
   for( i=0 ; i<N ; i+=n_Form )
   {
      fprintf( fw,"\n%s",Str[i] );
      if( Str[i+n_Form-1][0]!=';' )
      {   printf("\n Error Format !!! !!! !!!"); Fclose( fw ); exit(-1);  }
   }
   Fclose( fw );
}
/***************************************************************************/
//    отсортировать массив слов, лежащих в файле
/***************************************************************************/
void WordsSort( char *File1, char *File2 )
{  long i,N ;
   char **Str ;
   FILE *fw ;

   N=FileToWord( File1, &Str );
   qsort( Str,N,sizeof(char *),strcmp1 );
   fw=Fopen( File2,"w" );
   for( i=0 ; i<N ; i++ )
      fprintf( fw,"\n%s",Str[i] );
   Fclose( fw );
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
/***************************************************************************/
//    отсортировать массив слов, лежащих в файле по частоте
/***************************************************************************/
void WordsSort2( char *File1, char *File2 )
{  long i,j,N,*n ;
   char **ss,*s0="" ;
   FILE *fw ;

   N=FileToWord( File1, &ss );
   qsort( ss,N,sizeof(char *),strcmp1 );

   n =(long  *)calloc(N,sizeof(long  ));
   for( i=j=0 ; i<N ; i++ )
   {  j++ ;
      if( 0!=Strcmp(s0,ss[i]) )
      {   n[i]=j ; j=0 ;
      }
      s0=ss[i] ;
   }

   fw=Fopen( File2,"w" );

   long j1=0,n1=0 ;
   for( j=1000 ; 30<j ; j-=10 )
   {  for( i=1 ; i<N ; i++ )
      {  
	 if( j<=n[i] )
	 {  n1+=n[i] ;
	    fprintf( fw,"\n %3ld %4ld %4ld %s",j1++,n[i],n1,ss[i-1] ); n[i]=0 ;
	 }
      }
   }
   for( j=30 ; 0<j ; j-- )
   {  for( i=1 ; i<N ; i++ )
      {  
	 if( n[i]==j )
	 {  n1+=n[i] ;
	    fprintf( fw,"\n %3ld %4ld %4ld %s",j1++,j,n1,ss[i-1] ); n[i]=0 ;
	 }
      }
   }
   free(n);
   Fclose( fw );
}
/***************************************************************************/
//             отсортировать грамматический словарь
/***************************************************************************/
void SlowoSort( char *File1, char *File2, char n_Form )
{  long i,i1,N,N1 ;
   char **Str ;
   t_SlowoLine *ss ;
   FILE *fw ;

   N=FileToWord( File1, &Str );

   // ---- проверка корректности формата -------------
   for( i=0 ; i<N ; i+=n_Form )
   {
      if( Str[i+n_Form-1][0]!=';' )
      {   printf("\n Error Format !!! !!! !!!"); Fclose( fw ); exit(-1);  }
   }
   N1=N/n_Form ;
   ss=(t_SlowoLine *)Calloc( N1,sizeof(t_SlowoLine) );
   for( i=0 ; i<N1 ; i++ )
   {  ss[i].index=i ;
      ss[i].str  =Str[i*n_Form] ;
   }

   qsort( ss,N1,sizeof(t_SlowoLine),strcmp2 );
   fw=Fopen( File2,"w" );
   for( i=0 ; i<N ; i++ )
   {  fprintf( fw,"\n" );
      for( i1=0 ; i1<n_Form ; i1++ ) 
         fprintf( fw,"%s ",Str[ss[i].index*n_Form+i1] );
   }
   Fclose( fw );
}
