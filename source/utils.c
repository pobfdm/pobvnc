/*
 *      message.c
 *      
 *      Copyright 2008 Fabio DM <pobfdm@gmail.com>
 *      
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */
#include <gtk/gtk.h> 
 
void
info_message(GtkWidget *main_window,gchar* title, gchar *msg, gchar* winlabel)
{
	GtkWidget *dialog;
	title=g_strdup_printf("<span size=\"x-large\"><b>%s</b></span>", title);
	
  	dialog = gtk_message_dialog_new_with_markup (GTK_WINDOW (main_window),
				   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
				   GTK_MESSAGE_INFO,
				   GTK_BUTTONS_OK,
				   title);
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),
					"%s",(gchar *)msg);
	gtk_window_set_title(GTK_WINDOW(dialog),winlabel);
  	gtk_dialog_run (GTK_DIALOG (dialog));
  	gtk_widget_destroy (dialog);
}
 
 
gint
info_YesNo(GtkWidget *main_window,gchar* title, gchar *msg, gchar* winlabel)
{
  GtkWidget *dialog;
  gint result;
  title=g_strdup_printf("<span size=\"x-large\"><b>%s</b></span>", title);
 
  	dialog = gtk_message_dialog_new_with_markup (GTK_WINDOW (main_window),
				   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
				   GTK_MESSAGE_WARNING,
				   GTK_BUTTONS_YES_NO,
				   title);
	gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),
					"%s",(gchar *)msg);
	gtk_window_set_title(GTK_WINDOW(dialog),winlabel);
  	result = gtk_dialog_run (GTK_DIALOG (dialog));
	switch (result)
    {
      case GTK_RESPONSE_YES:
         result=GTK_RESPONSE_YES ;
		 break;
	  case GTK_RESPONSE_NO:
         result=GTK_RESPONSE_NO ;
		 break;	 
 
      default:
         result=GTK_RESPONSE_DELETE_EVENT;
         break;
    }
  	gtk_widget_destroy (dialog);
	return result;
 
}
 
void
err_message(GtkWidget *main_window,gchar* title, gchar *msg, gchar* winlabel)
{
	GtkWidget *dialog;
	title=g_strdup_printf("<span size=\"x-large\"><b>%s</b></span>", title);
	
	dialog = gtk_message_dialog_new_with_markup (GTK_WINDOW (main_window),
				   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
				   GTK_MESSAGE_ERROR,
				   GTK_BUTTONS_OK,title);
  gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),
    												"%s",(gchar *)msg);
  gtk_window_set_title(GTK_WINDOW(dialog),winlabel);
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}


int getBitsCpu()
{
	if(sizeof(int*) == 4) 
	{
	  return 32 ;
	}else{
		return 64 ;
	}
}


 
char* GetKey(const gchar* File,const gchar* group ,const gchar* key)
{
	//Una varibile per gli eventuali errori
	GError *error=NULL;
 
	//Mi preparo a leggere il file con le chiavi
	GKeyFile * mykey = g_key_file_new();
 
	g_key_file_load_from_file (mykey, File, G_KEY_FILE_KEEP_COMMENTS, &error);
	gchar* myval= g_key_file_get_value  (mykey, group, key, &error);
 
	g_key_file_free (mykey);
 
 
	//Gestisco eventuali errori di lettura
	if (error!=NULL)
	{
		 g_error("Attenzione errore nella lettura : %s\n",error->message);
		return NULL;
	}
 
	return myval;
} 
 
 
void* SetKey(const gchar* File,const gchar* group , const gchar* key, const gchar* content)
{
	//Una varibile per gli eventuali errori
	GError *error=NULL;
 
	//Mi preparo a leggere il file con le chiavi
	GKeyFile *mykey = g_key_file_new();
	g_key_file_load_from_file (mykey, File, G_KEY_FILE_KEEP_COMMENTS, &error);
 
	//Scrivo la chiave (immagine in memoria, non scritta sul file)
	g_key_file_set_string(mykey, group, key, content);
 
    //Adesso scrivo il file File
    gsize size;
    gchar* data = g_key_file_to_data (mykey, &size, &error);
    g_file_set_contents (File, data, size,  &error);
 
	//Faccio pulizia delle strutture non piu' necessarie;
	g_free (data);
    g_key_file_free (mykey);
 
    //Gestisco eventuali errori di lettura
	if (error!=NULL)
	{
		 g_error("Attenzione errore nella lettura : %s\n",error->message);
		return NULL;
	}
 
}  
