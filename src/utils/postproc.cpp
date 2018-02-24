/*
              постпроцессор перевода
*/
# include <string.h>
# include <mylib.h>

/************************************************************************/
void main_funk( char *File1, char *File2 )
{  long  i,j,L ;
   char *Mass ;
   FILE *fw ;

   L=file_to_mass( File1,&Mass );
   
   for( i=j=0 ; i<L ; i++)
   {
      if( 0==strncmp( Mass+i,"\\@@",3 ) ) { i+=2 ; continue ; }
      if( 0==strncmp( Mass+i,"\\##",3 ) ) { i+=2 ; continue ; }
      if( Mass[i]=='{' || Mass[i]=='}' ) continue ;
      Mass[j++]=Mass[i] ;
   }
   fw=Fopen( File2,"w" );
   fwrite( Mass,j,sizeof(char),fw );
   Fclose(fw);
}
/************************************************************************/
int main( int argc, char *argv[] )
{  char Str[200],Str1[200] ;

try
{  if( 1<argc )
   {   strcpy( Str ,argv[1] );
       strcpy( Str1,argv[1] );
       strcat( Str1,".post" );
   }
   else
   {   printf("\n Use postproc <filename>\n");
       exit(-1);
   }

   main_funk( Str, Str1 );
   return 0 ;
}
catch(...)
{  return -1 ;
}
}
/************************************************************************/

