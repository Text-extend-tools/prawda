/***************************************************************************/
//              "��������"-"������" - open source ����������
//          ���������������� � ����������� � ��������� GNU v 2.0
// 
//              ������ ������, �� ����� �������� ������
//       �������� �.�.                                    �������
/***************************************************************************/
# include <stdlib.h>
# include <string.h>
# include "mylib.h"

static uchar  SC[256] ;
       uchar T_to_lower[256] ;
static uchar T_koi_win[256] ;
static uchar T_win_koi[256]=
{ 0,   1,   2,   3,   4,   5,   6,   7,
  8,   9,  10,  11,  12,  13,  14,  15,
 16,  17,  18,  19,  20,  21,  22,  23,
 24,  25,  26,  27,  28,  29,  30,  31,
 32,  33,  34,  35,  36,  37,  38,  39,
 40,  41,  42,  43,  44,  45,  46,  47,
 48,  49,  50,  51,  52,  53,  54,  55,
 56,  57,  58,  59,  60,  61,  62,  63,
 64,  65,  66,  67,  68,  69,  70,  71,
 72,  73,  74,  75,  76,  77,  78,  79,
 80,  81,  82,  83,  84,  85,  86,  87,
 88,  89,  90,  91,  92,  93,  94,  95,
 96,  97,  98,  99, 100, 101, 102, 103,
104, 105, 106, 107, 108, 109, 110, 111,
112, 113, 114, 115, 116, 117, 118, 119,
120, 121, 122, 123, 124, 125, 126, 127,
128, 129, 130, 131, 132, 133, 134, 135,
136, 137, 138, 139, 140, 141, 142, 143,
144, 145, 146, 147, 148, 149, 150, 151,
152, 153, 154, 155, 156, 157, 158, 159,
160, 161, 162, 163, 164, 165, 166, 167,
168, 169, 170, 171, 172, 173, 174, 175,
176, 177, 178, 179, 180, 181, 182, 183,
184, 185, 186, 187, 188, 189, 190, 191,
225, 226, 247, 231, 228, 229, 246, 250,
233, 234, 235, 236, 237, 238, 239, 240,
242, 243, 244, 245, 230, 232, 227, 254,
251, 253, 255, 249, 248, 252, 224, 241,
193, 194, 215, 199, 196, 197, 214, 218,
201, 202, 203, 204, 205, 206, 207, 208,
210, 211, 212, 213, 198, 200, 195, 222,
219, 221, 223, 217, 216, 220, 192, 209 };

static char   Error[10000]="" ;

struct t_SC{  t_SC(void);  };

t_SC SC1 ;

/***************************************************************************/
void  *Calloc( long N, long S )
{  void *v ;
   if( N*S <=0 ) return NULL ;
   v=calloc( N,S );
   if( v==NULL ) throw(Err_Memory);
   return v ;
}
/***************************************************************************/
void  *Malloc( long N, long S )
{  void *v ;
   if( N*S <=0 ) return NULL ;
   v=malloc( N*S );
   if( v==NULL ) throw(Err_Memory);
   return v ;
}
/***************************************************************************/
void  *Realloc( void *M, long L )
{  void *v ;
   if( L <=0 ) return NULL ;
   v=realloc( M,L );
   if( v==NULL ) throw(Err_Memory);
   return v ;
}
/***************************************************************************/
FILE  *Fopen( char *Name, char *a )
{  FILE *f ;
   f=fopen( Name, a );
   if( f==NULL ) 
   {   sprintf( Error,"Error fopen(%s)\n",Name );
       throw(Err_Fopen);
   }
   return f ;
}
/***************************************************************************/
void   Fread( void *M, long S, long N, FILE *File )
{  long n ;
   n=fread( M, S, N, File );
   if( n!=N ) throw(Err_Fread);
}
/***************************************************************************/
void   Fwrite( void *M, long S, long N, FILE *File )
{  long n ;
   n=fwrite( M, S, N, File );
   if( n!=N ) throw(Err_Fwrite);
}
/***************************************************************************/
void   Fseek( FILE *fw, long Offset, long Mode )
{
   if( 0!=fseek( fw, Offset, Mode ) )throw(-1);
}
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
long file_to_mass( char *File, char **_Mass )
{
   char *Mass ; long L ; FILE *fr ;
   
   fr=Fopen( File,"r" );
   L=FileLength( fr );
   Mass=(char *)Calloc( L,1 );
   Fread( Mass,1,L,fr );
   Fclose(fr);

   *_Mass=Mass ;
   return L ;
}
/***************************************************************************/
void kodepage_reverse( uchar *T_reverse, uchar *T )
{  short i ;

   for( i=0 ; i<256 ; i++ )
      T_reverse[T[i]]=i ;
}
/***************************************************************************/
t_SC :: t_SC( void )
{  short i ;

   for( i=0 ; i<256 ; i++ )
      SC[i]=i ;
   // ----- ������������ ��������� ������� � ��������� ----
   for( i=65 ; i<91 ; i++ )
      SC[i]=i+32 ;
   // ----- ������������ ������� ������� � ��������� ----
   SC[(uchar)'�']=SC[(uchar)'�']=128 ;
   SC[(uchar)'�']=SC[(uchar)'�']=129 ;
   SC[(uchar)'�']=SC[(uchar)'�']=130 ;
   SC[(uchar)'�']=SC[(uchar)'�']=131 ;
   SC[(uchar)'�']=SC[(uchar)'�']=132 ;
   SC[(uchar)'�']=SC[(uchar)'�']=133 ;
   SC[(uchar)'�']=SC[(uchar)'�']=134 ;
   SC[(uchar)'�']=SC[(uchar)'�']=135 ;
   SC[(uchar)'�']=SC[(uchar)'�']=136 ;
   SC[(uchar)'�']=SC[(uchar)'�']=137 ;
   SC[(uchar)'�']=SC[(uchar)'�']=138 ;
   SC[(uchar)'�']=SC[(uchar)'�']=139 ;
   SC[(uchar)'�']=SC[(uchar)'�']=140 ;
   SC[(uchar)'�']=SC[(uchar)'�']=141 ;
   SC[(uchar)'�']=SC[(uchar)'�']=142 ;
   SC[(uchar)'�']=SC[(uchar)'�']=143 ;
   SC[(uchar)'�']=SC[(uchar)'�']=144 ;
   SC[(uchar)'�']=SC[(uchar)'�']=145 ;
   SC[(uchar)'�']=SC[(uchar)'�']=146 ;
   SC[(uchar)'�']=SC[(uchar)'�']=147 ;
   SC[(uchar)'�']=SC[(uchar)'�']=148 ;
   SC[(uchar)'�']=SC[(uchar)'�']=149 ;
   SC[(uchar)'�']=SC[(uchar)'�']=150 ;
   SC[(uchar)'�']=SC[(uchar)'�']=151 ;
   SC[(uchar)'�']=SC[(uchar)'�']=152 ;
   SC[(uchar)'�']=SC[(uchar)'�']=153 ;
   SC[(uchar)'�']=SC[(uchar)'�']=154 ;
   SC[(uchar)'�']=SC[(uchar)'�']=155 ;
   SC[(uchar)'�']=SC[(uchar)'�']=156 ;
   SC[(uchar)'�']=SC[(uchar)'�']=157 ;
   SC[(uchar)'�']=SC[(uchar)'�']=158 ;
   SC[(uchar)'�']=SC[(uchar)'�']=160 ;
   SC[(uchar)'�']=SC[(uchar)'�']=161 ;

// ----- ����������� ������� � ��������� ---------------
   for( i=0 ; i<256 ; i++ )
      T_to_lower[i]=i ;

   T_to_lower[(uchar)'A']='a' ;
   T_to_lower[(uchar)'B']='b' ;
   T_to_lower[(uchar)'C']='c' ;
   T_to_lower[(uchar)'D']='d' ;
   T_to_lower[(uchar)'E']='e' ;
   T_to_lower[(uchar)'F']='f' ;
   T_to_lower[(uchar)'G']='g' ;
   T_to_lower[(uchar)'H']='h' ;
   T_to_lower[(uchar)'I']='i' ;
   T_to_lower[(uchar)'J']='j' ;
   T_to_lower[(uchar)'K']='k' ;
   T_to_lower[(uchar)'L']='l' ;
   T_to_lower[(uchar)'M']='m' ;
   T_to_lower[(uchar)'N']='n' ;
   T_to_lower[(uchar)'O']='o' ;
   T_to_lower[(uchar)'P']='p' ;
   T_to_lower[(uchar)'Q']='q' ;
   T_to_lower[(uchar)'R']='r' ;
   T_to_lower[(uchar)'S']='s' ;
   T_to_lower[(uchar)'T']='t' ;
   T_to_lower[(uchar)'U']='u' ;
   T_to_lower[(uchar)'V']='v' ;
   T_to_lower[(uchar)'W']='w' ;
   T_to_lower[(uchar)'X']='x' ;
   T_to_lower[(uchar)'Y']='y' ;
   T_to_lower[(uchar)'Z']='z' ;

   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;
   T_to_lower[(uchar)'�']='�' ;

   kodepage_reverse( T_koi_win,T_win_koi );
}
/***************************************************************************/
uchar  *SortChar( void )
{
   return SC ;
}
/***************************************************************************/
uchar  *Koi_to_Win( void )
{
   return T_koi_win ;
}
/***************************************************************************/
uchar  *Win_to_Koi( void )
{
   return T_win_koi ;
}
/***************************************************************************/
//                         �������� ��� ������                             //
/***************************************************************************/
short Strcmp( char *Str1, char *Str2 )
{  short i,c ;
   register uchar c1,c2 ;

   for( i=0 ;  ; i++ )
   {  c1=(uchar)Str1[i] ; c2=(uchar)Str2[i] ;
      if( !(c1 && c2) ) return c1-c2 ;
      if( c1==c2 ) continue ;
      c = SC[c1] - SC[c2] ;
      if( c!=0 ) return c ;
   }
}
/***************************************************************************/
//                         �������� ��� ������                             //
/***************************************************************************/
short Strcmp1( char *Str1, char *Str2 )
{  short i,c ;
   register uchar c1,c2 ;

   for( i=0 ;  ; i++ )
   {  c1=(uchar)Str1[i] ; c2=(uchar)Str2[i] ;
      if( !(c1 && c2) ) return c1-c2 ;
      if( c1==c2 ) continue ;
      c = SC[c1] - SC[c2] ;
      if( c!=0 ) return c ;
   }
}
/***************************************************************************/
//                           �������� ������                               //
/***************************************************************************/
short Strncmp( char *Str1, char *Str2, short L )
{  short i,c ;

   for( i=0 ; i<L ; i++ )
   {  
      if( Str1[i]==0 && Str2[i]!=0 ) return -1 ;
      if( Str1[i]!=0 && Str2[i]==0 ) return  1 ;
      if( Str1[i]==0 && Str2[i]==0 ) return  0 ;
      c = SC[(uchar)Str1[i]] - SC[(uchar)Str2[i]] ;
      if( c!=0 ) return c ;
   }
   return 0 ;
}
/***************************************************************************/
void Strcpy( char *Str1, char *Str2, int Lmax )
{  short i ;
   for( i=0 ; i<Lmax-1 ; i++ )
   {  Str1[i]=Str2[i] ;
      if( Str2[i]==0 ) return ;
   }
   Str1[Lmax-1]=0 ;
}
/***************************************************************************/
void Strcat( char *Str1, char *Str2, int Lmax )
{  short i,j ;

   j=strlen( Str1 );
     
   for( i=0 ; i<Lmax-1 ; i++ )
   {  if( j==Lmax-1 ) break ;
      Str1[j++]=Str2[i] ;
      if( Str2[i]==0 ) return ;
   }
   Str1[Lmax-1]=0 ;
}
/***************************************************************************/
/*                          ������� ����������                             */
/***************************************************************************/
long   remark( char *Mass, long l_Mass )
{  long i,j ;
   char f=0 ;

   for( i=j=0 ; i<l_Mass-1 ; i++ )
   {  if( f==0 && Mass[i]=='/' && Mass[i+1]=='/' ) f=1 ;
//      if( f==0 && Mass[i]=='#' ) f=1 ;
      if( f==0 && Mass[i]=='/' && Mass[i+1]=='*' ) f=2 ;
      if( f==0 || Mass[i]=='\n' ) Mass[j++]=Mass[i] ;
      if( 0<i ) if( f==2 && Mass[i-1]=='*' && Mass[i]=='/' ) f=0 ;
      if( f==1 && Mass[i]=='\n') f=0 ;
   }
   if( f==0 ) Mass[j++]=Mass[i] ;
   return j ;
}
/***************************************************************************/
static char Emptystr[1100] ;
char *emptystr( short n )
{  short i ;
   if( 1099<n )
   {  printf("������ ������ ������������ ����� ������� ������ ������?"); exit(-1); }
   for( i=0 ; i<n ; i++ )
      Emptystr[i]=' ' ;
   Emptystr[i]=0 ;
   return Emptystr ;
}
/***************************************************************************/
void   error_set( char *s )
{
   strcpy( Error,s );
}
/***************************************************************************/
void   error_add( char *s )
{
   strcat( Error,s );
}
/***************************************************************************/
char  *error_get( void )
{
   return Error ;
}
/***************************************************************************/
void provokator( )
{
   for( long i=0 ; i<10000 ; i++ )
   {  char *c=(char *)Malloc( i,3 );
      Free( c );
   }
}
/***************************************************************************/
void list_error( void )
{
   error_set( "������ � t_List\n" );
   throw( Err_List );
}
