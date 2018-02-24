/***************************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                            ЛАЗАЛКА ПО ДИСКУ
//     Анисимов Д.В.                                         сегодня
/***************************************************************************/
# include <time.h>
# include <dirent.h>
# ifdef __WIN__
   # include <dir.h>
#else
   # include <unistd.h>
#endif

# include <sys/types.h>
# include <sys/stat.h>

# include <errno.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>

# include "mylib.h"
# include "dir1.h"
# include "color.h"
# include "video.h"
# include "window.h"

# ifdef __WIN__
   #define SLASH   '\\'
   #define SLASH1  "\\"
# else
   #define SLASH   '/'
   #define SLASH1  "/"
# endif

# define LENGTH_DIR 500

# ifndef __WIN__
e_WinMsg kommand_yxf( short y, short x, char f, char *str, short lenght,
		   char EIN, char **str1=NULL, short n_str1=0 );
# endif

char   schablon( char *s, char *n );
char   name_test( char *str );
void   name_zerstoren( char *sou, char *dst1, char *dst2 );
char   schablon1( char *s, char *n);

static short    File_Line=5, File_Pos=1,
		Line_Path=2, Pos_Path=3 ;

/* *********************************************************************** */

/***************************************************************************/
short t_Dir :: direktor( char *Antwort, char fsave )
{   short reg ;
    static char f=0 ;

    Antwort[0]=0 ;
    if( f==0 )
    {  File=(t_File1 *)Calloc( 500, sizeof(t_File1) );
       strcpy( shabl,"*.*" ) ;
    }

    karkas( fsave ) ;

    if( f==0 )
    {   getcwd( Path,100 );
	if( SLASH!=Path[strlen(Path)-1] ) strcat( Path,SLASH1 );
    }
    f=1 ;    
    paint_schabl( );

    reg=5 ;  /* -------------------------------- цикл работы ---- */
    while( 1 )
    {
       help( ) ;
       switch( reg )
       {  case  3 : in_schabl( ); reg=5 ; break ;
	  case  4 : if( 1==in_name( ) )
		    {  strcpy( Antwort,Path );
		       strcat( Antwort,Name ) ;
		       goto Ausgang ;
		    }
		    reg=5 ;
		    break ;
	  case  5 : reg=in_file( );
		    if( reg== -1 ) goto Ausgang ;
		    if( reg==0 )
		    {  strcpy( Antwort,Path );
		       strcat( Antwort,File[i_File].name ) ;
		       goto Ausgang ;
		    }
		    break ;
	  default : reg=5 ;
       }
    }
Ausgang :
   s_nacht( ) ;
   return reg ;
}
/* ----------------------------------------------------------------------- */
void t_Dir :: set_schabl( char *str )
{   strcpy( shabl,str ) ;  }
/* ----------------------------------------------------------------------- */
void t_Dir :: get_schabl( char *str )
{  strcpy( str,shabl ) ;  }
/* ----------------------------------------------------------------------- */
void t_Dir :: in_schabl( void )
{  char str[32] ;

   strcpy( str,shabl ) ;
   kommand_yxf( 8,52,schabl_Color,str,26,0 ) ;
   if( 0==name_test( str ) ) strcpy( shabl,str );
   s_text_yxf( 8,52,schabl_Color,emptystr(26) ) ;
   paint_schabl( ) ;
}
/* ----------------------------------------------------------------------- */
void t_Dir :: paint_schabl( void )
{  short l ;
   l=strlen(shabl) ;
   s_text_yxf1( 8,52  ,dir_file_Color,shabl ) ;
   s_text_yxf(  8,54+l,dir_file_Color,emptystr(24-l) ) ;
}
/* ----------------------------------------------------------------------- */
short t_Dir :: in_name( void )
{
   char str[32] ;

   str[0]=0 ;
   kommand_yxf( 11,52,schabl_Color,str,26,0 ) ;
   if( str[0]==0 ) goto M1 ;
   if( 0==name_test( str ) )
   {   strcpy( Name,str ) ;
       return 1 ;
   }

M1 : s_text_yxf( 11,52,dir_rame_Color,emptystr(26) ) ;
     return 0 ;
}
/* ----------------------------------------------------------------------- */
char name_test( char *str )
{  short i,p,l ;
   char  c ;

   p=l=strlen( str ) ;
   if( l>12 ) goto M1 ;
   for( i=0 ; i<l ; i++ )
      if( str[i]=='.' ){ p=i ; break ; }
   if( p>8 || l-p>4 ) goto M1 ;
   for( i=0 ; i<p ; i++ )
   {  c=str[i] ;
      if( ('!'<=c && c<='&') || ('0'<=c && c<='9')
	|| ('?'<=c && c<='Z') || ('a'<=c && c<='z')
	|| c=='*' || c=='_' ) ;
       else goto M1 ;
   }
   for( i=p+1 ; i<l ; i++ )
   {  c=str[i] ; 
      if( ('!'<=c && c<='&') || ('0'<=c && c<='9')
	|| ('?'<=c && c<='Z') || ('a'<=c && c<='z')
	|| c=='*' || c=='_' ) ;
       else goto M1 ;
   }
   return 0 ;
M1:
return -1 ;
}
/* ----------------------------------------------------------------------- */
short t_Dir :: in_file( void )
{  short i ;
   static short taste1,taste2 ;
   char  Path1[200] ;

strcpy( Path1,Path ) ;
read_file( Path ) ;
for( i=2 ; i<n_Line-5 ; i++ )
   s_text_yxf( File_Line+i,File_Pos,dir_file_Color,emptystr(49) );

Begin=i_File=0 ;
while( 1 )
{   paint_file( 0 ) ;

    s_getch( &taste1,&taste2 );
    switch( taste1 )
    {   case 0 :
	    switch( taste2 )
	    {   case S_key_Down : i_File++ ; break ;
		case S_key_Up   : i_File-- ; break ;
		case S_key_PgUp : i_File-=n_Line-8 ; break ;
		case S_key_PgDn : i_File+=n_Line-8 ; break ;
		case S_key_Home : i_File=0 ; break ;
		case S_key_End  : i_File=n_File-1 ; break ;
		case S_key_F3 : paint_file( 1 ) ; return 3 ;
		case S_key_F4 : paint_file( 1 ) ; return 4 ;

		case S_key_F10 : return -1 ;
	    }
	    if( i_File<0 ) i_File=0 ;
	    if( i_File<Begin ) Begin=i_File ;
	    if( i_File>=n_File ) i_File=n_File-1 ;
	    if( Begin+n_Line-8<i_File ) Begin=i_File-(n_Line-8) ;
	    break ;
	case '\r' :
	    if( File[i_File].type==0 ) return 0 ;
	    else
	    {  if( 0==strcmp( File[i_File].name,".." ) )
	       {  for( i=strlen( Path )-2 ; 0<=i ; i-- )
		     if( Path[i]==SLASH ){  Path[i+1]=0 ; break ;  }
	       }
	       else
	       {  strcpy( Path1,Path );
	          strcat( Path1,File[i_File].name );
	          if( 0>access(Path1,X_OK) ) break ;
	          strcat( Path,File[i_File].name ); strcat( Path,SLASH1 ); 
	       }
	       read_file( Path ) ;
	       Begin=i_File=0 ;
	    }
	    for( i=2 ; i<n_Line-5 ; i++ )
	       s_text_yxf( File_Line+i,File_Pos,dir_file_Color,emptystr(49) );
	    break ;
    }
}
}
/* ----------------------------------------------------------------------- */
void t_Dir :: karkas( char fsave )
{
   s_nacht();

   s_rame2_F( 1,  0, 3, 79, dir_rame_Color );
   s_rame2_F( 4, 51, 6, 79, dir_rame_Color );
   s_rame2_F( 7, 51, 9, 79, dir_rame_Color );
   s_rame2_F(10, 51,12, 79, dir_rame_Color );

   s_rame2_F( 4, 0, n_Line-1, 50, dir_rame_Color );

   if( fsave==0 ) s_text_yxf( 0,30,0x0f,"Прочесть файл");
       else       s_text_yxf( 0,30,0x0f,"Записать файл как");
   s_text_yxf( 1, 4,titl_Color,"Полный путь (path):" );
   s_text_yxf( 4,56,taste_Color,"F2 ");   s_text_yxf( 3,59,titl_Color,"диски" );
   s_text_yxf( 7,56,taste_Color,"F3 ");   s_text_yxf( 6,59,titl_Color,"шаблон" );
   s_text_yxf(10,56,taste_Color,"F4 ");   s_text_yxf( 9,59,titl_Color,"новый файл" );
   s_text_yxf( 4, 9,titl_Color," файлы и каталоги " );
   s_text_yxf( 5, 3,titlchen_Color,"имя                     дата      время  размер" );
   s_text_yxf( 6, 3,dir_rame_Color,"---------------------------------------------" );
}
/***************************************************************************/
void t_Dir :: help( void )
{  short y=13,x=51 ;

   s_rame1_F(y,x,y+5,x+28,dir_rame_Color) ;
   s_text_yxf(y+1,x+2,dir_rame_Color,"Боже, Боже!");
   s_text_yxf(y+2,x+2,dir_rame_Color,"дай мне полчаса времени");
   s_text_yxf(y+3,x+2,dir_rame_Color,"source - код вселенной");
   s_text_yxf(y+4,x+2,dir_rame_Color,"и хороший отладчик...");
}
/* ----------------------------------------------------------------------- */
void t_Dir :: paint_file( char Mode )
{  char f,str[32],str1[50] ;
   short i,i1,s1 ;

   s_text_yxf( Line_Path,Pos_Path,path_Color,emptystr(40) ) ;
   s_text_yxf( Line_Path,Pos_Path,path_Color,Path ) ;

   for( i=Begin ; i<min(Begin+n_Line-7,n_File) ; i++ )
   {  if( i==i_File && Mode==0 ) f=cur_dir_Color ; else f=dir_file_Color ;
      file_date( str, File[i] ) ;
      s1=File_Line+i+2-Begin ;
      strcpy( str1,emptystr(49) ) ;
      for( i1=0 ; i1<15 ; i1++ )
      {   if( File[i].name[i1]==0 ) break ;
	  str1[i1]=File[i].name[i1] ;
      }
      for( i1=0 ; i1<23 ; i1++ )
	 str1[26+i1]=str[i1] ;

      s_text_yxf( s1,File_Pos,f,str1 ) ;
   }
}
/* ----------------------------------------------------------------------- */
char t_Dir :: read_file( char *path )
{
short j ;
char path1[100] ;

DIR *my_dir ;
struct dirent *ent ;
static struct stat stat1 ;

my_dir = opendir( path );
if( my_dir==NULL ) return 0 ;

j=0 ;
while( 1 )
{
   ent = readdir( my_dir ) ;
   if( ent == NULL ) break ;
   strcpy(path1,path); strcat(path1,SLASH1); strcat(path1,ent->d_name);
   stat( path1,&stat1 ) ;
   if( 0==(S_IFDIR&stat1.st_mode) )
   {  if( 0==schablon( shabl,ent->d_name) ) continue ;
      strcpy( File[j].name,ent->d_name ) ;
      File[j].type=0 ;
      File[j].date=stat1.st_mtime ;
      File[j].size=stat1.st_size ;
      j++ ;
   }
   else
   {  if( 0==strcmp( ent->d_name,".") ) continue ;
      strcpy( File[j].name,ent->d_name ) ;
      File[j].type=1 ;
      j++ ;
   }
}
closedir( my_dir ) ;

   n_File=j ;
   sort_file( );
return 0 ;
}
/* ----------------------------------------------------------------------- */
void t_Dir :: file_date( char *antwort, struct t_File1 &file )
{
short uhr,minut,
      tag,monate,jahr ;
long  size ;
short i,i1,j ;
char  c[10] ;

struct tm *tm1 = localtime( &file.date );
minut  = tm1->tm_min ;
uhr    = tm1->tm_hour ;
tag    = tm1->tm_mday ;
monate = tm1->tm_mon ;
jahr   = tm1->tm_year ;
size   = file.size ;

strcpy( antwort,"00:00:00  00:00        " ) ;

sprintf( c,"%d",tag );
for( i=0 ; c[i]!=0 ; i++ ) ;
for( j=2-i,i1=0 ; c[i1]!=0 ; i1++ ) antwort[j+i1]=c[i1] ;
sprintf( c,"%d",1+monate );
for( i=0 ; c[i]!=0 ; i++ ) ;
for( j=5-i,i1=0 ; c[i1]!=0 ; i1++ ) antwort[j+i1]=c[i1] ;
sprintf( c,"%d",jahr%100 );
for( i=0 ; c[i]!=0 ; i++ ) ;
for( j=8-i,i1=0 ; c[i1]!=0 ; i1++ ) antwort[j+i1]=c[i1] ;

sprintf( c,"%d",uhr );
for( i=0 ; c[i]!=0 ; i++ ) ;
for( j=12-i,i1=0 ; c[i1]!=0 ; i1++ ) antwort[j+i1]=c[i1] ;
sprintf( c,"%d",minut );
for( i=0 ; c[i]!=0 ; i++ ) ;
for( j=15-i,i1=0 ; c[i1]!=0 ; i1++ ) antwort[j+i1]=c[i1] ;

if( file.type==0 )
{  sprintf( c,"%ld",size );
   for( i=0 ; c[i]!=0 ; i++ ) ;
   for( j=23-i,i1=0 ; c[i1]!=0 ; i1++ ) antwort[j+i1]=c[i1] ;
}
else strncpy( antwort+18,"<Dir>",5 ) ;

}
/* ----------------------------------------------------------------------- */
void t_Dir :: sort_file( void )
{  struct t_File1 Buf ;
   short i,j ;
   char f1,f2,f3,f4,t,t1 ;

   for( j=0 ; j<n_File ; j++ )
      for( i=1 ; i<n_File ; i++ )
      {  t=File[i].type ; t1=File[i-1].type ;
         f1 = t==1 && t1==0 ;
	 f2 = t==0 && t1==0 ;
	 f3 = t==1 && t1==1 ;
	 f4 = 0>strcmp( File[i].name,File[i-1].name ) ;
	 if( f1 || (f2 && f4) || (f3 && f4) )
	 {  Buf=File[i] ; File[i]=File[i-1] ; File[i-1]=Buf ;  }
      }
}
/* ----------------------------------------------------------------------- */
char schablon( char *s, char *n )
{  char s1[32],s2[32] ;
   char n1[32],n2[32] ;
   char f ;

   name_zerstoren( s,s1,s2 );
   name_zerstoren( n,n1,n2 );
   f=schablon1(s1,n1) & schablon1(s2,n2) ;
   return f ;
}
/* ----------------------------------------------------------------------- */
void name_zerstoren( char *sou, char *dst1, char *dst2 )
{
   short n,i,p,f ;

   n=strlen(sou);
   for( f=0,i=n-1 ; 0<=i ; i-- )
      if( sou[i]=='.'){ f=1 ; p=i ; break ; }
   if( f==1 )
   {  strcpy( dst1,sou ); dst1[p]=0 ;
      strcpy( dst2,sou+p+1 );
   }
   else
   {  strcpy(dst1,sou);
      dst2[0]=0 ;
   }
}
/* ----------------------------------------------------------------------- */
char schablon1( char *s, char *n)
{
   short i,f ;

   if( s[0]=='*' ) return 1 ;
   for( i=0 ; i<32 ; i++ )
   {  if( s[i]!='?' && s[i]!=n[i] ) { f=0 ; break ; }
      if( s[i]==0 ) { f=1 ; break ; }
   }
   return f ;
}
