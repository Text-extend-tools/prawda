# ifndef WORD1_H
# define WORD1_H

struct t_fWord
{  char *str ;         // строка (собственно слово)
   long  index ;       // индекс слова в тексте (нужно для определения разрыва выражений)
   long  i_s_word ;    // ссылка на sortWord
   long  i_text ;      // из какого места текста взято это слово
} ;
DECLARE_LIST_TYPE( t_fWord,t_fWordList );

struct t_sortWord
{  char *str ;
   long  index ;       // новое поле
   long  sy_word ;     // ссылка в sy_word
} ;

struct t_WordTree
{  long up,down,n_down ;
   long i_s_word ;      // ссылка в sortWord
   long counter,        // счетчик
        counter1 ;      // 
};
DECLARE_LIST_TYPE( t_WordTree,t_WordTreeList );

struct t_sortStruct
{  short  counter,
          counter1 ;    // 
   short  i_tree ;
   char  *str ;
};

class t_NewWord1 : public t_Win
{
   t_Text         *pText ;    // из какого текста это сделано
   t_fWordList     Word ;     // массив непереведенных слов
   t_sortWord     *sortWord ; // массив слов, отсортированных по алфавиту
   long            n_sortWord ;
   long           *sy_word ;  // ссылки на sortWord->Word (размерность Word.j)
   t_WordTreeList  WordTree ; // дерево (лес) выражений
   t_sortStruct   *sortStruct ;// сортированный список структур (размерность WordTree.j)
   char           *Mass ;
   char           *Mass1 ;
// ------------------------------
   t_Rect          wFrasa,    // окно фразы
                   wWordList, // окно списка слов
		   wNewWord ; // окно ввода новых выражений

   long            i_word,    // индекс текущего слова
                   i_begin,   // первое слово, видимое в списке слов
		   i_frasa ;  // индекс текущей фразы
public:
          t_NewWord1();
	  ~t_NewWord1();
   void   make_list_words( t_Text &Text );
   long   find_down( long z_tree, long i_s_word );
   void   add_down( long z_tree, long i_s_word );
   void   make_struct( long z_tree, char *Str );

   void   print_Word( char *File );
   void   print_sortWord( char *File );
   void   print_Tree( char *File );

   void     set_pos( short x1, short y1, short x2, short y2 );
   void     paint( );
   void     paint_frasa( char *Str );
   void     paint_wordlist(  );
   e_WinMsg main_loop( t_Text &Text );
   long     sort_to_word( long i );
};

# endif
