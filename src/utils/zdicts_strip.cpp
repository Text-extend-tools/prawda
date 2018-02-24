/* ========================================================================
   Удалить из русских грамматических словарей все слова, которые не
   задействованы в словарях перевода
======================================================================== */

# include <string.h>

# include <grammar.h>
# include <slowo2.h>
# include <slowo3.h>
# include <core.h>

char *Parts[]={ "глагол","сущ","прил",NULL  };
char *Dicts[]={ "ru_verb","ru_sub","ru_adj",NULL  };

struct t_Struct2Dict
{   char *s1,*s2 ;
    short i_struct,i_dict ;
};

// ---------- английские соответствия --------------------
t_Struct2Dict Struct2Dict_en[]= 
{
   { "v" ,"глагол", 0, 0 },
   { "m" ,"глагол", 0, 0 },
   { "b" ,"глагол", 0, 0 },
   { "s" ,"сущ",    0, 0 },
   { "a" ,"прил",   0, 0 },
   { "as","прил",   0, 0 },
   { "ia","прил",   0, 0 },

   { "v1","глагол", 0, 0 },
   { "v2","глагол", 0, 0 },
   { "v3","глагол", 0, 0 },
   { "v4","глагол", 0, 0 },

   { "b1","глагол", 0, 0 },
   { "b2","глагол", 0, 0 },
   { "b3","глагол", 0, 0 },
   { "b4","глагол", 0, 0 },

   { "h1","глагол", 0, 0 },
   { "h2","глагол", 0, 0 },
   { "h3","глагол", 0, 0 },
   { "h4","глагол", 0, 0 },
   { NULL,NULL    , 0, 0 }
};
// ---------- соответствия Эсперанто --------------------
t_Struct2Dict Struct2Dict_es[]= 
{
   { "v" ,"глагол", 0, 0 },
   { "b" ,"глагол", 0, 0 },
   { "s" ,"сущ",    0, 0 },
   { "a" ,"прил",   0, 0 },
   { NULL,NULL    , 0, 0 }
};

//t_Grammar    Grammar1 ;  // фиктивная strip-грамматика
t_Slowo3     Perevod ;
t_Slowo2    *SlowoF,*SlowoT ;
short       *i_FormatT ; // соответствие часть речи -> SlowoT
short       *i_FormatF ; // соответствие часть речи -> SlowoF
t_Core       Core ;

char        **Present ; // массив присутствия
long        *n_Present ; // массив присутствия
short       *Trans ;    // соответствие структура(например h4)->часть_речи(глагол)

char read_grammar1( void );
char read_grammar ( char *DirGrammar );
void lang( char *DirGrammar, t_Struct2Dict *Struct2Dict );

/************************************************************************/   
int main( void )
{
   long i,i1,i2,i3,is,z1,z2,N,N1,N2,N3 ;
   FILE *fw ;
   char Str[300] ;

   // ---------- прочесть фиктивную грамматику ---------------
   read_grammar1( );

   // ---------- захватить массивы под Present ---------------
   t_Lang &To=Grammar.to();

   Present  =(char **)Calloc( To.Part.j,sizeof(char *) );
   n_Present=(long  *)Calloc( To.Part.j,sizeof(long  ) );
   for( i=0 ; i<To.Part.j ; i++ )
   {  char *s1=To.Part[i].Name ;
      for( i1=0 ; Parts[i1]!=NULL ; i1++ )
      {  if( 0==strcmp(s1,Parts[i1]) )
         {   t_Slowo2 &Slowo=SlowoT[i1] ;
	     n_Present[i1]=Slowo.n_Word ;
	     Present[i1]=(char *)Calloc( Slowo.n_Word,sizeof(char) );
             break ;
	 }
      }
   }
   lang( "english_russian"  ,Struct2Dict_en );
   lang( "esperanto_russkij",Struct2Dict_es );
      
   // -------- напечатать словари ---------------------------
   for( i=0 ; i<To.Part.j ; i++ )
   {  for( i1=0 ; Parts[i1]!=NULL ; i1++ )
      {  if( 0==strcmp( To.Part[i].Name,Parts[i1]) )
         {   t_Slowo2 &Slowo=SlowoT[i1] ;
	     sprintf( Str,"../../dicts/z_knowlege/russkij/%s_s",Dicts[i1] ); // временное явление
	     fw=Fopen( Str, "w" );
	     for( i2=0 ; i2<n_Present[i1] ; i2++ )
	        if( Present[i1][i2]!=0 )
	            Slowo.print_word( fw,i2 );
	     Fclose(fw);
	 }
      }
   }
}
/****************************************************************************/
//                      обработать один язык
/****************************************************************************/
void lang( char *DirGrammar, t_Struct2Dict *Struct2Dict )
{
   long i,i1,i2,i3,is,z1,z2,N,N1,N2,N3 ;

   read_grammar( DirGrammar );

   t_PartDskList  &Rename=Grammar.format1()[0].DstPart ;

   t_Lang &To=Grammar.to();

   Trans=(short *)Calloc( Grammar.n_trans(),sizeof(short) );

   // ---------- доопределить данные -------------------------
   for( i=0 ; Struct2Dict[i].s1!=NULL ; i++ )
   {
      for( i1=0 ; i1<Rename.j ; i1++ )
         if( 0==strncmp( Struct2Dict[i].s1,Rename[i1].Tag,2 ) )
	     z1=Rename[i1].i_part ;
      for( i1=0 ; i1<To.Part.j ; i1++ )
         if( 0==strcmp( Struct2Dict[i].s2,To.Part[i1].Name ) )
	     z2=i1 ;
      Trans[z1]=z2 ;
   }

   // --------- заполнить Present ----------------------------
   N=Perevod.n_record() ; 
   for( i=0 ; i<N ; i++ )
   {  t_sRecord *RR=Perevod.record( i );
      N1=Perevod.get_n_perevod( i );
      for( i1=0 ; i1<N1 ; i1++ )
      {  t_sStruct *SS=Perevod.get_to( i, i1 );
         for( i2=0 ; i2<SS->n_Word ; i2++ )
	 {  t_sWord *WW=&SS->Word[i2] ;
	    is=Trans[WW->i_struct] ;
	    if( 0<n_Present[i_FormatT[is]] )
	    {   t_Slowo2 &Slowo=SlowoT[i_FormatT[is]] ;
		long       Ant[20] ;
		t_Form     Form[20] ;
	        N3=Slowo.quest( WW->str,Ant,Form );
		for( i3=0 ; i3<N3 ; i3++ )
		   Present[i_FormatT[is]][Ant[i3]]=1 ;
	    }
	 }
      }
   }
}

/****************************************************************************/
//                      прочитать strip-грамматику
/****************************************************************************/
char read_grammar1( void )
{
   t_File *F ;
   long   f,i,i1,N ;
   char   Filename[200] ;
   char   r ;
try
{
   r=Grammar.make_all( "../../grammar/strip/lang.txt" );
   if( r<0 ) return r ;
   File_Error=Fopen("mercury.err","w+"); 

   // ------ вычислить число частей речи -----------
   for( i1=0 ; Parts[i1]!=NULL ; i1++ ) ;
   N=i1 ;

   // ------ прочитать русские словари в соответствии с массивом Parts ------
   t_Lang &To=Grammar.to();
   SlowoT   =(t_Slowo2 *)Calloc( N,sizeof(t_Slowo2) );
   i_FormatT=(short *)Calloc( To.Part.j,sizeof(t_Slowo2) );
   for( i=0 ; i<To.Part.j ; i++ )
      i_FormatT[i]=-1 ;

   for( i=0 ; i<To.File.j ; i++ )
   {  F=&To.File[i] ;
      for( f=i1=0 ; Parts[i1]!=NULL ; i1++ )
         if( 0==strcmp( Grammar[F->i_part].To.Name,Parts[i1] ) )
         {  sprintf( Filename,"../../%s",F->FileName[0] );
            SlowoT[i1].read( Filename, &To, &To.Format[F->i_format] );
            i_FormatT[F->i_part]=i1 ;
         }
   }

   Fclose( File_Error );
   return Read_Error ; // если тут 0, то все в порядке   
}
catch( int E )
{
   Fclose( File_Error );
   return -1 ;
}
}
/****************************************************************************/
//               прочитать грамматику какого-л языка
/****************************************************************************/
char read_grammar( char *DirGrammar )
{
   t_File *F ;
   long   f,i,i1,N ;
   char   FileName[300] ;
   char   r ;
try
{
   sprintf( FileName,"../../grammar/%s/lang.txt",DirGrammar );
   r=Grammar.make_all( FileName );
   if( r<0 ) return r ;
   File_Error=Fopen("mercury.err","w+"); 

   // ------- сформировать массив i_FormatT -------------
   t_Lang &To=Grammar.to();
   i_FormatT=(short *)Calloc( To.Part.j,sizeof(t_Slowo2) );
   for( i=0 ; i<To.Part.j ; i++ )
      i_FormatT[i]=-1 ;

   for( i=0 ; i<To.File.j ; i++ )
   {  F=&To.File[i] ;
      for( f=i1=0 ; Parts[i1]!=NULL ; i1++ )
         if( 0==strcmp( Grammar[F->i_part].To.Name,Parts[i1] ) )
         {   i_FormatT[F->i_part]=i1 ;
	     SlowoT[i1].set_lf( &To, &To.Format[F->i_format] ) ;
	 }
   }
   // -------------- прочитать словари перевода ---------------
   sprintf( FileName,"../../",DirGrammar );
   Perevod.read( FileName,&Grammar );

   Fclose( File_Error );
   return Read_Error ; // если тут 0, то все в порядке   
}
catch( int E )
{
   Fclose( File_Error );
   return -1 ;
}
}
/***************************************************************************/
char *filename( char *Path, char *File )
{  static char Str[200] ;

   Strcpy( Str,Path,200 );
   if( Str[strlen(Str)-1]!='/' )
      Strcat( Str,"/",200 );
   Strcat( Str,File,200 );
   return Str ;
}
/****************************************************************************/
t_Core :: t_Core( )
{ ; }
t_Core :: ~t_Core( )
{ ; }
t_Variants * t_Core :: variants( short i_word, long i_struct )
{  return NULL ;  }
void t_Core :: universe( short i_word, long i_struct )
{ ; }
char * t_Core :: get_meaning( t_rStruct *V )
{  return NULL ;  }
t_rStruct * t_Core :: variant( t_rWord R )
{
   return NULL ;
}
void t_Core :: set_meaning( t_rStruct *V, char *Str )
{ ;
}
void t_Core :: real_param_up( t_rStruct *V )
{ ; }
/****************************************************************************/
t_Antwort :: t_Antwort( void )
{
   Str[0]=0 ;
}
/****************************************************************************/
t_rWord :: t_rWord( )
{
   type        = TNULL ;
   i_word      =-1 ;
   i_last_word =-1 ;
   i_struct    =-10;
   i_variant   =-1 ;
   index       =-1 ;
   i_slowo     =-1 ;
}
/***************************************************************************/
t_rStruct :: t_rStruct( void )
{
   type        = TNULL ;
   i_word      =-1 ;
   i_last_word =-1 ;
   i_struct    =-10;
   r_word      =-1 ;
   i_slowo     =-1 ;

   Form.init() ;
   i_meaning   =-1 ;
}
/****************************************************************************/
t_Tree :: t_Tree( void )
{  init( );  }
t_Tree :: ~t_Tree( void )
{  del( );  }
/***************************************************************************/
void t_Tree :: init( void )
{
   type  = TSTRUCT ;
   up    =-1 ;
   down  =-1 ;
   n_down= 0 ;
   i_word= 0 ;
   i_last_word=-1 ;
   i_struct=0 ;
   select=-1 ;
   link  =-1 ;
   index =-1 ;
   Form.init();
   i_slowo=-1 ;
   Str[0]=Str1[0]=0 ;
}
void t_Tree :: del( void )
{ ; }
/****************************************************************************/
DEFINE_LIST_BTYPE( t_aWord,t_aWordList )
DEFINE_LIST_BTYPE( t_rStruct,t_rStructList )
DEFINE_LIST_BTYPE( t_rWord,t_rWordList )
DEFINE_LIST_BTYPE( t_Item, t_ItemList )
DEFINE_LIST_TYPE ( t_Tree, t_TreeList )

