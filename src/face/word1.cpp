/*******************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//             Ввод новых слов и выражений (список новых слов)
//      Анисимов Д.В.                               сегодня
/*******************************************************************/
# include <string.h>
# include <list.h>
# include <mylib.h>
# include <core.h>
# include <color.h>
# include <face.h>
# include <hotkey.h>
# include <video.h>
# include <edit.h>
# include <word.h>
# include <word1.h>

extern t_AddWord     AddWord ;

/*******************************************************************/
//                        сравнение слов
/*******************************************************************/
int   wordcmp1( const void *a, const void *b )
{  t_sortWord *A=(t_sortWord *)a ;
   t_sortWord *B=(t_sortWord *)b ;
   int  f=strcmp( A->str,B->str ) ;
   
   if( f!=0 ) return f ;
       else   return A->index-B->index ;
}
/*******************************************************************/
//                        сравнение структур
/*******************************************************************/
int   structcmp( const void *a, const void *b )
{  t_sortStruct *A=(t_sortStruct *)a ;
   t_sortStruct *B=(t_sortStruct *)b ;
   int  f =A->counter -B->counter ;
   int  f1=A->counter1-B->counter1 ;
   
   if( f1!=0 )
       return -f1 ;
   else
   {   if( f!=0 ) return -f ;
           else   return A->i_tree-B->i_tree ;
   }
}
/*******************************************************************/
//
/*******************************************************************/
t_NewWord1 :: t_NewWord1()
{  
   Word.init() ;
   WordTree.init() ;
   sortWord  =NULL ;
   sy_word   =NULL ;
   sortStruct=NULL ;
   Mass      =NULL ;
   n_sortWord=0 ;
   i_word    =0 ;
   i_begin   =0 ;
}
/*******************************************************************/
//
/*******************************************************************/
t_NewWord1 :: ~t_NewWord1()
{  
   Word.del();
   WordTree.del();
   Free(sortWord);
   Free(sy_word);
   Free(sortStruct);
   Free(Mass);
}
/*******************************************************************/
//           построить массив новых слов и выражений
/*******************************************************************/
void t_NewWord1 :: make_list_words( t_Text &Text )
{  long   i,i1,j,L ;

   Word.del();
   WordTree.del();
   Free(sortWord);
   Free(sy_word);
   Free(sortStruct);

   // ----------- построить массив Word ---------------------
   pText=&Text ;
   L=Text.length();
   Mass=(char *)Calloc( L,sizeof(char) );
   long j_Word=0 ;

   for( i=j=0 ; i<Text.length() ; i=Text.next_frasa(i) )
   {  
      char *Str=Text.get_frasa(i);
      Core.make_from1( Str );
      Core.make_from2( );
      t_ItemList &Item=Core.from();

      for( i1=0 ; i1<Item.j ; i1++ )
      {  t_fWord W ;

	 if( 0==Item[i1].exist )
	 {  W.index=j_Word+i1 ;
	    W.str  =Mass+j ;
	    W.i_text=i ;
	    strcpy( W.str,Item[i1].Str );
	    j+=strlen( W.str )+1 ;
	    Word.add( W );
	 }
      }
      j_Word+=Item.j+1 ;
   }
   
   // --------- построить массив sortWord ------------------
   sortWord=(t_sortWord *)Calloc(Word.j+1,sizeof(t_sortWord));
   sy_word =(long       *)Calloc(Word.j+1,sizeof(long));
   for( i=0 ; i<Word.j ; i++ )
   {
      sortWord[i].str    =Word[i].str ;
      sortWord[i].index  =i ;
   }
   qsort( sortWord,Word.j,sizeof(t_sortWord),wordcmp1 );
   for( i=0 ; i<Word.j ; i++ )
   {  sy_word[i]=sortWord[i].index ;
      sortWord[i].sy_word=i ;
   }
   for( i=j=1 ; i<Word.j ; i++ )
   {  if( 0!=strcmp( sortWord[i-1].str,sortWord[i].str ) )
         sortWord[j++]=sortWord[i] ;
   }
   if( 0<Word.j ) n_sortWord=j ;
       else       n_sortWord=0 ;
   sortWord[n_sortWord].sy_word=Word.j ;
   
   print_sortWord( "Debug_sortWord" );
   
   for( i=0 ; i<n_sortWord ; i++ )
      for( i1=sortWord[i].sy_word ; i1<sortWord[i+1].sy_word ; i1++ )
         Word[sy_word[i1]].i_s_word=i ;

   print_Word( "Debug_Word" );

   // --------- построить дерево выражений -----------------
   // --------- внести нулевой элемент ----
   t_WordTree WT ;
   
   WT.up      =-1 ;
   WT.down    = 0 ;
   WT.n_down  = Word.j ;
   WT.i_s_word=-1 ;
   WT.counter = 0 ;
   WT.counter1= 0 ;
   WordTree.add(WT);

   WT.up      = 0 ;
   WT.down    =-1 ;
   WT.n_down  = 0 ;

   // --------- внести заголовки выражений ---------
   for( i=0 ; i<n_sortWord ; i++ )
   {  WT.i_s_word=i ;
      WT.counter=sortWord[i+1].sy_word-sortWord[i].sy_word ;
      WordTree.add(WT);
   }
   // --------- внести остальные выражения ---------
   short d ; // длинна выражения
   long  z1, // индекс начала выражения в массиве Word
         i2, // номер слова внутри выражения
         z2 ;// индекс слова в WordTree, соответствующий i1

   for( d=1 ; d<=4 ; d++ )
   {  for( i=0 ; i<n_sortWord ; i++ )
      {  for( i1=sortWord[i].sy_word ; i1<sortWord[i+1].sy_word ; i1++ )
	 {  z1=sy_word[i1] ;
	    z2=Word[z1].i_s_word+1 ;
            for( i2=0 ; i2<d ; i2++ )
	    {  if( Word.j<=z1+i2+1 )
	           goto M_Break ;
	       if( 1<Word[z1+i2+1].index-Word[z1+i2].index )
	           goto M_Break ; // разрыв
               if( i2<d-1 )
 	       {   z2=find_down( z2, Word[z1+i2+1].i_s_word ); // какая-то фигня с этим find_down
	           if( z2<0 )
	               goto M_Break ; // ошибка???
	       }
	    }
            add_down( z2, Word[z1+i2].i_s_word );
	    M_Break: ;
	 }
      }
   }
   
   // --------- подсчет "реальной" частоты выражений -------
   for( i=0 ; i<WordTree.j ; i++ )
      WordTree[i].counter1=WordTree[i].counter ; //UMR
   for( i=WordTree.j-1 ; 0<i ; i-- )
   {  z1=WordTree[i].up ;
      WordTree[z1].counter1-=WordTree[i].counter ;
   }
   print_Tree( "Debug_Tree" );
   // --------- построение сортированного списка выражений -------
   sortStruct=(t_sortStruct *)Calloc(WordTree.j,sizeof(t_sortStruct));
   for( i=0 ; i<WordTree.j ; i++ )
   {  sortStruct[i].counter =WordTree[i].counter ;
      sortStruct[i].counter1=WordTree[i].counter1 ;
      sortStruct[i].i_tree  =i ;      
   }
   qsort( sortStruct,WordTree.j,sizeof(t_sortStruct),structcmp );
   // --------- построение sortStruct[i].str ----------------------
   char Str[200] ;
   for( i=0,j=0 ; i<WordTree.j-1 ; i++ )
   {  make_struct( sortStruct[i].i_tree, Str );
      j+=1+strlen( Str );
   }
   L=j+1 ;
   Mass1=(char *)Calloc( L,sizeof(char) );
   for( i=0,j=0 ; i<WordTree.j-1 ; i++ )
   {  make_struct( sortStruct[i].i_tree, Str );
      strcpy( Mass1+j,Str );
      sortStruct[i].str=Mass1+j ;
      j+=1+strlen( Str );
      if( L<=j )
          printf("Error t_NewWord1 :: make_list_words\n");
   }
}
/************************************************************************/
//             есть ли у вершины потомок этого типа
//    z_tree - индекс вершины
//    i_s_word - индекс слова-потомка
/************************************************************************/
long t_NewWord1 :: find_down( long z_tree, long i_s_word )
{  long i,z ;

   for( i=0 ; i<WordTree[z_tree].n_down ; i++ )
   {  z=WordTree[z_tree].down+i ;
      if( WordTree[z].i_s_word==i_s_word )
          return z ;
   }
   return -1 ;    
}
/************************************************************************/
//                   добавить вершине потомка
//    z_tree - индекс вершины
//    i_s_word - индекс слова-потомка
/************************************************************************/
void t_NewWord1 :: add_down( long z_tree, long i_s_word )
{  long z ;

   z=find_down( z_tree, i_s_word );
   if( 0<=z )
       WordTree[z].counter++ ;
   else
   {   t_WordTree WT ;

       if( WordTree[z_tree].down==-1 )
	   WordTree[z_tree].down=WordTree.j ;
       WT.up=z_tree ;
       WT.down    =-1 ;
       WT.n_down  = 0 ;
       WT.counter = 1 ;
       WT.counter1= 0 ;
       WT.i_s_word=i_s_word ;
       WordTree.add(WT);
       WordTree[z_tree].n_down++ ;
       char Str[200] ;
       make_struct( WordTree.j-1, Str );
       z=0 ;
   }
}
/************************************************************************/
//         сделать выражение на основе дерева выражений
/************************************************************************/
void t_NewWord1 :: make_struct( long z_tree, char *Str )
{  long i,j,z ;
   long stack[20] ;
    
   z=z_tree ;
   j=0 ;
   while( 1 )
   {  if( 20<=j ) break ;
      stack[j++]=WordTree[z].i_s_word ;
      z=WordTree[z].up ;
      if( z<=0 ) break ;
   }
   Str[0]=0 ;
   for( i=j-1 ; 0<=i ; i-- )
   {  strcat( Str,sortWord[stack[i]].str ); // UMR // SegFault
      if( 0<i ) strcat( Str," " );
   }
}
/************************************************************************/
void t_NewWord1 :: print_Word( char *File )
{
   FILE *fw=Fopen( File,"w" );
   for( long i=0 ; i<Word.j ; i++ )
      fprintf( fw,"\n %4d %4d %s",Word[i].index,Word[i].i_s_word,Word[i].str );
   Fclose( fw );
}
/************************************************************************/
void t_NewWord1 :: print_sortWord( char *File )
{
   FILE *fw=Fopen( File,"w" );
   for( long i=0 ; i<n_sortWord ; i++ )
      fprintf( fw,"\n %4d %4d %s",
               sortWord[i].index,sortWord[i].sy_word,sortWord[i].str );
   Fclose( fw );
}
/************************************************************************/
void t_NewWord1 :: print_Tree( char *File )
{
   FILE *fw=Fopen( File,"w" );
   fprintf( fw,"\n  i | up down n_down |  i_s   c1   c2");
   for( long i=0 ; i<WordTree.j ; i++ )
   {  t_WordTree *T=&WordTree[i] ;
      fprintf( fw,"\n %2d | %4d %4d %4d | %4d %4d %4d",
               i,T->up,T->down,T->n_down,T->i_s_word,T->counter,T->counter1 );
   }
   Fclose( fw );
}
/************************************************************************/
void t_NewWord1 :: set_pos( short _x1, short _y1, short _x2, short _y2 )
{
   t_Win :: set_pos( _x1, _y1, _x2, _y2 );
   wFrasa.x1=wWordList.x1=wNewWord.x1=x1 ;
   wFrasa.x2=wWordList.x2=wNewWord.x2=x2 ;
   wFrasa.y1=y1 ;
   wFrasa.y2=y1+3 ;
   wWordList.y1=wFrasa.y2+2 ;
//   wWordList.y2=y1+(y2-y1)/2 ;
   wWordList.y2=y2 ;
   wNewWord.y1=wWordList.y2+2 ;
   wNewWord.y2=y2 ;
}
/************************************************************************/

/************************************************************************/
void t_NewWord1 :: paint( )
{
   s_rame2_F( y1-1, x1-1, y2+1, x2+1, 0x0f );
   s_rame1_f( wWordList.y1-1, wWordList.x1-1, wWordList.y2+1, wWordList.x2+1, 0x0f );
   s_rame2_f( y1-1, x1-1, y2+1, x2+1, 0x0f );
   s_text_yxf( y1-1,x1+3,0x0b,Name );

   if( 0<=i_word && i_word<WordTree.j-1 )
   {   long  i,i_sy ;
       char  Str[200] ;

       i_sy=sortWord[WordTree[sortStruct[i_word].i_tree].i_s_word].sy_word ;
       i=Word[sy_word[i_sy+i_frasa]].i_text ;

       strcpy( Str,pText->get_frasa(i) ) ; // безобразие !!! рискую нарваться на переполнение
       paint_frasa( Str );
   }
   paint_wordlist(  );
   paint_hotkey( n_Line-1, 0, hk_Word1 );
}
/************************************************************************/
//           нарисовать фразу, в которой встретилось слово
/************************************************************************/
void t_NewWord1 :: paint_frasa( char *Str )
{  long i,i1,j,z,i_str,L ;

   L=strlen( Str );
   // --------- rasstavit perevody strok ---------------
   for( i=0 ; i<L ; i++ )
      if( Str[i]=='\n' ) Str[i]=' ' ;
   for( z=i=0 ; i<L ; i++ )
      if( 80<=(i-z) )
      {   for( i1=i ; z<=i1 ; i1-- )
             if( Str[i1]==' ' )
	     {   Str[i1]='\n' ; z=i1 ; break ;  }
      }
   // --------- narisovat frasu na ekrane ---------------
   for( z=i_str=i=0 ; i<=L ; i++ )
      if( Str[i]=='\n' || Str[i]==0 )
      {   s_text_yxfl( y1+i_str,x1,text_Color,i-z,Str+z );
          z=i+1 ;
	  i_str++ ;
      }
}
/************************************************************************/
//            нарисовать список новых слов
/************************************************************************/
void t_NewWord1 :: paint_wordlist(  )
{  long i,cnt,cnt1,n_Struct ;
   char f ;
   char Str[100] ;

   n_Struct=WordTree.j-1 ;
   for( i=i_begin ; i<n_Struct ; i++ )
   {  if( wWordList.y2-wWordList.y1<i-i_begin ) break ;
      if( i==i_word ) f= buf_Color ;
          else        f=text_Color ;
      cnt =WordTree[sortStruct[i].i_tree].counter  ;
      cnt1=WordTree[sortStruct[i].i_tree].counter1 ;
      sprintf( Str,"%2d %2d %s",cnt1,cnt,sortStruct[i].str );
      s_text_yxf( wWordList.y1+i-i_begin,x1,f,Str );
   }
}
/************************************************************************/

/************************************************************************/
e_WinMsg t_NewWord1 :: main_loop( t_Text &Text )
{  short key1,key2,ZZ ;
   short i1,n_frasa,n_screen ;

   make_list_words( Text );
   n_screen=wWordList.y2-wWordList.y1 ;

   while( 1 )
   {
      paint();

      s_getch( &key1,&key2 ) ;
      ZZ=s_shiftstatus();

      switch( key1 )
      {
         case '\r' : 
	             AddWord.make_str( sortStruct[i_word].str );
		     AddWord.loop();
	             break ;
	 case S_key_Esc:
		     return WM_ESC ;
         case 0 :
	 switch( key2 )
	 {
	    case S_key_Up :   
	         if( 0<i_word )        
		 {  i_word-- ; i_frasa=0 ;  }
		 break ;
	    case S_key_Down : 
	         if( i_word<WordTree.j-2 ) 
		 {  i_word++ ; i_frasa=0 ;  }
		 break ;
	    case S_key_PgUp :
	         i_word-=n_screen ;
		 if( i_word<0 ) i_word=0 ;
		 i_frasa=0 ;
		 break ;
	    case S_key_PgDn :
	         i_word+=n_screen ;
		 if( WordTree.j-1<=i_word ) i_word=WordTree.j-1 ;
		 i_frasa=0 ;
		 break ;		 
	    case S_key_Left : 
	         if( 0<i_frasa ) 
		     i_frasa-- ;
		 break ;
	    case S_key_Right : 
                 i1=WordTree[sortStruct[i_word].i_tree].i_s_word ;
                 n_frasa=sortWord[i1+1].sy_word-sortWord[i1].sy_word ;
		 if( i_frasa<n_frasa-1 ) 
		     i_frasa++ ;
		 break ;
             case S_key_F1 :
	         Window_help( (void *)"newwords1.html") ; 
                 break ;
	    case S_key_F10 :  return WM_ESC ;
	 }
      }
      if( i_word<i_begin ) i_begin=i_word ;
      if( i_begin+n_screen<i_word ) i_begin=i_word-n_screen ;
   }
    
    return WM_ESC ;
}
/************************************************************************/
//          преобразовать индекс sortStruct в индекс sy_word
/************************************************************************/
long t_NewWord1 :: sort_to_word( long i )
{
   return sortWord[WordTree[sortStruct[i].i_tree].i_s_word].sy_word ;
}

DEFINE_LIST_BTYPE( t_fWord,t_fWordList )
DEFINE_LIST_BTYPE( t_WordTree,t_WordTreeList )

