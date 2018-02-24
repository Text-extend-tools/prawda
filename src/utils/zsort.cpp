/**************************************************************************/
//                 Отсортировать массив строк
//
/**************************************************************************/
# include <stdio.h>
# include <string.h>
# include <stdlib.h>

# define uchar   unsigned char

static uchar  SC[256] ;

struct t_SC{  t_SC(void);  };

t_SC SC1 ;

struct t_Str {  char str[50] ;  } ;

/**************************************************************************/
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
/**************************************************************************/
int strcmp1( const void *Str1, const void *Str2  )
{
   return Strcmp( ((t_Str *)Str1)->str,((t_Str *)Str2)->str );
}
/**************************************************************************/
int main( void )
{
   long   i,n ;
   char   Str[100] ;
   t_Str *Mass ;
   FILE  *fr ;

   fr=fopen( "input","r" );
   n=0 ;
   while( 1 )
   {
      if( EOF==fscanf(fr,"%s",Str) ) break ;
      n++ ;
   }
   fclose(fr);
   
   Mass=(t_Str *)calloc( n,sizeof(t_Str) );
      
   fr=fopen( "input","r" );
   n=0 ;
   while( 1 )
   {
      if( EOF==fscanf(fr,"%s",Mass[n].str ) ) break ;
      n++ ;
   }
   qsort( Mass,n,sizeof(t_Str),strcmp1 );
   Str[0]=0 ;
   for( i=0 ; i<n ; i++ )
   {  if( 0!=strcmp(Str,Mass[i].str) )
          printf("\n%s",Mass[i].str );
      strcpy(Str,Mass[i].str);
   }
   return 0 ;
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
