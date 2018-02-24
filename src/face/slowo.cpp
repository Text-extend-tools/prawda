/***************************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                    "Человеческий" словарь (ядро)
//     Анисимов Д.В.                                         сегодня
/***************************************************************************/

# include <stdio.h>
# include <malloc.h>
# include <string.h>
# include <dirent.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <stdlib.h>

# include "mylib.h"
# include "slowo.h"

# define uchar unsigned char

//char  **Files ;
//short   n_Files ;
static uchar  *SC,
        SC1[256], // сортировка по-русски
	SC2[256] ;// сортировка по-европейски
char    StrWord[40],StrTrans[40];
char   *StrAnswer=NULL ;
long    l_StrAnswer=0 ;

# define  is_divider(SS)  (0==strncmp( SS,Divider,l_Divider ))

short my_strncmp( char *Str1, char *Str2, short L );
void  SC_init( void );

/***************************************************************************/
t_Slowo :: t_Slowo( void )
{
   sy_Word    = NULL ;
   f_disk     = 0 ;
   Paragraf   = NULL ;
   n_sy_Word  = 0 ;
   n_Paragraf = 0 ;
   NameFile[0]= 0 ;
   i_Paragraf =-1 ;
   Buf        = NULL ;
   l_Buf      = 0;
   strcpy( Divider,"=" );
   l_Divider=strlen( Divider );
}
/***************************************************************************/
//        прочесть словарь из файла и построить все его структуры          //
/***************************************************************************/
char t_Slowo :: make( char *File )
{  FILE   *fr ;
   size_t  k,k1 ;
   long    i,i1,ii,j,j1 ;
   char    f1=0,f2=0 ;
   char    *m,Name[200] ;
try
{
   SC_init();

   fr=Fopen( File,"rb" );

   if( Buf     !=NULL ) Free(Buf);
   if( sy_Word !=NULL ) Free(sy_Word);
   if( Paragraf!=NULL ) Free(Paragraf);

   Buf=m=(char *)Calloc(  4000,sizeof(char) );
   n_sy_Word=1 ;
   while( 1 )
   {  k=fread( m,1,2000,fr );
      for( i=0 ; i<k ; i++ )
	 if( m[i]=='\n' ) n_sy_Word++ ;
      if( k<2000 ) break ;
   }
   Fseek( fr,0,SEEK_SET );
   sy_Word =(long       *)Calloc( n_sy_Word     ,sizeof(long) );
   Paragraf=(t_Paragraf *)Calloc( n_sy_Word/16+2,sizeof(t_Paragraf) );

   ii=0 ; j=0 ;
   while( 1 )
   {
      k=fread( m,1,2000,fr );
      for( i=0 ; i<k ; i++ )
      {   i1=ii+i ;
	  if( m[i]=='\n' || m[i]=='\r' ) { f1=1 ; f2=0 ; j1=0 ; }
	  if( f1==1 && !is_space( m[i]) )
	  {  sy_Word[++j]=i1 ; f1=0 ; f2=1 ; }
	  if( (j&0x0f)==0 && f2==1 && j1<8 )
	  {   Paragraf[j/16].word[ j1++ ]=m[i] ;
	      if( is_space(m[i]) || is_divider( m+i ) )
	      {   f2=0 ; Paragraf[j/16+1].word[ j1++ ]=0 ;  }
	  }
      }
      if( k<2000 ) break ;
      ii+=k ;
   }
   n_sy_Word=j ;
   n_Paragraf=j/16+1 ;
   Fclose( fr );
   strcpy( NameFile,File );
   k=0 ;
   for( i=48 ; i<n_sy_Word ; i+=16 )
      k=max( k,sy_Word[i]-sy_Word[i-48] );   
   if( k>4000 )
   {   Free(Buf); Buf =(char *)Calloc( k,sizeof(char) );  }
   k1=0 ;   
   for( i=1 ; i<n_sy_Word ; i++ )
   {  k1=max( k1,sy_Word[i]-sy_Word[i-1]+60 ); }
   if( k1>l_StrAnswer )
   {   if( StrAnswer!=NULL ) free( StrAnswer );
       StrAnswer =(char *)Calloc( k1,sizeof(char) );
       l_StrAnswer =k1 ;
   }
   return 0 ;
}
catch( int E )
{
   return -1 ;
}
}
/***************************************************************************/
//  найти индекс строки, в которой есть слово, максимально похожее на Str  //
/***************************************************************************/
long t_Slowo :: find( char *_Str )
{  long b,e,i,i1,i2,L ;
   long w,i_Save,i_Ret=-1 ;
   char Str2[80] ;

   b=0 ; e=n_Paragraf ;
   i_Save=i_Paragraf ;
   while( 1 )
   {  if( e-b<=1 ) break ;
      i=(b+e)/2 ;
      if( 0<my_strncmp( _Str,Paragraf[i].word,8 ) ) b=i ; else e=i ;
   }
   w=(i-1)*16 ;
   read_paragraf( w );

   // ------------- поиск точно такой строки ---------------
   L=strlen(_Str);
   for( i1=0 ; i1<48 ; i1++ )
   {
       if( n_sy_Word<=w+i1 ) continue ;
       strncpy( Str2,Buf+sy_Word[w+i1]-sy_Word[w],80 ); Str2[79]=0 ;
       for( i2=0 ; i2<79 ; i2++ )
	  if( is_divider( Str2+i2 ) ) Str2[i2]=0 ;
       if( 0==my_strncmp( Str2,_Str,L ) )
       {  i_Ret=w+i1 ; goto M_Ok ;  }
   }
   // --- если не найдена точно такая строк, найти похожую ----
   for( i=L-1 ; 0<i ; i-- )
   {  for( i1=0 ; i1<48 ; i1++ )
      {
	  if( n_sy_Word<=w+i1 ) continue ;
	  strncpy( Str2,Buf+sy_Word[w+i1]-sy_Word[w],80 ); Str2[79]=0 ;
	  for( i2=0 ; i2<79 ; i2++ )
	     if( is_divider( Str2+i2 ) ) Str2[i2]=0 ;
	  if( 0==my_strncmp( Str2,_Str,i ) )
	  {  i_Ret=w+i1 ; goto M_Ok ;  }
      }
   }

   read_paragraf( i_Save*16 );
M_Ok:
   return i_Ret ;
}
/***************************************************************************/
//            прочесть параграф, содержащий слово i_Wort                   //
/***************************************************************************/
void t_Slowo :: read_paragraf( long i_Wort )
{
   long  i,p,l ;
   FILE *fr ;

   i=i_Wort/16 ;
   p=sy_Word[i*16] ;
   if( i*16+48<n_sy_Word ) l=sy_Word[i*16+48]-p ;
       else                l=sy_Word[n_sy_Word]-p ;
   i_Paragraf=i ;

   fr=Fopen( NameFile,"rb" );
   Fseek( fr,p,SEEK_SET );
   Fread( Buf,1,l,fr );
   Fclose( fr );
}
/***************************************************************************/
//                     вернуть слово с индексом i                          //
/***************************************************************************/
char * t_Slowo :: wort( long i )
{  long i1,i2,off,FirstWord ;

   FirstWord=i_Paragraf*16 ;

   if( i<0 || n_sy_Word<i )
   {  StrWord[0]=0 ;  }
   else
   {
      off = sy_Word[i] - sy_Word[FirstWord] ;
      strncpy( StrWord,Buf+off,40 ); StrWord[39]=0 ;
      for( i1=0 ; i1<40 ; i1++ )
	 if( is_divider( StrWord+i1 ) ) StrWord[i1]=0 ;
   }
   return StrWord ;
}
/***************************************************************************/
//                вернуть транскрипцию слова с индексом i                  //
/***************************************************************************/
char * t_Slowo :: trans( long z_wort )
{  long  i,j,off,end,B,L ;
   char  *Str ;

   if( z_wort<0 || n_sy_Word<z_wort )
   {  StrTrans[0]=0 ;  }
   else
   {
      read_paragraf( z_wort );
      B=sy_Word[i_Paragraf*16] ;
      off = sy_Word[z_wort] - B ;
      end = sy_Word[z_wort+1] - B ;
      L=min( 1000,end-off );
      Str=Buf+off ;

      for( i=0 ; i<L ; i++ )
	 if( Str[i]=='[' ) break ;
      for( j=0 ; i<L && j<40 ; i++ )
      {  StrTrans[j++]=Str[i] ;
	 if( Str[i]==']' ) break ;
      }
   }
   StrTrans[j]=0 ;
   return StrTrans ;
}
/***************************************************************************/
//                   вернуть перевод слова с индексом i                    //
/***************************************************************************/
char * t_Slowo :: answer( long z_wort )
{  long  i,j,off,end,B,L ;
   char  f ;
   char  *Str,c,c1,c2 ;

   if( z_wort<0 || n_sy_Word<z_wort )
   {  StrTrans[0]=0 ;  }
   else
   {
      read_paragraf( z_wort );
      B=sy_Word[i_Paragraf*16] ;
      off = sy_Word[z_wort] - B ;
      end = sy_Word[z_wort+1] - B ;
      L=min( l_StrAnswer,end-off );
      Str=Buf+off ;

      for( i=j=0,f=0 ; i<L ; i++ )
      {  if( Str[i]=='[' ) f=1 ;
	 if( Str[i]==']' ) { f=0 ; continue ; }
	 if( f==1 ) continue ;
	 c=Str[i] ; c1=Str[i+1] ; c2=Str[i+2] ;
	 if( (c==' ' || '0'<=c1 && c1<='9') &&
	     '0'<=c1 && c1<='9' && (c2==')' || c2=='>') )
	     StrAnswer[j++]='\n' ;
	 StrAnswer[j++]=Str[i] ;
	 if( Str[i]==';' ) StrAnswer[j++]='\n' ;
      }
      StrAnswer[j]=0 ;
   }
   for( B=i=0 ; i<j ; i++ )
      if( is_divider( StrAnswer+i ) )
      {   B=i+l_Divider ; break ;  }


   return StrAnswer+B ;
}
/***************************************************************************/
//       установить какой разделитель будет между словом и переводом       //
/***************************************************************************/
void t_Slowo :: set_divider( char * _Divider )
{
   short  i,j ;
   char  *c ;

   c=Divider ;
   strcpy( c,_Divider );
   for( i=j=0 ; i<10 ; i++ )
   {  if( c[i]!='"' ) c[j++]=c[i] ;
      if( c[i]==0 ) break ;
   }
   l_Divider=strlen( Divider ) ;
}
/***************************************************************************/
//          установить, как будут сортироваться строки словаря             //
/***************************************************************************/
void t_Slowo :: set_sort( char f )
{  if( f==1 ) SC=SC2 ; else SC=SC1 ;  }
/***************************************************************************/
//                         сравнить две строки                             //
/***************************************************************************/
short my_strncmp( char *Str1, char *Str2, short L )
{  short i,c ;

   for( i=0 ; i<L ; i++ )
   {  if( Str1[i]==0 && Str2[i]!=0 ) return -1 ;
      if( Str2[i]==0 && Str1[i]!=0 ) return  1 ;
      c = SC[(uchar)Str1[i]] - SC[(uchar)Str2[i]] ;
      if( c!=0 ) return c ;
   }
   return 0 ;
}
/***************************************************************************/
void SC_init( void )
{  short i ;

   SC=SC1 ;

   for( i=0 ; i<256 ; i++ )
      SC[i]=i ;
   // ----- соответствие латинских больших и маленьких ----
   for( i=65 ; i<91 ; i++ )
      SC[i]=i+32 ;
   // ----- соответствие русских больших и маленьких в КОИ8 ----
   SC[193]=SC[225]=128 ;
   SC[194]=SC[226]=129 ;
   SC[215]=SC[247]=130 ;
   SC[199]=SC[231]=131 ;
   SC[196]=SC[228]=132 ;
   SC[197]=SC[229]=133 ;
   SC[214]=SC[246]=134 ;
   SC[218]=SC[250]=135 ;
   SC[201]=SC[233]=136 ;
   SC[202]=SC[234]=137 ;
   SC[203]=SC[235]=138 ;
   SC[204]=SC[236]=139 ;
   SC[205]=SC[237]=140 ;
   SC[206]=SC[238]=141 ;
   SC[207]=SC[239]=142 ;
   SC[208]=SC[240]=143 ;
   SC[210]=SC[242]=144 ;
   SC[211]=SC[243]=145 ;
   SC[212]=SC[244]=146 ;
   SC[213]=SC[245]=147 ;
   SC[198]=SC[230]=148 ;
   SC[200]=SC[232]=149 ;
   SC[195]=SC[227]=150 ;
   SC[222]=SC[254]=151 ;
   SC[219]=SC[251]=152 ;
   SC[221]=SC[253]=153 ;
   SC[216]=SC[248]=154 ;
   SC[217]=SC[249]=155 ;
   SC[223]=SC[255]=156 ;
   SC[220]=SC[252]=157 ;
   SC[192]=SC[224]=158 ;
   SC[209]=SC[241]=160 ;

   SC=SC2 ;

   for( i=0 ; i<256 ; i++ )
      SC[i]=i ;
   // ----- соответствие латинских больших и маленьких ----
   for( i=65 ; i<91 ; i++ )
      SC[i]=i+32 ;
   // ----- соответствие латинских умляутов, крышечек и тд ----
   SC[192]=SC[193]=SC[194]=SC[195]=SC[196]=SC[197]=97 ; //A
   SC[224]=SC[225]=SC[226]=SC[227]=SC[228]=SC[229]=97 ; //a
   SC[200]=SC[201]=SC[202]=SC[203]=101  ; //E
   SC[232]=SC[233]=SC[234]=SC[235]=101  ; //e
   SC[204]=SC[205]=SC[206]=SC[207]=SC[208]=111  ; //O
   SC[242]=SC[243]=SC[244]=SC[245]=SC[246]=111  ; //o
   SC[223]=115 ; //s
   SC[217]=SC[218]=SC[219]=SC[220]=117  ; //U
   SC[249]=SC[250]=SC[251]=SC[252]=117  ; //u
   SC[221]=SC[253]=SC[255]=121 ; // y
   
   SC=SC1 ;
}
