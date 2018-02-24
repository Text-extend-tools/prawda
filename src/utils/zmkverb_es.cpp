/***************************************************************************/
//    Сделать формы глаголов Эсперантл (причастия склоняются)
//
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
long FileToMass( char *File, char **_Mass )
{
   char *Mass ; long L ; FILE *fr ;
   
   fr=fopen( File,"r" );
   L=FileLength( fr );
   Mass=(char *)calloc( L,1 );
   fread( Mass,L,1,fr );
   fclose(fr);

   *_Mass=Mass ;
   return L ;
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
   ss=(char **)calloc(j+1,sizeof(char *));
   j=0 ;
   if( m[0]!=0 ) ss[j++]=m ;
   for( i=0 ; i<L ; i++ )
      if( m[i]==0 && m[i+1]!=0 ) ss[j++]=m+i+1 ;

   *_Str=ss ;
   return j ;
}
/***************************************************************************/
int main( int argc, char *argv[] )
{
   long i,L,N ;
   FILE *fw ;
   char  **Str ;
   char  Str1[100],Str2[100] ;
   
   N=FileToWord( "es_verb1",&Str );
   fw=fopen( "es_verb2","w" );
   
   for( i=0 ; i<N ; i++ )
   {  strcpy( Str1,Str[i] );
      L=strlen( Str1 );
      Str1[L-1]=0 ;
      fprintf( fw,"\n%si %sas %sis %sos %sus %su",Str1,Str1,Str1,Str1,Str1,Str1 );
      sprintf( Str2,"%santa",Str1 ); fprintf( fw,"\n\t%s %sn %sj %sjn",Str2,Str2,Str2,Str2 );
      sprintf( Str2,"%sinta",Str1 ); fprintf( fw,"\n\t%s %sn %sj %sjn",Str2,Str2,Str2,Str2 );
      sprintf( Str2,"%sonte",Str1 ); fprintf( fw,"\n\t%s %sn %sj %sjn",Str2,Str2,Str2,Str2 );
      sprintf( Str2,"%sata" ,Str1 ); fprintf( fw,"\n\t%s %sn %sj %sjn",Str2,Str2,Str2,Str2 );
      sprintf( Str2,"%sita" ,Str1 ); fprintf( fw,"\n\t%s %sn %sj %sjn",Str2,Str2,Str2,Str2 );
      sprintf( Str2,"%sote" ,Str1 ); fprintf( fw,"\n\t%s %sn %sj %sjn",Str2,Str2,Str2,Str2 );
      
      fprintf( fw,"\n\t%sante %sinte %sonte ;",Str1,Str1,Str1 );

   }
   fclose(fw);
   return 0 ;
}



