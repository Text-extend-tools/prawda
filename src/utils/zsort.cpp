/**************************************************************************/
//                 ������������� ������ �����
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
   // ----- ������������ ��������� ������� � ��������� ----
   for( i=65 ; i<91 ; i++ )
      SC[i]=i+32 ;
   // ----- ������������ ������� ������� � ��������� ----
   SC[(uchar)'�']=SC[(uchar)'�']=128 ;
   SC[(uchar)'�']=SC[(uchar)'�']=129 ;
   SC[(uchar)'�']=SC[(uchar)'�']=130 ;
   SC[(uchar)'�']=SC[(uchar)'�']=131 ;
   SC[(uchar)'�']=SC[(uchar)'�']=132 ;
   SC[(uchar)'�']=SC[(uchar)'�']=133 ;
   SC[(uchar)'�']=SC[(uchar)'�']=134 ;
   SC[(uchar)'�']=SC[(uchar)'�']=135 ;
   SC[(uchar)'�']=SC[(uchar)'�']=136 ;
   SC[(uchar)'�']=SC[(uchar)'�']=137 ;
   SC[(uchar)'�']=SC[(uchar)'�']=138 ;
   SC[(uchar)'�']=SC[(uchar)'�']=139 ;
   SC[(uchar)'�']=SC[(uchar)'�']=140 ;
   SC[(uchar)'�']=SC[(uchar)'�']=141 ;
   SC[(uchar)'�']=SC[(uchar)'�']=142 ;
   SC[(uchar)'�']=SC[(uchar)'�']=143 ;
   SC[(uchar)'�']=SC[(uchar)'�']=144 ;
   SC[(uchar)'�']=SC[(uchar)'�']=145 ;
   SC[(uchar)'�']=SC[(uchar)'�']=146 ;
   SC[(uchar)'�']=SC[(uchar)'�']=147 ;
   SC[(uchar)'�']=SC[(uchar)'�']=148 ;
   SC[(uchar)'�']=SC[(uchar)'�']=149 ;
   SC[(uchar)'�']=SC[(uchar)'�']=150 ;
   SC[(uchar)'�']=SC[(uchar)'�']=151 ;
   SC[(uchar)'�']=SC[(uchar)'�']=152 ;
   SC[(uchar)'�']=SC[(uchar)'�']=153 ;
   SC[(uchar)'�']=SC[(uchar)'�']=154 ;
   SC[(uchar)'�']=SC[(uchar)'�']=155 ;
   SC[(uchar)'�']=SC[(uchar)'�']=156 ;
   SC[(uchar)'�']=SC[(uchar)'�']=157 ;
   SC[(uchar)'�']=SC[(uchar)'�']=158 ;
   SC[(uchar)'�']=SC[(uchar)'�']=160 ;
   SC[(uchar)'�']=SC[(uchar)'�']=161 ;
}
