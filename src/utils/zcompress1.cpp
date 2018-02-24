/*****************************************************************************/
//               сжать-разжать словари
/*****************************************************************************/
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

char Full[300]  ="../dicts/" ;  // каталог полных файлов
char Reduce[300]="../dictsr/" ; // каталог сокращенных файлов

void compress( char *Name, short n_Form, t_Ende1 *EE=NULL );
void express ( char *Name, short n_Form, short LL, char f_shapka=1 );
void split_ru_verb( void );
void merge_ru_verb( void );
void print_ends( FILE *fw, t_Ende *Ende, long j_Ende, long n_Form );
void last_braket( FILE *fr );
int  read_cword( FILE *fr, char *Str1, long *index );
int  read_cword1( FILE *fr, char *Str1 );

/*****************************************************************************/
int main( int argc, char * argv[] )
{  char f=0 ;
   char Str[500] ;
  
   if( argc<2 )
   {
      printf( "  используйте следующим образом\n" );
      printf( "  zcompress <ключ> <каталог полных файлов> <каталог сжатых файлов>\n" );
      printf( "  значение ключа: c (латинское) - сжать, e - разжать\n" );
      printf( "  если каталоги полных и сжатых  файлов не заданы, то они считаются\n" );
      printf( "  ../dicts/ и ../dictsr/ соответственно.\n" );
      exit(-1);
   }
   else
      f=argv[1][0] ;
   if( 2<argc ) 
   {   strcpy( Full,argv[2] );
       if( Full[strlen(Full)-1]!='/' ) strcat( Full,"/" );
   }
   if( 3<argc ) 
   {   strcpy( Reduce,argv[3] );
       if( Reduce[strlen(Reduce)-1]!='/' ) strcat( Reduce,"/" );
   }

   if( f=='c' )
   {  // ---------- сжать ------------------------------
      split_ru_verb( );
      
      t_Ende1 E1,E2,E3,E4,E5 ;
      
      compress( "ru_verb1"    ,11,&E1 );
      compress( "ru_pass"     , 4,&E2 );
      compress( "ru_partizip" ,24,&E3 );
      compress( "ru_imperativ" ,2,&E4 );
      
      sprintf( Str,"cp %sru_plus %sru_plus.r",Full,Full );
      system( Str );
      // вот тут должно быть создание общего сжатого словаря глаголов

      E1.z_first=0 ;
      E2.z_first=E1.z_first+E1.l_Mass ;
      E3.z_first=E2.z_first+E2.l_Mass ;
      E4.z_first=E3.z_first+E3.l_Mass ;
      E5.z_first=E4.z_first+E4.l_Mass ;

      FILE *fw,*fr1,*fr2,*fr3,*fr4,*fr5 ;

      fw=Fopen( "ru_verb.r","w" );
      fprintf( fw,"@Шаблоны {" );
      print_ends( fw, E1.Mass, E1.l_Mass, 11 );
      print_ends( fw, E2.Mass, E2.l_Mass,  4 );
      print_ends( fw, E3.Mass, E3.l_Mass, 24 );
      print_ends( fw, E4.Mass, E4.l_Mass,  2 );
      fprintf( fw,"\n%d 0 - ;", E5.z_first );
      fprintf( fw,"\n}" );

      sprintf( Str,"%s/ru_verb1.r",Full );
      fr1=Fopen( Str,"r" );
      last_braket( fr1 );
      sprintf( Str,"%s/ru_pass.r",Full );
      fr2=Fopen( Str,"r" );
      last_braket( fr2 );
      sprintf( Str,"%s/ru_partizip.r",Full );
      fr3=Fopen( Str,"r" );
      last_braket( fr3 );
      sprintf( Str,"%s/ru_imperativ.r",Full );
      fr4=Fopen( Str,"r" );
      last_braket( fr4 );
      sprintf( Str,"%s/ru_plus.r",Full );
      fr5=Fopen( Str,"r" );

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

      compress( "ru_adj"  ,24 );
      compress( "ru_sub"  ,12 );
      compress( "en_sub"  ,2 );
      compress( "en_verb" ,5 );

      sprintf( Str,"mv %s*.r %s",Full,Reduce );
      system( Str );
      
      sprintf( Str, "rm %sru_verb1",     Full ); system( Str );
      sprintf( Str, "rm %sru_pass",      Full ); system( Str );
      sprintf( Str, "rm %sru_partizip",  Full ); system( Str );
      sprintf( Str, "rm %sru_imperativ", Full ); system( Str );
      sprintf( Str, "rm %sru_plus",      Full ); system( Str );
      exit(0);
   }
   if( f=='e' )
   {  // ---------- разжать ------------------------------
      sprintf( Str,"cp %s*.r %s",Reduce,Full );
      system( Str );

      express( "ru_verb1"    ,11,100, 0 );
      express( "ru_pass"     , 4,100, 0 );
      express( "ru_partizip" ,24,100, 0 );
      express( "ru_imperativ", 2,100, 0 );
      
      sprintf( Str,"cp %sru_plus.r %sru_plus",Full,Full );
      system( Str );

      merge_ru_verb();
      
      express( "ru_adj"  ,24,4 );
      express( "ru_sub"  ,12,6 );
      express( "en_sub"  ,2 ,100 );
      express( "en_verb" ,5 ,100 );

      sprintf( Str, "rm %sru_verb1",     Full ); system( Str );
      sprintf( Str, "rm %sru_pass",      Full ); system( Str );
      sprintf( Str, "rm %sru_partizip",  Full ); system( Str );
      sprintf( Str, "rm %sru_imperativ", Full ); system( Str );
      sprintf( Str, "rm %sru_plus",      Full ); system( Str );
      sprintf( Str, "rm %s*.r",          Full ); system( Str );
      
      exit(0);
   }
   printf("Не могу понять ваших намерений\n");
   return 0 ;
}
/*****************************************************************************/
//                     сжать один грамматический словарь
/*****************************************************************************/
void compress( char *Name, short n_Form, t_Ende1 *EE )
{
   long    i,i1,j_Mass=0,j_Word=0 ;
   short   j_Form=0,j_Str=0,i_best,n_best,f_remark=0 ;
   long   *Pattern ;
   FILE   *fr,*fw ;
   char   *Mass ;
   char    Str[50][50],Str1[100] ;
   short   j_Ende=0 ;
   t_Ende *Ende,Ende1 ;
   char    Name1[200] ;

   for( i=0 ; i<50 ; i++ )
      for( i1=0 ; i1<50 ; i1++ )
	 Str[i][i1]=0 ;

   Ende1.n=1 ;

   Ende    = (t_Ende *)calloc(   3000, sizeof(t_Ende) );
   Mass    = (char   *)calloc(5000000, sizeof(char)   );
   Pattern = (long   *)calloc( 100000, sizeof(long)  );

   sprintf( Name1,"%s%s",Full,Name );
   fr=Fopen( Name1,"r");
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

   sprintf( Name1,"%s%s.r",Full,Name );
   fw=Fopen( Name1,"w");
   // ---------------- печать окончаний ----------------------
   fprintf( fw,"@Шаблоны {");
   print_ends( fw, Ende, j_Ende, n_Form );
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
void print_ends( FILE *fw, t_Ende *Ende, long j_Ende, long n_Form )
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
      fprintf( fw, "\n %d %d",i,n_best );
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
void shapka( FILE *fw )
{
   fprintf( fw,"\n/* Сей файл является частью Open Source переводчика \"Меркурий\" */");
   fprintf( fw,"\n/* Распространяется под лицензией GNU GPL ver 2.0              */");
   fprintf( fw,"\n/* (C) Анисимов Д.В.                                   2003 г. */");
   fprintf( fw,"\n" );
}
/*****************************************************************************/
//                   разжать один грамматический словарь
/*****************************************************************************/
void express( char *Name, short n_Form, short LL, char f_shapka )
{
   FILE   *fr,*fw ;
   t_Ende *Ende,Ende1 ;
   char    Name1[200] ;
   char    Str[100],Str1[100],Str2[100] ;
   long    i,i1 ;
   short   l,j_Form=0,i_Ende,j_Ende=0,j_Str=0 ;

   sprintf( Name1,"%s%s.r",Full,Name );
   fr=Fopen( Name1,"r");
   Ende    = (t_Ende *)calloc(   2000, sizeof(t_Ende) );

   // -------- чтение массива шаблонов -----------------
   fscanf( fr,"%s",&Str );
   if( 0!=strcmp(Str,"@Шаблоны") )
   {  printf("ошибка формата"); exit(-1);  }
   fscanf( fr,"%s",&Str );
   if( 0!=strcmp(Str,"{") )
   {  printf("ошибка формата"); exit(-1);  }

   while( 1 )
   {
      if( EOF==fscanf(fr,"%s",&Str) )
      {  printf("ошибка формата"); exit(-1);  }
      if( 0==strcmp(Str,"}") ) break ;
      if( EOF==fscanf(fr,"%s",&Str) )
      {  printf("ошибка формата"); exit(-1);  }

      for( i=0 ; i<n_Form ; i++ ) 
      {  
         if( EOF==fscanf(fr,"%s",&Str) )
	 {  printf("ошибка формата"); exit(-1);  }
         strcpy( Ende1.str[i],Str+1 ) ;
      }
      Ende[j_Ende++]=Ende1 ;
      if( EOF==fscanf(fr,"%s",&Str) )
      {  printf("ошибка формата"); exit(-1);  }   
      if( 0!=strcmp(Str,";") )
      {  printf("ошибка формата"); exit(-1);  }
   }
   // --------- напечатать шапку ----------------------
   sprintf( Name1,"%s%s",Full,Name );
   fw=Fopen( Name1,"w");
   if( f_shapka==1 ) shapka( fw );
   
   // --------- чтение массива слов -------------------
   while( 1 )
   {
      if( EOF==fscanf(fr,"%s",&Str) ) goto M_Ende ;
      if( EOF==fscanf(fr,"%s",&Str1) ) goto M_Ende ;
      i_Ende=atoi( Str1 );
      if( EOF==fscanf(fr,"%s",&Str1) ) goto M_Ende ;
      if( Str1[0]!=';' )
      {  printf("ошибка формата"); exit(-1);  }
      l=strlen(Ende[i_Ende].str[0]);
      Str[strlen(Str)-l]=0 ;
      for( i=0 ; i<n_Form ; i++ )
      {  if( (i%LL)==0 ) fprintf( fw,"\n");
         fprintf( fw,"%s%s ",Str,Ende[i_Ende].str[i] );
      }
      fprintf( fw,"\n;");
   }
   M_Ende:
   fclose(fw);
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
   
   sprintf( Name,"%s/ru_verb",Full );
   N=FileToWord( "../dicts/ru_verb",&ss );
   sprintf( Name,"%s/ru_verb1",    Full );  fw1=Fopen( Name,"w");
   sprintf( Name,"%s/ru_pass",     Full );  fw2=Fopen( Name,"w");
   sprintf( Name,"%s/ru_partizip", Full );  fw3=Fopen( Name,"w");
   sprintf( Name,"%s/ru_imperativ",Full );  fw4=Fopen( Name,"w");
   sprintf( Name,"%s/ru_plus",     Full );  fw5=Fopen( Name,"w");
   
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
/*************************************************************************************/
void merge_ru_verb( void )
{  long i1,i2,i3,i4,i5,j ;
   long N1,N2,N3,N4,N5 ;
   char **ss1,**ss2,**ss3,**ss4,**ss5 ;
   FILE *fw ;
   char  Name[300] ;

   sprintf( Name,"%s/ru_verb1",    Full ); N1=FileToWord( Name, &ss1 );
   sprintf( Name,"%s/ru_pass",     Full ); N2=FileToWord( Name, &ss2 );
   sprintf( Name,"%s/ru_partizip", Full ); N3=FileToWord( Name, &ss3 );
   sprintf( Name,"%s/ru_imperativ",Full ); N4=FileToWord( Name, &ss4 );
   sprintf( Name,"%s/ru_plus",     Full ); N5=FileToWord( Name, &ss5 );

   sprintf( Name,"%s/ru_verb",     Full ); fw=Fopen( Name,"w");
   
   shapka( fw );
   
   for( i1=i2=i3=i4=i5=0 ; i1<N1 ; i1+=24,i2+=10,i4+=6,i3+=75,i5+=4 )
   {  // ====== несовершенная половина ==========
      // ---- ru_verb1 --------------------------
      fprintf( fw,"\n%s\n\t",ss1[i1] );
      for( j=1 ; j<7 ; j++ )
         fprintf( fw,"%s ",ss1[i1+j] );
      fprintf( fw,"\n\t" );
      for( j=7 ; j<11 ; j++ )
         fprintf( fw,"%s ",ss1[i1+j] );
      // ---- ru_pass ---------------------------
      fprintf( fw,"\n\t%s %s %s %s",ss2[i2],ss2[i2+1],ss2[i2+2],ss2[i2+3] );
      // ----- ru_imperativ --------------------
      fprintf( fw,"\n\n\t%s %s",ss4[i4],ss4[i4+1] );
      // ---- ru_partizip -----------------------
      fprintf( fw,"\n");
      for( j=0 ; j<24 ; j++ )
      {  if( (j%4)==0 ) fprintf( fw,"\n\t");
         fprintf( fw,"%s ",ss3[i3+j]);
      }
      fprintf( fw,"\n");
      for( j=0 ; j<24 ; j++ )
      {  if( (j%4)==0 ) fprintf( fw,"\n\t");
         fprintf( fw,"%s ",ss3[i3+25+j]);
      }
      // ----- ru_plus --------------------------
      fprintf( fw,"\n\n\t%s",ss5[i5] );
      
      // ====== совершенная половина ============
      // ---- ru_verb1 --------------------------
      fprintf( fw,"\n%s\n\t",ss1[i1+12] );
      for( j=1 ; j<7 ; j++ )
         fprintf( fw,"%s ",ss1[i1+j+12] );
      fprintf( fw,"\n\t" );
      for( j=7 ; j<11 ; j++ )
         fprintf( fw,"%s ",ss1[i1+j+12] );
      // ---- ru_pass ---------------------------
      fprintf( fw,"\n\t%s %s %s %s",ss2[i2+5],ss2[i2+6],ss2[i2+7],ss2[i2+8] );
      // ----- ru_imperativ ---------------------
      fprintf( fw,"\n\n\t%s %s",ss4[i4+3],ss4[i4+4] );
      // ---- ru_partizip -----------------------
      fprintf( fw,"\n");
      for( j=0 ; j<24 ; j++ )
      {  if( (j%4)==0 ) fprintf( fw,"\n\t");
         fprintf( fw,"%s ",ss3[i3+j+50]);
      }
      // ----- ru_plus --------------------------
      fprintf( fw,"\n\n\t%s",ss5[i5+2] );
      fprintf( fw,"\n;");
   }
   Fclose(fw);
}
