/***************************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//              Всякие мелкие, но очень полезные штучки
//       Анисимов Д.В.                                    сегодня
/***************************************************************************/
# ifndef MYLIB_H
# define MYLIB_H

# include <stdio.h>
# include <malloc.h>
# include <stdlib.h>

# define Err_Memory -2
# define Err_Fopen  -3
# define Err_Fread  -4
# define Err_Fwrite -5
# define Err_Data   -6
# define Err_List   -7

# define uchar       unsigned char

# define    max(a,b)    (((a) > (b)) ? (a) : (b))
# define    min(a,b)    (((a) < (b)) ? (a) : (b))
# define    Free( a )    if( a!=NULL ) {  free(a);   a=NULL ;  }
# define    Fclose( f )  if( f!=NULL ) {  fclose(f); f=NULL ;  }
# define    is_space(c)  ((c)==' ' || (c)=='\t' || (c)=='\n' || (c)=='\r')

void  *Calloc( long N, long S );
void  *Malloc( long N, long S );
void  *Realloc( void *M, long L );

FILE  *Fopen( char *Name, char *a );
void   Fread( void *M, long S, long N, FILE *File );
void   Fwrite( void *M, long S, long N, FILE *File );
void   Fseek( FILE *fw, long Offset, long Mode );
long   FileLength( FILE *fw );

short  Strcmp( char *Str1, char *Str2 );
short  Strcmp1( char *Str1, char *Str2 );
short  Strncmp( char *Str1, char *Str2, short L );
void   Strcpy( char *Str1, char *Str2, int Lmax );
void   Strcat( char *Str1, char *Str2, int Lmax );
char  *emptystr( short n );
uchar *SortChar( void );
uchar *Koi_to_Win( void );
uchar *Win_to_Koi( void );
long   remark( char *Mass, long l_Max );
long   file_to_mass( char *File, char **_Mass );

void   error_set( char *s );
void   error_add( char *s );
char  *error_get( void );
void   provokator( );

# endif
