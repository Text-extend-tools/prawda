/**************************************************************************/
//               Документатор моих замечательных исходников
//
/**************************************************************************/
# include <stdio.h>
# include <string.h>
# include <unistd.h>
# include <list.h>
# include <mylib.h>
# include <../utils/zlexer.h>

DECLARE_LIST_TYPE( char *,t_strList )

struct t_Cnf
{
   t_strList Dir ;
   t_strList FileH, FileCpp ;

   void read();
};

struct t_Funk
{  char  Name[100] ;
   short i_file ;
};
DECLARE_LIST_TYPE( t_Funk,t_FunkList )

struct t_Class
{  char  Name[40] ;
   short i_file ;
   t_FunkList Funk ;

   void  init( void );
   void  del( void );
   long  funk_exist( char *ss );
};
DECLARE_LIST_TYPE( t_Class,t_ClassList )

struct t_File
{  char    Name[100] ;
   long    i_File ;
   t_Lexer Lexer ;
   char   *Text ;
   long    l_Text ;

   void    init( void );
   void    del( void );
   void    read( void );
   void    write( void );
   void    mk_class( void );
   void    funk_name(  long i_token, char **Funk_name );
   void    funk_name1( long i_token, char **Class_name, char **Funk_name );
   char   *get_name( void );
};
DECLARE_LIST_TYPE( t_File,t_FileList )

t_Cnf       Cnf ;
t_ClassList Class ;
t_FileList  File ;

long class_exist( char *ss );
long file_exist( char *ss );
void print_class(  );
void print_files(  );
void print_shapka( FILE *fw );
void print_end( FILE *fw );

/***************************************************************************/
long FileToMass( char *File, char **_Mass )
{
   char *Mass ; long L ; FILE *fr ;
   
   fr=Fopen( File,"r" );
   L=FileLength( fr );
   Mass=(char *)Calloc( L+1,1 );
   fread( Mass,L,1,fr );
   Fclose(fr);

   *_Mass=Mass ;
   return L ;
}
/***************************************************************************/
long FileToWord( char *File, char ***_Str )
{  long  i,j,L ;
   char *m,**ss ;

   L=FileToMass( File, &m );

   for( i=0 ; i<L ; i++ )
      if( m[i]==' ' || m[i]=='\n' || m[i]=='\r' || m[i]=='\t' ) m[i]=0 ;
   for( i=j=0 ; i<L ; i++ )
      if( m[i]==0 && m[i+1]!=0 ) j++ ;
   ss=(char **)Calloc(j+1,sizeof(char *));
   j=0 ;
   if( m[0]!=0 ) ss[j++]=m ;
   for( i=0 ; i<L ; i++ )
      if( m[i]==0 && m[i+1]!=0 ) ss[j++]=m+i+1 ;

   *_Str=ss ;
   return j ;
}
/**************************************************************************/

/**************************************************************************/
int main( void )
{  long   i ;
   t_File FF ;
   
   Cnf.read();
   // ------ прочитать какие файлы будем обрабатывать ------
   for( i=0 ; i<Cnf.FileH.j ; i++ )
   {  strcpy( FF.Name,Cnf.FileH[i] );
      FF.i_File=File.j ;
      File.add( FF );
   }
   for( i=0 ; i<Cnf.FileCpp.j ; i++ )
   {  strcpy( FF.Name,Cnf.FileCpp[i] );
      FF.i_File=File.j ;
      File.add( FF );
   }
   // ------ создать образы файлов --------------
   for( i=0 ; i<File.j ; i++ )
      File[i].read();
   for( i=0 ; i<File.j ; i++ )
      File[i].mk_class();
/*
   for( i=0 ; i<Class.j ; i++ )
   {  printf("\n %s :: %s",Class[i].Name,File[Class[i].i_file].Name );
      long i1 ;
      for( i1=0 ; i1<Class[i].Funk.j ; i1++ )
      {  t_Funk *F=&Class[i].Funk[i1] ;
         if( F->i_file<0 )
	          printf("\n     %s :: нет_тела!!!",F->Name );
             else printf("\n     %s :: %s",F->Name,File[F->i_file].Name );
      }
   }
*/
   for( i=0 ; i<File.j ; i++ )
      File[i].write();
//   print_class(  );
//   print_files(  );
   return 0 ;
}
/**************************************************************************/
void t_File :: init( void )
{
   Name[0]=0 ;
   i_File =0 ;
   Text   =NULL;
   l_Text =0 ;
}
/**************************************************************************/
void t_File :: del( void )
{
    Free( Text );
}
/**************************************************************************/
//            прочитать файл и создать его token-образ
/**************************************************************************/
void t_File :: read( void )
{
   l_Text=FileToMass( Name,&Text );
   
   Lexer.set_breaker( (unsigned char *)".,;:#$%^=+-&*/(){}<>[]\'\"\\" );
   Lexer.init( Text,l_Text );
   Lexer.make_token();
}
/**************************************************************************/
//             создать список классов, находящихся в этом файле
/**************************************************************************/
void t_File :: mk_class( void )
{  long     i,i1,i2,stack=0,stack1=0 ;
   char    *s, fconst=0 ;
   char    *s_class,*s_funk ;
   char     cp=' ' ;
   t_Class  Class1 ;
   t_Funk   Funk1 ;
   
   for( i=0 ; i<Lexer.n_Token ; i++ )
   {  s=Lexer.token(i);
      if( s[0]=='(' && stack==0 )
      {   // ----- сделать заголовок функции в *.cpp файле
          funk_name1( i, &s_class, &s_funk );
          if( s_funk[0]!=0 )
	  {   i1=class_exist( s_class );
	      if( 0<=i1 )
	      {   i2=Class[i1].funk_exist( s_funk );
		  if( 0<=i2 )
	              Class[i1].Funk[i2].i_file=i_File ;
	      }	  
	  }      
      }
      // ----- определить не находимся ли мы внутри тела класса/функции
      if( 0<i ) {  s=Lexer.token(i-1); cp=s[0] ;  }
      s=Lexer.token(i);
      if( cp!='\\'  && (s[0]=='\"' || s[0]=='\'') ) 
          fconst=1-fconst ;
      if( fconst==0 && ( s[0]=='{' || s[0]=='(' ) ) stack++ ;
      if( fconst==0 && ( s[0]=='}' || s[0]==')' ) ) stack-- ;

      if( 0==strcmp( s,"class" ) || 0==strcmp( s,"struct" ) || 0==strcmp( s,"enum" ) )
      {   s=Lexer.token(i+2);
          if( 0<stack || s[0]==';' )
              continue ;
	  // ----- внесение класса ---------------------------------
          s=Lexer.token(i+1);
          if( 0>class_exist( s ) )
	  {   strcpy( Class1.Name,s );
	      Class1.i_file=i_File ;
	      Class.add( Class1 );

	      Lexer.Token[i].type =1 ;
	      Lexer.Token[i].type1=0 ;
	      Lexer.Token[i].i_class=Class.j-1 ;
	      Lexer.Token[i].i_funk =-1 ;
	  
	      // ------ внесение прототипов функций --------------------
	      for( i1=i+2 ; i1<Lexer.n_Token ; i1++ )
	      {  s=Lexer.token(i1);
		 if( s[0]=='{' ) stack1++ ;
		 if( s[0]=='}' ){ stack1-- ; if( stack1<=0 ) break ;  }
		 if( s[0]=='(' )
        	 {   funk_name( i1,&s_funk );
		     strcpy( Funk1.Name,s_funk );
	             Funk1.i_file=-1 ;
	             Class[Class.j-1].Funk.add( Funk1 );

		     Lexer.Token[i1-1].type =2 ;
		     Lexer.Token[i1-1].type1=1 ;
		     Lexer.Token[i1-1].i_class=Class.j-1 ;
		     Lexer.Token[i1-1].i_funk =Class[Class.j-1].Funk.j-1 ;
		 }
	      }
	  }
      }
      s=Lexer.token(i);
      if( 0==strcmp(s,"DECLARE_LIST_TYPE") )
      {   // ----- внесение класса - списка ---------------------------
          s=Lexer.token(i+4);
	  if( s[0]==',' ) s=Lexer.token(i+5);
          if( 0>class_exist( s ) )
	  {   strcpy( Class1.Name,s );
	      Class1.i_file=i_File ;
	      Class.add( Class1 );

	      Lexer.Token[i].type =1 ;
	      Lexer.Token[i].type1=0 ;
	      Lexer.Token[i].i_class=Class.j-1 ;
	      Lexer.Token[i].i_funk =-1 ;
          }
      }
   }
}
/**************************************************************************/
//  сформировать имя функции для прототипа (с учетом конструкций типа operator ==)
/**************************************************************************/
void t_File :: funk_name( long i_token, char **Funk_name )
{  char *s ;
   long  z=-1 ;
   static char Name1[100] ;
   
   strcpy( Name1,Lexer.token(i_token-1) );
   z=i_token-1 ;
   
   s=Lexer.token( i_token-2 );
   if( 0==strcmp(s,"operator") )
   {   strcpy( Name1,"operator " );
       strcat( Name1,Lexer.token( i_token-1 ));
       z=i_token-2 ;
   }
   s=Lexer.token( i_token-3 );
   if( 0==strcmp(s,"operator") )
   {   strcpy( Name1,"operator " );
       strcat( Name1,Lexer.token( i_token-2 ));
       strcat( Name1,Lexer.token( i_token-1 ));
       z=i_token-3 ;
   }
   *Funk_name =Name1 ;
   if( 0<=z )
   {   Lexer.Token[z].type =2 ;// 1 ;
       Lexer.Token[z].type1=1 ;
       Lexer.Token[z].i_class=Class.j-1 ;
       Lexer.Token[z].i_funk =Class[Class.j-1].Funk.j ;
   }
}
/**************************************************************************/
//  сформировать имя функции для тела (с учетом конструкций типа operator ==)
/**************************************************************************/
void t_File :: funk_name1( long i_token, char **Class_name, char **Funk_name )
{  char *s ;
   long  z=-1,i_class,i_funk ;
   static char Name1[100],Name2[100] ;
   
   s=Lexer.token( i_token-2 );
   Name1[0]=0 ; Name2[0]=0 ;
   if( s[0]==':' )
   {  strcpy( Name1,Lexer.token(i_token-1) );
      strcpy( Name2,Lexer.token(i_token-4) );
      z=i_token-5 ;
   }
   s=Lexer.token( i_token-2 );
   if( 0==strcmp(s,"operator") )
   {   strcpy( Name1,"operator " );
       strcat( Name1,Lexer.token( i_token-1 ));
       strcpy( Name2,Lexer.token( i_token-5 ));
       z=i_token-6 ;
   }
   s=Lexer.token( i_token-3 );
   if( 0==strcmp(s,"operator") )
   {   strcpy( Name1,"operator " );
       strcat( Name1,Lexer.token( i_token-2 ));
       strcat( Name1,Lexer.token( i_token-1 ));
       strcpy( Name2,Lexer.token( i_token-6 ));
       z=i_token-7 ;
   }
   *Class_name=Name2 ;
   *Funk_name =Name1 ;
   if( 0<=z )
   {   i_class=class_exist( Name2 ) ;
       if( 0<=i_class ) 
       {   i_funk =Class[i_class].funk_exist( Name1 );
	   if( 0<=i_funk )
	   {
	       Lexer.Token[z].type =1 ;
	       Lexer.Token[z].type1=1 ;
	       Lexer.Token[z].i_class=i_class ;
	       Lexer.Token[z].i_funk =i_funk ;
	   }
       }
   }
}
/**************************************************************************/
//              записать файл типа *.h.html или *.cpp.html
/**************************************************************************/
void t_File :: write( void )
{  long  i,i_token,i_class,i_file ;
   char  Str[200],*s,*s1,*s2 ;
   FILE    *fw ;
   t_Token *T ;
   
   strcpy( Str,"../doc/html_src/" );
   strcat( Str,get_name( ) );
   strcat( Str,".html" );
   fw=Fopen( Str,"w" );

   print_shapka( fw );
   
   for( i=i_token=0 ; i<l_Text ; i++ )
   {
      if( Text[i]=='<' ) {  fprintf( fw,"&LT;" ); i_token++ ; continue ;  }
      if( Text[i]=='>' ) {  fprintf( fw,"&GT;" ); i_token++ ; continue ;  }
      while( Lexer.Token[i_token].begin<i ) i_token++ ;
      if( Lexer.Token[i_token].begin<=i )
      {   T=&Lexer.Token[i_token] ;
          if( T->type==0 )
	  {
	      i_class=class_exist( Lexer.token(i_token) );
	      if( 0<=i_class )
	      {   // ------ сделать ссылки на классы ----------
	          s =File[Class[i_class].i_file].get_name();
	          s1=Class[i_class].Name ;
	          fprintf( fw,"<a href=\"%s.html#%s\">%s</a>",s,s1,s1 );	      
                  i+=strlen(s1);
		  i_token++ ;
	      }
	      if( 3<=i_token )
	      {	  // ------ сделать ссылки на "# include"
	          s=Lexer.token(i_token-2);
		  if( 0==strcmp( "include",s ) )
		  {   s=Lexer.token(i_token-3);
	              if( 0==strcmp( "#",s ) )
		      {   long i1 ;
			  Str[0]=0 ;
			  for( i1=i_token ; ; i1++ )
			  {  s=Lexer.token(i1);
		             if( s[0]=='>' || s[0]=='\"' ) break ;
		             strcat( Str,s );
			  }
			  if( 0<=file_exist( Str ) )
			  {   fprintf( fw,"<a href=\"%s.html\">%s</a>",Str,Str );
			      i+=strlen(Str);
			      i_token=i1 ;
                          }
		      }
		  }
              }
	  }
          if( T->type==1 ) // ------ если заголовок -------
	  {   
	      if( T->type1==0 ) 
	          // ------ заголовок класса
	          fprintf( fw,"\n<a name=\"%s\"></a>",Class[T->i_class].Name );
	      if( T->type1==1 )
	          // ------ заголовок функции
	          fprintf( fw,"<a name=\"%s-%s\"></a>\n",
		              Class[T->i_class].Name,
			      Class[T->i_class].Funk[T->i_funk].Name );
	  }
          if( T->type==2 ) // ------ если ссылка ---------
	  {   // ------ ссылка на класс
	      if( T->type1==0 )
	      {   s =File[Class[T->i_class].i_file].get_name();
	          s1=Class[T->i_class].Name ;
	          fprintf( fw,"<a href=\"%s.html#%s\">%s</a>",s,s1,s1 );
	      }
	      if( T->type1==1 )
	      {   // ------ ссылка на функцию
	          i_file=Class[T->i_class].Funk[T->i_funk].i_file ;
	          if( 0<=i_file )
	          {   s =File[i_file].get_name();
	              s1=Class[T->i_class].Name ;
		      s2=Class[T->i_class].Funk[T->i_funk].Name ;
		      fprintf( fw,"<a href=\"%s.html#%s-%s\">%s</a>",s,s1,s2,s2 );
		      i+=strlen(s2);
                  }
	      }
	  }
          i_token++ ;
      }
      fprintf( fw,"%c",Text[i]);
   }
   print_end( fw );
   Fclose( fw );
}
/**************************************************************************/
//                  дать неиерархическое имя файла
/**************************************************************************/
char * t_File :: get_name( void )
{  long i,z ;
   for( i=z=0 ; i<100 && Name[i]!=0 ; i++ )
      if( Name[i]=='/' ) z=i+1 ;
   return Name+z ;
}
/**************************************************************************/
void t_Class :: init( void )
{   Name[0]=0 ;
    i_file=0 ;
    Funk.init();
}
/**************************************************************************/
void t_Class :: del( void )
{
    Funk.del();
}
/**************************************************************************/
//               существует ли такая функция в этом классе
/**************************************************************************/
long t_Class :: funk_exist( char *ss )
{  long i ;
    for( i=0 ; i<Funk.j ; i++ )
       if( 0==strcmp( Funk[i].Name,ss ) ) return i ;
    return -1 ;
}
/**************************************************************************/
//               существует ли такой файл
/**************************************************************************/
long file_exist( char *ss )
{   long i ;
    for( i=0 ; i<File.j ; i++ )
       if( 0==strcmp( File[i].get_name(),ss ) ) return i ;
    return -1 ;
}
/**************************************************************************/
//               существует ли такой класс
/**************************************************************************/
long class_exist( char *ss )
{   long i ;
    for( i=0 ; i<Class.j ; i++ )
       if( 0==strcmp( Class[i].Name,ss ) ) return i ;
    return -1 ;
}
/**************************************************************************/
//               напечатать список классов
/***************************************************************************/
void print_class(  )
{  FILE *fw ;
   char *s,*s1 ;
   long  i_class ;

   fw=fopen("../doc/src_html/class.html","w");
   print_shapka( fw );

   for( i_class=0 ; i_class<Class.j ; i_class++ )
   {   s =File[Class[i_class].i_file].get_name();
       s1=Class[i_class].Name ;
       fprintf( fw,"\n <a href=\"%s.html#%s\">%s</a>",s,s1,s1 );	      
   }	  
   
   print_end( fw );
   Fclose(fw);
}
/***************************************************************************/
//              напечатать список файлов
/***************************************************************************/
void print_files(  )
{  FILE *fw ;
   char *s ;
   long  i ;

   fw=fopen("../doc/src_html/files.html","w");
   print_shapka( fw );

   for( i=0 ; i<File.j ; i++ )
   {   s =File[i].get_name();
       fprintf( fw,"\n <a href=\"%s.html\">%s</a>",s,s );
   }	  
   
   print_end( fw );
   Fclose(fw);
}
/***************************************************************************/
//                           напечатать шапку                              //
/***************************************************************************/
void print_shapka( FILE *fw )
{
   fprintf(fw,"\n<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">");
   fprintf(fw,"\n<html><LINK REV=\"made\" HREF=\"mailto:wolf@gambit.msk.su\">");
   fprintf(fw,"\n<title>UNIX programm from Dima Anisimow</title>");
   fprintf(fw,"\n<META HTTP-EQUIV=\"CONTENT-TYPE\" CONTENT=\"text/html;charset=koi8-r\">");
   fprintf(fw,"\n");
   fprintf(fw,"\n<body>\n<pre>\n");
}
/***************************************************************************/
//                           напечатать хвост                              //
/***************************************************************************/
void print_end( FILE *fw )
{
   fprintf(fw,"\n</pre>\n</body>\n</html>");
}
static char *d1="../face" ;
static char *d2="../trans" ;
/***************************************************************************/
//                    сформировать список классов
/***************************************************************************/
void t_Cnf :: read()
{  char *s,**ss ;
   char Str[100] ;
   long i,N ;

   // --------- сформировать каталоги -------------------
   Dir.add( d1 );
   Dir.add( d2 );

   // --------- сформировать файлы *.h -------------------
   unlink( "mkdoc.tmp" );
   for( i=0 ; i<Dir.j ; i++ )
   {  sprintf( Str,"ls %s/*.h >> mkdoc.tmp",Dir[i] );
      system( Str );
   }
   N=FileToWord( "mkdoc.tmp",&ss );
   for( i=0 ; i<N ; i++ )
   {        
      s=(char *)Calloc( strlen(ss[i])+1,sizeof(char) );
      strcpy( s,ss[i] );
      FileH.add( s );
   }
   // --------- сформировать файлы *.cpp ----------------
   unlink( "mkdoc.tmp" );
   for( i=0 ; i<Dir.j ; i++ )
   {  sprintf( Str,"ls %s/*.cpp >> mkdoc.tmp",Dir[i] );
      system( Str );
   }
   N=FileToWord( "mkdoc.tmp",&ss );
   for( i=0 ; i<N ; i++ )
   {        
      s=(char *)Calloc( strlen(ss[i])+1,sizeof(char) );
      strcpy( s,ss[i] );
      FileCpp.add( s );
   }
   unlink( "mkdoc.tmp" );
}
/**************************************************************************/
DEFINE_LIST_BTYPE( t_Funk ,t_FunkList )
DEFINE_LIST_TYPE( t_Class,t_ClassList )
DEFINE_LIST_TYPE( t_File ,t_FileList )
DEFINE_LIST_BTYPE( char *,t_strList )

