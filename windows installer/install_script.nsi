; Script generated by the HM NIS Edit Script Wizard.

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "Block Attack - Rise Of the Blocks"
!define PRODUCT_VERSION "2.9.0"
!define PRODUCT_PUBLISHER "Poul Sander"
!define PRODUCT_WEB_SITE "http://www.blockattack.net"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\blockattack.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_STARTMENU_REGVAL "NSIS:StartMenuDir"

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "../source/misc/icons/blockattack.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
;!insertmacro MUI_PAGE_LICENSE "..\Game\COPYING.TXT"
; Components page
!insertmacro MUI_PAGE_COMPONENTS
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Start menu page
var ICONS_GROUP
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "Block Attack - Rise Of the Blocks"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${PRODUCT_STARTMENU_REGVAL}"
!insertmacro MUI_PAGE_STARTMENU Application $ICONS_GROUP
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\blockattack.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "Setup.exe"
InstallDir "$PROGRAMFILES\Block Attack - Rise Of the Blocks"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "Game Files" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite try
  File "..\Game\blockattack.exe"
  File "..\Game\blockattack.data"
  SetOutPath "$INSTDIR"
  File "..\Game\COPYING.TXT"
  SetOutPath "$INSTDIR\mods"
  File "..\Game\mods\1.3.0.bricks.data"
  SetOutPath "$INSTDIR\locale\da\LC_MESSAGES"
  File "../source/misc/translation/locale/da/LC_MESSAGES/blockattack_roftb.mo"
  SetOutPath "$INSTDIR"

; Shortcuts
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateDirectory "$SMPROGRAMS\$ICONS_GROUP"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Block Attack - Rise Of the Blocks.lnk" "$INSTDIR\blockattack.exe"
  CreateShortCut "$DESKTOP\Block Attack - Rise Of the Blocks.lnk" "$INSTDIR\blockattack.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section -AdditionalIcons
  SetOutPath $INSTDIR
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk" "$INSTDIR\uninst.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\blockattack.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\blockattack.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC01} "The Game files, must be installed!"
!insertmacro MUI_FUNCTION_DESCRIPTION_END


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  !insertmacro MUI_STARTMENU_GETFOLDER "Application" $ICONS_GROUP
  Delete "$INSTDIR\blockattack.data"
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\docs\block_attack_manual.pdf"
  Delete "$INSTDIR\COPYING.TXT"
  Delete "$INSTDIR\gfx\counter\1.png"
  Delete "$INSTDIR\gfx\counter\2.png"
  Delete "$INSTDIR\gfx\counter\3.png"
  Delete "$INSTDIR\gfx\smileys\0.png"
  Delete "$INSTDIR\gfx\smileys\1.png"
  Delete "$INSTDIR\gfx\smileys\2.png"
  Delete "$INSTDIR\gfx\smileys\3.png"
  Delete "$INSTDIR\gfx\animations\cursor\2.png"
  Delete "$INSTDIR\gfx\animations\cursor\1.png"
  Delete "$INSTDIR\gfx\animations\bomb\bomb_1.png"
  Delete "$INSTDIR\gfx\animations\bomb\bomb_2.png"
  Delete "$INSTDIR\gfx\animations\explosion\0.png"
  Delete "$INSTDIR\gfx\animations\explosion\1.png"
  Delete "$INSTDIR\gfx\animations\explosion\2.png"
  Delete "$INSTDIR\gfx\animations\explosion\3.png"
  Delete "$INSTDIR\gfx\animations\ready\ready_1.png"
  Delete "$INSTDIR\gfx\animations\ready\ready_2.png"
  Delete "$INSTDIR\gfx\garbage\garbageMR.png"
  Delete "$INSTDIR\gfx\garbage\garbageML.png"
  Delete "$INSTDIR\gfx\garbage\garbageTL.png"
  Delete "$INSTDIR\gfx\garbage\garbageT.png"
  Delete "$INSTDIR\gfx\garbage\garbageR.png"
  Delete "$INSTDIR\gfx\garbage\garbageM.png"
  Delete "$INSTDIR\gfx\garbage\garbageL.png"
  Delete "$INSTDIR\gfx\garbage\garbageFill.png"
  Delete "$INSTDIR\gfx\garbage\garbageBR.png"
  Delete "$INSTDIR\gfx\garbage\garbageBL.png"
  Delete "$INSTDIR\gfx\garbage\garbageB.png"
  Delete "$INSTDIR\gfx\garbage\garbageTR.png"
  Delete "$INSTDIR\gfx\garbage\garbageGMR.png"
  Delete "$INSTDIR\gfx\garbage\garbageGML.png"
  Delete "$INSTDIR\gfx\garbage\garbageGM.png"
  Delete "$INSTDIR\gfx\balls\ballYellow.png"
  Delete "$INSTDIR\gfx\balls\ballTurkish.png"
  Delete "$INSTDIR\gfx\balls\ballPurple.png"
  Delete "$INSTDIR\gfx\balls\ballGreen.png"
  Delete "$INSTDIR\gfx\balls\ballGray.png"
  Delete "$INSTDIR\gfx\balls\ballRed.png"
  Delete "$INSTDIR\gfx\balls\ballBlue.png"
  Delete "$INSTDIR\gfx\bTheme.png"
  Delete "$INSTDIR\gfx\bRetry.png"
  Delete "$INSTDIR\gfx\bNext.png"
  Delete "$INSTDIR\gfx\bSkip.png"
  Delete "$INSTDIR\gfx\chainFrame.png"
  Delete "$INSTDIR\gfx\fileDialogbox.png"
  Delete "$INSTDIR\gfx\iGameOver.png"
  Delete "$INSTDIR\gfx\background.png"
  Delete "$INSTDIR\gfx\blackBackGround.png"
  Delete "$INSTDIR\gfx\bricks\green.png"
  Delete "$INSTDIR\gfx\bricks\blue.png"
  Delete "$INSTDIR\gfx\bricks\yellow.png"
  Delete "$INSTDIR\gfx\transCover.png"
  Delete "$INSTDIR\gfx\bricks\turkish.png"
  Delete "$INSTDIR\gfx\bricks\red.png"
  Delete "$INSTDIR\gfx\bricks\purple.png"
  Delete "$INSTDIR\gfx\bricks\grey.png"
  Delete "$INSTDIR\gfx\blockAttack.ico"
  Delete "$INSTDIR\gfx\background.jpg"
  Delete "$INSTDIR\gfx\14P_Arial_Angle_Red.png"
  Delete "$INSTDIR\gfx\24P_Arial_Blue.png"
  Delete "$INSTDIR\gfx\bVsGame.png"
  Delete "$INSTDIR\gfx\bVsGameConfig.png"
  Delete "$INSTDIR\gfx\iLoser.png"
  Delete "$INSTDIR\gfx\bTwoPlayers.png"
  Delete "$INSTDIR\gfx\bTimeTrial.png"
  Delete "$INSTDIR\gfx\bStageClear.png"
  Delete "$INSTDIR\gfx\bPuzzle.png"
  Delete "$INSTDIR\gfx\boardBackBack.png"
  Delete "$INSTDIR\gfx\bOnePlayer.png"
  Delete "$INSTDIR\gfx\dialogbox.png"
  Delete "$INSTDIR\gfx\iLevelCheck.png"
  Delete "$INSTDIR\gfx\iLevelCheckBox.png"
  Delete "$INSTDIR\gfx\bOn.png"
  Delete "$INSTDIR\gfx\bNewGame.png"
  Delete "$INSTDIR\gfx\bNetwork.png"
  Delete "$INSTDIR\gfx\iWinner.png"
  Delete "$INSTDIR\gfx\iStageClearLimit.png"
  Delete "$INSTDIR\gfx\bInternet.png"
  Delete "$INSTDIR\gfx\options.png"
  Delete "$INSTDIR\gfx\BackBoard.png"
  Delete "$INSTDIR\gfx\bSelectPuzzle.png"
  Delete "$INSTDIR\gfx\bHighScore.png"
  Delete "$INSTDIR\gfx\bForward.png"
  Delete "$INSTDIR\gfx\bReplays.png"
  Delete "$INSTDIR\gfx\bSave.png"
  Delete "$INSTDIR\gfx\bLoad.png"
  Delete "$INSTDIR\gfx\bNetwork.png"
  Delete "$INSTDIR\gfx\bHost.png"
  Delete "$INSTDIR\gfx\bConnect.png"
  Delete "$INSTDIR\gfx\bExit.png"
  Delete "$INSTDIR\gfx\bEndless.png"
  Delete "$INSTDIR\gfx\bConnect.png"
  Delete "$INSTDIR\gfx\bConfigure.png"
  Delete "$INSTDIR\gfx\bBack.png"
  Delete "$INSTDIR\gfx\icon.png"
  Delete "$INSTDIR\gfx\bChange.png"
  Delete "$INSTDIR\gfx\bHost.png"
  Delete "$INSTDIR\gfx\iCheckBoxArea.png"
  Delete "$INSTDIR\gfx\blackLine.png"
  Delete "$INSTDIR\gfx\b1024.png"
  Delete "$INSTDIR\gfx\bOptions.png"
  Delete "$INSTDIR\gfx\changeButtonsBack.png"
  Delete "$INSTDIR\gfx\crossover.png"
  Delete "$INSTDIR\gfx\iDraw.png"
  Delete "$INSTDIR\gfx\bOff.png"
  Delete "$INSTDIR\gfx\mouse.png"
  Delete "$INSTDIR\mods\1.3.0.bricks.data"
  Delete "$INSTDIR\music\bgMusic.ogg"
  Delete "$INSTDIR\music\highbeat.ogg"
  Delete "$INSTDIR\res\puzzle.levels"
  Delete "$INSTDIR\res\copy"
  Delete "$INSTDIR\res\testPuzzles"
  Delete "$INSTDIR\puzzles\puzzle.levels"
  Delete "$INSTDIR\puzzles\copy"
  Delete "$INSTDIR\puzzles\testPuzzles"
  Delete "$INSTDIR\sound\counter.ogg"
  Delete "$INSTDIR\sound\counterFinal.ogg"
  Delete "$INSTDIR\sound\typing.ogg"
  Delete "$INSTDIR\sound\cameraclick.ogg"
  Delete "$INSTDIR\sound\pop.ogg"
  Delete "$INSTDIR\zlib.dll"
  Delete "$INSTDIR\zlib1.dll"
  Delete "$INSTDIR\jpeg.dll"
  Delete "$INSTDIR\SDL.dll"
  Delete "$INSTDIR\SDL_mixer.dll"
  Delete "$INSTDIR\SDL_image.dll"
  Delete "$INSTDIR\libpng1.dll"
  Delete "$INSTDIR\block_attack.exe"
  Delete "$INSTDIR\blockattack.exe"
  Delete "$INSTDIR\gmon.out"
  Delete "$INSTDIR\stderr.txt"
  Delete "$INSTDIR\stdout.txt"

  Delete "$SMPROGRAMS\$ICONS_GROUP\Uninstall.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Website.lnk"
  Delete "$DESKTOP\Block Attack - Rise Of the Blocks.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Block Attack - Rise Of the Blocks.lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\Manual.lnk"

  RMDir "$SMPROGRAMS\$ICONS_GROUP"
  RMDir "$INSTDIR\sound"
  RMDir "$INSTDIR\res"
  RMDir "$INSTDIR\puzzles"
  RMDir "$INSTDIR\music"
  RMDir "$INSTDIR\gfx\garbage"
  RMDir "$INSTDIR\gfx\counter"
  RMDir "$INSTDIR\gfx\smileys"
  RMDir "$INSTDIR\gfx\bricks"
  RMDir "$INSTDIR\gfx\balls"
  RMDir "$INSTDIR\gfx\animations\cursor"
  RMDir "$INSTDIR\gfx\animations\bomb"
  RMDir "$INSTDIR\gfx\animations\explosion"
  RMDir "$INSTDIR\gfx\animations\ready"
  RMDir "$INSTDIR\gfx\animations"
  RMDir "$INSTDIR\gfx"
  RMDir "$INSTDIR\docs"
  RMDir "$INSTDIR\mods"
  RMDir /r "$INSTDIR\locale"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd
