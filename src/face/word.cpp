/*******************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                   Ввод новых слов и выражений
//      Анисимов Д.В.                               сегодня
/*******************************************************************/

# include <string.h>
# include "video.h"
# include "color.h"
# include "face.h"
# include <word.h>

extern t_NewWord NewWord ;
extern char      MercuryDir[] ;

e_WinMsg line_edit( short x, short y, char f, char *Str, short Length );

/************************************************************************/
t_NewWord :: t_NewWord( )
{  Mass=NULL ;
   j_Mass=n_Mass=z_Mass=0 ;
}
/************************************************************************/
t_NewWord :: ~t_NewWord( )
{  Free( Mass );
}
/************************************************************************/
//           добавить к массиву новых слов очередное слово
//           (для пополнения словаря)
/************************************************************************/
void t_NewWord :: add( char *str )
{  long  L,N ;
   char *M ;

   L=strlen( str );
   if( n_Mass<j_Mass+L+2 )
   {   N=max(n_Mass/4,L+2);
       M=(char *)realloc( Mass,(n_Mass+N)*sizeof(char) );
       if( M==NULL ) return ;
       Mass=M ; n_Mass+=N ;
   }
   strcpy( Mass+j_Mass,str );
   strcpy( Mass+j_Mass+L,"\n" );
   j_Mass+=L+1 ;
}
/************************************************************************/
//                  выдать массив новых слов
/************************************************************************/
char * t_NewWord :: get_all( void )
{
   return Mass ;
}
/************************************************************************/
//                      выдать новое слово
/************************************************************************/
char * t_NewWord :: get_new( void )
{
   return Mass+z_Mass ;
}
/************************************************************************/
void t_NewWord :: remember( )
{
    z_Mass=j_Mass ;
}
/************************************************************************/
int tagsort1( const void *a, const void *b )
{  short i1,i2 ;
   char  s1[3],s2[3] ;
   t_Format1  *FF=&Grammar.format1()[0] ;

   i1=*(short *)a ;
   i2=*(short *)b ;
   strcpy( s1,FF->get_tag(0,i1) );
   strcpy( s2,FF->get_tag(0,i2) );
   return strcmp( s1,s2 );
}
/************************************************************************/
int tagsort2( const void *a, const void *b )
{  short i1,i2 ;
   char  s1[3],s2[3] ;
   t_Format1  *FF=&Grammar.format1()[0] ;

   i1=*(short *)a ;
   i2=*(short *)b ;
   strcpy( s1,FF->get_tag(1,i1) );
   strcpy( s2,FF->get_tag(1,i2) );
   return strcmp( s1,s2 );
}
/************************************************************************/
t_AddWord :: t_AddWord()
{
   sStr [0]=dStr [0]=0 ;
   sStr1[0]=dStr1[0]=0 ;
   sStr2[0]=dStr2[0]=0 ;
   j_Sou=j_Dst=0 ;
   s_i_struct=d_i_struct=-1 ;
}
/************************************************************************/
//   рассортировать структуры на головные и телесные
/************************************************************************/
void t_AddWord :: init( void )
{
   short  i,i_struct ;
   e_Type t ;
   
   FF=&Grammar.format1()[0] ;

   sWord.j=dWord.j=sStruct.j=dStruct.j=0 ;
   
   sWord.add( i_struct=-1 );
   for( i=0 ; i<FF->SouPart.j ; i++ )
   {  i_struct=FF->SouPart[i].i_part ;
      t=Grammar[i_struct].From.type ;
      if( t==TNULL || t==TCONST || t==TCONST1 || t==TWORD0 ) continue ;
      if( t==TWORD || t==TSTRUCT || t==TSELECT || t==TSELECT1 || t==TSELECT2 )
      {   sWord.add( i_struct );  }
      if( t==TWORD || t==TSTRUCT1 || t==TSTRUCT2 )
      {   sStruct.add( i_struct );  }
   }
   qsort( sWord.list  , sWord.j   ,sizeof(short),tagsort1 );
   qsort( sStruct.list, sStruct.j ,sizeof(short),tagsort1 );

   dWord.add( i_struct=-1 );
   for( i=0 ; i<FF->DstPart.j ; i++ )
   {  i_struct=FF->DstPart[i].i_part ;
      t=Grammar[i_struct].To.type ;
      if( t==TNULL || t==TCONST || t==TCONST1 || t==TWORD0 ) continue ;
      if( t==TWORD || t==TSTRUCT || t==TSELECT || t==TSELECT1 || t==TSELECT2 )
      {   dWord.add( i_struct );  }
      if( t==TWORD || t==TSTRUCT1 || t==TSTRUCT2 )
      {   dStruct.add( i_struct );  }
   }
   qsort( dWord.list  , dWord.j   ,sizeof(short),tagsort2 );
   qsort( dStruct.list, dStruct.j ,sizeof(short),tagsort2 );
}
/************************************************************************/
void t_AddWord :: init1( void )
{
// sStr [0]=0 ;
   dStr [0]=0 ;
   sStr1[0]=dStr1[0]=0 ;
   sStr2[0]=dStr2[0]=0 ;
   j_Sou=j_Dst=0 ;
   s_i_struct=d_i_struct=-1 ;
}
/************************************************************************/
//                   задать переводимую структуру
/************************************************************************/
void t_AddWord :: make_str( char *S )
{
   strcpy( sStr,S );
}
/************************************************************************/
//                разбить фразу на слова, сделать Sou
/************************************************************************/
void t_AddWord :: make_str1( char to )
{  short    i,j,j1,j_Str ;
   char    *sdStr,*sdStr1 ;
   t_sWord *WW ;
   char     Str[100] ;

   if( to==0 ){  sdStr =sStr ; sdStr1=sStr1 ;  WW=&Sou[0] ;  }
       else   {  sdStr =dStr ; sdStr1=dStr1 ;  WW=&Dst[0] ;  }

   for( i=j=j1=j_Str=0 ; i<200 ; i++ )
   {  
      if( (sdStr[i]==' ' || sdStr[i]==0) && 0<j )
      {  Str[j]=0 ;
         WW[j1].i_struct=-1 ;
	 WW[j1++].str   =sdStr1+j_Str ;
	 strcpy( sdStr1+j_Str,Str );
	 sdStr1[j_Str+j]=0 ;
	 j_Str+=j+1 ;
         j=0 ;
	 if( sdStr[i]==0 ) break ;
	 continue ;
      }    
      Str[j++]=sdStr[i] ;   
   }
   if( to==0 ) j_Sou=j1 ;
       else    j_Dst=j1 ;
}
/************************************************************************/
//   сделать строку с тегами
/************************************************************************/
void t_AddWord :: make_str2( void )
{
   long  i ;
   t_Format1  FFF=Grammar.format1()[0] ;
   char  Str1[100] ;
   
   sStr2[0]=0 ;
   for( i=0 ; i<j_Sou ; i++ )
   {
      sprintf( Str1,"%s[%s]", FFF.get_tag( 0,Sou[i].i_struct ),Sou[i].str );
      strcat( sStr2,Str1 ); strcat( sStr2," " );
   }
}
/************************************************************************/
//  нарисовать теги и структуры
/************************************************************************/
void t_AddWord :: paint_struct( t_shortList &List, char to, short current )
{  short i,i1,begin,size_y1,i_struct ;
   char  f,*s ;
   t_Struct *SS ;
   char  Str[100] ;

   size_y1=y2-y1-11 ;
   begin=0 ;
   if( begin+size_y1<=current ) begin=current-size_y1 ;
   for( i=0 ; begin+i<List.j && i<=size_y1 ; i++ )
   {  if( begin+i==current ) f=0xf0 ; else f=0x0f ;
      i_struct=List[begin+i] ;
      if( 0<=i_struct )
      {	  if( to==0 ) SS=&Grammar[i_struct].From ;
              else    SS=&Grammar[i_struct].To ;
      }
      if( i_struct<0 ) s="константа" ; else s=SS->Name ;
      sprintf( Str," %2s  %s",FF->get_tag(to,i_struct),s );
      if( 0<=i_struct )
      {	 if( SS->type==TSTRUCT1 )
	 {   strcat( Str, " =" );
	     for( i1=0 ; i1<SS->Word.j ; i1++ )
	     {  strcat( Str, " " );
        	strcat( Str, FF->get_tag(to,SS->Word[i1].i_struct) );
	     }
	 }
      }
      s_text_yxf( y1+10+i,3,f,Str );
   }
}
/************************************************************************/
void t_AddWord :: paint( void )
{
   t_Win::paint();
   s_rame2_F( y1-1, x1-1, y2+1, x2+1, 0x0f );
   s_text_yxf( y1-1,x1+3 ,name_Color, Name );

   s_rame2_F ( y1  ,x1   ,y1+2, x2, 0x0f );
   s_text_yxf( y1  ,x1+3 ,0x0f, "Выражение источника" );
   if( Reg<=2 ) s_text_yxf( y1+1,x1+3 ,0x0f, sStr );
       else     s_text_yxf( y1+1,x1+3 ,0x0f, sStr2 );
   s_rame2_F ( y1+3,x1   ,y1+5, x2, 0x0f );
   s_text_yxf( y1+3,x1+3 ,0x0f, "Выражение приемника" );
   s_text_yxf( y1+4,x1+3 ,0x0f, dStr );
   s_rame2_F ( y1+6,x1   ,y1+8, x2, 0x0f );
   s_text_yxf( y1+6,x1+3 ,0x0f, "Предполагаемый тег" );
   s_rame2_F ( y1+9,x1   ,y2, x2, 0x0f );
   s_text_yxf( y1+9,x1+3 ,0x0f, "Часть речи" );
   paint_hotkey( n_Line-1, 0, hk_Word );
}
/************************************************************************/
void t_AddWord :: paint_from( long i_Word )
{  short i,z ;
   char  Str[100] ;
   uchar  f[16]={ 0xf0 };

   z=0 ;
   sprintf( sStr2,"%2s:",FF->get_tag( 0,s_i_struct ) );
   for( i=0 ; i<j_Sou ; i++ )
   {  if( i_Word==i+1 ) z=strlen(sStr2)+1 ;
      sprintf(Str," %2s[%s]",FF->get_tag( 0, Sou[i].i_struct ),Sou[i].str );
      strcat( sStr2,Str );
   }
   s_text_yxf( y1+1, x1+3 ,0x0f, sStr2 );
   s_color_yxt( y1+1, x1+3+z, f , "00" );
}
/************************************************************************/
void t_AddWord :: paint_to( long i_Word )
{  short i,z ;
   char  Str[100] ;
   uchar  f[16]={ 0xf0 };

   z=0 ;
   sprintf( dStr2,"%2s:",FF->get_tag( 1,d_i_struct ) );
   for( i=0 ; i<j_Dst ; i++ )
   {  if( i_Word==i+1 ) z=strlen(dStr2)+1 ;
      sprintf(Str," %2s[%s]",FF->get_tag( 1, Dst[i].i_struct ),Dst[i].str );
      strcat( dStr2,Str );
   }
   s_text_yxf( y1+4, x1+3 ,0x0f, dStr2 );
   s_color_yxt( y1+4, x1+3+z, f , "00" );
}
/************************************************************************/
e_WinMsg t_AddWord :: loop( void )
{
   e_WinMsg r ;
   
   Reg=1 ;
   init(  );
   init1(  );

   while( 1 )
   {
      paint( );
      switch( Reg )
      {
         case 1 : r=line_edit( x1+1,y1+1,0x1f,sStr,x2-x1-1 ); 
	          make_str1( 0 ); break ;
	 case 2 : r=tag_edit1( 0 ); break ;
	 case 3 : paint_from( -1 );
	          r=line_edit( x1+1,y1+4,0x1f,dStr,x2-x1-1 ); 
	          make_str1( 1 ); break ;
	 case 4 : r=tag_edit1( 1 ); break ;
      }
      switch( r )
      {
	 case WM_NEXT : case WM_OK :
            if( Reg==4 ) goto M_End ;
	        else  Reg++ ;
            break ;
         case WM_PREV : case WM_ESC :
            if( Reg==1 ) return WM_ESC ;
	        else  Reg-- ;
            break ;
      }
   }
M_End:

   NewWord.add( antwort() );
   message("Идет обновление словаря.\n        Ждите...");

   //Perevod.add_new( filename( MercuryDir,"dicts") ,NewWord.get_all() );
   Perevod.add_new1( antwort() );
   return WM_OK ;
}
/************************************************************************/
e_WinMsg line_edit( short x, short y, char f, char *Str, short Length )
{  e_WinMsg r ;


M1:r=kommand_yxf( y, x, f, Str, Length, 0x01, NULL, 0 );

   switch( r )
   {  case WM_HELP : Window_help( (void *)"newwords.html") ; goto M1;
      case WM_PREV : case WM_NEXT : case WM_ESC : case WM_OK :
                     return r ;
   }
   return WM_ESC ;
}
/************************************************************************/
e_WinMsg t_AddWord :: tag_edit1( char to )
{
   short       i,i_struct,*sd_i_struct,i_Word,n_Word ; 
   t_shortList *Struct ; 
   t_shortList *Word ; 
   t_sWord     *WW ;
   t_Struct    *SS ;
   e_WinMsg     r ;
   e_Type       t ;

   if( 0==to  ) 
   {  n_Word= j_Sou ;      
      Struct=&sStruct ;
      Word  =&sWord ; 
      WW    = Sou ;
      sd_i_struct=&s_i_struct ;

      sStruct1.j=0 ;
      for( i=0 ; i<sStruct.j ; i++ )
      {  t=Grammar[sStruct[i]].From.type ;
	 if( (t==TSTRUCT1 && Grammar[sStruct[i]].From.Word.j==n_Word) ||
             (t==TWORD    && n_Word==1 ) || t==TSTRUCT2 )
             sStruct1.add( sStruct[i] );
      }
      Struct=&sStruct1 ;
   }
   else
   {  n_Word= j_Dst ;
      Struct=&dStruct ;
      Word  =&dWord ;
      WW    = Dst ;
      sd_i_struct=&d_i_struct ;

      dStruct1.j=0 ;
      for( i=0 ; i<dStruct.j ; i++ )
      {  t=Grammar[dStruct[i]].To.type ;
	 if( (t==TSTRUCT1 && Grammar[dStruct[i]].To.Word.j==n_Word) ||
             (t==TWORD    && n_Word==1 ) || t==TSTRUCT2 )
             dStruct1.add( dStruct[i] );
      }
      Struct=&dStruct1 ;
      if( Grammar[s_i_struct].From.type==TWORD )
          WW[0].i_struct=d_i_struct=s_i_struct ;
   }
   i_Word=0 ;
   
   while( 1 )
   {
      if( 0==to ) paint_from( i_Word );
          else    paint_to  ( i_Word );

      if( i_Word==0 ) r=tag_edit( *Struct, to, i_Word, i_struct );
          else        r=tag_edit( *Word  , to, i_Word, i_struct );

      switch( r )
      {
	 case WM_NEXT : case WM_OK :
	    if( 0<i_Word ) 
	        WW[i_Word-1].i_struct=i_struct ;
	    else 
	    {  *sd_i_struct=i_struct ;
	       if( 0==to ) SS=&Grammar[i_struct].From ;
	           else    SS=&Grammar[i_struct].To ;
	       if( SS->type==TSTRUCT1 )
	       {   for( i=0 ; i<SS->Word.j ; i++ )
	              WW[i].i_struct=SS->Word[i].i_struct ;
	       }
	       if( SS->type==TWORD )
	       {   WW[0].i_struct=i_struct ;  }
	    }
            if( n_Word<=i_Word ) goto M_End ;
	    i_Word++ ;	    
            break ;
         case WM_PREV : case WM_ESC :
            if( i_Word<=0 ) return WM_ESC ;
	        else  i_Word-- ;
            break ;
      }
   }
M_End: ;
   return WM_OK ;
}
/************************************************************************/
e_WinMsg t_AddWord :: tag_edit( t_shortList &List, char to, short i_Word, 
                      short &i_struct )
{
   t_Format1  *FFF=&Grammar.format1()[0] ;
   char  Tag[10]="",*s ;
   short i,z,jTag=0,current ;
   short key1,key2 ;

   if( to==0 ){ if( i_Word==0 ) z=s_i_struct ; else z=Sou[i_Word-1].i_struct ; }
       else   { if( i_Word==0 ) z=d_i_struct ; else z=Dst[i_Word-1].i_struct ; }
   current=find( List,z ) ;
   if( current<0 ) current=0 ;

   while( 1 )
   {
      paint();
      if( 0==to ) paint_from( i_Word );
          else    paint_to  ( i_Word );
      s_text_yxf( y1+7,3,0x0b,Tag );
      paint_struct( List, to, current );

      s_getch( &key1,&key2 );

      if( key1==0 )
      {  switch(key2)
	 {
	    case S_key_F1   :  Window_help( (void *)"newwords.html") ; continue ;
	    case S_key_Left :  return WM_PREV ;
	    case S_key_Right:  goto M_End ;
	    case S_key_Up   :  if( 0<current        ) current-- ; break ;
	    case S_key_Down :  if( current<List.j-1 ) current++ ; break ;
	 }
      }
      else
      {  switch( key1 )
	 {  case  S_key_Back : if( 0<jTag ) Tag[--jTag]=0 ; break ;

	    case '\r'     : goto M_End ;
	    case S_key_Esc: return WM_ESC ;
	    default:  
	       Tag[jTag++]=key1 ; 
	       Tag[jTag]=0 ;
               for( i=0 ; i<List.j ; i++ )
	       {  s=FFF->get_tag( to,List[i] );
	          if( 0==strncmp(Tag,s,jTag ) )
		  {   current=i ; goto M_Ok ; }
	       }
               Tag[--jTag]=0 ;
	       M_Ok : ;
	       if( 2<=jTag ) goto M_End ;
         }
      }
   }
   M_End :
   i_struct=List[current] ;
   return WM_OK ;
}
/************************************************************************/
short  t_AddWord :: find( t_shortList &List, short z )
{  short i ;
   for( i=0 ; i<List.j ; i++ )
      if( List[i]==z ) return i ;
   return -1 ;
}
/************************************************************************/
char * t_AddWord :: antwort( void )
{  long i ;
   sprintf( Antwort,"%s=%s\n",sStr2,dStr2 );
   for( i=0 ; i<200 && Antwort[i]!=0 ; i++ )
      if( Antwort[i]=='@' && Antwort[i+1]=='1' )
          Antwort[i+1]='0' ;
   return Antwort;
}





