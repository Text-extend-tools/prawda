/***************************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                 словарь переводов слов и словосочетаний
//       Анисимов Д.В.                                    сегодня
/***************************************************************************/

# include <stdio.h>
# include <malloc.h>
# include <string.h>
# include <stdlib.h>

# include <mylib.h>
# include <grammar.h>
# include <slowo3.h>
# include <core.h>
# include <factorial.cpp>

# define DELTA_MASS   1000
# define DELTA_WORD    100
# define DELTA_STRUCT   40
# define DELTA_RECORD   10
# define DELTA_REL      50

static t_Slowo3 *Slowo3 ;
char   Word0[100] ;
t_Factorial Factorial(5) ;

char *filename( char *Path, char *DirLang, char *File );
short find_param1( t_Param1List *List, char *Str );
int   funk0( const void *a, const void *b );

/***************************************************************************/
//                          инициировать нулями
/***************************************************************************/
t_Slowo3 :: t_Slowo3( void )
{
   FileName[0]=0 ;
   Format  =NULL ;

   Mass    =NULL ;
   Word    =NULL ;
   Struct  =NULL ;
   Record  =NULL ;
   Relation=NULL ;

   n_Mass    = l_Mass    = 0 ;
   n_Word    = l_Word    = 0 ;
   n_Struct  = l_Struct  = 0 ;
   n_Record  = l_Record  = 0 ;
   n_Tree    = l_Tree    = 0 ;
   n_Relation= l_Relation= 0 ;

   short i ;
   for( i=0 ; i<256 ; i++ )
      Type[i]=0 ;
   Type['=']=Type[';']=Type[':']=Type['~']=1 ;
   Type['(']=Type[')']=Type['[']=Type[']']=Type['<']=Type['>']=1 ;
}
/***************************************************************************/
// 
/***************************************************************************/
void t_Slowo3 :: del( void )
{
   Free( Mass );
   Free( Word );
   Free( Struct );
   Free( Record );
   Free( Tree );
   Free( Relation );
}
/***************************************************************************/
//                     записать бинарный образ
/***************************************************************************/
void t_Slowo3 :: write_bin( char *File )
{  FILE *fw ;

   fw=Fopen( File,"wb" );
   
   Fwrite( &n_Mass    ,1,sizeof(long),fw );
   Fwrite( &n_Word    ,1,sizeof(long),fw );
   Fwrite( &n_Struct  ,1,sizeof(long),fw );
   Fwrite( &n_Record  ,1,sizeof(long),fw );
   Fwrite( &n_Tree    ,1,sizeof(long),fw );
   Fwrite( &n_Relation,1,sizeof(long),fw );

   Fwrite( Mass    ,n_Mass+1    ,sizeof(char)      ,fw );
   Fwrite( Word    ,n_Word+1    ,sizeof(t_sWord)   ,fw );
   Fwrite( Struct  ,n_Struct+1  ,sizeof(t_sStruct) ,fw );
   Fwrite( Record  ,n_Record+1  ,sizeof(t_sRecord) ,fw );
   Fwrite( Tree    ,n_Tree      ,sizeof(t_sTree)   ,fw );
   Fwrite( Relation,n_Relation+1,sizeof(t_Relation),fw );

   Fclose( fw );
}
/***************************************************************************/
//                     прочитать бинарный образ
/***************************************************************************/
char t_Slowo3 :: read_bin( char *File )
{  FILE *fr ;
   long  i ;
try
{
   fr=Fopen( File,"rb" );
   del( );
   
   Fread( &n_Mass    ,1,sizeof(long),fr );
   Fread( &n_Word    ,1,sizeof(long),fr );
   Fread( &n_Struct  ,1,sizeof(long),fr );
   Fread( &n_Record  ,1,sizeof(long),fr );
   Fread( &n_Tree    ,1,sizeof(long),fr );
   Fread( &n_Relation,1,sizeof(long),fr );

   Mass    =(uchar      *)Malloc( n_Mass+1    ,sizeof(uchar));
   Word    =(t_sWord    *)Malloc( n_Word+1    ,sizeof(t_sWord));
   Struct  =(t_sStruct  *)Malloc( n_Struct+1  ,sizeof(t_sStruct));
   Record  =(t_sRecord  *)Malloc( n_Record+1  ,sizeof(t_sRecord));
   Tree    =(t_sTree    *)Malloc( n_Tree      ,sizeof(t_sTree));
   Relation=(t_Relation *)Malloc( n_Relation+1,sizeof(t_Relation));

   Fread( Mass    ,n_Mass+1    ,sizeof(char)      ,fr );
   Fread( Word    ,n_Word+1    ,sizeof(t_sWord)   ,fr );
   Fread( Struct  ,n_Struct+1  ,sizeof(t_sStruct) ,fr );
   Fread( Record  ,n_Record+1  ,sizeof(t_sRecord) ,fr );
   Fread( Tree    ,n_Tree      ,sizeof(t_sTree)   ,fr );
   Fread( Relation,n_Relation+1,sizeof(t_Relation),fr );

   Fclose( fr );
   for( i=0 ; i<n_Struct ; i++ )
      Struct[i].Word=Word+Struct[i].sy_word ;

   for( i=0 ; i<n_Word ; i++ )
      Word[i].str=(char *)Mass+Word[i].sy ;

   l_Mass=n_Mass ;
   return 0 ;
}
catch( ... )
{
   return -1 ;
}
}
/***************************************************************************/
//              прочитать из текстового формата все словари, 
//              и сделать из них единый словарь
/***************************************************************************/
void t_Slowo3 :: read( char *Dir, t_Grammar *Gr )
{
   long  i,i1,j,N,j_Slowo ;
   long  j_Word,j_Struct,j_Record,j_Relation ;
   t_File    *FF ;
   t_Slowo3  *Slowo3,*SS ;
   t_FileList &File1=Gr->file1() ;
   char   Filename[300] ;

   del( );

   Format=&(Gr->format1()[0]) ;

   for( N=i=0 ; i<File1.j ; i++ )
      N+=File1[i].FileName.j ;

   Slowo3=(t_Slowo3 *)Calloc( N,sizeof(t_Slowo3) );

   // ------ сосчитать потребную память --------------------------
   n_Mass=n_Word=n_Struct=n_Record=n_Relation=0 ;
   for( j_Slowo=i=0 ; i<File1.j ; i++ )
   {  FF=&File1[i] ;
      for( i1=0 ; i1<FF->FileName.j ; i1++ )
      {  SS=&Slowo3[j_Slowo] ;
	 SS->Format=&(Gr->format1()[FF->i_format]) ;
	 
	 Strcpy( Filename,Dir,300 ); 
         if( Filename[strlen(Filename)-1]!='/' )
             Strcat( Filename,"/",300 );
	 Strcat( Filename,FF->FileName[i1],300 );
	 
	 SS->read0( Filename );
	 SS->parse( );
	 n_Mass     += SS->n_Mass ;
	 n_Word     += SS->n_Word ;
	 n_Struct   += SS->n_Struct ;
	 n_Record   += SS->n_Record ;
	 n_Relation += SS->n_Relation ;
	 j_Slowo++ ;
      }
   }

   // ------ захватить память ------------------------------------
   l_Mass     = n_Mass ;
   l_Word     = n_Word ;
   l_Struct   = n_Struct ;
   l_Record   = n_Record ;
   l_Relation = n_Relation ;
   if( 1 ) // Ой, что это?!!
   {
      l_Mass     += DELTA_MASS ;
      l_Word     += DELTA_WORD ;
      l_Struct   += DELTA_STRUCT ;
      l_Record   += DELTA_RECORD ;
      l_Relation += DELTA_REL ;   
   }

   Mass    =(uchar      *)Calloc( l_Mass+1    ,sizeof(uchar    ) );
   Word    =(t_sWord    *)Calloc( l_Word+1    ,sizeof(t_sWord  ) );
   Struct  =(t_sStruct  *)Calloc( l_Struct+1  ,sizeof(t_sStruct) );
   Record  =(t_sRecord  *)Calloc( l_Record+1  ,sizeof(t_sRecord) );
   Relation=(t_Relation *)Calloc( l_Relation+1,sizeof(t_Relation));

   // ------ переписать структуры и слова из всех словарей в один -
   j=j_Word=j_Struct=j_Record=j_Relation=0 ;
   for( i=0 ; i<N ; i++ )
   {  SS=&Slowo3[i] ;
      t_sRecord *R ;
      t_sStruct *S ;

      memcpy( Record+j_Record,SS->Record,SS->n_Record*sizeof(t_sRecord) );
      for( i1=0 ; i1<SS->n_Record ; i1++ )
      {  R=&Record[j_Record++] ;
         R->sy_struct+=j_Struct ;
	 R->i_dict=i ;
      }
      
      memcpy( Struct+j_Struct,SS->Struct,SS->n_Struct*sizeof(t_sStruct) );
      for( i1=0 ; i1<SS->n_Struct ; i1++ )
      {  S=&Struct[j_Struct++] ;
         S->sy_word   +=j_Word ;
	 S->i_relation+=j_Relation ;
      }

      memcpy( Word+j_Word,SS->Word,SS->n_Word*sizeof(t_sWord) );
      for( i1=0 ; i1<SS->n_Word ; i1++ )
	 Word[j_Word++].sy+=j ;

      for( i1=0 ; i1<SS->n_Relation ; i1++ )
         Relation[j_Relation++]=SS->Relation[i1] ;

      memcpy( Mass+j,SS->Mass,SS->n_Mass );
      j+=SS->n_Mass ;
   }
   // ----- освободить память от исходных словарей ----------------
   for( i=0 ; i<N ; i++ )
      Slowo3[i].del( );
   Free( Slowo3 );

   // ----- проставть конечные значения ---------------------------
   Struct[j_Struct].sy_word   =j_Word ;
   Struct[j_Struct].i_relation=j_Relation ;
   Record[j_Record].sy_struct =j_Struct ;

   // ----- проставть ссылки куда надо ----------------------------
   for( i=0 ; i<n_Struct ; i++ )
      Struct[i].Word=Word+Struct[i].sy_word ;

   for( i=0 ; i<n_Word ; i++ )
      Word[i].str=(char *)Mass+Word[i].sy ;

   split_unorder( );
   sort0( );
   make_tree( );
}
/***************************************************************************/
//             прочесть из файла и построить все структуры
/***************************************************************************/
void t_Slowo3 :: read( char *File, t_Format1 *_Format )
{
   Format = _Format ;

   read0( File );
   parse( );
   split_unorder( );
   sort0( );
   make_tree( );
}
/***************************************************************************/
//             прочесть из массива и построить все структуры
/***************************************************************************/
void t_Slowo3 :: make( char *_Mass, t_Format1 *_Format )
{
   Format = _Format ;

   make0( _Mass );
   parse( );
   sort0( );
   make_tree( );
}
/***************************************************************************/
//                     прочесть из файла
/***************************************************************************/
void t_Slowo3 :: read0( char *File )
{  FILE *fr ;

try
{
   fr=Fopen( File,"rb" );
   n_Mass=FileLength( fr );
   Mass  =(uchar *)Calloc( n_Mass+1,sizeof(uchar) );
   Fread( Mass,n_Mass,sizeof(uchar),fr );
   fclose(fr);
   n_Mass=remark( (char *)Mass, n_Mass );
}
catch( int E )
{  
   fprintf( File_Error,"\n Ребята, че-то я словарь %s не могу прочесть.",File );
   if( E==Err_Fopen ) fprintf( File_Error,"\n А он вообще-то есть?" );
   throw( E );
}
}
/***************************************************************************/
//                     прочесть из массива
/***************************************************************************/
void t_Slowo3 :: make0( char *_Mass )
{
   n_Mass=strlen( _Mass );
   Mass  =(uchar *)Calloc( n_Mass+1,1 );
   for( long i=0 ; i<n_Mass ; i++ )
      Mass[i]=_Mass[i] ;
   n_Mass=remark( (char *)Mass, n_Mass );
}
/***************************************************************************/
//                     построить все структуры
/***************************************************************************/
void t_Slowo3 :: parse( void )
{  long  i,i1,i2,j,j1 ;
   char  c,Msg[200]="" ;
   long  j_Word,j_Struct,j_Record,j_Relation ;
   char  fformat ;
   t_sStruct *S ;
   t_sWord   *W ;

try
{
   // ------------ посчитать число слов и переводов -----------------
   for( i=0,fformat=1 ; i<n_Mass ; i++ )
   {  c=Mass[i] ;
      if( c=='[' || c=='~' )
      {   n_Word++ ;
	  n_Struct+=fformat ;
	  continue ;
      }
      if( c=='\n' ) {  n_Record++ ; fformat=1 ; }
      if( c==':'  ) {  n_Struct++ ; fformat=0 ; }
   }

   n_Record++ ;
   // ------------ захватить память --------------------------------
   Word    =(t_sWord    *)Calloc( n_Word+1  ,sizeof(t_sWord) );
   Struct  =(t_sStruct  *)Calloc( n_Struct+1,sizeof(t_sStruct) );
   Record  =(t_sRecord  *)Calloc( n_Record+1,sizeof(t_sRecord) );
   Relation=(t_Relation *)Calloc( 256000    ,sizeof(t_Relation) );

   for( i=0 ; i<n_Struct ; i++ )
      Struct[i].Param.init();
   for( i=0 ; i<n_Word ; i++ )
      Word[i].Param.init();

   // ---------- расставить ссылки и определить части речи ---------------
   j_Word    =0 ; W=Word ;
   j_Struct  =0 ; S=Struct ;
   j_Record  =0 ;
   j_Relation=0 ;

   for( i=0 ; i<256 ; i++ )
      Type[i]=0 ;
   Type['=']=Type[';']=Type[':']=Type['~']=1 ;
   Type['(']=Type[')']=Type['[']=Type[']']=Type['<']=Type['>']=1 ;

   for( i=-1 ; i<n_Mass ; i++ )
   {  if( i!=-1 && Mass[i]!='\n' ) continue ;

      Record[j_Record++].sy_struct=j_Struct ;
      // -------- сформировать строку сообщения ------------
      for( i1=i+1,j=0 ; i1<n_Mass && Mass[i1]!='\n' ; i1++ )
      {   if( 198<j ) break ;
          Msg[j++]=Mass[i1] ;
      }
      Msg[j]=0 ;

      parse1( i, j_Word, j_Struct, j_Relation );
   }   
   // ----- проставть конечные значения --------------
   Struct[j_Struct].sy_word   =j_Word ;
   Struct[j_Struct].i_relation=j_Relation ;
   Record[j_Record].sy_struct =j_Struct ;

   // ----- проставть ссылки Struct[].Word -----------

   for( i=0 ; i<n_Struct ; i++ )
   {  Struct[i].Word=Word+Struct[i].sy_word ;
      Struct[i].n_Word=Struct[i+1].sy_word-Struct[i].sy_word ;
      Relation[Struct[i].i_relation].s1=Struct[i+1].i_relation-Struct[i].i_relation-1 ;
   }

   // ----- проставть Record[].n_struct --------------
   for( i=0 ; i<n_Record ; i++ )
      Record[i].n_struct=Record[i+1].sy_struct-Record[i].sy_struct ;

   // ------ проверить TSTRUCT1 ----------------------
{
   char f_error=0 ;
   for( i=0 ; i<n_Record ; i++ )
   {  t_Struct *S0 ;
      for( i1=0 ; i1<Record[i].n_struct ; i1++ )
      {  S=&Struct[Record[i].sy_struct+i1] ;
      
         if( S->i_struct<0 ) continue ;
         if( i1==0 ) S0=&Grammar[S->i_struct].From ;
	     else    S0=&Grammar[S->i_struct].To ;
         if( S0->type!=TSTRUCT1 ) continue ; // ABR
         if( S0->Word.j!=S->n_Word )
	 {   fprintf( File_Error,"\n Несоответствие числа слов в Структура1");
	     fprintf( File_Error,"\n строка %ld",i+1 );	     
	     f_error=1 ;
	 }
	 for( i2=0 ; i2<S0->Word.j ; i2++ )
	 {  if( S0->Word[i2].i_struct!=S->Word[i2].i_struct )
	    {   fprintf( File_Error,"\n Несоответствие типов слов в Структура1");
	        fprintf( File_Error,"\n строка %ld",i+1 );
		f_error=1 ;
	    }
	 }
      }
   }
   if( f_error==1 ) throw(-1);
}
   
   // ----- поджать массив Record --------------------
   for( i=j_Record=0 ; i<n_Record ; i++ )
      if( 0<Record[i].n_struct ) Record[j_Record++]=Record[i] ;
   n_Record=j_Record ;
   
   // ------ проставить ссылки Word[].str ------------
   for( i=0 ; i<n_Word ; i++ )
      Word[i].str=(char *)Mass+Word[i].sy ;

   // ------ исключить нулевые строки ----------------
   for( i=0 ; i<n_Mass ; i++ )
      if( Mass[i]=='\n' ) Mass[i]=0 ;     

   Relation=(t_Relation *)Realloc( Relation,j_Relation*sizeof(t_Relation) );
   n_Relation=j_Relation ;
}
catch( int E )
{
   fprintf( File_Error,"\nSlowo3::Строка=\"%s\"\n",Msg );
   Read_Error=-1 ;
   throw( E );
}
}
/************************************************************************/
//                прочесть одну строку словаря
/************************************************************************/
void t_Slowo3 :: parse1( long i, long &j_Word, long &j_Struct, long &j_Relation )
{
   long  i1,i2,j,j1 ;
   char  c,Reg[100] ;
   char  fformat,  // признак, что разбирается сложная структура
         fstr,     // признак, что разбирается заголовок структуры
         fdst;     // признак, что разбирается структура-приемник

   t_sStruct *S=&Struct[j_Struct] ;
   t_sWord   *W=&Word[j_Word] ;

   fformat=fdst=fstr=0 ;
   // -------- определить, является ли строка словом или структурой -------
   for( i1=i+1 ; i1<n_Mass && Mass[i1]!='[' ; i1++ )
      if( Mass[i1]==':' ) {  fformat=fstr=1 ; break ;  }

   // -------- разбор строки ----------------------------
   for( i1=i+1,j=0 ; i1<n_Mass && Mass[i1]!='\n' ; i1++ )
   {
      c=Mass[i1] ;
      if( j<98 ) Reg[j++]=c ;
      if( c<0 || Type[c]==0 ) continue ;

      if( c=='=' )
      {  fdst=1 ;
	 fstr=fformat ;
	 j=0 ;
      }
      if( c==';' )
      {  fstr=fformat ;
	 j=0 ;
	 continue ;
      }

      if( fstr==1 )
      {   // -------- если разбирается заголовок структуры --------
	  if( c==':' || c=='(')
	  {   short i_str ;

	      if( Mass[i1-1]!=')' )
	      {
		  Reg[j-1]=0 ;
		  for( j1=i2=0 ; i2<j ; i2++ )
		     if( Reg[i2]!=' ' ) Reg[j1++]=Reg[i2] ;
		  S->sy_word =j_Word ;
		  i_str=Format->find_struct( fdst,Reg );
		  if( i_str<=-10 )
        	  {   fprintf( File_Error,"\n Неизвестный тэг\"%s\"\n",Reg ); throw(-1);  }
		  S->i_struct=i_str ;
		  S->i_relation=j_Relation++ ;
		  S=Struct+(++j_Struct) ;
	      }
	      if( c==':' ) fstr=0 ;
	      j=0 ;
	  }
	  if( c==')' )
	  {  Reg[j-1]=0 ;
	     t_sStruct *S1=&Struct[j_Struct-1] ;
             make_relation1( fdst, S1, Reg );
	     j=0 ;
	  }
      }
      else
      {   // -------- если разбирается тело структуры --------
	  if( c=='[' || c=='~' )
	  {   short i_str ;

	      Reg[j-1]=0 ;
	      for( j1=i2=0 ; i2<j ; i2++ )
		 if( Reg[i2]!=' ' ) Reg[j1++]=Reg[i2] ;
	      i_str=Format->find_struct( fdst,Reg );
	      if( i_str<=-10 )
              {   fprintf( File_Error,"\n Неизвестный тэг\"%s\"\n",Reg ); throw(-1);  }
              W->i_struct=i_str ;
	      j=0 ;

	      if( c=='[' )
	      {  if( fformat==0 )
		 {   S->sy_word =j_Word ;
		     S->i_struct=W->i_struct ;
		     S->i_relation=j_Relation++ ;
		     S=Struct+(++j_Struct) ;
		 }
		 W->sy=i1+1 ;
	      }
	      else
	      {
		 Mass[i1]=0 ;
		 W->sy=i1 ;
		 W=Word+(++j_Word) ;
	      }
	      continue ;
	  }
	  if( c==']' )
	  {  Mass[i1]=0 ;
	     W=Word+(++j_Word) ;
	     j=0 ;
	     continue ;
	  }
	  if( c=='(' || c=='<' )
	     j=0 ;
	  if( c=='>' ) 
	  {  Reg[j-1]=0 ; Word[j_Word-1].order=atoi(Reg); j=0 ; }
	  if( c==')' )
	  {  Reg[j-1]=0 ;
	     t_sStruct *S1=&Struct[j_Struct-1] ;
	     short      iw=j_Word-S1->sy_word-1 ;
	     make_relation2( fdst, S1, iw, Reg, j_Relation );
	     j=0 ;
	  }
      }
   }
}
/***************************************************************************/
//           задать постоянные параметры заголовку структуры
//    fdst   - 0-источник 1-приемник
//    S      - заполняемая структура
//    Str    - строка, содержащая параметры, например "Родительный,Множественное"
/***************************************************************************/
void t_Slowo3 :: make_relation1( char fdst, t_sStruct *S, char *Str )
{
   short  i,j,j_param ;
   char   c,Str1[20] ;
   t_Form F ;

   for( i=j=j_param=0 ; i<100 ; i++ )
   {  c=Str[i] ;
      if( c==' ' ) continue ;
      if( c==0 || c==',' )
      {   Str1[j]=0 ; j=0 ;
          F.value[j_param]=find_value1( S->i_struct, j_param, fdst, Str1 ) ;
	  j_param++ ;
      }
      if( c==0 ) break ;
      if( c==',' ) continue ;
      Str1[j++]=c ;
   }
   S->Param=F ;
}
/***************************************************************************/
//           сделать таблицу передачи параметров для структуры,
//           и задать постоянные параметры
//    fdst   - 0-источник 1-приемник
//    S      - заполняемая структура
//    i_word - индекс части структуры  t_sStruct->Word[i_word]
//    Str    - строка, содержащая параметры, например "Родительный,Множественное"
//    j_Relation - ссылка на первый свободный элемент в массиве Relation
/***************************************************************************/
void t_Slowo3 :: make_relation2( char fdst, t_sStruct *S, long i_word, 
                  char *Str, long &j_Relation )
{
   t_sWord      *W=Word+S->sy_word+i_word ;
   t_Param1List *pParam ;
   t_Relation    R ;
   t_Form        F ;
   short  i,i1,j,j_param,v ;
   char   c,Str1[20] ;

   if( i_word<0 )
   {  fprintf( File_Error,"\nmake_relation i_word<0 !!!\n"); throw(-1);  }

   F.init();
   for( i=j=j_param=0 ; i<100 ; i++ )
   {  c=Str[i] ;
      if( c==' ' ) continue ;
      if( c==0 || c==',' )
      {   Str1[j]=0 ; j=0 ;

          v=find_value1( W->i_struct, j_param, fdst, Str1 ) ;

          if( 0<=v || 0==strcmp(Str1,"@0") )
	      F.value[j_param]=v ;
	  else
	  {
             if( S->i_struct<0 )
	     {  fprintf( File_Error,"\nmake_relation1::S->i_struct<0"); throw(-1);  }

	     if( fdst==0 )
	              pParam=&Grammar[S->i_struct].From.Param ;
		 else pParam=&Grammar[S->i_struct].To.Param ;

	     i1=find_param1( pParam, Str1 );

	     if( i1<0 )
	     {  fprintf( File_Error,"\nmake_relation2::параметр структуры не найден"); throw(-1);  }

	     if( pParam->list[i1].Dir==1 )
	     {  // ----- передача параметров от структуры к ее части
	     	R.s1=0 ;
		R.p1=i1 ;
		R.s2=i_word+1 ;
		R.p2=j_param ;
	     }
	     else
	     {  // ----- передача параметров от части к структуре
	     	R.s2=0 ;
		R.p2=i1 ;
		R.s1=i_word+1 ;
		R.p1=j_param ;
	     }
	     Relation[j_Relation++]=R ;
	  }
	  j_param++ ;
      }
      if( c==0 ) break ;
      if( c==',' ) continue ;
      Str1[j++]=c ;
   }
   W->Param=F ;
}
/***************************************************************************/
//   найти значение грамматического параметра по имени значения 
//   i_struct - индекс части речи или структуры
//   i_param  - индекс параметра в этой структуре t_Struct->Param[i_struct]
//   fdst     - 0-источник 1-приемник
//   Str1     - значение параметра, например "Дательный"
/***************************************************************************/
short t_Slowo3 :: find_value1( short i_struct, short i_param, char fdst, char *Str1 )
{  short    v, i_rename ;
   t_Param1List *pParam ;
   t_ValueList  *value ;

   if( i_struct<0 )
   {  fprintf( File_Error,"\nmake_relation2::W->i_struct<0"); throw(-1);  }

   if( fdst==0 )
   {   pParam=&Grammar[i_struct].From.Param ;
       if( i_param<0 || pParam->j<=i_param )
       {  fprintf( File_Error,"\nmake_relation2::i_param Error"); throw(-1);  }
       value =&Grammar.from().Param[pParam->list[i_param].param].Value ;
   }
   else
   {   pParam=&Grammar[i_struct].To.Param ;
       if( i_param<0 || pParam->j<=i_param )
       {  fprintf( File_Error,"\nmake_relation2::i_param Error"); throw(-1);  }
       value =&Grammar.to().Param[pParam->list[i_param].param].Value ;
   }

   if( pParam->j<=i_param )
   {  fprintf( File_Error,"\nmake_relation2::pParam.j<=i_param"); throw(-1);  }

   i_rename=find_rename( Str1 );
   if( 0<=i_rename )
	    v=find_value( value, Format->Rename[i_rename].Full );
       else v=find_value( value, Str1 );
	      
   return v ;
}
/***************************************************************************/
//
/***************************************************************************/
short t_Slowo3 :: find_rename( char *Str )
{
   for( short i=0 ; i<Format->Rename.j ; i++ )
      if( 0==strcmp( Str,Format->Rename[i].Reduce ) ) return i ;
   return -1 ;
}
/***************************************************************************/
//               построить дерево поиска слов и выражений
/***************************************************************************/
void t_Slowo3 :: make_tree( void )
{
   long  i,i1,j ;
   short s,s1,n ;
   char  *Str,*Str1 ;
   t_sStruct *S,*S1 ;
   t_sTree   *T ;

   Free(Tree);
   Tree=(t_sTree *)Calloc( 2+4*n_Word,sizeof(t_sTree) );

   Tree[0].up=-1 ;
   Tree[0].down=1 ;
   Tree[0].first =0 ;
   Tree[0].last  =n_Record ;
   Tree[0].rang  =-2 ;
   // ----------- первый уровень - по индексам структур --------
   Tree[1].first=0 ;
   Tree[1].rang =-1 ;
   s=Struct[Record[0].sy_struct].i_struct ;

   for( i=0,j=2 ; i<n_Record ; i++ )
   {  s1=Struct[Record[i].sy_struct].i_struct ;
      if( s1!=s )
      {   Tree[j-1].last = i-1 ;
	  Tree[j].first  = i ;
	  Tree[j].rang   =-1 ;
	  j++ ;
	  s=s1 ;
      }
   }
   Tree[j-1].last=n_Record ;
   Tree[0].n_down=j-1 ;
   // ----------- следующие уровни - по индексам слов --------
   for( i=1 ; i<j ; i++ )
   {  T=&Tree[i] ;
      n=T->rang+1 ;

      S=&Struct[Record[T->first].sy_struct] ;
      if( (T->last-T->first)<=1 && S->n_Word<=n )
      {
         T->down=-1 ;
	 T->n_down=0 ;
         continue ;
      }
      T->down=j ;
      S =&Struct[Record[T->first].sy_struct] ;
      S1=&Struct[Record[T->last ].sy_struct] ;
      if( S->n_Word<=n  ) s =-2 ; else s =S->Word[n].i_struct ;
      if( S1->n_Word<=n ) s1=-2 ; else s1=S1->Word[n].i_struct ;
      if( s!=s1 )
      {   // ---- структура с неодинаковыми сыновьями ----
          T->empty=2 ;
	  s=-10 ;
          for( i1=T->first ; i1<=T->last ; i1++ )
	  {  S=&Struct[Record[i1].sy_struct] ;
	     if( S->n_Word<=n ) s1=-2 ;
		 else           s1=S->Word[n].i_struct ;
	     if( s1!=s )
	     {   if( i1!=0 && s!=-10 ) Tree[j-1].last =i1-1 ;
		 Tree[j].up     =i ;
		 Tree[j].first  =i1 ;
		 Tree[j].rang   =n-1 ;
		 Tree[j].empty  =1 ;
		 s=s1 ;
		 j++ ;
	      }
	  }
	  Tree[j-1].last=T->last ;
	  T->n_down     =j-T->down ;
      }
      else
      {   // ---- структура с одинаковыми сыновьями ----
          Str="\7" ;
	  for( i1=T->first ; i1<=T->last ; i1++ )
	  {  S=&Struct[Record[i1].sy_struct] ;
	     if( S->n_Word<=n ) Str1="" ;
		 else           Str1=(char *)Mass+S->Word[n].sy ;
	     if( 0!=Strcmp( Str1,Str ) || S->n_Word==(n+1) )
	     {   if( i1!=T->first ) Tree[j-1].last =i1-1 ;
		 Tree[j].up     =i ;
		 Tree[j].first  =i1 ;
		 Tree[j].rang   =n ;
		 Str=Str1 ;
		 j++ ;
	     }
	  }
	  Tree[j-1].last=T->last ;
	  T->n_down     =j-T->down ;
       }
   }
   n_Tree=j ;
   Tree=(t_sTree *)Realloc( Tree,j*sizeof(t_sTree));
}
/***************************************************************************/
//             размножить все cтруктуры типа "беспорядок"
/***************************************************************************/
void t_Slowo3 :: split_unorder( void )
{  long i,i1,i2,f,ns,z,N ;
   long j_Record,  j_Struct,  j_Word,  j_Relation ;
   long n_Record1, n_Struct1, n_Word1, n_Relation1 ;
   e_Type     t ;
   t_sRecord *R ;
   t_sStruct *S ;

   // --------- сосчитать потребную дополнительную память -------------   
   n_Record1=n_Struct1=n_Word1=n_Relation1=0 ;

   for( f=i=0 ; i<n_Record ; i++ )
   {  S=&Struct[Record[i].sy_struct] ;
      if( S->i_struct<0 ) continue ;
      t=Grammar[S->i_struct].From.type ;
      if( t==TUNORDER1 || t==TUNORDER2 )
      {   N=Factorial.fak[S->n_Word]-1 ;
          n_Record1  +=N ;
	  n_Struct1  +=N*Record[i].n_struct ;
	  n_Word1    +=N*S->n_Word ;
	  n_Relation1+=N*(Relation[S->i_relation].s1+1) ;
      }
   }
   if( n_Record1==0 ) return ;

   long NN   =n_Record ;
   // --------- подхват памяти ----------------------------------------
   j_Record  =n_Record   ; n_Record  +=n_Record1 ;
   j_Struct  =n_Struct   ; n_Struct  +=n_Struct1 ;
   j_Word    =n_Word     ; n_Word    +=n_Word1 ;
   j_Relation=n_Relation ; n_Relation+=n_Relation1 ;
   Record  =(t_sRecord  *)Realloc( Record  ,(n_Record+1)* sizeof( t_sRecord  ) );
   Struct  =(t_sStruct  *)Realloc( Struct  ,(n_Struct+1)* sizeof( t_sStruct  ) );
   Word    =(t_sWord    *)Realloc( Word    ,n_Word      * sizeof( t_sWord    ) );
   Relation=(t_Relation *)Realloc( Relation,n_Relation  * sizeof( t_Relation ) );

   // ---- собственно размножение структур -----------------
   // ---- c Relation не понятно что делать
   for( f=i=0 ; i<NN ; i++ )
   {  R=&Record[i] ;
      S=&Struct[Record[i].sy_struct] ;
      if( S->i_struct<0 ) continue ;
      t=Grammar[S->i_struct].From.type ;
      if( t==TUNORDER1 || t==TUNORDER2 )
      {   N=Factorial.fak[S->n_Word] ;
          for( i1=1 ; i1<N ; i1++ )
	  {  Record[j_Record]=*R ;
	     Record[j_Record].sy_struct=j_Struct ;
	     Struct[j_Struct]=*S ;
	     Struct[j_Struct].sy_word=j_Word ;
	     for( i2=0 ; i2<S->n_Word ; i2++ )
	     {  z=Factorial.value[i1*Factorial.n+i2] ;
	        Word[j_Word++]=S->Word[z] ;
	     }
	     Relation[j_Relation++].s1=ns=Relation[S->i_relation].s1 ;
	     for( i2=0 ; i2<ns ; i2++ )
	     {  t_Relation R1=Relation[S->i_relation+i2+1] ;
	        Relation[j_Relation]   =R1 ;
	        Relation[j_Relation].s1=Factorial.use(i1,R1.s1) ;
		Relation[j_Relation].s2=Factorial.use(i1,R1.s2) ;
		j_Relation++ ;
	     }
	     j_Struct++ ;
	     for( i2=1 ; i2<R->n_struct ; i2++ )
	        Struct[j_Struct++]=S[i2] ;
	     j_Record++ ;
	  }
      }
      if( n_Record<j_Record || n_Struct<j_Struct || n_Word<j_Word || n_Relation<j_Relation )
      {   printf("\nВнутренняя ошибка в t_Slowo3 :: split_unorder\n"); 
          exit(-1);  
      }
   }
   // ----- проставть конечные значения ---------------------------
   Struct[j_Struct].sy_word   =j_Word ;
   Struct[j_Struct].i_relation=j_Relation ;
   Record[j_Record].sy_struct =j_Struct ;

   // ----- проставть ссылки куда надо ----------------------------
   for( i=0 ; i<n_Struct ; i++ )
      Struct[i].Word=Word+Struct[i].sy_word ;

   for( i=0 ; i<n_Word ; i++ )
      Word[i].str=(char *)Mass+Word[i].sy ;
}
/***************************************************************************/
//              функция упорядочивания структур (больше-меньше)
/***************************************************************************/
int funk0( const void *a, const void *b )
{  t_sStruct *S1,*S2 ;
   short f,iw,nw,is1,is2 ;

   S1=Slowo3->sstruct((*(t_sRecord **)a)->sy_struct) ;
   S2=Slowo3->sstruct((*(t_sRecord **)b)->sy_struct) ;

   if( S1->i_struct!=S2->i_struct ) return S1->i_struct-S2->i_struct ;

   nw=min( S1->n_Word,S2->n_Word ) ;
   for( iw=0 ; iw<nw ; iw++ )
   {
      is1=S1->Word[iw].i_struct ;
      is2=S2->Word[iw].i_struct ;
      if( is1 != is2 ) return is1-is2 ;
      f=Strcmp( S1->Word[iw].str,S2->Word[iw].str );
      if( f!=0 ) return f ;
   }
   if( S1->n_Word != S2->n_Word ) return S1->n_Word - S2->n_Word ;
   return (*(t_sRecord **)a)->i_dict - (*(t_sRecord **)b)->i_dict ;
}
/***************************************************************************/
//            Отсортировать структуры в должном порядке
/***************************************************************************/
void t_Slowo3 :: sort0( void )
{  long i ;
   t_sRecord *R, **RR ;
   
   Slowo3=this ;
   RR=(t_sRecord **)Malloc( n_Record,sizeof(t_sRecord *) );
   for( i=0 ; i<n_Record ; i++ )
      RR[i]=Record+i ;

   qsort( RR,n_Record,sizeof(t_sRecord *),funk0 );

   R =(t_sRecord *)Calloc( n_Record+1,sizeof(t_sRecord) );
   for( i=0 ; i<n_Record ; i++ )
      R[i]=*RR[i] ;
   Free( Record ); Free( RR );
   Record=R ;
}
/***************************************************************************/
//               Вставить новые выражения в словарь
//         устарела и нигде не используется, заменена на add_new1
/***************************************************************************/
void t_Slowo3 :: add_new( char *Dir, t_DirTrans *DT, char *str )
{
   long   L ;
   FILE  *fw ;
   t_FileList &File1=Grammar.file1() ;
   char  *ff ;
   char   Str[300] ;

   if( 0!=strcmp(File1[0].FileName[File1[0].FileName.j-1],"base_new.tmp" ) )
   {   ff=(char *)Malloc( 20,sizeof(char) );
       strcpy( ff,"base_new.tmp" );
       File1[0].FileName.add( ff );
   }
   L=strlen( str );
   sprintf( Str,"%s/%s/dicts/",Dir,DT->Dir );
   fw=Fopen( Str,"w" );
   Fwrite( str,L,sizeof(char),fw );
   Fclose( fw );
   File_Error=Fopen("mercury.err","w+"); 
   read( Dir, &Grammar );
   Fclose( File_Error );
}
/************************************************************************/
//    вычислить насколько увеличатся массивы при добавлении выражения
/************************************************************************/
void t_Slowo3 :: calc_delta( char *Str, long &d_Word, long &d_Struct )
{  long i ;
   char c, fformat ;

   d_Word=0 ;
   d_Struct=0 ;

   for( i=0,fformat=1 ; Str[i]!=0 && Str[i]!='\n' ; i++ )
   {  c=Str[i] ;
      if( c=='[' || c=='~' )
      {   d_Word++ ;
	  d_Struct+=fformat ;
	  continue ;
      }
      if( c==':'  ) {  d_Struct++ ; fformat=0 ; }
   }
}
/***************************************************************************/
//               Вставить новые выражения в словарь
/***************************************************************************/
void t_Slowo3 :: add_new1( char *Str )
{
   long     i,z,L,L1 ;
   long     z_Mass,z_Word,d_Word,d_Struct ;
   t_sRecord R ;

   // ---------- вставить строку в массив ----------------
   L=strlen( Str );
   z_Mass=n_Mass ;
   if( l_Mass-n_Mass<L )
   {   L1=max( l_Mass+DELTA_MASS,n_Mass+L );
       Mass=(uchar *)Realloc( Mass,L1*sizeof(char) );   
       l_Mass=L1 ;
   }
   strcpy( (char *)Mass+n_Mass,Str );
   n_Mass+=L ;

   // ---------- раздвинуть массивы ----------------------
   calc_delta( Str, d_Word, d_Struct );
   if( n_Word+d_Word<n_Word )
   {   L1=max( l_Word+DELTA_WORD,n_Word+d_Word );
       Word=(t_sWord *)Realloc( Word,L1*sizeof(t_sWord) );
       l_Word=L1 ;
       for( i=0 ; i<n_Word ; i++ )
          Word[i].str=(char *)Mass+Word[i].sy ;
   }
   if( n_Struct+d_Struct<n_Struct )
   {   L1=max( l_Struct+DELTA_STRUCT,n_Struct+d_Struct );
       Struct=(t_sStruct *)Realloc( Struct,L1*sizeof(t_sStruct) );
       l_Struct=L1 ;
       for( i=0 ; i<n_Struct ; i++ )
          Struct[i].Word=Word+Struct[i].sy_word ;
   }
   if( n_Record+1<n_Record )
   {   L1=max( l_Record+DELTA_RECORD,n_Record+1 );
       Record=(t_sRecord *)Realloc( Record,L1*sizeof(t_sRecord) );
       l_Record=L1 ;   
   }
   R.sy_struct=n_Struct ;
   R.n_struct =d_Struct ;
   R.i_dict   =0 ;

   // --------- протранслировать строку -----------------
   // с реляциями труба - будет писать за границы массива
   z_Word=n_Word ;
   parse1( z_Mass-1, n_Word, n_Struct, n_Relation );
   
   i=R.sy_struct ;
   Struct[i].Word  =Word+Struct[i].sy_word ;
   Struct[i+1].Word=Word+Struct[i+1].sy_word ;
   Struct[i].n_Word   = Struct[i+1].sy_word - Struct[i].sy_word ;
   Struct[i+1].n_Word = n_Word - Struct[i+1].sy_word ;
   for( i=z_Word ; i<n_Word ; i++ )
      Word[i].str=(char *)Mass+Word[i].sy ;
   // --------- таки вставить строку в массив Record -----
   z=find_insert( R );
   for( i=n_Record-1 ; z<=i ; i-- )
      Record[i+1]=Record[i] ;
   Record[z]=R ;
   
   // ---------- перестроить дерево ----------------------
   make_tree( );
}
/***************************************************************************/
//          разбиение фразы на составные части (итерационная)
/***************************************************************************/
void t_Slowo3 :: s_universe( short i_word, short i_struct )
{
   long  i,i1,i2,j,z,N1,i_tree ;
   char *Str ;
   t_sTree    *T ;
   t_sStruct  *S ;
   t_sWord    *W ;
   t_Variants *VV ;
   t_rStruct   SS ;
   t_rWord     R,*pR ;
   t_Tree      N,*pN ;
   t_TreeList  Node ;

   if( n_Tree<=0 ) return ;
   if( Core.From.j<=i_word ) return ;
   i_tree=root_of_struct( i_struct );
   if( i_tree<0 ) return ;

   // ---- построение дерева вариантов ------
   N.link=i_tree ;
   N.i_last_word=i_word ;
   Node.add(N);

   for( i=0 ; i<Node.j ; i++ )
   {  i_tree=Node[i].link ;
      T=&Tree[i_tree] ;

      if( Node[i].type==TNULL ) continue ; //  прописать вариант

      if( T->empty==2 )
      {   // ---- случай с произвольными структурами и разными сыновьями ----
	  for( i1=0 ; i1<T->n_down ; i1++ )
	  {  z=T->down+i1 ;
	     N.type       = TSTRUCT ;
	     N.up         = i ;
	     N.i_word     = Node[i].i_last_word ;
	     N.i_last_word= Node[i].i_last_word ;
	     N.link       = z ;
	     N.i_struct   =-1 ;
	     N.i_variant  = 0 ;
	     N.index      = T->rang ; // T->rang+1 
	     N.i_slowo    =-1 ;
	     Node.add(N);
	  }
	  continue ;
      }

      // --------- транслировать дальше --------------
      S=Struct+Record[T->first].sy_struct ;
      W=&S->Word[T->rang+1] ;

      if( W->i_struct==-1 )
      {  // ------ слово является константой ---------
         if( Core.n_word<=Node[i].i_last_word )
	     Antwort.j=0 ;
	 else
	     find_bin( Core.From[Node[i].i_last_word].Str1, i_tree );
	 for( i1=0 ; i1<Antwort.j ; i1++ )
	 {  z=Antwort[i1] ;
	    S=Struct+Record[Tree[z].first].sy_struct ;
	    W=&S->Word[Tree[z].rang] ;
	    for( i2=j=1 ; W->str[i2]!=0 ; i2++ )
	       if( W->str[i2]==' ' && W->str[i2-1]!=' ' ) j++ ;
	    N.type       = TSTRUCT ; // формальность
	    N.up         = i ;
	    N.i_word     = Node[i].i_last_word ;
	    N.i_last_word= Node[i].i_last_word+j ;
	    N.link       = z ;
	    N.i_struct   =-1 ;
	    N.i_variant  = i1 ;
	    N.index      = T->rang+1 ;
	    N.i_slowo    =-1 ;
	    if( Tree[z].n_down<=0 )
		N.type=TNULL ;
	    Node.add(N);
	 }
      }
      else
      {  // ------ слово может иметь всякостные формы ------
         if( Core.From.j<=Node[i].i_last_word ) 
	     continue ;
	 Core.universe( Node[i].i_last_word,W->i_struct );
	 VV=Core.variants( Node[i].i_last_word,W->i_struct );
	 // ---- литерал таки задан --------
	 // ---- вот здесь херачится большое количество ненужных вариантов
	 // ---- (связанных с тем, что по-русски это слово 10-ю способами переводится)
	 //for( i1=0 ; i1<VV->Variant.j ; i1++ )
	 //if( 0<VV->Variant.j )
	 long NN ;
	 if( IF_WORD( Grammar[W->i_struct].From.type ) )
	          NN=min( 1,VV->Variant.j ) ;
	     else NN=VV->Variant.j ;
	 for( i1=0 ; i1<NN; i1++ )	 
	 {  // ------- цикл по вариантам -------------
	    Str=Core.get_meaning( &VV->Variant[i1] ) ; // смысл варианта
	    find_bin( Str, i_tree );
	    for( i2=0 ; i2<Antwort.j ; i2++ )
	    {  z=Antwort[i2] ;
	       N.type       = TSTRUCT ; // формальность
	       N.up         =i ;
	       N.i_word     =VV->Variant[i1].i_word ;
	       N.i_last_word=VV->Variant[i1].i_last_word ;
	       N.link       =z ;
	       N.i_struct   =W->i_struct ;
	       N.i_variant  =i1 ;
	       N.index      =T->rang+1 ;
	       N.i_slowo    =VV->Variant[i1].i_slowo ;
	       if( Tree[z].n_down<=0 )
		   N.type=TNULL ;
	       Node.add(N);
	    }
	 }
      }
   }
   // ---- собирание вариантов ----------------
   t_Struct *S1=&Grammar[i_struct].From ;
   VV=Core.variants( i_word, i_struct );
   for( i=0 ; i<Node.j ; i++ )
   {  if( Node[i].type!=TNULL ) continue ;
      // ----- заполнение заголовка структуры ---------
      T=&Tree[Node[i].link] ;
      SS.type       =S1->type ;
      SS.i_word     =i_word ;
      SS.i_last_word=Node[i].i_last_word ;
      SS.i_struct   =i_struct ;
      SS.r_word     =Core.rWord.j ;
      SS.i_slowo    =T->first ;
      SS.Form       =Struct[Record[T->first].sy_struct].Param.form() ;
      N1=get_n_perevod( SS.i_slowo );

      if( 0==is_atom(SS.i_slowo) ) // к стати сюда же логично воткнуть проверку
          N1=min( 1,N1 );          // на IF_WORD а не там, где она сейчас стоит

      // ---- цикл по вариантам перевода этой структуры -----
      for( i1=0 ; i1<N1 ; i1++)
      {  
         SS.i_slowo1=i1 ;
         // ----- внесение, но не заполнение составляющих структуры ---
	 for( i2=0 ; i2<=T->rang ; i2++ )
	    Core.rWord.add(R);

         // ----- заполнение составляющих структуры -----------------------
	 // ----- (в порядке задом наперед от листа дерева к его корню) ---
	 z=i ;
	 while( 1 )
	 {  pN=&Node[z] ;
	    if( Tree[pN->link].empty!=1 )
	    {   pR =&Core.rWord[SS.r_word+Tree[pN->link].rang] ;
		if( pN->i_struct<0 )
	        	 pR->type =TCONST ;
                    else pR->type =Grammar[pN->i_struct].From.type ;
		pR->i_word     =pN->i_word ;
		pR->i_last_word=pN->i_last_word ;
		pR->i_struct   =pN->i_struct ;
		pR->i_variant  =pN->i_variant ;
		pR->index      =pN->index ;
		pR->i_slowo    =pN->i_slowo ;
		pR->i_slowo1   =0 ;           // не уверен
	    }
	    z=pN->up ;
	    if( z==0 ) break ;
	 }
	 VV->Variant.add(SS);
	 Core.real_param_up( &VV->Variant[VV->Variant.j-1] );
      }
   }
}
/**************************************************************************/
//                      одновариантный перевод слова
//   From     - переводимое слово
//   i_slowo  - строка записи в словаре t_Slowo3->Record[i_slowo]
//   i_slowo1 - вариант перевода
/**************************************************************************/
char * t_Slowo3 :: translate_word_i( char *From, long i_slowo, short i_slowo1 )
{
   strcpy( Word0,From );
   if( i_slowo<0 ) return Word0 ;
   
   t_sStruct *S=&Struct[Record[i_slowo].sy_struct+1+i_slowo1] ;

   Word0[0]=0 ;   
   for( short i=0 ; i<S->n_Word ; i++ )
   {  strcat( Word0,S->Word[i].str ); 
      if( i<S->n_Word-1 ) strcat( Word0," " );  
   }
   //strcpy( Word0,(char *)Mass+Struct[Record[i_slowo].sy_struct+1+i_slowo1].Word[0].sy );
   return Word0 ;
}
/**************************************************************************/
//                      одновариантный перевод слова
//   From     - переводимое слово
//   i_struct - часть речи, которой должно быть это слово
//   i_slowo1 - вариант перевода
/**************************************************************************/
char * t_Slowo3 :: translate_word_s( char *From, short i_struct, short i_slowo1 )
{
   t_longList Ant ;
   e_Type t ;
   long   a ;
   
   if( 0<=i_struct )
   {   t=Grammar[i_struct].To.type ;
       if( t==TSTRUCT1 || t==TSTRUCT2 )
       {   strcpy( Word0,"" );  return Word0 ;  }
   
       if( Grammar[i_struct].To.type!=TWORD )
           return NULL ; // признак неправильного вызова
   }
   strcpy( Word0,From );
   if( n_Tree<=0 ) return Word0 ;
   
   find( From, i_struct, &Ant );
   if( Antwort.j<=0 ) return Word0 ; //  такого слова нет в словаре

   a=Antwort[0] ;
   if( Record[a].n_struct<=i_slowo1+1 ) i_slowo1=0 ;
   strcpy( Word0,(char *)Mass+Struct[Record[a].sy_struct+1+i_slowo1].Word[0].sy );
   return Word0 ;
}
/***************************************************************************/
//     найти слово в дереве
//     From1  - искомое слово
//     i_struct - требуемая часть речи
//     *_a, *_b  диапазон номеров в t_sRecord встречается это слово
/***************************************************************************/
void t_Slowo3 :: find( char *From1, short i_struct, t_longList *Ant )
{
   long   i,zz ;

   Antwort.j=0 ;
   Ant->j=0 ;
   if( From1[0]==0 ) return ;
   if( n_Tree<=0 ) return ;
   
   zz=root_of_struct( i_struct );
   if( zz<0 ) return ;

   find_bin( From1, zz );
   
   for( i=0 ; i<Antwort.j ; i++ )
   {  Antwort[i]=Tree[Antwort[i]].first ;
      Ant->add( Antwort[i] );
   }
}
/***************************************************************************/
//     найти СТРУКТУРУ в дереве, начиная с определенной ветки
//     если не получилось, укоротить выражение, до тех пор пока не найдется
//     From1  - искомое слово
//     zz - ветка начала поиска
/***************************************************************************/
char t_Slowo3 :: find_bin( char *From1, long zz )
{
   long   i ;
   char   From2[100] ;

   Antwort.j=0 ;
   for( i=0 ; i<100 ; i++ )
   {  if( From1[i]==0 )
      {   From2[i]=0 ; find_bin1( From2, zz ); break ;  }
      if( From1[i]==' ' )
      {   From2[i]=0 ; 
          if( 0==find_bin1( From2, zz ) ) break ;
	  From2[i]=' ' ; 
      }
      From2[i]=From1[i] ;
   }
   if( Antwort.j<=0 ) return -1 ;
   return 0 ;
}
/***************************************************************************/
//     найти СТРУКТУРУ в дереве, начиная с определенной ветки
//     и без всяких хитростев сказать, есть оно в словаре или нет
//     From1  - искомое слово
//     i_tree - ветка начала поиска
/***************************************************************************/
char t_Slowo3 :: find_bin1( char *From1, long i_tree )
{
   long   a,b,b1,c,i,L,L1 ;
   char   f, *Str ;
   char   ff=0 ;     // факт наличия строки влючающей в себя Form1

   L=strlen(From1);
   a=Tree[i_tree].down ;
   b=b1=a+Tree[i_tree].n_down-1 ;
   while( 1 )
   {
      if( b-a<4 )
      {   for( i=a ; i<=b ; i++ )
	     if( 0==Strncmp( From1,word_src(i),L ) ) break ;
	  a=i ;
	  break ;
      }
      c=(a+b)/2 ;
      f=Strncmp( From1,word_src( c ),L );
      if( f<0 ) b=c ;
      if( f>0 ) a=c ;
      if( f==0 )
      {	  for( i=c ; a<i ; i-- )
	     if( 0!=Strncmp( From1,word_src(i),L) ) {  a=i+1 ; break ;  }	  
	  break ;
      }
   }
   for( i=a ; i<=b && i<=b1 ; i++ )
   {  Str=word_src(i);      
      f=Strncmp( From1,Str,L );
      if( 0>f ) break ;
      L1 =strlen(Str);      
      if( L==L1 && f==0 ) Antwort.add(i) ;
          else    ff=1 ;
      if( 0==L && 0<L1 ) break ;
   }
   return ff ;
}
/***************************************************************************/
//          найти место в массиве Record чтобы вставть очередную
/***************************************************************************/
long t_Slowo3 :: find_insert( t_sRecord &R )
{
   long  i,a,b,c ;
   short i_struct,i1_struct ;
   char  f ;
   t_sRecord *r1,*r2 ;
   
   // ------- а есть ли вообще такие структуры? -------------
   i_struct=Struct[R.sy_struct].i_struct ;
   for( i=0 ; i<Tree[0].n_down ; i++ )
   {  i1_struct=Struct[Record[Tree[1+i].first].sy_struct].i_struct ;
      if( i_struct<i1_struct )
          return Tree[1+i].first ;
      if( i_struct==i1_struct )
          break ;
   }
   // ------ найти подходящее место двоичным поиском ------- 
   a=Tree[i+1].first ;
   b=Tree[i+1].last ;
   while( 1 )
   {
      if( b-a<4 )
      {   for( i=a ; i<=b ; i++ )
          {  r1=&R ; r2=&Record[i] ;
	     if( 0>=funk0( &r1, &r2 ) ) 
	         return i ;
          }
	  return i ; // "аварийный" индекс
      }
      c=(a+b)/2 ;
      r1=&R ; r2=&Record[c] ;
      f=funk0( &r1,&r2 );
      if( f<0 ) b=c ;
      if( f>0 ) a=c ;
      if( f==0 )
      {	  for( i=c ; a<i ; i-- )
          {  r1=&R ; r2=&Record[i] ;
	     if( 0!=funk0( &r1, &r2 ) ) 
	         return i+1 ;
          }
      }
   }
}
/***************************************************************************/
//    найти ветвь в дереве, с которой начинаются структуры типа i_struct
/***************************************************************************/
long t_Slowo3 :: root_of_struct( short i_struct )
{  long i ;

   for( i=0 ; i<Tree[0].n_down ; i++ )
      if( Struct[Record[Tree[1+i].first].sy_struct].i_struct==i_struct )
          return i+1 ;
   return -1 ;
}
/***************************************************************************/
//                какими частями речи является слово From
/***************************************************************************/
short t_Slowo3 :: part( char *From, short *i_Part )
{  short i,j ;
   t_sStruct *S ;
   t_longList Ant ;

   if( n_Tree<=0 ) return j ;
   for( i=j=0 ; i<Tree[0].n_down ; i++ )
   {
      S=Struct+Record[Tree[1+i].first].sy_struct ;
      if( S->i_struct<0 ) continue ;
      if( Grammar[S->i_struct].From.type!=TWORD ) continue ;
      find( From, S->i_struct,&Ant );
      if( 0<Ant.j )
          i_Part[j++]=S->i_struct ;
   }
   return j ;
}
/***************************************************************************/
//                постоянные параметры структуры i_slowo
/***************************************************************************/
t_Form t_Slowo3 :: struct_param( long i_slowo, long i_variant )
{
   t_Form     Form0 ;
   t_sStruct *S ;

   if( i_slowo<0 || n_Record<=i_slowo ) return Form0 ;
   if( i_variant==-1 )
            S=&Struct[Record[i_slowo].sy_struct] ;
       else S=&Struct[Record[i_slowo].sy_struct+1+i_variant] ;
   return S->Param.form() ;
}
/***************************************************************************/
//                постоянные параметры слова i_slowo
/***************************************************************************/
t_Form t_Slowo3 :: word_param( long i_slowo, long i_variant, long i_word )
{
   t_Form     Form0 ;
   t_sStruct *S ;

   if( i_slowo<0 || n_Record<=i_slowo ) return Form0 ;
   if( i_variant<-1 || Record[i_slowo].n_struct<=i_variant-1 ) return Form0 ;
   if( i_variant==-1 )
            S=&Struct[Record[i_slowo].sy_struct] ;
       else S=&Struct[Record[i_slowo].sy_struct+1+i_variant] ;
   if( i_word<0 || S->n_Word<=i_word ) return Form0 ;
   return S->Word[i_word].Param.form() ;
}
/***************************************************************************/
//
/***************************************************************************/
t_Format1 * t_Slowo3 :: format( void )
{
   return Format ;
}
/***************************************************************************/
//
/***************************************************************************/
t_sRecord * t_Slowo3 :: record( long i_record )
{
   return &Record[i_record] ;
}
/***************************************************************************/
//
/***************************************************************************/
long t_Slowo3 :: n_record( void )
{
   return n_Record ;
}
/***************************************************************************/
//        есть ли у данной конструкции неатомарные потомки
//        возвращает 1 если есть
/***************************************************************************/
char t_Slowo3 :: is_atom( long i_record )
{
   if( i_record<0 ) return 1 ;

   t_sStruct *S=&Struct[Record[i_record].sy_struct] ;

   for( long i=0 ; i<S->n_Word ; i++ )
   {  if( S->Word[i].i_struct<0 ) continue ;
      e_Type t=Grammar[S->Word[i].i_struct].From.type ;
      if( IF_CONSTR(t) ) return 1 ;
   }
   return 0 ;
}
/***************************************************************************/
//              элемент дерева переводов
/***************************************************************************/
t_sTree * t_Slowo3 :: tree( long i_tree )
{
   return &Tree[i_tree] ;
}
/***************************************************************************/
//
/***************************************************************************/
t_sStruct * t_Slowo3 :: sstruct( long i_struct )
{
   return &Struct[i_struct] ;
}
/***************************************************************************/
//              дать переводимую структуру
/***************************************************************************/
t_sStruct * t_Slowo3 :: get_from( long i_slowo )
{
   if( i_slowo<0 || n_Record<=i_slowo ) return NULL ;
   return &Struct[Record[i_slowo].sy_struct] ;
}
/***************************************************************************/
//               перевод структуры
//               i_slowo   - индекс переводимой строки
//               i_variant - вариант перевода
/***************************************************************************/
t_sStruct * t_Slowo3 :: get_to( long i_slowo, long i_variant )
{
   if( i_slowo<0 || n_Record<=i_slowo ) return NULL ;
   if( Record[i_slowo].n_struct<i_variant+1 ) i_variant=0 ;
   return &Struct[Record[i_slowo].sy_struct+1+i_variant] ;
}
/***************************************************************************/
//               передача параметров
//               i_slowo   - индекс переводимой строки
//               i_variant - вариант перевода (-1 - оригинал)
/***************************************************************************/
t_RelationList1 t_Slowo3 :: get_relation( long i_slowo, long i_variant )
{
   t_RelationList1  RL ;
   t_sStruct       *S ;

   if( i_slowo<0 || n_Record<=i_slowo ) return RL ;
   if( i_variant==-1 )
            S=&Struct[Record[i_slowo].sy_struct] ;
       else S=&Struct[Record[i_slowo].sy_struct+1+i_variant] ;
   RL.list  =Relation+S->i_relation+1 ;
   RL.j=Relation[S->i_relation].s1 ;
   return RL ;
}
/***************************************************************************/
//             дать число возможных переводов по индексу структуры
//             типа "голова" и "жопа"
/***************************************************************************/
short t_Slowo3 :: get_n_perevod( long i_slowo )
{
   return Record[i_slowo].n_struct-1 ; // ABR
}
/***************************************************************************/
//             дать слово по индексу дерева
/***************************************************************************/
char * t_Slowo3 :: word_src( long i_tree )
{
   if( i_tree<0 || n_Tree<=i_tree )
   {   printf("\n Error t_Slowo3 :: word_src") ; throw(-1) ;  }
   return (char *)Mass+Struct[Record[Tree[i_tree].first].sy_struct].Word[Tree[i_tree].rang].sy ;
}
/***************************************************************************/
//             дать слово по индексу структуры
/***************************************************************************/
char * t_Slowo3 :: word_src1( long i_slowo )
{
   return (char *)Mass+Struct[Record[i_slowo].sy_struct].Word[0].sy ;
}
/***************************************************************************/
//             напечатать словарь
/***************************************************************************/
void t_Slowo3 :: print( char *File )
{
   FILE *fw=Fopen( File,"w" );
   for( long i=0 ; i<n_Record ; i++ )
      print_record( fw, i );
   Fclose( fw );
}
/***************************************************************************/
//             напечатать строку словаря
/***************************************************************************/
void t_Slowo3 :: print_record( FILE *fw, long i_record )
{
   t_sRecord *R=&Record[i_record] ;
   t_sStruct *S ;
   t_Struct  *SS ;
   long       i,j,is ;

   fprintf( fw,"\n" );
   S =&Struct[R->sy_struct];
   SS=&Grammar[S->i_struct].From ;
   
   // ------- язык оригинала ----------------------------
   if( SS->type!=TWORD )
   {  // ------ напечатать заголовок структуры ----------
      fprintf( fw,"%s",Format->get_tag(0,S->i_struct) );
      print_param( fw,0,S, 0 );
      fprintf( fw,":" );
   }
   for( i=0 ; i<S->n_Word ; i++ )
   {  
      is=S->Word[i].i_struct ;
      if( is<0 ) fprintf( fw,"@0" ) ;
          else   fprintf( fw,"%s",Format->get_tag(0,is) );
      if( S->Word[i].str[0]==0 ) fprintf( fw,"~") ;
          else                   fprintf( fw,"[%s]",S->Word[i].str ) ;
      if( 0<S->Word[i].order )
	  printf("<%d>",S->Word[i].order );
      print_param( fw,0,S, i+1 );
      if( i<S->n_Word-1 )
          fprintf( fw," ") ;
   }
   fprintf( fw,"=" );
   // ------- язык перевода ----------------------------
   for( j=1 ; j<R->n_struct ; j++ )
   {
      S=&Struct[R->sy_struct+j] ;
      if( 1<j ) fprintf( fw,";" );
      if( SS->type!=TWORD )
      {  // ------ напечатать заголовок структуры ----------
	 fprintf( fw,"%s",Format->get_tag(1,S->i_struct) );
	 print_param( fw,1,S, 0 );
	 fprintf( fw,":" );
      }
      for( i=0 ; i<S->n_Word ; i++ )
      {  
	 is=S->Word[i].i_struct ;
	 if( is<0 ) fprintf( fw,"@0" );
             else   fprintf( fw,"%s",Format->get_tag(1,is) );
	 if( S->Word[i].str[0]==0 ) fprintf( fw,"~" );
             else                   fprintf( fw,"[%s]",S->Word[i].str );
	 if( 0<S->Word[i].order )
	     printf("<%d>",S->Word[i].order );
         print_param( fw,1,S, i+1 );
	 if( i<S->n_Word-1 )
             fprintf( fw," ") ;
      }
   }  
}
/***************************************************************************/
//             напечатать параметры
//             i_struct0 - тип заголовка структуры
//             i_word    - номер слова в структуре(начинается с 1)
/***************************************************************************/
void t_Slowo3 :: print_param( FILE *fw, char to, t_sStruct *S, short i_word )
{
   short  i,i1,ip,n ;
   short  i_struct ;
   char   *s ;
   t_Form      Param ;
   t_Struct   *SS ;
   t_Relation *RR ;
   
   n =Relation[S->i_relation].s1 ;
   RR=&Relation[S->i_relation+1] ;
   if( i_word==0 )
   {  i_struct=S->i_struct ;
      Param   =S->Param.form() ;
   }
   else
   {  i_struct=S->Word[i_word-1].i_struct ;
      Param   =S->Word[i_word-1].Param.form() ;
   }
   
   if( to==0 ) SS=&Grammar[S->i_struct].From ;
       else    SS=&Grammar[S->i_struct].To ;
   
   for( i1=i=0 ; i<10 ; i++ )
      if( 0<=Param.value[i] ) i1++ ;
   for( i=0 ; i<n ; i++ )
      if( RR[i].s1==i_word || RR[i].s2==i_word ) i1++ ;
   if( i1==0 ) return ;
   
   fprintf( fw,"(" );
   for( i=0 ; i<10 ; i++ )
   {  if( 0<=Param.value[i] )
      {  // ------- постоянный параметр задан -------------
      	 if( to==0 )
	 {   if( Grammar[i_struct].From.Param.j<=i ) break ;
             ip=Grammar[i_struct].From.Param[i].param ;
	     s =Grammar.from().Param[ip].Value[Param.value[i]].Name;
	 }
	 else
	 {   if( Grammar[i_struct].To.Param.j<=i ) break ;
             ip=Grammar[i_struct].To.Param[i].param ;
	     s =Grammar.to().Param[ip].Value[Param.value[i]].Name;
	 }
	 for( i1=0 ; i1<Format->Rename.j ; i1++ )
	    if( 0==strcmp(Format->Rename[i1].Full,s) ) 
	    {   s=Format->Rename[i1].Reduce ; break ;  }

	 if( 0<i ) fprintf( fw,"," );
	 fprintf( fw,s );
      }
      else
      {  // ------- переменные параметры, нуждающиеся в обозначении
         for( i1=0 ; i1<n ; i1++ )
	 {  if( RR[i1].s1==i_word && RR[i1].p1==i )
	    {   if( i_word==0 ) s=SS->Param[i].Name ;
		    else        s=SS->Param[RR[i1].p2].Name ;
                goto M_continue ;
	    }
	    if( RR[i1].s2==i_word && RR[i1].p2==i )
	    {   if( i_word==0 ) s=SS->Param[i].Name ;
		    else        s=SS->Param[RR[i1].p1].Name ;
                goto M_continue ;
	    }
	 }	 
         break ;
	 M_continue :;
	 if( 0<i ) fprintf( fw,"," );
	 fprintf( fw,s );
      }
   }
   fprintf( fw,")" );
}
/***************************************************************************/
void t_sForm :: init( )
{  v0=0xf ; v1=0xf ; v2=0xf ; v3=0xf ; v4=0xf ; v5=0xf ; v6=0xf ; v7=0xf ;  }
/***************************************************************************/
t_sForm :: t_sForm()
{  v0=0xf ; v1=0xf ; v2=0xf ; v3=0xf ; v4=0xf ; v5=0xf ; v6=0xf ; v7=0xf ;  }
/***************************************************************************/
void t_sForm :: operator = ( t_Form &F )
{
   v0 = 0xf & F.value[0] ;
   v1 = 0xf & F.value[1] ;
   v2 = 0xf & F.value[2] ;
   v3 = 0xf & F.value[3] ;
   v4 = 0xf & F.value[4] ;
   v5 = 0xf & F.value[5] ;
   v6 = 0xf & F.value[6] ;
   v7 = 0xf & F.value[7] ;
}
/***************************************************************************/
t_Form t_sForm :: form( )
{
   t_Form F ;

   if( v0 == 0xf ) F.value[0]=-1 ; else F.value[0]=v0 ;
   if( v1 == 0xf ) F.value[1]=-1 ; else F.value[1]=v1 ;
   if( v2 == 0xf ) F.value[2]=-1 ; else F.value[2]=v2 ;
   if( v3 == 0xf ) F.value[3]=-1 ; else F.value[3]=v3 ;
   if( v4 == 0xf ) F.value[4]=-1 ; else F.value[4]=v4 ;
   if( v5 == 0xf ) F.value[5]=-1 ; else F.value[5]=v5 ;
   if( v6 == 0xf ) F.value[6]=-1 ; else F.value[6]=v6 ;
   if( v7 == 0xf ) F.value[7]=-1 ; else F.value[7]=v7 ;

   return F ;
}
