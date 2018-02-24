/***************************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                     данные, для отображения подсказки 
//     Анисимов Д.В.                                         сегодня
/***************************************************************************/

# include <string.h>
# ifndef __WIN__
   # include <unistd.h>
#endif
# include <list.h>
# include <mylib.h>
# include <help.h>
# include <face.h>

extern char   *HelpDir ;

/***************************************************************************/
long FileToWord( char *File, char ***_Str )
{  long  i,j,L ;
   char *m,**ss ;

   L=file_to_mass( File, &m );

   for( i=0 ; i<L ; i++ )
      if( m[i]==' ' || m[i]=='\n' || m[i]=='\r' || m[i]=='\t' ) m[i]=0 ;
   for( i=j=0 ; i<L-1 ; i++ )
      if( m[i]==0 && m[i+1]!=0 ) j++ ;
   ss=(char **)Calloc(j+1,sizeof(char *));
   j=0 ;
   if( m[0]!=0 ) ss[j++]=m ;
   for( i=0 ; i<L-1 ; i++ )
      if( m[i]==0 && m[i+1]!=0 ) ss[j++]=m+i+1 ;

   *_Str=ss ;
   return j ;
}
/******************************************************************************/
void t_HelpData :: make( void )
{  long  i,i1,z ;
   char  **ss ;
   char Str[200] ;
try
{
   Strcpy( Path,HelpDir,200 );
   unlink( "mkhelp.tmp" );
   sprintf( Str,"ls %s/*.html >> mkhelp.tmp",Path );
   system( Str );

   n_File=FileToWord( "mkhelp.tmp",&ss );
   File=(t_HelpFile *)Calloc( n_File,sizeof(t_HelpFile) );
   for( i=0 ; i<n_File ; i++ )
   {  for( i1=z=0 ; ss[i][i1]!=0 ; i1++ )
         if( ss[i][i1]=='/' ) z=i1+1 ;
      strcpy( File[i].Name,ss[i]+z );
   }
   unlink( "mkhelp.tmp" );
   
   for( i=0 ; i<n_File ; i++ )
      read_file( i ) ;
   mk_href( );
}
catch( int E )
{
   figna("Не знаю, что произошло,\nНо подсказки не будет...\nНажмите Enter");
}
}
/******************************************************************************/
void t_HelpData :: read_file( long i_File )
{
   long  i,i1,j,j1,z,L ;
   char *Text,f ;
   char  Str[200] ;
   t_HelpLink  L1 ;

   sprintf( Str,"%s/%s",Path,File[i_File].Name );
   L=file_to_mass( Str,&Text );

   strcpy( L1.Name,File[i_File].Name ) ;
   L1.i_file=i_File ;
   L1.i_pos =0 ;
   Link.add( L1 );

   f=0 ;
   for( i=j=0 ; i<L ; i++ )
   {  if( 0==strncmp(Text+i,"<pre>" ,5) )
      {  f=1 ; i+=5 ;  }
      if( 0==strncmp(Text+i,"</pre>",6) ) break ;
      if( 0==strncmp(Text+i,"<b>" ,3) )   {  Text[j++]=3 ; i+=3 ; }
      if( 0==strncmp(Text+i,"</b>",4) )   {  Text[j++]=1 ; i+=4 ; }
      if( 0==strncmp(Text+i,"<a name=",8) )
      {   for( i1=i+9,j1=0 ; i1<L && Text[i1]!='"' ; i1++ )
             L1.Name[j1]=Text[i1] ;
          L1.Name[j1]=0 ;
	  L1.i_file=i_File ;
	  L1.i_pos =j ;
          Link1.add( L1 );
	  i=i1+5 ; 
	  continue ;
      }
      if( 0==strncmp(Text+i,"<a href=",8) )
      {   Text[j++]=2 ;
          for( i1=i+9,j1=0 ; i1<L && Text[i1]!='<' ; i1++ )
          {  if( Text[i1]=='>' ) z=i1 ;
             L1.Name[j1++]=Text[i1] ;
          }
	  L1.i_file=0 ;
	  L1.i_pos =0 ;
	  L1.Name[j1]=0 ;
          Link.add( L1 );
	  i=z+1 ;	  
      }
      if( 0==strncmp(Text+i,"</a>",4) ) 
      {  Text[j++]=1 ;
         i+=3 ; continue ;      
      }
      if( f==1 )
          Text[j++]=Text[i] ;
   }
   Text[j]=0 ;
   File[i_File].Text  =Text ;
   File[i_File].l_Text=j ;
}
/******************************************************************************/
void t_HelpData :: mk_href( void )
{  long  i,i1,z,z1 ;
   char *Str ;

   for( i=0 ; i<Link.j ; i++ )
   {  // ------- найти начало имени ссылки --------------
      Str=Link[i].Name ;
      for( i1=0,z=-1 ; Str[i1]!=0 ; i1++ )
	 if( Str[i1]=='>' ) {  z=i1 ; break ;  }
      // ------- проставить файл и позицию --------------
      z1=find_link( Link[i].Name );
      if( 0<=z1 )
      {  Link[i].i_file=Link1[z1].i_file ;
         Link[i].i_pos =Link1[z1].i_pos  ;	 
      }
      strcpy( Str,Str+z+1 );
   }
}
/******************************************************************************/
long t_HelpData :: find_link( char *Str )
{  char *Str1="" ;
   long  i,i_File=-1 ;
   
   for( i=0 ; Str[i]!=0 ; i++ )
      if( Str[i]=='"' ) {  Str[i]=0 ; break ;  }
   for( i=0 ; Str[i]!=0 ; i++ )
      if( Str[i]=='#' ) {  Str[i]=0 ; Str1=Str+i+1 ; }
   for( i=0 ; i<n_File ; i++ )
      if( 0==strcmp(File[i].Name,Str) ) 
      {   i_File=i ; break ;  }
   if( i_File<0 ) return -1 ;
   for( i=0 ; i<Link1.j ; i++ )
      if( Link1[i].i_file==i_File && 0==strcmp(Link1[i].Name,Str1) )
          return i ;
   return -1 ;
}
/******************************************************************************/
char * t_HelpData :: get_help( char *Str )
{  long i ;
   static char *ss="\nПодсказка на эту тему не найдена.\n" ;

   for( i=0 ; i<Link.j ; i++ )
   {  if( 0==strcmp(Link[i].Name,Str) )
          return File[Link[i].i_file].Text ;
   }
   return ss ;
}
/******************************************************************************/
void t_HelpData :: hprint( void )
{  long i ;   
   for( i=0 ; i<Link.j ; i++ )
      printf("\n %s %ld",File[Link[i].i_file].Name,Link[i].i_pos );
   printf("\n");
}
DEFINE_LIST_BTYPE( t_HelpLink, t_HL_List )
