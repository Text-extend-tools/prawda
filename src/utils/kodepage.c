/*
             воткнуть charset=Windows-1251 в хтмл-файл.
*/

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
long file_to_mass( char *File, char **_Mass )
{
   char *Mass ; long L ; FILE *fr ;
   
   fr=fopen( File,"r" );
   if( fr==NULL ) {  printf("File %s not exist.",File); exit(-1);  }
   L=FileLength( fr );
   Mass=(char *)calloc( L,1 );
   fread( Mass,L,1,fr );
   fclose(fr);

   *_Mass=Mass ;
   return L ;
}
/***************************************************************************/
long main_funk( char *Mass, char **Antwort, long L )
{
   long  i,j ;
   char *Mass1=(char *)calloc( 2*L,sizeof(char) );
   
   for( i=j=0 ; i<L ; i++ )
   {  if( 0==strncmp( Mass +i,"charset=koi8-r",14 ) )
      {   strncpy( Mass1+j,"charset=Windows-1251",20 );
          i+=13 ; j+=20 ;
      }
      else
          Mass1[j++]=Mass[i] ;
   }
   Mass1[j++]=0 ;
   *Antwort=Mass1 ;
   return j ;
}
/***************************************************************************/
int main( int argc, char *argv[] )
{  long  L,L1 ;
   char *Mass,*Mass1 ;
   FILE *fw ;

   if( argc<3 )
   {   printf("\n Use <file_source> <file_destinition>"); return -1 ;  }

   L =file_to_mass( argv[1], &Mass );
   L1=main_funk( Mass,&Mass1, L );
   fw=fopen( argv[2],"w" );
   fprintf( fw,Mass1 );
   fclose( fw );
}
