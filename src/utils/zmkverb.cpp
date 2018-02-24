/***************************************************************************/


/***************************************************************************/
# include <stdio.h>
# include <malloc.h>
# include <string.h>
# include <stdlib.h>

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
void main_trans( FILE *fw, char *Text1 )
{  
   short  i,j,L ;
   char  *s1,*s2,*s3 ;
   char  *t1,*t2,*t3 ;
   char   Text[2000] ;
   
   strcpy( Text,Text1 );
   L=strlen( Text );
   for( i=j=0 ; i<L ; i++ )
   {  
      if( Text[i]=='{' )
      {  if( j==0 ) s1=Text+i+1 ;
         if( j==1 ) s2=Text+i+1 ;
         if( j==2 ) s3=Text+i+1 ;
	 j++ ;
      }
      if( Text[i]=='}' ) Text[i]=0 ;
      if( Text[i]=='=' ) break ;
   }
   for( j=0 ; i<L ; i++ )
   {  
      if( Text[i]=='{' )
      {  if( j==0 ) t1=Text+i+1 ;
         if( j==1 ) t2=Text+i+1 ;
         if( j==2 ) t3=Text+i+1 ;
	 j++ ;
      }
      if( Text[i]=='}' ) Text[i]=0 ;
   }
   fprintf( fw,"\nVV:V1[%s] %s @0[%s]=VV:%sV1[%s] @0[%s]",s1,s3,s2,t1,t2,t3 );
}
/***************************************************************************/
int main( int argc, char *argv[] )
{
   char *Text ;
   long i,j,L ;
   FILE *fr ;

   fr=fopen( "../dicts/verb.txt","rb" );
   L=FileLength( fr );
   Text=(char *)calloc( L+1,1 );
   fread( Text,L,1,fr );
   fclose(fr);

   for( i=0,j=0 ; i<L ; i++ )
      if( Text[i]!='\r' ) Text[j++]=Text[i] ;
   L=j ;
   for( i=0 ; i<L ; i++ )
      if( Text[i]=='\n' ) Text[i]=0 ;

   fr=fopen( "../dicts/base_enru_verb","w" );
   for( i=0,j=0 ; i<=L ; i++ )
   {  if( Text[i]==0 || i==L )
      {  Text[i]=0 ;
	 main_trans( fr, Text+j );
	 j=i+1 ;
      }
   }
   fclose(fr);

   return 0 ;
}



