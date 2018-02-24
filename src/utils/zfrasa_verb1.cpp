/*

*/

# include <libdic.cpp>

struct t_S1 
{  char Tag[10] ;
   char Str[90] ;
};

/************************************************************************/
int main( void )
{
   long  i,i1,n1,n2,N,N1 ;
   char **Str, **Str1 ;

   N =FileToString( "frasa_verb.txt",&Str  );
   N1=FileToString( "frasa_verb.txt",&Str1 );

   FILE *fw=Fopen( "frasa_verb1.txt","w" );

   for( i=0 ; i<N ; i++ )
      for( i1=0 ; i1<200 ; i1++ )
         if( Str1[i][i1]=='=' ) Str1[i][i1]=0 ;

   for( i=0 ; i<N ; i++ )
   {  if( Str1[i][0]==0 ) continue ;
      fprintf( fw,"\n%s",Str[i] );   
      for( i1=i+1 ; i1<N ; i1++ )
         if( 0==strcmp(Str1[i1],Str1[i]) )
	 {   fprintf( fw,";%s",Str1[i1]+strlen(Str1[i1])+1 );
	     Str1[i1][0]=0 ;
	 }
   }
   Fclose( fw );
   return 0 ;
}
