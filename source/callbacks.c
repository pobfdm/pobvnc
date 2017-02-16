#include <gtk/gtk.h>
#include "utils.h"
#include<libintl.h> //per gettex
#include<locale.h>	//per gettex
#define _(String) gettext (String) //per gettex

#if defined _WIN32 || defined __CYGWIN__
	#include <windows.h>
#endif

#define VERSION "0.2"
#define PROJECTURL "https://github.com/pobfdm/pobvnc"


typedef enum {NONE,EDIT, NEW} opBookmarks;
opBookmarks  operationBookmarks=NONE;

gboolean ServerMode = FALSE;
extern GObject *entryHost;
extern GObject *entryPort;
extern GObject *toggleServer;
extern GObject *MainWindow;
extern GObject *StartStop;
extern GObject *aboutWin;
extern GObject *bookmarksWindow, *editBookmarkWindow;
extern GObject *treeviewBookmarks;
extern GtkTreeModel *bookmarksModel;
extern GtkTreeIter  iterBookmarks; 
extern GObject *entryLabelBookmark, *entryHostBookmark, *entryPortBookmark  ;
extern GObject **btCancelBookmark, *btSaveBookmark;
gchar* labelBookmark;
gint State=0; 
gchar* winvnc4;
gchar* vncviewer;
extern gchar* 	logo_file ;
extern binPath;
gchar* logfile;
gboolean checkLog;

gboolean checkClientConnectionStatus()
{
    if (checkLog==FALSE) return;
    
    #if defined _WIN32 || defined __CYGWIN__
    //do not try c:\temp\WinVNC4.log even if it was not possible to create it
	gchar* winLogDir=g_build_filename("c:","temp", NULL);
	if (!g_file_test (winLogDir, G_FILE_TEST_EXISTS)) return;
    #endif
    
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
		m=g_strdup_printf(_("Connected to %s port %s"),host, port);
		setGreenStatus(m);
	}
	
	
	//If server stop connection
	if (g_strrstr (logContents, "viewer exited")!=NULL)
	{
		checkLog=FALSE;
		abortConnection();
		setGreenStatus(_("Connection closed by the server."));
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
	
	#if defined _WIN32 || defined __CYGWIN__
	//If server accept connection
	gchar* reverse_connect_ok=g_strdup_printf("Connections: accepted: %s::%s", host, port);
	if (g_strrstr (logContents, reverse_connect_ok)!=NULL)
	{
		m=g_strdup_printf(_("Connected to %s port %s"),host, port);
		setGreenStatus(m);
	}
	//If server stop connection
	gchar* reverse_connect_ko=g_strdup_printf("Connections: closed: %s::%s", host, port);
	if (g_strrstr (logContents, reverse_connect_ko)!=NULL)
	{
		
		m=g_strdup_printf(_("Connection closed by the server"),host, port);
		checkLog=FALSE;
		abortConnection();
		setGreenStatus(m);
	}
	//If client fail to connect
	gchar* connectFailed=g_strdup_printf("Failed to connect to address %s port %s", host,port );
	if (g_strrstr (logContents, connectFailed)!=NULL)
	{
		
		m=g_strdup_printf(_("Failed to connect to address %s port %s"),host,port);
		checkLog=FALSE;
		abortConnection();
		setRedStatus(m);
	}
	#endif
	
	
	
	return checkLog ;
}

gboolean checkServerConnectionStatus()
{
    if (checkLog==FALSE) return;
    
    #if defined _WIN32 || defined __CYGWIN__
    //do not try c:\temp\vncviewer.log even if it was not possible to create it
	gchar* winLogDir=g_build_filename("c:","temp", NULL);
	if (!g_file_test (winLogDir, G_FILE_TEST_EXISTS)) return;
    #endif
    
    g_print("Checking Server\n");
    const gchar *host=gtk_entry_get_text (GTK_ENTRY(entryHost));
	const gchar *port=gtk_entry_get_text (GTK_ENTRY(entryPort));
    gchar* m;
    
    #ifdef linux
    gchar* vncviewerLog="/tmp/vncviewer.log" ;
    #endif
    #if defined _WIN32 || defined __CYGWIN__
    gchar* vncviewerLog="c:\\temp\\vncviewer.log" ;
    #endif
    
    
    gchar** logContents;
    g_file_get_contents (vncviewerLog,&logContents, NULL,NULL);
    g_print("Tigervnc log:\n%s\n",logContents);
    
    //If server accept connection
	gchar* reverse_connect_ok=g_strdup_printf("initialisation done");
	if (g_strrstr (logContents, reverse_connect_ok)!=NULL)
	{
		m=g_strdup_printf(_("Connected to client"));
		setGreenStatus(m);
	}
	
	//If client stop connection (End of stream)
	gchar* reverse_connect_ko=g_strdup_printf("End of stream" );
	if (g_strrstr (logContents, reverse_connect_ko)!=NULL)
	{
		
		m=g_strdup_printf(_("Connection closed by the client"));
		checkLog=FALSE;
		abortConnection();
		setGreenStatus(m);
	}
	
	//If client stop connection (Connection reset by peer)
	gchar* reset_by_peer=g_strdup_printf("Connection reset by peer" );
	if (g_strrstr (logContents, reset_by_peer)!=NULL)
	{
		
		m=g_strdup_printf(_("Connection closed by the client"));
		checkLog=FALSE;
		abortConnection();
		setGreenStatus(m);
	}
	//If client stop connection (104)
	reset_by_peer=g_strdup_printf("(104)" );
	if (g_strrstr (logContents, reset_by_peer)!=NULL)
	{
		m=g_strdup_printf(_("Connection closed by the client"));
		checkLog=FALSE;
		abortConnection();
		setGreenStatus(m);
	}
	
	
	return checkLog ;
	
}





void StartStopConnection(GtkWidget *widget, gpointer user_data)
{
	GError *error = NULL;
	gchar *cmd;
	const gchar *host=gtk_entry_get_text (GTK_ENTRY(entryHost));
	const gchar *port=gtk_entry_get_text (GTK_ENTRY(entryPort));
	gchar* m;
	
	#ifdef linux
	logfile=g_build_filename(g_get_tmp_dir(),"pobvnc.log", NULL);
	#endif
	#if defined _WIN32 || defined __CYGWIN__
	//Winvnc4.exe has log filename fixed ('c:\temp\WinVNC4.log')
	gchar* winLogDir=g_build_filename("c:","temp", NULL);
	if (!g_file_test (winLogDir, G_FILE_TEST_EXISTS)) g_mkdir(winLogDir, 0755);
	logfile=g_build_filename(winLogDir,"WinVNC4.log", NULL);
	#endif	
	
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
	
	#ifdef __CYGWIN__
	//Copy winvnc4.exe to temp dir
	winvnc4=g_build_filename(g_get_user_config_dir (),"Temp","winvnc4.exe", NULL);
	GFile*  mySRC =  g_file_new_for_uri("resource:///org/pobvnc/res/bin-win32/winvnc4.exe");
	GFile*  myDEST =  g_file_new_for_path(winvnc4);
	g_file_copy (mySRC,  myDEST,  G_FILE_COPY_OVERWRITE, NULL, NULL,  NULL,    NULL);
	
	//Copy wm_hooks.dll to temp dir
	gchar* wm_hooks=g_build_filename(g_get_user_config_dir (),"Temp","wm_hooks.dll", NULL);
	GFile*  mySRC2 =  g_file_new_for_uri("resource:///org/pobvnc/res/bin-win32/wm_hooks.dll");
	GFile*  myDEST2 =  g_file_new_for_path(wm_hooks);
	g_file_copy (mySRC2,  myDEST2,  G_FILE_COPY_OVERWRITE, NULL, NULL,  NULL,    NULL);
	
	
	//Copy vncviewer.exe to temp dir
	vncviewer=g_build_filename(g_get_user_config_dir (),"Temp","vncviewer.exe", NULL);
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
			g_timeout_add(2000, checkClientConnectionStatus, NULL);
			
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
			#if defined _WIN32 || defined __CYGWIN__
			g_remove (logfile);
			cmd=g_strdup_printf("%s -SecurityTypes None -Log *:file:100", winvnc4);
			WinExec(cmd, SW_SHOWNORMAL);
			cmd=g_strdup_printf("%s -connect %s:%s", winvnc4,host, port); 
			WinExec(cmd, NULL);
			#endif
			
		}else{
			//Start Server mode
			m=g_strdup_printf(_("Listen to port %s"), port);
			setGreenStatus(m);
			checkLog=TRUE;
			g_timeout_add(2000, checkServerConnectionStatus, NULL);
			
			#ifdef linux
			g_remove (logfile);
			cmd=g_strdup_printf("vncviewer -listen %s -Log *:file:100", port); // /tmp/vncviewer.log
			//cmd=g_strdup_printf("vncviewer -listen %s ", port);
			g_print("Cmd->%s\n",cmd);
			g_spawn_command_line_async (cmd, &error);
			if (error!=NULL)
			{
				 err_message(MainWindow,"Problemi con vncviewer",error->message,"Errore fatale");
				 g_error_free (error);
				 abortConnection();
				 return ;
			}
			#endif
			#if defined _WIN32 || defined __CYGWIN__
			//cmd=g_strdup_printf("%s  -listen %s -Log *:file:100", vncviewer,port);
			//WinExec(cmd, SW_SHOWNORMAL);
			
			//cmd=g_strdup_printf("start /B %s  -listen %s -Log *:file:100", vncviewer,port);
			//system(cmd);
			cmd=g_strdup_printf("%s  -listen %s -Log *:file:100", vncviewer,port);
			StartProcess(cmd);
			#endif
		}
		State=1;
		gtk_button_set_label (GTK_BUTTON(widget), _("Disconnect"));
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
	#if defined _WIN32 || defined __CYGWIN__
	if (ServerMode==FALSE) 
	{
		 //Client
		 WinExec("taskkill /im winvnc4.exe /f",NULL);
		 
	}else{
		//server
		WinExec("taskkill /im vncviewer.exe /f",NULL);
	}
	#endif
	State=0;
	gtk_button_set_label (GTK_BUTTON(StartStop), _("Connect"));
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
		#if defined linux || __CYGWIN__ 
		getPublicIp();
		#endif     
	}else{
		g_print("Client mode...\n");
		gtk_widget_set_sensitive(GTK_WIDGET(entryHost), TRUE);
		gtk_entry_set_text(GTK_ENTRY(entryHost),"");
		ServerMode = FALSE;
	}
}

void showEditBookmarkWindow()
{
	operationBookmarks=EDIT;
	gchar* bookmarksFilePath=g_build_filename(g_get_user_config_dir(),"pobvnc","bookmarks.conf",NULL);
	gchar* label=labelBookmark;
	gchar* host=GetKey(bookmarksFilePath,labelBookmark,"host");
	gchar* port=GetKey(bookmarksFilePath,labelBookmark,"port");
	gtk_entry_set_text(GTK_ENTRY(entryLabelBookmark),labelBookmark);
	gtk_entry_set_text(GTK_ENTRY(entryHostBookmark),host);
	gtk_entry_set_text(GTK_ENTRY(entryPortBookmark),port);
	
	gtk_widget_set_visible(GTK_WIDGET(editBookmarkWindow),TRUE);
}

void showNewBookmarkWindow()
{
	operationBookmarks=NEW;
	gtk_entry_set_text(GTK_ENTRY(entryLabelBookmark),"");
	gtk_entry_set_text(GTK_ENTRY(entryHostBookmark),"");
	gtk_entry_set_text(GTK_ENTRY(entryPortBookmark),"");
	gtk_widget_set_visible(GTK_WIDGET(editBookmarkWindow),TRUE);
	
}

void hideEditBookmarkWindow()
{
	gtk_entry_set_text(GTK_ENTRY(entryLabelBookmark),"");
	gtk_entry_set_text(GTK_ENTRY(entryHostBookmark),"");
	gtk_entry_set_text(GTK_ENTRY(entryPortBookmark),"");
	gtk_widget_set_visible(GTK_WIDGET(editBookmarkWindow),FALSE);
}

void addToBookmarks(GtkMenuItem *menuitem, gpointer user_data)
{
	gchar* bookmarksFilePath=g_build_filename(g_get_user_config_dir(),"pobvnc","bookmarks.conf",NULL);
	
	gchar* label=gtk_entry_get_text(GTK_ENTRY(entryHost));
	gchar* host=gtk_entry_get_text(GTK_ENTRY(entryHost));
	gchar* port=gtk_entry_get_text(GTK_ENTRY(entryPort));
	
	SetKey(bookmarksFilePath,label , "host", host);
	SetKey(bookmarksFilePath,label , "port", port);
	showBookmarks(NULL,NULL);
	setGreenStatus(_("Bookmarks updated"));
}

void delBookmark()
{
	gchar* bookmarksFilePath=g_build_filename(g_get_user_config_dir(),"pobvnc","bookmarks.conf",NULL);
	GError *error=NULL;
	
	GKeyFile * keyFile = g_key_file_new();
	g_key_file_load_from_file (keyFile, bookmarksFilePath, G_KEY_FILE_KEEP_COMMENTS, &error);
	g_key_file_remove_group (keyFile, labelBookmark, &error);
	
	gsize size;
    gchar* data = g_key_file_to_data (keyFile, &size, &error);
    g_file_set_contents (bookmarksFilePath, data, size,  &error);
    g_free (data);
    g_key_file_free (keyFile);
	
	updateBookmarks(NULL,NULL);
}

gboolean checkIfbookmarkExsists(gchar* label)
{
	gchar* bookmarksFilePath=g_build_filename(g_get_user_config_dir(),"pobvnc","bookmarks.conf",NULL);
	GError *error=NULL;
 
	GKeyFile * keyFile = g_key_file_new();
 
	g_key_file_load_from_file (keyFile, bookmarksFilePath, G_KEY_FILE_KEEP_COMMENTS, &error);
	gchar* myval= g_key_file_get_value  (keyFile, label, "host", &error);
 
	g_key_file_free (keyFile);
	if (error!=NULL)return FALSE;
}

void saveBookmark()
{
	
	if(operationBookmarks==EDIT)delBookmark();
	gchar* bookmarksFilePath=g_build_filename(g_get_user_config_dir(),"pobvnc","bookmarks.conf",NULL);
	if (g_file_test (bookmarksFilePath,  G_FILE_TEST_EXISTS)==FALSE)
	{
		g_file_set_contents (bookmarksFilePath,"",NULL,NULL);
	} 
	
	
	gchar* label=gtk_entry_get_text(GTK_ENTRY(entryLabelBookmark));
	gchar* host=gtk_entry_get_text(GTK_ENTRY(entryHostBookmark));
	gchar* port=gtk_entry_get_text(GTK_ENTRY(entryPortBookmark));
	
	if(g_strcmp0 (host,"")==0 || g_strcmp0 (port,"")==0)
	{
		err_message(MainWindow,_("Error"), _("Fields host and port required!"), _("Error"));
		return;
	}
	
	
	SetKey(bookmarksFilePath,label , "host", host);
	SetKey(bookmarksFilePath,label , "port", port);
	hideEditBookmarkWindow();
	updateBookmarks(NULL,NULL);
	operationBookmarks=NONE;
}

void onTreeviewBookmarksSignleButtonPressed(GtkWidget *treeview, GdkEventButton *event, gpointer userdata)
{
	gchar* label;
	GtkTreePath *path=gtk_tree_model_get_path (bookmarksModel, &iterBookmarks);
	GtkTreeSelection * tsel = gtk_tree_view_get_selection (treeviewBookmarks);
	
	
	if ( event->button == 1 && event->type==GDK_BUTTON_RELEASE)
    {
		if ( gtk_tree_selection_get_selected ( tsel , &bookmarksModel , &iterBookmarks ) )
		{
			gtk_tree_model_get(bookmarksModel, &iterBookmarks, 0, &label, -1);
			//info_message(MainWindow,label, label, label);
			labelBookmark=g_strdup_printf("%s", label);
		}
	}
	
	
}


void
  onRowActivatedBookmark (GtkTreeView        *view,
                  GtkTreePath        *path,
                  GtkTreeViewColumn  *col, 
                  gpointer            user_data)
  {
    GtkTreeModel *model;
    GtkTreeIter   iter;
    GtkTreeSelection *sel = gtk_tree_view_get_selection (view);
	GError *error=NULL;
	gchar* bookmarksFilePath=g_build_filename(g_get_user_config_dir(),"pobvnc","bookmarks.conf",NULL);
	gchar* label;
	char* host;
	char* port;
 
    model = gtk_tree_view_get_model(view);
	
	if (gtk_tree_model_get_iter(model, &iter, path))
	{
		gtk_tree_model_get(model, &iter, 0, &label, -1);
		//gtk_tree_model_get(bookmarksModel, &iterBookmarks, 0, &label, -1);
		host=GetKey(bookmarksFilePath,label ,"host");
		port=GetKey(bookmarksFilePath,label ,"port");
		gtk_entry_set_text(GTK_ENTRY(entryHost),host);
		gtk_entry_set_text(GTK_ENTRY(entryPort),port);
		
		gtk_button_set_label (GTK_BUTTON(StartStop), _("Disconnect"));
		gtk_widget_set_sensitive(GTK_WIDGET(toggleServer), FALSE);
		
		StartStopConnection(NULL,NULL);
	}
 
 
 
  }
 
void updateBookmarks(GtkWindow *window,  gpointer   user_data)
{
	GError *error=NULL;
	gchar* bookmarksFilePath=g_build_filename(g_get_user_config_dir(),"pobvnc","bookmarks.conf",NULL);
	int i;
	
	if (g_file_test (bookmarksFilePath,  G_FILE_TEST_EXISTS)==TRUE)
	{
	
		GKeyFile * bookmarksFile = g_key_file_new();
		g_key_file_load_from_file (bookmarksFile,bookmarksFilePath , G_KEY_FILE_KEEP_COMMENTS, &error);
		
		gsize ngroups;
		gchar** groups = g_key_file_get_groups (bookmarksFile,  &ngroups);
		gtk_list_store_clear (bookmarksModel);
		if (ngroups==0) return;
		for (i=0;i<=ngroups-1;i++)
		{
			gtk_list_store_append(bookmarksModel, &iterBookmarks);
			gtk_list_store_set (bookmarksModel,  &iterBookmarks, 0, groups[i], -1);
		}
	}
}



void initBookmarksWindow()
{
	gchar* bookmarksFilePath=g_build_filename(g_get_user_config_dir(),"pobvnc","bookmarks.conf",NULL);
	if (g_file_test (bookmarksFilePath,  G_FILE_TEST_EXISTS)==FALSE)
	{
		g_file_set_contents (bookmarksFilePath,"",NULL,NULL);
	} 
	
	
	GtkCellRenderer *renderer;
	
	bookmarksModel = gtk_list_store_new (1, G_TYPE_STRING);
	renderer = gtk_cell_renderer_text_new (); 
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (treeviewBookmarks),
                                               -1,      
                                               "Bookmark",  
                                               renderer,
                                               "text", 
                                               NULL);
	
	
	gtk_tree_view_set_model (GTK_TREE_VIEW (treeviewBookmarks), GTK_TREE_MODEL (bookmarksModel));
	updateBookmarks(NULL, NULL);
	g_object_unref (bookmarksModel);
	
}


void getPublicIp()
{
	/*GError* error;
	gchar** publicIp;
	gchar* publicIpFile=g_build_filename(g_get_tmp_dir(),"publicIp.ini", NULL);
	GFile*  mySRC =  g_file_new_for_uri("http://www.freemedialab.org/pobvnc/myip.php");
	GFile*  myDEST =  g_file_new_for_path(publicIpFile);
	g_file_copy (mySRC,  myDEST,  G_FILE_COPY_OVERWRITE, NULL, NULL,  NULL,    &error);
	if (error!=NULL)
	{
		 err_message(MainWindow,"Problemi nel ricevere ip pubblico",error->message,"Errore fatale");
		 g_error_free (error);
		 return;
	}
	g_file_get_contents (publicIpFile,&publicIp, NULL,NULL);
	gtk_entry_set_text(GTK_ENTRY(entryHost),publicIp);
	g_free(publicIp);*/
	
	if (g_find_program_in_path ("curl")!=NULL || g_find_program_in_path ("curl.exe")!=NULL)
	{
		gchar** publicIp;
		
		
		#ifdef linux
		gchar* publicIpFile=g_build_filename(g_get_tmp_dir(),"publicIp.ini", NULL);
		g_unlink (publicIpFile);
		gchar* cmd= g_strdup_printf("curl http://www.freemedialab.org/pobvnc/myip.php > %s",publicIpFile );
		system(cmd);
		#endif
		#if defined _WIN32 || defined __CYGWIN__
		gchar* publicIpFile=g_build_filename(g_get_user_config_dir (),"Temp","publicIp.ini", NULL);
		g_unlink (publicIpFile);
		gchar* cmd= g_strdup_printf("curl.exe --max-time 5 http://www.freemedialab.org/pobvnc/myip.php -o %s",publicIpFile );
		
		STARTUPINFO si={sizeof(si)};
		PROCESS_INFORMATION pi;		
		if (CreateProcess(NULL, cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW , NULL, NULL, &si, &pi))
		{
			// Wait until child process exits.
			WaitForSingleObject( pi.hProcess, INFINITE );
			// Close process and thread handles. 
			CloseHandle( pi.hProcess );
			CloseHandle( pi.hThread );
		}else {
			MessageBox( NULL, GetLastError(), _("Warning"), MB_OK | MB_ICONERROR| MB_TASKMODAL);
		}
			
		sleep(1);
		#endif
		
		
		g_file_get_contents (publicIpFile,&publicIp, NULL,NULL);
		gtk_entry_set_text(GTK_ENTRY(entryHost),publicIp);
		g_free(publicIp);
	}
	
	
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
	gchar* pobvncShareApplicationsFolder=g_build_filename(g_get_home_dir (),".local/share/applications/",NULL);
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
		if (!g_file_test (pobvncShareApplicationsFolder, G_FILE_TEST_EXISTS)) g_mkdir(pobvncShareApplicationsFolder, 0755);
		
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

#if defined _WIN32 || defined __CYGWIN__
	info_message(MainWindow,_("Only for Gnu/Linux"),_("This feature is only for GNU/Linux"),_("Sorry..."));
#endif

}

void showBookmarks(GtkMenuItem *menuitem, gpointer     user_data)
{
	gtk_widget_show (GTK_WIDGET(bookmarksWindow));
	updateBookmarks(NULL,NULL);
	
}

void hideBookmarks()
{
	gtk_widget_hide(GTK_WIDGET(bookmarksWindow));
}

void deleteLogs()
{
	#if defined _WIN32 || defined __CYGWIN__
	gchar* clientLog=g_build_filename("c:","temp","WinVNC4.log",NULL);
	gchar* serverLog=g_build_filename("c:","temp","vncviewer.log",NULL);
	#endif
	
	#ifdef linux
	gchar* clientLog=g_build_filename(g_get_tmp_dir(),"pobvnc.log",NULL);
	gchar* serverLog=g_build_filename(g_get_tmp_dir(),"vncviewer.log",NULL);
	#endif
	
	if (g_file_test (clientLog, G_FILE_TEST_EXISTS)) g_remove(clientLog);
	if (g_file_test (serverLog, G_FILE_TEST_EXISTS)) g_remove(serverLog);
}


#if defined _WIN32 || defined __CYGWIN__
void StartProcess(char* szExe)
{

    STARTUPINFO startupInfo;
    PROCESS_INFORMATION processInfo;

    ZeroMemory( &startupInfo, sizeof(startupInfo) );
    startupInfo.cb = sizeof(startupInfo);
    ZeroMemory( &processInfo, sizeof(processInfo) );

    if (CreateProcess(0, szExe, 0, 0, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, 0, 0, &startupInfo, &processInfo))
    {
        //WaitForSingleObject( processInfo.hProcess, INFINITE );
    }else{
        MessageBox(NULL, szExe, "Server not started", MB_OK); 
    }
    CloseHandle( processInfo.hProcess );
    CloseHandle( processInfo.hThread );
}
#endif


void checkDependencies()
{
#ifdef linux
	if (g_find_program_in_path ("x11vnc")!=NULL && g_find_program_in_path ("vncviewer")!=NULL) return ;
	gchar* cmd;
	GError* error;
	
	
	
	//test if we are in Arch Linux based
	if (g_find_program_in_path ("pacman")!=NULL)
	{
		gint r =info_YesNo(MainWindow,_("Missing dependencies"),_("Can I try to install the dependencies?"),_("Check deps"));
		if (r==GTK_RESPONSE_YES)
		{
			//test for term
			if (getTerm()==NULL)
			{
				err_message(MainWindow,_("Please install xterm"),_("I need xterm to install dependencies"),"Error");
				return;
			}else{
				gchar* term=getTerm();
				if (g_find_program_in_path ("x11vnc")==NULL) cmd=g_strdup_printf("%s -e 'sudo pacman --noconfirm -S tigervnc x11vnc' &", term);
				if (g_find_program_in_path ("vncviewer")==NULL) cmd=g_strdup_printf("%s -e 'sudo pacman --noconfirm -S tigervnc x11vnc' &",term);
				g_print("%s\n",cmd);
				system(cmd);
			}
		}
	}//arch based
	
	//test if we are in Debian based
	if (g_find_program_in_path ("apt-get")!=NULL)
	{
		gint r =info_YesNo(MainWindow,_("Missing dependencies"),_("Can I try to install the dependencies?"),_("Check deps"));
		if (r==GTK_RESPONSE_YES)
		{
			//test for term
			if (getTerm()==NULL)
			{
				err_message(MainWindow,_("Please install xterm"),_("I need xterm to install dependencies"),"Error");
				return;
			}else{
				gchar* term=getTerm();
				if (g_find_program_in_path ("x11vnc")==NULL)
				{
					cmd=g_strdup_printf("%s -e 'sudo apt-get install x11vnc' &", term);
					g_print("%s\n",cmd);
					system(cmd);
				}
				if (g_find_program_in_path ("vncviewer")==NULL)
				{
					copyFromResource("resource:///org/pobvnc/res/install_tigervnc.sh", "/tmp/install_tigervnc.sh");
					cmd=g_strdup_printf("%s -e 'sudo sh /tmp/install_tigervnc.sh' &", term);
					g_print("%s\n",cmd);
					system(cmd); 
				}
				
				
				
			}
		}
	}//Debian based
	
	
	
	
#endif

}
