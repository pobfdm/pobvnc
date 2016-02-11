#include <gtk/gtk.h>
#include "utils.h"
#include<libintl.h> //per gettex
#include<locale.h>	//per gettex
#define _(String) gettext (String) //per gettex

#ifdef _WIN32 
	#include <windows.h>
#endif

#define VERSION "0.1"
#define PROJECTURL "http://www.freemedialab.org"


gboolean ServerMode = FALSE;
extern GObject *entryHost;
extern GObject *entryPort;
extern GObject *toggleServer;
extern GObject *MainWindow;
extern GObject *StartStop;
extern GObject *aboutWin;
gint State=0; 
gchar* winvnc4;
gchar* vncviewer;
gchar* publicIp="";
extern gchar* 	logo_file ;
extern binPath;
gchar* logfile;
gboolean checkLog;

gboolean checkConnectionStatus(){
    
    if (checkLog==FALSE) return;
    g_print("Checking \n");
    const gchar *host=gtk_entry_get_text (GTK_ENTRY(entryHost));
	const gchar *port=gtk_entry_get_text (GTK_ENTRY(entryPort));
    gchar* m;
    
    
    gchar** logContents;
    g_file_get_contents (logfile,&logContents, NULL,NULL);
    g_print(logContents);
	
	#ifdef linux
	//If server accept connection
	gchar* reverse_connect_ok=g_strdup_printf("reverse_connect: %s:%s/%s OK", host, port,host);
	if (g_strrstr (logContents, reverse_connect_ok)!=NULL)
	{
		m=g_strdup_printf(_("Connect to %s port %s"),host, port);
		setGreenStatus(m);
	}
	
	
	//If server stop connection
	if (g_strrstr (logContents, "viewer exited")!=NULL)
	{
		checkLog=FALSE;
		abortConnection();
		setGreenStatus(_("Connection closed."));
	}
	
	//If server is not up
	reverse_connect_ok=g_strdup_printf("reverse_connect: %s:%s failed", host, port);
	if (g_strrstr (logContents, reverse_connect_ok)!=NULL)
	{
		checkLog=FALSE;
		abortConnection();
		setRedStatus(_("Connection failed."));
	}
	
	#endif
	
	return checkLog ;
}


void StartStopConnection(GtkWidget *widget, gpointer user_data)
{
	GError *error = NULL;
	gchar *cmd;
	const gchar *host=gtk_entry_get_text (GTK_ENTRY(entryHost));
	const gchar *port=gtk_entry_get_text (GTK_ENTRY(entryPort));
	logfile=g_build_filename(g_get_tmp_dir(),"pobvnc.log", NULL);
	gchar* m;
	
	
	
	if (g_strcmp0 (host,"")==0 && ServerMode==FALSE)
	{
		err_message(MainWindow,_("Please, insert host"),_("The host field should not be empty"),_("Error"));
		return ;
	}
	if (g_strcmp0 (port,"")==0)
	{
		err_message(MainWindow,_("Please, insert port"),_("The port field should not be empty"),_("Error"));
		return ;
	}
	sleep(1);
	
	#ifdef _WIN32
	//Copy winvnc4.exe to temp dir
	winvnc4=g_build_filename(g_get_tmp_dir(),"winvnc4.exe", NULL);
	GFile*  mySRC =  g_file_new_for_uri("resource:///org/pobvnc/res/bin-win32/winvnc4.exe");
	GFile*  myDEST =  g_file_new_for_path(winvnc4);
	g_file_copy (mySRC,  myDEST,  G_FILE_COPY_OVERWRITE, NULL, NULL,  NULL,    NULL);
	
	//Copy wm_hooks.dll to temp dir
	gchar* wm_hooks=g_build_filename(g_get_tmp_dir(),"wm_hooks.dll", NULL);
	GFile*  mySRC2 =  g_file_new_for_uri("resource:///org/pobvnc/res/bin-win32/wm_hooks.dll");
	GFile*  myDEST2 =  g_file_new_for_path(wm_hooks);
	g_file_copy (mySRC2,  myDEST2,  G_FILE_COPY_OVERWRITE, NULL, NULL,  NULL,    NULL);
	
	
	//Copy vncviewer.exe to temp dir
	vncviewer=g_build_filename(g_get_tmp_dir(),"vncviewer.exe", NULL);
	GFile*  mySRC3 =  g_file_new_for_uri("resource:///org/pobvnc/res/bin-win32/vncviewer.exe");
	GFile*  myDEST3 =  g_file_new_for_path(vncviewer);
	g_file_copy (mySRC3,  myDEST3,  G_FILE_COPY_OVERWRITE, NULL, NULL,  NULL,    NULL);
	#endif
	
	
	
	if (State==0) //Not Connected
	{	
		if (ServerMode==FALSE)
		{
			//Start Client Mode
			setGreenStatus(_("Connection..."));
			
			//start Client log checking
			checkLog=TRUE;
			g_timeout_add(2000, checkConnectionStatus, NULL);
			
			#ifdef linux
			g_remove (logfile);
			cmd=g_strdup_printf("x11vnc -noxdamage -ncache 10 -connect_or_exit %s:%s -o %s", host, port,logfile);
			g_spawn_command_line_async (cmd, &error);
			if (error!=NULL)
			{
				 err_message(MainWindow,"Problemi con x11vnc",error->message,"Errore fatale");
				 g_error_free (error);
				 abortConnection();
				 return ;
			}
			
			#endif
			#ifdef _WIN32
			
			cmd=g_strdup_printf("%s -SecurityTypes None -Log *:stdout:100", winvnc4);
			WinExec(cmd, SW_SHOWNORMAL);
			cmd=g_strdup_printf("%s -connect %s:%s", winvnc4,host, port); 
			WinExec(cmd, NULL);
			#endif
			
		}else{
			//Start Server mode
			m=g_strdup_printf(_("Listen to port %s"), port);
			setGreenStatus(m);
			#ifdef linux
			cmd=g_strdup_printf("vncviewer  -listen %s", port);
			g_spawn_command_line_async (cmd, &error);
			if (error!=NULL)
			{
				 err_message(MainWindow,"Problemi con vncviewer",error->message,"Errore fatale");
				 g_error_free (error);
				 abortConnection();
				 return ;
			}
			#endif
			#ifdef _WIN32
			cmd=g_strdup_printf("%s  -listen %s", vncviewer,port);
			WinExec(cmd, SW_SHOWNORMAL); 
			#endif
		}
		State=1;
		gtk_button_set_label (GTK_BUTTON(widget), "gtk-disconnect");
		gtk_widget_set_sensitive(GTK_WIDGET(toggleServer), FALSE);
		
		
	}else{	//State ==1
		
		abortConnection();
	}	



}//end


void abortConnection()
{
	checkLog=FALSE;
	
	#ifdef linux
	if (ServerMode==FALSE) 
	{
		g_spawn_command_line_sync ("x11vnc -remote stop", NULL, NULL, NULL,NULL);
	}else{
		g_spawn_command_line_sync ("killall vncviewer", NULL, NULL, NULL,NULL);
		
	}
	#endif
	#ifdef _WIN32
	if (ServerMode==FALSE) 
	{
		 //Client
		 //gchar* cmd=g_strdup_printf("%s -DisconnectClients", winvnc4);
		 //WinExec(cmd,NULL); 
		 WinExec("taskkill /im winvnc4.exe /f",NULL);
		 
	}else{
		//server
		WinExec("taskkill /im vncviewer.exe /f",NULL);
	}
	#endif
	State=0;
	gtk_button_set_label (GTK_BUTTON(StartStop), "gtk-connect");
	gtk_widget_set_sensitive(GTK_WIDGET(toggleServer), TRUE);
	g_print("Abort Connection...");
	setGreenStatus(_("Disconnected."));
}


void isServer(GtkToggleButton* toggle, gpointer user_data)
{
	if (gtk_toggle_button_get_active (toggle)==TRUE)
	{
		g_print("Server mode...\n");
		ServerMode = TRUE;
		gtk_widget_set_sensitive(GTK_WIDGET(entryHost), FALSE);
		#ifdef linux 
		if (getBitsCpu()==64) getPublicIp(); 
		#endif     
	}else{
		g_print("Client mode...\n");
		gtk_widget_set_sensitive(GTK_WIDGET(entryHost), TRUE);
		gtk_entry_set_text(GTK_ENTRY(entryHost),"");
		ServerMode = FALSE;
	}
}


void getPublicIp()
{
	GError* error;
	gchar* publicIpFile=g_build_filename(g_get_tmp_dir(),"publicIp.ini", NULL);
	GFile*  mySRC =  g_file_new_for_uri("http://www.freemedialab.org/myip/index.php");
	GFile*  myDEST =  g_file_new_for_path(publicIpFile);
	g_file_copy (mySRC,  myDEST,  G_FILE_COPY_OVERWRITE, NULL, NULL,  NULL,    &error);
	if (error!=NULL)
	{
		 err_message(MainWindow,"Problemi nel ricevere ip pubblico",error->message,"Errore fatale");
		 g_error_free (error);
		 //publicIp=g_strdup_printf("%s", "");
		 return;
	}
	publicIp=GetKey(publicIpFile,"Connessione" ,"IP");
	g_print(publicIpFile);
	gtk_entry_set_text(GTK_ENTRY(entryHost),publicIp);
}


void aboutDialog()
{
	g_print("About\n");
	gtk_about_dialog_set_version (GTK_ABOUT_DIALOG(aboutWin),  VERSION);
	gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (aboutWin), PROJECTURL);
	
	GdkPixbuf *logo = gdk_pixbuf_new_from_file (logo_file, NULL);
	gtk_about_dialog_set_logo(GTK_DIALOG (aboutWin), logo);
	
	gtk_dialog_run (GTK_DIALOG (aboutWin));
	gtk_widget_hide (GTK_DIALOG (aboutWin));
	
}

void installRemove()
{
	
#ifdef linux
	gchar* strDesktopFile="[Desktop Entry] \n\
Type=Application\n\
Name=Pobvnc\n\
GenericName=Get Help!\n\
Comment=Help a friend!\n\
Exec=%s/.local/bin/pobvnc\n\
Icon=%s/.local/share/icons/lifesaver.svg\n\
Terminal=FALSE\n\
StartupNotify=TRUE\n\
Categories=Network;Utility;RemoteAccess;\n\
";
	strDesktopFile=g_strdup_printf(strDesktopFile, g_get_home_dir (),g_get_home_dir ());
	gchar* pobvncBinFile=g_build_filename(g_get_home_dir (),".local/bin/pobvnc",NULL);
	gchar* pobvncDesktopFile=g_build_filename(g_get_home_dir (),".local/share/applications/pobvnc.desktop",NULL);
	gchar* pobvncIconFile=g_build_filename(g_get_home_dir (),".local/share/icons/lifesaver.svg",NULL);
	
	gchar* iconsDir=g_build_filename(g_get_home_dir (),".local/share/icons/",NULL);
	gchar* binDir=g_build_filename(g_get_home_dir (),".local/bin/",NULL);
	
	gchar* desktopDir=g_get_user_special_dir (G_USER_DIRECTORY_DESKTOP);
	gchar* fileInDesktop=g_strdup_printf("%s/pobvnc.desktop", desktopDir);
	gint r;
	
	if (!g_file_test (pobvncBinFile, G_FILE_TEST_EXISTS))
	{
		r = info_YesNo(MainWindow,_("Do you want to install Pobvnc?"),_("Only local installation"), _("Installation"));
		if (r==GTK_RESPONSE_NO) return;
		
		
		g_print("Installer begin...");
		if (!g_file_test (binDir, G_FILE_TEST_EXISTS)) g_mkdir(binDir, 0755);
		if (!g_file_test (iconsDir, G_FILE_TEST_EXISTS)) g_mkdir(iconsDir, 0755);
		
		//Write .destop file in $HOME/.local/share/applications/
		g_file_set_contents (pobvncDesktopFile,strDesktopFile, -1,NULL);
		g_chmod (pobvncDesktopFile,0755);
		
		//Write .destop file in desktop
		g_file_set_contents (fileInDesktop,strDesktopFile, -1,NULL);
		g_chmod (fileInDesktop,0755);
		
		//Copy icon file
		GFile*  mySRC =  g_file_new_for_uri("resource:///org/pobvnc/res/lifesaver.svg");
		GFile*  myDEST =  g_file_new_for_path(pobvncIconFile);
		g_file_copy (mySRC,  myDEST,  G_FILE_COPY_OVERWRITE, NULL, NULL,  NULL,    NULL);
		
		//Copy bin file
		GFile*  mySRC2 =  g_file_new_for_path(binPath);
		GFile*  myDEST2 =  g_file_new_for_path(pobvncBinFile);
		g_file_copy (mySRC2,  myDEST2,  G_FILE_COPY_OVERWRITE, NULL, NULL,  NULL,    NULL);
		g_chmod (pobvncBinFile,0755);
		
		info_message(MainWindow,_("Installation complete."),_("Pobvnc is in your app menu."),_("Installation"));
		
	}else{
		r = info_YesNo(MainWindow,_("Do you want to uninstall Pobvnc?"),_("Remove all"), _("Unstallation"));
		if (r==GTK_RESPONSE_NO) return;
		
		g_print("Disinstaller...");
		g_remove (pobvncBinFile);
		g_remove (pobvncDesktopFile);
		g_remove (pobvncIconFile);
		g_remove (fileInDesktop);
		
		info_message(MainWindow,_("Remove complete."),_("Pobvnc is not in your app menu."),_("Remove"));
		
	}
#endif

#ifdef _WIN32

#endif

}




