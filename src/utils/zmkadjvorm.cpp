/**************************************************************************/
//            Распечатать проверить грамматику на корректность 
//
/**************************************************************************/
# include <string.h>
# include <stdlib.h>

int main( void )
{
   fr=fopen( "input","r");
   n=0 ;
   while( 1 )
   {  if( EOF==fscanf(fr,"%s",&Str) ) break ;
      n++
   }
   while( 1 )
   {  if( EOF==fscanf(fr,"%s",&Str) ) break ;
      n++
   }
   return 0 ;
}