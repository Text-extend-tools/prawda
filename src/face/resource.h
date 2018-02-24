/*******************************************************************/
//              "Меркурий"-"Правда" - open source переводчик
//          распространяется в соответсвии с лицензией GNU v 2.0
//
//                 действия, текстового редактора
//     Анисимов Д.В.                               сегодня
/*******************************************************************/

# ifndef RESOURSE_H
# define RESOURSE_H

enum E_Function {
   F_Nop,
   F_FileOpen,
   F_FileSave,
   F_FileSaveAs,
   F_FileClose,
   F_Exit,
   F_Find,
   F_FindForward,
   F_FindBackward,
   F_Replace,
   F_GoToLine,
   F_KodRusLat,
   F_KodLatRus,
   F_KodToUp,
   F_KodToDown,
   F_KodDosKoi,
   F_KodWinKoi,
   F_KodDosWin,
   F_KodKoiDos,
   F_KodKoiWin,
   F_KodWinDos,
   F_WindowMove,
   F_WindowGo,
   F_WindowChange,
   F_WindowMax,
   F_Up,
   F_Down,
   F_Left,
   F_Right,
   F_Home,
   F_End,
   F_PageUp,
   F_PageDown,
   F_BeginText,
   F_EndText,
   F_MUp,
   F_MDown,
   F_MLeft,
   F_MRight,
   F_MHome,
   F_MEnd,
   F_MPageUp,
   F_MPageDown,
   F_MBeginText,
   F_MEndText,
   F_Cat,
   F_Copy,
   F_IncCopy,
   F_Paste,
   F_Insert,
   F_BackSpase,
   F_Delete,
   F_DelStr,
   F_ShiftRight,
   F_ShiftLeft,
   F_PrevFrasa,
   F_NextFrasa,
   F_Mark,
   F_Unmark,
   F_Menu
};

/* ----------------------------------------------------------------------- */
struct t_Resource
{
public:
   char translate( short shift, short key1, short key2 );
};

# endif
