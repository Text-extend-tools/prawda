/*******************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                 Часть моего текстового редактора
//     Анисимов Д.В.                               сегодня
/*******************************************************************/

# include <string.h>
# include <stdlib.h>

# include "mylib.h"
# include "edit.h"
# include "video.h"

# define max(a,b)    (((a) > (b)) ? (a) : (b))
# define min(a,b)    (((a) < (b)) ? (a) : (b))
# define size_Tab    8
# define max_Length  1000000L

static char *Empty="" ;

/***************************************************************************/
/*                                                                         */
/***************************************************************************/
t_Text :: t_Text( void )
{
   Length = 0 ;
   Text = (char *)Calloc( max_Length,1 );

   i_str=0 ; i_pos=0 ;
   str_Txt = 1 ;
   is_const=NULL ;
   fformat =0 ;
}
/***************************************************************************/
t_Text :: ~t_Text( void )
{
   Free( Text );
}
/***************************************************************************/
void t_Text :: init( char *_Text )
{  long i,j ;

   for( i=0 ; i<max_Length ; i++ )
      Text[i]=0 ;
   str_Txt=1 ;
   for( i=j=0 ; i<max_Length ; i++ )
   {  if( _Text[i]==0 ) break ;
      if( _Text[i]!='\r' ) Text[j++] = _Text[i] ;
      if( _Text[i]=='\n' ) str_Txt++ ;
   }
   Length=j ;
   i_str=i_pos=0 ;
}
/***************************************************************************/
void t_Text :: clear( void )
{
   Length  = 0 ;
   i_str   = 0 ;
   i_pos   = 0 ;
   str_Txt = 1 ;
}
/***************************************************************************/
void t_Text :: write_file( char *Name, char f_cp1251 )
{  FILE *fw=NULL ;

try
{  fw=Fopen( Name,"wb" );
   if( f_cp1251==1 )
   {
      char  *T1=(char *)Calloc(Length,sizeof(char));
      uchar *TC=Koi_to_Win() ;
      for( long i=0 ; i<Length ; i++ )
	 T1[i]=(char )TC[ (uchar)(Text[i]) ] ;
      Fwrite( T1,Length,sizeof(char),fw );
   }
   else
   {  Fwrite( Text,Length,sizeof(char),fw );
   }
   Fclose( fw );
}
catch( int E )
{  Fclose( fw );  }
}
/***************************************************************************/
char t_Text :: read_file( char *Name, char f_cp1251 )
{  long i,i1,j,L ;
   FILE *fr=NULL ;
   char *m=NULL ;

try
{
   fr=Fopen( Name,"rb" );
   L=FileLength( fr );
   if( max_Length<L )
   {  error_set( "Файл слишком велик.\nВсе равно за один раз столько не переведете." );
      throw(-1) ;
   }
   m=(char *)Calloc(L,sizeof(char));
   Fread( m,L,sizeof(char),fr );
   str_Txt=1 ;
   for( i=j=0 ; i<L ; i++ )
   {  if( m[i]=='\t' )
      {   for( i1=0 ; i1<size_Tab ; i1++ )
          Text[j++]=' ' ;
	  continue ;
      }
      if( m[i]!='\r' ) Text[j++]=m[i] ;
      if( m[i]=='\n' ) str_Txt++ ;
   }
   Length=j ;
   for( i=j ; i<max_Length ; i++ )
      Text[i]=0 ;
   if( f_cp1251==1 )
   {  uchar *TC=Win_to_Koi() ;
      for( i=0 ; i<Length ; i++ )
	 Text[i]=(char )TC[ (uchar)(Text[i]) ] ;
   }
   Free(m);
   Fclose( fr );
   i_str=i_pos=0 ;
   return 0 ;
}
catch( int E )
{  Free( m );
   Fclose( fr );   
   throw( E ) ;
}
}
/***************************************************************************/
char * t_Text :: get_str( long i_Str, char *Buf )
{  char *s ;
   short i ;
   
   s=get_str( i_Str );
   for( i=0 ; i<256 ; i++ )
      Buf[i]=' ' ;
   for( i=0 ; i<256 ; i++ )
   {  if( s[i]==0 || s[i]=='\n' ) break ;
      Buf[i]=s[i] ;
   }      
   return Buf ;
}
/***************************************************************************/
char * t_Text :: get_str( long i_Str )
{  long p ;

   if( i_Str<0 || str_Txt<=i_Str ) return Empty ;
   p=pos_str( i_Str );   
   if( Length<=p ) return Empty ;
   return Text+p ;
}
/***************************************************************************/
long t_Text :: get_pos_xy( long py, long px )
{  long i,p ;

   if( py<0 ) return 0 ;
   if( str_Txt<=py ) return Length ;
   p=pos_str( py );
   for( i=0 ; i<px ; i++ )
   {  if( Length<=p+i ) break ;
      if( Text[p+i]=='\n' ) {  i++ ; break ;  }
   }
   return p+i ;
}
/***************************************************************************/
void t_Text :: get_xy_pos( long pos1, long *_py, long *_px )
{  long i ;
   long px,py ;

   if( pos1<0 ) {  *_py=0; *_px=0 ; return ;  }
   if( Length<=pos1 ) 
   {  *_py=str_Txt-1;
      for( i=1 ; i<Length ; i++ )
         if( Text[Length-i]=='\n' ) break ;
      *_px=i ;
      return ;
   }
   px=pos1 ; py=0 ;
   for( i=0 ; i<=pos1 ; i++ )
      if( Text[i]=='\n' ) {  py++ ; px=pos1-i-1 ;  }
   *_py=py ;
   *_px=px ;
}
/***************************************************************************/
//                    вернуть индекс начала строки
/***************************************************************************/
long t_Text :: pos_str( long i_Str )
{
   long i,i_str1 ;

   if( i_Str==0 ) return 0 ;
   for( i=i_str1=0 ; i<Length ; i++ )
   {
      if( Text[i]=='\n' )
      {  i_str1++ ;
	 if( i_str1==i_Str ) return i+1 ;
      }
   }
   return Length ;
}
/***************************************************************************/
//    вставить строку в позицию (предыдущая строка исчезает)
/***************************************************************************/
void t_Text :: put_str_ipos( long i1_pos, char *str )
{  long l ;
   long i,j,i1,i2 ;

   // ---- вычислить l - длинну строки
   for( l=0 ; l<256 ; l++ )
      if( str[l]==0 || str[l]=='\n' ) break ;
   j=l ;
   for( i=l=0 ; i<j ; i++ )
      if( str[i]!=' ' ) l=i+1 ;

   // ---- проверить позицию на легальность
   if( i1_pos<0 || Length<i1_pos ) throw(-1);

   // ---- найти i1 - начало строки
   for( i1=i1_pos-1 ; 0<=i1 ; i1-- )
      if( Text[i1]=='\n' ){  i1++ ; break ;  }
   if( i1<0 ) i1=0 ;

   // ---- найти i2 - конец строки
   for( i2=i1_pos ; i2<Length ; i2++ )
      if( Text[i2]=='\n' ){  i2++ ; break ;  }

   if( l+1 <= i2-i1 )
   {  // ---- если вставляемая строка короче предыдущей
      for( i=0,j=i1 ; i<l ; i++ )
	 Text[j++]=str[i] ;
      Text[j++]='\n' ;
      for( i=i2 ; i<Length ; i++ )
	 Text[j++]=Text[i] ;
   }
   else
   {  // ---- если вставляемая строка длиннее предыдущей
      for( i=Length-i2-1 ; 0<=i ; i-- )
	 Text[i1+l+1+i]=Text[i2+i] ;
      for( i=0,j=i1 ; i<l ; i++ )
	 Text[i1+i]=str[i] ;
      Text[i1+l]='\n' ;
   }
   Length+=(l+1)-(i2-i1);
   if( i1_pos<i_pos ) i_pos+=(l+1)-(i2-i1) ;
}
/***************************************************************************/
void t_Text :: put_str_istr( long i_Str, char *str )
{  long pos ;

   pos=pos_str( i_Str );
   put_str_ipos( pos, str );
}
/***************************************************************************/
//                 выдать слово в текущей позиции
/***************************************************************************/
char * t_Text :: get_word_punkt( long z )
{  long i,j ;
   char c ;
   static char Str[1000]="" ;

   if( Length<=z ) return Str ;

   // ---- найти  начало слова
   for(  ; 0<=z ; z-- )
      if( is_space(Text[z]) )  break ;

   z++ ;
   // ---- заполнить возвращаемую строку
   for( j=0,i=z ; i<Length ; i++ )
   {  c=Text[i] ;
      if( is_space(c) ) break ;
      Str[j++]=c ;
   }
   Str[j++]=0 ;
   return Str ;
}
/***************************************************************************/
long t_Text :: prev_word( long z )
{  char c ;

   for(  ; 0<=z ; z-- )
      if( is_space(Text[z]) )  break ;
   for(  ; 0<=z ; z-- )
      if( !is_space(Text[z]) )  break ;

   if( z<0 ) z=0 ;
   return z ;
}
/***************************************************************************/
long t_Text :: next_word( long z )
{  char c ;

   for(  ; z<Length ; z++ )
      if( is_space(Text[z]) )  break ;
   for(  ; z<Length ; z++ )
      if( !is_space(Text[z]) )  break ;

   if( Length<=z ) z=Length-1 ;
   return z ;   
}
/***************************************************************************/
//                 выдать фразу в текущей позиции
/***************************************************************************/
char * t_Text :: get_frasa( long pos )
{  long i,j,z1,z2 ;
   static char Str[1000]="" ;

   if( Length<=pos ) return Str ;

   z1=begin_frasa( pos );
   z2=end_frasa( pos ) ;
   for( i=z1,j=0 ; i<=z2 && j<999 ; i++ )
      Str[j++]=Text[i] ;
   Str[j++]=0 ;
   return Str ;
}
/***************************************************************************/
//                 вставить фразу в текущую позицию
/***************************************************************************/
void t_Text :: put_frasa( long z, char *str )
{  long  i,j,i1,i2,l ;

   l=strlen( str );
   if( Length<=z )
   {   Text[Length++]='\n' ;
       for( i=0 ; i<l ; i++ )
          Text[Length++]=str[i] ;
       goto M_End ;
   }
   // ---- найти i1 - начало фразы
   i1=begin_frasa( z ) ;
   i2=end_frasa( z );
   if( i2<z ) i1=i2=z ; // только что вставил (вставка после последней фразы)
   if( i1<0 ) i1=0 ;
   if( Text[i1]=='\n' ) i1++ ;

   if( l <= i2-i1 )
   {  // ---- если вставляемая строка короче предыдущей
      for( i=0,j=i1 ; i<l ; i++ )
	 Text[j++]=str[i] ;
      for( i=i2 ; i<Length ; i++ )
	 Text[j++]=Text[i] ;
      Text[j]=0 ;
   }
   else
   {  // ---- если вставляемая строка длиннее предыдущей
      for( i=Length-i2-1 ; 0<=i ; i-- )
	 Text[i1+l+i]=Text[i2+i] ;
      for( i=0,j=i1 ; i<l ; i++ )
	 Text[i1+i]=str[i] ;
   }
   Length+=l-(i2-i1);
   Text[Length]=0 ;
   
   M_End :
   for( str_Txt=1,i=0 ; i<Length ; i++ )   
      str_Txt+=(Text[i]=='\n') ;
}
/***************************************************************************/
//                          предыдущая фраза
/***************************************************************************/
long t_Text :: prev_frasa( long pos )
{  long  i ;

   i=begin_frasa( pos );
   i=begin_frasa( i-1 );
   if( i<0 ) i=0 ;
   return i ;
}
/***************************************************************************/
//                       последующая фраза
/***************************************************************************/
long t_Text :: next_frasa( long pos )
{  long i,z ;

   z=end_frasa( pos ) ;
   if( fformat==0 )
   {   return z+1 ;
   }
   else
   {
      for( i=z ; i<Length-4 ; i++ )
	 if( 0==strncmp(Text+i,"\\@@",3) || 0==strncmp(Text+i,"\\##",3) )
             return i ;
      return Length ;
   }
}
/***************************************************************************/
//                          начало фразы
/***************************************************************************/
long t_Text :: begin_frasa( long pos )
{  long  i ;
   char  c,c1 ;

   if( fformat==0 )
   {  // ----- просто текст -----------
      for( i=pos ; 0<=i ; i-- )
      {  c=Text[i] ;
	 if( c=='.' || c=='!' || c=='?' )
	 {  c1=Text[i+1] ;
	    if( is_space(c1) ) return i+1 ;
	 }
	 /*
	 {   if( is_const==NULL ) return i+1 ;
             if( 0==is_const( get_word_punkt(i) ) ) return i+1 ;
	 }
	 */
      }
      return 0 ;   
   }
   else
   {  // ----- текст с разметкой ------
      for( i=pos ; 0<=i ; i-- )
	 if( 0==strncmp(Text+i,"\\@@",3) || 0==strncmp(Text+i,"\\##",3) ) 
             break ;
      if( i<0 ) i=0 ;
      return i ;   
   }
}
/***************************************************************************/
//                          конец фразы
/***************************************************************************/
long t_Text :: end_frasa( long pos )
{  long  i,z ;
   char  c,c1,f=0 ;

   if( fformat==0 )
   {  // ----- просто текст -----------
      for( i=pos ; i<Length ; i++ )
      {  c=Text[i] ;
	 if( c=='.' || c=='!' || c=='?' ) 
	 {  c1=Text[i+1] ;
	    if( is_space(c1) ) return i+1 ;
	 }
         /*
	 {   if( is_const==NULL ) return i+1 ;
             if( 0==is_const( get_word_punkt(i) ) ) return i+1 ;
	 }
	 */
      }
      return Length ;
   }
   else
   {  // ----- текст с разметкой ------
      z=begin_frasa( pos );
      for( i=z ; i<Length ; i++ )
	 if( Text[i]=='{' ) break ;
      for( i++ ; i<Length ; i++ )
      {  c=Text[i] ;
	 if( c=='{' ) f++ ;
	 if( c=='}' )
	 {  if( f<=0 ) break ;
            f-- ;
	 }
      }
      return i+1 ;
   }
}
/***************************************************************************/
//     извлечь из текста строки с Str1 по Str2 с удалением из текста
/***************************************************************************/
long t_Text :: get_text1( long Str1, long Str2, char *List )
{
   long  i,pos1,pos2,L=0 ;

   pos2=pos_str( Str2+1 );
   pos1=pos_str( Str1 );

   L=pos2-pos1 ;
   for( i=0 ; i<L ; i++ )
      List[i]=Text[pos1+i] ;

   Length-=L ;
   for( i=pos1 ; i<Length ; i++ )
      Text[i]=Text[i+L] ;

   for( i=0,str_Txt=1 ; i<Length ; i++ )
      if( Text[i]=='\n' ) str_Txt++ ;
   return pos2-pos1 ;
}
/***************************************************************************/
//     извлечь из текста строки с Str1 по Str2 без удаления из текста
/***************************************************************************/
long t_Text :: get_text2( long Str1, long Str2, char *List )
{
   long  i,pos1,pos2,L=0 ;

   pos2=pos_str( Str1 );
   pos1=pos_str( Str2 );

   L=pos2-pos1 ;
   for( i=0 ; i<L ; i++ )
      List[i]=Text[pos1+i] ;

   return L ;
}
/***************************************************************************/
//                
/***************************************************************************/
void t_Text :: put_text1( long Str, char *List, long l_List )
{
   long  i,pos1 ;

   pos1 =pos_str( Str );

   for( i=Length-1 ; pos1<=i ; i-- )
      Text[i+l_List]=Text[i] ;
   Length+=l_List ;
   for( i=0 ; i<l_List ; i++ )
      Text[pos1+i]=List[i] ;
   for( i=0,str_Txt=1 ; i<Length ; i++ )
      if( Text[i]=='\n' ) str_Txt++ ;
}
/***************************************************************************/
//                вставить фразу взамен cуществующей
/***************************************************************************/
void t_Text :: put_text1p( long Pos1, long Pos2, char *Str ) 
{
   long i,L,D ;
   char f ;
   
   L=strlen( Str );
   if( Str[L-1]==' ' ) Str[L-1]='.' ;
   D=L-(Pos2-Pos1)+1 ;

   if( 0<D )
   {   // ---------- раздвинуть текст --------------
       for( i=Length ; Pos1<=i ; i-- )
	  Text[i+D]=Text[i] ;
       for( i=0 ; i<D ; i++ )
          Text[Pos1+i]=' ' ;
       Length+=D ;
   }
   if( D<0 )
   {   // ---------- сдвинуть текст ----------------
       for( i=Pos1 ; i<Length ; i++ )
	  Text[i]=Text[i-D] ;
       Length+=D ;
   }
   // ----------- вставить фразу ----------------
   // ---- танцы с f - чтобы сохранить переводы строк ----
   for( i=0,f=0 ; i<L ; i++ )
   {  //if( Text[Pos1+i]=='\n' ) f=1 ;
      Text[Pos1+i]=Str[i] ;
      //if( f==1 && Str[i]==' ' )
      //{   Text[Pos1+i]='\n' ; f=0 ;  }
   }
   
}
/***************************************************************************/
//                       вставить пустую строку
/***************************************************************************/
void t_Text :: insert_str( long PY )
{  short i,j ;

   j=pos_str( PY );
   if( max_Length<Length )
   {  printf("\n t_Text :: insert_str Length>=max_Length !!!" ); exit(-1) ; }
   for( i=Length ; j<i ; i-- )
	 Text[i]=Text[i-1] ;
   Text[j]='\n' ; Length++ ;
   str_Txt++ ;
}
/***************************************************************************/
void t_Text :: delete_str( long PY )
{  long j,j1 ;

   j=pos_str( PY );
   if( Length<=j ) return ;
   for( j1=j ; j1<Length ; j1++ )
      if( Text[j1]=='\n' ) break ;
   for( j1++ ; j1<Length ; j1++ )
      Text[j++]=Text[j1] ;   
   Length=j ;
   str_Txt-- ;
}
/***************************************************************************/
short t_Text :: max_length_str( void )
{  short i,j,L=0 ;

   for( i=j=0 ; i<Length ; i++ )
      if( Text[i]=='\n' )
      {  L=max( L,i-j ); j=i;  }
   L=max( L,i-j );
   return L ;
}
/***************************************************************************/
long t_Text :: length( void )
{  return Length ;  }
/************************************************************************/
long t_Text :: str_txt( )
{  return str_Txt ;  }
/***************************************************************************/
char t_Text :: operator []( long i )
{  return Text[i] ; }
