/***************************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                  Структуры, описывающие грамматику
//       Анисимов Д.В.                                     сегодня 
/***************************************************************************/
# include <stdio.h>
# include <malloc.h>
# include <string.h>
# include <stdlib.h>
# include <mylib.h>
# include <lexer.h>
# include <grammar.h>

# define Ifstr( a,b ) if( 0==strcmp(a,b) )

short find_param1( t_Param1List *List, char *Str );
short find_value2( t_strList    *List, char *Str );
short find_part(  t_StructList  *List, char *Str );

void  zerstoren_param( char *Str, char *Dir, char **Str1 );
void  zerstoren_word( char *Str, char *Imp, char **Str1 );
char  tst_const( t_Word *W, long i_str );

t_DirTrans *DirTrans ;        // список возможных направлений перевода
t_DirTrans *z_DirTrans=NULL ; // указатель на текущий язык перевода
t_Grammar   Grammar ;         // все данные, относящиеся к грамматике
FILE       *File_Error=NULL ; // Файл, в который пишутся ошибки lang.txt
char        Read_Error=0 ;    // Есть ли в lang.txt ошибки

/****************************************************************************/
//             прочесть все из файла и построить все индексы
//    File - имя файла lang.txt с путем
/****************************************************************************/
char t_Grammar :: make_all( char *File )
{
   short i ;

try
{
   del( );
   Read_Error=0 ;

   File_Error=Fopen("mercury.err","w");
   read( File );
   make_index( );

   for( short i1=0 ; i1<Format1.j ; i1++ )
      Format1[i1].make_part( this ) ;

   Fclose( File_Error );
   // ----------- определить какая часть речи - числительное ------
   i_Number   = 0 ;
   i_All      =-1 ;
   i_Surmise  =-1 ;
   i_Irregular=-1 ;
   i_Unknown  =-1 ;

   for( i=0 ; i<From.Part.j ; i++ )
      if( 0==strcmp( From.Part[i].Name,"числительное" ) )
      {  i_Number=i ; break ;  }
   for( i=0 ; i<Trans.j ; i++ )
   {  if( 0==strcmp( Grammar[i].From.Name,"все_все"     ) ) i_All      =i ;
      if( 0==strcmp( Grammar[i].From.Name,"@догадка"    ) ) i_Surmise  =i ;
      if( 0==strcmp( Grammar[i].From.Name,"@междометие" ) ) i_Irregular=i ;
      if( 0==strcmp( Grammar[i].From.Name,"@знаковые_части_речи" ) ) i_Unknown=i ;
   }

   return Read_Error ; // если тут 0, то все в порядке
}
catch( int E )
{
   if( File_Error!=NULL ) 
   {   fprintf( File_Error,"\n Файл %s не прочитан",File );
       error_add( "t_Grammar :: make_all( " );
       error_add( File );
       error_add( " )\n" );
   }
   Fclose( File_Error );
   Read_Error=-1 ;
   return -1 ;
}
}
/***************************************************************************/
void t_Grammar :: del( void )
{
   From.del() ;
   To.del() ;
   Format1.del() ;
   Format2.del() ;
   File1.del() ;
   File2.del() ;
   Trans.del() ;
}
/***************************************************************************/
//              прочесть из файла то, что можно прочесть                    
//    File - имя файла lang.txt с путем
/***************************************************************************/
void t_Grammar :: read( char *File )
{
   char    *Mass, f_version=0 ;
   long     i,L ;
   FILE    *fr ;

try
{  fr=Fopen( File,"r" );
   L=FileLength( fr );
   Mass=(char *)Calloc( L,1 );
   Fread( Mass,L,1,fr );
   Fclose(fr);

   Lexer.clr_breaker(  );
   Lexer.set_breaker( (uchar *)".,;:#$^=+-*(){}<>[]\"\\." );
   Lexer.init( Mass,L );
   Lexer.make_words();

   From.To=0 ;
   To.To=1 ;

   for( i=0 ; i<Lexer.n_word() ; i++ )
   {
      Ifstr( Lexer.Word[i],"@Версия")
      {   if( 0!=strcmp(Lexer.Word[i+1],"00_00_00") )
          {  fprintf( File_Error,"\n Не та версия lang.txt"); throw(-1);  }
	  f_version=1 ;
      }
      Ifstr( Lexer.Word[i],"@Атомы_источника" )
	 i=From.read( &Lexer,i );
      Ifstr( Lexer.Word[i],"@Атомы_приемника" )
	 i=To.read( &Lexer,i );
      Ifstr( Lexer.Word[i],"@Словари_перевода" )
	 i=read_slowo1( i );
      Ifstr( Lexer.Word[i],"@Словари_человека" )
	 i=read_slowo2( i );
      Ifstr( Lexer.Word[i],"@Переводы" )
	 i=read_trans( i );
   }
   Free( Mass );
   
   if( f_version==0 )
   {  fprintf( File_Error,"\n Отсутствует версия lang.txt"); throw(-1);  }
}
catch( int E )
{
   throw( E );
}
}
/***************************************************************************/
//                      прочесть словари переводов
// Begin - слово, с которого в lang.txt начинается описание словарей перевода
/***************************************************************************/
long t_Grammar :: read_slowo1( long Begin )
{  long i,i1,z,End,End1 ;
   t_Format1 Form,*Form1 ;
   t_File    File2 ;

   End=Lexer.find_paar( Begin,"{","}" );
   for( i=Begin ; i<End ; i++ )
   {  Ifstr( Lexer.Word[i],"@Формат_перевод" )
      {  Format1.add( Form );
	 Form1=Format1.list+Format1.j-1 ;
	 i=read_format1( i,Form1 );
      }
      char *s=Lexer.Word[i] ;
      if( 0==strcmp( s,"@Файлы_переводов" ) ||
          0==strcmp( s,"@Файлы_препроцессоров" ) ||
	  0==strcmp( s,"@Файлы_постпроцессоров" ) )
      {
	  File2.i_part=0 ;
	  z=find_format1( Lexer.Word[i+1] );
	  File2.i_format=z ;
	  File2.FileName.j=0 ;

	  End1=Lexer.find_paar( i+2,"{","}" );
	  for( i1=i+3 ; i1<End1 ; i1++ )
	  {  char *a ;
	     Ifstr( Lexer.Word[i1],"," ) continue ;
	     a=(char *)Calloc( strlen(Lexer.Word[i1])+1,1 ); // Memory Leak
	     strcpy( a, Lexer.Word[i1] );
	     File2.FileName.add( a );
	  }
	  Ifstr( s,"@Файлы_переводов" )
	     File1.add( File2 );
	  Ifstr( s,"@Файлы_препроцессоров" )
	     FilePre.add( File2 );
	  Ifstr( s,"@Файлы_постпроцессоров" )
	     FilePost.add( File2 );
	     
      }
   }
   return End ;
}
/**************************************************************************/
//           прочесть запись типа @Формат_перевода { ... }
//  Begin  - слово, с которого начинсется
//  Format - заполняемая структура
/**************************************************************************/
long t_Grammar :: read_format1( long Begin, t_Format1 *Format )
{  long i,i1,End,End1 ;
   t_PartDsk PartDsk ;

   Format->Name=Lexer.Word[Begin+1] ;

   Ifstr( Lexer.Word[Begin+2],"{" )
   {  End = Lexer.find_paar( Begin,"{","}" );
      if( End<0 ) {  fprintf( File_Error,"\n Ошибка непарности скобок"); throw(-1);  }
   }
   else {  fprintf( File_Error,"\n Ошибка формата"); throw(-1);  }

   for( i=Begin+3 ; i<End ; i++ )
   {
      Ifstr( Lexer.Word[i],"@Псевдоним" )
      {  // ------------- строка переименование ------------
	 t_Rename Ren ;
	 Ren.Reduce= Lexer.Word[i+1] ;
	 Ren.Full  = Lexer.Word[i+2] ;
	 Format->Rename.add( Ren );
	 i+=2 ;
	 continue ;
      }
      Ifstr( Lexer.Word[i],"@Части_речи_источника" )
      {  End1 = Lexer.find_paar( i,"{","}" );
	 for( i1=i+2 ; i1<End1 ; i1+=3 )
	 {  
	    PartDsk.Tag =Lexer.Word[i1] ;
	    PartDsk.Name=Lexer.Word[i1+1] ;
	    if( Lexer.Word[i1+2][0]!=';' )
	    {  fprintf( File_Error,"\n Ошибка формата"); throw(-1);  }
	    Format->SouPart.add( PartDsk );
	 }
	 i=End1 ;
      }
      Ifstr( Lexer.Word[i],"@Части_речи_приемника" )
      {  End1 = Lexer.find_paar( i,"{","}" );
	 for( i1=i+2 ; i1<End1 ; i1+=3 )
	 {  
	    PartDsk.Tag =Lexer.Word[i1] ;
	    PartDsk.Name=Lexer.Word[i1+1] ;
	    if( Lexer.Word[i1+2][0]!=';' )
	    {  fprintf( File_Error,"\n Ошибка формата"); throw(-1);  }
	    Format->DstPart.add( PartDsk );
	 }
      }
   }
   Format->find_struct( -1,"" ); // сбросить кеши поиска
   return End1 ;
}
/***************************************************************************/
//                      прочесть словари для человека
// Begin - слово, с которого в lang.txt начинается описание словарей человека
/***************************************************************************/
long t_Grammar :: read_slowo2( long Begin )
{  long i,i1,z,End,End1 ;
   t_Format2 Form,*Form2 ;
   t_File    File0 ;

   End=Lexer.find_paar( Begin,"{","}" );
   for( i=Begin ; i<End ; i++ )
   {  Ifstr( Lexer.Word[i],"@Формат_словаря" )
      {  Format2.add( Form );
	 Form2=&Format2[Format2.j-1] ;
	 i=read_format2( i,Form2 );
      }
      char *s=Lexer.Word[i] ;
      if( 0==strcmp( s,"@Файлы_словарей" ) )
      {
	  z=find_format2( Lexer.Word[i+1] );
	  File0.i_format=z ;
	  File0.FileName.j=0 ;

	  End1=Lexer.find_paar( i+2,"{","}" );
	  for( i1=i+3 ; i1<End1 ; i1++ )
	  {  char *a ;
	     Ifstr( Lexer.Word[i1],"," ) continue ;
	     a=(char *)Calloc( strlen(Lexer.Word[i1])+1,1 ); // Memory Leak
	     strcpy( a, Lexer.Word[i1] );
	     File0.FileName.add( a );
	  }
          File2.add( File0 );
      }
   }
   return End ;
}
/**************************************************************************/
//           прочесть запись типа @Формат_словаря { ... }
//  Begin  - слово, с которого начинсется
//  Format - заполняемая структура
/**************************************************************************/
long t_Grammar :: read_format2( long Begin, t_Format2 *Format )
{  long i,i1,End,End1 ;

   Format->Name=Lexer.Word[Begin+1] ;

   Ifstr( Lexer.Word[Begin+2],"{" )
   {  End = Lexer.find_paar( Begin,"{","}" );
      if( End<0 ) {  fprintf( File_Error,"\n Ошибка непарности скобок"); throw(-1);  }
   }
   else {  fprintf( File_Error,"\n Ошибка формата"); throw(-1);  }

   for( i=Begin+3 ; i<End ; i++ )
   {
      Ifstr( Lexer.Word[i],"@Разделитель" )
      {  char *Str=Lexer.Word[i+1] ;
         long L=strlen( Str );
	 //if( Str[0]!='\"' || Str[L-1]!='\"' )
         //{   fprintf( File_Error,"\n Разделитель не взят в кавычки"); throw(-1);  }
         strncpy( Format->Divider,Str,10 );
	 Format->Divider[min(9,L)]=0 ;	 
	 i+=2 ;
	 continue ;
      }
      Ifstr( Lexer.Word[i],"@Сортировка" )
      {  Format->fsort=Lexer.Word[i][0]-'0' ;
	 i+=2 ;
	 continue ;
      }
   }
   return End ;
}
/***************************************************************************/
//                      прочесть массив переводов
//   Begin  - слово, с которого начинсется
/***************************************************************************/
long t_Grammar :: read_trans( long Begin )
{  t_Struct S ;
   t_Trans  TT ;
   long i,i1,End ;

   End=Lexer.find_paar( Begin,"{","}" );
   for( i=Begin ; i<End ; i++ )
   {  Ifstr( Lexer.Word[i],"@Перевод" )
	 i=read_trans1( i );
   }
   
   S.type=TSTRUCT ;
   S.Name="@@все_все" ;
   TT.From=S ;
   TT.To  =S ;
   Trans.add(TT) ;
   
   copy_words();
   make_index_struct() ;
   for( i1=0 ; i1<Trans.j ; i1++ )
      Trans[i1].make_order( ) ;

   return i ;
}
/***************************************************************************/
//   скопировать описание частей речи в массив переводов t_Grammar->Trans
/***************************************************************************/
void t_Grammar :: copy_words( void )
{  short i,i1,i2,j ;
   char  *m1,*m2 ;
   t_Trans  T,*TT ;
   t_TransList Trans1 ;

   // ----------- найти "незадействованные" структуры --------
   m1=(char *)Calloc( From.Part.j,sizeof(char) );
   m2=(char *)Calloc( To.Part.j,sizeof(char) );
   for( i=0 ; i<Trans.j ; i++ )
   {  i1=find_part( &From.Part, Trans[i].From.Name );
      if( 0<=i1 )
      {   if( m1[i1]!=0 )
	  {   fprintf( File_Error,"\n Многократный перевод атома %ld",
		       Trans[i].From.i_str );
	       Read_Error=-1 ;
	  }
	  m1[i1]=1 ;
      }
      i2=find_part( &To.Part, Trans[i].To.Name );
      if( 0<=i2 )
      {   if( m2[i2]!=0 )
	  {   fprintf( File_Error,"\n Многократный перевод атома %ld",
		       Trans[i].From.i_str );
	       Read_Error=-1 ;
	  }
	  m2[i2]=1 ;
      }
   }
   // ----------- внести их в массив Trans1 -------------------
   for( i=0 ; i<From.Part.j ; i++ )
      if( m1[i]==0 )
      {  Trans1.add( T );
	 TT=Trans1.list+Trans1.j-1 ;
	 TT->From =From.Part[i] ;
	 //TT->From.type=TSTRUCT1 ;
      }
   for( i=j=0 ; i<To.Part.j ; i++ )
      if( m2[i]==0 )
      {  if( Trans1.j<=j )
	     Trans1.add( T );
	 TT=Trans1.list+j ;
	 TT->To=To.Part[i] ;
	 //TT->To.type=TSTRUCT1 ;
	 j++ ;
      }

   for( i=0 ; i<Trans.j ; i++ )
   {  i1=find_part( &From.Part, Trans[i].From.Name );
      i2=find_part( &To.Part  , Trans[i].To.Name );
      if( 0<=i1 && 0<=i2 )
      {  // ----- внести в массив Trans1 задействованные слова ------
	 Trans1.add( T );
	 TT=Trans1.list+Trans1.j-1 ;
	 TT->From=From.Part[i1] ;
	 TT->To  =To.Part[i2] ;
      }
      if( i1<0 && i2<0 )
	 // ------- внести в массив Trans1 все остальное ---
	 Trans1.add( Trans[i] );
   }
   Trans.del();
   Trans=Trans1 ;
   Trans1.del();
   Free(m1); Free(m2);
}
/***************************************************************************/
//                чтение одной записи типа @Перевод
//   Begin  - слово, с которого начинсется
/***************************************************************************/
long t_Grammar :: read_trans1( long Begin )
{  t_Trans TT,*T ;
   long  i,end ;
   char *pStr ;
   char  f ;

   Trans.add(TT) ;
   T=Trans.list+Trans.j-1 ;

   end=Lexer.find_paar( Begin+1,"{","}" );
   for( i=Begin+1,f=0 ; i<end ; i++ )
   {  pStr=Lexer.Word[i] ;
      if( 0==strcmp(pStr,"@Выбор") || 
          0==strcmp(pStr,"@Выбор1") ||
          0==strcmp(pStr,"@Выбор2") ||
          0==strcmp(pStr,"@Структура") ||
	  0==strcmp(pStr,"@Структура1") ||
	  0==strcmp(pStr,"@Структура2") ||
	  0==strcmp(pStr,"@Множество") ||
	  0==strcmp(pStr,"@Беспорядок") ||
	  0==strcmp(pStr,"@Беспорядок1") ||
	  0==strcmp(pStr,"@Беспорядок2") )
      {  if( f==0 ) {  i=T->From.read( i,&From ); f++ ;  }
	 if( f==1 ) {  i=T->To.read  ( i,&To );   f++ ;  }
      }
      if( 0==strcmp( Lexer.Word[i],"@Таблица" ) ||
	  0==strcmp( Lexer.Word[i],"@Таблица_источника" ) ||
	  0==strcmp( Lexer.Word[i],"@Таблица_приемника" ) ||
	  0==strcmp( Lexer.Word[i],"@Таблица_передачи" ) )      
 	  i=read_table( i+1,T );
   }
   T->make_order( );
   if( T->From.type!=T->To.type )
   {   fprintf( File_Error,"\n Выбор и структура в одном переводе %ld",
       	       Lexer.Str[Begin] );   
       Read_Error=-1 ;
   }

   return end ;
}
/***************************************************************************/
//                чтение одной записи типа @Таблица
//   Begin  - слово, с которого начинсется
//   TT     - заполняемая пара
/***************************************************************************/
long t_Grammar :: read_table( long Begin, t_Trans *TT )
{  long     i,j,i_param,End ;
   short    z ;
   t_2Index I ;
   char     f,f1,*pStr ;
   t_Table *T,T1 ;

   TT->Table.add( T1 );
   T=TT->Table.list+TT->Table.j-1 ;
   Ifstr( Lexer.Word[Begin-1],"@Таблица_источника" ) T->type=T_SOU ;
   Ifstr( Lexer.Word[Begin-1],"@Таблица_приемника" ) T->type=T_DST ;
   Ifstr( Lexer.Word[Begin-1],"@Таблица_передачи"  ) T->type=T_TRANS ;
   Ifstr( Lexer.Word[Begin-1],"@Таблица" )           T->type=T_TRANS ;

   End=Lexer.find_paar( Begin,"(",")" );
   // ------------------- трансляция заголовка таблицы ----------------
   for( f=f1=0,i=Begin+1 ; i<End ; i++ )
   {   pStr=Lexer.Word[i] ;
       Ifstr( pStr,"," ) {  f1=0 ; continue ;  }
       Ifstr( pStr,"=" ) {  f=1 ; f1=0 ; continue ;  }
       if( f1==0 )
       {  f1++ ;
	  if( 0==strcmp("@Выбор",pStr) ) {  i_param=-1 ; goto M_Found ;  }
	  if( T_SOU==T->type || (T_TRANS==T->type && 0==f) )
	      i_param=find_param( &From.Param,pStr );
          if( T_DST==T->type || (T_TRANS==T->type && 1==f) )
	      i_param=find_param( &To.Param  ,pStr );
       }
       else
       {
M_Found:  f1++ ;
	  if( T_SOU==T->type || (T_TRANS==T->type && 0==f) )
	      I=TT->From.find_param2(pStr) ; // --- найти параметр с таким именем в структуре источника
	  if( T_DST==T->type || (T_TRANS==T->type && 1==f) )
	      I=TT->To.find_param2(pStr) ; // --- найти параметр с таким именем в структуре приемника
	  I.i_param=i_param ;
	  I.Name   =pStr ;
	  if( 0==f ) T->In.add( I );
	      else   T->Out.add( I );
	  
          if( I.i1<0 ) Lexer.error( i );
       }
   }
   T->Size=T->In.j+T->Out.j ;
   if( T->In.j==0 || T->Out.j==0 )
   {  fprintf( File_Error,"\n Заголовок таблицы - плохой строка %ld",
	       Lexer.Str[i] );
      Read_Error=-1 ;
      return End ;
   }
   // ------------------- трансляция тела таблицы ----------------
   t_Param *P ;

   Begin=End+1 ;
   End=Lexer.find_paar( Begin,"{","}" );
   j=0 ;
   for( i=Begin+1 ; i<End ; i++ )
   {  pStr=Lexer.Word[i] ;
      Ifstr( pStr,";" ) {  j=0 ; continue ;  }
      Ifstr( pStr,"=" ) {  continue ;  }
      if( j<T->In.j )
           i_param=T->In[j].i_param ;          // значения до знака равно 
      else i_param=T->Out[j-T->In.j].i_param ; // значения после знака равно

      if( i_param<0 ) goto M_Select ;

      if( T_SOU==T->type || (T_TRANS==T->type && j<T->In.j) )
	 P=&From.Param[i_param] ;
      if( T_DST==T->type || (T_TRANS==T->type && T->In.j<=j) )
	 P=&To.Param[i_param] ;

      z=find_value( &P->Value,pStr );
      T->Value.add( z );
      j++ ;
      continue ;
      // ----------- обработка параметра @Выбор --------
      M_Select :
      if( 0==strcmp( pStr,"@0" ) )
               z=-1 ;
          else z=atoi(pStr) ;
      T->Value.add( z );
      j++ ;
   }
   return End ;
}
/***************************************************************************/
//        приведение в порядок ссылок на структуры по их именам
/***************************************************************************/
void t_Grammar :: make_index_struct( void )
{  t_Trans *T ;
   t_Word  *W ;
   long     i,i1,z ;
   char     F_Error=0 ;

   // ------ цикл по трансляционным парам -------------------
   for( i=0 ; i<Trans.j ; i++ )
   {  T=Trans.list+i ;
      make_index_param0( i );
      // ------ обработка структуры источника ---------------
      for( i1=0 ; i1<T->From.Word.j ; i1++ )
      {  W=T->From.Word.list+i1 ;

         if( tst_const( W,T->From.i_str ) ) continue ;
	 z=find_struct( FROM, W->Name );
	 if( z<0 )
	 {  fprintf( File_Error,"\n Не могу найти структуру \n Строка %ld %s",
		  T->From.i_str,W->Name );
	    F_Error=1 ; continue ;
	 }
	 W->i_struct=z ;
	 W->type    =Trans[z].From.type ;
	 make_index_param( FROM,i,i1 );
      }
      // ------ обработка структуры приемника ---------------
      for( i1=0 ; i1<T->To.Word.j ; i1++ )
      {  W=T->To.Word.list+i1 ;
      
         if( tst_const( W,T->From.i_str ) ) continue ;
	 z=find_struct( TO, W->Name );
	 if( z<0 )
	 {  fprintf( File_Error,"\n Не могу найти структуру \n Строка %ld %s",
		     T->To.i_str,W->Name );
	    F_Error=1 ; continue ;
	 }
	 W->i_struct=z ;
	 W->type    =Trans[z].To.type ;
	 make_index_param( TO,i,i1 );
      }
   }

   if( F_Error==1 )
       throw(-1) ;
}
/***************************************************************************/
//   приведение в порядок ссылок на параметры слов по именам параметров
//   i_trans - индекс трансляционной пары
/***************************************************************************/
void t_Grammar :: make_index_param0( long i_trans )
{  short i,i1 ;
   t_Param1List *P ;

   P=&Trans[i_trans].From.Param ;
   for( i=0 ; i<P->j ; i++ )
   {  i1=P->list[i].param ;
      P->list[i].value=find_value( &From.Param[i1].Value,P->list[i].Name );
   }
   P=&Trans[i_trans].To.Param ;
   for( i=0 ; i<P->j ; i++ )
   {  i1=P->list[i].param ;
      P->list[i].value=find_value( &To.Param[i1].Value,P->list[i].Name );
   }

}
/***************************************************************************/
//   приведение в порядок ссылок на параметры слов по именам параметров
//   Half - источник или приемник
//   i_trans - индекс трансляционной пары (он же индекс структуры)
//   i_word  - индекс подструктуры
/***************************************************************************/
void t_Grammar :: make_index_param( e_Half Half, long i_trans, long i_word )
{
   long      i,i1,i2,z,i_struct ;
   char      f ;
   t_Param1List  *p;
   t_Struct      *S;
   t_Word        *W ;
   t_Lang        *Lang ;
   t_Form        *F ;
   t_Format      *FF ;

   if( Half==FROM ) S=&Trans[i_trans].From ;
       else         S=&Trans[i_trans].To ;
   // --------- проверка на соответствие числа параметров -----------
   if( Half==FROM )
   {  W = &Trans[i_trans].From.Word[i_word] ;
      i_struct=W->i_struct ;
      p = &Trans[i_struct].From.Param ;
   }
   else
   {  W = &Trans[i_trans].To.Word[i_word] ;
      i_struct=W->i_struct ;
      p = &Trans[i_struct].To.Param ;
   }
   if( W->Param.j!=p->j )
   {  // --------------- Если не соответствует - печать об ошибке
      fprintf( File_Error,"\n Не соответствие числа параметров \n Строка %ld %s->%s",
		    S->i_str,S->Name, W->Name );
      Read_Error=-1 ;
   }
   else
   {  // -------- а если все в порядке - проставить номера параметров
      for( i=0 ; i<W->Param.j ; i++ )
      {  W->Param[i].param=i1=p->list[i].param ;
         if( W->Param[i].Name[0]=='%' )
	 {   if( Half==FROM ) z=find_value( &From.Param[i1].Value,W->Param[i].Name+1 );
	         else         z=find_value( &  To.Param[i1].Value,W->Param[i].Name+1 );
	     if( z<0 )
	     {
		 fprintf( File_Error,"\n Неизвесное значение параметра %s \n Строка %ld %s->%s",
			       W->Param[i].Name,S->i_str,S->Name, W->Name );
		 Read_Error=-1 ;
	     }
	     W->Param[i].value=z ;
         }
	 else
	     W->Param[i].value=-1 ;
      }
      // ----------- проверка на допустимые комбинации параметров ----
      if( W->Param.j==0 || W->type!=TWORD ) return ;
      if( Half==FROM ) Lang=&From ; else Lang=&To ;
      // ----------- проверка, что такую проверку надо проводить ----
      for( i=0,f=0 ; i<Lang->Part[W->i_struct].Param.j ; i++ )
         if( Lang->Part[W->i_struct].Param[i].Dir==0 )
	     f=1 ;
      if( f==0 ) return ;

      for( i=0 ; i<Lang->File.j ; i++ )
      {  if( Lang->File[i].i_part!=W->i_struct ) continue ;
	 FF=&Lang->Format[ Lang->File[i].i_format ] ;
	 for( i1=0 ; i1<FF->Record.j ; i1++ )
	 {
	    F= &FF->Record[i1] ;
	    for( i2=0 ; i2<W->Param.j ; i2++ )
	    {  if( 0<=W->Param[i2].value && 0<=F->value[i2] &&
		      W->Param[i2].value!=F->value[i2] ) goto M_No;
	    }
	    return ; // нужная комбинация найдена
	    M_No:;
	 }
      }
      fprintf( File_Error,"\n невозможная комбинация параметров \n Строка %ld %s->%s",
		    S->i_str,S->Name, W->Name );
      Read_Error=-1 ;
   }
}
/***************************************************************************/
//                установка индексов слов, структур, и т.д.
/***************************************************************************/
void t_Grammar :: make_index( void )
{  short    i,i1 ;
   t_Struct *S,*S1 ;

   for( i=0 ; i<Trans.j ; i++ )
   {  // ----- проверка на присутствие конструкций с одинаковыми именами ----
      for( i1=0 ; i1<i ; i1++ )
      {  S =&Trans[i].From ; S1=&Trans[i1].From ;
         if( S->type==TNULL || S->type==TNULL ) continue ;
         if( 0==strcmp( S->Name,S1->Name ) )
         {   fprintf( File_Error,"\n Конструкция %s описана два раза\n Строки %ld и %ld",
		    S->Name, S->i_str,S1->i_str );
             Read_Error=-1 ;
         }
         S =&Trans[i].To ; S1=&Trans[i1].To ;
         if( 0==strcmp( S->Name,S1->Name ) )
         {   fprintf( File_Error,"\n Конструкция %s описана два раза\n Строки %ld и %ld",
		    S->Name, S->i_str,S1->i_str );
             Read_Error=-1 ;
         }
      }
      // ------ собственно расстановка индексов -------------------
      S=&Trans[i].From ;
      for( i1=0 ; i1<S->Word.j ; i1++ )
	 index_word( &S->Word[i1], FROM );
      S=&Trans[i].To ;
      for( i1=0 ; i1<S->Word.j ; i1++ )
	 index_word( &S->Word[i1], TO );
      Trans[i].make_index_table( );
      Trans[i].From.trans_relation( );
      Trans[i].To.trans_relation( );
      Trans[i].make_order();
      Trans[i].From.tst_relation( );
      Trans[i].To.tst_relation( );
   }
}
/***************************************************************************/
//                 проставить индексы структур словам
/***************************************************************************/
void t_Grammar :: index_word( t_Word *W, e_Half Half )
{
   if( W->type!=TWORD && W->type!=TCONST && W->type!=TCONST1 && W->type!=TWORD0 )
   {   W->i_struct = find_struct( Half, W->Name );
       W->type     = Trans[W->i_struct].From.type ;
   }
}
/**************************************************************************/
//                 узнать номер структуры по ее имени
/**************************************************************************/
short t_Grammar :: find_struct( e_Half Half, char *Name )
{
   if( Half==FROM )
   {   for( short i=0 ; i<Trans.j ; i++ )
          if(  0==strcmp( Trans.list[i].From.Name,Name) ) return i ;
   }
   else
   {   for( short i=0 ; i<Trans.j ; i++ )
          if(  0==strcmp( Trans.list[i].To.Name,Name) ) return i ;
   }
   return -1 ;
}
/**************************************************************************/
//                 узнать номер формата перевода по его имени
/**************************************************************************/
short t_Grammar :: find_format1( char *Name )
{
   for( short i=0 ; i<Format1.j ; i++ )
      if( 0==strcmp( Format1[i].Name,Name ) ) return i ;
   return -1 ;
}
/**************************************************************************/
short t_Grammar :: find_format2( char *Name )
{
   for( short i=0 ; i<Format2.j ; i++ )
      if( 0==strcmp( Format2[i].Name,Name ) ) return i ;
   return -1 ;
}
/**************************************************************************/
t_Trans& t_Grammar :: operator []( long i )
{  if( i<0 || Trans.j<=i )
   {  printf("\n Error index grammar");
      error_set("Error index grammar!!!");
      throw(-1);  
   }
   return Trans.list[i];  
}
/**************************************************************************/
short  t_Grammar :: n_trans( void )
{  return Trans.j ;  }
/**************************************************************************/
short  t_Grammar :: main_struct( void )
{  return Trans.j-1 ;  }
/**************************************************************************/
short  t_Grammar :: i_number( void )
{  return i_Number ;  }
/**************************************************************************/
short  t_Grammar :: i_all( )
{  return  i_All ;  }
/**************************************************************************/
short  t_Grammar :: i_surmise( )
{  return  i_Surmise ;  }
/**************************************************************************/
short  t_Grammar :: i_irregular( )
{  return  i_Irregular ;  }
/**************************************************************************/
short  t_Grammar :: i_unknown( )
{  return  i_Unknown ;  }
/**************************************************************************/
t_Lang& t_Grammar :: from( void )
{  return From ;  }
/**************************************************************************/
t_Lang& t_Grammar :: to( void )
{  return To ;  }
/**************************************************************************/
t_Format1List& t_Grammar :: format1( void )
{  return Format1 ;  }
/**************************************************************************/
t_FileList& t_Grammar :: file1( void )
{  return File1 ;  }
/**************************************************************************/
t_FileList& t_Grammar :: file_pre( void )
{  return FilePre ;  }
/**************************************************************************/
t_FileList& t_Grammar :: file_post( void )
{  return FilePost ;  }
/***************************************************************************/
//            создание таблицы передачи параметров для t_Trans
/***************************************************************************/
void t_Trans :: make_index_table( void )
{  short    i,i1 ;
   t_Table *T ;
   e_Table  t ;
   t_2Index I ;

   for( i=0 ; i<Table.j ; i++ )
   {  T=&Table[i] ;
      t=T->type ;
      for( i1=0 ; i1<T->In.j ; i1++ )
      {  if( T_SOU==t || T_TRANS==t ) I=From.find_param2(T->In[i1].Name) ;
         if( T_DST==t )               I=  To.find_param2(T->In[i1].Name) ;
	 T->In[i1].i1=I.i1 ;
	 T->In[i1].i2=I.i2 ;
      }
      for( i1=0 ; i1<T->Out.j ; i1++ )
      {  if( T_SOU==t )               I=From.find_param2(T->Out[i1].Name) ;
	 if( T_DST==t || T_TRANS==t ) I=  To.find_param2(T->Out[i1].Name) ;
	 T->Out[i1].i1=I.i1 ;
	 T->Out[i1].i2=I.i2 ;
      }
   }
}
/***************************************************************************/
//     задание соответствия между составляющими источника и приемника
/***************************************************************************/
void t_Trans :: make_order( void )
{
   short     is1,is2,n_relation ;
   long      i,i1 ;

   for( i=0 ; i<To.Word.j ; i++ )
      To.Word[i].use=0 ;

   is1=-1 ;
   Relation1.j=Relation2.j=0 ;
   for( i=0 ; i<From.Word.j ; i++ )
	Relation1.add( is1 );
   for( i=0 ; i<To.Word.j ; i++ )
	Relation2.add( is1 );

   // --------------- формирование Relation1 -----------------------
   for( i=0 ; i<From.Word.j ; i++ )
   {
      if( From.Word[i].order<0 )
      {   // -------------- неявное задание порядка --------------
	  is1=From.Word[i].i_struct ;
	  n_relation=0 ;
	  for( i1=0 ; i1<To.Word.j ; i1++ )
	  {  if( 0<=To.Word[i1].order || To.Word[i1].use!=0 ) continue ;
	     is2=To.Word[i1].i_struct ;
	     if( is1==is2 )
	     {   Relation1[i] =i1 ;
		 Relation2[i1]=i ;
		 To.Word[i1].use=1 ;
		 n_relation++ ;
		 break ;
	     }
	  }
	  if( To.Word.j<n_relation )
	  {  fprintf( File_Error,"\n ошибка соответствия структур %s %s",
		     From.Name,To.Name );
	     Read_Error=-1 ;
	     throw(-1);
	  }
      }
      else
      {   // -------------- явное задание порядка --------------
	  for( i1=0 ; i1<To.Word.j ; i1++ )
	  {  if( To.Word[i1].order<0 ) continue ;	     
	     if( From.Word[i].order==To.Word[i1].order )
	     {   if( From.Word[i].i_struct!=To.Word[i1].i_struct )
		 {  fprintf( File_Error,"\n %s ошибка соответствия структур2",From.Name );
		    Read_Error=-1 ;
		    throw(-1);
		 }
		 Relation1[i]=i1;
		 Relation2[i1]=i;
	     }
	  }
      }
   }
   // ----- если это выбор, проверить его варианты на соответствие -----
   if( From.type==TSELECT || From.type==TSELECT2 )
   {
      if( From.Word.j!=To.Word.j )
      {  fprintf( File_Error,"\n %s : несоответствие числа вариантов выбора",From.Name );
	 Read_Error=-1 ;
      }      
      for( i=0 ; i<From.Word.j ; i++ )
         if( From.Word[i].i_struct!=To.Word[i].i_struct &&
	     0<=From.Word[i].i_struct && 0<=To.Word[i].i_struct )
	 {  fprintf( File_Error,"\n %s->%s : несоответствие вариантов выбора",
	             From.Name,From.Word[i].Name );
	    Read_Error=-1 ;
	 }
   }
}
/***************************************************************************/
//              прочесть запись типа @Структура @Выбор
//    Begin  - слово, с которого начинсется
//    Lang   - язык, к которому относится эта структура
/***************************************************************************/
long t_Struct :: read( long Begin, t_Lang *Lang )
{
   t_Word   W ;
   char     *pStr ;
   long     i,i1,end,End ;

   End=Grammar.Lexer.n_Word ;

   for( i=Begin ; i<End ; i++ )
   {
      pStr=Grammar.Lexer.Word[i] ;
      if( 0==strcmp(pStr,"@Часть_речи") ||
          0==strcmp(pStr,"@Структура") ||
	  0==strcmp(pStr,"@Структура1") ||
	  0==strcmp(pStr,"@Структура2") ||
	  0==strcmp(pStr,"@Выбор") ||
	  0==strcmp(pStr,"@Выбор1") ||
	  0==strcmp(pStr,"@Выбор2") ||
	  0==strcmp(pStr,"@Множество") ||
	  0==strcmp(pStr,"@Беспорядок") ||
	  0==strcmp(pStr,"@Беспорядок1") ||
	  0==strcmp(pStr,"@Беспорядок2") )
      {
	 Name=Grammar.Lexer.word(i+1);
	 
	 Ifstr( pStr,"@Часть_речи" ) type=TWORD ; 
	 Ifstr( pStr,"@Структура"  ) type=TSTRUCT ;
	 Ifstr( pStr,"@Структура1" ) type=TSTRUCT1 ;
	 Ifstr( pStr,"@Структура2" ) type=TSTRUCT2 ;
	 Ifstr( pStr,"@Выбор"      ) type=TSELECT ;
	 Ifstr( pStr,"@Выбор1"     ) type=TSELECT1 ;
	 Ifstr( pStr,"@Выбор2"     ) type=TSELECT2 ;
	 Ifstr( pStr,"@Множество"  ) type=TENUM ;
	 Ifstr( pStr,"@Беспорядок" ) type=TUNORDER ;
	 Ifstr( pStr,"@Беспорядок1") type=TUNORDER1 ;
	 Ifstr( pStr,"@Беспорядок2") type=TUNORDER2 ;

	 i_str=Grammar.Lexer.Str[i+1] ;

	 // -------------- трансляция параметров ----------------- //
	 i=trans_param( i, Lang );

	 if( type != TWORD && 0!=strcmp( Grammar.Lexer.Word[i],"=" ) )
	 {  fprintf( File_Error,"\n В строке %s нет знака равно",Name );
	    throw(-1);
	 }
	 // ----------- трансляция составляющих ----------------
	 end=Grammar.Lexer.find_word( i,";" );
	 for( i1=i+1 ; i1<end ; i1++ )
	    i=i1=trans_word1( i1 );

	 return i+1 ;
      }
   }
   return i ; //End ;
}
/***************************************************************************/
//        трансляция составляющих структуры или варианта выбора
//    Begin  - слово, с которого начинсется
/***************************************************************************/
long t_Struct :: trans_word1( long Begin )
{  long    i ;
   t_Word  W ;
   char    use,*pStr ;

   for( i=Begin ; i<Grammar.Lexer.n_word() ; i++ )
   {
      Ifstr( Grammar.Lexer.Word[i],";" ) {  return i+1 ;  }

      zerstoren_word( Grammar.Lexer.Word[i], &use, &pStr );
      W.Name=pStr ;
      W.meaning_use=use ;
      i=trans_order( i,&W );
      Word.add( W );
      return trans_word( i );
   }
   return Grammar.Lexer.n_word();
}
/***************************************************************************/
//   Трансляция вот таких фенечек   @Структура = часть<1> часть<2>
//   Begin  - слово, с которого начинсется            ^^^      ^^^
//   Word1  - часть структуры, которой проставляется order
/***************************************************************************/
long t_Struct :: trans_order( long Begin, t_Word *Word1 )
{  long i=Begin ;

   Ifstr( Grammar.Lexer.Word[i+1],"<" )
   {  Ifstr( Grammar.Lexer.Word[i+3],">" )
	   Word1->order=atoi( Grammar.Lexer.Word[i+2] );
      else Grammar.Lexer.error(i);
      i+=3 ;
   }
   Ifstr( Grammar.Lexer.Word[i+1],"[" )
   {  Ifstr( Grammar.Lexer.Word[i+3],"]" )
	 Word1->literal=Grammar.Lexer.Word[i+2] ;
      else
      {  fprintf( File_Error,"\n В строке %ld пустая константа",Grammar.Lexer.Str[i+2] );
         Read_Error=-1 ;
      }
      i+=3 ;
   }
   return i ;
}
/***************************************************************************/
//                     трансляция параметров структуры
//    Begin  - слово, с которого начинсется
//    Lang   - язык, к которому относится эта структура
/***************************************************************************/
long t_Struct :: trans_param( long Begin, t_Lang *Lang )
{  t_Param1 Param1 ;
   long  i,z,End ;
   char *pStr ;

   Ifstr( Grammar.Lexer.word(Begin+2),"(" )
   {  // -------------- если у структуры есть параметры -----------
      End=Grammar.Lexer.find_paar( Begin+2,"(",")" );
      for( i=Begin+3 ; i<End ; i++ )
      {  char d ;

	 Ifstr( Grammar.Lexer.Word[i],"," ) continue ;
	 z=find_param( &Lang->Param, Grammar.Lexer.Word[i] );
	 if( z<0 ) // continue ;
	 {  fprintf( File_Error,"\n В структуре (%s) неизвестный тип параметра(%s)",
	             Name, Grammar.Lexer.Word[i] );
            Read_Error=-1 ;
	 }
	 Param1.param=z ;	 
	 zerstoren_param( Grammar.Lexer.Word[i+1], &d, &pStr );
	 Param1.Name=pStr ;
	 Param1.Dir=d ;
	 Param.add( Param1 ); i++ ;
      }
      return End+1 ;
   }
   else
   {  // --------------- и если параметров нет -----------------
      return Begin+2 ;
   }
}
/***************************************************************************/
//     трансляция параметров части структуры
//     Begin  - слово, с которого начинсется
/***************************************************************************/
long t_Struct :: trans_word( long Begin )
{
   t_Param1  Param1 ;
   t_Word   *W=&Word[Word.j-1] ;
   long      i,End ;
   char      Dir,*Str ;

   Ifstr( Grammar.Lexer.Word[Begin+1],"(" )
	   End=Grammar.Lexer.find_paar( Begin+1,"(",")" );
      else End=Begin ;

   for( i=Begin+2 ; i<End ; i++ )
   {
      Ifstr( Grammar.Lexer.Word[i],"," ) continue ;
      zerstoren_param( Grammar.Lexer.Word[i],&Dir,&Str );

      Param1.param=-1 ;
      Param1.Dir  =Dir ;
      Param1.Name =Str ;
      Param1.value=-1 ;

      W->Param.add( Param1 );
   }
   return End ;
}
/**************************************************************************/
//     сформировать описание передачи параметров внутри конструкции
//     одного языка (Relation)
/**************************************************************************/
void t_Struct :: trans_relation( void )
{  t_Param1   *Param1 ;
   t_Relation *Index,I ;
   short      *gruppe ;
   long        i,i1,j,N ;

   if( type==TWORD ) return ;
   N=Param.j ;
   for( i=0 ; i<Word.j ; i++ )
      N+=Word[i].Param.j ;

   Param1=(t_Param1   *)Calloc( N,sizeof(t_Param1  ) );
   Index =(t_Relation *)Calloc( N,sizeof(t_Relation) );
   gruppe=(short      *)Calloc( N,sizeof(short) );

   // ----------- инициация массивов структуры -------------
   for( i=j=0 ; i<Param.j ; i++ )
   {  if( 0<=Param[i].value ) continue ;
      Param1[j]=Param[i] ;
      Index[j].s1=Index[i].s2=0 ;
      Index[j].p1=Index[i].p2=i ;
      gruppe[j]=j ;
      j++ ;
   }
   // ----------- инициация массивов составляющих -------------
   for( i=0 ; i<Word.j ; i++ )
   {  for( i1=0 ; i1<Word[i].Param.j ; i1++ )
      {  if( 0<=Word[i].Param[i1].value ) continue ;
	 Param1[j]=Word[i].Param[i1] ;
	 Index[j].s1=Index[j].s2=i+1 ;
	 Index[j].p1=Index[j].p2=i1 ;
	 gruppe[j]=j ;
	 j++ ;
      }
   }
   N=j ;
   // ----------- заполнение массивов Relation ----------------
   for( i=0 ; i<N ; i++ )
   {
      for( i1=i ; i1<N ; i1++ )
	 if( 0==strcmp(Param1[i].Name,Param1[i1].Name) )
	 {   gruppe[i1]=i ;
	     if( Param1[i].Dir==1 && Param1[i1].Dir==0 )
	     {   I.s1=Index[i].s1 ; I.s2=Index[i1].s1 ;
		 I.p1=Index[i].p1 ; I.p2=Index[i1].p1 ;
		 Relation.add( I );
	     }
	     if( Param1[i].Dir==0 && Param1[i1].Dir==1 )
	     {   I.s1=Index[i1].s1 ; I.s2=Index[i].s1 ;
		 I.p1=Index[i1].p1 ; I.p2=Index[i].p1 ;
		 Relation.add( I );
	     }
	 }
   }
   Free( Param1 ); Free( Index ); Free( gruppe );

}
/**************************************************************************/
//     найти в структуре положение ( индекс слова и индекс параметра ) 
//     параметра источника с именем Str
/**************************************************************************/
t_2Index t_Struct :: find_param2( char *Str )
{  t_2Index      r ;
   t_Param1List *P ;
   short         i,i1 ;

   if( 0==strcmp("@Выбор",Str) ) {  r.i1=0 ; r.i2=-1 ; return r ; }
   P=&Param ;
   // ------- поиск в заголовке структуры -------------
   for( i=0 ; i<P->j ; i++ )
   {  if( P->list[i].Dir==0 && type==TSTRUCT ) continue ;
      if( 0==strcmp(P->list[i].Name,Str) )
      {  r.i1=0 ; r.i2=i ; return r ;  }
   }
   // ------- поиск в составляющих структуры ----------
   for( i1=0 ; i1<Word.j ; i1++ )
   {  P=&Word[i1].Param ;
      for( i=0 ; i<P->j ; i++ )
      {  if( P->list[i].Dir==0 ) continue ;
	 if( 0==strcmp(P->list[i].Name,Str) )
	 {  r.i1=i1+1 ; r.i2=i ; return r ;  }
      }
   }
   fprintf( File_Error,"\n %ld В структуре не могу найти параметр \"%s\"",i_str,Str );
   Read_Error=-1 ;
   r.i1=-1 ; r.i2=-1 ; return r ;
}
/**************************************************************************/
//     проверка параметров на соответствие типов при прямой передаче
/**************************************************************************/
void t_Struct :: tst_relation( void )
{  long  i ;
   t_Relation   I ;
   t_Param1List *P1,*P2 ;

   for( i=0 ; i<Relation.j ; i++ )
   {  I=Relation[i] ;
      if( I.s1==0 ) P1=&Param ; 
          else      P1=&Word[I.s1-1].Param ;
      if( I.s2==0 ) P2=&Param ;
          else      P2=&Word[I.s2-1].Param ;
      if( P1->list[I.p1].param!=P2->list[I.p2].param )
      {   fprintf( File_Error,"\n Прямая передача значений между разнородными параметрами");  
          fprintf( File_Error,"\n Строка = %ld  структура = %s  параметр = %s", 
	                      i_str, Name, P1->list[I.p1].Name ) ;
          Read_Error=-1 ;
      }
   }
}
/***************************************************************************/
// прочесть запись типа @Атомы_источника{ ... } или @Атомы_приемника{ ... }
//     Lexer - Лексер, который содержит lang.txt
//     Begin - слово, с которого начинается
/***************************************************************************/
long t_Lang :: read( t_Lexer *_Lexer, long Begin )
{
   long     i,i1,End,End1 ;
   t_Value  Value1 ;
   t_Param  Param1 ;
   t_Struct Part1 ;
   t_Format Form,*Form1 ;
   t_File   File1 ;
   short    z ;

try
{
   Lexer=_Lexer ;
   Ifstr( Lexer->Word[Begin+1],"{" )
   {  End = Lexer->find_paar( Begin,"{","}" );
      if( End<0 ){  fprintf(File_Error,"\n Ошибка непарности скобок"); throw(-1);  }
   }
   else {  fprintf(File_Error,"\n Ошибка формата"); throw(-1);  }

   for( i=Begin+1 ; i<End ; i++ )
   {
       Ifstr( Lexer->Word[i],"@Параметр" )
       {   Param1.Name=Lexer->Word[i+1] ;
	   Param.add( Param1 );
	   t_Param *P=&Param[Param.j-1] ;
	   End1=Lexer->find_paar( i+2,"{","}" );
	   for( i1=i+3 ; i1<End1 ; i1++ )
	   {  Ifstr( Lexer->Word[i1],"," ) continue ;
	      Value1.Name=Lexer->Word[i1] ;
	      P->Value.add( Value1 );
	   }
       }
       if( 0==strcmp( Lexer->Word[i],"@Часть_речи" ) ||
           0==strcmp( Lexer->Word[i],"@Структура" ) ||
	   0==strcmp( Lexer->Word[i],"@Структура1" ) ||
	   0==strcmp( Lexer->Word[i],"@Структура2" ) ||
	   0==strcmp( Lexer->Word[i],"@Множество" ) ||
	   0==strcmp( Lexer->Word[i],"@Беспорядок" ) ||
	   0==strcmp( Lexer->Word[i],"@Беспорядок1" ) ||
	   0==strcmp( Lexer->Word[i],"@Беспорядок2" ) )
       {
	   Part.add( Part1 );
	   Part[Part.j-1].read( i,this );
       }
       Ifstr( Lexer->Word[i],"@Формат" )
       {  Format.add( Form );
	  Form1=&Format[Format.j-1] ;
	  read_format( i, Form1 );
       }
       Ifstr( Lexer->Word[i],"@Файлы" )
       {
	   z=find_part( &Part,Lexer->Word[i+1] );
	   File1.i_part=z ;
	   z=find_format( &Format,Lexer->Word[i+2] );
	   File1.i_format=z ;
	   File1.FileName.j=0 ;

	   End1=Lexer->find_paar( i+3,"{","}" );
	   for( i1=i+4 ; i1<End1 ; i1++ )
	   {  char *a ;
	      Ifstr( Lexer->Word[i1],"," ) continue ;
	      a=(char *)Calloc( strlen(Lexer->Word[i1])+1,1 ); // Memory Leak
	      strcpy( a,Lexer->Word[i1] );
	      File1.FileName.add( a );
	   }
	   File.add( File1 );
       }
   }
   return End ;
}
catch( int E )
{
   throw( E );
}
}
/**************************************************************************/
//              прочесть запись типа @Формат { ... }
//     Begin - слово, с которого начинается
//     Format1 - то, что надо заполнить
/**************************************************************************/
void t_Lang :: read_format( long Begin, t_Format *Format1 )
{
   long   i,i1,s,z,z1,z2 ;
   long   End ;
   t_Struct *P ;
   t_Form    Form[10] ;

   for( i=0 ; i<10 ; i++ ) Form[i].init() ;

   z=find_part( &Part,Lexer->Word[Begin+1] );
   if( z<0 ) {  fprintf( File_Error,"\n Ошибка поиска части речи (%s)",Lexer->Word[Begin+1] ); throw(-1);  }
   Format1->i_part=z ;
   P=&Part[z] ;
   Format1->Name=Lexer->Word[Begin+2] ;

   Ifstr( Lexer->Word[Begin+3],"{" )
   {  End = Lexer->find_paar( Begin,"{","}" );
      if( End<0 ) {  fprintf( File_Error,"\n Ошибка непарности скобок"); throw(-1);  }
   }
   else {  fprintf( File_Error,"\n Ошибка формата"); throw(-1);  }

   s=0 ;
   for( i=Begin+3 ; i<End ; i++ )
   {
      Ifstr( Lexer->Word[i],"{" ) {  Form[s+1]=Form[s] ; s++ ; continue ;  }
      Ifstr( Lexer->Word[i],"}" ) {  s-- ; continue ;  }
      Ifstr( Lexer->Word[i],"@Слово" )
      {  
	 Format1->Record.add( Form[s] );
	 continue ;
      }

      for( i1=0 ; i1<P->Param.j ; i1++ )
	 if( 0==strcmp(P->Param[i1].Name,Lexer->Word[i]) )
	 {  z1=i1 ; z=P->Param[i1].param ; goto M_Ok ; }
      fprintf( File_Error,"\n Часть речи (%s) не имеет параметра(%s)",
	     P->Name,Lexer->Word[i] );
      Read_Error=-1 ;
      M_Ok:;

      Ifstr( Lexer->Word[i+1],"=" )
      {  // -------------- найти значение параметра -------------
	 z2=find_value( &Param[z].Value,Lexer->Word[i+2] );
	 if( z2<0 )
	 {  fprintf( File_Error,"\n Параметр(%s) не имеет значения(%s)",
		    Param[z].Name,Lexer->Word[i+2] );
	    throw(-1);
	 }
	 Form[s].value[z1]=z2 ;
	 i+=2 ;
      }
   }
}
/***************************************************************************/
//        сделать индексы сруктур в списке переименований структур
/***************************************************************************/
void t_Format1 :: make_part( t_Grammar *Grammar )
{  short i,z ;

   for( i=0 ; i<SouPart.j ; i++ )
   {  z=Grammar->find_struct( FROM, SouPart[i].Name );
      if( z<0 ) 
      {  fprintf( File_Error,"\n @Словари_перевода :: не могу найти часть речи %s",
                         SouPart[i].Name );
         Read_Error=-1 ;
      }
      SouPart[i].i_part = z ;
   }
   for( i=0 ; i<DstPart.j ; i++ )
   {  z=Grammar->find_struct( TO, DstPart[i].Name );
      if( z<0 )
      {  fprintf( File_Error,"\n @Словари_перевода :: не могу найти часть речи %s",
                         DstPart[i].Name );   
         Read_Error=-1 ;
      }
      DstPart[i].i_part = z ;
   }
}
/***************************************************************************/
//             найти индекс структуры по ее псевдониму
//    fTo - 0-источник или 1-приемник
//    Tag - псевдоним
//    ошибка!!! при чтении другого языка кеши не сбрасываются !!!!
/***************************************************************************/
short t_Format1 :: find_struct( char fTo, char *Tag  )
{  short i ;
   static short j1=0,j2=0 ;
   static short kash1[8]={ 0,1,2,3,4,5,6,7 } ;
   static short kash2[8]={ 0,1,2,3,4,5,6,7 } ;

   if( Tag[0]=='@' && Tag[1]=='0' )
       return -1 ; // если это константа

   if( fTo==0 )
   {  // ------ поиск в кеше ------------
      for( i=0 ; i<8 ; i++ )
          if( 0==strncmp(Tag,SouPart.list[kash1[i]].Tag,2) )
	      return SouPart.list[kash1[i]].i_part ;
      // ------ нормальный поиск -------
      for( i=0 ; i<SouPart.j ; i++ )
          if( 0==strncmp(Tag,SouPart.list[i].Tag,2) )
	  {   kash1[j1++]=i ; j1&=0x07 ;
	      return SouPart.list[i].i_part ;
	  }
   }
   if( fTo==1 )
   {  // ------ поиск в кеше ------------
      for( i=0 ; i<8 ; i++ )
         if( 0==strncmp(Tag,DstPart.list[kash2[i]].Tag,2) )
	     return DstPart.list[kash2[i]].i_part ;
      // ------ нормальный поиск -------      
      for( i=0 ; i<DstPart.j ; i++ )
         if( 0==strncmp(Tag,DstPart.list[i].Tag,2) )
	 {   kash2[j2++]=i ; j2&=0x07 ;
	     return DstPart.list[i].i_part ;
         }
   }
   if( fTo==-1 )
   {  // ------- сброс кешей -------------
      for( i=0 ; i<8 ; i++ )
         kash1[i]=kash2[i]=i ;
   }
   return -10 ; // структура не найдена
}
/**************************************************************************/
//            дать псевдоним структуры по ее индексу
//    fTo      - 0-источник или 1-приемник
//    i_struct - индекс структуры
/**************************************************************************/
char * t_Format1 ::get_tag( char fTo, short i_struct )
{  static char Str[3] ;
   short i ;

   strcpy( Str,"@1" );
   if( fTo==0 )
   {   for( i=0 ; i<SouPart.j ; i++ )
          if( SouPart.list[i].i_part==i_struct )
	  {  strncpy( Str,SouPart.list[i].Tag,2 ); break ; }
   }
   else
   {   for( i=0 ; i<DstPart.j ; i++ )
          if( DstPart.list[i].i_part==i_struct )
	  {  strncpy( Str,DstPart.list[i].Tag,2 ); break ; }
   }
   return Str ;
}
/**************************************************************************/
//            дать имя структуры по ее индексу
//    fTo      - 0-источник или 1-приемник
//    i_struct - индекс структуры
/**************************************************************************/
char * t_Format1 ::get_name( char fTo, short i_struct )
{  short i ;

   if( fTo==0 )
   {   for( i=0 ; i<SouPart.j ; i++ )
          if( SouPart.list[i].i_part==i_struct )
	     return SouPart.list[i].Name ;
   }
   else
   {   for( i=0 ; i<DstPart.j ; i++ )
          if( DstPart.list[i].i_part==i_struct )
	     return DstPart.list[i].Name ;
   }
   return "@0" ;
}
/**************************************************************************/
void t_Lang :: del( void )
{
   Param.del( );
   Part.del( );
   Format.del( );
   File.del( );
}
/***************************************************************************/
//    проверка на то, что часть структуры является "жесткой константой"
//    W     - проверяемая часть структуры (Слово)
//    i_str - индекс строки файла lang.txt, в которой это "Слово" записано
/***************************************************************************/
char tst_const( t_Word *W, long i_str )
{
   Ifstr( "@0",W->Name )
   {   W->i_struct=-1 ;
       W->type    =TCONST ;

       if( W->literal[0]==0 )
       {   fprintf( File_Error,"\n У константы нет значения \n Строка %ld %s",
		     i_str,W->Name );
	   throw(-1);
       }
       if( 0<W->Param.j ) goto M_ErrParam ;
       return 1 ;
   }
   Ifstr( "@1",W->Name )
   {   W->i_struct=-1 ;
       W->type    =TCONST1 ;
       if( 0<W->Param.j ) goto M_ErrParam ;
       return 1 ;
   }
   Ifstr( "@00",W->Name )
   {   W->i_struct=-2 ;
       W->type    =TWORD0 ;
       if( 0<W->Param.j ) goto M_ErrParam ;
       return 1 ;
   }
   return 0 ;

M_ErrParam:
   fprintf( File_Error,"\n У константы какие-то параметры \n Строка %ld %s",
	     i_str,W->Name );
   throw(-1);
}
/**************************************************************************/
short find_param( t_ParamList  *List, char *Str )
{
   for( short i=0 ; i<List->j ; i++ )
      if( 0==strcmp(List->list[i].Name,Str) ) return i ;
   return -1 ;
}
/**************************************************************************/
short find_part(  t_StructList   *List, char *Str )
{
   for( short i=0 ; i<List->j ; i++ )
      if( 0==strcmp(List->list[i].Name,Str) ) return i ;
   return -1 ;
}
/**************************************************************************/
short find_format(t_FormatList *List, char *Str )
{
   for( short i=0 ; i<List->j ; i++ )
      if( 0==strcmp(List->list[i].Name,Str) ) return i ;
   return -1 ;
}
/**************************************************************************/
short find_value( t_ValueList  *List, char *Str )
{
   if( 0==strcmp( "@0",Str ) ) return -1 ;
   for( short i=0 ; i<List->j ; i++ )
      if( 0==strcmp(List->list[i].Name,Str) ) return i ;
   return -1 ;
}
/***************************************************************************/
t_Form :: t_Form( void )
{
   for( short i=9 ; 0<=i ; i-- ) value[i]=-1 ;
}
/***************************************************************************/
void t_Form :: init(void)
{
   for( short i=9 ; 0<=i ; i-- ) value[i]=-1 ;
}
/***************************************************************************/
void t_Format1 :: init( void )
{
   Name="" ;
   SouPart.init();
   DstPart.init();
}
/***************************************************************************/
void t_Format1 :: del( void )
{
   SouPart.del();
   DstPart.del();
}
/***************************************************************************/
void t_Format2 :: init( void )
{
   Name="" ;
   strcpy( Divider,"=" );
   fsort=0 ;
}
/***************************************************************************/
void t_Format2 :: del( void )
{
}
/***************************************************************************/
t_Param ::  t_Param( void ){  init(); }
t_Param :: ~t_Param( void ){  del();  }
void t_Param :: init( void )
{  Name="" ;
   Value.init();
}
void t_Param :: del( void )
{  Value.del();  }
/***************************************************************************/
t_Format ::  t_Format( void ){  init(); }
t_Format :: ~t_Format( void ){  del();  }
void t_Format :: init( void )
{
   Name="" ;
   i_part =0 ;
   Record.init() ;
}
void t_Format :: del( void )
{
   Record.del() ;
}
/***************************************************************************/
t_File ::  t_File( void ){  init(); }
t_File :: ~t_File( void ){  del();  }
void t_File :: init( void )
{  i_part  =0 ;
   i_format=0 ;
   FileName.init();
}
void t_File :: del( void )
{  // по идее надо освобождать и строки по этим адресам
   FileName.del();
}
/**************************************************************************/
void t_Table :: init( void )
{  Size=0 ;
   In.init();
   Out.init();
   Value.init();
}
/**************************************************************************/
void t_Table :: del( void )
{  In.del();
   Out.del();
   Value.del();
}
/**************************************************************************/
void t_Table :: operator = ( t_Table &T )
{  type =T.type ;
   Size =T.Size ;
   In   =T.In ;
   Out  =T.Out ;
   Value=T.Value ;
}
/**************************************************************************/
t_Word :: t_Word( void )
{
   init( );
}
/**************************************************************************/
void t_Word :: init( void )
{
   literal    = "" ;
   type       = TWORD ;
   Param.init();
   order      =-1 ;
   i_struct   =-1 ;
   meaning_use= 0 ;
}
/**************************************************************************/
void t_Word :: del( void )
{
   Param.del();
}
/**************************************************************************/
void t_Word :: operator = ( t_Word &W )
{
   literal     = W.literal ;
   Name        = W.Name ;
   type        = W.type ;
   Param       = W.Param ;
   order       = W.order ;
   i_struct    = W.i_struct ;
   use         = W.use ;
   meaning_use = W.meaning_use ;
}
/**************************************************************************/
t_Struct :: t_Struct( void )
{
   init( );
}
/**************************************************************************/
void t_Struct :: init( void )
{
   type    =TNULL ;
   Name    ="" ;
   i_str   =-1 ;
   Word.init();
   Param.init();
   Relation.init();
}
/**************************************************************************/
void t_Struct :: del( void )
{
   Word.del();
   Param.del();
   Relation.del();
}
/**************************************************************************/
void t_Struct :: operator = ( t_Struct &S )
{
   Name    = S.Name;
   type    = S.type;
   i_str   = S.i_str;
   Param   = S.Param;
   Word    = S.Word;
   Relation= S.Relation;
}
/**************************************************************************/
t_Trans :: t_Trans( void )
{  init( );  }
/**************************************************************************/
void t_Trans :: init( void )
{
   From.init();
   To.init();
   Relation1.init();
   Relation2.init();
   Param1.init();
   Param2.init();
   Table.init();
}
/**************************************************************************/
void t_Trans :: del( void )
{
   From.del();
   To.del();
   Relation1.del();
   Relation2.del();
   Param1.del();
   Param2.del();
   Table.del();
}
/**************************************************************************/
void t_Trans :: operator = ( t_Trans &SS )
{  short     i ;
   t_Struct *F,*T ;

   F =&SS.From ;
   T =&SS.To ;

   From.Param=F->Param ;
   To.Param  =T->Param ;
   // ------------ копирование слов источника -----------
   for( i=0 ; i<F->Word.j ; i++ )
      From.Word.add( F->Word[i] );

   // ------------ копирование слов приемника -----------
   for( i=0 ; i<T->Word.j ; i++ )
      To.Word.add( T->Word[i] );

   // ------------ копирование параметров и таблиц ------
   Relation1 = SS.Relation1 ;
   Relation2 = SS.Relation2 ;
   Param1    = SS.Param1 ;
   Param2    = SS.Param2 ;
   Table     = SS.Table ;
   From.type = F->type ;
   From.i_str= F->i_str ;
   To.type   = T->type ;
   To.i_str  = T->i_str ;
   From.Name = F->Name ;
   To.Name   = T->Name ;
}

/**************************************************************************/
short find_param1( t_Param1List *List, char *Str )
{
   for( short i=0 ; i<List->j ; i++ )
      if( 0==strcmp(List->list[i].Name,Str) ) return i ;
   return -1 ;
}
/**************************************************************************/
short find_value2( t_strList  *List, char *Str )
{
   if( 0==strcmp( "@0",Str ) ) return -1 ;
   for( short i=0 ; i<List->j ; i++ )
      if( 0==strcmp(List->list[i],Str) ) return i ;
   return -1 ;
}
/**************************************************************************/
void zerstoren_param( char *Str, char *Dir, char **Str1 )
{
   if( Str[0]=='&' ) { *Dir=1 ; *Str1=Str+1 ; }
       else          { *Dir=0 ; *Str1=Str ; }
}
/**************************************************************************/
void zerstoren_word( char *Str, char *use, char **Str1 )
{
   if( Str[0]=='!' ) { *use=1 ; *Str1=Str+1 ; }
       else          { *use=0 ; *Str1=Str ; }
}
/**************************************************************************/
t_DirTrans *current_dir_trans( void )
{
   return z_DirTrans ;
}
/**************************************************************************/
DEFINE_LIST_TYPE ( t_Trans,   t_TransList )
DEFINE_LIST_TYPE ( t_Table,   t_TableList )
DEFINE_LIST_BTYPE( t_Param1,  t_Param1List )
DEFINE_LIST_BTYPE( t_2Index,  t_2IndexList )
DEFINE_LIST_TYPE ( t_Word,    t_WordList )
DEFINE_LIST_TYPE ( t_Struct,  t_StructList )
DEFINE_LIST_BTYPE( t_Relation,t_RelationList )
DEFINE_LIST_BTYPE( t_PartDsk, t_PartDskList )
DEFINE_LIST_TYPE ( t_Format1, t_Format1List )
DEFINE_LIST_TYPE ( t_Format2, t_Format2List )
DEFINE_LIST_BTYPE( short,     t_shortList )
DEFINE_LIST_BTYPE( long,      t_longList )
DEFINE_LIST_BTYPE( t_Value,   t_ValueList )
DEFINE_LIST_TYPE ( t_Param,   t_ParamList )
DEFINE_LIST_TYPE ( t_Format,  t_FormatList )
DEFINE_LIST_BTYPE( char *,    t_strList )
DEFINE_LIST_TYPE ( t_File,    t_FileList )
DEFINE_LIST_BTYPE( t_Rename,  t_RenameList )
DEFINE_LIST_BTYPE( t_Form,    t_FormList )
