# include <string.h>
# include "mylib.h"
# include "../utils/zlexer.h"

# define Ifstr( a,b ) if( 0==strcmp(a,b) )

/***************************************************************************/
t_Lexer ::  t_Lexer( void )
{
   Mass    = NULL ;
   Mass1   = NULL ;
   Token   = NULL ;
   l_Mass  = 0 ;
   l_Mass1 = 0 ;
   n_Token = 0 ;
   clr_breaker( );
}
/***************************************************************************/
t_Lexer :: ~t_Lexer( void )
{
   Free( Mass );
   Free( Mass1 );
   Free( Token );
}
/***************************************************************************/
//    set_breaker( ".,;:#$%^=+-*/(){}<>[]\"\\" );
void t_Lexer :: set_breaker( unsigned char *Breaker )
{
   for( short i=0 ; i<256 && Breaker[i]!=0 ; i++ )
      Type[Breaker[i]]=1 ;
}
/***************************************************************************/
void t_Lexer :: clr_breaker( void )
{
   for( short i=0 ; i<256 ; i++ )
      Type[i]=0 ;
   Type[' '] =2 ; Type['\t']=2;
   Type['\n']=3 ; Type['\r']=3 ;
}
/***************************************************************************/
void t_Lexer :: init( char *_Mass, long _L )
{
   Free( Mass ); Free( Mass1 ); Free( Token );

   Mass =(char    *)Calloc(_L,sizeof(char));
   Token=(t_Token *)Calloc( max(3,_L/2) ,sizeof(t_Token));
   l_Mass=_L ;
   for( long i=0 ; i<_L ; i++ )
      Mass[i]=_Mass[i] ;

}
/***************************************************************************/
/*                     создать из текста массив слов                       */
/***************************************************************************/
void t_Lexer :: make_token( void )
{
   long  i,i1,j_Token=0 ;
   char  t,t1,f=0 ;
   unsigned char c,c1,c2 ;

   for( i=0 ; i<l_Mass ; i++ )
   {
      c=Mass[i] ; c1=Mass[i+1] ;
      t=Type[c] ; t1=Type[c1] ;

      // ------ комментарии взвести
      if( f==0 )
      {  if( c=='/' && c1=='/' ) f=1 ;
	 if( c=='/' && c1=='*' ) f=2 ;
      }
      if( c=='\n' ) {  if( f==1 ) f=0 ;  }
      if( f==0 )
      {
	 if( t==1 ) // ------ если Token-разделитель ----------
	 {
	    Token[j_Token].begin =i ;
	    Token[j_Token].end   =i+1 ;
	    Token[j_Token].length=1 ;
	    j_Token++ ;
	 }
	 if( t!=0 && t1==0 ) // ------ если нормальный Token ----------
	 {
	    c2=Mass[i+2] ;
            if( c1=='/' && (c2=='/' || c2=='*') ) continue ;
	    for( i1=i+1 ; i1<l_Mass ; i1++ )
	       if( Type[Mass[i1]]!=0 ) break ;
	    Token[j_Token].begin =i+1 ;
	    Token[j_Token].end   =i1-1 ;
	    Token[j_Token].length=i1-i-1 ;
	    j_Token++ ;
	 }
      }

      // ------ комментарии сбросить
      //if( c=='\n' ) {  if( f==1 ) f=0 ;  }
      if( f==2 ) if( c=='*' && c1=='/' ) { f=0 ; i++ ; }
   }
   n_Token=j_Token ;
}
/***************************************************************************/
char * t_Lexer :: token( long i )
{  static char Str[100] ;
   for( short i1=0 ; i1<100 ; i1++ ) Str[i1]=0 ;
   strncpy( Str,Mass+Token[i].begin,Token[i].length );
   return Str ;
}
