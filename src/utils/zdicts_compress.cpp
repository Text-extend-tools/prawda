/*****************************************************************************
       сжать словари (для программы, работающей со сжатыми словарями)

                                Алгоритм

1) расщепление словаря русских глаголов 
   ../../dicts/z_knowlege/russkij/ru_verb_s -> ../../dicts/z_tmp/ru_verb1
					       ../../dicts/z_tmp/ru_pass
					       ../../dicts/z_tmp/ru_partizip
					       ../../dicts/z_tmp/ru_imperativ
					       ../../dicts/z_tmp/ru_plus

2) раздельное сжатие словарей
   ../../dicts/z_tmp/ru_verb1    -> ../../dicts/z_tmp/ru_verb1.r
   ../../dicts/z_tmp/ru_pass        ../../dicts/z_tmp/ru_pass.r
   ../../dicts/z_tmp/ru_partizip    ../../dicts/z_tmp/ru_partizip.r
   ../../dicts/z_tmp/ru_imperativ   ../../dicts/z_tmp/ru_imperativ.r

3) получение общего сжатого словаря русских глаголов
   ../../dicts/z_tmp/ru_verb1.r    -> ../../dicts/z_form/russkij/ru_verb_r
   ../../dicts/z_tmp/ru_pass.r
   ../../dicts/z_tmp/ru_partizip.r
   ../../dicts/z_tmp/ru_imperativ.r

4) сжатие остальных русских словарей   
   ../../dicts/z_knowlege/russkij/ru_adj ->  ../../dicts/zform/russkij/ru_adj_r
   ../../dicts/z_knowlege/russkij/ru_sub     ../../dicts/zform/russkij/ru_sub_r

5) сжатие английских словарей
   ../../dicts/z_knowlege/english/en_sub  -> ../../dicts/zform/english/en_sub_r
   ../../dicts/z_knowlege/english/en_verb    ../../dicts/zform/english/en_verb_r

6) сжатие словарей Эсперанто
   ../../dicts/z_knowlege/esperanto/es_sub -> ../../dicts/zform/esperanto/es_sub_r
   ../../dicts/z_knowlege/esperanto/es_adj    ../../dicts/zform/esperanto/es_adj_r
   ../../dicts/z_knowlege/esperanto/es_verb   ../../dicts/zform/esperanto/es_verb_r

 *****************************************************************************/
# include <stdio.h>
# include <string.h>
# include <malloc.h>
# include <stdlib.h>
# include <mylib.cpp>

struct t_Ende
{  char  str[50][50] ;
   long  n ;       // число таких шаблонов
   short index ;
   char cmp( t_Ende &other, short n_Form );
};

struct t_Ende1
{  t_Ende *Mass ;
   long    l_Mass ;
   long    z_first ;
} ;

void compress( char *NameF, char *NameR, short n_Form, t_Ende1 *EE=NULL );
void split_ru_verb( void );
void merge_ru_verb( void );
void print_ends( FILE *fw, t_Ende *Ende, long j_Ende, long n_Form, long z_First );
void last_braket( FILE *fr );
int  read_cword( FILE *fr, char *Str1, long *index );
int  read_cword1( FILE *fr, char *Str1 );
void make_shapka( char *Name );

long FileToMass( char *File, char **_Mass );

/*****************************************************************************/
int main( int argc, char * argv[] )
{  char f=0 ;
   char Str[500] ;

   // ------ расщепление словаря русских глаголов ----------
   split_ru_verb( );

   // ------ раздельное сжатие словарей --------------------
   t_Ende1 E1,E2,E3,E4,E5 ;

   compress( "../../dicts/z_tmp/ru_verb1"    ,"../../dicts/z_tmp/ru_verb1.r"    ,11,&E1 );
   compress( "../../dicts/z_tmp/ru_pass"     ,"../../dicts/z_tmp/ru_pass.r"     , 4,&E2 );
   compress( "../../dicts/z_tmp/ru_partizip" ,"../../dicts/z_tmp/ru_partizip.r" ,24,&E3 );
   compress( "../../dicts/z_tmp/ru_imperativ","../../dicts/z_tmp/ru_imperativ.r", 2,&E4 );

   system( "cp ../../dicts/z_tmp/ru_plus ../../dicts/z_tmp/ru_plus.r" );
   
   // ------------ создание общего сжатого словаря глаголов -----------------
   E1.z_first=0 ;
   E2.z_first=E1.z_first+E1.l_Mass ;
   E3.z_first=E2.z_first+E2.l_Mass ;
   E4.z_first=E3.z_first+E3.l_Mass ;
   E5.z_first=E4.z_first+E4.l_Mass ;

   FILE *fw,*fr1,*fr2,*fr3,*fr4,*fr5 ;

   fw=Fopen( "../../dicts/zform/russkij/ru_verb_r","w" );
   fprintf( fw,"@Шаблоны {" );
   print_ends( fw, E1.Mass, E1.l_Mass, 11, E1.z_first );
   print_ends( fw, E2.Mass, E2.l_Mass,  4, E2.z_first );
   print_ends( fw, E3.Mass, E3.l_Mass, 24, E3.z_first );
   print_ends( fw, E4.Mass, E4.l_Mass,  2, E4.z_first );
   fprintf( fw,"\n%d 0 - ;", E5.z_first );
   fprintf( fw,"\n}" );

   fr1=Fopen( "../../dicts/z_tmp/ru_verb1.r","r" );
   last_braket( fr1 );
   fr2=Fopen( "../../dicts/z_tmp/ru_pass.r","r" );
   last_braket( fr2 );
   fr3=Fopen( "../../dicts/z_tmp/ru_partizip.r","r" );
   last_braket( fr3 );
   fr4=Fopen( "../../dicts/z_tmp/ru_imperativ.r","r" );
   last_braket( fr4 );
   fr5=Fopen( "../../dicts/z_tmp/ru_plus.r","r" );

   while( 1 )
   {  long index ;

      if( EOF==read_cword( fr1, Str, &index ) ) break ;
      fprintf( fw,"\n%s %d",Str,index );
      if( EOF==read_cword( fr2, Str, &index ) ) break ;
      fprintf( fw,"\n%s %d",Str,index+E2.z_first );
      if( EOF==read_cword( fr4, Str, &index ) ) break ;
      fprintf( fw,"\n%s %d",Str,index+E4.z_first );
      if( EOF==read_cword( fr3, Str, &index ) ) break ;
      fprintf( fw,"\n%s %d",Str,index+E3.z_first );
      if( EOF==read_cword( fr3, Str, &index ) ) break ;
      fprintf( fw,"\n%s %d",Str,index+E3.z_first );
      if( EOF==read_cword1(fr5, Str ) ) break ;
      fprintf( fw,"\n%s %d",Str,E5.z_first );

      if( EOF==read_cword( fr1, Str, &index ) ) break ;
      fprintf( fw,"\n%s %d",Str,index );
      if( EOF==read_cword( fr2, Str, &index ) ) break ;
      fprintf( fw,"\n%s %d",Str,index+E2.z_first );
      if( EOF==read_cword( fr4, Str, &index ) ) break ;
      fprintf( fw,"\n%s %d",Str,index+E4.z_first );
      if( EOF==read_cword( fr3, Str, &index ) ) break ;
      fprintf( fw,"\n%s %d",Str,index+E3.z_first );
      if( EOF==read_cword1(fr5, Str ) ) break ;
      fprintf( fw,"\n%s %d",Str,E5.z_first );
      fprintf( fw,"\n;");
   }

   Fclose( fw );
   Fclose( fr1 );
   Fclose( fr2 );
   Fclose( fr3 );
   Fclose( fr4 );
   // ------ сжатие остальных русских словарей ----------------
   compress( "../../dicts/z_knowlege/russkij/ru_adj"  ,"../../dicts/zform/russkij/ru_adj_r" ,28 );
   compress( "../../dicts/z_knowlege/russkij/ru_sub"  ,"../../dicts/zform/russkij/ru_sub_r" ,12 );

   // ------ сжатие английских словарей -----------------------
   compress( "../../dicts/z_knowlege/english/en_sub"  ,"../../dicts/zform/english/en_sub_r" , 2 );
   compress( "../../dicts/z_knowlege/english/en_verb" ,"../../dicts/zform/english/en_verb_r", 5 );

   // ------ сжатие словарей Эсперанто ------------------------
   compress( "../../dicts/z_knowlege/esperanto/es_sub" ,"../../dicts/zform/esperanto/es_sub_r" , 4 );
   compress( "../../dicts/z_knowlege/esperanto/es_adj" ,"../../dicts/zform/esperanto/es_adj_r" , 4 );
   compress( "../../dicts/z_knowlege/esperanto/es_verb","../../dicts/zform/esperanto/es_verb_r", 33 );

   // ------ удаление промежуточных файлов --------------------
   system( "rm ../../dicts/z_tmp/*.r" );

   system( "rm ../../dicts/z_tmp/ru_verb1" );
   system( "rm ../../dicts/z_tmp/ru_pass" );
   system( "rm ../../dicts/z_tmp/ru_partizip" );
   system( "rm ../../dicts/z_tmp/ru_imperativ" );
   system( "rm ../../dicts/z_tmp/ru_plus" );

   // ------- приделывание шапки -----------------------------
   make_shapka( "../../dicts/zform/russkij/ru_verb_r" );
   make_shapka( "../../dicts/zform/russkij/ru_adj_r" );
   make_shapka( "../../dicts/zform/russkij/ru_sub_r" );
   
   make_shapka( "../../dicts/zform/english/en_sub_r" );
   make_shapka( "../../dicts/zform/english/en_verb_r" );
   
   make_shapka( "../../dicts/zform/esperanto/es_sub_r" );
   make_shapka( "../../dicts/zform/esperanto/es_adj_r" );
   make_shapka( "../../dicts/zform/esperanto/es_verb_r" );
   
   return 0 ;
}
/*****************************************************************************/
//                  сжать один грамматический словарь
/*****************************************************************************/
void compress( char *NameF, char *NameR, short n_Form, t_Ende1 *EE )
{
   long    i,i1,j_Mass=0,j_Word=0 ;
   short   j_Form=0,j_Str=0,i_best,n_best,f_remark=0 ;
   long   *Pattern ;
   FILE   *fr,*fw ;
   char   *Mass ;
   char    Str[50][50],Str1[100] ;
   short   j_Ende=0 ;
   t_Ende *Ende,Ende1 ;

   for( i=0 ; i<50 ; i++ )
      for( i1=0 ; i1<50 ; i1++ )
	 Str[i][i1]=0 ;

   Ende1.n=1 ;

   Ende    = (t_Ende *)Calloc(   3000, sizeof(t_Ende) );
   Mass    = (char   *)Calloc(5000000, sizeof(char)   );
   Pattern = (long   *)Calloc( 100000, sizeof(long)  );

   fr=Fopen( NameF,"r");
   f_remark=0 ;
   while( 1 )
   {  // ------ прочесть все словоформы ------------------
      for( i=0 ; i<n_Form ; )      
      {         
	 if( EOF==fscanf(fr,"%s",&Str1) ) goto M_Ende ;
	 if( 0==strcmp(Str1,"/*") ) f_remark=1 ;
	 if( f_remark==0 ) strcpy( Str[i++],Str1 );
	 if( 0==strcmp(Str1,"*/") ) f_remark=0 ;
      }
      strcpy( Mass+j_Mass,Str[0] );
      j_Mass+=strlen( Str[0] )+1 ;

      fscanf(fr,"%s",&Str1) ;
      // ------ определить где кончается корень ----------
      for( i=0 ; ; i++ )
      {  char c=Str[0][i] ;
	 for( i1=0 ; i1<n_Form ; i1++ )
	 {
	    if( Str[i1][i]==0 ) goto M1 ;
	    if( Str[i1][i]!=c ) goto M1 ;
	 }
      }
      M1:
      // ------ создать шаблон ---------------------------
      for( i1=0 ; i1<n_Form ; i1++ )
	 strcpy( Ende1.str[i1],Str[i1]+i ) ;
      // ------ сравнить его с существующими -------------
      for( i1=0 ; i1<j_Ende ; i1++ )
	 if( 0==Ende1.cmp( Ende[i1],n_Form) )
	 {  Ende[i1].n++ ; break ; }
      if( i1==j_Ende )
	  Ende[j_Ende++]=Ende1 ;
      if( 2900<j_Ende )
      {   printf("Че-то на вас шаблонов не напасешься"); exit(-1);  }

      Pattern[j_Word++]=i1 ;
   }
M_Ende:
   fclose( fr );

   fw=Fopen( NameR,"w");
   // ---------------- печать окончаний ----------------------
   fprintf( fw,"@Шаблоны {");
   print_ends( fw, Ende, j_Ende, n_Form, 0 );
   fprintf( fw,"\n}");
   j_Mass=0 ;
   // --------------- печать собственно слов -------------------
   for( i=0 ; i<j_Word ; i++ )
   {  fprintf( fw,"\n%s %d ;",Mass+j_Mass,Ende[Pattern[i]].index );
      j_Mass+=strlen(Mass+j_Mass)+1 ;
   }

   if( EE!=NULL )
   {  EE->Mass=Ende ;
      EE->l_Mass=j_Ende ;
   }
   Free( Mass );
   // Ende сознательно не освобождается
   fclose(fw);
}
/************************************************************************/
//              найти скобку, закрывающую массив шаблонов
/************************************************************************/
void last_braket( FILE *fr )
{  char Str[100] ;

   while( 1 )
   {
      if( EOF==fscanf(fr,"%s",&Str) ) return ;
      if( Str[0]=='}' ) break ;
   }
}
/************************************************************************/
int read_cword( FILE *fr, char *Str1, long *index )
{  char Str[100] ;

   if( EOF==fscanf(fr,"%s",&Str) ) return EOF ;
   strcpy( Str1,Str );
   if( EOF==fscanf(fr,"%s",&Str) ) return EOF ;
   *index=atoi( Str );
   if( EOF==fscanf(fr,"%s",&Str) ) return EOF ;
   if( Str[0]!=';' )
   {  printf("Error cword"); exit(-1) ;  }

   return 1 ;      
}
/************************************************************************/
int read_cword1( FILE *fr, char *Str1 )
{  char Str[100] ;

   if( EOF==fscanf(fr,"%s",&Str) ) return EOF ;
   strcpy( Str1,Str );
   if( EOF==fscanf(fr,"%s",&Str) ) return EOF ;
   if( Str[0]!=';' )
   {  printf("Error cword"); exit(-1) ;  }

   return 1 ;      
}
/************************************************************************/
//                         Напечатать шаблоны
/************************************************************************/
void print_ends( FILE *fw, t_Ende *Ende, long j_Ende, long n_Form, long z_First )
{  long i,i1,i_best,n_best ;

   for( i=0 ; i<j_Ende ; i++ )
      Ende[i].index=-1 ;
   for( i=0 ; i<j_Ende ; i++ )
   {  i_best=n_best=0 ;
      for( i1=0 ; i1<j_Ende ; i1++ )
      {  if( n_best<Ende[i1].n && Ende[i1].index<0 )
	 {  n_best=Ende[i1].n ; i_best=i1 ;  }
      }
      Ende[i_best].index=i ;
      fprintf( fw, "\n %d %d",z_First+i,n_best );
      for( i1=0 ; i1<n_Form ; i1++ )
	 fprintf( fw," -%s",Ende[i_best].str[i1] );
      fprintf( fw," ;");
   }
}
/*************************************************************************/
char t_Ende :: cmp( t_Ende &other, short n_Form )
{
   for( short i=0 ; i<n_Form ; i++ )
      if( 0!=strcmp(str[i],other.str[i]) )
	  return -1 ;

   return 0 ;
}
/*****************************************************************************/
//                     приделать шапку к файлу
/*****************************************************************************/
void make_shapka( char *Name )
{  char *Mass ;
   long  L ;
   L=FileToMass( Name,&Mass );
   
   FILE *fw=Fopen( Name,"w" );

   fprintf( fw,"\n/* Сей файл является частью Open Source переводчика \"Меркурий\" */");
   fprintf( fw,"\n/* Распространяется под лицензией GNU GPL ver 2.0              */");
   fprintf( fw,"\n/* (C) Анисимов Д.В.                                   2004 г. */");
   fprintf( fw,"\n" );

   fwrite( Mass,L,sizeof(char),fw );
   fclose( fw );
}
/***************************************************************************/
long FileToMass( char *File, char **_Mass )
{
   char *Mass ; long L ; FILE *fr ;
   
   fr=Fopen( File,"r" );
   L=FileLength( fr );
   Mass=(char *)Calloc( L,1 );
   fread( Mass,L,1,fr );
   Fclose(fr);

   *_Mass=Mass ;
   return L ;
}
/***************************************************************************/
long FileToWord( char *File, char ***_Str )
{  long  i,j,L ;
   char *m,**ss ;

   L=FileToMass( File, &m );

   for( i=0 ; i<L ; i++ )
      if( m[i]==' ' || m[i]=='\n' || m[i]=='\r' || m[i]=='\t' ) m[i]=0 ;
   for( i=j=0 ; i<L ; i++ )
      if( m[i]==0 && m[i+1]!=0 ) j++ ;
   ss=(char **)Calloc(j+1,sizeof(char *));
   j=0 ;
   if( m[0]!=0 ) ss[j++]=m ;
   for( i=0 ; i<L ; i++ )
      if( m[i]==0 && m[i+1]!=0 ) ss[j++]=m+i+1 ;

   *_Str=ss ;
   return j-1 ;
}
/*************************************************************************************/
void split_ru_verb( void )
{  long  i,i1,N ;
   char **ss ;
   FILE *fw1, *fw2,*fw3,*fw4,*fw5 ;
   char  Name[300] ;
   
   N=FileToWord( "../../dicts/z_knowlege/russkij/ru_verb",&ss );
   fw1=Fopen( "../../dicts/z_tmp/ru_verb1",    "w");
   fw2=Fopen( "../../dicts/z_tmp/ru_pass",     "w");
   fw3=Fopen( "../../dicts/z_tmp/ru_partizip", "w");
   fw4=Fopen( "../../dicts/z_tmp/ru_imperativ","w");
   fw5=Fopen( "../../dicts/z_tmp/ru_plus",     "w");
   
   // ------- найти конец шапки ----------------
   for( i=i1=0 ; i<N ; i++ )
      if( ss[i][0]=='*' && ss[i][1]=='/' ) i1=i ;
   
   for( i=i1+1 ; i<N ; i+=109 )
   {
      // ---- ru_verb1 ----------------------
      for( i1=0 ; i1<11 ; i1++ )
         fprintf( fw1,"%s ",ss[i+i1] );
      fprintf( fw1,";\n");
      for( i1=0 ; i1<11 ; i1++ )
         fprintf( fw1,"%s ",ss[66+i+i1] );
      fprintf( fw1,";\n");
      
      // ---- ru_pass -----------------------
      fprintf( fw2,"%s %s %s %s ;\n",ss[i+11],ss[i+12],ss[i+13],ss[i+14] );
      fprintf( fw2,"%s %s %s %s ;\n",ss[66+i+11],ss[66+i+12],ss[66+i+13],ss[66+i+14] );

      // ---- ru_partizip -------------------
      for( i1=0 ; i1<24 ; i1++ )
         fprintf( fw3,"%s ",ss[i+17+i1] );
      fprintf( fw3," ;\n");
      for( i1=0 ; i1<24 ; i1++ )
         fprintf( fw3,"%s ",ss[i+17+24+i1] );
      fprintf( fw3," ;\n");
      for( i1=0 ; i1<24 ; i1++ )
         fprintf( fw3,"%s ",ss[66+i+17+i1] );
      fprintf( fw3," ;\n");

      // ---- ru_imperativ -------------------
      fprintf( fw4,"%s %s ;\n",ss[i+15],ss[i+16] );
      fprintf( fw4,"%s %s ;\n",ss[66+i+15],ss[66+i+16] );

      // ---- ru_plus -------------------
      fprintf( fw5,"%s ;\n",ss[i+65] );
      fprintf( fw5,"%s ;\n",ss[66+i+41] );
   }
   Fclose( fw1 );
   Fclose( fw2 );
   Fclose( fw3 );
   Fclose( fw4 );
   Fclose( fw5 );
}
