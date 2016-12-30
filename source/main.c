/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 */




#include<libintl.h> //per gettex
#include<locale.h>	//per gettex
 
#include <gtk/gtk.h>   
#include "callbacks.h" 
#include "utils.h"
 
#define _(String) gettext (String) //per gettex


GtkBuilder *xml;                                                 
GObject   *widget, *MainWindow, *StartStop, *toggleServer, *entryHost, *entryPort, *aboutWin;  
GObject *bookmarksWindow, *editBookmarkWindow;
GObject *treeviewBookmarks, *entryLabelBookmark, *entryHostBookmark, *entryPortBookmark;
GObject	*btAddBookmark, *btEditBookmark , *btDelBookmark, *btCancelBookmark, *btSaveBookmark;

GtkTreeModel *bookmarksModel;
GtkTreeIter    iterBookmarks;
GObject	*lblStatus;	
gchar* 	logo_file ;
gchar* binPath;


void on_MainWindow_delete_event(GtkWidget *widget, gpointer user_data)       
{                                                                         
    abortConnection();
    deleteLogs();
    gtk_main_quit();
}


void init()
{
	gtk_window_set_title(GTK_WINDOW(MainWindow),"Pobvnc");
	gtk_window_resize(GTK_WINDOW(MainWindow),400, 200);
	
	
	//Set Main Image
	gchar* 	logo_file = g_build_filename(g_get_tmp_dir(),"lifesaver.png", NULL);
	GFile*  mySRC =  g_file_new_for_uri("resource:///org/pobvnc/res/lifesaver.png");
	GFile*  myDEST =  g_file_new_for_path(logo_file);
	g_file_copy (mySRC,  myDEST,  G_FILE_COPY_OVERWRITE, NULL, NULL,  NULL,    NULL); 
	
	GObject *imgMain= gtk_builder_get_object (xml,"imgMain");
	gtk_image_set_from_file(GTK_IMAGE(imgMain),logo_file);
	
	GdkPixbuf *logo = gdk_pixbuf_new_from_file (logo_file, NULL);
	gtk_window_set_default_icon (logo);
	gtk_window_set_icon (GTK_WINDOW(MainWindow), logo);
	
	setGreenStatus(_("I'm ready."));
	
	GObject* lblHost=gtk_builder_get_object (xml,"lblHost" );
	gtk_label_set_text (GTK_LABEL(lblHost), _("Address"));
	
	GObject* lblPort=gtk_builder_get_object (xml,"lblPort" );
	gtk_label_set_text (GTK_LABEL(lblPort), _("Port"));
	
	gtk_button_set_label (GTK_BUTTON(StartStop), _("Connect"));
	
	#ifdef _WIN32 //Hide install menu item on windows
	GObject* installMenuItem=gtk_builder_get_object (xml,"imagemenuitemInstall" );
	gtk_widget_set_visible(GTK_IMAGE_MENU_ITEM(installMenuItem),FALSE);
	#endif	
	
	if (g_find_program_in_path ("pobvnc")!=NULL) //Hide install menu item if installed
	{
		GObject* installMenuItem=gtk_builder_get_object (xml,"imagemenuitemInstall" );
		gtk_widget_set_visible(GTK_IMAGE_MENU_ITEM(installMenuItem),FALSE);
	}
	
	
	GObject* imagemenuitemQuit=gtk_builder_get_object (xml,"imagemenuitemQuit" );
	g_signal_connect (imagemenuitemQuit, "activate", G_CALLBACK(on_MainWindow_delete_event), NULL);
	
	
	menuInit();
	checkDependencies();
	initBookmarksWindow();
} 



void menuInit()
{
	//Callbacks menu
	GObject* imagemenuitemQuit=gtk_builder_get_object (xml,"imagemenuitemQuit" );
	g_signal_connect (imagemenuitemQuit, "activate", G_CALLBACK(on_MainWindow_delete_event), NULL);
	
	GObject* imagemenuitemAbout=gtk_builder_get_object (xml,"imagemenuitemAbout" );
	g_signal_connect (imagemenuitemAbout, "activate", G_CALLBACK(aboutDialog), NULL);
	
	GObject* imagemenuitemInstall=gtk_builder_get_object (xml,"imagemenuitemInstall" );
	g_signal_connect (imagemenuitemInstall, "activate", G_CALLBACK(installRemove), NULL);
	
	GObject* mnuAddBookmark=gtk_builder_get_object (xml,"mnuAddBookmark" );
	g_signal_connect (mnuAddBookmark, "activate", G_CALLBACK(addToBookmarks), NULL);
	
	GObject* mnuShowBookmarks=gtk_builder_get_object (xml,"mnuShowBookmarks" );
	g_signal_connect (mnuShowBookmarks, "activate", G_CALLBACK(showBookmarks), NULL);

}



int main(int argc, char* argv[])
{
    binPath=g_strdup_printf("%s", argv[0]);
    initGettex();
    
	gtk_init (&argc, &argv);
  	GError* error = NULL;
	gchar* glade_file = g_build_filename(g_get_tmp_dir(),"gui.xml", NULL);
	
	GFile*  mySRC =  g_file_new_for_uri("resource:///org/pobvnc/res/gui.xml");
	GFile*  myDEST =  g_file_new_for_path(glade_file);
	g_file_copy (mySRC,  myDEST,  G_FILE_COPY_OVERWRITE, NULL, NULL,  NULL,    &error);
	
    /*Infine carica come disolito il file dell'interfaccia */
	xml = gtk_builder_new ();
	if (!gtk_builder_add_from_file (xml, glade_file, &error))
	{
		g_warning (_("Couldn\'t load builder file: %s"), error->message);
		g_error_free (error);
	}
	
	
	
	/*Callbacks Window widgets connect*/
	aboutWin=gtk_builder_get_object (xml,"aboutdialog");
	bookmarksWindow=gtk_builder_get_object(xml,"BookmarksWindow");
	editBookmarkWindow=gtk_builder_get_object(xml,"editBookmarkWindow");
	treeviewBookmarks=gtk_builder_get_object(xml,"treeviewBookmarks");
	entryHost= gtk_builder_get_object (xml,"entryHost");
	entryPort= gtk_builder_get_object (xml,"entryPort");
	lblStatus= gtk_builder_get_object (xml,"lblStatus");
	
	entryLabelBookmark=gtk_builder_get_object (xml,"entryLabelBookmark" );
	entryHostBookmark=gtk_builder_get_object (xml,"entryHostBookmark" );
	entryPortBookmark=gtk_builder_get_object (xml,"entryPortBookmark" );
	btCancelBookmark=gtk_builder_get_object(xml,"btCancelBookmark");
	btSaveBookmark=gtk_builder_get_object(xml,"btSaveBookmark");
	
	btAddBookmark=gtk_builder_get_object(xml,"btAddBookmark");
	g_signal_connect (btAddBookmark, "clicked", G_CALLBACK(showNewBookmarkWindow), NULL);
	
	btDelBookmark=gtk_builder_get_object(xml,"btDelBookmark");
	g_signal_connect(btDelBookmark,"clicked",G_CALLBACK(delBookmark),NULL);
	
	btEditBookmark=gtk_builder_get_object(xml,"btEditBookmark");
	g_signal_connect(btEditBookmark,"clicked",G_CALLBACK(showEditBookmarkWindow),NULL);
	
	MainWindow=gtk_builder_get_object (xml,"MainWindow" );
	g_signal_connect (MainWindow, "delete_event", G_CALLBACK(on_MainWindow_delete_event), NULL);
	
	StartStop=gtk_builder_get_object (xml,"btnStartStop" );
	g_signal_connect (StartStop, "clicked", G_CALLBACK(StartStopConnection), NULL);
 
	toggleServer=gtk_builder_get_object (xml,"checkbuttonServer" );
	g_signal_connect (toggleServer, "toggled", G_CALLBACK(isServer), NULL);
	
	g_signal_connect (btCancelBookmark, "clicked", G_CALLBACK(hideEditBookmarkWindow), NULL);
	g_signal_connect (btSaveBookmark, "clicked", G_CALLBACK(saveBookmark), NULL);
	
	g_signal_connect (bookmarksWindow, "delete_event", G_CALLBACK(hideBookmarks), NULL);
	g_signal_connect (editBookmarkWindow, "delete_event", G_CALLBACK(hideEditBookmarkWindow), NULL);
	
	g_signal_connect(treeviewBookmarks, "row-activated", G_CALLBACK(onRowActivatedBookmark), NULL);
	g_signal_connect(treeviewBookmarks, "button-release-event", (GCallback) onTreeviewBookmarksSignleButtonPressed, NULL);
	
	/*Initializations*/
	init();
	
	
	gtk_main ();
    return 0;
}
