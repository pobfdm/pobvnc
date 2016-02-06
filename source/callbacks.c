#include <gtk/gtk.h>
#include "utils.h"
#include<libintl.h> //per gettex
#include<locale.h>	//per gettex
#define _(String) gettext (String) //per gettex

#ifdef _WIN32 
#include "windows.h" 
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

void StartStopConnection(GtkWidget *widget, gpointer user_data)
{
	GError *error = NULL;
	gchar *cmd;
	const gchar *host=gtk_entry_get_text (GTK_ENTRY(entryHost));
	const gchar *port=gtk_entry_get_text (GTK_ENTRY(entryPort));
	
	#ifdef _WIN32
	//Copy WinVNC.exe to temp dir
	winvnc4=g_build_filename(g_get_tmp_dir(),"WinVNC.exe", NULL);
	GFile*  mySRC =  g_file_new_for_uri("resource:///org/pobvnc/res/bin-win32/winvnc4.exe");
	GFile*  myDEST =  g_file_new_for_path(winvnc4);
	g_file_copy (mySRC,  myDEST,  G_FILE_COPY_OVERWRITE, NULL, NULL,  NULL,    NULL);
	
	//Copy VNCHooks.dll to temp dir
	gchar* wm_hooks=g_build_filename(g_get_tmp_dir(),"VNCHooks.dll", NULL);
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
			#ifdef linux
			cmd=g_strdup_printf("x11vnc -ncache 10 -connect_or_exit %s:%s", host, port);
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
			cmd=g_strdup_printf("%s  -connect %s:%s", winvnc4,host, port);
			WinExec(winvnc4, NULL); 
			WinExec(cmd, NULL);
			#endif
			
		}else{
			//Start Server mode
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
		 gchar* cmd=g_strdup_printf("%s -disconnect", winvnc4);
		 WinExec(cmd,NULL); sleep(3);
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
}


void isServer(GtkToggleButton* toggle, gpointer user_data)
{
	if (gtk_toggle_button_get_active (toggle)==TRUE)
	{
		g_print("Server mode...\n");
		ServerMode = TRUE;
		gtk_widget_set_sensitive(GTK_WIDGET(entryHost), FALSE);
		#ifdef linux 
		getPublicIp(); 
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
		 publicIp=g_strdup_printf("%s", "");
		 return;
	}
	
	publicIp=GetKey(publicIpFile,"Connessione" ,"IP");
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
