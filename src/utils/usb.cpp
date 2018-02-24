/************************************************************************/
//       Удобная копировалка с флешки и на флешку
/************************************************************************/
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

# include "video.h"

struct t_File2
{  char name[200] ;   /* полное имя файла с расширением */
   long size ;        // размер файла
   long date ;
   long time ;
   char type ;        /* 0-файл 1-каталог */
   char status ;     // 0 -такой-же, 1-совсем новый, 2-отличающийся
   char copy ;       // копировать или нет
} ;

char       Path1[200],  // куда копировать
           Path2[200] ; // откуда копировать
t_File2   *File1, *File2 ;
long     j_File1,j_File2 ;

/************************************************************************/
//         прочесть каталог (рекурсивная)
/************************************************************************/
int read_file( char *Path, t_File2 *File, long &j_File )
{
   char path1[200] ;

   DIR *my_dir ;
   struct dirent *ent ;
   static struct stat stat1 ;

   my_dir = opendir( Path );
   if( my_dir==NULL ) return 0 ;

   while( 1 )
   {
      if( 1000<=j_File ) break ;
      ent = readdir( my_dir ) ;
      if( ent == NULL ) break ;
      strcpy(path1,Path); strcat(path1,"/"); strcat(path1,ent->d_name);
      stat( path1,&stat1 ) ;
      if( 0==(S_IFDIR&stat1.st_mode) )
      {  //if( 0==schablon( shabl,ent->d_name) ) continue ;
	 strcpy( File[j_File].name,path1 ) ;
	 File[j_File].type=0 ;
	 File[j_File].date=stat1.st_mtime ;
	 File[j_File].size=stat1.st_size ;
	 j_File++ ;
      }
      else
      {  if( 0==strcmp( ent->d_name,".") ) continue ;
         if( 0==strcmp( ent->d_name,"..") ) continue ;
         strcpy(path1,Path); strcat(path1,"/"); strcat(path1,ent->d_name);
         read_file( path1,File,j_File );
      }
   }
   closedir( my_dir ) ;
}
/************************************************************************/
//               прочесть два каталога
//   Path1 - старый каталог  Path2 - новый каталог
/************************************************************************/
void read_files( char *Path1, char *Path2 )
{  long i,L ;

   File1=(t_File2 *)calloc( 1000,sizeof(t_File2) );
   File2=(t_File2 *)calloc( 1000,sizeof(t_File2) );
   j_File1=j_File2=0 ;

   read_file( Path1,File1,j_File1 );
   read_file( Path2,File2,j_File2 );

   L=strlen( Path1 );
   for( i=0 ; i<j_File1 ; i++ )
      strcpy( File1[i].name,File1[i].name+L );
   L=strlen( Path2 );
   for( i=0 ; i<j_File2 ; i++ )
      strcpy( File2[i].name,File2[i].name+L );
}
/************************************************************************/
//           сравнить каталоги, отметить различающиеся файлы
/************************************************************************/
void compare(  )
{  long i1,i2 ;

   for( i2=0 ; i2<j_File2 ; i2++ )
   {  File2[i2].copy=0 ;
      for( i1=0 ; i1<j_File1 ; i1++ )
      {  if( 0==strcmp(File2[i2].name,File1[i1].name) )
         {   if( File1[i1].date<File2[i2].date )
	          File2[i2].status=2 ;
	     else File2[i2].status=0 ;
	     goto M_Found ;
	 }
      }
      File2[i2].status=1 ;
      M_Found:;
   }
}
/************************************************************************/
//                исключить из списка одинаковые файлы
/************************************************************************/
void reduce(  )
{  long i,j ;
   for( i=j=0 ; i<j_File2 ; i++ )
      if( 0!=File2[i].status )
          File2[j++]=File2[i] ;
   j_File2=j ;
}
/************************************************************************/
void paint( long begin, long current )
{  long   i ;
   short  sx,sy ;
   char  *ss,*s1,f ;
   char   Str[200] ;

   s_get_size( sx,sy );
   
   s_text_yxf( 0,1,0x0b,"Откуда" );
   s_text_yxf( 1,1,0x0b,"Куда  " );
   s_text_yxf( 0,8,0x0f,Path2 );
   s_text_yxf( 1,8,0x0f,Path1 );
   
   s_rame1_F( 2,0,sy-1,sx-1,0x0f );
   
   for( i=0 ; i<sy-4 ; i++ )
   {
      if( j_File2<=begin+i ) break ;
      switch( File2[begin+i].status )
      {  case 0 : ss="     " ; break ;
         case 1 : ss="Новый" ; break ;
	 case 2 : ss="Мод  " ; break ;
      }
      if( File2[begin+i].copy==1 ) s1="1" ; else s1=" " ;

      sprintf( Str,"%s  %s %s",ss,s1,File2[begin+i].name );
      if( begin+i==current ) f=0xf0 ;
          else               f=0x07 ;
      s_text_yxf( 3+i,1,f,Str );
   }
   s_text_yxf( sy-1,1,0xf0,"<Пробел> - отметить <Ввод> - начать копирование <Esc> - выход" );
}
/************************************************************************/
void loop( )
{  long  begin,current ;
   short sx,sy ;
   short key1,key2 ;
   
   begin=current=0 ;

   while( 1 )
   {
      s_get_size( sx,sy );
      paint( begin, current );
      s_getch( &key1, &key2 );
      switch( key2 )
      {  case S_key_Up   : current-- ; break ;
         case S_key_Down : current++ ; break ;
         case S_key_PgUp : current-=sy-5 ; break ;
         case S_key_PgDn : current+=sy-5 ; break ;
	 case S_key_Ins  : File2[current].copy=1-File2[current].copy ;
	                   current++ ;
	                   break ;
      }
      switch( key1 )
      {  case S_key_Esc : exit( 0 ); break ;
         case ' ' : File2[current].copy=1-File2[current].copy ;
	            current++ ;
	            break ;
         case '\r' : return ;
      }
      if( current<0 ) current=0 ;
      if( j_File2<=current ) current=j_File2-1 ;
      if( begin>current ) begin=current ;
      if( begin+(sy-5)<current ) begin=current-(sy-5) ;
   }
}
/************************************************************************/
int   compare1( const void *a, const void *b )
{  t_File2 *A=(t_File2 *)a ;
   t_File2 *B=(t_File2 *)b ;
   return strcmp( A->name,B->name ) ;
}
/************************************************************************/
void copy1( char *From, char *To )
{  long L ;
   char Str[400] ;

   if( 390<strlen( From ) + strlen( To ) )
       return ;
   sprintf( Str,"cp -p %s %s",From,To );
   system( Str );
   sprintf( Str,"chmod 644 %s",To );
   system( Str );
}
/************************************************************************/
void copy( )
{  long  i,begin ;
   short sx,sy ;
   short key1,key2 ;
   char  From[200],To[200] ;

   s_get_size( sx,sy );
   
   for( i=begin=0 ; i<j_File2 ; i++ )
   {  if( File2[i].copy==0 ) continue ;
      if( begin+sy<i )
          begin=i-sy ;
      paint( begin,i );
      s_refresh( );
      if( 198<strlen( Path2 )+strlen(File2[i].name) )
          continue ;
      if( 198<strlen( Path1 )+strlen(File2[i].name) )
          continue ;
      sprintf( From,"%s/%s",Path2,File2[i].name );
      sprintf( To  ,"%s/%s",Path1,File2[i].name );
      copy1( From, To );
   }
}
/************************************************************************/
int main( int argc, char *argv[] )
{
   if( argc<3 )
   {   printf( "\n Я - утилита для сравниния двук каталогов");
       printf( "\n Используйте меня как usb <dir_dst> <dir_src>\n");
       return -1 ;
   }
   else
   {
      if( 199<strlen(argv[1]) || 199<strlen(argv[1]) )
      {   printf( "\n Очень длинные (>200) имена каталогов");
          exit(-1);
      }
      strcpy( Path1,argv[1] );
      strcpy( Path2,argv[2] );
   }

   s_begin_schone(  );
   read_files( Path1,Path2 );
   compare( );
   reduce( );
   qsort( File2,j_File2,sizeof(t_File2),compare1 );
   loop();
   copy( );
   return 0 ;
}
/************************************************************************/


