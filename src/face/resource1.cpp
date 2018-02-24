/*******************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                 настройки, размеры, шрифты и т.п.
//     Анисимов Д.В.                               сегодня
/*******************************************************************/

# include <string.h>
# include <mylib.h>
# include <resource1.h>

/************************************************************************/
int t_Option :: read( char *File )
{  long   i,L ;
   char  *Mass ;
   FILE  *ff ;
   t_OptionParam PP ;

   Param.init() ;
   ff=fopen( File,"r" );
   if( ff==NULL ) return -1 ;
   fclose(ff);
   L=file_to_mass( File,&Mass );

   t_Lexer Lexer ;

   Lexer.set_breaker((uchar *)":");
   Lexer.init( Mass,L );
   Lexer.remark();
   Lexer.make_words();

   for( i=0 ; i<Lexer.n_word(); i++ )
      if( 0==strcmp(Lexer.word(i),":") )
      {   if( i==0 || i==Lexer.n_word()-1 ) continue ;
          Strcpy( PP.Tag  ,Lexer.word(i-1),50 );
	  Strcpy( PP.Value,Lexer.word(i+1),200 );
	  Param.add( PP );
      }
   return 0 ;
}
/************************************************************************/
char * t_Option :: get( char *Tag )
{  long i ;

   for( i=0 ; i<Param.j ; i++ )
      if( 0==strcmp(Tag,Param[i].Tag) )
          return Param[i].Value ;
    return "" ;
}

DEFINE_LIST_BTYPE( t_OptionParam,t_OptionParamList )
