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
