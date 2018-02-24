/*******************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//     t_Windows - главный класс интерфейса (ничего общего с Микрософтом)
//     Анисимов Д.В.                               сегодня
/*******************************************************************/

# include <string.h>
# include <stdlib.h>
# include <unistd.h>

# include "mylib.h"
# include "video.h"
# include "color.h"
# include "core.h"
# include "face.h"
# include "menu.h"
# include "edit.h"
# include "edit1.h"
# include "edit2.h"
# include "debug.h"
# include "path.h"
# include "word.h"
# include "word1.h"
# include "slowo1.h"

char hk_Edit[]  ="[F1]Подсказ[F2]Словарь[^C ->]Фраза+1[^C <-]Фраза-1[F7]Пер.текста[F8]Пер.фразы[F9]Варианты[F10]Меню" ;
char hk_Help[]  ="[Enter] переход по ссылке [F10] возврат в основной режим" ;
char hk_Path[]  ="[F1] Подсказка [Tab] переход между окнами [Стрелки] хождение по дереву вариантов [F10] Выход" ;
char hk_Slowo[] ="[F1] Подсказка [Стрелки] выбрать [Shift+PgDn/PgUp] След/пред слово [Esc] стереть слово [F10] Выход" ;
char hk_Word[]  ="[F1] Подсказка [^C <-] След режим [^C ->] пред режим [->] Выбор структуры [F10] Выход" ;
char hk_Word1[] ="[F1] Подсказка [<- ->] След/пред фраза [Enter] Задать перевод [F10] Выход" ;

char hk_First[] ="[F1] Подсказка [F3] Источник [F4] Приемник [F5] Отладчик [Enter] Подробнее [F10]Выход" ;
char hk_Sou[]   ="[F1] Подсказка [F2] Первое чтение [F4] Приемник [F5] Отладчик [F10] Выход" ;
char hk_Dst[]   ="[F1] Подсказка [F2] Первое чтение [F3] Источник [F5] Отладчик [F10] Выход" ;
char hk_Debug1[]="[F1] Подсказка [<- ->] След/пред слово [Пробел] полый/сокр список [Enter] Список вариантов [F10] Выход";
char hk_Debug2[]="[F1] Подсказка [Enter] Структура варианта [<-|] возврат в главную таблицу [F10] Выход" ;
char hk_Debug3[]="[F1] Подсказка [Enter] шаг вниз [<-|] шаг вверх по дереву конструкции [F10] Выход" ;
char hk_Gramm[] ="[F1]Подсказка[Enter]вниз[<-|]вверх по дереву грамматики[Tab]Источник/приемник[F8]Параметры[F10]Выход" ;
char hk_Perev[] ="[F1]Подсказка [Enter]шаг вниз [<-|]шаг вверх по дереву словаря [F10]Выход" ;
char hk_Dicts[] ="[F1]Подсказка [Enter] просмотреть список форм словаря [F10]Выход" ;

t_Windows     Windows ;
t_Dir         Dir ;
t_HelpData    HelpData ;
t_Text        HelpText,ErrorText ;
t_Edit        HelpView,ErrorView ;
t_EditTrans   Edit1,Edit2 ;
t_Edit2       EditFrom ;
t_Edit3       EditTree1(0),EditTree2(1) ;
t_Debug       Debug ;
t_Path        Path ;
t_GrammarView GrammarView ;
t_SlowoView   SlowoView ;
t_Slowo3View  Slowo3View ;
t_NewWord     NewWord ;
t_NewWord1    NewWord1 ;
t_AddWord     AddWord ;
t_Slowo1      Slowo ;
t_Option      Option ;
char          FileName[100]="" ;
char          MercuryDir[200]="../" ;

void select_lang( void *S );

/***************************************************************************/
//          функция определения "слова с точкой"
/***************************************************************************/
char   is_const( char *Str )
{
   short       part[20] ;
   if( 0<Perevod.part( Str,part ) ) return 1 ;
   return 0 ;
}
/***************************************************************************/
void  Window_help( void *Void )
{  char *Str=(char *)Void ;
   short sx,sy ;

   s_get_size( sx,sy );
   s_save( 1,1,sy-1,sx-1 );
   HelpView.init( HelpData.get_help( Str ) );
   HelpView.main( );
   s_restore();
}
/***************************************************************************/
t_Windows :: t_Windows( )
{
   Reg='1' ;
}
/***************************************************************************/
void t_Windows :: init( void )
{
   HelpView.set_text( &HelpText );
   HelpView.set_view( 1 ) ;
   ErrorView.set_text( &ErrorText );
   ErrorView.set_view( 1 ) ;
   
   Edit1.set_name("Исходный_текст");
   Edit2.set_name("Переведенный_текст");
   EditFrom.set_name("Первое чтение");
   EditTree1.set_name("Структура источника");
   EditTree2.set_name("Структура приемника");
   HelpView.set_name("Подсказка");
   ErrorView.set_name("Ошибки описания грамматики");
   AddWord.set_name("Новые слова");
   NewWord1.set_name("Новые слова 1");
   Slowo.set_name("Словарь");
   Edit1.set_funk_const( &is_const );
}
/***************************************************************************/
void t_Windows :: set_pos( void )
{
   short sx,sy,y ;
   s_get_size( sx,sy );
   y=sy/2 ;

   Edit1.set_pos      ( 0, 1  , sx-1, y    );
   Edit2.set_pos      ( 0, y+1, sx-1, sy-2 );
   EditFrom.set_pos   ( 1, 2  , sx-2, sy-2 );
   EditTree1.set_pos  ( 1, 2  , sx-2, sy-2 );
   EditTree2.set_pos  ( 1, 2  , sx-2, sy-2 );
   Debug.set_pos      ( 1, 2  , sx-2, sy-2 );
   Path.set_pos       ( 1, 2  , sx-2, sy-2 );
   GrammarView.set_pos( 1, 2  , sx-2, sy-2 );
   SlowoView.set_pos  ( 1, 2  , sx-2, sy-2 );
   Slowo3View.set_pos ( 1, 2  , sx-2, sy-2 );
   HelpView.set_pos   ( 1, 2  , sx-2, sy-2 );
   ErrorView.set_pos  ( 1, 2  , sx-2, sy-2 );
   AddWord.set_pos    ( 1, 2  , sx-2, sy-2 );
   NewWord1.set_pos   ( 1, 2  , sx-2, sy-2 );
}
/***************************************************************************/
void  Window_menu( void *Void )
{
   char d0,d1=0 ;
   char f_cp1251=0 ;

   d0=((char *)Void)[0] ;
   if( d0=='1' || d0=='s' || d0=='3' )
   {   d1=((char *)Void)[1] ;
       if( d1=='w' ) f_cp1251=1 ; 
   }

   switch( d0 )
   {
      case '1' : Windows.create( NULL,f_cp1251 );  break ;
      case 's' : Windows.save_src( f_cp1251 );     break ;
      case '2' : Windows.save( f_cp1251 );         break ;
      case '3' : Windows.save_as( f_cp1251 );      break ;
      case '4' : Windows.exit( );         break ;
      case '5' : Windows.find_forward( ); break ;
      case '6' : Windows.find_backward( );break ;
      case '7' : Windows.replace( );      break ;
      case '8' : Windows.goto_str( );     break ;
      case 'T' : Windows.trans( );        break ;
      case 't' : Windows.trans_frasa( );  break ;
      case 'B' : Windows.trans_bilingvo();break ;
      case 'f' : Windows.win_firstlook(); break ;

      case 'R' : {  
		    null_menu( );
		    message("Идет загрузка данных.\n        Ждите...");
		    s_refresh();
		    if( 0>reread_grammar( MercuryDir,z_DirTrans ))
		    {    ErrorView.text()->read_file("mercury.err");
			 ErrorView.main( );
		    }
		    else ErrorView.init("");
		    Windows.paint();
		 }
		 break ;
      case 'P' : {   select_lang( z_DirTrans );
                 }
		 break ;
      case 'F' : if( Edit1.Text1.fformat==0 )
                 {   Edit1.Text1.fformat=1 ;
		     Edit2.Text1.fformat=1 ;
                     menu_set( "Спец Формат   [ ]","Спец Формат   [x]" );
		 }else
		 {   Edit1.Text1.fformat=0 ;
		     Edit2.Text1.fformat=0 ;
		     menu_set( "Спец Формат   [x]","Спец Формат   [ ]" );
		 }
                 break ;
   }
}
/***************************************************************************/
void t_Windows :: main_loop( void )
{  e_WinMsg r ;

   init();
   set_pos();
   null_menu( );

   while( 1 )
   {
      paint( );
      switch( Reg )
      {  case '1': r=Edit1.main_loop( ); Reg1=Reg ; break ;
	 case '2': r=Edit2.main_loop( ); Reg1=Reg ; break ;
	 case '3': r=EditFrom.loop( );   break ;
	 case '4': r=EditTree1.loop( );  break ;
	 case '5': r=EditTree2.loop( );  break ;
	 case '6': r=Debug.loop( );      break ;
	 case '7': r=GrammarView.loop( );break ;
	 case '8': r=SlowoView.loop( );  break ;
	 case '9': r=Slowo3View.loop( ); break ;
         case 'b': r=win_path( );        break ;
         case 'c': r=win_path2( );       break ;
	 case 's': r=win_slowo( );       break ;
         case 'd': r=AddWord.loop( );    break ;
         case 'D': r=NewWord1.main_loop( *(Edit1.text()) ); break ;
	 case 'e': r=ErrorView.main();   break ;
      }      
      switch( r )
      {
         case WM_NEXT :
	      if( Reg=='1' ) { Reg='2' ; break ; }
	      if( Reg=='2' ) { Reg='1' ; break ; }
         case WM_ESC    : Reg=Reg1 ; break ;
	 case WM_EDIT1  : Reg='1' ; break ;
	 case WM_EDIT2  : Reg='2' ; break ;
	 case WM_FIRST  : Reg='3' ; break ;
	 case WM_SRC    : Reg='4' ; break ;
	 case WM_DST    : Reg='5' ; break ;
	 case WM_DEBUG  : Reg='6' ; break ;
	 case WM_PATH   : Reg='b' ; break ;
	 case WM_PATH2  : Reg='c' ; break ;
	 case WM_GRAMMAR: Reg='7' ; break ;
	 case WM_SLOWO  : Reg='8' ; break ;
	 case WM_BASE   : Reg='9' ; break ;
         case WM_ADD_WORD : Reg='d' ; break ;
	 case WM_ADD_WORD1: Reg='D' ; break ;
	 case WM_SLOWO1: Reg='s' ; break ;
	 case WM_ERROR:  Reg='e' ; break ;
      }
   }
}
/***************************************************************************/
e_WinMsg t_Windows :: win_path2( void )
{
   // ------ установление размеров окошек ---------------
   short sx,sy,y1,y2 ;

   if( Read_Error!=0 )
   {  figna("Грамматика или словари не прочитаны.\n       Перевод невозможен!!!");
      return WM_ESC ;
   }
   s_get_size( sx,sy );
   y2=sy/2 ; y1=y2/2 ;

   Edit1.set_pos ( 0, 1   , sx-1, y1   );
   Edit2.set_pos( 0, y1+1, sx-1, y2   );
   Path.set_pos ( 0, y2+1, sx-1, sy-1 );

   // ------ чтение текущей фразы ----------------------
   char *Str,Str1[1000] ;
   
   Str =Edit1.get_frasa( Edit1.current_pos( ) );
   if( Str[0]!=0 ) Core.translate( Str,Str1 );

   e_WinMsg r ;
   char    Reg2 ,Reg3 ;

   Reg2=Reg3='b' ;

   null_menu( );
   while( 1 )
   {
      Edit1.raden_y3();
      Edit2.raden_y3();
      Edit1.paint();
      Edit2.paint();

      switch( Reg2 )
      {  case '1': r=Edit1.main_loop( ); Reg3=Reg2 ; break ;
	 case '2': r=Edit2.main_loop( ); Reg3=Reg2 ; break ;
         case 'b': r=Path.loop( );       Reg3=Reg2 ; break ;
      }
      switch( r )
      {
	 case WM_NEXT :
            switch( Reg2 ) 
            {   case '1' : Reg2='2' ; break ;
		case '2' : Reg2='b' ; break ;
		case 'b' : Reg2='1' ; break ;
	    }
            break ;  
	 case WM_ESC : 
	      if( Reg2=='1' || Reg2=='2' || Reg2=='b') goto M_End ;
	          else Reg2=Reg3 ;
	      break ;
	 case WM_OK :
	      if( Reg2!='b' ) break ;
	      Str=Path.Antwort ;
	      if( Str[0]==0 ) break ;
	      Edit2.put_frasa( Edit2.current_pos( ),Str );
	      Edit1.next_frasa();
	      Edit2.next_frasa();
	      Str =Edit1.get_frasa( Edit1.current_pos( ) );
	      if( Str[0]!=0 ) Core.translate( Str,Str1 );
              break ;
         case WM_N_FRASA:
	 case WM_P_FRASA:
 	      if( Reg2!='b' ) break ;
	      if( r==WM_N_FRASA )
	      {  Edit1.next_frasa();
	         Edit2.next_frasa();
	      }
	      else
	      {  Edit1.prev_frasa();
	         Edit2.prev_frasa();
	      }
	      Str =Edit1.get_frasa( Edit1.current_pos( ) );
	      if( Str[0]!=0 ) Core.translate( Str,Str1 );	      
 	      break ;
	 case WM_EDIT1  : Reg='1' ; break ;
	 case WM_EDIT2  : Reg='2' ; break ;
	 case WM_FIRST  : Reg='3' ; break ;
	 case WM_SRC    : Reg='4' ; break ;
	 case WM_DST    : Reg='5' ; break ;
	 case WM_DEBUG  : Reg='6' ; break ;
	 case WM_PATH   : Reg='b' ; break ;
	 case WM_PATH2  : Reg='c' ; break ;
	 case WM_GRAMMAR: Reg='7' ; break ;
	 case WM_SLOWO  : Reg='8' ; break ;
	 case WM_BASE   : Reg='a' ; break ;
      }
   }
M_End:
   Edit1.set_pos( 0, 1   , sx-1, y2   );
   Edit2.set_pos( 0, y2+1, sx-1, sy-1 );
   Path.set_pos ( 1, 2   , sx-2, sy-2 );
   return WM_ESC ;
}
/***************************************************************************/
//                  окно выбора варианта перевода
/***************************************************************************/
e_WinMsg t_Windows :: win_path( void )
{
   e_WinMsg r ;
   char *Str ;
   static char Str1[1000] ;

   if( Read_Error!=0 )
   {  figna("Грамматика или словари не прочитаны.\n       Перевод невозможен!!!");
      return WM_ESC ;
   }
   Str =Edit1.get_frasa( Edit1.current_pos( ) );
   if( Str[0]!=0 )
       Core.translate( Str,Str1 );

   r=Path.loop( );
   Str=Path.Antwort ;
   if( Str[0]!=0 )
       Edit2.put_frasa( Edit2.current_pos( ),Str );

   Edit1.paint();
   Edit2.get_str();
   Edit2.paint();
   return r ;
}
/***************************************************************************/
//                   работа со словарем для человека
/***************************************************************************/
e_WinMsg t_Windows :: win_slowo( void )
{
   // ------ установление размеров окошек ---------------
   short sx,sy,xx,yy ;

   if( Read_Error!=0 )
   {  figna("Грамматика или словари не прочитаны.\n       Перевод невозможен!!!");
      return WM_ESC ;
   }
   s_get_size( sx,sy );

   yy=sy/2 ;
/*
   xx=sx*2/3 ;

   Edit1.set_pos(  0,    1, xx-1, yy   );
   Edit2.set_pos(  0, yy+1, xx-1, sy-1 );
   Slowo.set_pos( xx,    1, sx-1, sy-2, 0 );
*/
   short y1,y2 ;
   y2=sy/2 ; y1=y2/2 ;

   Edit1.set_pos( 0, 1   , sx-1, y1   );
   Edit2.set_pos( 0, y1+1, sx-1, y2   );
   Slowo.set_pos( 0, y2+1, sx-1, sy-2, 1 );

   char *Str1="" ;
   e_WinMsg r ;

   null_menu( );

   Edit1.raden_y3();
   Edit2.raden_y3();
   Edit1.paint();
   Edit2.paint();

   if( 0<Grammar.File2.j )
   {  char r,Str[300] ;

      sprintf( Str,"%s/%s",MercuryDir,Grammar.File2[0].FileName[0] );
      r=Slowo.read_buch( Str );
      if( r!=0 )
      {  char Str1[400] ;
         sprintf( Str1,"Не могу прочесть словарь \n%s",Str );
         figna( Str1 );
	 return WM_ESC ;
      }
   }
   else
   {  figna( "А словаря-то нет!" );
      return WM_ESC ;
   }
   while( 1 )
   {  short i,n ;
      char  c,Str[300] ;
   
      Str1=Edit1.get_word( Edit1.current_pos( ) );

      // ------ отрезать знаки препинания от слова -------------
      strcpy( Str,Str1 );
      n=strlen( Str );
      for( i=n-1 ; 0<i ; i-- )
      {  c=Str[i] ;
         if( !(c==',' || c=='.' || c=='!' || c=='?' || c==':' ||
	       c=='\''|| c=='`'|| c=='\"'|| c=='-' )) break ;
         Str[i]=0 ;
      }
            
      Slowo.set_str( Str );
      r=Slowo.main_loop( );
      switch( r )
      {
         case WM_ESC : goto M_End ;
	 case WM_N_WORD: 
	      Edit1.next_word( );
	      Edit1.paint();
	      break ;
	 case WM_P_WORD: 
	      Edit1.prev_word( );
	      Edit1.paint();
	      break ;
      }
   }
M_End :
   Edit1.set_pos( 0, 1   , sx-1, yy   );
   Edit2.set_pos( 0, yy+1, sx-1, sy-1 );
   return WM_ESC ;
}
/***************************************************************************/
void t_Windows :: paint( void )
{  
   short sx,sy,yy ;
   static short sx1=80,sy1=25 ;

   s_get_size( sx,sy );
   yy=sy/2 ;
   s_rame2_f( 1,0,sy-1,sx-1,red_rame_Color );
   if( sx!=sx1 || sy!=sy1 )
   {  // ------------ переразместить все окошки ----------------
      set_pos( );
      null_menu();
   }
   Edit1.Bold=Edit2.Bold=0 ; ;
   if( Reg=='1' ) Edit1.Bold=1 ;
   if( Reg=='2' ) Edit2.Bold=1 ;
   Edit1.paint();
   Edit2.paint();
}
/***************************************************************************/
void t_Windows :: trans( void )
{  long  i,L ;
   char *Str,Str1[1000],Str2[50] ;
   char *Text ;
   
   if( Read_Error!=0 )
   {  figna("Грамматика или словари не прочитаны.\n       Перевод невозможен!!!");
      return ;
   }
   Text=(char *)Calloc( 1000000,sizeof(char) );

   L=Edit1.text()->length() ;
   for( i=0 ; i<L ; i=Edit1.text()->next_frasa(i) )
   {  sprintf( Str2,"Переведено %d/%d",i,L );
      message( Str2 );
      Str =Edit1.get_frasa(i);
      Core.translate( Str,Str1 );
      strcat( Text, Str1 );
      strcat( Text, "\n" );
   }
   Edit2.init( Text );
   Free( Text );
   
   Edit1.paint();
   Edit2.get_str();
   Edit2.paint();
}
/***************************************************************************/
void t_Windows :: trans_bilingvo( void )
{  long  i,i1,i2,z,L ;
   char *Str,Str1[1000],Str2[50] ;
   char *Text ;
   
   if( Read_Error!=0 )
   {  figna("Грамматика или словари не прочитаны.\n       Перевод невозможен!!!");
      return ;
   }
   Text=(char *)Calloc( 1000000,sizeof(char) );

   L=Edit1.text()->length() ;
   for( i=0 ; i<L ; )
   {  z=i ;
      // ----- вставить пять фраз на языке оригинала
      strcat( Text, "\n" );
      for( i1=0 ; i1<5 && i<L ; i1++ )
      { 
         Str =Edit1.get_frasa(i);
	 for( i2=0 ; Str[i2]!=0 ; i2++ )
	    if( Str[i2]=='\n' ) Str[i2]=' ' ;
	 strcat( Text, Str );
	 strcat( Text, "\n" );
         i=Edit1.text()->next_frasa(i);
      }
      // ----- вставить пять фраз на языке перевода
      strcat( Text, "\n" );
      i=z ;
      for( i1=0 ; i1<5 && i<L ; i1++ )
      {  sprintf( Str2,"Переведено %d/%d",i,L );
	 message( Str2 );
	 Str =Edit1.get_frasa(i);
	 Core.translate( Str,Str1 );
	 strcat( Text, Str1 );
	 strcat( Text, "\n" );
	 i=Edit1.text()->next_frasa(i);
      }
   }
   Edit2.init( Text );
   Free( Text );
   
   Edit1.paint();
   Edit2.get_str();
   Edit2.paint();
}
/***************************************************************************/
void t_Windows :: trans_frasa( void )
{  
   char *Str ;
   long p ;
   static char Str1[1000] ;

   if( Read_Error!=0 )
   {  figna("Грамматика или словари не прочитаны.\n       Перевод невозможен!!!");
      return ;
   }
   p=Edit1.current_pos( ) ;
   Str =Edit1.get_frasa( p );
   Core.translate( Str,Str1 );
   Edit2.put_frasa( Edit2.current_pos( ),Str1 );

   Edit1.paint();
   Edit2.get_str();
   Edit2.paint();
}
/***************************************************************************/
void t_Windows :: win_firstlook( void )
{  
   char *Str ;
   static char Str1[1000] ;

   Str =Edit1.get_frasa( Edit1.current_pos( ) );
   Core.translate( Str,Str1 );
   paint( );
   EditFrom.loop( );
   paint( );
}
/***************************************************************************/
void t_Windows :: exit( void )
{
   if( 1==fragen( "Запомнить файл перевода? (1-Да 0-Нет)"  ) ) 
       save( 0 ) ;

   s_nacht( ) ;
   s_redraw( );
   s_end_schone( );
   ::exit(0);
}
/***************************************************************************/
void t_Windows :: save_src( char f_cp1251 )
{  char Buf[200] ;

   Edit1.write( Edit1.Name ,f_cp1251 );
   sprintf( Buf,"Файл \"%s\" сохранен",Edit1.Name );
   figna( Buf );
}
/***************************************************************************/
void t_Windows :: save( char f_cp1251 )
{
   Edit2.write( "mercury_out.txt",f_cp1251 );
   figna( "Файл \"mercury_out.txt\" сохранен" );
}
/***************************************************************************/
void t_Windows :: save_as( char f_cp1251 )
{  char f ;
   char Buf[200] ;

   f=Dir.direktor( Buf,1 );   
   if( f<0 )
      strcpy( Buf,"mercury_out.txt" );
   Edit2.set_name( Buf );
   Edit2.write( Buf, f_cp1251 );

   paint( );
}
/***************************************************************************/
void t_Windows :: find_forward( void )
{  
   if( Reg=='1') Edit1.find_forward( );
   if( Reg=='2') Edit2.find_forward( );
}
/***************************************************************************/
void t_Windows :: find_backward( void )
{  
   if( Reg=='1') Edit1.find_backward( );
   if( Reg=='2') Edit2.find_backward( );
}
/***************************************************************************/
void t_Windows :: replace( void )
{  
   if( Reg=='1') Edit1.replace( );
   if( Reg=='2') Edit2.replace( );
}
/***************************************************************************/
void t_Windows :: goto_str( void )
{  
   if( Reg!='1' ) Edit1.goto_str( );
   if( Reg!='2' ) Edit2.goto_str( );
}
/***************************************************************************/
void t_Windows :: prev_frasa( void )
{
   Edit1.prev_frasa();
   Edit2.prev_frasa();
   Edit1.paint();
   Edit2.paint();
}
/***************************************************************************/
void t_Windows :: next_frasa( void )
{
   Edit1.next_frasa();
   Edit2.next_frasa();
   Edit1.paint();
   Edit2.paint();
}
/***************************************************************************/
void t_Windows :: create( char *Name, char f_cp1251 )
{  char f ;
   char Buf[200] ;

try
{  if( Name==NULL )
	{  f=Dir.direktor( Buf,0 );  }
   else {  strcpy(Buf,Name); f=0;  }
   if( f>=0 )
   {
      if( f==0 )
	 Edit1.read( Buf, f_cp1251 );
      Edit1.set_name( Buf );
   }
   else
      create( "new.txt",0 );
   
   paint( );
}
catch( ... )
{   paint();
    figna( error_get() );
}
}
/************************************************************************/
void preproc_exe( void *S )
{  char Str[200] ;
   int  r ;

   if( Edit1.Text1.fformat==1 )
   {  figna("Вы уже запускали препроцессор. Хватит.");
      return ;
   }
   Strcpy( Str,MercuryDir, 200 );
   Strcat( Str,"/bin/", 200 );
   Strcat( Str,(char *)S, 200 );
   Strcat( Str," ", 200 );
   Strcat( Str,Edit1.Name, 200 );
   r=system( Str );
   if( r==0 )
   {   Strcpy( Str,Edit1.Name, 200 );
       Strcat( Str,".pre", 200 );
       Strcpy( Edit1.Name,Str, 200 );
       Edit1.read( Str );
       Edit1.Text1.fformat=1 ;
   }
   else
   {   figna("Препроцессор не смог нормально отработать.\nПочему - не знаю.");
   }
}
/************************************************************************/
void postproc_exe( void *S )
{  char Str[200] ;
   int  r ;

   Edit2.write( "tmp" );
   Strcpy( Str,MercuryDir, 200 );
   Strcat( Str,"/bin/postproc tmp", 200 );
   r=system( Str );
   if( r==0 )
   {
       Edit2.read( "tmp.post" );
   }
   else
   {   figna("Постпроцессор не смог нормально отработать.\nПочему - не знаю.");
   }
   unlink( "tmp" );
   unlink( "tmp.post" );
   Edit2.get_str();
   Edit2.paint();
}
/************************************************************************/
void select_lang( void *S )
{  int r ;

   null_menu( );
   message("Идет загрузка данных.\n        Ждите...");
   s_refresh();

   z_DirTrans=(t_DirTrans *)S ;
   r=read_grammar( MercuryDir,z_DirTrans );
   if( r<0 ) 
   {  ErrorView.text()->read_file("mercury.err");
      error_add("Грамматика или словари не прочитаны!!!\n");
      error_add("Корректная работа невозможна!!!");
      figna( error_get() );
      ErrorView.main( );
   }
   else
   {  ErrorView.init("");
   }
   Windows.paint();
}
/************************************************************************/
//                 построить данные для препроцессора
/************************************************************************/
void preproc_rc( char *File )
{  long   i,L ;
   char  *Mass ;
   FILE  *ff ;

   ff=fopen( File,"r" );
   if( ff==NULL ) return ;
   fclose(ff);
   L=file_to_mass( File,&Mass );

   t_Lexer Lexer ;

   Lexer.set_breaker((uchar *)":");
   Lexer.init( Mass,L );
   Lexer.remark();
   Lexer.make_words();

   t_Menu *PP_Menu=(t_Menu *)Calloc(1+Lexer.n_word(),sizeof(t_Menu) );

   for( i=0 ; i<Lexer.n_word(); i++ )
   {  PP_Menu[i].flag=F_FUNK ;
      PP_Menu[i].ret =0 ;
      PP_Menu[i].funk=preproc_exe ;
      PP_Menu[i].data=Calloc( 1+strlen(Lexer.word(i)),1 );
      strcpy( (char *)PP_Menu[i].data,Lexer.word(i) );
      Strcpy( PP_Menu[i].name,Lexer.word(i),30 );
   }
   PP_Menu[i].flag=F_ENDE ;
   t_Menu *PP1_Menu=menu_find( "Препроцессор" );
   if( PP1_Menu!=NULL )
   {   PP1_Menu->flag=F_MENU ;
       PP1_Menu->data=PP_Menu ;   
   }
}
/************************************************************************/
//             построить данные для направления перевода
//      че-то мне не нравится, что это здесь
//      надо разделить на две функции - чтение и формирование_меню
/************************************************************************/
void language_rc( char *File )
{  long   i,L,n_Lang ;
   char  *s,*s1,*Mass ;
   FILE  *ff ;

try
{
   // -------- чтение файла --------------------------
   ff=fopen( File,"r" );
   if( ff==NULL ) throw(-1) ;
   fclose(ff);
   L=file_to_mass( File,&Mass );

   t_Lexer Lexer ;

   Lexer.set_breaker((uchar *)":");
   Lexer.init( Mass,L );
   Lexer.remark();
   Lexer.make_words();

   t_Menu *PP_Menu=(t_Menu *)Calloc(1+Lexer.n_word()/2,sizeof(t_Menu) );
   DirTrans=(t_DirTrans *)Calloc( Lexer.n_Word/2 , sizeof(t_DirTrans) );

   // ------- формирование массива DirectTranslation ------------------
   n_Lang=Lexer.n_Word/2 ;
   for( i=0 ; i<n_Lang ; i++ )
   {  s=Lexer.Word[2*i] ;
      s1=(char *)Calloc( strlen(s)+1,sizeof(char) );
      strcpy( s1,s );
      DirTrans[i].Dir=s1 ;
      s=Lexer.Word[2*i+1] ;
      s1=(char *)Calloc( strlen(s)+1,sizeof(char) );
      strcpy( s1,s );
      DirTrans[i].Name=s1 ;
   }

   // ------- формирование меню ------------------------
   for( i=0 ; i<n_Lang ; i++ )
   {  PP_Menu[i].flag=F_FUNK ;
      PP_Menu[i].ret =0 ;
      PP_Menu[i].funk=select_lang ;
      PP_Menu[i].data=&DirTrans[i] ;
      Strcpy( PP_Menu[i].name,DirTrans[i].Name,30 );
   }
   PP_Menu[i].flag=F_ENDE ;
   t_Menu *PP1_Menu=menu_find( "Направление" );
   if( PP1_Menu!=NULL )
   {   PP1_Menu->flag=F_MENU ;
       PP1_Menu->data=PP_Menu ;   
   }
   PP1_Menu->flag=F_MENU ;
}
catch( int E )
{     
   printf("File \""); printf( File ); printf("\" not read." );
   throw( E );
}
}
