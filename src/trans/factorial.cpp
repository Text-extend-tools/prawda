# include <stdio.h>
# include <malloc.h>

struct t_Factorial
{  short  n,      // величина факториала n!
          N ;     // число сочетаний
   char  *value ; // массив, в котором лежат сочетания
   long   n_value ;
   short  fak[10] ; // число сочетаний от i
          // ------ величины, нужные при построении -------
   long   j ;
   short  I[10]   ; // значения бегучих индексов
   short  flag[10]; // занятые индексы
   
   t_Factorial( short nn );
   long   calc_factorial( short _n );
   void   make( short z );
   long   use( long Variant, long Word );
};

/************************************************************************/
long t_Factorial :: calc_factorial( short nn )
{  long i,NN ;
   for( NN=i=1 ; i<=nn ; i++ )
   {  NN*=i ; fak[i]=NN ;  }
   return NN ;
}
/************************************************************************/
void t_Factorial :: make( short z )
{  long i,z1 ;

   if( z<0 )
   {  j++ ;
      z1=n*(N-j) ;
      for( i=0 ; i<n ; i++ )
	 value[z1+i]=I[i] ;      
      return ;
   }
  
   for( I[z]=0 ; I[z]<n ; I[z]++ )
   {  if( flag[I[z]]==0 )
      {   flag[I[z]]=1 ;          
	  make( z-1 );
          flag[I[z]]=0 ;
      }
   }
}
/************************************************************************/
t_Factorial :: t_Factorial( short _n )
{   short i,i1 ;

    for( i=0 ; i<10 ; i++ )
       I[i]=flag[i]=0 ;
    n=_n ;
    N=calc_factorial( n );
    n_value=n*N ;
    value=(char *)calloc( n_value,sizeof(char) );
    j=0 ;
    make( n-1 );
/*
    for( i=0 ; i<N ; i++ )
    {  printf("\n");
       for( i1=0 ; i1<n ; i1++ )
          printf( "%d",value[i*n+i1] );
    }  
*/
}
/************************************************************************/
long t_Factorial :: use( long Variant, long Word )
{
   return value[Variant*n+Word] ;
}
