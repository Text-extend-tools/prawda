/***************************************************************************/
//              "��������"-"������" - open source ����������
//          ���������������� � ����������� � ��������� GNU v 2.0
//
//              �����, ����������� ��������� ���� �� �����
//       �������� �.�.                                    �������
/***************************************************************************/
# include <string.h>
# include "mylib.h"
# include "lexer.h"

# define Ifstr( a,b ) if( 0==strcmp(a,b) )

/***************************************************************************/
t_Lexer ::  t_Lexer( void )
{
   Mass   = NULL ;
   Mass1  = NULL ;
   Word   = NULL ;
   Str    = NULL ;
   l_Mass = 0 ;
   n_Word = 0 ;
   clr_breaker( );
}
/***************************************************************************/
t_Lexer :: ~t_Lexer( void )
{
   Free( Mass );
   Free( Mass1 );
   Free( Word );
   Free( Str );
}
/***************************************************************************/
//          ���������� ����� ������� �������� ������������� ����
/***************************************************************************/
//    set_breaker( ".,;:#$%^=+-*/(){}<>[]\"\\" );
void t_Lexer :: set_breaker( unsigned char *Breaker )
{
   for( short i=0 ; i<256 && Breaker[i]!=0 ; i++ )
      Type[Breaker[i]]=1 ;
}
/***************************************************************************/
//         ���������� ��������� �������, ����������� ����
/***************************************************************************/
void t_Lexer :: clr_breaker( void )
{
   for( short i=0 ; i<256 ; i++ )
      Type[i]=0 ;
   Type[' '] =2 ;Type['\t']=2;
   Type['\n']=3 ;Type['\r']=3 ;
}
/***************************************************************************/
//        ������ ������, ������� ����� ����������� �� �����
/***************************************************************************/
void t_Lexer :: init( char *_Mass, long _L )
{
   Free( Mass ); Free( Mass1 ); Free( Word ); Free( Str );

   Mass=(char *)Calloc(_L,sizeof(char));
   Str =(long *)Calloc( max(3,_L/2) ,sizeof(long)); 
   // ����� ���� ���� ��������� ����� ���������
   l_Mass=_L ;
   memcpy( Mass,_Mass,l_Mass );
   remark( );
}
/***************************************************************************/
void t_Lexer :: init1( char *_Mass, long _L )
{
   Free( Mass ); Free( Mass1 ); Free( Word ); Free( Str );

   Mass=(char *)Calloc(_L,sizeof(char));
   Str =(long *)Calloc( max(3,_L/2) ,sizeof(long)); 
   // ����� ���� ���� ��������� ����� ���������
   l_Mass=_L ;
   memcpy( Mass,_Mass,l_Mass );
   remark1( );
}
/***************************************************************************/
//                          ������� ����������
/***************************************************************************/
void t_Lexer :: remark( void )
{  long i,j,N ;
   char  c1,c2=' ' ;
   register char f,c ;

   N=l_Mass-1 ;
   f=0 ;
   for( i=j=0 ; i<N ; i++ )
   {  c=Mass[i] ; 
      if( f==0 )
      {
         f=( c=='#' );
	 if( c=='/' )
	 {   c1=Mass[i+1] ;
	     if( c1=='/' ) f=1 ;
	     if( c1=='*' ) f=2 ;
	 }
      }
      if( c=='\n' && f==1 ) f=0 ;
      if( f==0 ) continue ;
          else   Mass[i]=' ' ;
      if( f==2 && c2=='*' && c=='/' ) f=0 ;      
      c2=c ;
   }
}
/***************************************************************************/
//                          ������� ����������
/***************************************************************************/
void t_Lexer :: remark1( void )
{  long i,j,N ;
   char  c1,c2=' ' ;
   register char f,c ;

   N=l_Mass-1 ;
   f=0 ;
   for( i=j=0 ; i<N ; i++ )
   {  c=Mass[i] ; 
      if( f==0 )
      {
	 if( c=='/' )
	 {   c1=Mass[i+1] ;
	     if( c1=='/' ) f=1 ;
	     if( c1=='*' ) f=2 ;
	 }
      }
      if( f==0 ) continue ;
          else   Mass[i]=' ' ;
      if( c=='\n' && f==1 ) f=0 ;
      if( f==2 && c2=='*' && c=='/' ) f=0 ;      
      c2=c ;
   }
}
/***************************************************************************/
//                     ������� �� ������ ������ ����
/***************************************************************************/
void t_Lexer :: make_words( void )
{
   long i,j,j_Word,n,j_Str=0 ;
   register char t,c ;

   Mass1=(char  *)Calloc( 2*l_Mass,sizeof(char) );

   for( i=j=n=0 ; i<l_Mass ; i++ )
   {  c=Mass[i] ;
      
      if( 0<=c )
      {  t=Type[c] ;
	 
	 if( t==1 ) // ------ ���� ����������� ----------
	 {
	    if( 0<j && Mass1[j-1]!=0 )
	    {   Mass1[j++]=0 ; Str[n]=j_Str ; n++ ;  }
	    Mass1[j++]=c ;
	    Mass1[j++]=0 ;
	    Str[n]=j_Str ; /* ABW1 */
	    n++ ;
	    continue ;
	 }

	 if( t==2 || t==3 ) // ------ ���� ����� ������ ----------
	 {  if( Mass[i]=='\n' ) j_Str++ ;
	    if( 0<j && Mass1[j-1]!=0 )
	    {   Mass1[j++]=0 ; Str[n]=j_Str ; n++ ;  }
	    continue ;
	 }
      }
      Mass1[j++]=c ;
   }
   if( 0==Type[Mass[l_Mass-1]] ) // ������� ����� ���������� �����
   {  Mass1[j++]=0 ; n++ ;  }
   
   n_Word=n ;
   Word =(char **)Calloc( n_Word+1,sizeof(char *));

   Word[0]=Mass1 ;
   for( i=j_Word=0 ; i<j ; i++ )
   {
      if( 0==Mass1[i] )
      {
	 Word[++j_Word]=Mass1+i+1 ;
      }
   }
}
/* ---------------------------------------------------------------------- */
char* t_Lexer :: word( long i )
{  return Word[i] ;  }
/* ---------------------------------------------------------------------- */
long t_Lexer :: n_word( void )
{  return n_Word ;  }
/* ---------------------------------------------------------------------- */
long t_Lexer :: str( long i )
{  return Str[i] ;  }
/**************************************************************************/
//    ����� ����������� ������ ��� ��������������� �����������
//    Begin - � ������ ����� �������� ������
//    Open  - ��� �� ������� ����������� �������
//    Close - ��� �� ������� ����������� �������
/**************************************************************************/
long t_Lexer :: find_paar( long Begin, char *Open, char *Close )
{  long i,s=-1 ;

   for( i=Begin ; i<n_Word ; i++ )
      if( 0==strcmp(Word[i],Open) )  {  s=0 ; break ;  }
   if( s<0 ) return Begin ;
   for( ; i<n_Word ; i++ )
   {  Ifstr( Word[i],Open ) s++ ;
      Ifstr( Word[i],Close) s-- ;
      if( s==0 ) return i ;
   }
   return -1 ;
}
/**************************************************************************/
//    ����� � ������ ������ �����
//    Begin - � ������ ����� �������� ������
//    W     - ��� �� ����� �� ����
/**************************************************************************/
long t_Lexer :: find_word( long Begin, char *W )
{
   for( long i=Begin ; i<n_Word ; i++ )
      if( 0==strcmp(Word[i],W) )  return i ;
   return -1 ;
}
/* ---------------------------------------------------------------------- */
void t_Lexer :: error( long i )
{  printf("\n ������ %ld ����������� ����� %s",Str[i],Word[i] );
   throw( -1 );
}


