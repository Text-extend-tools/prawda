# include <string.h>
# include <stdlib.h>

# include <mylib.h>
# include <slowo2.h>
# include <slowo3.h>
# include <grammar.h>
# include <core.h>

/***************************************************************************/
int main( short argc, char *argv[] )
{  char  Sou[1000],Dst[1000] ;
   long  i,i1,z,N,L,J ;
   char *mass ;
   FILE *fr,*fw ;
   t_rStruct *pV ;
      
   read_grammar( ".." );

   fr=fopen( "tst/test1.txt","r" );
   L=FileLength( fr );
   mass=(char *)malloc( L );
   fread( mass,L,1,fr );

   long n_frasa,n_figna,n_variant,n_ok ;
   n_frasa=n_figna=n_variant=n_ok=0 ;

   fw=Fopen("perevod1.err","w");

   for( J=z=i=0 ; i<L ; i++ )
   {  if( mass[i]=='.' || mass[i]=='?' || mass[i]=='!' )
      {
	  strncpy( Sou,mass+z,i-z+1 ); Sou[i-z+1]=0 ;
	  Core.translate( Sou, Dst );
	  for( N=i1=0 ; i1<Core.n_antwort() ; i1++ )
	  {
	     if( Core.variants( 0,i_main_struct )->Variant.j<=i1 ) continue ;
	     pV=Core.variant( 0,i_main_struct,i1 );
	     if( pV->i_last_word==Core.from().j ) N++ ;

	  }
	  if( N==0 ) n_figna++ ;
	      else { n_ok++ ; n_variant+=N ;  }
          n_frasa++ ;
	  printf("\n %ld %s",N,Dst );
	  if( N==0 ) fprintf(fw,"\n%s\n %s",Sou,Dst );
	  z=i+1 ;   
      }
      if( mass[i]==':' ) 
      {  z=i+1 ;
         printf("\n test%ld",++J );
      }
   }
   Fclose(fw);
   
   printf("\n");
   printf("\n число фраз           = %ld",n_frasa );
   printf("\n число непереведенных = %ld",n_figna );
   printf("\n число вариантов      = %ld",n_variant );
   printf("\n число вариантов/фразу= %f",(float)n_variant/(float)n_ok );
   return (0) ;
}

