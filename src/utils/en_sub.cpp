/***************************************************************************/
//    Анисимов Д.В.
//    сделать множественное число английских существительныых 
//    ../../dicts/d2_eng/en_sub1  ->  ../../dicts/d2_eng/en_sub
/***************************************************************************/
# include <stdio.h>
# include <string.h>
# include <stdlib.h>

struct t_S1 { char s[50] ; };
struct t_S2 { char s1[50],s2[50],s3[50] ; };

int main( int argc, char *argv[] )
{
   long  i,i1,j,l,L,n1,n2 ;
   char  c ;
   FILE *fr,*fw ;
   char  Str[100],Str1[100] ;

   fr=fopen( "en_sub2.txt","r" );
   fw=fopen( "en_sub.new","w" );

   while( 1 )
   {
      if( EOF==fscanf(fr,"%s",Str ) ) break ;

      strcpy( Str1,Str );
      l=strlen( Str );
      if( Str[l-1]=='y' )
      {   c=Str[l-2] ;
	  if( c=='a' || c=='e' || c=='i' || c=='o' || c=='u' ) strcat(Str1,"s");
	      else { Str1[l-1]=0 ; strcat(Str1,"ies"); }
      }
      else
      if( Str[l-1]=='o' )
      {   strcat(Str1,"es"); }
      else
      if( Str[l-1]=='f' )
      {   Str1[l-1]=0 ; strcat(Str1,"ves"); }
      else
      if( 0==strcmp(Str+l-2,"fe") )
      {   Str1[l-2]=0 ; strcat(Str1,"ves"); }
      else
      if( 0==strcmp(Str+l-2,"us") )
      {   Str1[l-2]=0 ; strcat(Str1,"i"); }
      else
      if( 0==strcmp(Str+l-2,"um") )
      {   Str1[l-2]=0 ; strcat(Str1,"a"); }
      else
      if( 0==strcmp(Str+l-2,"is") )
      {   Str1[l-2]=0 ; strcat(Str1,"es"); }
      else
      if( 0==strcmp(Str+l-2,"ex") || 0==strcmp(Str+l-2,"ix") )
      {   Str1[l-2]=0 ; strcat(Str1,"ices"); }      
      else
      if( 0==strcmp( Str+l-2,"ss" ) ||
	  0==strcmp( Str+l-2,"ch" ) ||
	  0==strcmp( Str+l-2,"sh" ) ||
	  0==strcmp( Str+l-1,"x"  ) ||
	  0==strcmp( Str+l-1,"s"  ) ) strcat(Str1,"es") ;
      else
      if( Str[l-1]=='a' )
      {   strcat(Str1,"e"); }      
      else strcat(Str1,"s") ;
      
      fprintf( fw,"\n %-15s %s ;",Str,Str1 );
   }
   fclose( fr );
   fclose( fw );   
}

