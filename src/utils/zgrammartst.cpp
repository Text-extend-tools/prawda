/**************************************************************************/
//            Распечатать проверить грамматику на корректность 
//
/**************************************************************************/
# include <string.h>
# include <stdlib.h>
# include <mylib.h>
# include <grammar.h>

int main( void )
{  t_Grammar Grammar ;

   system("g++ -E -o ../grammar/lang.txt -I../grammar ../grammar/lang.cpp");
   if( Grammar.make_all( "../grammar/lang.txt" ) )
        system( "more mercury.err" );
   else printf("\n Грамматика описана корректно\n");

   return 0 ;
}
