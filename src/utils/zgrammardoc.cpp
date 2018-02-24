/**************************************************************************/
//                Распечатать грамматику в красивом виде
//
/**************************************************************************/
# include <string.h>
# include <stdlib.h>
# include <unistd.h>
# include <mylib.h>
# include <grammar.h>

struct t_dFile
{  char Name[100] ;
};
struct t_dStruct
{  short i_file ; 
   long  i_rem ; 
};
struct t_Rem
{  
   short i_file,
         i_struct,
         type ;     // 0 - вне перевода 1-внутри перевода 
	            // 2-после первой структуры 3-после второй
   char  Str[100] ; 
};
struct t_String
{   char  Str[1000] ;

          t_String( void ) {  Str[0]=0 ; };
    void  add_s( char  *s );
    void  add_d( short  d );
    void  clr( void )      {  Str[0]=0 ; };
    void  print( FILE *fw ){  fprintf( fw,Str ); Str[0]=0 ;  };
    short strlen( void )   {  return :: strlen(Str) ;  };

    void  print_param( char to, short i_param, char f=0 );
    void  print_value( char to, short i_param, short i_value );
    void  print_struct( char to, short i_struct );
    void  print_word( char to, t_Word *W, char f=0 );
};

t_dFile    File[20] ;
t_dStruct *Struct=NULL ;
t_Rem     *Rem   =NULL ;

long       n_Struct=0,n_Rem=0,n_File=0,j_Rem=0 ;

char *get_name( char *Str );
char *get_rem( char *Str );
void  set_rem_index( short i_struct );
void  set_rem_file( short i_file );
void  my_more_main( void );

/***************************************************************************/
void t_String :: add_s( char  *s )
{  short i,j ;
   char s1[200] ;

   for( i=j=0 ; i<200 ; i++ )
   {  if( s[i]=='%' ) continue ;
      s1[j++]=s[i] ;
      if( s[i]==0 ) break ;
   }
   strcat( Str,s1 );
}    
void t_String :: add_d( short d )
{   char Buf[20] ;
    sprintf( Buf,"%d",d );
    strcat( Str,Buf );
}
/* --------- напечатать имя параметра ----------------------------------- */
void t_String :: print_param( char to, short i_param, char f )
{  char *s,Buf[200] ;

   if( to==0 ) s=Grammar.from().Param[i_param].Name ;
       else    s=Grammar.to().Param[i_param].Name ;

   if( f==1 )
   {  if( to==0 ) sprintf( Buf,"<a href=\"lang.h.html#%s_1\">%c",s,'%' );
          else    sprintf( Buf,"<a href=\"lang.h.html#%s_2\">%c",s,'%' );
      add_s(Buf);
   }
   if( f==2 )
   {  if( to==0 ) sprintf( Buf,"<a name=\"%s_1\"></a>",s );
          else    sprintf( Buf,"<a name=\"%s_2\"></a>",s );
      add_s(Buf);
   }   
   add_s(s);
   add_s("</a>");   
}
/* --------- значение параметра ----------------------------------------- */
void t_String :: print_value( char to, short i_param, short i_value )
{  
   if( i_value<0 ) {  add_s( "@0" ); return ;  }
   if( to==0 ) add_s( Grammar.from().Param[i_param].Value[i_value].Name );
       else    add_s( Grammar.to().Param[i_param].Value[i_value].Name );
}
/* --------- напечатать имя структуры ----------------------------------- */
void t_String :: print_struct( char to, short i_struct )
{  e_Type t ;

   if( i_struct<0 ) return ;
   if( to==0 ) t=Grammar[i_struct].From.type ;
       else    t=Grammar[i_struct].To.type ;

   if( t==TCONST  ) {  add_s("@0");  return ;  }
   if( t==TCONST1 ) {  add_s("@1");  return ;  }
   if( t==TWORD0  ) {  add_s("@00"); return ;  }
   if( to==0 ) add_s( Grammar[i_struct].From.Name );
       else    add_s( Grammar[i_struct].To.Name );
}
/* -------- напечатать слово -------------------------------------------- */
void t_String :: print_word( char to, t_Word *W, char f )
{  short i ;
   char  Buf[200] ;

   if( f==1 )
   {   if( W->type==TCONST  ) add_s("<b>@0</b>");
       if( W->type==TCONST1 ) add_s("<b>@1</b>");
       if( W->type==TWORD0  ) add_s("<b>@00</b>");
   }
   else
   {   if( W->type==TCONST  ) add_s("@0");
       if( W->type==TCONST1 ) add_s("@1");
       if( W->type==TWORD0  ) add_s("@00");
   }
   if( W->i_struct<0 ) return ;
   
   if( f==1 ) 
   {  sprintf( Buf,"<a href=\"%s.html#%s\">%c",
               File[Struct[W->i_struct].i_file].Name,
               Grammar[W->i_struct].From.Name,'%' );
      add_s(Buf);
   }
   print_struct( to, W->i_struct );
   if( f==1 ) add_s("</a>");
   
   if( W->literal[0]!=0 )
   {   add_s( "[" );
       add_s( W->literal );
       add_s( "]" );
   }
   if( 0<W->Param.j ) add_s("(");
   for( i=0 ; i<W->Param.j ; i++ )
   {  
      if( W->Param[i].Dir==1 ) add_s(" &"); else add_s(" ");
      add_s( W->Param[i].Name );
      if( i<W->Param.j-1 ) add_s(",");
   }
   if( 0<W->Param.j ) add_s(" )");
   add_s(" ");
}
/* --------- напечатать часть речи -------------------------------------- */
void Print_struct( FILE *fw, char to, short i_struct )
{
   t_String  Str0,Str1 ;
   t_Struct *S ;
   e_Type    t ;
   short     i,L0,L1 ;

   if( to==0 ) S=&Grammar[i_struct].From ;
       else    S=&Grammar[i_struct].To ;
   t=S->type ;

   // ------ первоначальная запись заголовка -------------
   if( t==TWORD    ) Str0.add_s("@Часть_речи ");
   if( t==TSTRUCT  ) Str0.add_s("@Структура  ");
   if( t==TSTRUCT1 ) Str0.add_s("@Структура1 ");
   if( t==TSTRUCT2 ) Str0.add_s("@Структура2 ");
   if( t==TSELECT  ) Str0.add_s("@Выбор      ");
   if( t==TSELECT1 ) Str0.add_s("@Выбор1     ");
   if( t==TSELECT2 ) Str0.add_s("@Выбор2     ");
   
   Str0.add_s( S->Name );
   if( 0<S->Param.j ) Str0.add_s("( ");
   for( i=0 ; i<S->Param.j ; i++ )
   {  Str0.print_param( to, S->Param[i].param );
      if( S->Param[i].Dir==1 ) Str0.add_s(" &"); else Str0.add_s(" ");
      Str0.add_s( S->Param[i].Name );
      if( i<S->Param.j-1 ) Str0.add_s(", ");
   }
   if( 0<S->Param.j ) Str0.add_s(" )");
   if( 0<S->Word.j ) Str0.add_s(" = ");
   L0=Str0.strlen();

   // ------ окончательная запись заголовка -------------
   Str0.clr();
   if( t==TWORD    ) Str0.add_s("<b>@Часть_речи</b> ");
   if( t==TSTRUCT  ) Str0.add_s("<b>@Структура </b> ");
   if( t==TSTRUCT1 ) Str0.add_s("<b>@Структура1</b> ");
   if( t==TSTRUCT2 ) Str0.add_s("<b>@Структура2</b> ");
   if( t==TSELECT  ) Str0.add_s("<b>@Выбор     </b> ");
   if( t==TSELECT1 ) Str0.add_s("<b>@Выбор1    </b> ");
   if( t==TSELECT2 ) Str0.add_s("<b>@Выбор2    </b> ");
   
   Str0.add_s( S->Name );
   if( 0<S->Param.j ) Str0.add_s("( ");
   for( i=0 ; i<S->Param.j ; i++ )
   {  Str0.print_param( to, S->Param[i].param,1 );
      if( S->Param[i].Dir==1 ) Str0.add_s(" &"); else Str0.add_s(" ");
      Str0.add_s( S->Param[i].Name );
      if( i<S->Param.j-1 ) Str0.add_s(", ");
   }
   if( 0<S->Param.j ) Str0.add_s(" )");
   if( 0<S->Word.j ) Str0.add_s(" = ");
   
   for( i=L1=0 ; i<S->Word.j ; i++ )
   {  Str1.clr();
      Str1.print_word( to, &S->Word[i] );
      L1+=Str1.strlen();      
   }
   Str0.print( fw );
   if( 80<L0+L1 ) fprintf(fw,"\n               ");
   if( L1<65 )
   {   for( i=0 ; i<S->Word.j ; i++ )
       {  Str1.clr();
	  Str1.print_word( to, &S->Word[i],1 );
	  Str1.print( fw );
       }
   }
   else
   {   for( i=0 ; i<S->Word.j ; i++ )
       {  Str1.clr();
	  Str1.print_word( to, &S->Word[i],1 );
	  Str1.print( fw );
	  fprintf( fw,"\n               ");
       }
   }   
   fprintf( fw,";");   
}
/* --------- напечатать формат ------------------------------------------ */
void print_format( FILE *fw, char to, t_Format *F )
{  t_Lang       *Lang ;
   t_Param1List *Param ;
   short i,i1 ;
   t_String Str ;

   if( to==0 ) Lang=&Grammar.from() ;
       else    Lang=&Grammar.to() ;
   Param=&Lang->Part[F->i_part].Param ;

   fprintf( fw,"\n<b>@Формат</b> ");
   Str.print_struct( to, F->i_part );
   Str.print( fw );
   fprintf( fw," %s",F->Name );
   fprintf( fw,"\n{\n   ");

   for( i=0 ; i<F->Record.j ; i++ )
   {
      for( i1=0 ; i1<Param->j ; i1++ )
      {  t_Param1 *P1=&Param->list[i1] ;
         fprintf( fw,"%s = ",P1->Name );
	 Str.print_value( to, P1->param, F->Record[i].value[i1] );
	 Str.print( fw );
	 fprintf( fw," ");
      }
      fprintf( fw," <b>@Слово</b>\n   ");
   }
   fprintf( fw,"\n}");
}
/* --------- напечатать язык -------------------------------------------- */
void print_lang( FILE *fw, char to )
{
   t_Lang *Lang ;
   short i,i1 ;
   t_String Str ;
   
   if( to==0 ) Lang=&Grammar.from() ;
       else    Lang=&Grammar.to() ;
   // --------- напечатать параметры ------------------
   for( i=0 ; i<Lang->Param.j ; i++ )
   {  fprintf( fw,"\n   <b>@Параметр</b> ");
      Str.print_param( to, i, 2 );
      Str.print( fw );
      fprintf( fw,"{ ");
      for( i1=0 ; i1<Lang->Param[i].Value.j ; i1++ )
      {   Str.print_value( to, i, i1 );
          Str.print( fw );
          fprintf( fw," ");
      }	 
      fprintf( fw," };");
   }
   fprintf( fw,"\n");
   // --------- напечатать части речи -----------------
   for( i=0 ; i<Lang->Part.j ; i++ )
   {  fprintf( fw,"\n   ");
      Print_struct( fw, to, i );
   }
   fprintf( fw,"\n");
/*
   // --------- напечатать форматы -----------------
   for( i=0 ; i<Lang->Format.j ; i++ )
      print_format( fw, to, &Lang->Format[i] );
   fprintf( fw,"\n");
*/
   // --------- напечатать файлы -------------------
   for( i=0 ; i<Lang->File.j ; i++ )
   {  fprintf( fw,"\n   <b>@Файлы</b> %s {",Lang->Format[Lang->File[i].i_format].Name );
      for( i1=0 ; i1<Lang->File[i].FileName.j ; i1++ )
         fprintf( fw," %s",Lang->File[i].FileName[i1] );
      fprintf( fw," };");
   }
}
/**************************************************************************/
void print_head( FILE *fw )
{
   // ----- голова -------------------------
   fprintf( fw,"\n<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML//EN\">" );
   fprintf( fw,"\n<html>");
   fprintf( fw,"\n<head>");
   fprintf( fw,"\n<meta http-equiv=\"Content-Type\" content=\"text/html; charset=koi8-r\">");
   fprintf( fw,"\n<title>Анисимов \"Грамматика\"</title>");
   fprintf( fw,"\n</head>");
   fprintf( fw,"\n<body>");
   fprintf( fw,"\n<pre>");
}
/**************************************************************************/
void print_tail( FILE *fw )
{
   // ----- хвост -------------------------
   fprintf( fw,"\n</pre> " );
   fprintf( fw,"\n</body>" );
   fprintf( fw,"\n</html>" );
}
/**************************************************************************/
void print_rem( FILE *fw, short i_file, short i_struct, short type )
{  long i1 ;

   for( i1=0 ; i1<n_Rem ; i1++ )
   {  if( Rem[i1].type!=type ) continue ;
      if( Rem[i1].i_file!=i_file ) continue ;
      if( Rem[i1].i_struct!=i_struct ) continue ;
      fprintf( fw,"\n%s",Rem[i1].Str );
   }
}
/**************************************************************************/
void make_remark( void )
{  long   i,j,L,j_File ;
   short  type,i_struct ;
   t_Rem  Rem1 ;
   char  *Mass ;

   L=file_to_mass( "grammar.more", &Mass );
   
   Struct=(t_dStruct *)Calloc(1000,sizeof(t_dStruct));
   Rem   =(t_Rem     *)Calloc(1000,sizeof(t_Rem)    );
   
   for( i=j=0 ; i<L ; i++ )
   {  if( 0==strncmp( Mass+i,"::",2 ) )
          strcpy( File[j++].Name,get_name(Mass+i+2) );
   }
   n_File=j ;
   
   type=0 ;
   Rem1.i_struct=-1 ;
   Rem1.i_file  =-1 ;
   for( i=j=j_File=0 ; i<L ; i++ )
   {  if( 0==strncmp( Mass+i,"::",2 ) )
      {   set_rem_file( j_File-1 );
          j_File++ ;
      }
      if( 0==strncmp( Mass+i,"@Перевод",8) ) 
          type=1 ;
      if( 0==strncmp( Mass+i,"@Структура",10) ) 
      {   type++ ; 
          if( 4<=type ) type=0 ;
          i_struct=Grammar.find_struct( TO,get_name(Mass+i+11) );
	  if( i_struct<0 ) 
	      continue ;
          if( 1000<=i_struct )
	  {   printf("\n Table of Structs overflow!!!\n"); exit(-1);
	  }
	  Struct[i_struct].i_file=j_File-1 ;
          set_rem_index( i_struct );
      }
      if( 0==strncmp( Mass+i,"@Выбор",6) ) 
      {   type++ ;
          if( 4<=type ) type=0 ;
          i_struct=Grammar.find_struct( TO,get_name(Mass+i+7) );
	  if( i_struct<0 ) 
	      continue ;
	  Struct[i_struct].i_file=j_File-1 ;
          set_rem_index( i_struct );
      }
      if( Mass[i]=='}' ) type=0 ;
      if( 0==strncmp( Mass+i,"//",2) )
      {   Rem1.type=type ;
          if( type<=1 ) Rem1.i_struct=-1 ;
              else      Rem1.i_struct=i_struct ;
	  strcpy( Rem1.Str,get_rem( Mass+i ) );
	  Rem[j_Rem++]=Rem1 ;
	  Rem[j_Rem].i_struct=-1 ;
	  Rem[j_Rem].i_file  =-1 ;
      }
   }
   set_rem_file( j_File-1 );
   n_Rem=j_Rem ;
//   free(Mass);
}
/**************************************************************************/

/**************************************************************************/
int main( void )
{  short   i,i_File ;
   char    Buf[100] ;
   e_Type  t ;
   FILE   *fw ;

   my_more_main( );
   Grammar.make_all( "../grammar/lang.txt" );
   make_remark();

   for( i_File=0 ; i_File<n_File ; i_File++ )
   {  sprintf( Buf,"../doc/html_grammar/%s.html",File[i_File].Name );
      fw=Fopen( Buf,"w" );
      print_head( fw );
      
      for( i=Grammar.from().Part.j ; i<Grammar.n_trans() ; i++ )
      {  t=Grammar[i].From.type ;
	 if( Struct[i].i_file!=i_File ) continue ;
	 
         fprintf( fw,"\n<a name=\"%s\"></a>",Grammar[i].From.Name );	 
         print_rem( fw, i_File, i, 0 );
         fprintf( fw,"\n<b>@Перевод\n{</b>");
	 print_rem( fw, i_File, i, 1 );
         fprintf( fw,"\n   ");
         Print_struct( fw,0, i );
         print_rem( fw, i_File, i, 2 );
         fprintf( fw,"\n   ");
         Print_struct( fw,1, i );     
	 print_rem( fw, i_File, i, 3 );
         fprintf( fw,"\n<b>}</b>");
      }
      
      print_tail( fw );
      Fclose(fw);
   }
   fw=Fopen( "../doc/html_grammar/lang.h.html","w" );

   print_head( fw );
   fprintf( fw,"\n<B>@Атомы_источника\n{</B>");
   print_lang( fw,0 );
   fprintf( fw,"\n<b>}</b>");
   fprintf( fw,"\n<B>@Атомы_приемника\n{</B>");
   print_lang( fw,1 );
   fprintf( fw,"\n<b>}</b>");
   print_tail( fw );

/*
   fprintf( fw,"\n<b>@Переводы\n{</b>");
   for( i=0 ; i<Grammar.n_trans() ; i++ )
   {  t=Grammar[i].From.type ;
      if( t==TWORD || t==TSTRUCT2 || t==TSTRUCT1 || t==TNULL ) continue ;
      printf("\n<a name=\"%s\"></a>",Grammar[i].From.Name );
      printf("\n<b>@Перевод\n{</b>");
      printf("\n   ");
      Print_struct( 0, i );
      printf("\n   ");
      Print_struct( 1, i );     
      printf("\n<b>}</b>");
   }
   printf("\n<b>}</b>");
*/ 
   unlink("grammar.more");
//   printf("\nEnd");
   return 0 ;  
}
/**************************************************************************/
void my_more( char *Name )
{
   char Buf[100] ;

   sprintf( Buf,"echo ::%s >> grammar.more",Name );
   system ( Buf );
   sprintf( Buf,"more ../grammar/%s >> grammar.more",Name);
   system ( Buf );
}
/**************************************************************************/
//   создать временный файл грамматики
/**************************************************************************/
void my_more_main( void )
{
   system( "rm -f grammar.more");
   my_more("lang.h");
   my_more("atom.h");
   my_more("num.h");
   my_more("sub.h");
   my_more("adj.h");
   my_more("adv.h");
   my_more("verb.h");
   my_more("verb_be.h");
   my_more("verb_there_is.h");
   my_more("verb_have.h");
   my_more("verb_mod.h");
   my_more("verb_mod_be.h");
   my_more("verb_mod_have.h");
   my_more("verb_c.h");
   my_more("tail.h");
   my_more("tail_is.h");
   my_more("wh_frasa.h");
   my_more("core.h");
   my_more("wh_frasa.h");
   my_more("lang.cpp");
}
/**************************************************************************/
//                 достать строку из текста
/**************************************************************************/
char *get_name( char *Str )
{  static char Buf[100] ;
   short  i,j=0 ;

   for( i=0 ; i<99 ; i++ )
      if( Str[i]!=' ' ) break ;

   for( ; i<99 ; i++ )
   {  if( Str[i]==' ' || Str[i]=='\n' || Str[i]=='\t' || Str[i]=='(' ) break ;
      Buf[j++]=Str[i] ;
   }
   Buf[j]=0 ;
   return Buf ;
}
/**************************************************************************/
//       достать коментарий из текста
/**************************************************************************/
char *get_rem( char *Str )
{  static char Buf[100] ;
   short  i ;

   for( i=0 ; i<99 ; i++ )
   {  if( Str[i]=='\n' ) break ;
      Buf[i]=Str[i] ;
   }
   Buf[i]=0 ;
   return Buf ;
}
/**************************************************************************/
void set_rem_file( short i_file )
{  long i ;

   if( i_file<0 ) return ;
   for( i=j_Rem-1 ; 0<=i ; i-- )
   {  if( 0<=Rem[i].i_file ) break ;
      Rem[i].i_file=i_file ;
   }
}
/**************************************************************************/
void set_rem_index( short i_struct )
{  long i ;

   if( i_struct<0 ) return ;
   for( i=j_Rem-1 ; 0<=i ; i-- )
   {  if( 0<=Rem[i].i_struct ) break ;
      Rem[i].i_struct=i_struct ;
   }
   if( i<Struct[i_struct].i_rem ) Struct[i_struct].i_rem=i ;
}
