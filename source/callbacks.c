#include <gtk/gtk.h>
#include "utils.h"
#include<libintl.h> //per gettex
#include<locale.h>	//per gettex
#define _(String) gettext (String) //per gettex

#ifdef _WIN32 
#include "windows.h" 
#endif


gboolean ServerMode = FALSE;
extern GObject *entryHost;
extern GObject *entryPort;
extern GObject *toggleServer;
extern GObject *MainWindow;
extern GObject *StartStop;
gint State=0; 
gchar* WinVNC;
gchar* publicIp="";

void StartStopConnection(GtkWidget *widget, gpointer user_data)
{
	GError *error = NULL;
	gchar *cmd;
	const gchar *host=gtk_entry_get_text (GTK_ENTRY(entryHost));
	const gchar *port=gtk_entry_get_text (GTK_ENTRY(entryPort));
	
	#ifdef _WIN32
	//Copy WinVNC.exe to temp dir
	WinVNC=g_build_filename(g_get_tmp_dir(),"WinVNC.exe", NULL);
	GFile*  mySRC =  g_file_new_for_uri("resource:///org/pobvnc/res/bin-win32/WinVNC.exe");
	GFile*  myDEST =  g_file_new_for_path(WinVNC);
	g_file_copy (mySRC,  myDEST,  G_FILE_COPY_OVERWRITE, NULL, NULL,  NULL,    NULL);
	
	//Copy VNCHooks.dll to temp dir
	gchar* VNCHooks=g_build_filename(g_get_tmp_dir(),"VNCHooks.dll", NULL);
	GFile*  mySRC2 =  g_file_new_for_uri("resource:///org/pobvnc/res/bin-win32/VNCHooks.dll");
	GFile*  myDEST2 =  g_file_new_for_path(VNCHooks);
	g_file_copy (mySRC2,  myDEST2,  G_FILE_COPY_OVERWRITE, NULL, NULL,  NULL,    NULL);
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
			cmd=g_strdup_printf("%s -connect %s::%s", WinVNC,host, port);
			WinExec(WinVNC, NULL); 
			WinExec(cmd, NULL);
			#endif
			
		}else{
			//Start Server mode
			#ifdef linux
			cmd=g_strdup_printf("vncviewer -listen %s", port);
			g_print("%s\n",cmd);
			g_spawn_command_line_async (cmd, &error);
			if (error!=NULL)
			{
				 err_message(MainWindow,"Problemi con vncviewer",error->message,"Errore fatale");
				 g_error_free (error);
				 abortConnection();
				 return ;
			}
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
		 gchar* cmd=g_strdup_printf("%s -kill", WinVNC);
		 WinExec(cmd,NULL);
	}else{
		//server
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
