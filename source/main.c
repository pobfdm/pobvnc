#include<libintl.h> //per gettex
#include<locale.h>	//per gettex
 
#include <gtk/gtk.h>   
#include "callbacks.h" 
#include "utils.h"
 
#define _(String) gettext (String) //per gettex


GtkBuilder *xml;                                                 
GObject   *widget, *MainWindow, *StartStop, *toggleServer, *entryHost, *entryPort, *aboutWin;  



void on_MainWindow_delete_event(GtkWidget *widget, gpointer user_data)       
{                                                                         
    abortConnection();
    gtk_main_quit();
}


void init()
{
	gtk_window_set_title(GTK_WINDOW(MainWindow),"Pobvnc");
	gtk_window_resize(GTK_WINDOW(MainWindow),400, 200);
	
	
	//Set Main Image
	gchar* image_file = g_build_filename(g_get_tmp_dir(),"lifesaver.png", NULL);
	GFile*  mySRC =  g_file_new_for_uri("resource:///org/pobvnc/res/lifesaver.png");
	GFile*  myDEST =  g_file_new_for_path(image_file);
	g_file_copy (mySRC,  myDEST,  G_FILE_COPY_OVERWRITE, NULL, NULL,  NULL,    NULL); 
	
	
	GObject *imgMain= gtk_builder_get_object (xml,"imgMain");
	gtk_image_set_from_file(GTK_IMAGE(imgMain),image_file);
} 

int main(int argc, char* argv[])
{
    /*Gettex*/
    setlocale(LC_ALL,"");
    bindtextdomain("pobvnc",g_get_tmp_dir());
    textdomain("pobvnc");
 
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
	entryHost= gtk_builder_get_object (xml,"entryHost");
	entryPort= gtk_builder_get_object (xml,"entryPort");
	
	
	MainWindow=gtk_builder_get_object (xml,"MainWindow" );
	g_signal_connect (MainWindow, "delete_event", G_CALLBACK(on_MainWindow_delete_event), NULL);
	
	StartStop=gtk_builder_get_object (xml,"btnStartStop" );
	g_signal_connect (StartStop, "clicked", G_CALLBACK(StartStopConnection), NULL);
 
	toggleServer=gtk_builder_get_object (xml,"checkbuttonServer" );
	g_signal_connect (toggleServer, "toggled", G_CALLBACK(isServer), NULL);
	
	//Callbacks menu
	GObject* imagemenuitemQuit=gtk_builder_get_object (xml,"imagemenuitemQuit" );
	g_signal_connect (imagemenuitemQuit, "activate", G_CALLBACK(on_MainWindow_delete_event), NULL);
	
	GObject* imagemenuitemAbout=gtk_builder_get_object (xml,"imagemenuitemAbout" );
	g_signal_connect (imagemenuitemAbout, "activate", G_CALLBACK(aboutDialog), NULL);
	
	
	/*Initializations*/
	init();
	
	gtk_main ();
    return 0;
}
