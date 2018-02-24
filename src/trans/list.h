/***************************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//              Массив, которому можно дописывать в конец
//       Анисимов Д.В.                                    сегодня
/***************************************************************************/
# ifndef LIST_HPP
# define LIST_HPP

# include <malloc.h>

void list_error( void );

# define DECLARE_LIST_TYPE( t,t1 )  \
				    \
class t1                    \
{                                   \
public:                             \
   long  j,n ;                      \
   t    *list ;                     \
				    \
	 t1( void );                \
	~t1( void );                \
   long  size( void );              \
   void  operator = ( t1 &a );      \
   t &   operator []( long i );     \
   long  add( t &v );               \
   void  resize( void );            \
   void  init( void );              \
   void  del( void );               \
} ;

# define COMMON_FUNKTION( t,t1 )    \
				    \
t1 :: t1( void )                    \
{  init();  }                       \
				    \
void t1 :: init( void )             \
{                                   \
   j=n=0 ;                          \
   list=NULL ;                      \
}                                   \
				    \
t1 :: ~t1( void )                   \
{                                   \
   del();                           \
}                                   \
				    \
long t1 :: size ( void )            \
{                                   \
   return n*sizeof(t1);             \
}                                   \
				    \
void t1 :: operator = ( t1 &a )                    \
{                                                  \
   if( a.j>a.n || a.n>0 && a.list==NULL )          \
   {  printf("\n Incorrect data in operator =" );  \
      throw( a ) ;                                 \
   }                                               \
   if( this==&a ) return ;                         \
   del();                                          \
   /* if( list!=NULL ) free( list ); */            \
   j=a.j ;                                         \
   n=a.n ;                                         \
   if( a.list==NULL ){  list=NULL ; return ;  }    \
   list=(t *)calloc(n,sizeof(t));                  \
   if( list==NULL ) throw( (char )-1 );            \
   for( long i=0 ; i<j ; i++ )                     \
      list[i]=a.list[i] ;                          \
}                                                  \
						   \
t& t1 :: operator [] (long i)                      \
{  if( i<0 || j<=i )                               \
   {  printf("\n Error index in t_List %ld %ld",i,j);\
      list_error();                                \
   }                                               \
   return list[i];                                 \
}                                                  \
						   \
void t1 :: resize( void )                          \
{                                                  \
n=j ;                                              \
list=(t *)realloc( list,n*sizeof(t) );             \
if( list==NULL ) throw( (char )-1 );               \
}                                                  \


# define DEFINE_LIST_TYPE( t,t1 )                  \
						   \
COMMON_FUNKTION( t,t1 )                            \
						   \
void t1 :: del( void )                             \
{  long i ;                                        \
   for( i=0 ; i<j ; i++ )                          \
      list[i].del() ;                              \
   j=n=0 ;                                         \
   if( list!=NULL ) {  free(list) ; list=NULL ;  } \
}                                                  \
						   \
long t1 ::  add( t &v )                            \
{                                                  \
long i ;                                           \
t*   list1 ;                                       \
						   \
if( n<=0 )                                         \
{  n=4 ;                                           \
   list=(t *)calloc( n,sizeof(t) );                \
   if( list==NULL ) throw( (char )-1 );            \
   for( i=j ; i<n ; i++ )                          \
      list[i].init( ) ;                            \
}                                                  \
						   \
if( j>=n )                                         \
{  n=n+n/4 ;                                       \
   list1=(t *)realloc( list,n*sizeof(t) );         \
   if( list1==NULL ) throw( (char )-1 );           \
   list=list1 ;                                    \
   for( i=j ; i<n ; i++ )                          \
      list[i].init( ) ;                            \
}                                                  \
list[j++]=v ;                                      \
return j-1 ;                                       \
}                                                  \


# define DEFINE_LIST_BTYPE( t,t1 )                 \
						   \
COMMON_FUNKTION( t,t1 )                            \
						   \
void t1 :: del( void )                             \
{                                                  \
   j=n=0 ;                                         \
   if( list!=NULL ) {  free(list) ; list=NULL ;  } \
}                                                  \
						   \
long t1 ::  add( t &v )                            \
{                                                  \
t*   list1 ;                                       \
						   \
if( n<=0 )                                         \
{  n=4 ;                                           \
   list=(t *)calloc( n,sizeof(t) );                \
   if( list==NULL ) throw( (char )-1 );            \
}                                                  \
						   \
if( j>=n )                                         \
{  n=n+n/4 ;                                       \
   list1=(t *)realloc( list,n*sizeof(t) );         \
   if( list1==NULL ) throw( (char )-1 );           \
   list=list1 ;                                    \
}                                                  \
list[j++]=v ;                                      \
return j-1 ;                                       \
}                                                  \

# endif

