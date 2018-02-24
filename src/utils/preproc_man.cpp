# include <string.h>
# include <list.h>
# include <mylib.h>
# include <lexer.h>
# include <grammar.h>
# include "resource1.h"

enum e_TagType
{   P_FRASA,
    P_VERBATIM,
    P_STRUCT,
    P_CONST,
    P_TAG_LEFT,
    P_TAG_RIGHT,
    P_VERB
};
enum e_TagType1
{   P_OPEN,
    P_CLOSE,
    P_OPEN1,
    P_SINGLE
};

struct t_TagType
{   char      *str ;
    short      l_str ;
    short      i_struct ;
    e_TagType  type  ;
    e_TagType1 type1 ;
};

DECLARE_LIST_TYPE( t_TagType,t_TagTypeList );

static t_Lexer   Lexer1 ;
t_TagTypeList    TTList ;
extern t_Grammar Grammar ;

/************************************************************************/
void read_cnf( char *Path )
{
   char      *Mass,*s,*s1 ;
   long       i,L ;
   t_TagType  T ;
   char       Str[200] ;
   
   strcpy( Str,Path );
   strcat( Str,"/grammar/z_man.rc" );

   L=file_to_mass( Str,&Mass );
   Lexer1.init1( Mass,L );
   Lexer1.make_words( );
   for( i=0 ; i<Lexer1.n_Word ; i++ )
   {  s=Lexer1.Word[i] ;
      if( 0==strcmp( s,"\\Структуры_типа" ) )
      {   s1=Lexer1.Word[i+1] ;
          if( 0==strcmp( s1,"\\@@" ) ) T.type=P_FRASA ;
	  if( 0==strcmp( s1,"\\##" ) ) T.type=P_VERBATIM ;
	  if( 0==strcmp( s1,"\\@~" ) ) T.type=P_STRUCT ;
	  if( 0==strcmp( s1,"\\@=" ) ) T.type=P_CONST ;
	  if( 0==strcmp( s1,"\\#<" ) ) T.type=P_TAG_LEFT ;
	  if( 0==strcmp( s1,"\\#>" ) ) T.type=P_TAG_RIGHT ;
	  if( 0==strcmp( s1,"\\#"  ) ) T.type=P_VERB ;
	  i++ ;
	  continue ;
      }   
      if( 0==strcmp( s,"\\Открывающий" ) )
      {  T.type1=P_OPEN  ; continue ; }
      if( 0==strcmp( s,"\\Закрывающий" ) )
      {  T.type1=P_CLOSE ; continue ; }
      if( 0==strcmp( s,"\\Открывающий_на_одну_строку" ) )
      {  T.type1=P_OPEN1 ; continue ; }
      if( 0==strcmp( s,"\\Простой" ) )
      {  T.type1=P_SINGLE ; continue ; }
      T.str  =s ;
      T.l_str=strlen( s );
      T.i_struct=-1 ;
      if( (T.type==P_STRUCT || T.type==P_CONST || T.type==P_VERB) &&
          (T.type1==P_OPEN || T.type1==P_OPEN1) )
      {   T.i_struct=Grammar.find_struct( FROM,Lexer1.word(i+1) ); i++ ; }
      TTList.add(T);
   }
}
/************************************************************************/
void main_funk( char *File1, char *File2 )
{  long i,i1,L ;
   char *Mass,c,c1 ;
   t_TagType *T,TT ;
   FILE *fw ;
   char  fText=1,fText1=1 ;
   char  Str[200] ;

   TT.type1=P_OPEN ;
   T=&TT ;

   L=file_to_mass( File1,&Mass );
   fw=Fopen( File2,"w" );
   fputs( "\\##{ Mercury_Preprocessor_File }\n" ,fw );
   
   for( i=0 ; i<L ; i++)
   {
      if( Mass[i]=='\n' && T->type1==P_OPEN1 )
      {   fputs( "\n}\n" ,fw );
          if( T->type==P_VERBATIM ) fText=fText1 ;
	  T=&TT ;
	  continue ;
      }
      for( i1=0 ; i1<TTList.j ; i1++ )
      {
         if( 0==strncmp( Mass+i,TTList[i1].str,TTList[i1].l_str ) )
         {  T=&TTList[i1] ;
	    switch( T->type )
	    {
//	       case P_FRASA :
	       case P_VERBATIM :
	            if( T->type1==P_OPEN1 )
		    {   if( fText==2 ) {  fputs( "}\n",fw ); fText=1 ;  }
		        sprintf( Str,"\\## { %s ",T->str );
			fText1=fText ; fText=0 ;
	            }
		    if( T->type1==P_OPEN )
		    {   if( fText==2 ) {  fputs( "}\n",fw ); fText=1 ;  }
		        sprintf( Str,"\\## s{ %s\n",T->str );
		        fText1=fText ; fText=0 ;
	            }
		    if( T->type1==P_CLOSE )
		    {   sprintf( Str,"\n}\n%s",T->str );
			fText=fText1 ;
	            }
		    if( T->type1==P_SINGLE )
		        sprintf( Str,"\\## { %s } ",T->str );
		    fputs( Str,fw );
	            break ;
	       case P_STRUCT :
	            if( T->type1==P_OPEN || T->type1==P_OPEN1 )
	            {   if( fText==1 )
		            fputs( "\n\\@@ { ",fw );
		        sprintf( Str,"\\#<{%s} \\@~ %s {",
	                	 T->str, Grammar[T->i_struct].From.Name );
			fputs( Str,fw );
                        fText=2 ;
                    }
		    if( T->type1==P_CLOSE )
		    {   sprintf( Str,"\\#>{%s}",T->str );
		        fputs( Str,fw );
		    }
		    break ;
	       case P_CONST :
	            if( T->type1==P_OPEN || T->type1==P_OPEN1 )
	            {   if( fText==1 ) fputs( "\n\\@@ { ",fw );
	        	sprintf( Str,"\\@= %s { %s ",
			         Grammar[T->i_struct].From.Name,T->str );
			fputs( Str,fw );
			fText=2 ;
		    }
		    if( T->type1==P_CLOSE )
		    {   sprintf( Str,"%s } ",T->str );
		        fputs( Str,fw );
		    }
		    break ;
	       case P_TAG_LEFT :
	            if( T->type1==P_OPEN )
	                sprintf( Str,"\\#<{%s}",T->str );
		    if( T->type1==P_OPEN1 )
		        sprintf( Str,"\\#<{%s",T->str );
		    if( T->type1==P_CLOSE )
		        sprintf( Str,"%s }",T->str );
		    if( T->type1==P_SINGLE )
		        sprintf( Str,"\\#<{%s}",T->str );
		    fputs( Str,fw );
		    break ;
	       case P_TAG_RIGHT:
	            if( T->type1==P_OPEN )
	                sprintf( Str,"\\#>{%s}",T->str );
		    if( T->type1==P_OPEN1 )
		        sprintf( Str,"\\#>{%s",T->str );
		    if( T->type1==P_CLOSE )
		        sprintf( Str,"%s }",T->str );
		    if( T->type1==P_SINGLE )
		        sprintf( Str,"\\#>{%s}",T->str );
		    fputs( Str,fw );
		    break ;
	       case P_VERB :
	            if( T->type1==P_OPEN || T->type1==P_OPEN1 )
	                sprintf( Str,"\\# %s { %s",
			         Grammar[T->i_struct].From.Name,T->str );
		    if( T->type1==P_CLOSE )
		        sprintf( Str," %s }",T->str );
		    if( T->type1==P_SINGLE )
		        Str[0]=0 ;
		    fputs( Str,fw );
		    break ;
	    }
	    i+=TTList[i1].l_str-1 ;
	    goto MM ;
	 }
      }
      c=Mass[i] ; c1=Mass[i+1] ;
      // -------- начало фразы ----------------------------
      if( c!=' ' && c!='\n' && c!='\t' && fText==1 )
      {  fputs( "\n\\@@ { ",fw ); fText=2 ;  }
      
      // ----- "ненастоящая" точка (например man.cnf) -----
      if( c=='.' && !( c1==' ' || c1=='\t' || c1=='\n' || c1=='.' ) )
      {   fputc( '.',fw ); continue ;  }
      
      // -------- конец фразы ----------------------------
      if( (c=='.' || c=='?' || c=='!' || c==';') && fText==2 )
      {   if( Mass[i+1]=='.' ) {  fputc('.',fw); i++ ;  }
          if( Mass[i+1]=='.' ) {  fputc('.',fw); i++ ;  }
          fputs(".}",fw);  fText=1 ; continue ; 
      }
      fputc( Mass[i],fw );

      MM: ;
   }
   if( fText==2 )
       fputs( "}\n",fw );
   Fclose(fw);
}
/************************************************************************/
int main( int argc, char *argv[] )
{  t_Option    Option ;
   char *h,Str[200],Str1[200],Str2[200] ;

try
{
   if( 1<argc )
   {   strcpy( Str ,argv[1] );
       strcpy( Str1,argv[1] );
       strcat( Str1,".pre" );
   }
   else
   {   printf("\n Use preproc_man <filename>\n");
       exit(-1);
   }
   
   h=getenv( "HOME" );
   if( h!=NULL ) {  Strcpy( Str2,h,200 );  }
   Strcat( Str2,"/.mercuryrc",200 );
   Option.read( Str2 );
   strcpy( Str2,Option.get("path") );
   strcat( Str2,"/grammar/lang.txt" );

   Grammar.make_all( Str2 );
   read_cnf( Option.get("path") );
   main_funk( Str, Str1 );
   return 0 ;
}
catch(...)
{  return -1 ;
}
}
/************************************************************************/
DEFINE_LIST_BTYPE( t_TagType,t_TagTypeList );
