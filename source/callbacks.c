#include <gtk/gtk.h>
#include "utils.h"

gboolean ServerMode = FALSE;
extern GObject *entryHost;
extern GObject *entryPort;
extern GObject *toggleServer;
extern GObject *MainWindow;
extern GObject *StartStop;
gint State=0; 


void StartStopConnection(GtkWidget *widget, gpointer user_data)
{
	GError* error = NULL;
	gchar *cmd;
	gchar* host=gtk_entry_get_text (entryHost), *port=gtk_entry_get_text (entryPort);
	
	
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
		
		#ifdef linux
		if (ServerMode==FALSE) g_spawn_command_line_sync ("x11vnc -remote stop", NULL, NULL, NULL,NULL);
		if (ServerMode==TRUE)  g_spawn_command_line_sync ("killall vncviewer", NULL, NULL, NULL,NULL);
		#endif
		
		State=0;
		gtk_button_set_label (GTK_BUTTON(widget), "gtk-connect");
		gtk_widget_set_sensitive(GTK_WIDGET(toggleServer), TRUE);
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
	}else{
		g_print("Client mode...\n");
		ServerMode = FALSE;
	}
}
