/**************************************************************************/
//         Найти все конструкции, на которые нет ссылок
//
/**************************************************************************/
# include <grammar.h>

int main( void )
{  short i,i1,j,z,*m,*n ;
   t_Struct *S ;

   Grammar.make_all( "../grammar/lang.txt" );
   m=(short *)calloc( Grammar.n_trans(),sizeof(short) );
   n=(short *)calloc( Grammar.n_trans(),sizeof(short) );
   m[0]=Grammar.n_trans()-1 ; n[0]=1 ;
   for( i=0,j=1 ; i<j ; i++ )
   {  S=&Grammar[m[i]].From ;
      for( i1=0 ; i1<S->Word.j ; i1++ )
      {  z=S->Word[i1].i_struct ;
         if( 0<=z && n[z]==0 )
	 {  m[j++]=z ; n[z]=1 ; }
      }
   }
   for( i=0 ; i<Grammar.n_trans() ; i++ )
   {  if( n[i]==0 )
          printf("\n %s",Grammar[i].From.Name );
   }
   return 0 ;
}
