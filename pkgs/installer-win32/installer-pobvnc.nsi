!include "MUI.nsh"

; Il nome dell'installatore
Name "Pobvnc"

;Icona del file dell'installatore
!define MUI_ICON lifesaver.ico


;Banner laterale
!define MUI_WELCOMEFINISHPAGE_BITMAP "banner.bmp" ;immagine per la pagina di benvenuto
!define MUI_HEADERIMAGE							  ;Usa immagine d'intestazione
!define MUI_HEADERIMAGE_BITMAP "header.bmp"		  ;immagine d'intestazione per le varie schermate	

; Il nome del file dell'installatore
OutFile "pobvnc-setup.exe"

;Le varie "pagine" da visualizzare (Benvenuto, licenza, scelta cartella,conclusione etc...)
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "LICENSE"
!insertmacro MUI_PAGE_DIRECTORY
;!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

;Lingue nelle quali deve essere disponibile l'installer
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "German"
!insertmacro MUI_LANGUAGE "French"
!insertmacro MUI_LANGUAGE "Italian"



; La directory di installazione predefinita
InstallDir $PROGRAMFILES\pobvnc

; Del testo da mostrare al momento della scelta della directory
;DirText "This will install Logout-fml on your computer. Choose a directory"

;--------------------------------

; La sezione dedicata all'installazione
Section "Install" 

	; Setto la directory dove mettere i file da questo momento in poi
	; (se la directory non esiste, la creera') e copio i file.
	SetOutPath $INSTDIR
	File /r pobvnc_bundle\*.*

	;Cambio directory e copio altri file 
	;SetOutPath $INSTDIR\share
	

	;Menu start
	CreateDirectory "$SMPROGRAMS\Pobvnc"
	CreateShortCut "$SMPROGRAMS\Pobvnc\Pobvnc.lnk" "$INSTDIR\pobvnc.exe"
	CreateShortCut "$SMPROGRAMS\Pobvnc\uninstall.lnk" "$INSTDIR\Uninstall.exe"

	;Desktop shortcut
	CreateShortcut "$desktop\Pobvnc.lnk" "$instdir\pobvnc.exe"
	
	
	; Informiamo il compilatore che sara' presente una sezione per il disinstallatore  
	WriteUninstaller $INSTDIR\Uninstall.exe
	
SectionEnd 



; La sezione dedicata alla disinstallazione
Section "un.Uninstall"

	;Rimuoviamo i file del programma
	Delete $INSTDIR\Uninstall.exe
	RMDir /r $INSTDIR
	
	; Rimuoviamo tutti i link dallo start menu
	Delete "$SMPROGRAMS\Pobvnc\Pobvnc.lnk" 
	Delete "$SMPROGRAMS\Pobvnc\uninstall.lnk" 
	Delete "$desktop\Pobvnc.lnk"
	RMDIR  "$SMPROGRAMS\Pobvnc"

SectionEnd 